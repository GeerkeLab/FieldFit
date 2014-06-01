#pragma once
#ifndef __VEC4_H__
#define __VEC4_H__

#include "../common/types.h"

class Vec4I;

class Vec4 
{
    friend Vec4 operator+( const Vec4 &, const Vec4 & );
    friend Vec4 operator*( const Vec4 &, const Vec4 & );
    friend Vec4 operator-( const Vec4 &, const Vec4 & );
    friend Vec4 operator-( const Vec4 & );
    friend Vec4 operator*( const Vec4 &, const F32 );
    friend Vec4 operator*( const F32 , const Vec4 & );
    friend Vec4 operator/( const Vec4 &, const F32 );
    friend Vec4 operator/( const Vec4 &, const Vec4 & );

public:

    Vec4();
    Vec4( const F32 x, const F32 y, const F32 z, const F32 w );
    Vec4( const Vec4 &v );
    Vec4( const Vec4I &v );

    Vec4 &operator=( const Vec4 &other );
    Vec4 &operator-=( const Vec4 &v );
    Vec4 &operator+=( const Vec4 &v );
    Vec4 &operator/=( const F32 s );
    Vec4 &operator*=( const F32 s );

    bool operator==( const Vec4 &other ) const;
    bool operator!=( const Vec4 &other ) const;

    F32 &operator[]( const U8 axis );
    const F32 &operator[]( const U8 axis ) const;

    F32 Dot( const Vec4 &v ) const;

    F32 Length2() const;
    F32 Length() const;

    F32 Distance2( const Vec4 &v ) const;
    F32 Distance( const Vec4 &v ) const;

    void SetValue( const F32 x, const F32 y, const F32 z, const F32 w );
    void Clear();

    Vec4 SafeNormalise();
    Vec4 Normalise();

    Vec4 Lerp( const Vec4 &v, F32 t ) const;

    Vec4 Absolute() const;

    U8 MinAxis() const;
    U8 MaxAxis() const;

    void SetX( const F32 x );
    F32 GetX() const;

    void SetY( const F32 y );
    F32 GetY() const;

    void SetZ( const F32 z );
    F32 GetZ() const;

    void SetW( const F32 w );
    F32 GetW() const;

    void SetZero();
    bool IsZero() const;
    bool IsFuzzyZero() const;
private:

    F32 mValues[4];
};

Vec4 operator+( const Vec4 &v1, const Vec4 &v2 );

Vec4 operator-( const Vec4 &v1, const Vec4 &v2 );
Vec4 operator-( const Vec4 &v );

Vec4 operator*( const Vec4 &v1, const Vec4 &v2 );
Vec4 operator*( const Vec4 &v, const F32 s );
Vec4 operator*( const F32 s, const Vec4 &v );

Vec4 operator/( const Vec4 &v, const F32 s );
Vec4 operator/( const Vec4 &v1, const Vec4 &v2 );

#endif