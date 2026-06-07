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
float NiSqrDistance::Compute(const NiTrigon& kTri, 
    const NiParallelogram& kPgm, float& fTriParam0, float& fTriParam1, 
    float& fPgmParam0, float& fPgmParam1)
{
    float fMin;
    float fMin0;
    float fTmpTriParam0;
    float fTmpTriParam1;
    float fTmpPgmParam0;
    float fTmpPgmParam1;
    NiSegment kSeg;

    // compare edges of kTri against all of kPgm
    kSeg.m_kOrigin = kTri.m_kOrigin;
    kSeg.m_kDirection = kTri.m_kEdge0;
    fMin = NiSqrDistance::Compute(kSeg, kPgm, fTriParam0, fPgmParam0, 
        fPgmParam1);
    fTriParam1 = 0;

    kSeg.m_kDirection = kTri.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kPgm, fTmpTriParam1, fTmpPgmParam0, 
        fTmpPgmParam1);
    fTmpTriParam0 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTriParam0 = fTmpTriParam0;
        fTriParam1 = fTmpTriParam1;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kSeg.m_kOrigin = kSeg.m_kOrigin + kTri.m_kEdge0;
    kSeg.m_kDirection = kSeg.m_kDirection - kTri.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kPgm, fTmpTriParam1, fTmpPgmParam0, 
        fTmpPgmParam1);
    fTmpTriParam0 = 1 - fTmpTriParam1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTriParam0 = fTmpTriParam0;
        fTriParam1 = fTmpTriParam1;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    // compare edges of kPgm against all of kTri
    kSeg.m_kOrigin = kPgm.m_kOrigin;
    kSeg.m_kDirection = kPgm.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri, fTmpPgmParam0, fTmpTriParam0, 
        fTmpTriParam1);
    fTmpPgmParam1 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTriParam0 = fTmpTriParam0;
        fTriParam1 = fTmpTriParam1;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kSeg.m_kDirection = kPgm.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri, fTmpPgmParam1, fTmpTriParam0, 
        fTmpTriParam1);
    fTmpPgmParam0 = 0;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTriParam0 = fTmpTriParam0;
        fTriParam1 = fTmpTriParam1;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge1;
    kSeg.m_kDirection = kPgm.m_kEdge0;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri, fTmpPgmParam0, fTmpTriParam0, 
        fTmpTriParam1);
    fTmpPgmParam1 = 1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTriParam0 = fTmpTriParam0;
        fTriParam1 = fTmpTriParam1;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    kSeg.m_kOrigin = kPgm.m_kOrigin + kPgm.m_kEdge0;
    kSeg.m_kDirection = kPgm.m_kEdge1;
    fMin0 = NiSqrDistance::Compute(kSeg, kTri, fTmpPgmParam1, fTmpTriParam0, 
        fTmpTriParam1);
    fTmpPgmParam0 = 1;
    if (fMin0 < fMin)
    {
        fMin = fMin0;
        fTriParam0 = fTmpTriParam0;
        fTriParam1 = fTmpTriParam1;
        fPgmParam0 = fTmpPgmParam0;
        fPgmParam1 = fTmpPgmParam1;
    }

    return fMin;
}
//---------------------------------------------------------------------------

