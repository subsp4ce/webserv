/***********
* includes *
***********/

#include "../includes/BlockAlgorithm.hpp"
#include "../includes/Cgi.hpp"
#include "../includes/CgiResponse.hpp"
#include "../includes/Connection.hpp"
#include "../includes/DeleteResource.hpp"
#include "../includes/DirectoryListing.hpp"
#include "../includes/ErrorResponse.hpp"
#include "../includes/GenericResponse.hpp"
#include "../includes/Request.hpp"
#include "../includes/ResolvePath.hpp"
#include "../includes/utils.hpp"
#include <iostream> /* std::cout */

/***************************
* constructor + destructor *
***************************/

Connection::Connection( ConfigParser *conf, const int fd, const int port ) :
_conf(conf),
_fd(fd),
_port(port),
_state(State::ACCEPTED),
_request(NULL),
_response(NULL),
_responseType(0),
_cgiData(NULL),
_blockMatch(false),
_isReadPipeFd(false),
_isWritePipeFd(false),
_startTime(0) {

	std::memset(&_data, 0, sizeof(_data));
	_data._statusCode = 200;
}

Connection::~Connection( void ) {

	delete (_request);
	delete (_response);
	delete (_cgiData);
}

/*******************
* public functions *
*******************/

int					Connection::getFd( void ) { return (_fd); }
int					Connection::getState( void ) { return (_state); }
long int const &	Connection::getStartTime( void ) const { return ( _startTime ); }
int					Connection::getResponseType( void ) { return (_responseType); }
Cgi* 				Connection::getCgiData( void ) { return (_cgiData); }
Request	*			Connection::getRequest( void ) const { return ( _request ); }
void				Connection::setState( int state ) { _state = state; }
void				Connection::setIsReadPipeFd( const bool isReadPipeFd ) { _isReadPipeFd = isReadPipeFd; }
void				Connection::setIsWritePipeFd( const bool isWritePipeFd ) { _isWritePipeFd = isWritePipeFd; }

void	Connection::createRequest( void ) {

	_request = new Request(this);

	if (_request->_headerParsed == true ||
		_state == State::ERR) {

		matchBlocks();
		_blockMatch = true;
	}
}

void	Connection::keepReceiving( void ) {

	_request->readRequest();
	if ((_request->_headerParsed == true && _blockMatch == false) ||
		_state == State::ERR) {

		matchBlocks();
		_blockMatch = true;
	}
}

void	Connection::createResponse( void ) {

	if (_state == State::ERR || _state == State::REDIRECT) {

		_response = new ErrorResponse(this);
		return;
	}
	if (_responseType == ResponseType::AUTOINDEX)
		_response = new DirectoryListing(this);
	else if (_responseType == ResponseType::DELETE)
		_response = new DeleteResource(this);
	else if (_responseType == ResponseType::CGI)
		_response = new CgiResponse(this); 
	else
		_response = new GenericResponse(this);

	if (_state == State::ERR) {
		delete (_response);
		_response = new ErrorResponse(this);
	}
}

void	Connection::chunkedResponse( void ) {

	_response->sendResponse();
}

void	Connection::createCgi( void ) {

	_cgiData = new Cgi(this);
}

void	Connection::cgiHandler( void ) {

	if (_cgiData->getCgiState() == _cgiData->Cgi::WAITING)
		_cgiData->waitForChild();
	else if (_cgiData->getCgiState() == Cgi::WRITE)
		_cgiData->launchCgi();
	else if (_cgiData->getCgiState() == Cgi::WRITING && _isWritePipeFd == true)
		_cgiData->writeToCgi();
	else if (_cgiData->getCgiState() == Cgi::WRITING)
		_cgiData->cgiWait();
	else if (_cgiData->getCgiState() == Cgi::READING && _isReadPipeFd == true) {

		_cgiData->readFromCgi();
		if (_state == State::ERR || _state == State::REMOVE ||
			_cgiData->getCgiState() == _cgiData->Cgi::READING)
			return ;
		_state = State::DONE;
	}
	else if (_cgiData->getCgiState() == Cgi::DONE)
		_state = State::DONE;
}

int		Connection::errorLog( int statusCode, std::string msg, bool response ) {

	if (statusCode == EXIT) {
		
		std::cout << RED << "[fatal error] " << msg << RESET << std::endl;
		exit(EXIT);
	}
	else if (_data._statusCode == 200) {

		_data._statusCode = statusCode;
		std::cout << YELLOW << "[" << getStatus(statusCode) << "] " << msg << RESET << std::endl;
	}

	if (response == true)
		_state = State::ERR;

	return (ERROR);
}

/********************
* private functions *
********************/

void	Connection::matchBlocks( void ) {

	BlockAlgorithm(_conf, this);
	if (_state == State::REDIRECT)
		return ;
	resolvePath();
}

void	Connection::resolvePath( void ) {

	ResolvePath(this);
}

int Connection::startTimer( void ) {

	_startTime = (static_cast<long int>(time(NULL)));
	if (_startTime == ERROR) {
		_state = State::TIMER_FAILED;
		return (errorLog(500, "unable to set start time for request", true));
	}
	return (0);
}
