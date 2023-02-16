/***********
* includes *
***********/

#include "../includes/CgiResponse.hpp"
#include "../includes/Cgi.hpp"
#include "../includes/Connection.hpp"
#include "../includes/utils.hpp"

/***************************
* constructor + destructor *
***************************/

CgiResponse::CgiResponse( Connection *connection ) :
AResponse(connection),
_contentLength(0) {

	generateBody();
	generateResponse();
	sendResponse();
}

CgiResponse::~CgiResponse( void ) {}

/********************
* private functions *
********************/

int	CgiResponse::generateBody( void ) {

	std::string cgiResponse = _connection->_cgiData->getBody();
	size_t pos = cgiResponse.find("\r\n\r\n");

	if (pos != std::string::npos)
		_contentLength = cgiResponse.substr(pos + 4).length();
	else
		_contentLength = cgiResponse.length();

	_cgiHeaderFields = cgiResponse.substr(0, pos) + CRLF;
	_body = cgiResponse.substr(pos + 4);
	return (0);
}

void	CgiResponse::generateHeaderFields( void ) {

	std::string headerFields =
		"Content-Length: " + toString(_contentLength) + CRLF +
		"Date: " + getTime() + CRLF +
		"Server: " + _serverName + CRLF + CRLF;

	_headerFields = _cgiHeaderFields.append(headerFields);
}
