#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <sstream>
#include <stdio.h>
#include <cstring>
#include <iomanip>

#include "../common/types.h"

namespace Util
{
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
    std::string ToStringFormatted( const T &value, U32 decimals, const S32 prefLength = -1 )
    {	
        std::stringstream ss;
        ss << std::fixed << std::setprecision(decimals) << value;
       
        //clip
        std::string intermediate = ss.str();
        //intermediate.erase( intermediate.size() - decimals + 1 );         
        
        //add length
        if ( prefLength > 0 )
        	intermediate = AddWhiteSpace( intermediate, prefLength );
        
        return intermediate;
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