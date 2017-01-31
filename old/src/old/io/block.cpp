#include "block.h"

#include <assert.h>

Block::Block( const std::string & title, const std::vector< std::string > &buffer ) : mTitle( title )
{
	mTokens.reserve( buffer.size() );
	
	for ( std::vector< std::string >::const_iterator it = buffer.begin(), itend = buffer.end();
		it != itend; ++it )
	{
		mTokens.push_back( Token( *it ) );
	}
}

void Block::Debug()
{
	std::cout << mTitle << std::endl;
	
	for ( std::vector< Token >::iterator it = mTokens.begin(), itend = mTokens.end();
		  it != itend; ++it )
	{
		it->Debug();	
	}
	
	std::cout << "[END]" << std::endl;
}

size_t Block::Size() const
{
	return mTokens.size();	
}

const std::string & Block::GetTitle() const
{
	return mTitle;
}

std::vector< Block::Token >::const_iterator Block::Begin() const
{
	return 	mTokens.begin();
}

std::vector< Block::Token >::const_iterator Block::End() const
{
	return mTokens.end();
}

const Block::Token* Block::GetToken( U32 index ) const
{
	assert ( index < mTokens.size() );
	
	return &mTokens[ index ];
}