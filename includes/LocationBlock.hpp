#pragma once

/***********
* includes *
***********/

#include <string>
#include <vector>

/********
* class *
*********/

class LocationBlock {

public:

	/***********
	* typedefs *
	***********/

	typedef std::vector<std::string>	s_vector;

	/********
	* enums *
	*********/

	enum method {

		GET,
		POST,
		DELETE
	};

	enum directive {

		ALLOWED_METHODS,
		RETURN,
		ROOT,
		ALIAS,
		AUTOINDEX,
		DEFAULT_FILE,
		CGI,
		FILE_UPLOAD
	};

	/***************************
	* constructor + destructor *
	***************************/

	LocationBlock( void );
	~LocationBlock( void );

	/**********
	* getters *
	***********/

	bool		const & getDirectiveExists( int directive ) const;
	bool		const & getMethodExists( int method ) const;
	std::string	const & getPath( void ) const;
	s_vector	const & getAllowedMethods( void ) const;
	s_vector	const & getRedirect( void ) const;
	std::string	const & getRoot( void ) const;
	std::string	const & getAlias( void ) const;
	s_vector	const & getDefaultFile( void ) const;
	bool		const & getAutoindex ( void ) const;
	s_vector	const & getCgi( void ) const;
	std::string	const & getFileUpload( void ) const;

	/**********
	* setters *
	***********/

	void		setDirectiveExists( int directive );
	void		setMethodExists( int method );
	void		setPath( std::string path );
	void		setAllowedMethods ( std::string method );
	void		setRedirect( std::string redirect );
	void		setRoot( std::string root );
	void		setAlias( std::string alias );
	void		setDefaultFile( std::string defaultFile );
	void		setAutoindex( bool status );
	void		setCgi( std::string cgi );
	void		setFileUpload( std::string fileUpload );

private:

	/********************
	* private variables *
	********************/

	bool		_directiveExists[10];
	bool		_methodExists[3];
	std::string	_path;
	s_vector	_allowedMethods;
	s_vector	_redirect; /* redirect[0] = status code, redirect[1] = new url */
	std::string	_root;
	std::string	_alias;
	s_vector	_defaultFile;
	bool		_autoindex;
	s_vector	_cgi; /* cgi[0] = file extension, cgi[1] = path */
	std::string	_fileUpload;
};
