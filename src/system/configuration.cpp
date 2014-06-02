#include "configuration.h"

#include <iostream>

#include "../io/block.h"
#include "../io/blockParser.h"

#include "../util/util.h"

Configuration::Configuration( const std::string &file )
{
    BlockParser bp( file );

    mStatus = bp.GetStatus();
    if ( Error::FAILED( mStatus ) )
    {
        Error::Warn( std::cout, std::string( "file ") + file + " was not present!" );
        return;
    }                                   

    mStatus = ReadSites( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadFitSites( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadPermSites( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadValueConstraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadSymConstraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = ReadSumConstraints( bp );
    if ( Error::FAILED( mStatus ) )
    {
    	return;
    }
    
    mStatus = Error::STATUS::OK;
}

const Error::STATUS Configuration::GetStatus() const
{
    return mStatus;
}

U32 Configuration::Size() const
{
    return static_cast< U32 >( mFitSites.size() ); 
}

U32 Configuration::ValueConstr() const
{
	return static_cast< U32 >( mValueConstraints.size() );
}

U32 Configuration::SymConstr() const
{
	return static_cast< U32 >( mSymConstraints.size() );
}

U32 Configuration::SumConstr() const
{
	return static_cast< U32 >( mSumConstraints.size() );
}

U32 Configuration::RespRestr() const
{
	return static_cast< U32 >( mRespConstraints.size() );
}

U32 Configuration::UnknownID()
{	
	return 0xFFFFFF;
}

U32 Configuration::GetIndexFromID( const U32 id )
{
	std::map< U32, U32 >::iterator it = mIndices.find( id );
	
	if ( it == mIndices.end() )
	{
		return 	UnknownID();
	}
	
	return it->second;
}

bool Configuration::IdIsPresent( const U32 id )
{
	std::map< U32, U32 >::iterator it = mIndices.find( id );
	
	if ( it == mIndices.end() )
	{
		return false;
	}
	
	return true;
}

const Configuration::FitSite * Configuration::GetSite( const U32 index ) const
{
    if ( index < Size() )
    {
        return &mFitSites[ index ];
    }

    return NULL;
}

const Configuration::ValueConstraint * Configuration::GetValueConstraint( const U32 index ) const
{
    if ( index < mValueConstraints.size() )
    {
        return &mValueConstraints[ index ];
    }

    return NULL;
}

const Configuration::SymConstraint * Configuration::GetSymConstraint( const U32 index ) const
{
    if ( index < mSymConstraints.size() )
    {
        return &mSymConstraints[ index ];
    }

    return NULL;
}

const Configuration::SumConstraint * Configuration::GetSumConstraint( const U32 index ) const
{
    if ( index < mSumConstraints.size() )
    {
        return &mSumConstraints[ index ];
    }

    return NULL;
}

const Configuration::RespRestraint * Configuration::GetRespRestraint( const U32 index ) const
{
    if ( index < mRespConstraints.size() )
    {
        return &mRespConstraints[ index ];
    }

    return NULL;
}

Configuration::FitSite * Configuration::GetSiteMod( const U32 index )
{
    return &mFitSites[ index ];
}


F32 Configuration::FitSite::GetValue( const valueType type ) const
{
    return values[ static_cast<U32>( type ) ];
}

void Configuration::FitSite::SetValue( const valueType type, const F32 val )
{
    values[ static_cast<U32>( type ) ] = val;
}

F32 Configuration::FitSite::GetPermValue( const valueType type ) const
{
    return permValues[ static_cast<U32>( type ) ];
}

void Configuration::FitSite::SetPermValue( const valueType type, const F32 val )
{
    permValues[ static_cast<U32>( type ) ] = val;
}

void Configuration::FitSite::Raport( std::ostream &stream )
{
	std::string flagString;
    for ( U32 j=0; j < 9; j++ )
    {
         if ( fitFlags & ( 1 << j ) )
             flagString += "1";
         else
            flagString += "0";
    }
    
    std::string permString;
    for ( U32 j=0; j < 9; j++ )
    {
         if ( permFlags & ( 1 << j ) )
             permString += "1";
         else
            permString += "0";
    }
	
    stream << "\t " << ID << " \t " << name << " \t " << atomicCharge << " \t " << position.GetX() 
           << "\t " << position.GetY() << " \t " << position.GetZ() << std::endl;  
    
    stream << " \t " << flagString << " \t ";
    for ( U32 i=0; i < 9; ++i )
    	stream << values[i] << " \t ";
    stream << std::endl;   
    
    stream << " \t " << permString << " \t ";
    for ( U32 i=0; i < 9; ++i )
    	stream << permValues[i] << " \t ";
    stream << std::endl;
    
    stream << std::endl;
}

void Configuration::Raport( std::ostream &stream )
{
	stream << "[RUNINPUT]" << std::endl;
	stream << "#\t ID \t Atomic Charge \t Name \t coordX \t coordY \t coordZ" << std::endl;
	stream << "#\t 1) Fit values; " << std::endl;   
    stream << "#\t 2) Permanent values; " << std::endl;   
    stream << "#\t 3) Value constraints; " << std::endl; 
    stream << "#\t Flags,  Charge, DipoleX,  DipoleY,  DipoleZ,   Qd20,  Qd21c,  Qd21s,  Qd22c,  Qd22s " << std::endl;
	for ( U32 i=0; i < mFitSites.size(); ++i )
	{
		mFitSites[i].Raport( stream );
	}
	
	stream << "[END]" << std::endl;
}

Error::STATUS Configuration::ReadSites( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[SITES]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [SITES] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [SITES] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 sites = block->GetToken( 0 )->GetValue< U32 >();

    if ( sites < 1 )
    {
        Error::Warn( std::cout, "At least 1 site was expected!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() != ( 1 + ( sites * 6 ) ) )
    {
        Error::Warn( std::cout, "block [SITES] did not have the right amount of arguments based on the size indicator !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 index = 1;

    for ( U32 i=0; i < sites; ++i )
    {
    	U32 id = block->GetToken( index )->GetValue< U32 >();
    	
        std::string name = block->GetToken( index+1 )->GetToken();
        
        F32 acharge = block->GetToken( index+2 )->GetValue< F32 >();

        F32 x = block->GetToken( index+3 )->GetValue< F32 >();
        F32 y = block->GetToken( index+4 )->GetValue< F32 >();
        F32 z = block->GetToken( index+5 )->GetValue< F32 >();
        
        if ( mIndices.find( id ) != mIndices.end() )
        {
        	Error::Warn( std::cout, "[SITES] contains a duplicate site ID!" );
        	return 	Error::STATUS::FAILED_IO;
        }
        
        mFitSites.push_back( FitSite( id, name, acharge, x, y, z ) );
        mIndices.insert( std::pair< U32, U32 >( id,  mFitSites.size() - 1 ) );
        	
        index += 6;
    } 
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadFitSites( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[FITSITES]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [FITSITES] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [FITSITES] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 fitSites = block->GetToken( 0 )->GetValue< U32 >();

    if ( fitSites < 1 )
    {
        Error::Warn( std::cout, "At least 1 fit site was expected!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() != ( 1 + ( fitSites * 2 ) ) )
    {
        Error::Warn( std::cout, "block [FITSITES] did not have the right amount of arguments based on the size indicator !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 index = 1;
    
    for ( U32 i=0; i < fitSites; ++i )
    {
    	
    	U32 id = block->GetToken( index )->GetValue< U32 >();
    	std::string fitflags = block->GetToken( index+1 )->GetToken();

        U32 convFlags = 0;
        for ( U32 j=0; j < fitflags.size(); ++j )
        {
            char flag = fitflags[j];

            if ( flag == '1' )
            {
                convFlags += ( 1 << j );
            }
        }
        
        //find the id 
        std::map< U32, U32 >::iterator it = mIndices.find( id );
        
        if ( it == mIndices.end() )
        {
        	Error::Warn( std::cout, "block [fitsites] contains an index ( "+Util::ToString( id )+
        		                    " ) that was not present in the sites definition!" );
        	return Error::STATUS::FAILED_IO;
        }
        
        mFitSites[ it->second ].fitFlags = convFlags;
        
    	index += 2;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadPermSites( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[PERMSITES]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [PERMSITES] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [PERMSITES] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 permSites = block->GetToken( 0 )->GetValue< U32 >();

    if ( block->Size() != ( 1 + ( permSites * 11 ) ) )
    {
        Error::Warn( std::cout, "block [PERMSITES] did not have the right amount of arguments based on the size indicator !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 index = 1;
    
    for ( U32 i=0; i < permSites; ++i )
    {
    	
    	U32 id = block->GetToken( index )->GetValue< U32 >();
    	std::string fitflags = block->GetToken( index+1 )->GetToken();

        U32 convFlags = 0;
        for ( U32 j=0; j < fitflags.size(); ++j )
        {
            char flag = fitflags[j];

            if ( flag == '1' )
            {
                convFlags += ( 1 << j );
            }
        }
        
        //find the id 
        std::map< U32, U32 >::iterator it = mIndices.find( id );
        
        if ( it == mIndices.end() )
        {
        	Error::Warn( std::cout, "block [fitsites] contains an index ( "+Util::ToString( id )+
        		                    " ) that was not present in the sites definition!" );
        	return Error::STATUS::FAILED_IO;
        }
        
        mFitSites[ it->second ].permFlags = convFlags;
        
        for ( U32 j=0; j <  9; ++j )
        {
        	F32 val = block->GetToken( index+2+j )->GetValue< U32 >();	
        	
        	mFitSites[ it->second ].SetPermValue( static_cast< valueType >( j ), val );
        }
        
    	index += 11;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadValueConstraints( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[VALUECONSTR]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [VALUECONSTR] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [VALUECONSTR] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 constraints = block->GetToken( 0 )->GetValue< U32 >();

    if ( block->Size() != ( 1 + ( constraints * 11 ) ) )
    {
        Error::Warn( std::cout, "block [VALUECONSTR] did not have the right amount of arguments based on the size indicator !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 index = 1;
	
    for ( U32 i=0; i < constraints; ++i )
    {
    	U32 id = block->GetToken( index )->GetValue< U32 >();
    	
    	std::string fitflags = block->GetToken( index+1 )->GetToken();
        U32 convFlags = 0;
        for ( U32 j=0; j < fitflags.size(); ++j )
        {
            char flag = fitflags[j];

            if ( flag == '1' )
            {
                convFlags += ( 1 << j );
            }
        }
        
        for ( U32 j=0; j <  9; ++j )
        {
        	F32 val = block->GetToken( index+2+j )->GetValue< U32 >();	
        	
        	if ( convFlags & ( 1 << j ) )
        	{
        		mValueConstraints.push_back( ValueConstraint( static_cast< valueType >( j ), id, val ) );	
        	}
        }

    	index += 11;
    }
    
    return Error::STATUS::OK;
}


Error::STATUS Configuration::ReadSymConstraints( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[SYMCONSTR]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [SYMCONSTR] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [SYMCONSTR] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 constraints = block->GetToken( 0 )->GetValue< U32 >();

    U32 index = 1;
	
    for ( U32 i=0; i < constraints; ++i )
    {
    	//read the amount of indices
    	if ( index >= block->Size() )
    	{
    		Error::Warn( std::cout, "block [SYMCONSTR] was too small 1!" );
    		return Error::STATUS::FAILED_IO;
    	}
    	
    	U32 indices = block->GetToken( index )->GetValue< U32 >();
    	
    	if ( indices < 2 )
    	{
    		Error::Warn( std::cout, "at least two indices expected in a sym constraint!" );
    		return Error::STATUS::FAILED_IO;
    	}
    	
    	if ( ( index+2+indices) > block->Size() )
    	{
    		Error::Warn( std::cout, "block [SYMCONSTR] was too small 2!" );
    		return Error::STATUS::FAILED_IO;
    	}
    	
    	std::vector< U32 > ind;
    	for ( U32 j=0 ; j < indices; ++j )
    	{
    		ind.push_back( block->GetToken( index+1+j )->GetValue< U32 >() );
    	}
    	
    	std::string fitflags = block->GetToken( index+1+(indices-1) )->GetToken();
        U32 convFlags = 0;
        for ( U32 j=0; j < fitflags.size(); ++j )
        {
            char flag = fitflags[j];

            if ( flag == '1' )
            {
                convFlags += ( 1 << j );
            }
        }
    	
        //add the constraints pairwise
    	for ( U32 j=1 ; j < indices; ++j )
    	{
    		U32 id1 = ind[j-1];
    		U32 id2 = ind[j];
    		
    		for ( U32 n=0; n < 9; ++n )
    		{
    			if ( convFlags & ( 1 << n ) )
    				mSymConstraints.push_back( SymConstraint( static_cast< valueType >( n ), id1, id2 ) );
    		}
    	}
    	
    	index += ( 2 + indices );
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadSumConstraints( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[SUMCONSTR]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [SUMCONSTR] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [SUMCONSTR] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 constraints = block->GetToken( 0 )->GetValue< U32 >();

    U32 index = 1;
	
    for ( U32 i=0; i < constraints; ++i )
    {
    	//read the amount of indices
    	if ( index >= block->Size() )
    	{
    		Error::Warn( std::cout, "block [SUMCONSTR] was too small!" );
    		return Error::STATUS::FAILED_IO;
    	}
    	
    	U32 indices = block->GetToken( index )->GetValue< U32 >();
    	
    	if ( indices < 2 )
    	{
    		Error::Warn( std::cout, "at least two indices expected in a sym constraint!" );
    		return Error::STATUS::FAILED_IO;
    	}
    	
    	if ( ( index+2+(indices*2)) > block->Size() )
    	{
    		Error::Warn( std::cout, "block [SUMCONSTR] was too small!" );
    		return Error::STATUS::FAILED_IO;
    	}
    	
    	std::vector< std::pair< U32, valueType > > ind;
    	for ( U32 j=0 ; j < indices; ++j )
    	{
    		U32 id = block->GetToken( index )->GetValue< U32 >();
    		
    		std::string fitflags = block->GetToken( index+1 )->GetToken();
			U32 convFlags = 0;
			for ( U32 j=0; j < fitflags.size(); ++j )
			{
				char flag = fitflags[j];
	
				if ( flag == '1' )
				{
					convFlags += ( 1 << j );
				}
			}
			
			for ( U32 n=0; n < 9; ++n )
    		{
    			if ( convFlags & ( 1 << n ) )
    				ind.push_back( std::pair< U32, valueType >( id, static_cast< valueType >( n ) ) );
    		}
    		
    		index += 2;
    	}
    	
    	F32 value = block->GetToken( index )->GetValue< F32 >();
    	
        mSumConstraints.push_back( SumConstraint( ind, value ) );
    	
    	index += 1;
    }
    
    return Error::STATUS::OK;
}

Error::STATUS Configuration::ReadRespRestraints( const BlockParser &bp )
{
	const Block *block = bp.GetBlock("[RESP]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [RESP] was not present!" );
        return Error::STATUS::FAILED_IO;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [RESP] was too small ( at least 1 argument expected ) !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 constraints = block->GetToken( 0 )->GetValue< U32 >();

    if ( block->Size() != ( 1 + ( constraints * 11 ) ) )
    {
        Error::Warn( std::cout, "block [RESP] did not have the right amount of arguments based on the size indicator !" );
        return Error::STATUS::FAILED_IO;
    }

    U32 index = 1;
	
    for ( U32 i=0; i < constraints; ++i )
    {
    	U32 id = block->GetToken( index )->GetValue< U32 >();
    	
    	std::string fitflags = block->GetToken( index+1 )->GetToken();
        U32 convFlags = 0;
        for ( U32 j=0; j < fitflags.size(); ++j )
        {
            char flag = fitflags[j];

            if ( flag == '1' )
            {
                convFlags += ( 1 << j );
            }
        }
        
        for ( U32 j=0; j <  9; ++j )
        {
        	F32 val = block->GetToken( index+2+j )->GetValue< U32 >();	
        	
        	if ( convFlags & ( 1 << j ) )
        	{
        		mRespConstraints.push_back( RespRestraint( static_cast< valueType >( j ), id, val ) );	
        	}
        }

    	index += 11;
    }
    
    return Error::STATUS::OK;
}



