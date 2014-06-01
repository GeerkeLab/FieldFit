#include "quaternion.h"
#include "mathf.h"
#include "vec4.h"

#include <assert.h>

Quaternion::Quaternion()
{
}

Quaternion::Quaternion( const F32 yaw, const F32 pitch, const F32 roll )
{
    SetEuler( yaw, pitch, roll );
}

Quaternion::Quaternion( const F32 x, const F32 y, const F32 z, const F32 w )
{
    SetValue( x, y, z, w );
}

Quaternion::Quaternion( const Vec3 &axis, const F32 angle )
{
    SetRotation( axis, angle );
}

Quaternion::Quaternion( const Quaternion &other )
{
    mValues[0] = other.mValues[0];
    mValues[1] = other.mValues[1];
    mValues[2] = other.mValues[2];
    mValues[3] = other.mValues[3];
}

Quaternion::Quaternion( const Matrix3 &m )
{
    *this = m.GetRotation();
}

Quaternion::Quaternion( const Vec4 &axis )
{
    SetValue( axis[0], axis[1], axis[2], axis[3] );
}

Quaternion &Quaternion::operator=( const Quaternion &q )
{
    mValues[0] = q[0];
    mValues[1] = q[1];
    mValues[2] = q[2];
    mValues[3] = q[3];

    return *this;
}

Quaternion &Quaternion::operator+=( const Quaternion &q )
{
    mValues[0] += q[0];
    mValues[1] += q[1];
    mValues[2] += q[2];
    mValues[3] += q[3];

    return *this;
}

Quaternion &Quaternion::operator-=( const Quaternion &q )
{
    mValues[0] -= q[0];
    mValues[1] -= q[1];
    mValues[2] -= q[2];
    mValues[3] -= q[3];

    return *this;
}

Quaternion &Quaternion::operator*=( const Quaternion &q )
{
    SetValue( mValues[3] * q[0] + mValues[0] * q[3] + mValues[1] * q[2] - mValues[2] * q[1],
              mValues[3] * q[1] + mValues[1] * q[3] + mValues[2] * q[0] - mValues[0] * q[2],
              mValues[3] * q[2] + mValues[2] * q[3] + mValues[0] * q[1] - mValues[1] * q[0],
              mValues[3] * q[3] - mValues[0] * q[0] - mValues[1] * q[1] - mValues[2] * q[2] );

    return *this;
}

Quaternion &Quaternion::operator*=( const F32 s )
{
    mValues[0] *= s;
    mValues[1] *= s;
    mValues[2] *= s;
    mValues[3] *= s;

    return *this;
}

Quaternion &Quaternion::operator/=( const F32 s )
{
    assert( s != 0.0f );
    return *this *= 1.0f / s;
}

Quaternion Quaternion::operator+( const Quaternion &q ) const
{
    return Quaternion( mValues[0] + q[0], mValues[1] + q[1], mValues[2] + q[2], mValues[3] + q[3] );
}

Quaternion Quaternion::operator-( const Quaternion &q ) const
{
    return Quaternion( mValues[0] - q[0], mValues[1] - q[1], mValues[2] - q[2], mValues[3] - q[3] );
}

Quaternion Quaternion::operator*( const F32 s ) const
{
    return Quaternion( mValues[0] * s, mValues[1] * s, mValues[2] * s, mValues[3] * s );
}

Quaternion Quaternion::operator/( const F32 s ) const
{
    assert( s != 0.0f );
    return *this * ( 1.0f / s );
}

Quaternion Quaternion::operator-() const
{
    return Quaternion( -mValues[0], -mValues[1],  -mValues[2],  -mValues[3] );
}

F32 Quaternion::operator[]( const U8 axis ) const
{
    return mValues[ axis ];
}

F32 Quaternion::operator[]( const U8 axis )
{
    return mValues[ axis ];
}

void Quaternion::SetEuler( const F32 yaw, const F32 pitch, const F32 roll )
{
    const F32 p = yaw * 0.5f;
    const F32 y = pitch * 0.5f;
    const F32 r = roll * 0.5f;

    const F32 siny = Mathf::Sin( y );
    const F32 sinp = Mathf::Sin( p );
    const F32 sinr = Mathf::Sin( r );
    const F32 cosy = Mathf::Cos( y );
    const F32 cosp = Mathf::Cos( p );
    const F32 cosr = Mathf::Cos( r );

    SetValue( cosr * sinp * cosy + sinr * cosp * siny,
              cosr * cosp * siny - sinr * sinp * cosy,
              sinr * cosp * cosy - cosr * sinp * siny,
              cosr * cosp * cosy + sinr * sinp * siny );
}

void Quaternion::SetRotation( const Vec3 &axis, const F32 angle )
{
    assert( axis.Length() != 0.0f );

    F32 s = Mathf::Sin( angle * 0.5f ) / axis.Length();
    SetValue( axis[0] * s, axis[1] * s, axis[2] * s, Mathf::Cos( angle * 0.5f ) );
}

void Quaternion::SetValue( F32 x, F32 y, F32 z, F32 w )
{
    mValues[0] = x;
    mValues[1] = y;
    mValues[2] = z;
    mValues[3] = w;
}

Quaternion Quaternion::GetInverse() const
{
    return Quaternion( -mValues[0], -mValues[1], -mValues[2], mValues[3] );
}

Quaternion Quaternion::SafeNormalise()
{
    const Vec4 absv = Vec4( mValues[0], mValues[1], mValues[2], mValues[3] ).Absolute();
    const U8 max = absv.MaxAxis();

    if ( absv[max] > 0 )
    {
        *this /= absv[max];
        return *this /= Length();
    }

    SetValue( 1.0f, 0.0f, 0.0f, 0.0f );
    return *this;
}

Quaternion Quaternion::Normalise()
{
    assert( Length() != 0.0f );

    return *this /= Length();
}


Quaternion Quaternion::LookAt( const Vec3 &origin, const Vec3 &lookAt, const Vec3 &up )
{
    //rescale to local coordinate system
    Vec3 eyeDir = lookAt - origin;

    return LookTo( eyeDir, up );
}

Quaternion Quaternion::LookTo( const Vec3 &eyeDir, const Vec3 &up )
{
    Vec3 w = -eyeDir;
    w.Normalise();

    Vec3 u = up.Cross( w ).Normalise();
    Vec3 v = w.Cross( u ).Normalise();

    Matrix3 newMatrix;
    newMatrix.SetRow( 0, u );
    newMatrix.SetRow( 1, v );
    newMatrix.SetRow( 2, w );

    Quaternion newQ( newMatrix );

    return Quaternion( newQ.GetAxis(), newQ.GetAngle() );
}

Quaternion Quaternion::GetViewRotation() const
{
    return GetInverse();
}

F32 Quaternion::GetAngleFromQuaternion( const Quaternion &q ) const
{
    const F32 s = Mathf::Sqrt( Length2() * q.Length2() );

    assert( s != 0.0f );

    return Mathf::Acos( Mathf::Clamp( Dot( q ) / s, -1.0f, 1.0f ) );
}

F32 Quaternion::GetAngle() const
{
    return 2.0f * Mathf::Acos( Mathf::Clamp( mValues[3], -1.0f, 1.0f ) );
}

Vec3 Quaternion::GetAxis() const
{
    const F32 s2 = 1.0f - mValues[3] * mValues[3];

    if ( s2 < Mathf::GetEpsilon() )
        return Vec3( 1.0f, 0.0f, 0.0f );

    const F32 s = 1.0f / Mathf::Sqrt( s2 );
    return Vec3( mValues[0] * s, mValues[1] * s, mValues[2] * s );
}

F32 Quaternion::Dot( const Quaternion &q ) const
{
    return mValues[0] * q.mValues[0] + mValues[1] * q.mValues[1] + mValues[2] * q.mValues[2] + mValues[3] * q.mValues[3];
}

F32 Quaternion::Length2() const
{
    return Dot( *this );
}

F32 Quaternion::Length() const
{
    return Mathf::Sqrt( Length2() );
}

Quaternion Quaternion::Slerp( const Quaternion &q, const F32 t ) const
{
    const F32 dotp = Dot( q );

    if ( dotp > ( 1.0f - Mathf::GetEpsilon() ) )
    {
        return Lerp( q, t );
    }

    const F32 theta = Mathf::Acos( Mathf::Clamp( dotp, -1.0f, 1.0f ) ) * t;
    Quaternion q2 = q - *this * dotp;
    q2.Normalise();

    return *this * Mathf::Cos( theta ) + q2 * Mathf::Sin( theta );
}

Quaternion Quaternion::Nlerp( const Quaternion &q, const F32 t ) const
{
    const Vec4 tv( mValues[0], mValues[1], mValues[2], mValues[3] );
    const Vec4 qv( q.mValues[0], q.mValues[1], q.mValues[2], q.mValues[3] );
    return Quaternion( tv.Lerp( qv, t ).Normalise() );
}


Quaternion Quaternion::Lerp( const Quaternion &q, const F32 t ) const
{
    const Vec4 tv( mValues[0], mValues[1], mValues[2], mValues[3] );
    const Vec4 qv( q.mValues[0], q.mValues[1], q.mValues[2], q.mValues[3] );
    return Quaternion( tv.Lerp( qv, t ) );
}

void Quaternion::Clear()
{
    SetValue( 0.0f, 0.0f, 0.0f, 1.0f );
}

Quaternion operator*( const Quaternion &q1, const Quaternion &q2 )
{
    return Quaternion( q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1],
                       q1[3] * q2[1] + q1[1] * q2[3] + q1[2] * q2[0] - q1[0] * q2[2],
                       q1[3] * q2[2] + q1[2] * q2[3] + q1[0] * q2[1] - q1[1] * q2[0],
                       q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] );
}

Quaternion operator*( const Quaternion &q, const Vec3 &v )
{
    return Quaternion( +q[3] * v[0] + q[1] * v[2] - q[2] * v[1],
                       +q[3] * v[1] + q[2] * v[0] - q[0] * v[2],
                       +q[3] * v[2] + q[0] * v[1] - q[1] * v[0],
                       -q[0] * v[0] - q[1] * v[1] - q[2] * v[2] );
}

Quaternion operator*( const Vec3 &v, const Quaternion &q )
{
    return Quaternion( +v[0] * q[3] + v[1] * q[2] - v[2] * q[1],
                       +v[1] * q[3] + v[2] * q[0] - v[0] * q[2],
                       +v[2] * q[3] + v[0] * q[1] - v[1] * q[0],
                       -v[0] * q[0] - v[1] * q[1] - v[2] * q[2] );
}