/***********
* includes *
***********/

#include "../includes/DirectoryListing.hpp"
#include "../includes/Connection.hpp"
#include "../includes/Request.hpp"
#include <sys/stat.h>
#include <errno.h>

/***************************
* constructor + destructor *
***************************/

DirectoryListing::DirectoryListing( Connection *connection ) :
AResponse(connection),
_connection(connection) {

	if (openDir() == ERROR || generateBody() == ERROR)
		return ;
	generateResponse();
	sendResponse();
}

DirectoryListing::~DirectoryListing( void ) {}

/********************
* private functions *
********************/

int		DirectoryListing::openDir( void ) {

	_dir = opendir(_connection->_data._path.c_str());
	if (_dir == NULL)
		return (_connection->errorLog(500, "Directory failed to open", true));
	return (0);
}

int	DirectoryListing::generateBody( void ) {

	struct dirent *entry;
	std::string printPath, target;

	target = setTarget();
	printPath = setPrintPath();
	_body = "<h1>Index of /" + printPath + "</h1>";
	errno = 0;
	while((entry = readdir(_dir))) {
	
		if (entry->d_type == DT_DIR || entry->d_type == DT_REG || entry->d_type == DT_LNK)
			_body = _body + "<a href=\"" + target + entry->d_name + "\">" + entry->d_name + "</a><br>";
	}
	if (errno != 0)
		return (_connection->errorLog(EXIT, "readdir() failed", true));
	if (closedir(_dir) == ERROR)
		return (_connection->errorLog(EXIT, "closedir() failed", true));
	_connection->_data._fileExtention = "html";
	return (0);
}

std::string	DirectoryListing::setTarget( void ) {

	std::string target;

	if (_connection->_request->getRequestTarget().back() != '/')
		target = _connection->_request->getRequestTarget() + "/";
	else
		target = _connection->_request->getRequestTarget();
	return (target);
}

std::string	DirectoryListing::setPrintPath( void ) {

	std::string printPath;

	if (_connection->_data._path == _connection->_data._path)
		printPath = "";
	else
		printPath = _connection->_data._path.substr(_connection->_data._path.length() + 1);
	return (printPath);
}
