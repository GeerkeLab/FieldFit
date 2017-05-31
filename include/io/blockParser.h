#pragma once
#ifndef __BLOCKPARSER_H__
#define __BLOCKPARSER_H__

#include "common/types.h"

#include <string>
#include <map>
#include <vector>

namespace FieldFit
{
    class Block;
    
    /*
    **	Class to parse the blocks within a series of files
    */
    class BlockParser
    {
    public:
    	
    	BlockParser( const std::vector< std::string >  &files  );	
    	
    	const std::vector< Block > * GetBlockArray( const std::string &block ) const;
    	
        const Block * GetBlock( const std::string &block ) const;
         
        void DeleteBlock( const std::string &block );

        void Clear(); 
         
    private:	
    	
    	void ParseFile( const std::string &file );
        std::map< std::string, std::vector< Block > > mBlocks;
    };

}

#endif