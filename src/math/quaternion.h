/**
 * Copyright (c) 2014 Koen Visscher, Paul Visscher and individual contributors.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "matrix3.h"
#include "vec3.h"

class Vec4;

class Quaternion
{
public:
    Quaternion();
    Quaternion( const F32 yaw, const F32 pitch, const F32 roll );
    Quaternion( const F32 x, const F32 y, const F32 z, const F32 w );

    Quaternion( const Vec3 &axis, const F32 angle );
    Quaternion( const Quaternion &other );
    Quaternion( const Matrix3 &m );
    Quaternion( const Vec4 &axis );

    Quaternion &operator=( const Quaternion &v );

    Quaternion &operator+=( const Quaternion &q );
    Quaternion &operator-=( const Quaternion &q );
    Quaternion &operator*=( const Quaternion &q );
    Quaternion &operator*=( const F32 s );
    Quaternion &operator/=( const F32 s );

    Quaternion operator+( const Quaternion &q ) const;
    Quaternion operator-( const Quaternion &q ) const;
    Quaternion operator*( const F32 s ) const;
    Quaternion operator/( const F32 s ) const;

    Quaternion operator-() const;

    F32 operator[]( const U8 axis ) const;
    F32 operator[]( const U8 axis );

    void Clear();

    void SetEuler( const F32 yaw, const F32 pitch, const F32 roll );
    void SetRotation( const Vec3 &axis, const F32 angle );
    void SetValue( const F32 x, const F32 y, const F32 z, const F32 w );

    Quaternion GetInverse() const;
    Quaternion SafeNormalise();
    Quaternion Normalise();

    static Quaternion LookAt( const Vec3 &origin, const Vec3 &lookAt, const Vec3 &up );
    static Quaternion LookTo( const Vec3 &eyeDir, const Vec3 &up );
    Quaternion GetViewRotation() const;

    F32 GetAngleFromQuaternion( const Quaternion &q ) const;
    F32 GetAngle() const;
    Vec3 GetAxis() const;

    F32 Dot( const Quaternion &q ) const;
    F32 Length2() const;
    F32 Length() const;

    Quaternion Slerp( const Quaternion &q, const F32 t ) const;
    Quaternion Nlerp( const Quaternion &q, const F32 t ) const;
    Quaternion Lerp( const Quaternion &q, const F32 t ) const;

private:

    F32 mValues[4];
};

Quaternion operator*( const Quaternion &q1, const Quaternion &q2 );
Quaternion operator*( const Quaternion &q, const Vec3 &v );
Quaternion operator*( const Vec3 &v, const Quaternion &q );

#endif