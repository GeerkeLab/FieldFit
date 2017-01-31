#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <sstream>
#include <stdio.h>
#include <cstring>
#include <iomanip>

#include "common/types.h"

namespace Util
{
    std::string PostWhiteSpace( const std::string &str, const U32 totalLength );
	std::string AddWhiteSpace( const std::string &str, const U32 totalLength );
	
    template <typename T>
    std::string ToString( const T &value, const S32 prefLength = -1 )
    {
        std::stringstream ss;
        ss << value;
        
        std::string result = ss.str();
        
        if ( prefLength > 0 )
        	 result = AddWhiteSpace( result, prefLength );
        
        return result;
    }

    template <typename T>
    T FromString( const std::string &str )
    {
        std::stringstream ss( str );
        T result;
        return ss >> result ? result : 0;
    }

    template<>
    float FromString< float >( const std::string &str );
    
	std::string Trim(const std::string &s);
	
	U32 ToFlags( const std::string &flagString );
	std::string FromFlags( const U32 convFlags );
}


#endif