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

#include "NiCollisionPCH.h"
#include "NiSqrDistance.h"

//---------------------------------------------------------------------------
float NiSqrDistance::Compute(const NiPoint3& kPoint, const NiBox& kBox,
    float& fBoxParam0, float& fBoxParam1, float& fBoxParam2)
{
    float fSqrDistance = 0.0f;

    // Compute the squared distance in the box's coordinate system.
    NiPoint3 kDiff = kPoint - kBox.m_kCenter, kClosest;
    for (int i = 0; i < 3; i++)
    {
        float fDelta;
        kClosest[i] = kDiff.Dot(kBox.m_akAxis[i]);
        if (kClosest[i] < -kBox.m_afExtent[i])
        {
            fDelta = kClosest[i] + kBox.m_afExtent[i];
            fSqrDistance += fDelta*fDelta;
            kClosest[i] = -kBox.m_afExtent[i];
        }
        else if (kClosest[i] > kBox.m_afExtent[i])
        {
            fDelta = kClosest[i] - kBox.m_afExtent[i];
            fSqrDistance += fDelta*fDelta;
            kClosest[i] = kBox.m_afExtent[i];
        }
    }

    fBoxParam0 = kClosest[0];
    fBoxParam1 = kClosest[1];
    fBoxParam2 = kClosest[2];

    return fSqrDistance;
}
//---------------------------------------------------------------------------
