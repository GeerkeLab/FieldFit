#include "error.h"

namespace Console
{
	void Report( std::ostream &stream, const std::string &msg )
	{
		stream << msg.c_str() << std::endl;
	}
	
	void Debug( std::ostream &stream, const std::string &msg )
    {
    #ifdef USE_REPORTING

        stream << msg << std::endl;

    #endif
    }
}

namespace Error
{
	bool FAILED( STATUS status )
	{
		bool result;
		
		switch(status)
		{
		case STATUS::OK:
			result = false;
			break;
		case STATUS::OPTIONAL_BLOCK:
			result = false;
			break;
		default:
			result = true;
			break;
		}
		
		return result;
	}

	void Warn( std::ostream &stream, const std::string &msg )
	{
		stream << "<<< [ERROR] >>> " << msg.c_str() << std::endl;
	}
	
}