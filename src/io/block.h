#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <vector>
#include <string>
#include <assert.h>
#include <iostream>

#include "../common/types.h"
#include "../util/util.h"

class Block
{
public:

	class Token
	{
	public:
		
		Token( const std::string &token) : mToken( token )
		{
		}
		
		Token& operator=(Token const &rhs) {
			
			mToken = rhs.GetToken();
			
			return *this;
		};
		
		template< class T >
		T GetValue() const
		{
			return Util::FromString< T >( mToken );
		}
		
		const std::string & GetToken() const
		{
			return mToken;	
		}
		
		bool Equals( const std::string &comp )
		{
			return 	comp.compare( mToken ) == 0 ? true : false;
		}
		
		void Debug()
		{
			std::cout << "\t" << mToken << std::endl;
		}
		
	private:
		
		std::string mToken;	
	};
	
	Block( const std::string & title, const std::vector< std::string > &buffer );
	
	size_t Size() const; 
	
	void Debug();
	
	const std::string & GetTitle() const;
	
	const Token* GetToken( U32 index ) const;
	
	std::vector< Token >::const_iterator Begin() const;
	std::vector< Token >::const_iterator End() const;
	
private:
	
	const std::string mTitle;
	std::vector< Token > mTokens;
};

#endif