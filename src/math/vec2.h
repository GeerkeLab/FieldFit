#pragma once
#ifndef __VEC2_H__
#define __VEC2_H__

#include "../common/types.h"

class Vec2I;

class Vec2 
{
    friend Vec2 operator+( const Vec2 &, const Vec2 & );
    friend Vec2 operator*( const Vec2 &, const Vec2 & );
    friend Vec2 operator-( const Vec2 &, const Vec2 & );
    friend Vec2 operator-( const Vec2 & );
    friend Vec2 operator*( const Vec2 &, const F32 );
    friend Vec2 operator*( const F32 , const Vec2 & );
    friend Vec2 operator/( const Vec2 &, const F32 );
    friend Vec2 operator/( const Vec2 &, const Vec2 & );
public:

    Vec2();
    Vec2( const F32 x, const F32 y );
    Vec2( const Vec2 &v );
    Vec2( const Vec2I &v );

    Vec2 &operator=( const Vec2 &other );
    Vec2 &operator-=( const Vec2 &v );
    Vec2 &operator+=( const Vec2 &v );
    Vec2 &operator/=( const F32 s );
    Vec2 &operator*=( const F32 s );

    bool operator==( const Vec2 &other ) const;
    bool operator!=( const Vec2 &other ) const;

    F32 &operator[]( const U8 axis );
    const F32 &operator[]( const U8 axis ) const;

    F32 Dot( const Vec2 &v ) const;

    F32 Length2() const;
    F32 Length() const;

    F32 Distance2( const Vec2 &v ) const;
    F32 Distance( const Vec2 &v ) const;

    void SetValue( F32 x, F32 y );
    void Clear();

    Vec2 SafeNormalise();
    Vec2 Normalise();

    Vec2 Lerp( const Vec2 &v, const F32 t ) const;
    Vec2 Nlerp( const Vec2 &v, const F32 t ) const;

    Vec2 Rotate( const F32 angle ) const;
    Vec2 Absolute() const;

    U8 MinAxis() const;
    U8 MaxAxis() const;

    F32 Angle( const Vec2 &v ) const;

    void SetX( const F32 x );
    F32 GetX() const;

    void SetY( const F32 y );
    F32 GetY() const;

    void SetZero();
    bool IsZero() const;

    bool IsFuzzyZero() const;

private:

    F32 mValues[2];
};

Vec2 operator+( const Vec2 &v1, const Vec2 &v2 );

Vec2 operator-( const Vec2 &v1, const Vec2 &v2 );
Vec2 operator-( const Vec2 &v );

Vec2 operator*( const Vec2 &v1, const Vec2 &v2 );
Vec2 operator*( const Vec2 &v, const F32 s );
Vec2 operator*( const F32 s, const Vec2 &v );

Vec2 operator/( const Vec2 &v, const F32 s );
Vec2 operator/( const Vec2 &v1, const Vec2 &v2 );

#endif