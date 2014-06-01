#ifndef __MATHF_H__
#define __MATHF_H__

#include "../common/types.h"

namespace Mathf
{
    F32 GetEpsilon();
    F32 GetPi();
    F32 Get2Pi();
    F32 GetSqrt2();
    F32 GetE();
    F32 GetLog2E();
    F32 GetLog10E();
    F32 GetLn2();
    F32 GetLn10();

    F32 Acos( const F32 f );
    F32 Asin( const F32 f );
    F32 Atan( const F32 f );
    F32 Atan2( const F32 x, const F32 y );
    F32 Cos( const F32 f );
    F32 Sin( const F32 f );
    F32 Tan( const F32 f );

    F32 Exp( const F32 f );
    F32 Log( const F32 f );
    F32 Log10( const F32 f );
    F32 Pow( const F32 base, const F32 exp );
    F32 Sqrt( const F32 f );

    F32 Clamp( const F32 f, const F32 min, const F32 max );
    F32 Lerp( const F32 a, const F32 b, const F32 t );
    F32 RadToDeg( const F32 f );
    F32 DegToRad( const F32 f );
    F32 Abs( const F32 f );

    F32 Ceil( const F32 f );
    F32 Floor( const F32 f );
    F32 Round( const F32 f );

    bool IsPow2( const U32 n );
    U32 NextPow2( const U32 x );

    bool Equal( const F32 a, const F32 b );

    template< typename T >
    T GetMin( const T &a, const T &b )
    {
        return a < b ? a : b;
    }

    template< typename T >
    T GetMax( const T &a, const T &b )
    {
        return a > b ? a : b;
    }
}

#endif