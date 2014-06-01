#include "vec2i.h"
#include "vec2.h"

#include <assert.h>

Vec2I::Vec2I()
{
}

Vec2I::Vec2I( const Vec2 &v )
{
    SetValue( ( S32 )v.GetX(), ( S32 )v.GetY() );
}

Vec2I::Vec2I( const S32 x, const S32 y )
{
    SetValue( x, y );
}

Vec2I::Vec2I( const F32 x, const F32 y )
{
    SetValue( ( S32 )x, ( S32 )y );
}

S32 &Vec2I::operator[]( const U8 axis )
{
    return mValues[ axis ];
}

const S32 &Vec2I::operator[]( const U8 axis ) const
{
    return mValues[ axis ];
}

void Vec2I::SetX( const S32 &val )
{
    mValues[0] = val;
}

S32 Vec2I::GetX() const
{
    return mValues[0];
}

void Vec2I::SetY( const S32 &val )
{
    mValues[1] = val;
}

S32 Vec2I::GetY() const
{
    return mValues[1];
}

void Vec2I::SetValue( const S32 &x, const S32 &y )
{
    mValues[0] = x;
    mValues[1] = y;
}

Vec2 Vec2I::ToVec() const
{
    return Vec2( ( F32 )mValues[0], ( F32 )mValues[1] );
}

void Vec2I::Clear()
{
    SetValue( 0, 0 );
}

Vec2I operator+( const Vec2I &v1, const Vec2I &v2 )
{
    return Vec2I( v1.mValues[0] + v2.mValues[0],
                  v1.mValues[1] + v2.mValues[1] );
}

Vec2I operator*( const Vec2I &v1, const Vec2I &v2 )
{
    return Vec2I( v1.mValues[0] * v2.mValues[0],
                  v1.mValues[1] * v2.mValues[1] );
}

Vec2I operator-( const Vec2I &v1, const Vec2I &v2 )
{
    return Vec2I( v1.mValues[0] - v2.mValues[0],
                  v1.mValues[1] - v2.mValues[1] );
}

Vec2I operator-( const Vec2I &v )
{
    return Vec2I( -v.mValues[0], -v.mValues[1] );
}

Vec2I operator*( const Vec2I &v, const F32 s )
{
    return Vec2I( v.mValues[0] * s, v.mValues[1] * s );
}

Vec2I operator*( const F32 s, const Vec2I &v )
{
    return v * s;
}

Vec2I operator/( const Vec2I &v, const F32 s )
{
    assert( s != 0.0f );

    return v * ( F32( 1.0 ) / s );
}

Vec2I operator/( const Vec2I &v1, const Vec2I &v2 )
{
    return Vec2I( v1.mValues[0] / v2.mValues[0],
                  v1.mValues[1] / v2.mValues[1] );
}