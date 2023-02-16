#pragma once

/***********
* includes *
***********/

#include <vector>
#include <string>
#include <sys/stat.h> /* struct stat */

/**********************
* forward declaration *
**********************/

class Connection;

/**********
* defines *
**********/

#define CGI_EXT "py"

/********
* class *
*********/

class ResolvePath {

	/***********
	* typedefs *
	***********/

	typedef	std::vector<std::string>::const_iterator	vec_c_iterator;

public:

	/***************************
	* constructor + destructor *
	***************************/

	ResolvePath( Connection *connection );
	~ResolvePath( void );

private:

	/********************
	* private variables *
	********************/

	Connection	*_connection;
	std::string	_root;
	std::string	_path;
	bool		_alias;
	struct stat	_fileStat;

	/********************
	* private functions *
	********************/

	int		mapRequest( void );
	void	setRoot( void );
	void	setPath( void );
	void	setFileExtention( void );
	void	checkCgiDirective( void );
	void	checkCgiExt( void );
	void	resolvePath( void );
	bool	searchFile( void );
	bool	setDefaultFile( void );
	bool	setIndexFile( void );
	void	checkAutoindex( void );
	bool	isDirectory( void );
	bool	isRegularFile( void );
};
