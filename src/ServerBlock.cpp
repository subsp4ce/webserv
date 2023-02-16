#include "../includes/ServerBlock.hpp"

/***************************
* constructor + destructor *
***************************/

ServerBlock::ServerBlock( void ) :
_host("0.0.0.0"),
_port(80),
_root("www"),
_clientMaxBodySize(1000000),
_locationIdx(0),
_locationExists(false) {

	std::memset(_directiveExists, 0, sizeof(_directiveExists));
}

ServerBlock::~ServerBlock( void ) {}

/**********
* getters *
***********/

bool					const & ServerBlock::getDirectiveExists( int directive ) const { return (_directiveExists[directive]); }
std::string				const & ServerBlock::getListen( void ) const { return (_listen); }
std::string				const & ServerBlock::getHost( void ) const { return (_host); }
int						const & ServerBlock::getPort( void ) const {return (_port); }
ServerBlock::s_vector	const & ServerBlock::getServerName( void ) const { return (_serverName); }
std::string				const & ServerBlock::getRoot( void ) const { return (_root); }
ServerBlock::s_map		const & ServerBlock::getErrorPage( void ) const { return (_errorPage); }
int						const & ServerBlock::getClientMaxBodySize( void ) const { return (_clientMaxBodySize); }
ServerBlock::lb_vector	&		ServerBlock::getLocation( void ) { return (_location); }
int						const & ServerBlock::getLocationIdx( void ) const { return( _locationIdx ); }
bool					const & ServerBlock::getLocationExists( void ) { return ( _locationExists ); }

/**********
* setters *
***********/

void		ServerBlock::setDirectiveExists( int directive ) { _directiveExists[directive] = true; }
void		ServerBlock::setListen( std::string listen ) { _listen = listen; }
void		ServerBlock::setHost( std::string host ) { _host = host; }
void		ServerBlock::setPort( int port ) { _port = port; }
void		ServerBlock::setServerName( std::string serverName ) { _serverName.push_back(serverName); }
void		ServerBlock::setRoot( std::string root ) { _root = root; }
void		ServerBlock::setErrorPage( std::pair<std::string, std::string> pair ) { _errorPage.insert(pair); }
void		ServerBlock::setClientMaxBodySize( int clientMaxBodySize ) { _clientMaxBodySize = clientMaxBodySize; }
void		ServerBlock::setLocation( void ) { _location.push_back(LocationBlock()); }
void		ServerBlock::setLocationIdx( int locationIdx ) { _locationIdx = locationIdx; }
void		ServerBlock::setlocationExists( bool locationExists ) { _locationExists = locationExists; }
