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
	
	void Report( std::ostream &stream, const std::string &msg )
	{
		#ifdef USE_REPORTING
			
		stream << msg << std::endl;
		
		#endif
	}

	void Warn( std::ostream &stream, const std::string &msg )
	{
		stream << msg << std::endl;
	}
	
}