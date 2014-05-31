#ifndef __BLOCKPARSER_H__
#define __BLOCKPARSER_H__

#include <string>
#include <map>
#include <vector>

#include "../error/error.h"
#include "../common/types.h"

class Block;

class BlockParser
{
public:
	
	BlockParser( const std::string &file  );	
	
	const Block * GetBlock( const std::string &block ) const;
	
	const Error::STATUS GetStatus() const;
	      
	void Debug();
	
private:	
	
	Error::STATUS ParseFile( const std::string &file );
	
	Error::STATUS mStatus;
	std::map< std::string, Block > mBlocks;
};

#endif