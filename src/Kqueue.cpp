/***********
* includes *
***********/

#include "../includes/Kqueue.hpp"
#include "../includes/Cgi.hpp"
#include "../includes/ConfigParser.hpp"
#include "../includes/Connection.hpp"
#include "../includes/ListeningSocket.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/utils.hpp"
#include <sys/event.h> /* kqueue, kevent */
#include <unistd.h> /* close */
#include <signal.h> /* kill */
#include <fcntl.h>

/***************************
* constructor + destructor *
***************************/

Kqueue::Kqueue( ConfigParser *conf ) :
_conf(conf),
_fd(kqueue()),
_writePipeEventFd(0),
_readPipeEventFd(0) {

	if (_fd == ERROR)
		throw (Kqueue::KqueueCreationError());

	setListeningSockets();
	for (size_t i = 0; i < _lSockets.size(); i++)
		registerReadKevent(_lSockets[i]->getFd());

	setTimerEvent();
}

Kqueue::~Kqueue() {

	for (size_t i = 0; i < _lSockets.size(); i++)
		delete (_lSockets[i]);
	if (close(_fd) == ERROR)
		throw (Kqueue::CloseFailed());
}

/*******************
* public functions *
*******************/

int	const & Kqueue::getFd( void ) const { return (_fd); }

void	Kqueue::setTimerEvent( void ) {

	struct kevent kev;

	EV_SET(&kev, TIMER_EVENT, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, TIMER, NULL);
	if (kevent(_fd, &kev, 1, NULL, 0, NULL) == ERROR)
		throw (Kqueue::KeventSetError());
}

int	Kqueue::locateSocket( int eventFd ) {

	int len = _lSockets.size();

	for (int i = 0; i < len; i++) {

		if (_lSockets[i]->getFd() == eventFd)
			return (i);
	}
	return (NOT_FOUND);
}

void	Kqueue::acceptConnection( int eventFd ) {

	int i = locateSocket(eventFd);
	if (i == NOT_FOUND)
		errorLogNoResponse(500, "listening socket not found");
	else {

		int connSock = accept(_lSockets[i]->getFd(), (struct sockaddr*)&_lSockets[i]->getAddress(), (socklen_t*)&_lSockets[i]->getAddrLen()) ;
		if (connSock == ERROR)
			errorLogNoResponse(EXIT, "accept() failed while trying to create connection socket");
		else {

			if (fcntl(connSock, F_SETFL, O_NONBLOCK) == ERROR)
				errorLogNoResponse(EXIT, "fcntl() failed on connection socket");
			_connection[connSock] = new Connection(_conf, connSock, _lSockets[i]->getPort());
			if (_connection[connSock]->startTimer() == ERROR) {

				_connection[connSock]->createResponse();
				removeConnection(connSock);
			}
			registerReadKevent(connSock);
		}
	}
}

void	Kqueue::evEof( int eventFd ) {

	if (checkFdType(eventFd) == Kqueue::EVENT_FD)
		removeConnection(eventFd);
	else if (checkFdType(eventFd) == Kqueue::READ_PIPE_FD) {

		int connectionFd = findConnectionFd(eventFd);
		cgi(connectionFd, Kqueue::READ_PIPE_FD);
	}
}

void	Kqueue::evError( int eventFd ) {

	int connectionFd;

	if (checkFdType(eventFd) == Kqueue::EVENT_FD)
		removeConnection(eventFd);
	else if (checkFdType(eventFd) == Kqueue::READ_PIPE_FD) {

		connectionFd = findConnectionFd(eventFd);
		removeConnection(connectionFd);
	}
	else {

		connectionFd = findConnectionFd(eventFd);
		removeConnection(connectionFd);
	}
}

/* if Cgi class returns an error then Connection state is set to ERR and will be caught in evWrite */
void	Kqueue::evRead( int eventFd ) {

	if (checkFdType(eventFd) == Kqueue::READ_PIPE_FD) {

		int connectionFd = findConnectionFd(eventFd);
		cgi(connectionFd, Kqueue::READ_PIPE_FD);
		if (_connection[connectionFd]->getState() == State::REMOVE)
			removeConnection(connectionFd);
	}
	else if (checkFdType(eventFd) == Kqueue::EVENT_FD) {
		if (_connection[eventFd]->getState() == State::ACCEPTED)
			_connection[eventFd]->createRequest();
		else if (_connection[eventFd]->getState() == State::RECEIVE)
			_connection[eventFd]->keepReceiving();

		if (_connection[eventFd]->getState() == State::DISCONNECTED ||
			_connection[eventFd]->getState() == State::TIMER_FAILED ||
			_connection[eventFd]->getState() == State::REMOVE) /* protect recv() in Request */
			return (removeConnection(eventFd));
		else if (_connection[eventFd]->getState() == State::ERR ||
			_connection[eventFd]->getState() == State::REDIRECT) {

			disableReadKevent(eventFd);
			registerWriteKevent(eventFd);
		}
		else if (_connection[eventFd]->getState() == State::DONE) {
				
			if (_connection[eventFd]->getResponseType() == ResponseType::CGI) {

				_connection[eventFd]->setState(State::CGI);
				_connection[eventFd]->createCgi();
			}
			registerWriteKevent(eventFd);
		}
	}
}

void	Kqueue::evWrite( int eventFd ) {

	if (checkFdType(eventFd) == Kqueue::WRITE_PIPE_FD) {

		int connectionFd = findConnectionFd(eventFd);
		cgi(connectionFd, Kqueue::WRITE_PIPE_FD);
		if (_connection[connectionFd]->getState() == State::REMOVE) /* for write() failure in cgi */
			removeConnection(connectionFd);
	}
	else if (checkFdType(eventFd) == Kqueue::EVENT_FD) {

		if (_connection[eventFd]->getState() == State::ERR ||
			_connection[eventFd]->getState() == State::REDIRECT ||
			_connection[eventFd]->getState() == State::DONE)
			_connection[eventFd]->createResponse();
		else if (_connection[eventFd]->getState() == State::CHUNKED)
			_connection[eventFd]->chunkedResponse();
		else if (_connection[eventFd]->getState() == State::CGI) {

			_connection[eventFd]->cgiHandler();
			if (_connection[eventFd]->getCgiData()->getPipeEventsRegistered() == false) {

				registerWriteKevent(_connection[eventFd]->getCgiData()->getServerToCgiWriteFd());
				registerReadKevent(_connection[eventFd]->getCgiData()->getCgiToServerReadFd());
				_connection[eventFd]->getCgiData()->setPipeEventsRegistered(true);
			}
		}
		if (_connection[eventFd]->getState() == State::DONE ||
			_connection[eventFd]->getState() == State::REMOVE) /* protect send() and read() in Aresponse */
			removeConnection(eventFd);
	}
}

bool	Kqueue::timeout( void ) {

	long int	_currentTime = static_cast<long int>(std::time(NULL));

	if (_currentTime == ERROR)
		errorLogNoResponse(EXIT, "system function time() failed");

	for (map_it = _connection.begin(); map_it != _connection.end(); map_it++) {

			if (_currentTime - map_it->second->getStartTime() > MAX_TIME) {
				
				if (map_it->second->getRequest()) {

					if (map_it->second->getCgiData())
						map_it->second->errorLog(504, "request exceeded " + toString(MAX_TIME) + "s", true);
					else
						map_it->second->errorLog(408, "request exceeded " + toString(MAX_TIME) + "s", true);
					map_it->second->createResponse();
				}
				removeConnection(map_it->first);
				return (true);
			}
	}
	return (false);
}

/********************
* private functions *
********************/

void Kqueue::setListeningSockets( void ) {

	std::vector<int> ports;
	for (size_t i = 0; i < _conf->getServerBlock().size(); i++) {

		if (find(ports.begin(), ports.end(), _conf->getServerBlock()[i].getPort()) == ports.end()) {

			ports.push_back(_conf->getServerBlock()[i].getPort());
			_lSockets.push_back(new ListeningSocket(_conf->getServerBlock()[i].getPort()));
		}
	}
}

void	Kqueue::registerReadKevent( int eventFd ) {

	struct kevent kev;

	EV_SET(&kev, eventFd, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);
	if (kevent(_fd, &kev, 1, NULL, 0, NULL) == ERROR)
		throw (Kqueue::KeventSetError());
}

void	Kqueue::registerWriteKevent( int eventFd ) {

	struct kevent kev;

	EV_SET(&kev, eventFd, EVFILT_WRITE, EV_ADD | EV_ENABLE , 0, 0, NULL);
	if (kevent(_fd, &kev, 1, NULL, 0, NULL) == ERROR)
		throw (Kqueue::KeventSetError());
}

void	Kqueue::disableReadKevent( int eventFd ) {

	struct kevent kev;

	EV_SET(&kev, eventFd, EVFILT_READ, EV_DISABLE , 0, 0, NULL);
	if (kevent(_fd, &kev, 1, NULL, 0, NULL) == ERROR)
		throw (Kqueue::KeventSetError());
}

int		Kqueue::findConnectionFd( int pipeFd) {

	for (map_it = _connection.begin(); map_it != _connection.end(); map_it++) {

		if (map_it->second->getCgiData() != NULL) {

			if (map_it->second->getCgiData()->getServerToCgiWriteFd() == pipeFd)
				return (map_it->first);
			if (map_it->second->getCgiData()->getCgiToServerReadFd() == pipeFd)
				return (map_it->first);
		}
	}
	return (0);
}

int		Kqueue::checkFdType( int eventFd ) {

	if (_connection.find(eventFd) != _connection.end()) {
        return (Kqueue::EVENT_FD);
    }
	else {
		for (map_it = _connection.begin(); map_it != _connection.end(); map_it++) {
			if (map_it->second->getCgiData() != NULL) {

				if (map_it->second->getCgiData()->getServerToCgiWriteFd() == eventFd) {
					_writePipeEventFd = map_it->first;
					return (Kqueue::WRITE_PIPE_FD);
				}
				if (map_it->second->getCgiData()->getCgiToServerReadFd() == eventFd) {
					_readPipeEventFd = map_it->first;
					return (Kqueue::READ_PIPE_FD);
				}
			}
		}
	}
	return (ERROR);
}

void	Kqueue::cgi(int fd, int fdType ) {

	if (_connection[fd]->getState() == State::ERR) {
		_connection[fd]->createResponse();
		removeConnection(fd);
	}
	else {
		if (fdType == Kqueue::WRITE_PIPE_FD)
			_connection[fd]->setIsWritePipeFd(true);
		else
			_connection[fd]->setIsReadPipeFd(true);
		_connection[fd]->cgiHandler();
		_connection[fd]->setIsReadPipeFd(false);
		_connection[fd]->setIsWritePipeFd(false);
	}
}

void	Kqueue::removeConnection( const int eventFd ) {

	if (_connection[eventFd]->getCgiData()) {

		if (_connection[eventFd]->getCgiData()->getPid() > 0)
			kill(_connection[eventFd]->getCgiData()->getPid(), SIGKILL);
		if (_connection[eventFd]->getCgiData()->getCgiToServerClosed() == false &&
			_connection[eventFd]->getCgiData()->getCgiToServerReadFd() > 0) {

			if (close(_connection[eventFd]->getCgiData()->getCgiToServerReadFd()) == ERROR)
				errorLogNoResponse(EXIT, "close() failed on readPipeFd");
		}
		if (_connection[eventFd]->getCgiData()->getServerToCgiClosed() == false &&
			_connection[eventFd]->getCgiData()->getServerToCgiWriteFd() > 0) {

			if (close(_connection[eventFd]->getCgiData()->getServerToCgiWriteFd()) == ERROR)
				errorLogNoResponse(EXIT, "close() failed on writePipeFd");
		}
	}
	delete _connection[eventFd];
	_connection.erase(eventFd);
	if (close(eventFd) == ERROR)
		throw (Kqueue::CloseFailed());
}

/*************
* exceptions *
**************/

const char*	Kqueue::KqueueCreationError::what() const throw() {

	return ("creation of kqueue failed");
}

const char*	Kqueue::KeventPollingError::what() const throw() {

	return ("kevent failed to poll for events in kqueue");
}

const char*	Kqueue::KeventSetError::what() const throw() {

	return ("kevent failed to set event in kqueue");
}

const char*	Kqueue::CloseFailed::what() const throw() {

	return ("close failed");
}
