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

#include "NiBound.h"
#include "NiMath.h"
#include "NiMatrix3.h"
#include "NiNode.h"
#include "NiPoint3.h"
#include "NiFloat16.h"
#include <NiRTLib.h>

float NiBound::ms_fFuzzFactor = 1e-04f;
float NiBound::ms_fTolerance  = 1e-06f;

//---------------------------------------------------------------------------
bool NiBound::operator==(const NiBound& bound)
{
    return (m_fRadius == bound.m_fRadius && m_kCenter == bound.m_kCenter);
}
//---------------------------------------------------------------------------
bool NiBound::operator<=(const NiBound& bound)
{
    // returns true iff 'this' is contained in 'bound'
    NiPoint3 diff = bound.m_kCenter - m_kCenter;
    return diff.Length()+m_fRadius <= bound.m_fRadius + ms_fFuzzFactor;
}
//---------------------------------------------------------------------------
bool NiBound::operator>=(const NiBound& bound)
{
    // returns true iff 'this' contains 'bound'
    NiPoint3 diff = bound.m_kCenter - m_kCenter;
    return diff.Length()+bound.m_fRadius <= m_fRadius + ms_fFuzzFactor;
}
//---------------------------------------------------------------------------
template <typename TData, NiUInt32 uiDataCompCount>
class ComputeFromDataImpl
{
public:
    inline void Execute(NiBound* pkThis, int iQuantity, const TData* pkData)
    {
        if (iQuantity <= 0)
        {
            pkThis->SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
            return;
        }

        // compute the axis-aligned box containing the data
        NiPoint3 kMin(pkData[0], pkData[1], pkData[2]);
        NiPoint3 kMax = kMin;
        int i;
        for (i = 1; i < iQuantity; i++) 
        {
            NiUInt32 uiIndex = i * uiDataCompCount;
            if (kMin.x > pkData[uiIndex])
                kMin.x = pkData[uiIndex];
            if (kMin.y > pkData[uiIndex + 1])
                kMin.y = pkData[uiIndex + 1];
            if (kMin.z > pkData[uiIndex + 2])
                kMin.z = pkData[uiIndex + 2];
            if (kMax.x < pkData[uiIndex])
                kMax.x = pkData[uiIndex];
            if (kMax.y < pkData[uiIndex + 1])
                kMax.y = pkData[uiIndex + 1];
            if (kMax.z < pkData[uiIndex + 2])
                kMax.z = pkData[uiIndex + 2];
        }

        // sphere center is the axis-aligned box center
        pkThis->SetCenter(0.5f * (kMin + kMax));

        // compute the radius
        float fRadiusSqr = 0.0f;
        for(i = 0; i < iQuantity; i++) 
        {
            NiUInt32 uiIndex = i * uiDataCompCount;
            NiPoint3 kDiff(pkData[uiIndex], pkData[uiIndex + 1], 
                pkData[uiIndex + 2]);
                kDiff -= pkThis->GetCenter();
            float fLengthSqr = kDiff * kDiff;
            if (fLengthSqr > fRadiusSqr)
                fRadiusSqr = fLengthSqr;
        }
        pkThis->SetRadius(NiSqrt(fRadiusSqr));
    }
};
void NiBound::ComputeFromData(int iQuantity, const void* pkData, 
    const NiUInt32 uiStride)
{
    if (uiStride == sizeof(float) * 3)
    {
        ComputeFromDataImpl<float, 3> kFunctor;
        kFunctor.Execute(this, iQuantity, (float*)pkData);
    }
    else if (uiStride == sizeof(NiFloat16) * 4)
    {
        ComputeFromDataImpl<NiFloat16, 4> kFunctor;
        kFunctor.Execute(this, iQuantity, (NiFloat16*)pkData);
    }
    else
    {
        NIASSERT(0 && "Could not compute the bound incorrect format");
    }
}
//---------------------------------------------------------------------------
void NiBound::ComputeFromData(int iQuantity, const NiPoint3* pkData)
{
    if (iQuantity <= 0)
    {
        m_kCenter = NiPoint3::ZERO;
        m_fRadius = 0.0f;
        return;
    }

    // compute the axis-aligned box containing the data
    NiPoint3 kMin = pkData[0];
    NiPoint3 kMax = kMin;
    int i;
    for (i = 1; i < iQuantity; i++) 
    {
        if (kMin.x > pkData[i].x)
            kMin.x = pkData[i].x;
        if (kMin.y > pkData[i].y)
            kMin.y = pkData[i].y;
        if (kMin.z > pkData[i].z)
            kMin.z = pkData[i].z;
        if (kMax.x < pkData[i].x)
            kMax.x = pkData[i].x;
        if (kMax.y < pkData[i].y)
            kMax.y = pkData[i].y;
        if (kMax.z < pkData[i].z)
            kMax.z = pkData[i].z;
    }

    // sphere center is the axis-aligned box center
    m_kCenter = 0.5f * (kMin + kMax);

    // compute the radius
    float fRadiusSqr = 0.0f;
    for(i = 0; i < iQuantity; i++) 
    {
        NiPoint3 diff = pkData[i] - m_kCenter;
        float fLengthSqr = diff * diff;
        if (fLengthSqr > fRadiusSqr)
            fRadiusSqr = fLengthSqr;
    }
    m_fRadius = NiSqrt(fRadiusSqr);
}
//---------------------------------------------------------------------------
void NiBound::ComputeFromData(int iQuantity, const NiPoint4* pkData)
{
    if (iQuantity <= 0)
    {
        m_kCenter = NiPoint3::ZERO;
        m_fRadius = 0.0f;
        return;
    }

    // compute the axis-aligned box containing the data
    NiPoint3 kMin = NiPoint3(pkData[0].X(), pkData[0].Y(), pkData[0].Z());
    NiPoint3 kMax = kMin;
    int i;
    for (i = 1; i < iQuantity; i++) 
    {
        if (kMin.x > pkData[i].X())
            kMin.x = pkData[i].X();
        if (kMin.y > pkData[i].Y())
            kMin.y = pkData[i].Y();
        if (kMin.z > pkData[i].Z())
            kMin.z = pkData[i].Z();
        if (kMax.x < pkData[i].X())
            kMax.x = pkData[i].X();
        if (kMax.y < pkData[i].Y())
            kMax.y = pkData[i].Y();
        if (kMax.z < pkData[i].Z())
            kMax.z = pkData[i].Z();
    }

    // sphere center is the axis-aligned box center
    m_kCenter = 0.5f * (kMin + kMax);

    // compute the radius
    float fRadiusSqr = 0.0f;
    for(i = 0; i < iQuantity; i++) 
    {
        NiPoint3 kData = NiPoint3(pkData[i].X(), pkData[i].Y(), pkData[i].Z());
        NiPoint3 diff = kData - m_kCenter;
        float fLengthSqr = diff * diff;
        if (fLengthSqr > fRadiusSqr)
            fRadiusSqr = fLengthSqr;
    }
    m_fRadius = NiSqrt(fRadiusSqr);
}
//---------------------------------------------------------------------------
void NiBound::ComputeMinimalBound(NiTPrimitiveArray<const NiBound*>& kBounds)
{
    // assert:  array contains only non-null pointers to NiBound
    NIASSERT(kBounds.GetSize() > 0);

    // get bounding sphere of first child
    const NiBound* pkBound = kBounds.GetAt(0);
    m_kCenter = pkBound->m_kCenter;
    m_fRadius = pkBound->m_fRadius;

    // process remaining bounding spheres
    for (unsigned int i = 1; i < kBounds.GetSize(); i++)
        Merge(kBounds.GetAt(i));
}
//---------------------------------------------------------------------------
bool NiBound::TestIntersect(float fTime, 
    const NiBound& kB0, const NiPoint3& kV0, 
    const NiBound& kB1, const NiPoint3& kV1)
{
    NiPoint3 kDeltaV = kV1 - kV0;
    float fA = kDeltaV.SqrLength();
    NiPoint3 kDeltaC = kB1.GetCenter() - kB0.GetCenter();
    float fC = kDeltaC.SqrLength();
    float fRadiusSum = kB0.GetRadius() + kB1.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;

    if (fA > 0.0f)
    {
        float fB = kDeltaC.Dot(kDeltaV);
        if (fB <= 0.0f)
        {
            if (-fTime * fA <= fB)
                return fA * fC - fB * fB <= fA * fRadiusSumSqr;
            else
                return fTime * (fTime * fA + 2.0f * fB) + fC <= fRadiusSumSqr;
        }
    }

    return fC <= fRadiusSumSqr;
}
//---------------------------------------------------------------------------
bool NiBound::FindIntersect(float fTime, 
    const NiBound& kB0, const NiPoint3& kV0, 
    const NiBound& kB1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    // compute the theoretical first time and point of contact

    NiPoint3 kDeltaV = kV1 - kV0;
    float fA = kDeltaV.SqrLength();
    NiPoint3 kDeltaC = kB1.GetCenter() - kB0.GetCenter();
    float fC = kDeltaC.SqrLength();
    float fRadiusSum = kB0.GetRadius() + kB1.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;

    if (fA > 0.0f)
    {
        float fB = kDeltaC.Dot(kDeltaV);
        if (fB <= 0.0f)
        {
            if (-fTime * fA <= fB
                || fTime * (fTime * fA + 2.0f * fB) + fC <= fRadiusSumSqr)
            {
                float fCDiff = fC - fRadiusSumSqr;
                float fDiscr = fB * fB - fA * fCDiff;
                if (fDiscr >= 0.0f)
                {
                    if (fCDiff <= 0.0f)
                    {
                        // spheres are already intersecting, use midpoint
                        // of segment connecting centers
                        fIntrTime = 0.0f;
                        kIntrPt = 0.5f*(kB0.GetCenter()+kB1.GetCenter());

                        if (bCalcNormals)
                        {
                            kNormal0 = kDeltaC;
                            kNormal0.Unitize();
                            kNormal1 = -kNormal0;
                        }
                    }
                    else
                    {
                        // first time of contact is in [0, tmin]
                        fIntrTime = -(fB+NiSqrt(fDiscr))/fA;

                        // clamp to handle floating point round-off errors
                        if (fIntrTime < 0.0f)
                            fIntrTime = 0.0f;
                        else if (fIntrTime > fTime)
                            fIntrTime = fTime;

                        NiPoint3 kCenDiff = kDeltaC + fIntrTime*kDeltaV;

                        kIntrPt = kB0.GetCenter() + fIntrTime*kV0 +
                            (kB0.GetRadius()/fRadiusSum)*kCenDiff;

                        if (bCalcNormals)
                        {
                            kNormal0 = kCenDiff;
                            kNormal0.Unitize();
                            kNormal1 = -kNormal0;
                        }
                    }
                    return true;
                }
            }
            return false;
        }
    }

    if (fC <= fRadiusSumSqr)
    {
        // spheres are already intersecting, use midpoint of segment
        // connecting centers
        fIntrTime = 0.0f;
        kIntrPt = 0.5f*(kB0.GetCenter()+kB1.GetCenter());

        if (bCalcNormals)
        {
            kNormal0 = kDeltaC;
            kNormal0.Unitize();
            kNormal1 = -kNormal0;
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
#ifndef __SPU__
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiBound::LoadBinary(NiStream& kStream)
{
    m_kCenter.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
void NiBound::SaveBinary(NiStream& kStream)
{
    m_kCenter.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
char* NiBound::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 65;
    char* pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "%s = (%g, %g, %g) , %g", pPrefix, m_kCenter.x, 
        m_kCenter.y, m_kCenter.z, m_fRadius);
    return pString;
}
//---------------------------------------------------------------------------

#endif
