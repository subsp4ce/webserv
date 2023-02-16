#pragma once

/***********
* includes *
***********/

#include <string>
#include <vector>

/***********************
* forward declarations *
***********************/

class ServerBlock;
class LocationBlock;
class Request;
class AResponse;
class ConfigParser;
class Cgi;

/*********
* define *
*********/

#define ERROR -1

struct Data {

	ServerBlock		*_serverBlock;
	LocationBlock	*_locationBlock;
	std::string		_root;
	std::string		_path;
	std::string		_fileExtention;
	int				_statusCode;
};

struct State {

	enum type {

		ACCEPTED,
		CHUNKED,
		ERR,
		DISCONNECTED,
		RECEIVE,
		REDIRECT,
		CGI,
		TIMER_FAILED,
		REMOVE,
		DONE
	};
};

struct ResponseType {

	enum type {

		NONE,
		CGI,
		ERR,
		AUTOINDEX,
		REGULAR,
		DELETE
	};
};

/********
* class *
*********/

class Connection {

	friend class AResponse;
	friend class BlockAlgorithm;
	friend class Cgi;
	friend class CgiResponse;
	friend class DeleteResource;
	friend class DirectoryListing;
	friend class ErrorResponse;
	friend class GenericResponse;
	friend class Request;
	friend class ResolvePath;

public:

	/***************************
	* constructor + destructor *
	***************************/

	Connection( ConfigParser *conf, const int fd, const int port );
	virtual ~Connection( void );

	/*******************
	* public functions *
	*******************/

	int					getFd( void );
	int					getState( void );
	long int const & 	getStartTime( void ) const;
	int					getResponseType( void );
	Cgi *				getCgiData( void );
	Request	*			getRequest( void ) const;
	void				setState( int state );
	void				setIsReadPipeFd( const bool isReadPipeFd );
	void				setIsWritePipeFd( const bool isWritePipeFd );
	
	void	createRequest( void );
	void	keepReceiving( void );
	void	createResponse( void );
	void	chunkedResponse( void );
	void	createCgi( void );
	void	cgiHandler( void );
	int 	startTimer( void );
	int		errorLog( int statusCode, std::string msg, bool response );

private:

	/********************
	* private variables *
	********************/

	ConfigParser	*_conf;
	const int 		_fd;
	const int		_port;
	int				_state;
	Request 		*_request;
	AResponse 		*_response;
	Data			_data;
	int				_responseType;
	bool			_error;
	Cgi				*_cgiData;
	bool			_blockMatch;
	bool			_isReadPipeFd;
	bool			_isWritePipeFd;
	long int 		_startTime;

	/********************
	* private functions *
	********************/

	void	matchBlocks( void );
	void	resolvePath( void );
};
