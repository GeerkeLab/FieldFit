#include "multiMatrix.h"

multiMatrix::multiMatrix( U32 rows, U32 rowSize /*= 1 */ ) : mHeight( 0 ), mWidth( 0 )
{
    mInternalMatrix.resize( rows );

    for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
        itend = mInternalMatrix.end(); it != itend; ++it )
    {
        it->resize( rowSize, 0 );
    }
}

F64 & multiMatrix::GetField( U32 i, U32 j )
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
        for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
            itend = mInternalMatrix.end(); it != itend; ++it )
        {
            it->resize( mWidth, 0 );
        }
    }

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

void multiMatrix::Debug( std::ostream &stream )
{
    for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
        itend = mInternalMatrix.end(); it != itend; ++it )
    {
        for ( std::vector< F64 >::iterator itj = it->begin(),
            itendj = it->end(); itj != itendj; ++itj )
        {
            stream << "\t" << *itj;
        }

        stream << std::endl;
    }
}

F64 * multiMatrix::GetBuffer()
{
    mBuffer.clear();
    mBuffer.reserve( mWidth * mHeight );

    for ( std::vector< std::vector< F64 > >::iterator it = mInternalMatrix.begin(),
        itend = mInternalMatrix.end(); it != itend; ++it )
    {
        mBuffer.insert( mBuffer.end(), it->begin(), it->end() );
    }

    return  mBuffer.data();
}

U32 multiMatrix::Columns() const
{
    return mWidth;
}

U32 multiMatrix::Rows() const
{
    return mHeight;
}
