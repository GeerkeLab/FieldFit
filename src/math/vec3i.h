#pragma once
#ifndef __VEC3I_H__
#define __VEC3I_H__

#include "../common/types.h"

#include <stddef.h>

class Vec3;

class Vec3I
{
    friend Vec3I operator+( const Vec3I &, const Vec3I & );
    friend Vec3I operator*( const Vec3I &, const Vec3I & );
    friend Vec3I operator-( const Vec3I &, const Vec3I & );
    friend Vec3I operator-( const Vec3I & );
    friend Vec3I operator*( const Vec3I &, const F32 );
    friend Vec3I operator*( const F32 , const Vec3I & );
    friend Vec3I operator/( const Vec3I &, const F32 );
    friend Vec3I operator/( const Vec3I &, const Vec3I & );

public:

    Vec3I();
    Vec3I( const Vec3 &v );
    Vec3I( const S32 x, const S32 y, const S32 z );
    Vec3I( const F32 x, const F32 y, const F32 z );

    S32 &operator[]( const U8 axis );
    const S32 &operator[]( const U8 axis ) const;

    void SetX( const S32 val );
    S32 GetX() const;

    void SetY( const S32 val );
    S32 GetY() const;

    void SetZ( const S32 val );
    S32 GetZ() const;

    void SetValue( const S32 x, const S32 y, const S32 z );
    void Clear();

    Vec3 ToVec() const;

private:

    S32 mValues[3];
};

Vec3I operator+( const Vec3I &v1, const Vec3I &v2 );
Vec3I operator*( const Vec3I &v1, const Vec3I &v2 );

Vec3I operator-( const Vec3I &v1, const Vec3I &v2 );
Vec3I operator-( const Vec3I &v );

Vec3I operator*( const Vec3I &v, const F32 s );
Vec3I operator*( F32 s, const Vec3I &v );

Vec3I operator/( const Vec3I &v, F32 s );
Vec3I operator/( const Vec3I &v1, const Vec3I &v2 );

#endif