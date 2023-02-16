#pragma once

/***********
* includes *
***********/

#include <list>
#include <map>
#include <string>

/**********
* defines *
**********/

#define CGI_PY "python"
#define CGI_PY3 "python3"
#define ERROR -1

/**********************
* forward declaration *
**********************/

class Connection;

/********
* env *
*********/

extern char **environ;

/********
* class *
*********/

class Cgi { 

public:

	enum state {

		CREATED,
		WRITE,
		READING,
		WRITING,
		WAITING,
		DONE
	};

	/***********
	* typedefs *
	***********/

	typedef	std::list<std::string>				s_list;
	typedef	std::map<std::string, std::string>	s_map;

	/***************************
	* constructor + destructor *
	***************************/

	Cgi( Connection *connection );
	virtual ~Cgi( void );

	/********************
	* public functions *
	********************/

	std::string	const	getBody( void ) const;
	int 				getCgiState( void ) const;
	int					getPid( void ) const;
	int 				getServerToCgiWriteFd( void ) const;
	int					getCgiToServerReadFd( void ) const;
	bool				getPipeEventsRegistered( void ) const;
	bool				getServerToCgiClosed( void ) const;
	bool				getCgiToServerClosed( void ) const;
	void				setPipeEventsRegistered( bool registered );
	int					waitForChild( void );
	int					launchCgi( void );
	int 				writeToCgi( void );
	int					readFromCgi( void );
	int					cgiWait( void );

private:

	/********************
	* private variables *
	********************/

	Connection	*_connection;
	int			_cgiState;
	std::string	_scriptBin;
	std::string	_pathInfo;
	std::string	_queryString;
	std::string	_cgiBin;
	std::string	_uploadDir;
	char		**_argv;
	pid_t		_pid;
	int			_cgiToServer[2];
	int			_serverToCgi[2];
	int			_stat;
	std::string	_body;
	std::string	_requestBody;
	bool		_pipeEventsRegistered;
	bool		_serverToCgiClosed;
	bool		_cgiToServerClosed;
	int			_waitResult;
	s_map		_headerFields;

	/********************
	* private functions *
	********************/

	int			dupClose( int readFd, int writeFd );
	int			executeCgi( void );
	int			makeNonBlocking( int readFd, int writeFd );
	int 		closeFds( int readFd, int writeFd );
	int 		closeFd( int fd, bool *isClosed );
	void		prepArgv( void );
	int			postRequest( void );
	int			handleUploadDir( void );
	void		setUploadDir( void );
	int			checkUploadDir( void );
	int			checkCgiBin( void );
	int			parseCgiUri( void );
	s_list		getComponents( void );
	void		parseComponents( s_list components );
	int			checkScriptPath( void );
	int			setEnvVars( void );
};
