#include "field.h"

#include <iostream>

#include "../io/block.h"

#include "../fitting/fitter.h"

Field::Field( const BlockParser &bp )
{
    const Block *block = bp.GetBlock("[FIELD]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [FIELD] was not present!" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [FIELD] was too small ( at least 1 argument1 expected ) !" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    //U32 genSites = block->GetToken( 0 )->GetValue< U32 >();
    U32 potentialSites = block->GetToken( 0 )->GetValue< U32 >();

    //test for the correct size
    if ( block->Size() != ( 1  + ( potentialSites * 4 ) ) )
    {
        Error::Warn( std::cout, "block [FIELD] did not have the right amount of arguments based on the size indicators !" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    U32 index = 1;
    
    for ( U32 i=0; i < potentialSites; ++i )
    {
        F64 potential = block->GetToken( index )->GetValue< F64 >();
        F32 x = block->GetToken( index+1 )->GetValue< F32 >();
        F32 y = block->GetToken( index+2 )->GetValue< F32 >();
        F32 z = block->GetToken( index+3 )->GetValue< F32 >();

        mFieldPositions.push_back( Vec3( x, y, z ) );
        mFieldPotential.push_back( potential );

        index += 4;
    }

    //make sure the perm potential is init
    mPermPotential.resize( potentialSites, 0.0 );
    
    mStatus = Error::STATUS::OK;
}

Error::STATUS Field::GetStatus() const
{
    return mStatus;
}

U32 Field::Size() const
{
    return mFieldPositions.size();
}

const Vec3 & Field::GetFieldPosition( const U32 index ) const
{
    return mFieldPositions[ index ];
}

F64 Field::GetFieldPotential( const U32 index ) const
{
    return mFieldPotential[ index ];
}

F64 Field::GetFieldDiff( const U32 index ) const
{
	return GetFieldPotential( index ) - GetPermPotential( index );
}

F64 Field::GetPermPotential( const U32 index ) const
{
	return mPermPotential[ index ];
}

F64 Field::GetGridPointPotential( const Vec3 &gridPoint, const Configuration &conf ) const
{	
	const F64 f=2.541748;
    const F64 f1=1.3450351;
	
    F64 potential = 0.0;
    
	for ( U32 i=0; i < conf.Size(); ++i )
    {
        const Configuration::FitSite *site = conf.GetSite( i );
        const Vec3 & pos   = site->position;
        const U32 permFlags = site->permFlags;
        
        F64 dist_2 = gridPoint.Distance2( pos );
        F64 dist   = Mathf::Sqrt( dist_2 );
        
        //charges
        {
        	const F64 ccoef = 1.0 / dist;
        	
        	potential += ( ccoef * site->GetValue( Configuration::valueType::charge ) );
        	
        	if ( permFlags & 0x01 )	
        		potential += ( ccoef * site->GetPermValue( Configuration::valueType::charge ) );
        }
        
        //dipoles
        {
        	for ( U32 j=1; j < 4; ++j )
        	{
        		Configuration::valueType type = static_cast< Configuration::valueType >( j );
        		
        		const F64 dcoef = Fitter::DelComp( type, gridPoint, pos ) / ( dist_2 * dist ); 
        		
        		potential += ( dcoef * ( site->GetValue( type ) / f ) );
        		
        		if ( permFlags & ( 1 << j ) )
        			potential += ( dcoef * ( site->GetPermValue( type ) / f ) );   
        	}
        }
        
        //qpol
        {
        	for ( U32 j=4; j < 9; ++j )
        	{
        		Configuration::valueType type = static_cast< Configuration::valueType >( j );
        	
				const F64 qcoef = Fitter::DelComp( type, gridPoint, pos ) / ( dist_2 * dist_2 * dist ); 
				
				potential += ( qcoef  * ( site->GetValue( type ) / f1 ) );
				
				if ( permFlags & ( 1 << j ) )
					potential += ( qcoef  * ( site->GetPermValue( type ) / f1 ) ); 
			
			}
        }
    }
    
    return potential;
}
    
Error::STATUS Field::SetPermField( const Configuration &conf )
{
	for ( U32 i=0; i <  mFieldPositions.size(); ++i )
	{
		mPermPotential[ i ] = GetGridPointPotential( mFieldPositions[i], conf );
	}
	
	return Error::STATUS::OK;
}

F64 Field::GetFieldStats( const Configuration &conf ) const
{
	F64 chiSqr = 0.0;
	
	for ( U32 i=0; i <  mFieldPositions.size(); ++i )
	{	
		F64 pot = mFieldPotential[i];
		F64 potCalc = GetGridPointPotential( mFieldPositions[i], conf );
		F64 diff = pot - potCalc;
		
		chiSqr += ( diff * diff );
	}
	
	return chiSqr;
}



