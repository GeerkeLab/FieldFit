#include "blockParser.h"
#include "block.h"
#include "../util/util.h"
#include "tokenizer.h"

#include <fstream>
#include <assert.h>
#include <iostream>

BlockParser::BlockParser( const std::string &file ) 
{
	mStatus = ParseFile( file );
}
	
const Block * BlockParser::GetBlock( const std::string &block ) const
{
	std::map< std::string, Block >::const_iterator it = mBlocks.find( block );
	
	if ( it == mBlocks.end() )
	{
		return NULL;	
	}
	
	return &it->second;
}
	
const Error::STATUS BlockParser::GetStatus() const
{
	return mStatus;
}

void BlockParser::Debug()
{
	for ( std::map< std::string, Block >::iterator it = mBlocks.begin(), itend = mBlocks.end();
		  it != itend; ++it )
	{
		it->second.Debug();	
	}
}

Error::STATUS BlockParser::ParseFile( const std::string &file )
{
	std::ifstream stream;
	
	stream.open( file.c_str(), std::ios::in );
	
	if ( !stream.is_open() )
	{
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
    			mBlocks.insert( std::pair< std::string, Block >( title, Block( title, tn.GetBuffer() ) ) );	
    			
    			tn.Empty();
    			title = "";
    		}
    	}
    }
	
    if ( tn.IsEnd() && tn.Size() > 0 && title.size() > 0 )
    {
    	mBlocks.insert( std::pair< std::string, Block >( title, Block( title, tn.GetBuffer() ) ) );	
    }
    
	stream.close();
	
	return Error::STATUS::OK;
}