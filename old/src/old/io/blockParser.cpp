#include "blockParser.h"
#include "block.h"
#include "../util/util.h"
#include "tokenizer.h"

#include <fstream>
#include <assert.h>
#include <iostream>

BlockParser::BlockParser( const std::vector< std::string > &files ) 
{
	for( U32 i=0; i < files.size(); ++i )
	{
		mStatus = ParseFile( files[i] );
		
		if ( Error::FAILED( mStatus ) )
			return;
	}
}
	
const std::vector< Block > * BlockParser::GetBlockArray( const std::string &block ) const
{
	std::map< std::string, std::vector< Block > >::const_iterator it = mBlocks.find( block );
	
	if ( it == mBlocks.end() )
	{
		return nullptr;
	}
	
	return &it->second;
}

const Block * BlockParser::GetBlock( const std::string &block ) const
{
    std::map< std::string, std::vector< Block > >::const_iterator it = mBlocks.find( block );
    
    if ( it == mBlocks.end() )
    {
        return nullptr;
    }
    
    const std::vector< Block > &blockArray = it->second;
    
    // Now perform size test
    if ( blockArray.size() != 1 )
    {
        Error::Warn( std::cout, "A call to GetBlock suggests that only one block of requested type" + block +" should exist.");
        
        return nullptr;
    }
    
    return blockArray.data();
}
	
Error::STATUS BlockParser::GetStatus() const
{
	return mStatus;
}

Error::STATUS BlockParser::ParseFile( const std::string &file )
{
	std::ifstream stream;
	
	stream.open( file.c_str(), std::ios::in );
	
	if ( !stream.is_open() )
	{
		Error::Warn( std::cout, "Unable to open file "+file+" !" );
		return Error::STATUS::FAILED_IO;
	}
	
    std::string line, title = "";
    Tokenizer tn;
	while ( getline ( stream, line ) )
    {
    	line = Util::Trim( line );
    	
    	if ( line.size() > 0 )
    	{
    		//test for comment
    		if ( line[0] == '#' )
    		{
    			continue;	
    		}
    		
    		if ( title.size() == 0 )
    		{
    			title = line;
    			
    			continue;
    		}
    		
    		tn.Tokenize( line, " \t;" );
    		
    		if ( tn.IsEnd() )
    		{
                std::map< std::string, std::vector< Block > >::const_iterator it = mBlocks.find( title );
                
                if ( it == mBlocks.end() )
                {
                    mBlocks.insert( std::pair< std::string, std::vector< Block > >( title, std::vector< Block >() ) );
                }
                
                mBlocks[title].push_back( Block( title, tn.GetBuffer() ) );
    			
    			tn.Empty();
    			title = "";
    		}
    	}
    }
	
    // final flush
    if ( tn.IsEnd() && tn.Size() > 0 && title.size() > 0 )
    {
        std::map< std::string, std::vector< Block > >::const_iterator it = mBlocks.find( title );
        
        if ( it == mBlocks.end() )
        {
            mBlocks.insert( std::pair< std::string, std::vector< Block > >( title, std::vector< Block >() ) );
        }
        
        mBlocks[title].push_back( Block( title, tn.GetBuffer() ) );
    }
    
	stream.close();
	
	return Error::STATUS::OK;
}