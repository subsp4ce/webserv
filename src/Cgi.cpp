/***********
* includes *
***********/

#include "../includes/Cgi.hpp"
#include "../includes/Connection.hpp"
#include "../includes/LocationBlock.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <signal.h> /* kill */
#include <unistd.h> /* pipe */
#include <sys/stat.h>
#include <fcntl.h>

/***************************
* constructor + destructor *
***************************/

Cgi::Cgi( Connection *connection ) :
_connection(connection),
_cgiState(Cgi::CREATED),
_queryString(_connection->_request->getQueryString()),
_cgiBin(_connection->_data._locationBlock->getCgi()[1]),
_uploadDir(_connection->_data._root), /* set default upload directory */
_argv(NULL),
_pid(0),
_pipeEventsRegistered(false),
_serverToCgiClosed(false),
_cgiToServerClosed(false),
_waitResult(0),
_headerFields(_connection->_request->getHeaderFields()) {

	_serverToCgi[1] = 0;
	_cgiToServer[0] = 0;

	if (checkCgiBin() == ERROR ||
		parseCgiUri() == ERROR ||
		postRequest() == ERROR)
		return ;

	_cgiState = Cgi::WRITE;
}

Cgi::~Cgi( void ) {

	if (_argv != NULL) {
		for (int i = 0; i < 2; i++)
			delete _argv[i];
		delete _argv;
	}
}

/********************
* public functions *
********************/

std::string	const	Cgi::getBody( void ) const { return (_body); }
int					Cgi::getCgiState( void ) const { return (_cgiState); }
int 				Cgi::getPid( void ) const { return (_pid); }
int 				Cgi::getServerToCgiWriteFd( void ) const { return (_serverToCgi[1]); }
int					Cgi::getCgiToServerReadFd( void ) const { return (_cgiToServer[0]); }
bool 				Cgi::getPipeEventsRegistered( void ) const { return (_pipeEventsRegistered); }
bool 				Cgi::getServerToCgiClosed( void ) const { return (_serverToCgiClosed); }
bool 				Cgi::getCgiToServerClosed( void ) const { return (_cgiToServerClosed); }
void				Cgi::setPipeEventsRegistered( bool registered ) { _pipeEventsRegistered = registered; }

int	Cgi::waitForChild( void ) {

	if (cgiWait() == ERROR)
		return (ERROR);
	if (_waitResult == 0)
		_cgiState = Cgi::WAITING;
	else
		_cgiState = Cgi::READING;
	return (0);
}

int	Cgi::launchCgi( void ) {

	prepArgv();
	if (pipe(_serverToCgi) == ERROR || pipe(_cgiToServer) == ERROR)
		return (_connection->errorLog(EXIT, "system function pipe() failed in cgi", true));
	if (makeNonBlocking(_cgiToServer[0], _serverToCgi[1]) == ERROR)
		return (ERROR);
	_pid = fork();
	if (_pid == ERROR)
		return (_connection->errorLog(EXIT, "system function fork() failed in cgi", true));
	else if (_pid == 0) {

		if (closeFds(_cgiToServer[0], _serverToCgi[1]) == ERROR)
			return (ERROR);
		executeCgi();
	}
	else {

		if (closeFds(_serverToCgi[0], _cgiToServer[1]) == ERROR)
			return (ERROR);
		_requestBody = _connection->_request->getBody();
		_cgiState = Cgi::WRITING;
		if (cgiWait() == ERROR)
			return (ERROR);
	}
	return (0);
}

int Cgi::writeToCgi( void ) {

	int bytes = 0;
	int bytesToWrite = std::min(BUF_SIZE, static_cast<int>(_requestBody.size()));

	bytes = write(_serverToCgi[1], _requestBody.c_str(), bytesToWrite);
	if (bytes == ERROR) {
	
		_connection->_state = State::REMOVE;
		return (_connection->errorLog(500, "write to cgi failed", false));
	}
	_requestBody.erase(0, bytes);

	if (_requestBody.size() != 0) {
		_cgiState = Cgi::WRITING;
		return (0);
	}
	if (closeFd(_serverToCgi[1], &_serverToCgiClosed) == ERROR)
		return (ERROR);
	waitForChild();
	return (0);
}

int	Cgi::readFromCgi( void ) {

	char buffer[BUF_SIZE + 1];
	memset(buffer, '\0', BUF_SIZE + 1);

	int bytesRead = read(_cgiToServer[0], buffer, BUF_SIZE);
	if (bytesRead == ERROR) {

		_connection->_state = State::REMOVE;
		return (_connection->errorLog(500, "read from cgi failed", false));
	}
	if (bytesRead == 0) {

		_cgiState = Cgi::DONE;
		if (closeFd(_cgiToServer[0], &_cgiToServerClosed) == ERROR)
			return (ERROR);
	}
	else {

		_body.append(buffer);
		_cgiState = Cgi::READING;
	}
	return (0);
}

/********************
* private functions *
********************/

int	Cgi::cgiWait( void ) {

	_waitResult = waitpid(_pid, &_stat, WUNTRACED | WNOHANG);
	if (_waitResult == ERROR) {

		std::string err = strerror(errno);
		return (_connection->errorLog(502, "cgi: " + err, true));
	}
	if (WIFEXITED(_stat)) {

		if (WEXITSTATUS(_stat) != 0)
			return (_connection->errorLog(502, "cgi: child process exited with non zero status", true));
	}
	return (0);
}

int Cgi::executeCgi( void ) {

	if (chdir("www/cgi-bin") == ERROR)
		return (_connection->errorLog(EXIT, "cgi: failed to navigate to cgi-bin", true));
	setEnvVars();
	if (dupClose(_serverToCgi[0], _cgiToServer[1]) == ERROR)
		return (ERROR);
	execve(_cgiBin.c_str(), _argv, environ);
	return (_connection->errorLog(EXIT, "cgi: execve failed", true));
}

int	Cgi::dupClose( int readFd, int writeFd ) {

	if (dup2(readFd, STDIN_FILENO) == ERROR)
		return (_connection->errorLog(EXIT, "cgi: dup2() failed to point STDIN to reading side of pipe", true));
	if (dup2(writeFd, STDOUT_FILENO) == ERROR)
		return (_connection->errorLog(EXIT, "cgi: dup2() failed to point STDOUT to writing side of pipe", true));
	if (close(readFd) == ERROR)
		return (_connection->errorLog(EXIT, "cgi: close(_serverToCgi[0]) failed", true));
	if (close(writeFd) == ERROR)
		return (_connection->errorLog(EXIT, "cgi: close(_cgiToServer[1]) failed", true));
	return (0);
}

int	Cgi::makeNonBlocking( int readFd, int writeFd ) {

	if (fcntl(readFd, F_SETFL, O_NONBLOCK) == ERROR)
		return (_connection->errorLog(EXIT, "system function fcntl() failed for readFd in cgi", true));
	if (fcntl(writeFd, F_SETFL, O_NONBLOCK) == ERROR)
		return (_connection->errorLog(EXIT, "system function fcntl() failed for writeFd in cgi", true));
	return (0);
}

int Cgi::closeFd( int fd, bool *isClosed ) {

	if (close(fd) == ERROR)
		return (_connection->errorLog(EXIT, "cgi: close failed in closeFd", true));
	*isClosed = true;
	return (0);
}

int Cgi::closeFds( int readFd, int writeFd ) {

	if (close(readFd) == ERROR)
		return (_connection->errorLog(EXIT, "system function close failed on readFd in closeFds()", true));
	if (close(writeFd) == ERROR)
		return (_connection->errorLog(EXIT, "system function close failed on writeFd in closeFds()", true));
	return (0);
}

void Cgi::prepArgv( void ) {

	_argv = new char*[3];
	_argv[0] = new char[_cgiBin.length() + 1];
	std::strcpy(_argv[0], _cgiBin.c_str());
	_argv[1] = new char[_scriptBin.length() + 1];
	std::strcpy(_argv[1], _scriptBin.c_str());
	_argv[2] = NULL;
	return ;
}

int	Cgi::postRequest() {

	if (_connection->_request->getRequestMethod() == Method::POST) {

		if (_connection->_request->getBody().size() == 0)
			return(_connection->errorLog(400, "unable to handle post request, request doesn't contain a body", true));
		if (handleUploadDir() == ERROR)
			return (ERROR);
	}
	return (0);
}

int	Cgi::handleUploadDir( void ) {
	
	setUploadDir();
	if (checkUploadDir() == ERROR)
		return (ERROR);
	return (0);
}

void	Cgi::setUploadDir( void ) {

	if (_connection->_data._locationBlock != NULL) {

		if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::FILE_UPLOAD) == true) /* set custom upload directory */
			_uploadDir = _connection->_data._root + _connection->_data._locationBlock->getFileUpload();
	}
}

int		Cgi::checkUploadDir( void ) {

	struct stat fileStat;

	if (stat(_uploadDir.c_str(), &fileStat) == 0) {

		if (fileStat.st_mode & S_IFDIR)
			return (0);
		else
			return (_connection->errorLog(403, "requested upload directory is not a folder", true));
	}
	else
		return (_connection->errorLog(500, "cannot find the requested upload directory", true));
}

int	Cgi::checkCgiBin( void ) {

	struct stat fileStat;

	if (stat(_cgiBin.c_str(), &fileStat) == 0) {

		if ((fileStat.st_mode & S_IEXEC)) {

			if (!endsWith(_cgiBin, CGI_PY) && !endsWith(_cgiBin, CGI_PY3))
				return (_connection->errorLog(500, "invalid path to cgi binary", true));
		}
		else
			return (_connection->errorLog(403, "cgi binary has no executable rights", true));
	}
	else
		return (_connection->errorLog(500, "cannot find the requested cgi binary", true));
	return (0);
}

int	Cgi::parseCgiUri( void ) {

	parseComponents(getComponents());
	if (checkScriptPath() == ERROR)
		return (ERROR);
	return (0);
}

Cgi::s_list	Cgi::getComponents( void ) {

	size_t pos;
	std::string component;
	std::list<std::string> components;

	while ((pos = _connection->_data._path.find("/")) != std::string::npos) {

		component = _connection->_data._path.substr(0, pos);
		if (component != "")
			components.push_back(component);
		_connection->_data._path.erase(0, pos + 1);
	}
	if (_connection->_data._path.length() > 0)
		components.push_back(_connection->_data._path);

	return (components);
}

void		Cgi::parseComponents( s_list components ) {

	bool scriptPathFound = false;

	for (std::list<std::string>::iterator it = components.begin(); it != components.end(); it++) {

		if (endsWith(*it, "py") && scriptPathFound == false) {
			_scriptBin = *it;
			scriptPathFound = true;
		}
		else if (scriptPathFound == true)
			_pathInfo.append("/" + *it);
	}
}

int			Cgi::checkScriptPath( void ) {

	struct stat fileStat;
	std::string filePath = "www/cgi-bin/" + _scriptBin;

	if (stat(filePath.c_str(), &fileStat) == 0) {

		if ((fileStat.st_mode & S_IEXEC))
			return (0);
		else
			return (this->_connection->errorLog(403, "cgi script has no executable rights", true));
	}
	return (this->_connection->errorLog(404, "cannot find the requested cgi script", true));
}

int	Cgi::setEnvVars( void ) {

	/* CONTENT_LENGTH */

	if (_connection->_request->getBody().length() > 0)
		setenv("CONTENT_LENGTH", toString(_connection->_request->getBody().length()).c_str(), 1);
	else
		setenv("CONTENT_LENGTH", "", 1);

	/* CONTENT_TYPE */

	if (_headerFields.find("Content-Type") != _headerFields.end())
		setenv("CONTENT_TYPE", _headerFields["Content-Type"].c_str(), 1);
	else
		setenv("CONTENT_TYPE", "", 1);

	/* GATEWAY_INTERFACE */

	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);

	/* PATH_INFO => http://localhost/sayhello.py/this/is/path/info?say=hi&to=mom */

	setenv("PATH_INFO", _pathInfo.c_str(), 1);

	/* PATH_TRANSLATED => https://www.nginx.com/resources/wiki/start/topics/tutorials/config_pitfalls/ */

	setenv("PATH_TRANSLATED", (_connection->_data._root + _pathInfo).c_str(), 1);

	/* QUERY_STRING */

	setenv("QUERY_STRING", _connection->_request->getQueryString().c_str(), 1);

	/* REMOTE_HOST */

	if (_headerFields.find("Host") != _headerFields.end())
		setenv("REMOTE_HOST", _headerFields["Host"].c_str(), 1);
	else
		setenv("REMOTE_HOST", "", 1);

	/* REQUEST_METHOD */

	setenv("REQUEST_METHOD", _connection->_request->getMethodToString(_connection->_request->getRequestMethod() ).c_str(), 1);

	/* SCRIPT_NAME */

	setenv("SCRIPT_NAME", _scriptBin.c_str(), 1);

	/* SERVER_NAME */
	
	setenv("SERVER_NAME", "Windmill", 1);

	/* SERVER_PORT */

	setenv("SERVER_PORT", toString(_connection->_port).c_str(), 1);

	/* UPLOAD_DIR */

	setenv("UPLOAD_DIR", _uploadDir.c_str(), 1);
	return (0);
}
