#pragma once

/***********
* includes *
***********/

#include <vector>
#include <map>
#include <string>

/**********************
* forward declarations *
**********************/

class ServerBlock;
class LocationBlock;

/**********
* defines *
**********/

#define SERVER_MAX_BODY_SIZE 8

/********
* class *
*********/

class ConfigParser {

	/***********
	* typedefs *
	***********/

	typedef	std::vector<std::string>::iterator					vec_iterator;
	typedef	std::vector<std::string>::const_iterator			vec_c_iterator;
	typedef	std::map<std::string, std::string>::const_iterator	map_c_iterator;
	typedef std::vector<std::string>							s_vector; /* string vector */
	typedef	std::map<std::string, std::string>					s_map; /* string => string map */

	enum error {
		
		INVALID_AMOUNT_OF_ARGS,
		INVALID_CONF_EXTENSION,
		OPEN_FAILED,
		EMPTY_FILE,
		NO_SERVER_BLOCK_FOUND,
		GETLINE_FAILED,
		SERVER_BLOCK_EXPECTED,
		MISSING_OPENING_BRACKET,
		DIRECTIVE_NOT_ALLOWED_HERE,
		UNKNOWN_DIRECTIVE,
		INVALID_NUM_OF_ARGS,
		INVALID_VALUE,
		DUPLICATE_VALUE,
		DUPLICATE_DIRECTIVE,
		INVALID_AUTOINDEX_VALUE,
		MISSING_CLOSING_BRACKET,
		UNEXPECTED_EOF,
		UNEXPECTED_CHAR,
		INVALID_PORT,
		MAX_SIZE_EXCEEDED,
		INVALID_STATUS_CODE,
		INVALID_CGI_EXTENSION,
		INVALID_STATUS_CODE_RD
	};

public:

	/**********
	* typedef *
	***********/

	typedef std::vector<ServerBlock>	sb_vector; /* ServerBlock vector */

	/***************************
	* constructor + destructor *
	***************************/

	ConfigParser( int argc, char **argv );
	~ConfigParser( void );

	/*******************
	* public functions *
	*******************/

	sb_vector &			getServerBlock();
	std::string	const &	getFileName( void ) const;

private:

	/********************
	* private variables *
	********************/

	std::string		_fileName;
	s_vector		_tokens;
	sb_vector		_serverBlock;
	int				_lineCount;
	bool			_inServerBlock;
	bool			_inLocationBlock;
	vec_iterator	_token;
	int				_locationIdx;
	int				_serverIdx;
	bool			_server;
	bool			_location;
	ServerBlock* 	_currSb;
	LocationBlock* 	_currLb;

	/********************
	* private functions *
	********************/

	void		readFile( void );
	void		makeTokens( std::string line );
	bool		handleSpecialChar( std::string token );
	void		setDirective( void );
	void		newServerBlock( void );
	void		newLocationBlock( void );
	void		closeBlock( void );
	void		serverDirective( void );
	void		locationDirective( void );
	s_vector	getArgs( void );
	void		setListen( void );
	void		splitListen( size_t pos, std::string directiveName, s_vector args );
	void		setServerName( void );
	void		setServerRoot( void );
	void		setErrorPage( void );
	void		setClientMaxBodySize( void );
	void		setAllowedMethods( void );
	void		setRedirect( void );
	bool		validRedirect(int rdCode );
	void		setLocationRoot( void );
	void		setAlias( void );
	void		setAutoindex( void );
	void		setDefaultFile( void );
	void		setCgi( void );
	void		setFileUpload( void );
	void		bracketCheck( void );
	std::string	error( int type, std::string str, std::string usage );

	/***************************
	* private helper functions *
	***************************/

	bool		emptyLine( std::string line );
	bool		specialChar( std::string token );
	bool		isAllDigit( std::string str );

	/**************************
	* private debug functions *
	**************************/

	void		printTokens( void );
	void		printServerBlock( void );
	void		printServerBlock( size_t i, size_t j );
};
