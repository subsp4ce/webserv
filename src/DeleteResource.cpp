/***********
* includes *
***********/

#include "../includes/DeleteResource.hpp"
#include "../includes/Connection.hpp"
#include <sys/stat.h> /* struct stat */

/***************************
* constructor + destructor *
***************************/

DeleteResource::DeleteResource( Connection *connection ) :
AResponse(connection) {

	if (deleteResource() == ERROR)
		return ;
	generateBody();
	generateResponse();
	sendResponse();
}

DeleteResource::~DeleteResource( void ) {}

/********************
* private functions *
********************/

int	DeleteResource::deleteResource( void ) {

	struct stat	fileStat;

	if (stat(_connection->_data._path.c_str(), &fileStat) == 0) {

		if (remove(_connection->_data._path.c_str()) == ERROR)
			return (_connection->errorLog(500, "the server failed to remove the requested resource", true));
		_connection->_data._statusCode = 204;
	}
	else
		return (_connection->errorLog(404, "the server cannot find the requested resource", true));
	return (0);
}

void	DeleteResource::generateHeaderFields( void ) {

	std::string headerFields =
	"Content-Length: " + std::to_string(_body.size()) + CRLF +
	"Server: " + _serverName + CRLF +
	"Date: " + getTime() + CRLF + CRLF;

	_headerFields = generateStatusLine() + headerFields;
}

int	DeleteResource::generateBody( void ) { return (0); }
