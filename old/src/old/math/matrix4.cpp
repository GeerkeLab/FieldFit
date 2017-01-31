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

#include "matrix4.h"
#include "matrix3.h"
#include "mathf.h"

#include <assert.h>

Matrix4::Matrix4()
{
}

Matrix4::Matrix4( const F32 a11, const F32 a12, const F32 a13, const F32 a14, const F32 a21, const F32 a22,
                  const F32 a23, const F32 a24, const F32 a31, const F32 a32, const F32 a33, const F32 a34, const F32 a41, const F32 a42,
                  const F32 a43, const F32 a44 )
{
    SetValue( a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44 );
}

Matrix4::Matrix4( const Vec4 &v1, const Vec4 &v2, const Vec4 &v3, const Vec4 &v4 )
{
    mValues[0] = v1;
    mValues[1] = v2;
    mValues[2] = v3;
    mValues[3] = v4;
}

Matrix4::Matrix4( const Matrix4 &other )
{
    mValues[0] = other.mValues[0];
    mValues[1] = other.mValues[1];
    mValues[2] = other.mValues[2];
    mValues[3] = other.mValues[3];
}

Matrix4 &Matrix4::operator=( const Matrix4 &m )
{
    mValues[0] = m.mValues[0];
    mValues[1] = m.mValues[1];
    mValues[2] = m.mValues[2];
    mValues[3] = m.mValues[3];

    return *this;
}

Matrix4 &Matrix4::operator*=( const Matrix4 &m )
{
    SetValue( m.Dotx( mValues[0] ), m.Doty( mValues[0] ), m.Dotz( mValues[0] ), m.Dotw( mValues[0] ),
              m.Dotx( mValues[1] ), m.Doty( mValues[1] ), m.Dotz( mValues[1] ), m.Dotw( mValues[1] ),
              m.Dotx( mValues[2] ), m.Doty( mValues[2] ), m.Dotz( mValues[2] ), m.Dotw( mValues[2] ),
              m.Dotx( mValues[3] ), m.Doty( mValues[3] ), m.Dotz( mValues[3] ), m.Dotw( mValues[3] ) );

    return *this;
}

Matrix4 &Matrix4::operator+=( const Matrix4 &m )
{
    SetValue( mValues[0][0] + m.mValues[0][0], mValues[0][1] + m.mValues[0][1], mValues[0][2] + m.mValues[0][2],
              mValues[0][3] + m.mValues[0][3],
              mValues[1][0] + m.mValues[1][0], mValues[1][1] + m.mValues[1][1], mValues[1][2] + m.mValues[1][2],
              mValues[1][3] + m.mValues[1][3],
              mValues[2][0] + m.mValues[2][0], mValues[2][1] + m.mValues[2][1], mValues[2][2] + m.mValues[2][2],
              mValues[2][3] + m.mValues[2][3],
              mValues[2][0] + m.mValues[3][0], mValues[3][1] + m.mValues[3][1], mValues[3][2] + m.mValues[3][2],
              mValues[2][3] + m.mValues[3][3] );

    return *this;
}

Matrix4 &Matrix4::operator-=( const Matrix4 &m )
{
    SetValue( mValues[0][0] - m.mValues[0][0], mValues[0][1] - m.mValues[0][1], mValues[0][2] - m.mValues[0][2],
              mValues[0][3] - m.mValues[0][3],
              mValues[1][0] - m.mValues[1][0], mValues[1][1] - m.mValues[1][1], mValues[1][2] - m.mValues[1][2],
              mValues[1][3] - m.mValues[1][3],
              mValues[2][0] - m.mValues[2][0], mValues[2][1] - m.mValues[2][1], mValues[2][2] - m.mValues[2][2],
              mValues[2][3] - m.mValues[2][3],
              mValues[3][0] - m.mValues[3][0], mValues[3][1] - m.mValues[3][1], mValues[3][2] - m.mValues[3][2],
              mValues[3][3] - m.mValues[3][3] );

    return *this;
}

bool Matrix4::operator==( const Matrix4 &m ) const
{
    return mValues[0] == m.mValues[0] && mValues[1] == m.mValues[1] && mValues[2] == m.mValues[2] &&
           mValues[3] == m.mValues[3];
}

bool Matrix4::operator!=( const Matrix4 &m ) const
{
    return !( *this == m );
}

void Matrix4::SetColumn( const U8 column, const Vec4 &v )
{
    mValues[0][column] = v[0];
    mValues[1][column] = v[1];
    mValues[2][column] = v[2];
    mValues[3][column] = v[3];
}

Vec4 Matrix4::GetColumn( const U8 column ) const
{
    return Vec4( mValues[0][column], mValues[1][column], mValues[2][column], mValues[3][column] );
}

void Matrix4::SetRow( const U8 row, const Vec4 &v )
{
    mValues[row] = v;
}

Vec4 Matrix4::GetRow( const U8 row ) const
{
    return mValues[row];
}

Matrix4 Matrix4::Scale( const Vec4 &v ) const
{
    return Matrix4( mValues[0][0] * v[0], mValues[0][1] * v[1], mValues[0][2] * v[2], mValues[0][3] * v[3],
                    mValues[1][0] * v[0], mValues[1][1] * v[1], mValues[1][2] * v[2], mValues[1][3] * v[3],
                    mValues[2][0] * v[0], mValues[2][1] * v[1], mValues[2][2] * v[2], mValues[2][3] * v[3],
                    mValues[3][0] * v[0], mValues[3][1] * v[1], mValues[3][2] * v[2], mValues[3][3] * v[3] );
}

Matrix4 Matrix4::GetTranspose() const
{
    return Matrix4( mValues[0][0], mValues[1][0], mValues[2][0], mValues[3][0],
                    mValues[0][1], mValues[1][1], mValues[2][1], mValues[3][1],
                    mValues[0][2], mValues[1][2], mValues[2][2], mValues[3][2],
                    mValues[0][3], mValues[1][3], mValues[2][3], mValues[3][3] );
}

Matrix4 Matrix4::GetAbsolute() const
{
    return Matrix4( Mathf::Abs( mValues[0][0] ), Mathf::Abs( mValues[0][1] ), Mathf::Abs( mValues[0][2] ),
                    Mathf::Abs( mValues[0][3] ),
                    Mathf::Abs( mValues[1][0] ), Mathf::Abs( mValues[1][1] ), Mathf::Abs( mValues[1][2] ),
                    Mathf::Abs( mValues[1][3] ),
                    Mathf::Abs( mValues[2][0] ), Mathf::Abs( mValues[2][1] ), Mathf::Abs( mValues[2][2] ),
                    Mathf::Abs( mValues[2][3] ),
                    Mathf::Abs( mValues[3][0] ), Mathf::Abs( mValues[3][1] ), Mathf::Abs( mValues[3][2] ),
                    Mathf::Abs( mValues[3][3] ) );
}

Matrix4 Matrix4::GetAdjoint() const
{
    return Matrix4( GetCofactor( 0, 0 ), GetCofactor( 0, 1 ), GetCofactor( 0, 2 ), GetCofactor( 0, 3 ),
                    GetCofactor( 1, 0 ), GetCofactor( 1, 1 ), GetCofactor( 1, 2 ), GetCofactor( 1, 3 ),
                    GetCofactor( 2, 0 ), GetCofactor( 2, 1 ), GetCofactor( 2, 2 ), GetCofactor( 2, 3 ),
                    GetCofactor( 3, 0 ), GetCofactor( 3, 1 ), GetCofactor( 3, 2 ), GetCofactor( 3, 3 ) );
}

Matrix4 Matrix4::GetInverse() const
{
    const F32 det = GetDeterminant();

    assert( det != 0.0f );

    const F32 s = 1.0f / det;
    return GetAdjoint().GetTranspose() * s;
}

void Matrix4::SetIdentity()
{
    SetValue( 1.0f, 0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f, 0.0f,
              0.0f, 0.0f, 1.0f, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f );
}

F32 Matrix4::GetCofactor( const U8 row, const U8 column ) const
{
    F32 cof = Matrix3( mValues[( row + 1 ) % 4][( column + 1 ) % 4], mValues[( row + 1 ) % 4][( column + 2 ) % 4],
                       mValues[( row + 1 ) % 4][( column + 3 ) % 4],
                       mValues[( row + 2 ) % 4][( column + 1 ) % 4], mValues[( row + 2 ) % 4][( column + 2 ) % 4],
                       mValues[( row + 2 ) % 4][( column + 3 ) % 4],
                       mValues[( row + 3 ) % 4][( column + 1 ) % 4], mValues[( row + 3 ) % 4][( column + 2 ) % 4],
                       mValues[( row + 3 ) % 4][( column + 3 ) % 4] ).GetDeterminant();

    if ( ( ( column + row ) & 1 ) == 1 )
        cof *= -1;

    return cof;
}

F32 Matrix4::GetDeterminant() const
{
    return  mValues[0][0] * GetCofactor( 0, 0 ) +
            mValues[1][0] * GetCofactor( 1, 0 ) +
            mValues[2][0] * GetCofactor( 2, 0 ) +
            mValues[3][0] * GetCofactor( 3, 0 );
}

void Matrix4::SetValue( const F32 a11, const F32 a12, const F32 a13, const F32 a14, const F32 a21, const F32 a22,
                        const F32 a23, const F32 a24, const F32 a31, const F32 a32, const F32 a33, const F32 a34, const F32 a41, const F32 a42,
                        const F32 a43, const F32 a44 )
{
    mValues[0].SetValue( a11, a12, a13, a14 );
    mValues[1].SetValue( a21, a22, a23, a24 );
    mValues[2].SetValue( a31, a32, a33, a34 );
    mValues[2].SetValue( a41, a42, a43, a44 );
}

F32 Matrix4::Dotx( const Vec4 &v ) const
{
    return mValues[0][0] * v[0] + mValues[1][0] * v[1] + mValues[2][0] * v[2] + mValues[3][0] * v[3];
}

F32 Matrix4::Doty( const Vec4 &v ) const
{
    return mValues[0][1] * v[0] + mValues[1][1] * v[1] + mValues[2][1] * v[2] + mValues[3][1] * v[3];
}

F32 Matrix4::Dotz( const Vec4 &v ) const
{
    return mValues[0][2] * v[0] + mValues[1][2] * v[1] + mValues[2][2] * v[2] + mValues[3][2] * v[3];
}

F32 Matrix4::Dotw( const Vec4 &v ) const
{
    return mValues[0][2] * v[0] + mValues[1][2] * v[1] + mValues[2][2] * v[2] + mValues[3][2] * v[3];
}

Matrix4 operator+( const Matrix4 &m1, const Matrix4 &m2 )
{
    return Matrix4( m1.mValues[0][0] + m2.mValues[0][0], m1.mValues[0][1] + m2.mValues[0][1],
                    m1.mValues[0][2] + m2.mValues[0][2], m1.mValues[0][3] + m2.mValues[0][3],

                    m1.mValues[1][0] + m2.mValues[1][0], m1.mValues[1][1] + m2.mValues[1][1],
                    m1.mValues[1][2] + m2.mValues[1][2], m1.mValues[1][3] + m2.mValues[1][3],

                    m1.mValues[2][0] + m2.mValues[2][0], m1.mValues[2][1] + m2.mValues[2][1],
                    m1.mValues[2][2] + m2.mValues[2][2], m1.mValues[2][3] + m2.mValues[2][3],

                    m1.mValues[3][0] + m2.mValues[3][0], m1.mValues[3][1] + m2.mValues[3][1],
                    m1.mValues[3][2] + m2.mValues[3][2], m1.mValues[3][3] + m2.mValues[3][3] );
}

Matrix4 operator-( const Matrix4 &m1, const Matrix4 &m2 )
{
    return Matrix4( m1.mValues[0][0] - m2.mValues[0][0], m1.mValues[0][1] - m2.mValues[0][1],
                    m1.mValues[0][2] - m2.mValues[0][2], m1.mValues[0][3] - m2.mValues[0][3],

                    m1.mValues[1][0] - m2.mValues[1][0], m1.mValues[1][1] - m2.mValues[1][1],
                    m1.mValues[1][2] - m2.mValues[1][2], m1.mValues[1][3] - m2.mValues[1][3],

                    m1.mValues[2][0] - m2.mValues[2][0], m1.mValues[2][1] - m2.mValues[2][1],
                    m1.mValues[2][2] - m2.mValues[2][2], m1.mValues[2][3] - m2.mValues[2][3],

                    m1.mValues[3][0] - m2.mValues[3][0], m1.mValues[3][1] - m2.mValues[3][1],
                    m1.mValues[3][2] - m2.mValues[3][2], m1.mValues[3][3] - m2.mValues[3][3] );
}

Matrix4 operator*( const Matrix4 &m1, const Matrix4 &m2 )
{
    return Matrix4( m2.Dotx( m1.mValues[0] ), m2.Doty( m1.mValues[0] ), m2.Dotz( m1.mValues[0] ), m2.Dotz( m1.mValues[0] ),
                    m2.Dotx( m1.mValues[1] ), m2.Doty( m1.mValues[1] ), m2.Dotz( m1.mValues[1] ), m2.Dotz( m1.mValues[1] ),
                    m2.Dotx( m1.mValues[2] ), m2.Doty( m1.mValues[2] ), m2.Dotz( m1.mValues[2] ), m2.Dotz( m1.mValues[2] ),
                    m2.Dotx( m1.mValues[2] ), m2.Doty( m1.mValues[2] ), m2.Dotz( m1.mValues[2] ), m2.Dotz( m1.mValues[3] ) );
}
Matrix4 operator*( const Matrix4 &m, const F32 s )
{
    return Matrix4( m.mValues[0][0] * s, m.mValues[0][1] * s, m.mValues[0][2] * s, m.mValues[0][3] * s,
                    m.mValues[1][0] * s, m.mValues[1][1] * s, m.mValues[1][2] * s, m.mValues[1][3] * s,
                    m.mValues[2][0] * s, m.mValues[2][1] * s, m.mValues[2][2] * s, m.mValues[2][3] * s,
                    m.mValues[3][0] * s, m.mValues[3][1] * s, m.mValues[3][2] * s, m.mValues[3][3] * s );
}

Vec4 operator*( const Matrix4 &m, const Vec4 &v )
{
    return Vec4( m.mValues[0].Dot( v ), m.mValues[1].Dot( v ), m.mValues[2].Dot( v ), m.mValues[3].Dot( v ) );
}
