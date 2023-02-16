/***********
* includes *
***********/

#include "../includes/AResponse.hpp"
#include "../includes/Connection.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <sys/socket.h> /* send */
#include <iostream> /* std::cout */

/***************************
* constructor + destructor *
***************************/

AResponse::AResponse( Connection *connection ) :
_connection(connection),
_serverName("Windmill"),
_statusText(initStatusText()),
_mimeTypes(initMimeTypes()),
_httpVersion("HTTP/1.1") {}

AResponse::~AResponse( void ) {}

/********************
* private functions *
********************/

is_map		AResponse::initStatusText( void ) {

	is_map	statusText;

	statusText[200]	= "OK";
	statusText[201]	= "Created";
	statusText[301]	= "Moved Permanently";
	statusText[302]	= "Found";
	statusText[303]	= "See Other";
	statusText[307]	= "Temporary Redirect";
	statusText[308]	= "Permanent Redirect";
	statusText[400]	= "Bad Request";
	statusText[403]	= "Forbidden";
	statusText[404]	= "Not Found";
	statusText[405]	= "Method Not Allowed";
	statusText[408]	= "Request Timeout";
	statusText[409]	= "Conflict";
	statusText[413]	= "Payload Too Large";
  	statusText[414]	= "Request-URI Too Long";
	statusText[415]	= "Unsupported Media Type";
	statusText[431]	= "Request Header Fields Too Large";
	statusText[500]	= "Internal Server Error";
	statusText[501]	= "Not Implemented";
	statusText[504]	= "Gateway Timeout";
	statusText[505]	= "HTTP Version Not Supported";

	return (statusText);
}

s_map		AResponse::initMimeTypes( void ) {

	s_map mimeTypes;

	mimeTypes["html"]		= "text/html";
	mimeTypes["htm"]		= "text/html";
	mimeTypes["shtml"]		= "text/html";
	mimeTypes["css"]		= "text/css";
	mimeTypes["jpeg"]		= "image/jpeg";
	mimeTypes["jpg"]		= "image/jpg";
	mimeTypes["js"]			= "application/javascript";
	mimeTypes["mp4"]		= "video/mp4";
	mimeTypes["png"]		= "image/png";
	mimeTypes["pdf"]		= "application/pdf";
	mimeTypes["gif"]		= "image/gif";
	mimeTypes["ico"]		= "image/x-icon";
	mimeTypes["xml"]		= "text/xml";
	mimeTypes["atom"]		= "application/atom+xml";
	mimeTypes["rss"]		= "application/rss+xml";
	mimeTypes["mml"]		= "text/mathml";
	mimeTypes["txt"]		= "text/plain";
	mimeTypes["jad"]		= "text/vnd.sun.j2me.app-descriptor";
	mimeTypes["wml"]		= "text/vnd.wap.wml";
	mimeTypes["htc"]		= "text/x-component";
	mimeTypes["avif"]		= "image/avif";
	mimeTypes["svg"]		= "image/svg+xml";
	mimeTypes["svgz"]		= "image/svg+xml";
	mimeTypes["tif"]		= "image/tiff";
	mimeTypes["tiff"]		= "image/tiff";
	mimeTypes["wbmp"]		= "image/vnd.wap.wbmp";
	mimeTypes["webp"]		= "image/webp";
	mimeTypes["jng"]		= "image/x-jng";
	mimeTypes["bmp"]		= "image/x-ms-bmp";
	mimeTypes["woff"]		= "font/woff";
	mimeTypes["woff2"]		= "font/woff2";
	mimeTypes["jar"]		= "application/java-archive";
	mimeTypes["war"]		= "application/java-archive";
	mimeTypes["ear"]		= "application/java-archive";
	mimeTypes["json"]		= "application/json";
	mimeTypes["hqx"]		= "application/mac-binhex40";
	mimeTypes["doc"]		= "application/msword";
	mimeTypes["ps"]			= "application/postscript";
	mimeTypes["eps"]		= "application/postscript";
	mimeTypes["ai"]			= "application/postscript";
	mimeTypes["rtf"]		= "application/rtf";
	mimeTypes["m3u8"]		= "application/vnd.apple.mpegurl";
	mimeTypes["kml"]		= "application/vnd.google-earth.kml+xml";
	mimeTypes["kmz"]		= "application/vnd.google-earth.kmz";
	mimeTypes["xls"]		= "application/vnd.ms-excel";
	mimeTypes["eot"]		= "application/vnd.ms-fontobject";
	mimeTypes["ppt"]		= "application/vnd.ms-powerpoint";
	mimeTypes["odg"]		= "application/vnd.oasis.opendocument.graphics";
	mimeTypes["odp"]		= "application/vnd.oasis.opendocument.presentation";
	mimeTypes["ods"]		= "application/vnd.oasis.opendocument.spreadsheet";
	mimeTypes["odt"]		= "application/vnd.oasis.opendocument.text";
	mimeTypes["pptx"]		= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mimeTypes["xlsx"]		= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mimeTypes["docx"]		= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mimeTypes["wmlc"]		= "application/vnd.wap.wmlc";
	mimeTypes["wasm"]		= "application/wasm";
	mimeTypes["7z"]			= "application/x-7z-compressed";
	mimeTypes["cco"]		= "application/x-cocoa";
	mimeTypes["jardiff"]	= "application/x-java-archive-diff";
	mimeTypes["jnlp"]		= "application/x-java-jnlp-file";
	mimeTypes["run"]		= "application/x-makeself";
	mimeTypes["pl"]			= "application/x-perl";
	mimeTypes["pm"]			= "application/x-perl";
	mimeTypes["prc"]		= "application/x-pilot";
	mimeTypes["pdb"]		= "application/x-pilot";
	mimeTypes["rar"]		= "application/x-rar-compressed";
	mimeTypes["rpm"]		= "application/x-redhat-package-manager";
	mimeTypes["sea"]		= "application/x-sea";
	mimeTypes["swf"]		= "application/x-shockwave-flash";
	mimeTypes["sit"]		= "application/x-stuffit";
	mimeTypes["tcl"]		= "application/x-tcl";
	mimeTypes["tk"]			= "application/x-tcl";
	mimeTypes["der"]		= "application/x-x509-ca-cert";
	mimeTypes["pem"]		= "application/x-x509-ca-cert";
	mimeTypes["crt"]		= "application/x-x509-ca-cert";
	mimeTypes["xpi"]		= "application/x-xpinstall";
	mimeTypes["xhtml"]		= "application/xhtml+xml";
	mimeTypes["xspf"]		= "application/xspf+xml";
	mimeTypes["zip"]		= "application/zip";
	mimeTypes["bin"]		= "application/octet-stream";
	mimeTypes["exe"]		= "application/octet-stream";
	mimeTypes["dll"]		= "application/octet-stream";
	mimeTypes["deb"]		= "application/octet-stream";
	mimeTypes["dmg"]		= "application/octet-stream";
	mimeTypes["iso"]		= "application/octet-stream";
	mimeTypes["img"]		= "application/octet-stream";
	mimeTypes["msi"]		= "application/octet-stream";
	mimeTypes["msp"]		= "application/octet-stream";
	mimeTypes["msm"]		= "application/octet-stream";
	mimeTypes["mid"]		= "audio/midi";
	mimeTypes["midi"]		= "audio/midi";
	mimeTypes["kar"]		= "audio/midi";
	mimeTypes["mp3"]		= "audio/mpeg";
	mimeTypes["ogg"]		= "audio/ogg";
	mimeTypes["m4a"]		= "audio/x-m4a";
	mimeTypes["ra"]			= "audio/x-realaudio";
	mimeTypes["3gpp"]		= "video/3gpp";
	mimeTypes["3gp"]		= "video/3gpp";
	mimeTypes["ts"]			= "video/mp2t";
	mimeTypes["mpeg"]		= "video/mpeg";
	mimeTypes["mpg"]		= "video/mpeg";
	mimeTypes["mov"]		= "video/quicktime";
	mimeTypes["webm"]		= "video/webm";
	mimeTypes["flv"]		= "video/x-flv";
	mimeTypes["m4v"]		= "video/x-m4v";
	mimeTypes["mng"]		= "video/x-mng";
	mimeTypes["asx"]		= "video/x-ms-asf";
	mimeTypes["asf"]		= "video/x-ms-asf";
	mimeTypes["wmv"]		= "video/x-ms-wmv";
	mimeTypes["avi"]		= "video/x-msvideo";

	return (mimeTypes);
}

void		AResponse::generateHeaderFields( void ) {

	std::string headerFields =
	"Content-Length: " + toString(_body.size()) + CRLF +
	"Server: " + _serverName + CRLF +
	"Date: " + getTime() + CRLF
	"Content-Type: " + _mimeTypes[_connection->_data._fileExtention] + CRLF + CRLF;

	_headerFields = generateStatusLine().append(headerFields);
}

void		AResponse::printResponse( void ) {

	std::cout << BOLDCYAN "\n*******************" << RESET << std::endl;
	std::cout << BOLDCYAN "* RESPONSE HEADER *" << RESET << std::endl;
	std::cout << BOLDCYAN "*******************\n" << RESET << std::endl;
	std::cout << CYAN << _headerFields << RESET;
	std::cout << BOLDCYAN "***********************" << RESET << std::endl;
	std::cout << BOLDCYAN "* END RESPONSE HEADER *" << RESET << std::endl;
	std::cout << BOLDCYAN "***********************\n" << RESET << std::endl;
}

/**********************
* protected functions *
**********************/

std::string	AResponse::generateStatusLine( void ) {

	return (_httpVersion + SP +
	toString(_connection->_data._statusCode) + SP +
	_statusText[_connection->_data._statusCode]) + CRLF;
}

void		AResponse::generateResponse( void ) {

	generateHeaderFields();
	_response = _headerFields + _body;
}

void		AResponse::sendResponse( void ) {

	ssize_t bytesSent;
	ssize_t chunkSize = std::min(_response.length(), static_cast<size_t>(CHUNK_SIZE));

	bytesSent = send(_connection->_fd, _response.c_str(), chunkSize, 0);
	if (bytesSent == ERROR) {

		_connection->errorLog(500, "send response failed", false);
		_connection->_state = State::REMOVE;
		return ;
	}
	_response.erase(0, bytesSent);
	if (_response.size() > 0)
		_connection->_state = State::CHUNKED;
	else {

		if (_connection->getRequest() != NULL)
			_connection->getRequest()->printRequest();
		printResponse();
		_connection->_state = State::DONE;
	}
}

std::string	getDate( std::string date, std::string gmt, int startPos, int len, std::string str) {

	std::string updated;

	updated = gmt.substr(startPos, len);
	return (date + updated + str);
}

std::string	AResponse::getTime( void ) {

	std::string date;
	std::string day;

	time_t ttime = std::time(0);
	tm *gmt_time = std::gmtime(&ttime);
	std::string gmt = std::asctime(gmt_time);

	date = getDate(date, gmt, 0, 3, ", ");
	day = gmt.substr(8, 2);
	if (day[0] == ' ')
		day[0] = '0';
	date += day + " ";
	date = getDate(date, gmt, 4, 3, " ");
	date = getDate(date, gmt, 20, 4, " ");
	date = getDate(date, gmt, 11, 8, " GMT");
	return (date);
}
