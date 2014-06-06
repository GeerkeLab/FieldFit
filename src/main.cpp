#include "system/field.h"
#include "system/configuration.h"

#include "fitting/fitter.h"
#include "error/error.h"
#include "io/blockParser.h"
#include "io/block.h"

#include "testing/testing.h"

#include "system/report.h"

#include <iostream>

int main( int argc, char** argv)
{	
	std::vector< std::string > arguments;
	arguments.reserve( argc );
	for ( S32 i=0; i < argc; ++i )
	{
		arguments.push_back( std::string( argv[i] ) );
	}
	
	BlockParser bp( arguments );

    if ( Error::FAILED( bp.GetStatus() ) )
    {
        Error::Warn( std::cout, "Unable to open all presented files!" );
        return 1;
    }  
    
    Field field( bp );
    
    if ( Error::FAILED( field.GetStatus() ) )
    {
    	Error::Warn( std::cout, "Unable to generate a field input, fatal error!" );

         system("pause");

    	return 1;
    }
    
    Configuration conf( bp );

    if ( Error::FAILED( conf.GetStatus() ) )
    {
    	Error::Warn( std::cout, "Unable to generate a configuration input, fatal error!" );
    	
         system("pause");

        return 1;
    }
    
    conf.Raport( std::cout );
          
    //init the permanent field
    if ( Error::FAILED( field.SetPermField( conf ) ) )
    {
    	Error::Warn( std::cout, "Unable to set up the permanent field contributions, fatal error!" );
    	
        system("pause");

        return 1;	
    }
    
    if ( Error::FAILED( Fitter::FitSites( conf, field ) ) )
    {
    	Error::Warn( std::cout, "Unable to fit the selected sites, fatal error!" );

         system("pause");

    	return 1;
    }
    
    Report rp( conf, field );
    rp.WriteToStream( std::cout );
    
    //test the final values to fortran values if a test block was given
    if ( Error::FAILED(  Testing::CompareToF( conf, bp ) ) )
    {
    	Error::Warn( std::cout, "System failed the unit test, see output!" );

    	return 1;		
    }

	return 0;
}