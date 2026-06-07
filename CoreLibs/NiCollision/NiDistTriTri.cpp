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

#include <NiMath.h>
#include "NiSqrDistance.h"

//---------------------------------------------------------------------------
float NiSqrDistance::Compute(const NiTrigon& kTri0, const NiTrigon& kTri1, 
    float& fTri0Param0, float& fTri0Param1, float& fTri1Param0, 
    float& fTri1Param1)
{
    float fMin;
    float fMin0;
    float fTmpTri0Param0;
    float fTmpTri0Param1;
    float fTmpTri1Param0;
    float fTmpTri1Param1;
    NiSegment kSeg;

    // compare edges of kTri0 against all of kTri1
    kSeg.m_kOrigin = kTri0.m_kOrigin;
    kSeg.m_kDirection = kTri0.m_kEdge0;
    fMin = NiSqrDistance::Compute(kSeg, kTri1, fTri0Param0, fTri1Param0, 
        fTri1Param1);
    fTri0Param1 = 0;

    kSeg.m_kDirection = kTri0.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri1, fTmpTri0Param1, 
        fTmpTri1Param0, fTmpTri1Param1);
    fTmpTri0Param0 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTri0Param0 = fTmpTri0Param0;
        fTri0Param1 = fTmpTri0Param1;
        fTri1Param0 = fTmpTri1Param0;
        fTri1Param1 = fTmpTri1Param1;
    }

    kSeg.m_kOrigin = kSeg.m_kOrigin + kTri0.m_kEdge0;
    kSeg.m_kDirection = kSeg.m_kDirection - kTri0.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri1, fTmpTri0Param1, 
        fTmpTri1Param0, fTmpTri1Param1);
    fTmpTri0Param0 = 1 - fTmpTri0Param1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTri0Param0 = fTmpTri0Param0;
        fTri0Param1 = fTmpTri0Param1;
        fTri1Param0 = fTmpTri1Param0;
        fTri1Param1 = fTmpTri1Param1;
    }

    // compare edges of kTri1 against all of kTri0
    kSeg.m_kOrigin = kTri1.m_kOrigin;
    kSeg.m_kDirection = kTri1.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri0, fTmpTri1Param0, 
        fTmpTri0Param0, fTmpTri0Param1);
    fTmpTri1Param1 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTri0Param0 = fTmpTri0Param0;
        fTri0Param1 = fTmpTri0Param1;
        fTri1Param0 = fTmpTri1Param0;
        fTri1Param1 = fTmpTri1Param1;
    }

    kSeg.m_kDirection = kTri1.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri0, fTmpTri1Param1, 
        fTmpTri0Param0, fTmpTri0Param1);
    fTmpTri1Param0 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTri0Param0 = fTmpTri0Param0;
        fTri0Param1 = fTmpTri0Param1;
        fTri1Param0 = fTmpTri1Param0;
        fTri1Param1 = fTmpTri1Param1;
    }

    kSeg.m_kOrigin = kSeg.m_kOrigin + kTri1.m_kEdge0;
    kSeg.m_kDirection = kSeg.m_kDirection - kTri1.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri0, fTmpTri1Param1, 
        fTmpTri0Param0, fTmpTri0Param1);
    fTmpTri1Param0 = 1 - fTmpTri1Param1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTri0Param0 = fTmpTri0Param0;
        fTri0Param1 = fTmpTri0Param1;
        fTri1Param0 = fTmpTri1Param0;
        fTri1Param1 = fTmpTri1Param1;
    }

    return fMin;
}
//---------------------------------------------------------------------------

