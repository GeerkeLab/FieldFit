#pragma once
#ifndef __VEC3_H__
#define __VEC3_H__

#include "../common/types.h"

class Vec3I;

class Vec3 
{
    friend Vec3 operator+( const Vec3 &, const Vec3 & );
    friend Vec3 operator*( const Vec3 &, const Vec3 & );
    friend Vec3 operator-( const Vec3 &, const Vec3 & );
    friend Vec3 operator-( const Vec3 & );
    friend Vec3 operator*( const Vec3 &, const F32 );
    friend Vec3 operator*( const F32 , const Vec3 & );
    friend Vec3 operator/( const Vec3 &, const F32 );
    friend Vec3 operator/( const Vec3 &, const Vec3 & );

public:

    Vec3();
    Vec3( const F32 x, const F32 y, const F32 z );
    Vec3( const Vec3 &v );
    Vec3( const Vec3I &v );

    Vec3 &operator=( const Vec3 &other );
    Vec3 &operator-=( const Vec3 &v );
    Vec3 &operator+=( const Vec3 &v );
    Vec3 &operator/=( const F32 s );
    Vec3 &operator*=( const F32 s );

    bool operator==( const Vec3 &other ) const;
    bool operator!=( const Vec3 &other ) const;

    F32 &operator[]( const U8 axis );
    const F32 &operator[]( const U8 axis ) const;

    F32 Dot( const Vec3 &v ) const;

    F32 Length2() const;
    F32 Length() const;

    F32 Distance2( const Vec3 &v ) const;
    F32 Distance( const Vec3 &v ) const;

    void SetValue( const F32 x, const F32 y, const F32 z );
    void Clear();

    Vec3 SafeNormalise();
    Vec3 Normalise();

    Vec3 Lerp( const Vec3 &v, const F32 t ) const;
    Vec3 Slerp( const Vec3 &v, const F32 t ) const;
    Vec3 Nlerp( const Vec3 &v, const F32 t ) const;

    Vec3 Rotate( const Vec3 &axis, const F32 angle ) const;
    Vec3 Cross( const Vec3 &v ) const;
    Vec3 Absolute() const;

    U8 MinAxis() const;
    U8 MaxAxis() const;

    F32 Angle( const Vec3 &v ) const;

    void SetX( const F32 x );
    F32 GetX() const;

    void SetY( const F32 y );
    F32 GetY() const;

    void SetZ( const F32 z );
    F32 GetZ() const;

    void SetZero();
    bool IsZero() const;

    bool IsFuzzyZero() const;

private:

    F32 mValues[3];
};

Vec3 operator+( const Vec3 &v1, const Vec3 &v2 );

Vec3 operator-( const Vec3 &v1, const Vec3 &v2 );
Vec3 operator-( const Vec3 &v );

Vec3 operator*( const Vec3 &v1, const Vec3 &v2 );
Vec3 operator*( const Vec3 &v, const F32 s );
Vec3 operator*( const F32 s, const Vec3 &v );

Vec3 operator/( const Vec3 &v, const F32 s );
Vec3 operator/( const Vec3 &v1, const Vec3 &v2 );

#endif