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

class GenericResponse : public AResponse {

public:

	/***************************
	* constructor + destructor *
	***************************/

	GenericResponse( Connection *connection );
	virtual ~GenericResponse( void );

private:

	/*******************
	* private function *
	*******************/

	int	generateBody( void );
};
