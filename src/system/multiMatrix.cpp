#include "multiMatrix.h"

#include <iostream>

multiMatrix::multiMatrix( U32 rows, U32 rowSize /*= 1 */ ) : mHeight( 0 ), mWidth( 0 )
{
    mInternalMatrix.resize( rows );

    for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
        itend = mInternalMatrix.end(); it != itend; ++it )
    {
        it->resize( rowSize, 0 );
    }
}

void multiMatrix::FitIndices( U32 i, U32 j )
{
    //elegant resize
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
        for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
            itend = mInternalMatrix.end(); it != itend; ++it )
        {
            it->resize( mWidth, 0 );
        }
    }
}

F64 & multiMatrix::GetField( U32 i, U32 j )
{
    FitIndices( i, j );

    return mInternalMatrix[i][j];
}

F64 & multiMatrix::operator()( U32 i, U32 j )
{
    return GetField( i, j );
}

F64 & multiMatrix::operator()( U32 i )
{
    return GetField( i, 0 );
}

void multiMatrix::Report( const std::string& blockName )
{
    printf( (std::string("[")+blockName+"]\n").c_str() );
    
    for ( U32 i=0; i < Rows(); ++i )
    {
        for ( U32 j=0; j < Columns(); ++j )
        {
           printf( "  %12.5f", mInternalMatrix[i][j] );
        }

         printf( "\n" );
    }
    
    printf( "[END]\n" );
}

F64 * multiMatrix::GetBuffer()
{
    mBuffer.clear();
    mBuffer.reserve( mWidth * mHeight );

    U32 rows = 0;
    for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
        itend = mInternalMatrix.end(); it != itend; ++it )
    {
        //only take the correct row widths
        mBuffer.insert( mBuffer.end(), it->begin(), it->begin()+mWidth );

        if ( rows == mHeight )
        {
            break;
        }

        rows++;
    }

    return  mBuffer.data();
}

//copy back to the main mechanism
void multiMatrix::BufferSwap()
{
    U32 stride = 0;

    for ( U32 i=0; i < mHeight; ++i )
    {
        mInternalMatrix[i].assign( mBuffer.begin()+stride, mBuffer.begin()+stride+mWidth );

        stride += mWidth;
    }
}

U32 multiMatrix::Columns() const
{
    return mWidth;
}

U32 multiMatrix::Rows() const
{
    return mHeight;
}
