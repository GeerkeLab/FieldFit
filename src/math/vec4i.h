#pragma once
#ifndef __VEC4I_H__
#define __VEC4I_H__

#include "../common/types.h"

#include <stddef.h>

class Vec4;

class Vec4I
{
    friend Vec4I operator+( const Vec4I &, const Vec4I & );
    friend Vec4I operator*( const Vec4I &, const Vec4I & );
    friend Vec4I operator-( const Vec4I &, const Vec4I & );
    friend Vec4I operator-( const Vec4I & );
    friend Vec4I operator*( const Vec4I &, const F32 );
    friend Vec4I operator*( const F32 , const Vec4I & );
    friend Vec4I operator/( const Vec4I &, const F32 );
    friend Vec4I operator/( const Vec4I &, const Vec4I & );

public:
    Vec4I();
    Vec4I( const Vec4 &v );
    Vec4I( const S32 &x, const S32 &y, const S32 &z, const S32 &w );
    Vec4I( F32 x, F32 y, F32 z, F32 w );

    S32 &operator[]( const size_t axis );
    const S32 &operator[]( const size_t axis ) const;

    S32 GetX() const;
    void SetX( const S32 &val );

    S32 GetY() const;
    void SetY( const S32 &val );

    S32 GetZ() const;
    void SetZ( const S32 &val );

    S32 GetW() const;
    void SetW( const S32 &val );

    void SetValue( const S32 &x, const S32 &y, const S32 &z, const S32 &w );
    void Clear();

    Vec4 ToVec() const;

private:
    S32 mValues[4];

};

Vec4I operator+( const Vec4I &v1, const Vec4I &v2 );
Vec4I operator*( const Vec4I &v1, const Vec4I &v2 );

Vec4I operator-( const Vec4I &v1, const Vec4I &v2 );
Vec4I operator-( const Vec4I &v );

Vec4I operator*( const Vec4I &v, const F32 s );
Vec4I operator*( const F32 s, const Vec4I &v );

Vec4I operator/( const Vec4I &v, const F32 s );
Vec4I operator/( const Vec4I &v1, const Vec4I &v2 );

#endif