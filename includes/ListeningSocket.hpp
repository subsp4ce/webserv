#pragma once

/***********
* includes *
***********/

#include "netinet/in.h" /* struct sockaddr_in */
#include <string>

/*********
* define *
*********/

#define ERROR -1

/********
* class *
*********/

class ListeningSocket {

enum error {

	BIND,
	FCNTL,
	LISTEN,
	SETSOCKOPT,
	SOCKET
};

public:

	/***************************
	* constructor + destructor *
	***************************/

	ListeningSocket( int port );
	~ListeningSocket( void );

	/*******************
	* public functions *
	*******************/

	struct sockaddr_in	const & getAddress( void ) const;
	int					const & getFd( void ) const;
	int					const & getPort( void ) const;
	int					const & getAddrLen( void ) const;

private:

	/********************
	* private variables *
	********************/

	struct sockaddr_in	_address;
	int					_fd;
	const int			_port;
	int					_addrLen;
	const int 			_enable;

	/********************
	* private functions *
	********************/

	void		init_address( void );
	std::string	error( int type, int port );
};
