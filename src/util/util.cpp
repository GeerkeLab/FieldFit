#include "util.h"

#include <stdlib.h> 
#include <ctype.h>
#include <iostream>

template<>
float Util::FromString( const std::string &str )
{
    return std::atof( str.c_str() );
}

std::string Util::AddWhiteSpace( const std::string &str, const U32 totalLength )
{
	std::string prefix = "";
	if ( totalLength > str.size() )
    {
    	prefix.resize( totalLength-str.size(), ' ' );
    }
        
    return prefix+str;
}

std::string Util::Trim(const std::string &s)
{
    int nSize = s.size();
    int nSPos = 0, nEPos = 1, i;
    for(i = 0; i< nSize; ++i) {
        if( !isspace( s[i] ) ) {
            nSPos = i ;
            break;
        }
    }
    for(i = nSize -1 ; i >= 0 ; --i) {
        if( !isspace( s[i] ) ) {
            nEPos = i;
            break;
        }
    }
    return std::string(s, nSPos, nEPos - nSPos + 1);
}

U32 Util::ToFlags( const std::string &flagString )
{
	U32 convFlags = 0;
	for ( U32 j=0; j < flagString.size(); ++j )
	{
		char flag = flagString[j];
	
		if ( flag == '1' )
		{
			convFlags += ( 1 << j );
		}
	}
	
	return convFlags;
}

std::string Util::FromFlags( const U32 convFlags )
{
	std::string rstring;
	
	for ( U32 i=0; i < 9; ++i )
	{
		if ( convFlags & ( 1 << i ) )
			
			rstring += '1';
		else
			rstring += '0';
		
	}
	
	return rstring;
}
