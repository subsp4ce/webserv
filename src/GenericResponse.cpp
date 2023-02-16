/***********
* includes *
***********/

#include "../includes/GenericResponse.hpp"
#include "../includes/Connection.hpp"
#include <sstream> /* std::stringtream */
#include <fstream> /* std::ifstream */

/***************************
* constructor + destructor *
***************************/

GenericResponse::GenericResponse( Connection *connection ) :
AResponse(connection) {

	if (generateBody() == ERROR)
		return ;
	generateResponse();
	sendResponse();
}

GenericResponse::~GenericResponse( void ) {}

/*******************
* private function *
*******************/

int	GenericResponse::generateBody( void ) {

	std::stringstream buffer;

	std::ifstream f(_connection->_data._path, std::ifstream::binary);
	if (f.is_open())
		buffer << f.rdbuf();
	else
		return (_connection->errorLog(404, "unable to open file", true));
	_body = buffer.str();
	f.close();
	return (0);
}
