/***********
* includes *
***********/

#include "../includes/ListeningSocket.hpp"
#include "../includes/utils.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h> /* close */

/***************************
* constructor + destructor *
***************************/

ListeningSocket::ListeningSocket( int port ) : _port(port), _enable(1) {

	init_address();
	_addrLen = sizeof(_address);
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == ERROR)
		throw(error(SOCKET, port));
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_enable, sizeof(_enable)) < 0)
		throw(error(SETSOCKOPT, port));
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw(error(BIND, port));
	if (listen(_fd, SOMAXCONN) < 0)
		throw(error(LISTEN, port));
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == ERROR)
		throw(error(FCNTL, port));
}

ListeningSocket::~ListeningSocket( void ) {

	close(_fd);
}

/*******************
* public functions *
*******************/

sockaddr_in		const &	ListeningSocket::getAddress( void ) const { return (_address); }
int				const & ListeningSocket::getFd( void ) const { return (_fd); }
int				const & ListeningSocket::getPort( void ) const {return (_port); }
int				const & ListeningSocket::getAddrLen ( void ) const { return (_addrLen); }

/********************
* private functions *
********************/

void	ListeningSocket::init_address( void ) {

	_address.sin_family = AF_INET;
	_address.sin_port = htons(this->_port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	std::memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));
}

std::string	ListeningSocket::error( int type, int port ) {

	close(_fd);
	switch (type) {

		case SOCKET:
			return ("socket failed on port " + toString(port));
		case SETSOCKOPT:
			return ("setsockopt(SO_REUSEADDR) failed on port " + toString(port));
		case BIND:
			return ("bind failed on port " + toString(port));
		case LISTEN:
			return ("listen failed on port " + toString(port));
		case FCNTL:
			return ("fcntl failed on port " + toString(port));	

	}
	return ("");
}
