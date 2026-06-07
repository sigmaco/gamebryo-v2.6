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
float NiSqrDistance::Compute(const NiSegment& kSeg, 
    const NiParallelogram& kPgm, float& fSegParam, float& fPgmParam0, 
    float& fPgmParam1)
{
    const float fTolerance = 1e-05f;
    NiSegment kRectSeg;
    NiPoint3 kPoint;
    float fMin;
    float fMin0;
    float fTmpSegParam;
    float fTmpPgmParam0;
    float fTmpPgmParam1;

    // This code first determines if the segment and parallelogram
    // are parallel.  If they are, this is a degenerate case that is handled
    // by code at the end of this function.  The condition to enter the 
    // first (determinant calculation based) case is to determine if the
    // angle between the plane normal (E0 x E1) and the segment direction (D)
    // is basically 90 degrees.
    // cos(a) = ((E0 x E1) * D) / (|(E0 x E1)||D|)
    NiPoint3 kPlaneN = kPgm.m_kEdge0.Cross(kPgm.m_kEdge1);

    if (NiAbs(kPlaneN * kSeg.m_kDirection) > (fTolerance * (kPlaneN.Length() *
        kSeg.m_kDirection.Length())))
    {
        NiPoint3 kDiff = kPgm.m_kOrigin - kSeg.m_kOrigin;
        float fA00 = kSeg.m_kDirection.Dot(kSeg.m_kDirection);
        float fA01 = -(kSeg.m_kDirection.Dot(kPgm.m_kEdge0));
        float fA02 = -(kSeg.m_kDirection.Dot(kPgm.m_kEdge1));
        float fA11 = kPgm.m_kEdge0.Dot(kPgm.m_kEdge0);
        float fA12 = kPgm.m_kEdge0.Dot(kPgm.m_kEdge1);
        float fA22 = kPgm.m_kEdge1.Dot(kPgm.m_kEdge1);
        float fB0 = -(kDiff.Dot(kSeg.m_kDirection));
        float fB1 = kDiff.Dot(kPgm.m_kEdge0);
        float fB2 = kDiff.Dot(kPgm.m_kEdge1);
        float fCof00 = fA11 * fA22 - fA12 * fA12;
        float fCof01 = fA02 * fA12 - fA01 * fA22;
        float fCof02 = fA01 * fA12 - fA02 * fA11;
        float fDet = fA00 * fCof00 + fA01 * fCof01 + fA02 * fCof02;

        if (NiAbs(fDet) >= fTolerance)
        {
            float fCof11 = fA00 * fA22 - fA02 * fA02;
            float fCof12 = fA02 * fA01 - fA00 * fA12;
            float fCof22 = fA00 * fA11 - fA01 * fA01;
            float fInvDet = 1.0f / fDet;
            float fRhs0 = -fB0 * fInvDet;
            float fRhs1 = -fB1 * fInvDet;
            float fRhs2 = -fB2 * fInvDet;

            fSegParam = fCof00 * fRhs0 + fCof01 * fRhs1 + fCof02 * fRhs2;
            fPgmParam0 = fCof01 * fRhs0 + fCof11 * fRhs1 + fCof12 * fRhs2;
            fPgmParam1 = fCof02 * fRhs0 + fCof12 * fRhs1 + fCof22 * fRhs2;

            if (fSegParam < 0)
            {
                if (fPgmParam0 < 0)
                {
                    if (fPgmParam1 < 0)  // region 6m
                    {
                        // fMin on face fPgmParam0=0 or fPgmParam1=0 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 5m
                    {
                        // fMin on face fPgmParam0=0 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else  // region 4m
                    {
                        // fMin on face fPgmParam0=0 or fPgmParam1=1 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
                else if (fPgmParam0 <= 1)
                {
                    if (fPgmParam1 < 0)  // region 7m
                    {
                        // fMin on face fPgmParam1=0 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam0);
                        fPgmParam1 = 0;
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 0m
                    {
                        // fMin on face fSegParam=0
                        fMin = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fPgmParam0, fPgmParam1);
                        fSegParam = 0;
                    }
                    else  // region 3m
                    {
                        // fMin on face fPgmParam1=1 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam0);
                        fPgmParam1 = 1;
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
                else
                {
                    if (fPgmParam1 < 0)  // region 8m
                    {
                        // fMin on face fPgmParam0=1 or fPgmParam1=0 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 1m
                    {
                        // fMin on face fPgmParam0=1 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else  // region 2m
                    {
                        // fMin on face fPgmParam0=1 or fPgmParam1=1 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
            }
            else if (fSegParam <= 1)
            {
                if (fPgmParam0 < 0)
                {
                    if (fPgmParam1 < 0)  // region 6
                    {
                        // fMin on face fPgmParam0=0 or fPgmParam1=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 5
                    {
                        // fMin on face fPgmParam0=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                    }
                    else // region 4
                    {
                        // fMin on face fPgmParam0=0 or fPgmParam1=1
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
                else if (fPgmParam0 <= 1)
                {
                    if (fPgmParam1 < 0)  // region 7
                    {
                        // fMin on face fPgmParam1=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam0);
                        fPgmParam1 = 0;
                    }
                    else if (fPgmParam1 <= 1)  // region 0
                    {
                        // global minimum is interior
                        fMin = NiAbs(fSegParam * 
                            (fA00 * fSegParam + fA01 * fPgmParam0 + 
                            fA02 * fPgmParam1 + 2.0f * fB0) + 
                            fPgmParam0 * 
                            (fA01 * fSegParam + fA11 * fPgmParam0 + 
                            fA12 * fPgmParam1 + 2.0f * fB1) + 
                            fPgmParam1 * 
                            (fA02 * fSegParam + fA12 * fPgmParam0 + 
                            fA22 * fPgmParam1 + 2.0f * fB2) + 
                            kDiff.Dot(kDiff));
                    }
                    else  // region 3
                    {
                        // fMin on face fPgmParam1=1
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam0);
                        fPgmParam1 = 1;
                    }
                }
                else
                {
                    if (fPgmParam1 < 0)  // region 8
                    {
                        // fMin on face fPgmParam0=1 or fPgmParam1=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 1
                    {
                        // fMin on face fPgmParam0=1
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                    }
                    else  // region 2
                    {
                        // fMin on face fPgmParam0=1 or fPgmParam1=1
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
            }
            else
            {
                if (fPgmParam0 < 0)
                {
                    if (fPgmParam1 < 0)  // region 6p
                    {
                        // fMin on face fPgmParam0=0 or fPgmParam1=0 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 5p
                    {
                        // fMin on face fPgmParam0=0 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else  // region 4p
                    {
                        // fMin on face fPgmParam0=0 or fPgmParam1=1 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 0;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
                else if (fPgmParam0 <= 1)
                {
                    if (fPgmParam1 < 0)  // region 7p
                    {
                        // fMin on face fPgmParam1=0 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam0);
                        fPgmParam1 = 0;
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 0p
                    {
                        // fMin on face fSegParam=1
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin = NiSqrDistance::Compute(kPoint, kPgm,
                            fPgmParam0, fPgmParam1);
                        fSegParam = 1;
                    }
                    else  // region 3p
                    {
                        // fMin on face fPgmParam1=1 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam0);
                        fPgmParam1 = 1;
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
                else
                {
                    if (fPgmParam1 < 0)  // region 8p
                    {
                        // fMin on face fPgmParam0=1 or fPgmParam1=0 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 0;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else if (fPgmParam1 <= 1)  // region 1p
                    {
                        // fMin on face fPgmParam0=1 or fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                    else  // region 2p
                    {
                        // fMin on face fPgmParam0=1 or fPgmParam1=1 or
                        //   fSegParam=0
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
                        kRectSeg.m_kDirection = kPgm.m_kEdge1;
                        fMin = NiSqrDistance::Compute(kSeg, kRectSeg,
                            fSegParam, fPgmParam1);
                        fPgmParam0 = 1;
                        kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
                        kRectSeg.m_kDirection = kPgm.m_kEdge0;
                        fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, 
                            fTmpSegParam, fTmpPgmParam0);
                        fTmpPgmParam1 = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                        kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
                        fMin0 = NiSqrDistance::Compute(kPoint, kPgm, 
                            fTmpPgmParam0, fTmpPgmParam1);
                        fTmpSegParam = 1;
                        if (fMin0 < fMin)
                        {
                            fMin = fMin0;
                            fSegParam = fTmpSegParam;
                            fPgmParam0 = fTmpPgmParam0;
                            fPgmParam1 = fTmpPgmParam1;
                        }
                    }
                }
            }

            return fMin;
        }
    }

    // kSeg and rectoid are parallel
    kRectSeg.m_kOrigin = kPgm.m_kOrigin;
    kRectSeg.m_kDirection = kPgm.m_kEdge0;
    fMin = NiSqrDistance::Compute(kSeg, kRectSeg, fSegParam, 
        fPgmParam0);
    fPgmParam1 = 0;

    kRectSeg.m_kDirection = kPgm.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, fTmpSegParam, 
        fTmpPgmParam1);
    fTmpPgmParam0 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fSegParam = fTmpSegParam;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
    kRectSeg.m_kDirection = kPgm.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, fTmpSegParam, 
        fTmpPgmParam0);
    fTmpPgmParam1 = 1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fSegParam = fTmpSegParam;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kRectSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
    kRectSeg.m_kDirection = kPgm.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kRectSeg, fTmpSegParam, 
        fTmpPgmParam1);
    fTmpPgmParam0 = 1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fSegParam = fTmpSegParam;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    fMin0 = NiSqrDistance::Compute(kSeg.m_kOrigin, kPgm, fTmpPgmParam0, 
        fTmpPgmParam1);
    fTmpSegParam = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fSegParam = fTmpSegParam;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kPoint = kSeg.m_kOrigin + kSeg.m_kDirection;
    fMin0 = NiSqrDistance::Compute(kPoint, kPgm, fTmpPgmParam0, 
        fTmpPgmParam1);
    fTmpSegParam = 1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fSegParam = fTmpSegParam;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    return fMin;
}
//---------------------------------------------------------------------------

