#include "error.h"

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
		default:
			result = true;
			break;
		}
		
		return result;
	}
	
    void Debug( std::ostream &stream, const std::string &msg )
    {
    #ifdef USE_REPORTING

        stream << msg << std::endl;

    #endif
    }

	void Report( std::ostream &stream, const std::string &msg )
	{
		stream << msg.c_str() << std::endl;
	}

	void Warn( std::ostream &stream, const std::string &msg )
	{
		stream << msg.c_str() << std::endl;
	}
	
}