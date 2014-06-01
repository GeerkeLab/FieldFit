#include "inputFile.h"

#include <iostream>

#include "../io/block.h"
#include "../io/blockParser.h"

InputFile::InputFile ( const std::string &file )
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
}