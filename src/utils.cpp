/***********
* includes *
***********/

#include "../includes/utils.hpp"
#include <iostream> /* std::cout */
#include <csignal> /* std::signal */

/************
* functions *
************/

std::string	percentDecoding( std::string str ) {

	sc_map		charEncoding;
	std::string decoded;

	initCharDecoding(charEncoding);
	for (size_t i = 0; i < str.size(); i++) {

		if (str[i] == '%') {

			std::string key = str.substr(i, 3);
			decoded.push_back(charEncoding[key]);
			i = i + 2;
		}
		else if (str[i] == '+')
			decoded.push_back(' ');
		else
			decoded.push_back(str[i]);
	}
	return(decoded);
}

void	initCharDecoding( sc_map &charEncoding ) {

	charEncoding["%3A"]	= ':';
	charEncoding["%2F"]	= '/';
	charEncoding["%3F"]	= '?';
	charEncoding["%23"]	= '#';
	charEncoding["%5B"]	= '[';
	charEncoding["%5D"]	= ']';
	charEncoding["%40"]	= '@';
	charEncoding["%21"]	= '!';
	charEncoding["%24"]	= '$';
	charEncoding["%26"]	= '&';
	charEncoding["%27"]	= '\'';
	charEncoding["%28"]	= '(';
	charEncoding["%29"]	= ')';
	charEncoding["%2A"]	= '*';
	charEncoding["%2B"]	= '+';
	charEncoding["%2C"]	= ',';
	charEncoding["%3B"]	= ';';
	charEncoding["%3D"]	= '=';
	charEncoding["%25"]	= '%';
	charEncoding["%20"]	= ' ';
}

void	signalHandler( int signum )
{
	if (signum == SIGINT) {
		std::cout << GREEN "\n\b\bGoodbye." << RESET << std::endl;
		exit(1);
	}
	if (signum == SIGQUIT) {
		std::cout << GREEN "\n\b\bGoodbye." << RESET << std::endl;
		exit(1);
	}
}

void	setSignals( void ) {

	if (std::signal(SIGINT, signalHandler) == SIG_ERR ||
		std::signal(SIGQUIT, signalHandler) == SIG_ERR ||
		std::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {

		errorLogNoResponse(500, "failed to set signal handler");
		throw ("Error: failed to set signal handler");
	}
}

std::string	getStatus( int statusCode ) {

	is_map statusMessage;

	statusMessage[400] = "400: Bad Request";
	statusMessage[403] = "403: Forbidden";
	statusMessage[404] = "404: Not Found";
	statusMessage[405] = "405: Method Not Allowed";
	statusMessage[408] = "408: Request Timeout";
	statusMessage[409] = "409: Conflict";
	statusMessage[413] = "413: Payload Too Large";
	statusMessage[414] = "414: Request-URI Too Long";
	statusMessage[415] = "415: Unsupported Media Type";
	statusMessage[431] = "431: Request Header Fields Too Large";
	statusMessage[500] = "500: Internal Server Error";
	statusMessage[501] = "501: Not Implemented";
	statusMessage[502] = "502: Bad Gateway";
	statusMessage[504] = "504: Gateway Timeout";
	statusMessage[505] = "505: HTTP Version Not Supported";

	return (statusMessage[statusCode]);
}

void	errorLogNoResponse( int statusCode, std::string msg ) {

	if (statusCode == EXIT) {

		std::cout << RED << "[fatal error] " << msg << RESET << std::endl;
		exit(EXIT);
	}
	else
		std::cout << YELLOW << "[error] " << msg << RESET << std::endl;
}

bool	isAllDigit( std::string str ) {

	for (size_t i = 0; i < str.size(); i++) {

		if (!isdigit(str[i]))
			return (false);
	}
	return (true);
}

std::string ltrim(const std::string &s) {

	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {

	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
	
	return rtrim(ltrim(s));
}

bool		endsWith( std::string arg, std::string extension ) {

	if (arg.size() > extension.size() &&
		arg.compare(arg.size() - extension.size(),
			extension.size(), extension) == 0)
		return (true);
	return (false);
}
