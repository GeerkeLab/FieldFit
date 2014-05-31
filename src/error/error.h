#ifndef __ERROR_H__
#define __ERROR_H__

#include <iostream>

namespace Error
{

	enum class STATUS
	{
		OK = 1,
		FAILED_IO = 2,
	};

	bool FAILED( STATUS status );
	
	void Report( std::ostream &stream, const std::string &msg );

	void Warn( std::ostream &stream, const std::string &msg );
}

#endif