#include "field.h"

#include <iostream>

#include "../io/block.h"
#include "../io/blockParser.h"



Field::Field( const std::string &file )
{
    BlockParser bp( file );

    if ( Error::FAILED( bp.GetStatus() ) )
    {
        Error::Warn( std::cout, std::string( "file ") + file + " was not present!" );
        mStatus = bp.GetStatus();
        return;
    }

    const Block *block = bp.GetBlock("[FIELD]");

    if ( !block )
    {
        Error::Warn( std::cout, "block [FIELD] was not present!" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    if ( block->Size() < 2 )
    {
        Error::Warn( std::cout, "block [FIELD] was too small ( at least 2 arguments expected ) !" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    U32 genSites = block->GetToken( 0 )->GetValue< U32 >();
    U32 potentialSites = block->GetToken( 1 )->GetValue< U32 >();

    //test for the correct size
    if ( block->Size() != ( 2 + ( genSites * 3 ) + ( potentialSites * 4 ) ) )
    {
        Error::Warn( std::cout, "block [FIELD] did not have the right amount of arguments based on the size indicators !" );
        mStatus =  Error::STATUS::FAILED_IO;
        return;
    }

    U32 index = 2;

    for ( U32 i=0; i < genSites; ++i )
    {
        F32 x = block->GetToken( index )->GetValue< F32 >();
        F32 y = block->GetToken( index+1 )->GetValue< F32 >();
        F32 z = block->GetToken( index+2 )->GetValue< F32 >();

        mGenerationSites.push_back( Vec3( x, y, z ) );

        index += 3;
    }

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

    mStatus = Error::STATUS::OK;
}

const Error::STATUS Field::GetStatus() const
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

const F64 Field::GetFieldPotential( const U32 index ) const
{
    return mFieldPotential[ index ];
}



