#ifndef __FITMATRIX_H__
#define __FITMATRIX_H__

#include <vector>
#include <fstream>
#include <stdio.h>

#include "../common/types.h"

class multiMatrix
{
public:

    multiMatrix( U32 rows, U32 rowSize = 1 );

    F64 & GetField( U32 i, U32 j );

    F64 & operator()( U32 i, U32 j );

    F64 & operator()( U32 i );  

    void Debug( std::ostream &stream );

    F64 * GetBuffer();

    U32 Columns() const;

    U32 Rows() const;

    void BufferSwap();

private:

    void FitIndices( U32 i, U32 j = 0 );

    U32 mWidth, mHeight;

    std::vector< std::vector< F64 > > mInternalMatrix;

    std::vector< F64 > mBuffer;
};

#endif