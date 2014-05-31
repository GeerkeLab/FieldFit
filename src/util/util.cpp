#include "util.h"

#include <stdlib.h> 
#include <ctype.h>
#include <iostream>

template<>
float Util::FromString( const std::string &str )
{
    return std::atof( str.c_str() );
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
