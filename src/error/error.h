#ifndef __ERROR_H__
#define __ERROR_H__

#include <iostream>

namespace Console
{
	void Report( std::ostream &stream, const std::string &msg );
	
	void Debug( std::ostream &stream, const std::string &msg );
}

namespace Error
{

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

	bool FAILED( STATUS status );

	void Warn( std::ostream &stream, const std::string &msg );
}

#endif