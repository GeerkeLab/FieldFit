#include "vec3i.h"
#include "vec3.h"

#include <assert.h>

Vec3I::Vec3I()
{
    mValues[0] = 0;
    mValues[1] = 0;
    mValues[2] = 0;
}

Vec3I::Vec3I( const Vec3 &v )
{
    SetValue( ( S32 )v.GetX(), ( S32 )v.GetY(), ( S32 )v.GetZ() );
}

Vec3I::Vec3I( const S32 x, const S32 y, const S32 z )
{
    SetValue( x, y, z );
}

Vec3I::Vec3I( F32 x, F32 y, F32 z )
{
    SetValue( ( S32 )x, ( S32 )y, ( S32 )z );
}

S32 &Vec3I::operator[]( const U8 axis )
{
    return mValues[ axis ];
}

const S32 &Vec3I::operator[]( const U8 axis ) const
{
    return mValues[ axis ];
}

void Vec3I::SetX( const S32 val )
{
    mValues[0] = val;
}

S32 Vec3I::GetX() const
{
    return mValues[0];
}

void Vec3I::SetY( const S32 val )
{
    mValues[1] = val;
}

S32 Vec3I::GetY() const
{
    return mValues[1];
}

void Vec3I::SetZ( const S32 val )
{
    mValues[2] = val;
}

S32 Vec3I::GetZ() const
{
    return mValues[2];
}

void Vec3I::SetValue( S32 x, S32 y, S32 z )
{
    mValues[0] = x;
    mValues[1] = y;
    mValues[2] = z;
}

Vec3 Vec3I::ToVec() const
{
    return Vec3( ( F32 )mValues[0], ( F32 )mValues[1], ( F32 )mValues[2] );
}

void Vec3I::Clear()
{
    SetValue( 0, 0, 0 );
}

Vec3I operator+( const Vec3I &v1, const Vec3I &v2 )
{
    return Vec3I( v1.mValues[0] + v2.mValues[0],
        v1.mValues[1] + v2.mValues[1],
        v1.mValues[2] + v2.mValues[2] );
}

Vec3I operator*( const Vec3I &v1, const Vec3I &v2 )
{
    return Vec3I( v1.mValues[0] * v2.mValues[0],
        v1.mValues[1] * v2.mValues[1],
        v1.mValues[2] * v2.mValues[2] );
}

Vec3I operator-( const Vec3I &v1, const Vec3I &v2 )
{
    return Vec3I( v1.mValues[0] - v2.mValues[0],
        v1.mValues[1] - v2.mValues[1],
        v1.mValues[2] - v2.mValues[2] );
}

Vec3I operator-( const Vec3I &v )
{
    return Vec3I( -v.mValues[0], -v.mValues[1], -v.mValues[2] );
}

Vec3I operator*( const Vec3I &v, const F32 s )
{
    return Vec3I( v.mValues[0] * s, v.mValues[1] * s, v.mValues[2] * s );
}

Vec3I operator*( F32 s, const Vec3I &v )
{
    return v * s;
}

Vec3I operator/( const Vec3I &v, F32 s )
{
    assert( s != 0.0f );
    return v * ( F32( 1.0 ) / s );
}

Vec3I operator/( const Vec3I &v1, const Vec3I &v2 )
{
    return Vec3I( v1.mValues[0] / v2.mValues[0],
        v1.mValues[1] / v2.mValues[1] ,
        v1.mValues[2] / v2.mValues[2] );
}