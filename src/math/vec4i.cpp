#include "vec4i.h"
#include "vec4.h"

#include <assert.h>


Vec4I::Vec4I()
{
    mValues[0] = 0;
    mValues[1] = 0;
    mValues[2] = 0;
    mValues[3] = 0;
}

Vec4I::Vec4I( const Vec4 &v )
{
    SetValue( ( S32 )v.GetX(), ( S32 )v.GetY(), ( S32 )v.GetZ(), ( S32 )v.GetW() );
}

Vec4I::Vec4I( const S32 &x, const S32 &y, const S32 &z, const S32 &w )
{
    SetValue( x, y, z, w );
}

Vec4I::Vec4I( F32 x, F32 y, F32 z, F32 w )
{
    SetValue( ( S32 )x, ( S32 )y, ( S32 )z, ( S32 )w );
}

S32 &Vec4I::operator[]( const size_t axis )
{
    return mValues[ axis ];
}

const S32 &Vec4I::operator[]( const size_t axis ) const
{
    return mValues[ axis ];
}

S32 Vec4I::GetX() const
{
    return mValues[0];
}
void Vec4I::SetX( const S32 &val )
{
    mValues[0] = val;
}

S32 Vec4I::GetY() const
{
    return mValues[1];
}

void Vec4I::SetY( const S32 &val )
{
    mValues[1] = val;
}

S32 Vec4I::GetZ() const
{
    return mValues[2];
}

void Vec4I::SetZ( const S32 &val )
{
    mValues[2] = val;
}

S32 Vec4I::GetW() const
{
    return mValues[3];
}

void Vec4I::SetW( const S32 &val )
{
    mValues[3] = val;
}

void Vec4I::SetValue( const S32 &x, const S32 &y, const S32 &z, const S32 &w )
{
    mValues[0] = x;
    mValues[1] = y;
    mValues[2] = z;
    mValues[3] = w;
}

Vec4 Vec4I::ToVec() const
{
    return Vec4( ( F32 )mValues[0], ( F32 )mValues[1], ( F32 )mValues[2], ( F32 )mValues[3] );
}

void Vec4I::Clear()
{
    SetValue( 0, 0, 0, 0 );
}

Vec4I operator+( const Vec4I &v1, const Vec4I &v2 )
{
    return Vec4I( v1.mValues[0] + v2.mValues[0],
        v1.mValues[1] + v2.mValues[1],
        v1.mValues[2] + v2.mValues[2],
        v1.mValues[3] + v2.mValues[3] );
}

Vec4I operator*( const Vec4I &v1, const Vec4I &v2 )
{
    return Vec4I( v1.mValues[0] * v2.mValues[0],
        v1.mValues[1] * v2.mValues[1],
        v1.mValues[2] * v2.mValues[2],
        v1.mValues[3] * v2.mValues[3] );
}

Vec4I operator-( const Vec4I &v1, const Vec4I &v2 )
{
    return Vec4I( v1.mValues[0] - v2.mValues[0],
        v1.mValues[1] - v2.mValues[1],
        v1.mValues[2] - v2.mValues[2],
        v1.mValues[3] - v2.mValues[3] );
}

Vec4I operator-( const Vec4I &v )
{
    return Vec4I( -v.mValues[0], -v.mValues[1], -v.mValues[2], -v.mValues[3] );
}

Vec4I operator*( const Vec4I &v, const F32 s )
{
    return Vec4I( v.mValues[0] * s, v.mValues[1] * s, v.mValues[2] * s, v.mValues[3] * s );
}

Vec4I operator*( const F32 s, const Vec4I &v )
{
    return v * s;
}

Vec4I operator/( const Vec4I &v, const F32 s )
{
    assert( s != 0.0f );

    return v * ( F32( 1.0 ) / s );
}

Vec4I operator/( const Vec4I &v1, const Vec4I &v2 )
{
    return Vec4I( v1.mValues[0] / v2.mValues[0],
        v1.mValues[1] / v2.mValues[1],
        v1.mValues[2] / v2.mValues[2],
        v1.mValues[3] / v2.mValues[3] );
}
