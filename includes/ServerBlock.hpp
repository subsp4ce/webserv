#pragma once

/***********
* includes *
***********/

#include "LocationBlock.hpp"
#include <map>
#include <vector>
#include <string>

/********
* class *
*********/

class ServerBlock {


	/***********
	* typedefs *
	***********/

	typedef	std::map<std::string, std::string>	s_map;
	typedef std::vector<std::string>			s_vector;
	typedef std::vector<LocationBlock>			lb_vector;

public:

	/********
	* enums *
	*********/

	enum directive {

		LISTEN,
		SERVER_NAME,
		SERVER_ROOT,
		ERROR_PAGE,
		CLIENT_MAX_BODY_SIZE
	};

	/***************************
	* constructor + destructor *
	***************************/

	ServerBlock( void );
	~ServerBlock( void );

	/**********
	* getters *
	***********/

	bool		const & getDirectiveExists( int directive ) const;
	std::string	const & getListen( void ) const;
	std::string	const & getHost( void ) const;
	int			const & getPort( void ) const;
	s_vector	const & getServerName( void ) const;
	std::string	const & getRoot( void ) const;
	s_map		const & getErrorPage( void ) const;
	int			const & getClientMaxBodySize( void ) const;
	lb_vector	& 		getLocation( void );
	int			const & getLocationIdx( void ) const;
	bool		const & getLocationExists( void );

	/**********
	* setters *
	***********/

	void		setDirectiveExists( int directive );
	void		setListen( std::string listen );
	void		setHost( std::string host );
	void		setPort( int port );
	void		setServerName( std::string serverName );
	void		setRoot( std::string root );
	void		setErrorPage( std::pair<std::string, std::string> pair );
	void		setClientMaxBodySize( int clientMaxBodySize );
	void		setLocation( void );
	void		setLocationIdx( int locationIdx );
	void		setlocationExists( bool locationExists );

private:

	/********************
	* private variables *
	********************/

	bool		_directiveExists[10];
	std::string	_listen;
	std::string	_host;
	int			_port;
	s_vector	_serverName;
	std::string	_root;
	s_map		_errorPage;
	int			_clientMaxBodySize;
	lb_vector	_location;
	int			_locationIdx;
	bool		_locationExists;
};
