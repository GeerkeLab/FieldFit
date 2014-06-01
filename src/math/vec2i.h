#pragma once
#ifndef __VEC2I_H__
#define __VEC2I_H__

#include "../common/types.h"

#include <stddef.h>

class Vec2;

class Vec2I
{
    friend Vec2I operator+( const Vec2I &, const Vec2I & );
    friend Vec2I operator*( const Vec2I &, const Vec2I & );
    friend Vec2I operator-( const Vec2I &, const Vec2I & );
    friend Vec2I operator-( const Vec2I & );
    friend Vec2I operator*( const Vec2I &, const F32 );
    friend Vec2I operator*( const F32 , const Vec2I & );
    friend Vec2I operator/( const Vec2I &, const F32 );
    friend Vec2I operator/( const Vec2I &, const Vec2I & );
public:

    Vec2I();
    Vec2I( const Vec2 &v );
    Vec2I( const S32 x, const S32 y );
    Vec2I( const F32 x, const F32 y );

    S32 &operator[]( const U8 axis );
    const S32 &operator[]( const U8 axis ) const;

    void SetX( const S32 &val );
    S32 GetX() const;

    void SetY( const S32 &val );
    S32 GetY() const;

    void SetValue( const S32 &x, const S32 &y );
    void Clear();

    Vec2 ToVec() const;

private:

    S32 mValues[2];

};

Vec2I operator+( const Vec2I &v1, const Vec2I &v2 );

Vec2I operator*( const Vec2I &v1, const Vec2I &v2 );

Vec2I operator-( const Vec2I &v1, const Vec2I &v2 );
Vec2I operator-( const Vec2I &v );

Vec2I operator*( const Vec2I &v, const F32 s );
Vec2I operator*( const F32 s, const Vec2I &v );

Vec2I operator/( const Vec2I &v, const F32 s );
Vec2I operator/( const Vec2I &v1, const Vec2I &v2 );

#endif