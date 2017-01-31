#include "field.h"

#include <iostream>

#include "../io/block.h"

#include "../fitting/fitter.h"

#include "../system/constants.h"

Field::Field( const BlockParser &bp )
{
    mStatus = ReadFieldSites( bp );
    
    if ( Error::FAILED( mStatus ) )
    {
        Error::Warn( std::cout, "Failed to init the field structure!" );
        
        return;
    }
    
    mStatus = ReadElectricFieldSites( bp );
    
    if ( Error::FAILED( mStatus ) )
    {
        Error::Warn( std::cout, "Failed to init the efield structure!" );
        
        return;
    }
    
    // Consistency checks
    if ( mEFieldCollections.size() > 0 )
    {
        if ( mCollections.size() != mEFieldCollections.size() )
        {
            Error::Warn( std::cout, "Number of field blocks should equal number of efield blocks!" );
            
            mStatus = Error::STATUS::FAILED_IO;
            
            return;
        }
    }
}

bool Field::ContainsElectricField() const
{
    return mEFieldCollections.size() > 0;
}

Error::STATUS Field::ReadFieldSites( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[FIELD]");
    
    if ( !blockArray )
    {
        Error::Warn( std::cout, "block [FIELD] was not present!" );

        return Error::STATUS::FAILED_IO;
    }
    
    mCollections.resize(blockArray->size());
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [FIELD] was too small ( at least 1 argument1 expected ) !" );
            
            return  Error::STATUS::FAILED_IO;
        }
        
        //U32 genSites = block->GetToken( 0 )->GetValue< U32 >();
        U32 potentialSites = block.GetToken( 0 )->GetValue< U32 >();
        
        //test for the correct size
        if ( block.Size() != ( 1  + ( potentialSites * 4 ) ) )
        {
            Error::Warn( std::cout, "block [FIELD] did not have the right amount of arguments based on the size indicators !" );
            
            return  Error::STATUS::FAILED_IO;
        }
        
        U32 index = 1;
        
        for ( U32 i=0; i < potentialSites; ++i )
        {
            // NOTE e / bohr unit
            F64 potential = block.GetToken( index )->GetValue< F64 >();
            
            // NOTE all in bohr
            F32 x = block.GetToken( index+1 )->GetValue< F32 >();
            F32 y = block.GetToken( index+2 )->GetValue< F32 >();
            F32 z = block.GetToken( index+3 )->GetValue< F32 >();
            
            mCollections[coll].fieldPositions.push_back( Vec3( x, y, z ) );
            mCollections[coll].fieldPotential.push_back( potential );
            
            index += 4;
        }
        
        //make sure the perm potential is init
        mCollections[coll].permPotential.resize( mCollections[coll].fieldPositions.size(), 0.0 );
        
        coll++;
    }
    
    return Error::STATUS::OK;
}



Error::STATUS Field::ReadElectricFieldSites( const BlockParser &bp )
{
    const std::vector< Block > *blockArray = bp.GetBlockArray("[EFIELD]");
    
    if ( !blockArray )
    {
        return Error::STATUS::OPTIONAL_BLOCK;
    }
    
    mEFieldCollections.resize(blockArray->size());
    
    size_t coll = 0;
    for ( const Block &block : *blockArray )
    {
        if ( block.Size() < 1 )
        {
            Error::Warn( std::cout, "block [EFIELD] was too small ( at least 1 argument1 expected ) !" );
 
            return Error::STATUS::FAILED_IO;
        }
        
        //U32 genSites = block->GetToken( 0 )->GetValue< U32 >();
        U32 potentialSites = block.GetToken( 0 )->GetValue< U32 >();
        
        //test for the correct size
        if ( block.Size() != ( 1  + ( potentialSites * 5 ) ) )
        {
            Error::Warn( std::cout, "block [EFIELD] did not have the right amount of arguments based on the size indicators !" );
            
            return Error::STATUS::FAILED_IO;
        }
        
        U32 index = 1;
        
        for ( U32 i=0; i < potentialSites; ++i )
        {
            
            
            
            F32 ex = block.GetToken( index+2 )->GetValue< F32 >();
            F32 ey = block.GetToken( index+3 )->GetValue< F32 >();
            F32 ez = block.GetToken( index+4 )->GetValue< F32 >();
            
            // kJ / mol nm e
            Vec3 efield( ex, ey, ez );
            
            // convert to e / nm^2
            efield /= Constants::four_pi_eps_rcp;
            
            // convert to e / bohr^2
            efield /= ( Constants::nm_bohr * Constants::nm_bohr );
            
            mEFieldCollections[coll].efieldSite.push_back( efield );
            
            index += 5;
        }
        
        coll++;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Field::GetStatus() const
{
    return mStatus;
}

size_t Field::Size( const U32 collection ) const
{
    return mCollections[collection].fieldPositions.size();
}

const Vec3 & Field::GetFieldPosition( const U32 collection, const U32 index ) const
{
    return mCollections[collection].fieldPositions[ index ];
}

F64 Field::GetFieldPotential( const U32 collection, const U32 index ) const
{
    return mCollections[collection].fieldPotential[ index ];
}

F64 Field::GetFieldDiff( const U32 collection, const U32 index ) const
{
	return GetFieldPotential( collection, index ) - GetPermPotential( collection, index );
}

F64 Field::GetPermPotential( const U32 collection, const U32 index ) const
{
	return mCollections[collection].permPotential[ index ];
}

F64 Field::GetGridPointPotential( const U32 coll, const Vec3 &gridPoint, const Configuration &conf ) const
{	
    F64 potential = 0.0;
    
	for ( U32 i=0; i < conf.FitSites(coll); ++i )
    {
        const Configuration::FitSite *site = conf.GetSite( coll, i );
        
        const Vec3 & pos   = site->position;
        const U32 permFlags = site->permFlags;
        
        F64 dist_2 = gridPoint.Distance2( pos );
        F64 dist   = Mathf::Sqrt( dist_2 );
        
        //charges
        {
        	const F64 ccoef = 1.0 / dist;
        	
        	potential += ( ccoef * site->GetValue( Configuration::valueType::charge ) );
        	
            if ( permFlags & 0x01 )
            {
        		potential += ( ccoef * site->GetPermValue( Configuration::valueType::charge ) );
            }
        }
        
        //dipoles
        {
        	for ( U32 j=1; j < 4; ++j )
        	{
        		Configuration::valueType type = static_cast< Configuration::valueType >( j );
        		
        		const F64 dcoef = Fitter::DelComp( type, gridPoint, pos ) / ( dist_2 * dist ); 
        		
                potential += ( dcoef * ( site->GetValue( type ) / Constants::eBohr_D ) );
        		
        		if ( permFlags & ( 1 << j ) )
                {
                    potential += ( dcoef * ( site->GetPermValue( type ) / Constants::eBohr_D ) );
                }
        	}
        }
        
        //qpol
        {
        	for ( U32 j=4; j < 9; ++j )
        	{
        		Configuration::valueType type = static_cast< Configuration::valueType >( j );
        	
				const F64 qcoef = Fitter::DelComp( type, gridPoint, pos ) / ( dist_2 * dist_2 * dist ); 
				
                potential += ( qcoef  * ( site->GetValue( type ) / Constants::eBohr2_DA ) );
				
				if ( permFlags & ( 1 << j ) )
                {
                    potential += ( qcoef  * ( site->GetPermValue( type ) / Constants::eBohr2_DA ) );
                }
			
			}
        }
    }
    
    return potential;
}

F64 Field::GetAlpha( const U32 coll, const U32 siteIndex, const Configuration &conf ) const
{
    F64 alpha = 0.0;
    
    if ( ContainsElectricField() )
    {
        const Configuration::FitSite *site = conf.GetSite( coll, siteIndex );
        const EFieldCollection &ecoll = mEFieldCollections[coll];
        const Vec3 &efield = ecoll.efieldSite[siteIndex];
        
        const F64 alpha_xx = ( site->GetValue( Configuration::valueType::dipoleX ) * Constants::D_eBohr / efield[0] ) * Constants::bohr3_nm3;
        const F64 alpha_yy = ( site->GetValue( Configuration::valueType::dipoleY ) * Constants::D_eBohr / efield[1] ) * Constants::bohr3_nm3;
        const F64 alpha_zz = ( site->GetValue( Configuration::valueType::dipoleZ ) * Constants::D_eBohr / efield[2] ) * Constants::bohr3_nm3;
        
        alpha = ( alpha_xx + alpha_yy + alpha_zz ) / 3;
    }
    
    return alpha;
}

const Vec3 & Field::GetEfield( const U32 coll, const U32 fitSite ) const
{
    const EFieldCollection &ecoll = mEFieldCollections[coll];
    
    return ecoll.efieldSite[fitSite];
}

Error::STATUS Field::SetPermField( const Configuration &conf )
{
    U32 coll = 0;
    for ( FieldCollection &collecion : mCollections  )
    {
        for ( U32 i=0; i <  collecion.fieldPositions.size(); ++i )
        {
            collecion.permPotential [ i ] = GetGridPointPotential( coll, collecion.fieldPositions[i], conf );
        }
        
        coll++;
    }
	
	return Error::STATUS::OK;
}

F64 Field::GetFieldStats( const Configuration &conf ) const
{
	F64 chiSqr = 0.0;
	
    U32 coll = 0;
    for ( const FieldCollection &collecion : mCollections  )
    {
        for ( U32 i=0; i <  collecion.fieldPositions.size(); ++i )
        {
            F64 pot = collecion.fieldPotential[i];
            F64 potCalc = GetGridPointPotential( coll, collecion.fieldPositions[i], conf );
            F64 diff = pot - potCalc;
		
            chiSqr += ( diff * diff );
        }
        
        coll++;
    }
	
	return chiSqr;
}

size_t Field::NumCollections() const
{
    return mCollections.size();
}



