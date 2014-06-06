#ifndef __ERROR_H__
#define __ERROR_H__

#include <iostream>

/*
**
**	Namespace for general output to the console	
**
*/
namespace Console
{
	void Report( std::ostream &stream, const std::string &msg );
	
	void Debug( std::ostream &stream, const std::string &msg );
}

/*
**
**	Namespace for general error handling	
**
*/
namespace Error
{
	/*
	**	All return error types;
	*/
	enum class STATUS
	{
		OK = 1,
		OPTIONAL_BLOCK = 2,
		FAILED_IO = 3,
		FAILED_CONSTR_R = 4,
		FAILED_FIT = 5,
		FAILED_UNSUP_PARAM_COMBO = 6,
		FAILED_FTEST = 7
	};
	
	/*
	**	Function that tests if the the return status is a fatal error
	*/
	bool FAILED( STATUS status );

	/*
	**	Formatted console write
	*/
	void Warn( std::ostream &stream, const std::string &msg );
}

#endif