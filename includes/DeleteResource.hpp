#pragma once

/***********
* includes *
***********/

#include "AResponse.hpp"

/***********************
* forward declaration *
***********************/

class Connection;

/*********
* define *
*********/

#define ERROR -1

/********
* class *
*********/

class DeleteResource : public AResponse {

public:

	/***************************
	* constructor + destructor *
	***************************/

	DeleteResource( Connection *connection );
	virtual	~DeleteResource( void );

private:

	/********************
	* private functions *
	********************/

	int		deleteResource( void );
	void	generateHeaderFields( void );
	int		generateBody( void );
};
