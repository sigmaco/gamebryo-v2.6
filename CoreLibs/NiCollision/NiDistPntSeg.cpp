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
float NiSqrDistance::Compute(const NiPoint3& kPnt, const NiSegment& kSeg,
    float& fSegParam)
{
    NiPoint3 kDiff = kPnt - kSeg.m_kOrigin;
    fSegParam = kSeg.m_kDirection.Dot(kDiff);

    if (fSegParam <= 0.0f)
    {
        fSegParam = 0.0f;
    }
    else
    {
        float fLenSqr = kSeg.m_kDirection.SqrLength();
        if (fSegParam >= fLenSqr)
        {
            fSegParam = 1.0f;
            kDiff -= kSeg.m_kDirection;
        }
        else
        {
            fSegParam /= fLenSqr;
            kDiff -= fSegParam * kSeg.m_kDirection;
        }
    }

    return kDiff.SqrLength();
}
//---------------------------------------------------------------------------

