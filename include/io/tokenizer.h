#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <vector>
#include <string>

namespace FieldFit
{
    /*
    **	Class to transform a string containing whitespace, tabs and tokens to a vector of tokens;
    */
    class Tokenizer
    {
    public:
    	
    	void Tokenize( const std::string &line, const std::string &delimiters );
    	void Empty();
    	
    	bool IsEnd();
    	
    	size_t Size();
    	
    	const std::vector< std::string > & GetBuffer();
    	
    private:
    		
    	std::vector< std::string > mBuffer;
    };

}

#endif