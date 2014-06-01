#ifndef __FITMATRIX_H__
#define __FITMATRIX_H__

#include <vector>
#include <fstream>
#include <stdio.h>

template< class T >
class multiMatrix
{
public:

    multiMatrix( U32 rowSize , U32 rows ) : mHeight( 0 ), mWidth( 0 )
    {
        mInternalMatrix.resize( rows );

        for ( std::vector< std::vector< T > >::iterator it = mInternalMatrix.begin(),
            itend = mInternalMatrix.end(); it != itend; ++it )
        {
            it->resize( rowSize, 0 );
        }
    }

    T & operator()( U32 i, U32 j )
    {
        bool resizeRows = false;
        
        if ( j >= mWidth )
        {
            mWidth = j+1;

            resizeRows = true;
        }

        if ( i >= mHeight )
        {
            mHeight = i+1;
            mInternalMatrix.resize( mHeight );

            resizeRows = true;
        }

        if ( resizeRows )
        {
            for ( std::vector< std::vector< T > >::iterator it = mInternalMatrix.begin(),
                itend = mInternalMatrix.end(); it != itend; ++it )
            {
                it->resize( mWidth, 0 );
            }
        }

        return mInternalMatrix[i][j];
    }

    void Debug( std::ostream &stream )
    {
        for ( std::vector< std::vector< T > >::iterator it = mInternalMatrix.begin(),
            itend = mInternalMatrix.end(); it != itend; ++it )
        {
            for ( std::vector< T >::iterator itj = it->begin(),
                itendj = it->end(); itj != itendj; ++itj )
            {
                stream << "\t" << *itj;
            }

            stream << std::endl;
        }
    }

    //ugly old c-style for double formatting
    void Debug( FILE *stream )
    {
        for ( std::vector< std::vector< T > >::iterator it = mInternalMatrix.begin(),
            itend = mInternalMatrix.end(); it != itend; ++it )
        {
            for ( std::vector< T >::iterator itj = it->begin(),
                itendj = it->end(); itj != itendj; ++itj )
            {
                 fprintf (stream, " %15.7f", *itj );
            }

            fprintf (stream, "\n" );
        }
    }

    T * GetBuffer()
    {
        mBuffer.clear();
        mBuffer.reserve( mWidth * mHeight );

        for ( std::vector< std::vector< T > >::iterator it = mInternalMatrix.begin(),
            itend = mInternalMatrix.end(); it != itend; ++it )
        {
            mBuffer.insert( mBuffer.end(), it->begin(), it->end() );
        }

        return  mBuffer.data();
    }

private:

    U32 mWidth, mHeight;

    std::vector< std::vector< T > > mInternalMatrix;

    std::vector< T > mBuffer;
};

#endif