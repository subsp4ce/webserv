#pragma once

/***********
* includes *
***********/

#include "AResponse.hpp"
#include <dirent.h> /* DIR */

/***********************
* forward declaration *
***********************/

class Connection;

/*********
* define *
*********/

#define ERROR -1
#define EXIT 1

/********
* class *
*********/

class DirectoryListing : public AResponse {

public:

	/***************************
	* constructor + destructor *
	***************************/

	DirectoryListing( Connection *connection );
	virtual ~DirectoryListing( void );

private:

	/********************
	* private variables *
	********************/

	Connection	*_connection;
	DIR			*_dir;

	/********************
	* private functions *
	********************/

	int			openDir( void );
	int			generateBody( void );
	std::string	setTarget( void );
	std::string setPrintPath( void );
};
