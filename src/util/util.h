#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <sstream>
#include <stdio.h>
#include <cstring>

#include "../common/types.h"

namespace Util
{
    template <typename T>
    std::string ToString( const T &value )
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
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
	
	template< class T > 
	T* CloneBuffer( T *in, U32 length )
	{
		T *newb = new T[ length ];
		
		memcpy( newb, in, length * sizeof( T ) );
		
		return newb;
	}
}


#endif