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
#include "NiCollisionPCH.h"

#include "NiSqrDistance.h"

//---------------------------------------------------------------------------
#define COMPARE_MIN(fTmpMinD, fMinD, fTmpSeg, fSeg, fTmpTri0, fTri0, \
    fTmpTri1, fTri1) \
{ \
    if(fTmpMinD<fMinD) \
    { \
        fMinD=fTmpMinD; \
        fSeg=fTmpSeg; \
        fTri0=fTmpTri0; \
        fTri1=fTmpTri1; \
    } \
}
//---------------------------------------------------------------------------
float NiSqrDistance::Compute(const NiSegment& kSeg, const NiTrigon& kTri, 
    float& fSegParm, float& fTriParm0, float& fTriParm1)
{
    const float fTolerance = 1e-05f;
    NiSegment kTriSeg;
    NiPoint3 kPoint;
    float fMin, fMin0, fTmpSegParm, fTmpTriParm0, fTmpTriParm1;

    NiPoint3 kDiff = kTri.m_kOrigin - kSeg.m_kOrigin;
    float fA00 = kSeg.m_kDirection.Dot(kSeg.m_kDirection);
    float fA01 = -(kSeg.m_kDirection.Dot(kTri.m_kEdge0));
    float fA02 = -(kSeg.m_kDirection.Dot(kTri.m_kEdge1));
    float fA11 = kTri.m_kEdge0.Dot(kTri.m_kEdge0);
    float fA12 = kTri.m_kEdge0.Dot(kTri.m_kEdge1);
    float fA22 = kTri.m_kEdge1.Dot(kTri.m_kEdge1);
    float fB0 = -kDiff.Dot(kSeg.m_kDirection);
    float fB1 = kDiff.Dot(kTri.m_kEdge0);
    float fB2 = kDiff.Dot(kTri.m_kEdge1);

    // This code first determines if the segment and triangle are parallel.
    // If they are, this case is a degenerate case that is handled by code at
    // the end of this function.  The condition to enter the first
    // (determinant calculation based) case is to determine if the angle
    // between the plane normal (E0 x E1) and the segment direction (D) is
    // basically 90 degrees.

    // cos(a) = ((E0 x E1) * D) / (|(E0 x E1)||D|)

    NiPoint3 kPlaneN = kTri.m_kEdge0.Cross(kTri.m_kEdge1);

    if (NiAbs(kPlaneN * kSeg.m_kDirection)  // If not parallel ...
        > (fTolerance * (kPlaneN.Length() * kSeg.m_kDirection.Length())))
    {
        float fCof00 = fA11 * fA22 - fA12 * fA12;
        float fCof01 = fA02 * fA12 - fA01 * fA22;
        float fCof02 = fA01 * fA12 - fA02 * fA11;
        float fCof11 = fA00 * fA22 - fA02 * fA02;
        float fCof12 = fA02 * fA01 - fA00 * fA12;
        float fCof22 = fA00 * fA11 - fA01 * fA01;

        float fDet = fA00 * fCof00 + fA01 * fCof01 + fA02 * fCof02;
        float fInvDet = 1.0f/fDet;
        
        float fRhs0 = -fB0 * fInvDet;
        float fRhs1 = -fB1 * fInvDet;
        float fRhs2 = -fB2 * fInvDet;

        fSegParm = fCof00 * fRhs0 + fCof01 * fRhs1 + fCof02 * fRhs2;
        fTriParm0 = fCof01 * fRhs0 + fCof11 * fRhs1 + fCof12 * fRhs2;
        fTriParm1 = fCof02 * fRhs0 + fCof12 * fRhs1 + fCof22 * fRhs2;

        if (fSegParm < 0.0f)
        {
            if (fTriParm0 + fTriParm1 <= 1.0f)
            {
                if (fTriParm0 < 0.0f)
                {
                    if (fTriParm1 < 0.0f)   // region 4m
                    {
                        // min on face TP0=0 or TP1=0 or SP=0
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                            fTriParm1);
                        fTriParm0 = 0.0f;
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg,
                            fTmpSegParm, fTmpTriParm0);
                        fTmpTriParm1 = 0.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri, 
                            fTmpTriParm0, fTmpTriParm1);
                        fTmpSegParm = 0.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    }
                    else    // region 3m, min on face TP0=0 or SP=0
                    {
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                            fTriParm1);
                        fTriParm0 = 0.0f;
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri,
                            fTmpTriParm0, fTmpTriParm1);
                        fTmpSegParm = 0.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    }
                }
                else if (fTriParm1 < 0.0f)  // region 5m
                {
                    // min on face TP1=0 or SP=0
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm0);
                    fTriParm1 = 0.0f;
                    fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri,
                        fTmpTriParm0, fTmpTriParm1);
                    fTmpSegParm = 0.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else    // region 0m, min face on SP=0
                {
                    fMin = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri,
                        fTriParm0, fTriParm1);
                    fSegParm = 0.0f;
                }
            }
            else
            {
                if (fTriParm0 < 0.0f)   // region 2m
                {
                    // min on face TP0=0 or TP0+TP1=1 or SP=0
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge1;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm1);
                    fTriParm0 = 0.0f;
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1-kTri.m_kEdge0;
                    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm,
                        fTmpTriParm1);
                    fTmpTriParm0 = 1.0f - fTmpTriParm1;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri,
                        fTmpTriParm0, fTmpTriParm1);
                    fTmpSegParm = 0.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else if (fTriParm1 < 0.0f)  // region 6m
                {
                    // min on face TP1=0 or TP0+TP1=1 or SP=0
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm0);
                    fTriParm1 = 0.0f;
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1-kTri.m_kEdge0;
                    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm,
                        fTmpTriParm1);
                    fTmpTriParm0 = 1.0f - fTmpTriParm1;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri,
                        fTmpTriParm0, fTmpTriParm1);
                    fTmpSegParm = 0.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else    // region 1m, min on face TP0+TP1=1 or SP=0
                {
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1-kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm, 
                        fTriParm1);
                    fTriParm0 = 1.0f - fTriParm1;
                    fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri, 
                        fTmpTriParm0, fTmpTriParm1);
                    fTmpSegParm = 0.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
            }
        }
        else if (fSegParm <= 1.0f)
        {
            if (fTriParm0 + fTriParm1 <= 1.0f)
            {
                if (fTriParm0 < 0.0f)
                {
                    if (fTriParm1 < 0.0f)   // region 4
                    {
                        // min on face TP0=0 or TP1=0
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                            fTriParm1);
                        fTriParm0 = 0.0f;
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg,
                            fTmpSegParm, fTmpTriParm0);
                        fTmpTriParm1 = 0.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    }
                    else    // region 3, min on face TP0=0
                    {
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                            fTriParm1);
                        fTriParm0 = 0.0f;
                    }
                }
                else if (fTriParm1 < 0.0f)  // region 5, min on face TP1=0
                {
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm0);
                    fTriParm1 = 0.0f;
                }
                else    // region 0, global minimum is interior, so done
                {
                    fMin = 0.0f;
                }
            }
            else
            {
                if (fTriParm0 < 0.0f)   // region 2
                {
                    // min on face TP0=0 or TP0+TP1=1
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge1;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm, 
                        fTriParm1);
                    fTriParm0 = 0.0f;
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1 - kTri.m_kEdge0;
                    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm,
                        fTmpTriParm1);
                    fTmpTriParm0 = 1.0f - fTmpTriParm1;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else if (fTriParm1 < 0.0f)  // region 6
                {
                    // min on face TP1=0 or TP0+TP1=1
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm, 
                        fTriParm0);
                    fTriParm1 = 0.0f;
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1 - kTri.m_kEdge0;
                    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm,
                        fTmpTriParm1);
                    fTmpTriParm0 = 1.0f - fTmpTriParm1;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else    // region 1, min on face TP0+TP1=1
                {
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1 - kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm, 
                        fTriParm1);
                    fTriParm0 = 1.0f - fTriParm1;
                }
            }
        }
        else    // fSegParm > 1
        {
            if (fTriParm0 + fTriParm1 <= 1.0f)
            {
                if (fTriParm0 < 0.0f)
                {
                    if (fTriParm1 < 0.0f)   // region 4p
                    {
                        // min on face TP0=0 or TP1=0 or SP=1
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                            fTriParm1);
                        fTriParm0 = 0.0f;
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg,
                            fTmpSegParm, fTmpTriParm0);
                        fTmpTriParm1 = 0.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kTri,
                            fTmpTriParm0, fTmpTriParm1);
                        fTmpSegParm = 1.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    }
                    else    // region 3p, min on face TP0=0 or SP=1
                    {
                        kTriSeg.m_kOrigin = kTri.m_kOrigin;
                        kTriSeg.m_kDirection = kTri.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                            fTriParm1);
                        fTriParm0 = 0.0f;
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kTri,
                            fTmpTriParm0, fTmpTriParm1);
                        fTmpSegParm = 1.0f;
                        COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                            fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    }
                }
                else if (fTriParm1 < 0.0f)  // region 5p
                {
                    // min on face TP1=0 or SP=1
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm, 
                        fTriParm0);
                    fTriParm1 = 0.0f;
                    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                    fMin0 = NiSqrDistance::Compute(kPoint, kTri, fTmpTriParm0,
                        fTmpTriParm1);
                    fTmpSegParm = 1.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else    // region 0p, min face on SP=1
                {
                    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                    fMin = NiSqrDistance::Compute(kPoint, kTri, fTriParm0,
                        fTriParm1);
                    fSegParm = 1.0f;
                }
            }
            else
            {
                if (fTriParm0 < 0.0f)   // region 2p
                {
                    // min on face TP0=0 or TP0+TP1=1 or SP=1
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge1;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm1);
                    fTriParm0 = 0.0f;
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1 - kTri.m_kEdge0;
                    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm,
                        fTmpTriParm1);
                    fTmpTriParm0 = 1.0f - fTmpTriParm1;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                    fMin0 = NiSqrDistance::Compute(kPoint, kTri, fTmpTriParm0,
                        fTmpTriParm1);
                    fTmpSegParm = 1.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else if (fTriParm1 < 0.0f)  // region 6p
                {
                    // min on face TP1=0 or TP0+TP1=1 or SP=1
                    kTriSeg.m_kOrigin = kTri.m_kOrigin;
                    kTriSeg.m_kDirection = kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm0);
                    fTriParm1 = 0.0f;
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1 - kTri.m_kEdge0;
                    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm,
                        fTmpTriParm1);
                    fTmpTriParm0 = 1.0f - fTmpTriParm1;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                    fMin0 = NiSqrDistance::Compute(kPoint, kTri, fTmpTriParm0,
                        fTmpTriParm1);
                    fTmpSegParm = 1.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
                else    // region 1p, min on face TP0+TP1=1 or SP=1
                {
                    kTriSeg.m_kOrigin = kTri.m_kOrigin + kTri.m_kEdge0;
                    kTriSeg.m_kDirection = kTri.m_kEdge1 - kTri.m_kEdge0;
                    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm,
                        fTriParm1);
                    fTriParm0 = 1.0f - fTriParm1;
                    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                    fMin0 = NiSqrDistance::Compute(kPoint, kTri, fTmpTriParm0,
                        fTmpTriParm1);
                    fTmpSegParm = 1.0f;
                    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm,
                        fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1);
                }
            }
        }

        return fMin;
    }

    // Segment and triangle are parallel.
    kTriSeg.m_kOrigin = kTri.m_kOrigin;
    kTriSeg.m_kDirection = kTri.m_kEdge0;
    fMin = NiSqrDistance::Compute(kSeg, kTriSeg, fSegParm, fTriParm0);
    fTriParm1 = 0.0f;

    kTriSeg.m_kDirection = kTri.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm, fTmpTriParm1);
    fTmpTriParm0 = 0.0f;
    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0,
        fTmpTriParm1, fTriParm1);

    kTriSeg.m_kOrigin = kTriSeg.m_kOrigin + kTri.m_kEdge0;
    kTriSeg.m_kDirection = kTriSeg.m_kDirection - kTri.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kTriSeg, fTmpSegParm, fTmpTriParm1);
    fTmpTriParm0 = 1.0f - fTmpTriParm1;
    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0,
        fTmpTriParm1, fTriParm1);

    fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kTri, fTmpTriParm0,
        fTmpTriParm1);
    fTmpSegParm = 0.0f;
    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0,
        fTmpTriParm1, fTriParm1);

    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
    fMin0 = NiSqrDistance::Compute(kPoint, kTri, fTmpTriParm0, fTmpTriParm1);
    fTmpSegParm = 1.0f;
    COMPARE_MIN(fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0,
        fTmpTriParm1, fTriParm1);

    return fMin;
}
//---------------------------------------------------------------------------
