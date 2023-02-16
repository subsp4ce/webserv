/***********
* includes *
***********/

#include "../includes/ResolvePath.hpp"
#include "../includes/Connection.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/LocationBlock.hpp"
#include "../includes/Request.hpp"
#include <fstream> /* std::ifstream */

/***************************
* constructor + destructor *
***************************/

ResolvePath::ResolvePath( Connection *connection ) :
_connection(connection),
_root("www"),
_alias(false) {

	if (mapRequest() == ERROR)
		return ;

	setFileExtention();
	if (_connection->_data._fileExtention == CGI_EXT ||
		_connection->_request->getRequestMethod() == Method::POST) {

		checkCgiDirective();
		checkCgiExt();
		return ;
	}

	/* We return before resolving the path, otherwise it would for example in case of this
	request "curl -X DELETE http://localhost" resolve to "http://localhost/index.html" and delete this file */

	if (_connection->_request->getRequestMethod() == Method::DELETE) {

		_connection->_responseType = ResponseType::DELETE;
		_connection->_data._path = _path;
		return ;
	}

	resolvePath();
	setFileExtention();
	_connection->_data._path = _path;
}

ResolvePath::~ResolvePath( void ) {}

/********************
* private functions *
********************/

int	ResolvePath::mapRequest( void ) {

	setRoot();

	/* An error could have occurred. In this situation we still need to resolve the root
	so that we can find a custom error page if this has been set in the config file */

	if (_connection->_state == State::ERR)
		return (ERROR);
	setPath();
	return (0);
}

void	ResolvePath::setRoot( void ) {

	if (_connection->_data._serverBlock->getDirectiveExists(ServerBlock::SERVER_ROOT) == true)
		_root = _connection->_data._serverBlock->getRoot();
	if (_connection->_data._serverBlock->getLocationExists() == true) {

		if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::ALIAS) == true) {

			_root = _connection->_data._locationBlock->getAlias();
			_alias = true;
		}
		else if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::ROOT) == true)
			_root = _connection->_data._locationBlock->getRoot();
	}
	_connection->_data._root = _root; /* _root variable is needed outside this class for error page response */
}

void	ResolvePath::setPath( void ) {

	if (_alias == true)
		_path = _root + _connection->_request->getRequestTarget().erase(0,
		_connection->_data._locationBlock->getPath().length());
	else
		_path = _root + _connection->_request->getRequestTarget();
}

void	ResolvePath::setFileExtention( void ) {

	std::string chunk;
	size_t posPeriod;
	size_t posFwdSlash;

	if ((posPeriod = _path.find_last_of(".")) != std::string::npos)

		chunk = _path.substr(posPeriod + 1);
		if ((posFwdSlash = chunk.find_first_of("/")) == std::string::npos)
			_connection->_data._fileExtention = chunk;
		else
			_connection->_data._fileExtention = chunk.substr(0, posFwdSlash);
}

void	ResolvePath::checkCgiDirective( void ) {

	if (_connection->_data._locationBlock != NULL) {

		if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::CGI) == true) {

			_connection->_data._path = _path;
			_connection->_responseType = ResponseType::CGI;
			return ;
		}
	}
	_connection->errorLog(403, "cgi execution not allowed", true);
}

void	ResolvePath::checkCgiExt( void ) {

	if (_connection->_request->getRequestMethod() == Method::POST
		&& _connection->_data._fileExtention != CGI_EXT)
		_connection->errorLog(400, "no script path in request target to handle post request", true);
	else
		_connection->_responseType = ResponseType::CGI;
	return;
}

void	ResolvePath::resolvePath( void ) {

	if (stat(_path.c_str(), &_fileStat) == 0) {

			if (isDirectory()) {
				if (searchFile())
					return ;
				else {

					checkAutoindex();
					return ;
				}
			}
			else if (isRegularFile())
				return ;
	}
	_connection->errorLog(404, "the server cannot find the requested resource", true);
}

bool	ResolvePath::searchFile( void ) {

	if (_connection->_data._locationBlock != NULL) {

		if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::DEFAULT_FILE)) {

			if (setDefaultFile())
				return (true);
		}
		else {

			if (setIndexFile())
				return (true);
		}
	}
	else {

		if (setIndexFile())
			return (true);
	}
	return (false);	
}

bool	ResolvePath::setDefaultFile( void ) {

	std::string filePath;

	for (vec_c_iterator it = _connection->_data._locationBlock->getDefaultFile().begin();
		it != _connection->_data._locationBlock->getDefaultFile().end(); it++) {

			if (_path.back() != '/' && *it->begin() != '/')
				filePath = _path + "/" + *it;
			else
				filePath = _path + *it;
			std::ifstream validPath(filePath);
			if (validPath) {

				_path = filePath;
				validPath.close();
				return (true);
			}
	}
	return (false);
}

bool	ResolvePath::setIndexFile( void ) {

	std::string filePath;

	if (_path.back() != '/')
		filePath = _path + "/" + "index.html";
	else
		filePath = _path + "index.html";
	std::ifstream validPath(filePath);
	if (validPath) {

		_path = filePath;
		validPath.close();
		return (true);
	}
	return (false);
}

void	ResolvePath::checkAutoindex( void ) {

	if (_connection->_data._locationBlock != NULL) {

		if (_connection->_data._locationBlock->getAutoindex() == true)
			_connection->_responseType = ResponseType::AUTOINDEX;
		else
				_connection->errorLog(403, "autoindex is disabled", true);
	}
	else
		_connection->errorLog(403, "autoindex is disabled", true);
	return ;
}

bool	ResolvePath::isDirectory( void ) {

	if (_fileStat.st_mode & S_IFDIR)
		return (true);
	return (false);
}

bool	ResolvePath::isRegularFile( void ) {

	if (_fileStat.st_mode & S_IFREG)
		return (true);
	return (false);
}
