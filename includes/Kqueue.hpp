#pragma once

/***********
* includes *
***********/

#include <vector>
#include <map>

/**********
* defines *
**********/

#define NOT_FOUND -1
#define MAX_TIME 30
#define TIMER 1000
#define TIMER_EVENT 1
#define ERROR -1

/***********************
* forward declarations *
***********************/

class ConfigParser;
class Connection;
class ListeningSocket;

/********
* class *
*********/

class Kqueue {

	/***********
	* typedefs *
	***********/

	typedef std::vector<ListeningSocket*>	ls_vector;
	std::map<int, Connection*>::iterator 	map_it;

public:

	enum type {

		EVENT_FD,
		READ_PIPE_FD,
		WRITE_PIPE_FD
	};

	/***************************
	* constructor + destructor *
	***************************/

	Kqueue( ConfigParser *conf );
	~Kqueue( void );

	/*******************
	* public functions *
	*******************/

	int	const & getFd( void ) const;
	void		setTimerEvent( void );

	int		locateSocket( int eventFd );
	void	acceptConnection( int eventFd );
	void	evEof( int eventFd );
	void	evError( int eventFd );
	void	evRead( int eventFd );
	void	evWrite( int eventFd );
	bool	timeout( void );

	/*************
	* exceptions *
	**************/

	class KqueueCreationError : public std::exception {

		public:

			const char*	what() const throw();
	};

	class KeventPollingError : public std::exception {

		public:

			const char*	what() const throw();
	};

	class KeventSetError : public std::exception {

		public:

			const char*	what() const throw();
	};

	class CloseFailed : public std::exception {

		public:

			const char*	what() const throw();
	};

private:

	/********************
	* private variables *
	********************/

	ConfigParser				*_conf;
	const int					_fd;
	ls_vector					_lSockets;
	int							_writePipeEventFd;
	int							_readPipeEventFd;
	std::map<int, Connection*>	_connection;

	/********************
	* private functions *
	********************/

	void	setListeningSockets( void );
	void	registerReadKevent( int eventFd );
	void	disableReadKevent( int eventFd );
	void	registerWriteKevent( int eventFd );
	int		checkFdType( int eventFd );
	int		findConnectionFd( int pipeFd);
	void	cgi( int fd, int fdType );
	void	removeConnection( int eventFd );
};
