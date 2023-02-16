#pragma once

/***********
* includes *
***********/

#include "AResponse.hpp"
#include <string>
#include <vector>
#include <map>

/**********************
* forward declaration *
**********************/

class Connection;

/********
* class *
*********/

class ErrorResponse : public AResponse {

	/***********
	* typedefs *
	***********/

	typedef	std::map<std::string, std::string>			s_map;
	typedef	std::vector<std::string>::const_iterator	vec_c_iterator;

public:

	/***************************
	* constructor + destructor *
	***************************/

	ErrorResponse( Connection *connection );
	virtual ~ErrorResponse( void );

private:

	/*******************
	* private variable *
	*******************/

	s_map	_errorPage;

	/********************
	* private functions *
	********************/

	int			generateBody( void );
	std::string defaultErrorPage( void );
	std::string customErrorPage( void );
	void		generateHeaderFields( void );
	int			firstDigit( void ) const ;
	std::string	getAllow( void );
};
