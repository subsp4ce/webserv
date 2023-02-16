/***********
* includes *
***********/

#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include "../includes/Connection.hpp"
#include "../includes/ServerBlock.hpp"
#include <sys/socket.h> /* recv */
#include <iostream> /* std::cout */

/***************************
* constructor + destructor *
***************************/

Request::Request( Connection *connection ) :
_connection(connection),
_requestLineParsed(false),
_httpProtocol("HTTP/1.1"),
_headerParsed(false),
_contentLength(0),
_headerLength(0),
_queryString(""),
_host("0.0.0.0") {

	readRequest();
}

Request::~Request( void ) {}

/**********
* getters *
***********/

int				const & Request::getRequestMethod( void ) const { return (_requestMethod); }
std::string		& 		Request::getRequestTarget( void ) { return (_requestTarget); }
std::string		const & Request::getQueryString( void ) const { return ( _queryString ); }
Request::s_map	const & Request::getHeaderFields( void ) const { return (_requestHeaderFields); }
std::string		&		Request::getHeaderValue( std::string key ) { return (_requestHeaderFields[key]); }
std::string		const & Request::getHost( void ) const { return ( _host ); }
std::string		const & Request::getBody( void ) const { return ( _body ); }
std::string		const & Request::getRedirectLocation( void ) const { return (_redirectLocation); }
size_t			const & Request::getContentLength( void ) const { return (_contentLength); }
std::string		& 		Request::getMethodToString( int method ) {

	_methodToString[Method::GET] = "GET";
	_methodToString[Method::POST] = "POST";
	_methodToString[Method::DELETE] = "DELETE";

	return (_methodToString[method]);
}

/**********
* setters *
***********/

void	Request::setRedirectLocation( std::string redirectLocation ) { _redirectLocation = redirectLocation; }

/********************
* private functions *
********************/

int		Request::readRequest( void ) {

	char buf[BUF_SIZE + 1];
	std::memset(buf, '\0', BUF_SIZE + 1);
	int bytesRead = 0;

	bytesRead = recv(_connection->_fd, buf, BUF_SIZE, 0);
	if (bytesRead == ERROR) {

		_connection->errorLog(500, "recv failed", false);
		_connection->_state = State::REMOVE;
		return (0);
	}
	if (bytesRead == 0) {
		_connection->errorLog(400, "client disconnected", false);
		_connection->_state = State::DISCONNECTED;
		return (0);
	}

	parseBuf(buf, bytesRead);
	return (0);
}

void	Request::parseBuf( char *buf, int bytesRead ) {

	if (!_requestLineParsed) {

		if (parseRequestLine(buf) == ERROR)
			return ;
		parseRequest();
		return ;
	}
	_request.append(buf, bytesRead);
	parseRequest();
}

int		Request::parseRequest( void ) {

	size_t pos = _request.find("\r\n\r\n");

	if (pos != std::string::npos && _headerParsed == false) {

		_header = _request.substr(0, pos);
		if (checkMaxHeaderSize(_header.length()) == ERROR)
			return (ERROR);
		_request.erase(0, pos + 4);
		if (parseHeader() == ERROR)
			return (ERROR);
	}
	else if (_headerParsed == true) {
		if (checkBodySize() == ERROR)
			return (ERROR);
		_body.append(_request);
		_request.clear();
	}
	updateState();
	return (0);
}

int		Request::parseHeader( void ) {

	size_t pos;

	prepHeader();
	_headerParsed = true;
	while ((pos = _header.find(CRLF)) != std::string::npos) {

		_line = _header.substr(0, pos);
		if (_line.find(':') != std::string::npos) {

			setHeaderField();
			if (_connection->getState() == State::ERR)
				return (ERROR);
		} 
		else
			return (_connection->errorLog(400, "header is malformed", true));
		_header.erase(0, pos + 2);
		_line.clear();
	}
	if (checkBody() == ERROR)
			return (ERROR);
	return (0);
}

/*
	remove CRLF, see comment parseRequestLine()
	get last header field in while loop
*/

void	Request::prepHeader( void ) {

	if (_header.length() > 0) {

		_header.erase(0, 2);
		_header.append(CRLF);
	}
}

/* Request-Line = Method SP Request-URI SP HTTP-Version CRLF */

int		Request::parseRequestLine( std::string buf ) {

	std::string requestLine;

	_requestLine.append(buf);
	size_t pos = _requestLine.find(CRLF);
	if (pos != std::string::npos) {
		
		requestLine = _requestLine.substr(0, pos);
		_requestLineParsed = true;
	}
	else
		return (0);
	if (splitRequestLine(requestLine) == ERROR || 
		setRequestMethod() == ERROR ||
		setRequestTarget() == ERROR ||
		setHttpProtocol() == ERROR )
		return (ERROR);
	_requestLine.erase(0, pos); /* keep CRLF to find potential end of header on next recv */
	_request.append(_requestLine);
	return (0);
}

int		Request::splitRequestLine( std::string requestLine ) {

	std::string	delimiter = " ";
	size_t pos = 0;

	if (checkMaxHeaderSize(requestLine.length()) == ERROR)
		return (ERROR);

	while ((pos = requestLine.find(delimiter)) != std::string::npos) {

		if (_requestLineTokens.size() == 2 || pos == 0)
			return (_connection->errorLog(400, "request-line is malformed", true));
		_requestLineTokens.push_back(requestLine.substr(0, pos));
		requestLine.erase(0, pos + delimiter.length());
	}
	_requestLineTokens.push_back(requestLine);
	if (_requestLineTokens.size() != 3)
		return (_connection->errorLog(400, "request-line is malformed", true));
	return (0);
}

int		Request::setRequestMethod( void ) {

	if (_requestLineTokens[RequestLine::METHOD] == "GET")
		_requestMethod = Method::GET;
	else if (_requestLineTokens[RequestLine::METHOD] == "POST")
		_requestMethod = Method::POST;
	else if (_requestLineTokens[RequestLine::METHOD] == "DELETE")
		_requestMethod = Method::DELETE;
	else
		return (_connection->errorLog(501, "request method is not implemented", true));
	return (0);
}

int		Request::setRequestTarget( void ) {

	size_t pos = 0;
	_requestTarget = _requestLineTokens[RequestLine::REQUEST_TARGET];
	if (_requestTarget.length() > MAX_URI_SIZE)
		return (_connection->errorLog(414, "URI too long", true));
	if ((pos = _requestTarget.find("?")) != std::string::npos) {

		_queryString =  _requestLineTokens[RequestLine::REQUEST_TARGET].substr(pos + 1,
						_requestLineTokens[RequestLine::REQUEST_TARGET].length() - pos);
		size_t pos = _requestTarget.find("?");
		_requestTarget.erase(pos, _requestTarget.size() - pos);
	}
	_requestTarget = percentDecoding(_requestTarget);
	return (0);
}

int		Request::setHttpProtocol( void ) {

	if (_requestLineTokens[RequestLine::PROTOCOL] != _httpProtocol)
		return (_connection->errorLog(505, "http protocol version not supported", true));
	return (0);
}

void	Request::setHeaderField( void ) {

	std::string fieldName = _line.substr(0, _line.find(":"));
	if (fieldName.find_first_of(WHITESPACE) != std::string::npos)
		_connection->errorLog(400, "header field contains whitespace", true);
	std::string	fieldValue = _line.substr(_line.find(":") + 1, (_line.size() - 1) - _line.find(":")); /* +1 to skip the ':' */
	fieldValue = trim(fieldValue); /* trim leading and trailing whitespace from header field value */
	_requestHeaderFields.insert(std::pair<std::string, std::string>(fieldName, fieldValue));

	if (fieldName == "Host")
		setHost();
	else if (fieldName == "Content-Length")
		setContentLength();
	else if (fieldName == "Transfer-Encoding")
		_connection->errorLog(415, "Transfer-Encoding not supported", true);
}

void	Request::setHost( void ) {

	_host = trim(_line.erase(0, 5));
	if (_host.find_first_of(WHITESPACE) != std::string::npos)
		_connection->errorLog(400, "Host header field has multiple values", true);
	if ( _host.find(":") != std::string::npos )
		_host = _line.substr(0, _host.find(":"));
}

void	Request::setContentLength( void ) {

	size_t maxLength = SERVER_MAX_BODY_SIZE * 1000000;

	if (!isAllDigit(_requestHeaderFields["Content-Length"]))
		_connection->errorLog(400, "Content-Length header value is malformed", true);
	else
		_contentLength = toInt(_requestHeaderFields["Content-Length"]);
	if (_contentLength > maxLength)
		_connection->errorLog(413, "server max body size exceeded " + toString(SERVER_MAX_BODY_SIZE) + "mb", true);
}

int		Request::checkMaxHeaderSize( int len ) {

	_headerLength += len;
	if (_headerLength > MAX_HEADER_SIZE)
		return (_connection->errorLog(431, "header length exceeds server maximum of " + toString(MAX_HEADER_SIZE), true));
	return (0);
}

int		Request::checkBody( void ) {

	_body.append(_request);
	_request.clear();
	if (_body.size() > 0 && _requestHeaderFields.find("Content-Length") == _requestHeaderFields.end())
		return (_connection->errorLog(400, "request contains a message-body but Content-Length is not given", true));
	return (0);
}

int		Request::checkBodySize( void ) {

	if (static_cast<int>(_body.size() + _request.size()) > _connection->_data._serverBlock->getClientMaxBodySize())
		return(_connection->errorLog(413, "body exceeds client maximum body size", true));
	return (0);
}

void	Request::updateState( void ) {

	if ( _headerParsed == false) {

		_connection->_state = State::RECEIVE;
		return ;
	}

	if (_body.length() == _contentLength)
		_connection->_state = State::DONE;
	else if (_body.length() > _contentLength)
		_connection->errorLog(400, "body length exceeds content-length header value", true);
	else
		_connection->_state = State::RECEIVE;
}

void		Request::printRequest( void ) {

	std::cout << MAGENTA "\n*******************" << RESET << std::endl;
	std::cout << MAGENTA "* REQUEST HEADER *" << RESET << std::endl;
	std::cout << MAGENTA "*******************\n" << std::endl;
	printRequestLineTokens();
	for (s_map::iterator it = _requestHeaderFields.begin(); it != _requestHeaderFields.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << "\n***********************" << RESET << std::endl;
	std::cout << MAGENTA "* END REQUEST HEADER *" << RESET << std::endl;
	std::cout << MAGENTA "***********************\n" << RESET << std::endl;
}

/**************************
* private debug functions *
**************************/

void		Request::printParsedRequest( void ) {

	std::cout << BOLDBLUE "PARSED REQUEST HEADERS" RESET <<  std::endl;
	std::cout << BLUE "_requestMethod: " << getMethodToString(_requestMethod) << RESET << std::endl;
	std::cout << BLUE "_requestTarget: " << _requestTarget << RESET << std::endl;
	std::cout << BLUE << "_queryString: " << _queryString << RESET << std::endl;
	std::cout << BLUE "_httpProtocol: " << _httpProtocol << RESET << std::endl;
	std::cout << BLUE << "_host: " << _host << RESET << std::endl;
}

void		Request::printRequestLineTokens( void ) {

	for (s_vector::iterator it = _requestLineTokens.begin(); it != _requestLineTokens.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl;
}

void		Request::printHeaderFields( void ) {

	for (s_map_it it = _requestHeaderFields.begin(); it != _requestHeaderFields.end(); ++it)
		std::cout << it->first << " => " << it->second << std::endl;
}

std::string	Request::printState( int &state ) {

	is_map	stateText;

	stateText[0]	= "ACCEPTED";
	stateText[1]	= "CHUNKED";
	stateText[2]	= "ERR";
	stateText[3]	= "DISCONNECTED";
	stateText[4]	= "RECEIVE";
	stateText[5]	= "REDIRECT";
	stateText[6]	= "CGI";
	stateText[7]	= "DONE";

	return (stateText[state]);
}
