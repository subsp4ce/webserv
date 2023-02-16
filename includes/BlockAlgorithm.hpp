#pragma once

/***********
* includes *
***********/

#include "LocationBlock.hpp"
#include <vector>
#include <list>
#include <map>

/**********
* defines *
**********/

#define ERROR -1

/**********************
* forward declaration *
**********************/

class Connection;
class ConfigParser;

/********
* class *
*********/

class BlockAlgorithm {

	struct Match {

		enum state {

			ONE,
			MULTIPLE,
			NONE
		};
	};

	/***********
	* typedefs *
	***********/

	typedef std::vector<LocationBlock>::iterator	l_vector_it;
	typedef std::map<size_t, size_t> 				st_map;
	std::map< size_t, size_t >::iterator 			st_map_it;

public:

	/***************************
	* constructor + destructor *
	***************************/

	BlockAlgorithm( ConfigParser *conf, Connection *connection );
	~BlockAlgorithm( void );

private:

	/********************
	* private variables *
	********************/

	ConfigParser	*_conf;
	Connection		*_connection;
	int				_server;
	int				_location;
	bool 			_defaultSet;
	st_map 			_locationMap;
	size_t			_locationLen;
	std::list<int>	_serverBlock;

	/********************
	* private functions *
	********************/

	void	matchServerBlock( void );
	bool	listenDirectiveMatch( void );
	int		exactMatch( void );
	int		portMatch( void );
	void	addMatch( int i );
	int 	isMatch( void );
	bool	serverNameDirectiveMatch( void );
	void	matchLocationBlock( void );
	void	setMaxValue( void );
	void	setMatch( void );
	int		errorCheck( void );
	void	checkRedirect( void );
	int		requestMethodAllowed( void );
	int		maxBodySize( void );

	/**************************
	* private debug functions *
	**************************/

	void	printLists( void );
	void	printMap( st_map matchMap );
};
