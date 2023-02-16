/***********
* includes *
***********/

#include "../includes/BlockAlgorithm.hpp"
#include "../includes/ConfigParser.hpp"
#include "../includes/Connection.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <iostream> /* std::cout */

/***************************
* constructor + destructor *
***************************/

BlockAlgorithm::BlockAlgorithm( ConfigParser *conf, Connection *connection ) :
_conf(conf),
_connection(connection),
_server(0),
_location(0),
_defaultSet(false) {

	matchServerBlock();
	matchLocationBlock();
	setMatch();

	if (_connection->_state == State::ERR || errorCheck() == ERROR)
		return ;
	checkRedirect();
}

BlockAlgorithm::~BlockAlgorithm() {}

/********************
* private functions *
********************/

void    BlockAlgorithm::matchServerBlock( void ) {

	if (listenDirectiveMatch())
		return ;
	serverNameDirectiveMatch();
}

bool	BlockAlgorithm::listenDirectiveMatch( void ) {

	int res = exactMatch();
	if (res == Match::ONE)
		return (true);
	if (res == Match::NONE && portMatch() == Match::ONE)
		return (true);
	return (false);
}

int	BlockAlgorithm::exactMatch( void ) {

	for (size_t i = 0; i < _conf->getServerBlock().size(); i++) {

		if ((_conf->getServerBlock()[i].getHost() == _connection->_request->getHost())
			&& (_conf->getServerBlock()[i].getPort() == _connection->_port))
			addMatch(i);
	}
	return (isMatch());
}

int	BlockAlgorithm::portMatch( void ) {

	for (size_t i = 0; i < _conf->getServerBlock().size(); i++) {

		if ((_conf->getServerBlock()[i].getHost() == "0.0.0.0")
			&& (_conf->getServerBlock()[i].getPort() == _connection->_port))
			addMatch(i);
	}
	return (isMatch());
}

void	BlockAlgorithm::addMatch( int i ) {

	_serverBlock.push_back(i);
	if (_defaultSet == false) {

		_server = i;
		_defaultSet = true;
	}
}

int 	BlockAlgorithm::isMatch( void ) {

	if (_serverBlock.size() == 1) {
		_server = *(_serverBlock.begin());
		return (Match::ONE);
	}
	else if (_serverBlock.size() > 1)
		return (Match::MULTIPLE);
	return (Match::NONE);
}

bool	BlockAlgorithm::serverNameDirectiveMatch( void ) {

	for (std::list<int>::iterator it = _serverBlock.begin(); it != _serverBlock.end(); it++) {

		for(size_t i = 0; i < _conf->getServerBlock()[*it].getServerName().size(); i++) {
			if (_conf->getServerBlock()[*it].getServerName()[i] == _connection->_request->getHost()) {
				_server = *it;
				return (true);
			}
		}
	}
	return (false);
}

void	BlockAlgorithm::matchLocationBlock( void ) {

	std::vector<LocationBlock> locationBlocks = _conf->getServerBlock()[_server].getLocation();

	for (size_t i = 0; i < locationBlocks.size(); i++) {

		_locationLen = locationBlocks[i].getPath().length();
		if (_connection->_request->getRequestTarget().length() >= _locationLen) {

			if (_connection->_request->getRequestTarget().compare(0, _locationLen, locationBlocks[i].getPath()) == 0)
				_locationMap[i] = _locationLen;
		}
	}

	if (_locationMap.size() == 0)
		_conf->getServerBlock()[_server].setlocationExists(false);
	else {

		setMaxValue();
		_conf->getServerBlock()[_server].setlocationExists(true);
		_conf->getServerBlock()[_server].setLocationIdx(_location);
	}
}

void	BlockAlgorithm::setMaxValue( void ) {

	std::pair<size_t, size_t> maxValue = std::make_pair(0, 0);

	for (st_map_it = _locationMap.begin(); st_map_it != _locationMap.end(); st_map_it++) {

		if (st_map_it->second > maxValue.second)
			maxValue = std::make_pair(st_map_it->first, st_map_it->second);
	}
	_location = maxValue.first;
}

void	BlockAlgorithm::setMatch( void ) {

	_connection->_data._serverBlock = &_conf->getServerBlock()[_server];
	if (_connection->_data._serverBlock->getLocationExists() == true)
		_connection->_data._locationBlock = &_connection->_data._serverBlock->getLocation()[_location];
}

int	BlockAlgorithm::errorCheck( void ) {

	if (requestMethodAllowed() == ERROR ||
		maxBodySize() == ERROR)
		return (ERROR);
	return (0);
}

int		BlockAlgorithm::requestMethodAllowed( void ) {

	if (_connection->_data._locationBlock != NULL) {

		if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::ALLOWED_METHODS) == true) {

			if (_connection->_data._locationBlock->getMethodExists(_connection->_request->getRequestMethod()) == true)
				return (0);
		}
	}
	return (_connection->errorLog(405, "request method not allowed", true));
}

int		BlockAlgorithm::maxBodySize() {

	if (_connection->_request->getHeaderFields().find("Content-Length") != _connection->_request->getHeaderFields().end()) {

		if (_connection->_request->getHeaderValue("Content-Length").size() > 0) {

			if (toInt(_connection->_request->getHeaderValue("Content-Length")) >
				_connection->_data._serverBlock->getClientMaxBodySize()) {

				return (_connection->errorLog(413, "client max body size exceeded " +
					toString(_connection->_data._serverBlock->getClientMaxBodySize()) + "m", true));
			}
		}
	}
	return (0);
}

void	BlockAlgorithm::checkRedirect() {

	if (_connection->_data._locationBlock) {

		if (_connection->_data._locationBlock->getDirectiveExists(LocationBlock::RETURN) == true) {

			if (_connection->_request->getMethodToString(_connection->_request->getRequestMethod()) == "GET") {

				_connection->_request->setRedirectLocation(_connection->_data._locationBlock->getRedirect()[1]);
				_connection->_data._statusCode = toInt(_connection->_data._locationBlock->getRedirect()[0]);
				_connection->_state = State::REDIRECT;
			}
			else
				_connection->errorLog(403, "redirects only allowed on GET requests", true);
		}
	}
}

/**************************
* private debug functions *
**************************/

void	BlockAlgorithm::printLists( void ) {

	std::cout << MAGENTA "SERVER BLOCKS" << std::endl;
	for (std::list< int >::iterator it = _serverBlock.begin(); it != _serverBlock.end(); it++)
		std::cout << "_serverBlocks = " << *it << std::endl;
}

void	BlockAlgorithm::printMap( st_map matchMap ) {

	for (st_map_it = matchMap.begin(); st_map_it != matchMap.end(); st_map_it++)
		std::cout << "key: " << (*st_map_it).first << std::endl << "value: " << (*st_map_it).second << std::endl;
}
