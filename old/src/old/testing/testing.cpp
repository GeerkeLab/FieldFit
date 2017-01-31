#include "testing.h"

#include "../system/configuration.h"

#include "../error/error.h"
#include "../io/blockParser.h"
#include "../io/block.h"
#include "../util/util.h"

Error::STATUS Testing::CompareToF( const Configuration& conf, const BlockParser&bp )
{
	const Block *block = bp.GetBlock("[F_RESULT]");

	//we dont have to do testing against fortran
    if ( !block )
    {
        return Error::STATUS::OPTIONAL_BLOCK;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [F_RESULT] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_FTEST;
    }

    U32 sites = block->GetToken( 0 )->GetValue< U32 >();

    if ( sites < 1 )
    {
        Error::Warn( std::cout, "At least 1 site was expected!" );
        return Error::STATUS::FAILED_FTEST;
    }
    
    // Always use collection 0, as fortran doesnt support multiframe
    const U32 collection = 0;
    
    if ( conf.FitSites(collection) != sites )
    {
    	Error::Warn( std::cout, "The [F_RESULT] input and [SITES] input are not of equal size!" );
        return Error::STATUS::FAILED_FTEST;
    }

    if ( block->Size() != ( 1 + ( sites * 23 ) ) )
    {
        Error::Warn( std::cout, "block [F_RESULT] did not have the right amount of arguments based on the size indicator!" );
        return Error::STATUS::FAILED_FTEST;
    }

    U32 index = 1;
    
    std::cout << "[SYSTEM_TEST]" << std::endl;
    std::cout << "Comparing c++ fitting to fortran fitting!" << std::endl;
    for ( U32 i=0; i < sites; ++i )
    {
    	const Configuration::FitSite *site = conf.GetSite( collection, i );
    	
    	F32 x = block->GetToken( index+1 )->GetValue< F32 >();
    	F32 y = block->GetToken( index+2 )->GetValue< F32 >();
		F32 z = block->GetToken( index+3 )->GetValue< F32 >();	
		
		if ( !IsEqual( x, site->position.GetX(), 0.00001f ) ||
			 !IsEqual( y, site->position.GetY(), 0.00001f ) ||
		     !IsEqual( z, site->position.GetZ(), 0.00001f )
		   )
		{
			Error::Warn( std::cout, std::string( std::string("For site with ID ")+Util::ToString( site->ID )
					                                +" the coordinates do not match the F_Result input!" ) );
			return Error::STATUS::FAILED_FTEST;
		}
		
		index += 5;
		
		for ( U32 j=0; j < 9; ++j )
		{
			F32 refVal = block->GetToken( index+(j*2)+1 )->GetValue< F32 >();
			F32 currValue = site->GetValue( static_cast< Configuration::valueType >( j )  );
			
			if ( !IsEqual( refVal, currValue, 0.0001f ) )
			{
				Error::Warn( std::cout, std::string("For site with ID ")+Util::ToString( site->ID )
												    +" type "+Util::FromFlags( ( 1 << j ) )+" the fitted value does not match the F_Result input!" );
				Error::Warn( std::cout, std::string("Reference value of " )+Util::ToString( refVal )+" vs a fitted value of "+Util::ToString( currValue ) );
				
				return Error::STATUS::FAILED_FTEST;
			}
			
		}
		
		index += 18;
    }
    
    std::cout << "[END]" << std::endl;
    
    return Error::STATUS::OK;
}



