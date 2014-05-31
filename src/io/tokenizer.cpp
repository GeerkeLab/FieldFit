#include "tokenizer.h"

void Tokenizer::Tokenize( const std::string &line, const std::string &delimiters )
{
	std::string::size_type position, lastPosition = 0;
	
	while( true )
	{
		position = line.find_first_of( delimiters, lastPosition );
		
		if ( position == std::string::npos )
		{
			position = line.length(); 
		}
		
		//if we have at least a bit of data in between
		if ( position != lastPosition )
		{
			mBuffer.push_back( line.substr( lastPosition, position - lastPosition ) );
		}	
		
		if ( position == line.length() )
		{
			break;	
		}
	
		lastPosition = position + 1;
	}
}

size_t Tokenizer::Size()
{
	return mBuffer.size();
}

bool Tokenizer::IsEnd()
{
	if ( mBuffer.size() > 1 )
	{
		if ( mBuffer.back().compare("END") == 0 )
		{
			mBuffer.pop_back();
			
			return true;
		}
	}
	
	return false;
}

void Tokenizer::Empty()
{
	mBuffer.clear();	
}
	
const std::vector< std::string > & Tokenizer::GetBuffer()
{
	return mBuffer;
}