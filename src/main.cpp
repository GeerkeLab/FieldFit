#include "system/field.h"
#include "system/configuration.h"

#include "fitting/fitter.h"
#include "error/error.h"


#include <iostream>

int main()
{	
    Field field( "../testInput/basicDipole/1007_esp.in" );
    
    if ( Error::FAILED( field.GetStatus() ) )
    {
    	Error::Warn( std::cout, "Unable to generate a field input, fatal error!" );

         system("pause");

    	return 1;
    }
    
    Configuration conf( "../testInput/basicDipole/1007.fsite" );

    if ( Error::FAILED( conf.GetStatus() ) )
    {
    	Error::Warn( std::cout, "Unable to generate a configuration input, fatal error!" );
    	
         system("pause");

        return 1;
    }
    
    conf.Raport( std::cout );
                                   
    if ( Error::FAILED( Fitter::FitSites( conf, field ) ) )
    {
    	Error::Warn( std::cout, "Unable to fit the selected sites, fatal error!" );

         system("pause");

    	return 1;
    }
    
    system("pause");

	return 0;
}