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

//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::operator* (float c) const
{
    return NiQuaternion(c * m_fW, c * m_fX, c * m_fY, c * m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion operator* (float c, const NiQuaternion& q)
{
    return NiQuaternion(c * q.m_fW, c * q.m_fX, c * q.m_fY, c * q.m_fZ);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::Exp (const NiQuaternion& q)
{
    // q = A*(x*i+y*j+z*k) where (x,y,z) is unit length
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k)
    float fAngle = NiSqrt(q.m_fX*q.m_fX + q.m_fY*q.m_fY + q.m_fZ*q.m_fZ);
    float sn, cs;
    NiSinCos(fAngle, sn, cs);

    // When A is near zero, sin(A)/A is approximately 1.  Use
    // exp(q) = cos(A)+A*(x*i+y*j+z*k)
    float coeff = ( NiAbs(sn) < ms_fEpsilon ? 1.0f : sn/fAngle );

    NiQuaternion result(cs, coeff * q.m_fX, coeff * q.m_fY, coeff * q.m_fZ);

    return result;
}
//---------------------------------------------------------------------------
// *** The following code must be updated in the NiQuaternion documentation
// if it changes. ***

// Compute 1/sqrt(s) using a tangent line approximation.
// These constants are outside of the function because
// not all compilers are smart enough to precompute the values.
static const float ISQRT_NEIGHBORHOOD = 0.959066f;
static const float ISQRT_SCALE = 1.000311f;
static const float ISQRT_ADDITIVE_CONSTANT = ISQRT_SCALE / 
    (float)sqrt(ISQRT_NEIGHBORHOOD);
static const float ISQRT_FACTOR = ISQRT_SCALE * (-0.5f / 
    (ISQRT_NEIGHBORHOOD * (float)sqrt(ISQRT_NEIGHBORHOOD)));
inline float NiQuaternion::ISqrt_approx_in_neighborhood(float s) 
{
    return ISQRT_ADDITIVE_CONSTANT + (s - ISQRT_NEIGHBORHOOD) * ISQRT_FACTOR;
}
//---------------------------------------------------------------------------
// Normalize a quaternion using the above approximation.
void NiQuaternion::FastNormalize() 
{
    float s = m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ + m_fW*m_fW; // length^2
    float k = ISqrt_approx_in_neighborhood(s);

    if (s <= 0.91521198f) {
        k *= ISqrt_approx_in_neighborhood(k * k * s);

        if (s <= 0.65211970f) {
            k *= ISqrt_approx_in_neighborhood(k * k * s);
        }
    }

    m_fX *= k;
    m_fY *= k;
    m_fZ *= k;
    m_fW *= k;
}
//---------------------------------------------------------------------------
inline float NiQuaternion::Lerp(float v0, float v1, float fPerc)
{
    return v0 + fPerc * (v1 - v0);
}
//---------------------------------------------------------------------------
// CounterWarp: A helper function used by Slerp.
inline float NiQuaternion::CounterWarp(float t, float fCos) 
{
    const float ATTENUATION = 0.82279687f;
    const float WORST_CASE_SLOPE = 0.58549219f;

    float fFactor = 1.0f - ATTENUATION * fCos;
    fFactor *= fFactor;
    float fK = WORST_CASE_SLOPE * fFactor;

    return t*(fK*t*(2.0f*t - 3.0f) + 1.0f + fK);
}
//---------------------------------------------------------------------------
NiQuaternion NiQuaternion::Slerp(float t, const NiQuaternion& p,
    const NiQuaternion& q)
{
    // assert:  Dot(p,q) >= 0 (guaranteed in NiRotKey::Interpolate methods)
    // (but not necessarily true when coming from a Squad call)

    // This algorithm is Copyright (c) 2002 Jonathan Blow, from his article 
    // "Hacking Quaternions" in Game Developer Magazine, March 2002.
    
    float fCos = Dot(p, q);

    float fTPrime;
    if (t <= 0.5f) {
        fTPrime = CounterWarp(t, fCos);
    } else {
        fTPrime = 1.0f - CounterWarp(1.0f - t, fCos);
    }

    NiQuaternion kResult(
        Lerp(p.GetW(), q.GetW(), fTPrime),
        Lerp(p.GetX(), q.GetX(), fTPrime),
        Lerp(p.GetY(), q.GetY(), fTPrime),
        Lerp(p.GetZ(), q.GetZ(), fTPrime));

    kResult.FastNormalize();
    return kResult;
}
//---------------------------------------------------------------------------
void NiQuaternion::Slerp(float t, const NiQuaternion& p,
    const NiQuaternion& q, NiQuaternion* pkResult)
{
    NIASSERT(pkResult);

    // assert:  Dot(p,q) >= 0 (guaranteed in NiRotKey::Interpolate methods)
    // (but not necessarily true when coming from a Squad call)

    // This algorithm is Copyright (c) 2002 Jonathan Blow, from his article 
    // "Hacking Quaternions" in Game Developer Magazine, March 2002.
    
    float fCos = Dot(p, q);

    float fTPrime;
    if (t <= 0.5f) {
        fTPrime = CounterWarp(t, fCos);
    } else {
        fTPrime = 1.0f - CounterWarp(1.0f - t, fCos);
    }

    pkResult->SetValues(
        Lerp(p.GetW(), q.GetW(), fTPrime),
        Lerp(p.GetX(), q.GetX(), fTPrime),
        Lerp(p.GetY(), q.GetY(), fTPrime),
        Lerp(p.GetZ(), q.GetZ(), fTPrime));

    pkResult->FastNormalize();
}
//---------------------------------------------------------------------------
