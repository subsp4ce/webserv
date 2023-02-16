/***********
* includes *
***********/

#include "../includes/ErrorResponse.hpp"
#include "../includes/Connection.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/LocationBlock.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <fstream> /* std::ifstream */

/***************************
* constructor + destructor *
***************************/

ErrorResponse::ErrorResponse( Connection *connection ) :
AResponse(connection) {

	if (_connection->_data._serverBlock)
		_errorPage = _connection->_data._serverBlock->getErrorPage();

	generateBody();
	generateResponse();
	sendResponse();
}

ErrorResponse::~ErrorResponse( void ) {}

/********************
* private functions *
********************/

int			ErrorResponse::generateBody( void ) {

	if (_errorPage.find(toString(_connection->_data._statusCode)) == _errorPage.end())
		_body = defaultErrorPage();
	else
		_body = customErrorPage();
	return (0);
}

std::string	ErrorResponse::defaultErrorPage( void ) {

	std::string error;
	std::string	errorTail =
	"<hr><center>Windmill</center>"
	"</body>"
	"</html>";

	switch (_connection->_data._statusCode) {

		case 301:
			error =
			"<html>" CRLF
			"<head><title>301 Moved Permanently</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>301 Moved Permanently</h1></center>" CRLF;
			break;
		case 302:
			error =
			"<html>" CRLF
			"<head><title>302 Found</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>302 Found</h1></center>" CRLF;
			break;
		case 303:
			error =
			"<html>" CRLF
			"<head><title>303 See Other</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>303 See Other</h1></center>" CRLF;
			break;
		case 307:
			error =
			"<html>" CRLF
			"<head><title>307 Temporary Redirect</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>307 Temporary Redirect</h1></center>" CRLF;
			break;
		case 308:
			error =
			"<html>" CRLF
			"<head><title>308 Permanent Redirect</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>308 Permanent Redirect</h1></center>" CRLF;
			break;
		case 400:
			error =
			"<html>" CRLF
			"<head><title>400 Bad Request</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>400 Bad Request</h1></center>" CRLF;
			break;
		case 403:
			error =
			"<html>" CRLF
			"<head><title>403 Forbidden</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>403 Forbidden</h1></center>" CRLF;
			break;
		case 404:
			error =
			"<html>" CRLF
			"<head><title>404 Not Found</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>404 Not Found</h1></center>" CRLF;
			break;
		case 405:
			error =
			"<html>" CRLF
			"<head><title>405 Not Allowed</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>405 Not Allowed</h1></center>" CRLF;
			break;
		case 408:
			error = 
			"<html>" CRLF
			"<head><title>408 Request Time-out</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>408 Request Time-out</h1></center>" CRLF;
			break;
		case 409:
			error = 
			"<html>" CRLF
			"<head><title>409 Conflict</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>409 Conflict</h1></center>" CRLF;
			break;
		case 413:
			error =
			"<html>" CRLF
			"<head><title>413 Request Entity Too Large</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>413 Request Entity Too Large</h1></center>" CRLF;
			break;
		case 414:
			error =
			"<html>" CRLF
			"<head><title>414 Request-URI Too Long</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>414 Request-URI Too Long</h1></center>" CRLF;
			break;
		case 415:
			error =
			"<html>" CRLF
			"<head><title>415 Unsupported Media Type</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>415 Unsupported Media Type</h1></center>" CRLF;
			break;
		case 431:
			error =
			"<html>" CRLF
			"<head><title>431 Request Header Fields Too Large</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>431 Request Header Fields Too Large</h1></center>" CRLF;
			break;
		case 500:
			error =
			"<html>" CRLF
			"<head><title>500 Internal Server Error</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>500 Internal Server Error</h1></center>" CRLF;
			break;
		case 501:
			error =
			"<html>" CRLF
			"<head><title>501 Not Implemented</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>501 Not Implemented</h1></center>" CRLF;
			break;
		case 502:
			error =
			"<html>" CRLF
			"<head><title>502 Bad Gateway</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>502 Bad Gateway</h1></center>" CRLF;
			break;
		case 504:
			error =
			"<html>" CRLF
			"<head><title>504 Gateway Timeout</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>504 Gateway Timeout</h1></center>" CRLF;
			break;
		case 505:
			error =
			"<html>" CRLF
			"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF;
	}
	return (error + errorTail);
}

std::string	ErrorResponse::customErrorPage( void ) {

	std::stringstream buffer;

	std::string path = _errorPage[toString(_connection->_data._statusCode)];
	if (_connection->_data._root.back() != '/' && path[0] != '/')
		path = _connection->_data._root + "/" + _errorPage[toString(_connection->_data._statusCode)];
	else
		path = _connection->_data._root + _errorPage[toString(_connection->_data._statusCode)];

	std::ifstream ifs(path); /* if path doesn't exist, return default error page */
	if (!ifs)
		return (defaultErrorPage());
	buffer << ifs.rdbuf();
	ifs.close();
	return (buffer.str());
}

void		ErrorResponse::generateHeaderFields( void ) {

	std::string headerFields =
		"Content-Length: " + toString(_body.length()) + CRLF +
		"Server: " + _serverName + CRLF +
		"Date: " + getTime() + CRLF +
		"Content-Type: text/html" + CRLF;

	if (_connection->_data._serverBlock) {

		if (firstDigit() == 3)
			headerFields.append("Location: " + _connection->_request->getRedirectLocation() + CRLF);
		else if (_connection->_data._statusCode == 405)
			headerFields.append(getAllow() + CRLF);
	}
	headerFields.append(CRLF);
	_headerFields = generateStatusLine() + headerFields;
}

int			ErrorResponse::firstDigit( void ) const {

	int n = _connection->_data._statusCode;

	while (n >= 10)
		n /= 10;

	return (n);
}

std::string	ErrorResponse::getAllow( void ) {

	std::string allow = "Allow: ";

	if (_connection->_data._locationBlock != NULL) {

		for (vec_c_iterator it = _connection->_data._locationBlock->getAllowedMethods().begin();
			it != _connection->_data._locationBlock->getAllowedMethods().end(); it++) {

			allow.append(*it + ", ");
		}
		if (allow != "Allow: ") {

			allow.pop_back();
			allow.pop_back();
		}
	}
	return (allow);
}
