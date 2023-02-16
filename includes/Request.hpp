#pragma once

/***********
* includes *
***********/

#include <map>
#include <string>
#include <vector>

/**********************
* forward declaration *
**********************/

class Connection;

/**********
* defines *
**********/

#define SERVER_MAX_BODY_SIZE 8
#define MAX_HEADER_SIZE 8000
#define MAX_URI_SIZE 2048
#define BUF_SIZE 1024
#define ERROR -1
#define CRLF "\r\n"

/********
* enums *
*********/

struct RequestLine {

	enum Type {

		METHOD,
		REQUEST_TARGET,
		PROTOCOL
	};
};

struct Method {

	enum Type {

		GET,
		POST,
		DELETE
	};
};

/********
* class *
*********/

class Request {

	/***************
	* friend class *
	****************/

	friend class Connection;

	/***********
	* typedefs *
	***********/

	typedef	std::map<std::string, std::string>				s_map;
	typedef	std::map<int, std::string>						is_map;
	typedef std::map<std::string, std::string>::iterator	s_map_it;
	typedef std::vector<std::string>						s_vector;

public:

	/***************************
	* constructor + destructor *
	***************************/

	Request( Connection *connection );
	~Request( void );

	/**********
	* getters *
	***********/

	int				const & getRequestMethod( void ) const;
	std::string		&		getRequestTarget( void );
	std::string 	const & getQueryString( void ) const;
	s_map			const & getHeaderFields( void ) const;
	std::string		&	 	getHeaderValue( std::string key );
	std::string 	const & getHost( void ) const;
	std::string		const & getBody( void ) const;
	std::string		const & getRedirectLocation( void ) const;
	size_t			const & getContentLength( void ) const;
	std::string		& 		getMethodToString( int method ) ;

	/**********
	* setters *
	***********/

	void			setRedirectLocation( std::string redirectLocation );

	/******************
	* public function *
	******************/

	void			printRequest( void );

private:

	/********************
	* private variables *
	********************/

	Connection			*_connection;
	bool				_requestLineParsed;
	const std::string	_httpProtocol;
	std::string			_request;
	std::string			_requestLine;
	std::string			_header;
	bool				_headerParsed;
  	size_t				_contentLength;
	size_t				_headerLength;
	std::string			_line;
	s_vector			_requestLineTokens; /* [method] [requestTarget] [protocol] */
	int					_requestMethod;
	std::string			_requestTarget;
	std::string 		_queryString;
	s_map				_requestHeaderFields;
	std::string 		_host;
	std::string 		_body;
	std::string			_redirectLocation;
	is_map				_methodToString;

	/********************
	* private functions *
	********************/

	int				readRequest( void );
	void 			parseBuf( char *buf, int bytesRead );
	int				parseRequest( void );
	int				parseHeader( void );
	void			prepHeader( void );
	int				parseRequestLine( std::string buf );
	int				splitRequestLine( std::string line );
	int				setRequestMethod( void );
	int				setRequestTarget( void );
	int				setHttpProtocol( void );
	void			setHeaderField( void );
	void			setHost( void );
	void			setContentLength( void );
	int				checkMaxHeaderSize( int len );
	int				checkBody( void );
	int				checkBodySize( void );
	void			updateState( void );

	/**************************
	* private debug functions *
	**************************/

	void			printParsedRequest( void );
	void			printRequestLineTokens( void );
	void			printHeaderFields( void );
	std::string		printState( int &state );
};
