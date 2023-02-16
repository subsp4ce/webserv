/***********
* includes *
***********/

#include "../includes/ConfigParser.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/utils.hpp"
#include <fstream> /* std::ifstream */
#include <iostream> /* std::cout */

/***************************
* constructor + destructor *
***************************/

ConfigParser::ConfigParser( int argc, char **argv ) :
_lineCount(1),
_inServerBlock(false),
_inLocationBlock(false),
_locationIdx(0),
_serverIdx(0),
_server(false),
_location(false),
_currSb(NULL),
_currLb(NULL) {

	if (argc == 1)
		_fileName = "config_files/default.conf";
	else if (argc > 2)
		throw (error(INVALID_AMOUNT_OF_ARGS, "", "\"./webserv [path to config file]\""));
	else if (!endsWith(argv[1], ".conf"))
		throw (error(INVALID_CONF_EXTENSION, "", ""));
	else
		_fileName = argv[1];

	readFile();
}

ConfigParser::~ConfigParser( void ) {}


/*******************
* public functions *
*******************/

std::string	const &			ConfigParser::getFileName( void ) const { return (_fileName); }
ConfigParser::sb_vector & 	ConfigParser::getServerBlock() { return ( _serverBlock ); }

/********************
* private functions *
********************/

void	ConfigParser::readFile( void ) {

	std::string line;

	std::ifstream ifs(_fileName);
	if (!ifs)
		throw(error(OPEN_FAILED, _fileName, ""));
	if (ifs.peek() == EOF)
		throw (error(EMPTY_FILE, _fileName, ""));
	while ((std::getline(ifs, line))) {

		if (ifs.fail() || ifs.bad())
			throw (error(GETLINE_FAILED, _fileName, ""));
		if (!emptyLine(line)) {
			makeTokens(line);
			line.clear();
		}
		_tokens.push_back("\n"); /* adding a newline for line count */
	}
	ifs.close();
	setDirective();
	if (_server == false)
		throw (error(NO_SERVER_BLOCK_FOUND, _fileName, ""));
	bracketCheck();
}

void	ConfigParser::makeTokens( std::string line ) {

	int 		start = 0;
	int 		count = 0;
	std::string token;

	for (size_t i = 0; i < line.size(); i++) {

		count = 0;
		while (std::isspace(line[i]))
			i++;
		start = i; /* start of token */
		if (line[start] == '#') /* if line or word starts with a '#' the rest of the line can be ignored */
			return ;
		while (std::isprint(line[i]) && line[i] != ' ') { /* count word length */

			count++;
			i++;
		}
		token = line.substr(start, count); /* holds token */
		if (specialChar(token)) { /* check if tokens has special characters */

			if (handleSpecialChar(token)) /* modify token according to character, if function returns 1 a comment is found and you can return */
				return ;
		}
		else if (count != 0)
			_tokens.push_back(token);
	}
}

bool	ConfigParser::handleSpecialChar( std::string token ) {

	std::string	newToken;

	for (std::string::size_type i = 0; i < token.size(); i++) {

		if (token[i] != ';' && token[i] != '{' && token[i] != '}' && token[i] != '#')
			newToken += token[i];
		else if (token[i] == ';' || token[i] == '{' || token[i] == '}' || token[i] == '#') {

			if (newToken.size() > 0) { /* if size is 0, the token starts with for example a ';' */
				_tokens.push_back(newToken);
				newToken.clear();
			}
			if (token[i] == ';')
				_tokens.push_back(";");
			else if (token[i] == '{')
				_tokens.push_back("{");
			else if (token[i] == '}')
				_tokens.push_back("}");
			else if (token[i] == '#') {
				return (1);
			}
		}
	}
	if (newToken.size() > 0) /* the last bit of the token */
		_tokens.push_back(newToken);
	return (0);
}

void	ConfigParser::setDirective( void ) {

	for (_token = _tokens.begin(); _token != _tokens.end(); _token++) {

		if (*_token == "\n")
			_lineCount++;
		else if (*_token == "{")
			throw (error(UNEXPECTED_CHAR, *_token, ""));
		else if (_inServerBlock == false && _inLocationBlock == false)
			newServerBlock();
		else if (*_token == "location" && _inServerBlock == true && _inLocationBlock == false)
			newLocationBlock();
		else if (*_token == "}")
			closeBlock();
		else if (_inServerBlock == true && _inLocationBlock == false)
			serverDirective();
		else if (_inServerBlock == true && _inLocationBlock == true)
			locationDirective();
		if (_token == _tokens.end())
			break;
	}
}

void	ConfigParser::newServerBlock( void ) {

	_locationIdx = 0;
	_location = false;

	if (*_token == "}")
		throw (error(UNEXPECTED_CHAR, *_token, ""));
	else if (*_token == "location")
		throw(error(DIRECTIVE_NOT_ALLOWED_HERE, *_token, ""));
	else if (*_token != "server")
		throw (error(SERVER_BLOCK_EXPECTED, *_token, ""));
	else if ((_token + 1) == _tokens.end())
		throw (error(UNEXPECTED_EOF, "", ""));
	else if (*(++_token) != "{")
		throw(error(MISSING_OPENING_BRACKET, *(_token - 1), ""));

	_inServerBlock = true;
	_serverBlock.push_back(ServerBlock());

	if (_server == false)
		_server = true;
	else
		_serverIdx++;
	_currSb = &_serverBlock[_serverIdx];
}

void	ConfigParser::newLocationBlock( void ) {

	std::string	directiveName = *_token;

	_serverBlock[_serverIdx].setLocation();
	if (_location == false)
		_location = true;
	else
		_locationIdx++;

	_currLb = &_serverBlock[_serverIdx].getLocation()[_locationIdx];
	if (++_token == _tokens.end())
		throw (error(UNEXPECTED_EOF, "", ""));
	else if (*_token == "{")
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"location [route]\""));
	else
		_currLb->setPath(*_token);

	if (++_token == _tokens.end())
		throw (error(UNEXPECTED_EOF, "", ""));
	else if (*_token != "{")
		throw (error(MISSING_OPENING_BRACKET, directiveName, ""));
	_inLocationBlock = true;
}

void	ConfigParser::closeBlock( void ) {

	if (_inServerBlock == true && _inLocationBlock == true)
		_inLocationBlock = false;
	else if (_inServerBlock == true && _inLocationBlock == false)
		_inServerBlock = false;
}

void	ConfigParser::serverDirective( void ) {

	if (*_token == ";")
		throw (error(UNEXPECTED_CHAR, *_token, ""));
	else if (*_token == "listen")
		setListen();
	else if (*_token == "server_name")
		setServerName();
	else if (*_token == "root")
		setServerRoot();
	else if (*_token == "error_page")
		setErrorPage();
	else if (*_token == "client_max_body_size")
		setClientMaxBodySize();
	else
		throw (error(UNKNOWN_DIRECTIVE, *_token, ""));
}

void	ConfigParser::locationDirective( void ) {

	if (*_token == ";")
		throw (error(UNEXPECTED_CHAR, *_token, ""));
	if (*_token == "allowed_methods")
		setAllowedMethods();
	else if (*_token == "return")
		setRedirect();
	else if (*_token == "root")
		setLocationRoot();
	else if (*_token == "alias")
		setAlias();
	else if (*_token == "autoindex")
		setAutoindex();
	else if (*_token == "default_file")
		setDefaultFile();
	else if (*_token == "cgi")
		setCgi();
	else if (*_token == "file_upload")
		setFileUpload();	
	else
		throw (error(UNKNOWN_DIRECTIVE, *_token, ""));
}

std::vector<std::string>	ConfigParser::getArgs( void ) {

	std::vector<std::string>	value;
	std::string 				directiveName = *_token;

	while (++_token != _tokens.end()) {

		if (*_token == ";")
			break ;
		else if (*_token == "}")
			throw (error(UNEXPECTED_CHAR, *_token, ""));
		value.push_back(*_token);
	}
	return (value);
}

void	ConfigParser::setListen( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();
	size_t pos;

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"listen [HOST:PORT]\" or \"listen [HOST]\" or \"listen [PORT]\""));

	if ((pos = args[0].find(':')) != std::string::npos)
		splitListen(pos, directiveName, args);
	else {

		if (isAllDigit(args[0])) {
			
			if (args[0].length() > 5)
				throw (error(INVALID_PORT, directiveName, ""));
			_currSb->setPort(toInt(args[0]));
			if (_currSb->getPort() < 1 || _currSb->getPort() > 65535)
				throw (error(INVALID_PORT, directiveName, ""));
		}
		else
			_currSb->setHost(args[0]);
	}
	if (_currSb->getDirectiveExists(ServerBlock::LISTEN) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	_currSb->setListen(args[0]);
	_currSb->setDirectiveExists(ServerBlock::LISTEN);
}

void	ConfigParser::splitListen( size_t pos, std::string directiveName, s_vector args ) {

	std::string leftPart = args[0].substr(0, pos); /* left part is either a host name or an IP address */
	std::string rightPart = args[0].substr(pos + 1, args[0].size() - pos - 1);
	if (isAllDigit(rightPart)) {

		_currSb->setHost(leftPart);
		if (rightPart.length() > 5)
			throw (error(INVALID_PORT, directiveName, ""));
		_currSb->setPort(toInt(rightPart));
		if (_currSb->getPort() < 1 || _currSb->getPort() > 65535)
			throw (error(INVALID_PORT, directiveName, ""));
	}
	else
		throw (error(INVALID_PORT, directiveName, ""));
}

void	ConfigParser::setServerName( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() == 0)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"server_name [example] [example.com] [example.org] [...]\""));

	for (vec_iterator it = args.begin(); it != args.end(); it++)
		_currSb->setServerName(*it);

	_currSb->setDirectiveExists(ServerBlock::SERVER_NAME);
}

void	ConfigParser::setServerRoot( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"root [path to root directory]\""));
	else if (_currSb->getDirectiveExists(ServerBlock::SERVER_ROOT) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	_currSb->setRoot(args[0]);
	_currSb->setDirectiveExists(ServerBlock::SERVER_ROOT);
}

void	ConfigParser::setErrorPage( void ) {

	std::string 				directiveName = *_token;
	std::vector<std::string>	args = getArgs();

	if (args.size() != 2)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"error_page [status code] [filename]\""));
	else if (isAllDigit(args[0]) == false)
		throw (error(INVALID_STATUS_CODE, args[0], ""));
	else if (args[0].length() > 3)
		throw (error(INVALID_STATUS_CODE, args[0], ""));
	else if (toInt(args[0]) < 300 || toInt(args[0]) > 599)
			throw (error(INVALID_STATUS_CODE, args[0], ""));

	if (_currSb->getErrorPage().find(args[0]) == _currSb->getErrorPage().end()) /* if status code is already in map, don't overwrite it */
			_currSb->setErrorPage(std::make_pair(args[0], args[1]));

	_currSb->setDirectiveExists(ServerBlock::ERROR_PAGE);
}

void	ConfigParser::setClientMaxBodySize( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();
	int clientMax = 0;

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"client_max_body_size [size in megabytes]\""));
	else if (_currSb->getDirectiveExists(ServerBlock::CLIENT_MAX_BODY_SIZE) == true)
		throw (error(DUPLICATE_VALUE, directiveName, ""));

	if (*(args[0].rbegin()) == 'm' && args[0].size() == 1)
		throw (error(INVALID_VALUE, directiveName, "expected a digit"));
	else if (*(args[0].rbegin()) == 'm')
		args[0].pop_back();

	if (isAllDigit(args[0]) == false)
		throw (error(INVALID_VALUE, directiveName, "expected a digit"));
	if (args[0].size() > 2)
		throw(error(MAX_SIZE_EXCEEDED, directiveName, ""));
	clientMax = toInt(args[0]);
	if(clientMax > SERVER_MAX_BODY_SIZE)
		throw(error(MAX_SIZE_EXCEEDED, directiveName, ""));
	if (clientMax == 0)
		clientMax = 1000000;
	else
		clientMax *= 1000000;
	_currSb->setClientMaxBodySize(clientMax);
	_currSb->setDirectiveExists(ServerBlock::CLIENT_MAX_BODY_SIZE);
}

void	ConfigParser::setAllowedMethods( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() < 1 || args.size() > 3)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"allowed_methods [GET] and/or [POST] and/or [DELETE]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::ALLOWED_METHODS) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	for (vec_iterator it = args.begin(); it != args.end(); it++) {

		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			throw (error(INVALID_VALUE, directiveName, "expected [GET] and/or [POST] and/or [DELETE]"));
		else if (*it == "GET" && _currLb->getMethodExists(LocationBlock::GET) == false)
			_currLb->setMethodExists(LocationBlock::GET);
		else if (*it == "POST" && _currLb->getMethodExists(LocationBlock::POST) == false)
			_currLb->setMethodExists(LocationBlock::POST);
		else if (*it == "DELETE" && _currLb->getMethodExists(LocationBlock::DELETE) == false)
			_currLb->setMethodExists(LocationBlock::DELETE);
		else
			throw (error(DUPLICATE_VALUE, directiveName, ""));
		_currLb->setAllowedMethods(*it);
	}
	_currLb->setDirectiveExists(LocationBlock::ALLOWED_METHODS);
}

void	ConfigParser::setRedirect( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 2)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"return [STATUS_CODE] [URL]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::RETURN) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	if (isAllDigit(args[0]) == false)
		throw (error(INVALID_VALUE, directiveName, "expected a digit"));
	else if (args[0].length() > 3)
		throw (error(INVALID_STATUS_CODE_RD, args[0], ""));
	else if (!validRedirect(toInt(args[0])))
		throw (error(INVALID_STATUS_CODE_RD, args[0], ""));
	_currLb->setRedirect(args[0]);
	_currLb->setRedirect(args[1]);
	_currLb->setDirectiveExists(LocationBlock::RETURN);
}

bool	ConfigParser::validRedirect( int rdCode ) {

	std::vector<int> rdCodes;

	rdCodes.push_back(301);
	rdCodes.push_back(302);
	rdCodes.push_back(303);
	rdCodes.push_back(307);
	rdCodes.push_back(308);

	if (std::find(rdCodes.begin(), rdCodes.end(), rdCode) != rdCodes.end())
		return (true);
	return (false);
}

void	ConfigParser::setLocationRoot( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"root [path to root directory]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::ROOT) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	_currLb->setRoot(args[0]);
	_currLb->setDirectiveExists(LocationBlock::ROOT);
}

void	ConfigParser::setAlias( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"alias [path to alias directory]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::ALIAS) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	_currLb->setAlias(args[0]);
	_currLb->setDirectiveExists(LocationBlock::ALIAS);
}

void	ConfigParser::setAutoindex( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"autoindex [on]\" or \"autoindex [off]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::AUTOINDEX) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));
	else if (args[0] != "on" && args[0] != "off")
		throw (error(INVALID_AUTOINDEX_VALUE, args[0], ""));

	if (args[0] == "on")
		_currLb->setAutoindex(true);

	_currLb->setDirectiveExists(LocationBlock::AUTOINDEX);
}

void	ConfigParser::setDefaultFile( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() == 0)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"default_file [index.html] [index.htm] [...]\""));

	for (vec_iterator it = args.begin(); it != args.end(); it++)
		_currLb->setDefaultFile(*it);

	_currLb->setDirectiveExists(LocationBlock::DEFAULT_FILE);
}

void	ConfigParser::setCgi( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 2)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"cgi [file extention] [path to executable]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::CGI) == true)
		throw (error(DUPLICATE_VALUE, directiveName, ""));

	if (args[0][0] != '.' || args[0] != ".py")
		throw (error(INVALID_CGI_EXTENSION, "", ""));

	_currLb->setCgi(args[0]);
	_currLb->setCgi(args[1]);
	_currLb->setDirectiveExists(LocationBlock::CGI);
}

void	ConfigParser::setFileUpload( void ) {

	std::string directiveName = *_token;
	std::vector<std::string> args = getArgs();

	if (args.size() != 1)
		throw (error(INVALID_NUM_OF_ARGS, directiveName, "\"file_upload [path to upload location]\""));
	else if (_currLb->getDirectiveExists(LocationBlock::FILE_UPLOAD) == true)
		throw (error(DUPLICATE_DIRECTIVE, directiveName, ""));

	_currLb->setFileUpload(args[0]);
	_currLb->setDirectiveExists(LocationBlock::FILE_UPLOAD);
}

void	ConfigParser::bracketCheck( void ) {

	int	openingBracket = 0, closingBracket = 0;

	for (vec_iterator it = _tokens.begin(); it != _tokens.end(); it++) {

		if (*it == "{")
			openingBracket++;
		else if (*it == "}")
			closingBracket++;
	}
	if (openingBracket != closingBracket)
		throw (error(MISSING_CLOSING_BRACKET, "", ""));
}

std::string	ConfigParser::error( int type, std::string str, std::string usage ) {

	switch (type) {

		case INVALID_AMOUNT_OF_ARGS:
			return ("webserv: invalid amount of arguments\nUSAGE: " + usage);
		case INVALID_CONF_EXTENSION:
			return ("webserv: invalid file: \"[filename].conf expected\"");
		case OPEN_FAILED:
			return ("webserv: \"" + str + "\" failed to open");
		case EMPTY_FILE:
			return ("webserv: \"" + str + "\" is empty");
		case NO_SERVER_BLOCK_FOUND:
			return ("webserv: no server block found in \"" + str + "\"");
		case GETLINE_FAILED:
			return ("webserv: getline() failed");
		case SERVER_BLOCK_EXPECTED:
			return ("webserv: unknown directive \"" + str + "\", server block expected at line " + toString(_lineCount));
		case MISSING_OPENING_BRACKET:
			return ("webserv: directive \"" + str + "\" has no opening \"{\" at line " + toString(_lineCount));
		case DIRECTIVE_NOT_ALLOWED_HERE:
			return ("webserv: \"" + str + "\" directive is not allowed here at line " + toString(_lineCount));
		case UNKNOWN_DIRECTIVE:
			return ("webserv: unknown directive \"" + str + "\" at line " + toString(_lineCount));
		case INVALID_NUM_OF_ARGS:
			return ("webserv: invalid number of arguments in \"" + str + "\" directive at line " + toString(_lineCount) + "\n" + "USAGE: " + usage);
		case INVALID_VALUE:
			return ("webserv: \"" + str + "\" directive invalid value: " + usage);
		case DUPLICATE_VALUE:
			return ("webserv: duplicate value \"" + str + "\" directive");
		case DUPLICATE_DIRECTIVE:
			return ("webserv: duplicate directive \"" + str + "\" at line " + toString(_lineCount));
		case INVALID_AUTOINDEX_VALUE:
			return ("webserv: invalid value \"" + str + "\" in \"autoindex\" directive, it must be \"on\" or \"off\" at line " + toString(_lineCount));
		case MISSING_CLOSING_BRACKET:
			return ("webserv: missing \"}\" at line " + toString(_lineCount));
		case UNEXPECTED_EOF:
			return ("webserv: unexpected end of file");
		case UNEXPECTED_CHAR:
			return ("webserv: unexpected \"" + str + "\" at line: " + toString(_lineCount));
		case INVALID_PORT:
			return ("webserv: invalid port number in \"" + str + "\" directive at line: " + toString(_lineCount));
		case MAX_SIZE_EXCEEDED:
			return ("webserv: client_max_body_size exceeds allowed server maximum of " + toString(SERVER_MAX_BODY_SIZE) + "m at line: " + toString(_lineCount));
		case INVALID_STATUS_CODE:
			return ("webserv: directive error_page: value \"" + str + "\" must be between 300 and 599 at line: " + toString(_lineCount));
		case INVALID_STATUS_CODE_RD:
			return ("webserv: directive redirect: value \"" + str + "\" must be either [301] [302] [303] [307] [308] at line: " + toString(_lineCount));
		case INVALID_CGI_EXTENSION:
			return ("webserv: invalid file extension: \".py\" expected at line " + toString(_lineCount));
	}
	return ("");
}

/***************************
* private helper functions *
***************************/

bool	ConfigParser::emptyLine( std::string line ) {

	if (line.size() == 0)
		return (true);
	for	(size_t i = 0; i < line.size(); i++) {
	
		if (std::isspace(line[i]) == 0)
			return (false);
	}
	return (true);
}

bool	ConfigParser::specialChar( std::string token ) {

	if (token.size() == 0)
		return (0);
	else if (token.find(';') != std::string::npos ||
	token.find('{') != std::string::npos ||
	token.find('}') != std::string::npos ||
	token.find('#') != std::string::npos)
		return (1);

	return (0);
}

bool	ConfigParser::isAllDigit( std::string str ) {

	for (size_t i = 0; i < str.size(); i++) {

		if (!isdigit(str[i]))
			return (false);
	}
	return (true);
}

/**************************
* private debug functions *
**************************/

void	ConfigParser::printServerBlock( void ) {

	std::cout << std::endl;
	for (size_t i = 0; i < _serverBlock.size(); i++) {

		std::cout << GREEN << "SERVER BLOCK " << i << RESET << std::endl;

		std::cout << "[server] " << "[listen]\t" << _serverBlock[i].getListen() << std::endl;
		std::cout << "[server] " << "[host]\t\t" << _serverBlock[i].getHost() << std::endl;
		std::cout << "[server] " << "[port]\t\t" << _serverBlock[i].getPort() << std::endl;		
		std::cout << "[server] " << "[root]\t\t" << _serverBlock[i].getRoot() << std::endl;		
		std::cout << "[server] " << "[serverName]\t";
		for (vec_c_iterator it = _serverBlock[i].getServerName().begin(); it != _serverBlock[i].getServerName().end(); ++it)
			std::cout << *it <<  " ";
		std::cout << std::endl << "[server] " << "[errorPage] \t";
		for (map_c_iterator it = _serverBlock[i].getErrorPage().begin(); it != _serverBlock[i].getErrorPage().end(); ++it)
			std::cout << "[" << it->first << " => " << it->second << "]" << " ";
		std::cout << std::endl;
		std::cout << "[server] " << "[maxSize]\t" << _serverBlock[i].getClientMaxBodySize() << std::endl;
		std::cout << std::endl;

		for (size_t j = 0; j < _serverBlock[i].getLocation().size(); j++) {

			std::cout << std::endl;
			std::cout << GREEN << "	LOCATION BLOCK " << j << RESET << std::endl;

			std::cout << "\t[location] " << "[path]\t\t" << _serverBlock[i].getLocation()[j].getPath() << std::endl;
			std::cout << "\t[location] " << "[methods]\t\t";
			for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getAllowedMethods().begin(); it != _serverBlock[i].getLocation()[j].getAllowedMethods().end(); it++)
				std::cout << *it << " ";
			std::cout << std::endl;
			std::cout << "\t[location] " << "[root]\t\t" << _serverBlock[i].getLocation()[j].getRoot() << std::endl;
			std::cout << "\t[location] " << "[alias]\t\t" << _serverBlock[i].getLocation()[j].getAlias() << std::endl;
			std::cout << "\t[location] " << "[autoindex]\t\t" << _serverBlock[i].getLocation()[j].getAutoindex() << std::endl;
			std::cout << "\t[location] " << "[cgi]\t\t";
			for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getCgi().begin(); it != _serverBlock[i].getLocation()[j].getCgi().end(); it++)
				std::cout << *it << " ";
			std::cout << std::endl;
			std::cout << "\t[location] " << "[return]\t\t";
			for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getRedirect().begin(); it != _serverBlock[i].getLocation()[j].getRedirect().end(); it++)
				std::cout << *it << " ";	
			std::cout << std::endl;
			std::cout << "\t[location] " << "[default_file]\t";
			for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getDefaultFile().begin(); it != _serverBlock[i].getLocation()[j].getDefaultFile().end(); it++)
				std::cout << *it << " ";	
			std::cout << std::endl;
			std::cout << "\t[location] " << "[file_upload]\t" << _serverBlock[i].getLocation()[j].getFileUpload() << std::endl;
		}
		std::cout << std::endl;
	}
}

void	ConfigParser::printServerBlock( size_t i, size_t j ) {

	std::cout << std::endl;
	std::cout << GREEN << "SERVER BLOCK " << i << RESET << std::endl;

	std::cout << "[server] " << "[listen]\t" << _serverBlock[i].getListen() << std::endl;
	std::cout << "[server] " << "[host]\t\t" << _serverBlock[i].getHost() << std::endl;
	std::cout << "[server] " << "[port]\t\t" << _serverBlock[i].getPort() << std::endl;		
	std::cout << "[server] " << "[root]\t\t" << _serverBlock[i].getRoot() << std::endl;		
	std::cout << "[server] " << "[serverName]\t";
	for (vec_c_iterator it = _serverBlock[i].getServerName().begin(); it != _serverBlock[i].getServerName().end(); ++it)
		std::cout << *it <<  " ";
	std::cout << std::endl << "[server] " << "[errorPage] \t";
	for (map_c_iterator it = _serverBlock[i].getErrorPage().begin(); it != _serverBlock[i].getErrorPage().end(); ++it)
		std::cout << "[" << it->first << " => " << it->second << "]" << " ";
	std::cout << std::endl;
	std::cout << "[server] " << "[maxSize]\t" << _serverBlock[i].getClientMaxBodySize() << std::endl;
	std::cout << std::endl;

	if ( _serverBlock[i].getLocationExists() == true ) {

		std::cout << GREEN << "	LOCATION BLOCK " << j << RESET << std::endl;

		std::cout << "\t[location] " << "[path]\t\t" << _serverBlock[i].getLocation()[j].getPath() << std::endl;
		std::cout << "\t[location] " << "[methods]\t\t";
		for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getAllowedMethods().begin(); it != _serverBlock[i].getLocation()[j].getAllowedMethods().end(); it++)
			std::cout << *it << " ";
		std::cout << std::endl;
		std::cout << "\t[location] " << "[root]\t\t" << _serverBlock[i].getLocation()[j].getRoot() << std::endl;
		std::cout << "\t[location] " << "[alias]\t\t" << _serverBlock[i].getLocation()[j].getAlias() << std::endl;
		std::cout << "\t[location] " << "[autoindex]\t\t" << _serverBlock[i].getLocation()[j].getAutoindex() << std::endl;
		std::cout << "\t[location] " << "[cgi]\t\t";
		for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getCgi().begin(); it != _serverBlock[i].getLocation()[j].getCgi().end(); it++)
			std::cout << *it << " ";
		std::cout << std::endl;
		std::cout << "\t[location] " << "[return]\t\t";
		for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getRedirect().begin(); it != _serverBlock[i].getLocation()[j].getRedirect().end(); it++)
			std::cout << *it << " ";	
		std::cout << std::endl;
		std::cout << "\t[location] " << "[default_file]\t";
		for (vec_c_iterator it = _serverBlock[i].getLocation()[j].getDefaultFile().begin(); it != _serverBlock[i].getLocation()[j].getDefaultFile().end(); it++)
			std::cout << *it << " ";	
		std::cout << std::endl;
		std::cout << "\t[location] " << "[file_upload]\t" << _serverBlock[i].getLocation()[j].getFileUpload() << std::endl;
	}
	std::cout << std::endl;
}

void	ConfigParser::printTokens( void ) {

	for (vec_iterator it = _tokens.begin(); it != _tokens.end(); it++) {

		if (*it != "\n")
			std::cout << "[" << *it << "] ";
	}
	std::cout << std::endl;
}
