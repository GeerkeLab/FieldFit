#ifndef __BLOCKPARSER_H__
#define __BLOCKPARSER_H__

#include <string>
#include <map>
#include <vector>

#include "../error/error.h"
#include "../common/types.h"

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
    
	Error::STATUS GetStatus() const;
	      
private:	
	
	Error::STATUS ParseFile( const std::string &file );
	
	Error::STATUS mStatus;
    std::map< std::string, std::vector< Block > > mBlocks;
};

#endif