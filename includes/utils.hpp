#pragma once

/***********
* includes *
***********/

#include <sstream> /* std::stringstream */
#include <map>

/**********
* defines *
**********/

#define WHITESPACE " \n\r\t\f\v"
#define EXIT 1

/*********
* colors *
*********/

#define RESET		"\033[0m"
#define YELLOW		"\033[33m"
#define BLUE		"\033[34m"
#define MAGENTA		"\033[35m"
#define CYAN		"\033[36m"
#define GREEN		"\033[0;32m"
#define RED			"\033[0;31m"
#define BOLDBLUE	"\033[1m\033[34m"
#define BOLDMAGENTA	"\033[1m\033[35m"
#define BOLDCYAN	"\033[1m\033[36m"

/************
* templates *
************/

template<typename T>
std::string	toString(T val) {

	std::stringstream	s;

	s << val;
	return (s.str());
}

template<typename T>
int toInt(T val) {

	std::stringstream	s;
	long int	num;

	s << val;
	s >> num;
	return (num);
}

/***********
* typedefs *
***********/

typedef std::map<std::string, char>	sc_map;
typedef	std::map<int, std::string>  is_map;

/************
* functions *
************/

std::string	percentDecoding( std::string str );
void		initCharDecoding( sc_map &charEncoding );
void		signalHandler(int signum);
void		setSignals(void);
std::string	getStatus( int statusCode );
void		errorLogNoResponse( int statusCode, std::string error );
bool		isAllDigit( std::string str);
std::string	ltrim( const std::string &s );
std::string	rtrim( const std::string &s );
std::string	trim( const std::string &s );
bool		endsWith( std::string arg, std::string extension );
