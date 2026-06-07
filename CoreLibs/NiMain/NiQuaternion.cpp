// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "NiMainPCH.h"

#include <NiRTLib.h>
#include "NiMatrix3.h"
#include "NiQuaternion.h"

const NiQuaternion NiQuaternion::IDENTITY(1.0f,0.0f,0.0f,0.0f);
const float NiQuaternion::ms_fEpsilon = 1e-03f;

//---------------------------------------------------------------------------
NiQuaternion::NiQuaternion(float w, float x, float y, float z) :
    m_fW(w), m_fX(x), m_fY(y), m_fZ(z)
{
}
//---------------------------------------------------------------------------
NiQuaternion::NiQuaternion(float fAngle, const NiPoint3 &axis)
{
    FromAngleAxis (fAngle, axis);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::operator+ (const NiQuaternion& q) const
{
    return NiQuaternion(m_fW + q.m_fW, m_fX + q.m_fX,
        m_fY + q.m_fY, m_fZ + q.m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::operator- (const NiQuaternion& q) const
{
    return NiQuaternion(m_fW - q.m_fW, m_fX - q.m_fX,
        m_fY - q.m_fY, m_fZ - q.m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::operator- () const
{
    return NiQuaternion(-m_fW, -m_fX, -m_fY, -m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::operator* (const NiQuaternion& q) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    /*
    // This code is left here for anyone who gets the urge to switch to 
    // a "faster" quaternion multiplication routine.  While on some 
    // architectures this might be, on a Pentium it is NOT.
    //
    float A = (m_fW + m_fX) * (q.m_fW + q.m_fX);
    float B = (m_fZ - m_fY) * (q.m_fY - q.m_fZ);
    float C = (m_fX - m_fW) * (q.m_fY + q.m_fZ);
    float D = (m_fY + m_fZ) * (q.m_fX - q.m_fW);
    float E = (m_fX + m_fZ) * (q.m_fX + q.m_fY);
    float F = (m_fX - m_fZ) * (q.m_fX - q.m_fY);
    float G = (m_fW + m_fY) * (q.m_fW - q.m_fZ);
    float H = (m_fW - m_fY) * (q.m_fW + q.m_fZ);

    return NiQuaternion
    (
        B + (-E - F + G + H) / 2,
        A -  (E + F + G + H) / 2,
       -C +  (E - F + G - H) / 2,
       -D +  (E - F - G + H) / 2
    );
    */

    return NiQuaternion
    (
        m_fW * q.m_fW - m_fX * q.m_fX - m_fY * q.m_fY - m_fZ * q.m_fZ,
        m_fW * q.m_fX + m_fX * q.m_fW + m_fY * q.m_fZ - m_fZ * q.m_fY,
        m_fW * q.m_fY + m_fY * q.m_fW + m_fZ * q.m_fX - m_fX * q.m_fZ,
        m_fW * q.m_fZ + m_fZ * q.m_fW + m_fX * q.m_fY - m_fY * q.m_fX
    );
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::UnitInverse(const NiQuaternion& p)
{
    // assert:  p is unit length

    return NiQuaternion(p.m_fW, -p.m_fX, -p.m_fY, -p.m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::Log(const NiQuaternion& q)
{
    // q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length
    // log(q) = A*(x*i+y*j+z*k)

    float angle = NiACos(q.m_fW);
    float sn = NiSin(angle);

    // When A is near zero, A/sin(A) is approximately 1.  Use
    // log(q) = sin(A)*(x*i+y*j+z*k)
    float coeff = ( NiAbs(sn) < ms_fEpsilon ? 1.0f : angle/sn );

    return NiQuaternion(0.0f, coeff * q.m_fX, coeff * q.m_fY, coeff * q.m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::Intermediate(const NiQuaternion& q0, 
    const NiQuaternion& q1, const NiQuaternion& q2)
{
    // assert:  q0, q1, q2 are unit quaternions

    NiQuaternion inv = UnitInverse(q1);
    NiQuaternion exp = Exp(-0.25f * (Log(inv * q0) + Log(inv * q2)));

    return q1 * exp;
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::Squad(float t, const NiQuaternion& p,
    const NiQuaternion& a, const NiQuaternion& b, const NiQuaternion& q)
{
    return Slerp(2.0f*t*(1.0f-t),Slerp(t,p,q),Slerp(t,a,b));
}

//---------------------------------------------------------------------------
void NiQuaternion::FromAngleAxis(float angle, const NiPoint3& axis)
{
    float fHalfAngle = angle * 0.5f;
    float sn;
    NiSinCos(fHalfAngle, sn, m_fW);
    m_fX = axis.x * sn;
    m_fY = axis.y * sn;
    m_fZ = axis.z * sn;
}
//---------------------------------------------------------------------------
void NiQuaternion::ToAngleAxis(float& angle, NiPoint3& axis) const
{
    float length = NiSqrt(m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
                         
    if ( length < ms_fEpsilon )
    {
        angle = 0;
        axis.x = 0;
        axis.y = 0;
        axis.z = 0;
    }
    else
    {
        angle = 2.0f * NiACos(m_fW);
        float invLength = 1.0f/length;
        axis.x = m_fX * invLength;
        axis.y = m_fY * invLength;
        axis.z = m_fZ * invLength;
    }
}
//---------------------------------------------------------------------------
void NiQuaternion::FromRotation(const NiMatrix3& rot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGraPH course notes
    // article "Quaternion Calculus and Fast Animation".

    float fTrace = rot.GetEntry(0,0) + rot.GetEntry(1,1) + rot.GetEntry(2,2);
    float fRoot;

    if ( fTrace > 0.0f )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = NiSqrt(fTrace+1.0f);  // 2w
        m_fW = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;  // 1/(4w)

        m_fX = (rot.GetEntry( 2,1 ) - rot.GetEntry( 1,2 )) * fRoot;
        m_fY = (rot.GetEntry( 0,2 ) - rot.GetEntry( 2,0 )) * fRoot;
        m_fZ = (rot.GetEntry( 1,0 ) - rot.GetEntry( 0,1 )) * fRoot;
    }
    else
    {
        // |w| <= 1/2
        static int next[3] = { 1, 2, 0 };
        int i = 0;
        if ( rot.GetEntry( 1,1 ) > rot.GetEntry( 0,0 ) )
            i = 1;
        if ( rot.GetEntry( 2,2 ) > rot.GetEntry( i,i ) ) 
            i = 2;
        int j = next[i];
        int k = next[j];

        fRoot = NiSqrt(rot.GetEntry( i,i ) - 
            rot.GetEntry( j,j ) - rot.GetEntry( k,k ) + 1.0f);
        float* quat[3] = { &m_fX, &m_fY, &m_fZ };
        *quat[i] = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        m_fW = (rot.GetEntry( k,j ) - rot.GetEntry( j,k )) * fRoot;
        *quat[j] = (rot.GetEntry( j,i ) + rot.GetEntry( i,j )) * fRoot;
        *quat[k] = (rot.GetEntry( k,i ) + rot.GetEntry( i,k )) * fRoot;
    }
}
//---------------------------------------------------------------------------
void NiQuaternion::Snap()
{
    const float epsilon = 1e-08f;
    if (NiAbs(m_fX) <= epsilon && m_fX != 0.0f)
    {
        //NIASSERT(NiAbs(m_fX) > 1e-32f);
        m_fX = 0.0f;
    }

    if (NiAbs(m_fY) <= epsilon  && m_fY != 0.0f)
    {
        //NIASSERT(NiAbs(m_fY) > 1e-32f);
        m_fY = 0.0f;
    }

    if (NiAbs(m_fZ) <= epsilon && m_fZ != 0.0f)
    {
        //NIASSERT(NiAbs(m_fZ) > 1e-32f);
        m_fZ = 0.0f;
    }

    if (NiAbs(m_fW) <= epsilon && m_fW != 0.0f)
    {
        //NIASSERT(NiAbs(m_fW) > 1e-32f);
        m_fW = 0.0f;
    }

}
//---------------------------------------------------------------------------
void NiQuaternion::Normalize()
{
     float fLength = m_fW * m_fW + m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
     float fInvLength = 1.0f / NiSqrt(fLength);
     m_fW *= fInvLength;
     m_fX *= fInvLength;
     m_fY *= fInvLength;
     m_fZ *= fInvLength;
}

#ifndef __SPU__
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiQuaternion::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,m_fW);
    NiStreamLoadBinary(stream,m_fX);
    NiStreamLoadBinary(stream,m_fY);
    NiStreamLoadBinary(stream,m_fZ);
}
//---------------------------------------------------------------------------
void NiQuaternion::SaveBinary(NiStream& stream)
{
    Snap();
    NiStreamSaveBinary(stream,m_fW);
    NiStreamSaveBinary(stream,m_fX);
    NiStreamSaveBinary(stream,m_fY);
    NiStreamSaveBinary(stream,m_fZ);
}
//---------------------------------------------------------------------------
char* NiQuaternion::GetViewerString(const char* pcPrefix) const
{
    size_t stLen = strlen(pcPrefix) + 128;
    char* pcString = NiAlloc(char, stLen);
    NiSprintf(pcString, stLen, "%s: (w=%g,x=%g,y=%g,z=%g)", pcPrefix, 
        m_fW, m_fX, m_fY, m_fZ); 
    return pcString;
}
//---------------------------------------------------------------------------
#endif

