#ifndef __TESTING_H__
#define __TESTING_H__

#include <cmath>
#include <limits>

#include "../error/error.h"

class BlockParser;
class Configuration;

namespace Testing
{	
	template< class T >
	bool IsEqual( T a, T b, T eps ) 
	{
		return std::fabs(a - b) < eps;
	}
	
	Error::STATUS CompareToF( const Configuration& conf, const BlockParser& );
}

#endif