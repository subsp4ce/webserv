#pragma once

/***********
* includes *
***********/

#include <map>
#include <string>

/**********************
* forward declaration *
**********************/

class Connection;

/**********
* defines *
**********/

#define SP " "
#define CRLF "\r\n"
#define CHUNK_SIZE 1024
#define ERROR -1

/***********
* typedefs *
***********/

typedef	std::map<int, std::string>			is_map;
typedef	std::map<std::string, std::string>	s_map;

/********
* class *
*********/

class AResponse {

	/***************
	* friend class *
	****************/

	friend class Connection;

public:

	/***************************
	* constructor + destructor *
	***************************/

	AResponse( Connection *connection );
	virtual ~AResponse( void );

protected:

	/**********************
	* protected variables *
	**********************/

	Connection	*_connection;
	std::string	_serverName;
	std::string	_headerFields;
	std::string	_body;

	/**********************
	* protected functions *
	**********************/

	virtual	std::string	generateStatusLine( void );
	void				generateResponse( void );
	void				sendResponse( void );
	std::string			getTime( void );

private:

	/********************
	* private variables *
	********************/

	is_map		_statusText;
	s_map 		_mimeTypes;
	std::string	_httpVersion;
	std::string	_statusLine;
	std::string	_response;

	/********************************
	* private pure virtual function *
	********************************/

	virtual int	generateBody( void ) = 0;

	/********************
	* private functions *
	********************/

	is_map				initStatusText( void );
	s_map				initMimeTypes( void );
	virtual void		generateHeaderFields( void );
	void				printResponse( void );
};
