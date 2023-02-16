#pragma once

/***********
* includes *
***********/

#include "AResponse.hpp"

/**********************
* forward declaration *
**********************/

class Connection;

/********
* class *
*********/

class CgiResponse : public AResponse {

public:

	/***************************
	* constructor + destructor *
	***************************/

	CgiResponse( Connection *connection );
	virtual	~CgiResponse( void );

private:

	/********************
	* private variables *
	********************/

	int			_contentLength;
	std::string	_cgiHeaderFields;

	/********************
	* private functions *
	********************/

	int		generateBody( void );
	void	generateHeaderFields( void );
};
