/***********
* includes *
***********/

#include "../includes/LocationBlock.hpp"

/***************************
* constructor + destructor *
***************************/

LocationBlock::LocationBlock( void ) :
_autoindex(false) {

	std::memset(_directiveExists, 0, sizeof(_directiveExists));
	std::memset(_methodExists, 0, sizeof(_methodExists));
}

LocationBlock::~LocationBlock( void ) {}

/*******************
* public functions *
*******************/

bool					const & LocationBlock::getDirectiveExists( int directive ) const { return (_directiveExists[directive]); }
bool					const & LocationBlock::getMethodExists( int method ) const { return (_methodExists[method]); }
std::string				const & LocationBlock::getPath( void ) const { return (_path); }
LocationBlock::s_vector	const & LocationBlock::getAllowedMethods( void ) const { return (_allowedMethods); }
LocationBlock::s_vector	const & LocationBlock::getRedirect( void ) const { return (_redirect); }
std::string				const & LocationBlock::getRoot( void ) const { return (_root); }
std::string				const & LocationBlock::getAlias( void ) const { return (_alias); }
LocationBlock::s_vector	const & LocationBlock::getDefaultFile( void ) const { return (_defaultFile); }
bool					const & LocationBlock::getAutoindex( void ) const { return (_autoindex); }
LocationBlock::s_vector	const & LocationBlock::getCgi( void ) const { return (_cgi); }
std::string				const & LocationBlock::getFileUpload( void ) const { return (_fileUpload); }

void	LocationBlock::setDirectiveExists( int directive ) { _directiveExists[directive] = true; }
void	LocationBlock::setMethodExists( int method ) { _methodExists[method] = true; }
void	LocationBlock::setPath( std::string path ) { _path = path; }
void	LocationBlock::setAllowedMethods ( std::string method ) { _allowedMethods.push_back(method); }
void	LocationBlock::setRedirect( std::string value ) { _redirect.push_back(value); }
void	LocationBlock::setRoot( std::string root ) { _root = root; }
void	LocationBlock::setAlias( std::string alias ) { _alias = alias; }
void    LocationBlock::setDefaultFile( std::string defaultFile ) { _defaultFile.push_back(defaultFile); }
void	LocationBlock::setAutoindex( bool status ) { _autoindex = status; }
void	LocationBlock::setCgi( std::string cgi ) { _cgi.push_back(cgi); }
void	LocationBlock::setFileUpload( std::string fileUpload ) { _fileUpload = fileUpload; }
