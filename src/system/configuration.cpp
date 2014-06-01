#include "configuration.h"

#include <iostream>

#include "../io/block.h"
#include "../io/blockParser.h"

Configuration::Configuration( const std::string &file )
{
    BlockParser bp( file );

    if ( Error::FAILED( bp.GetStatus() ) )
    {
        Error::Warn( std::cout, std::string( "file ") + file + " was not present!" );
        mStatus = bp.GetStatus();
        return;
    }

    const Block *block = bp.GetBlock("[FITSITES]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [FITSITES] was not present!" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    if ( block->Size() < 1 )
    {
        Error::Warn( std::cout, "block [FITSITES] was too small ( at least 1 argument expected ) !" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    U32 fitSites = block->GetToken( 0 )->GetValue< U32 >();

    if ( fitSites < 1 )
    {
        Error::Warn( std::cout, "At least 1 fit site was expected!" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    if ( block->Size() != ( 1 + ( fitSites * 6 ) ) )
    {
        Error::Warn( std::cout, "block [FITSITES] did not have the right amount of arguments based on the size indicator !" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    U32 index = 1;

    for ( U32 i=0; i < fitSites; ++i )
    {
        std::string name = block->GetToken( index )->GetToken();
        
        F32 acharge = block->GetToken( index+1 )->GetValue< F32 >();

        F32 x = block->GetToken( index+2 )->GetValue< F32 >();
        F32 y = block->GetToken( index+3 )->GetValue< F32 >();
        F32 z = block->GetToken( index+4 )->GetValue< F32 >();

        std::string fitflags = block->GetToken( index+5 )->GetToken();

        U32 convFlags = 0;
        for ( U32 j=0; j < fitflags.size(); ++j )
        {
            char flag = fitflags[j];

            if ( flag == '1' )
            {
                convFlags += ( 1 << j );
            }
        }

        mFitSites.push_back( FitSite( name, acharge, x, y, z, convFlags ) );

        index += 6;
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

const Configuration::FitSite * Configuration::GetSite( const U32 index ) const
{
    if ( index < Size() )
    {
        return &mFitSites[ index ];
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
