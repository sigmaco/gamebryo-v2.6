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
#include "NiBoxCapsuleIntersector.h"
#include "NiSqrDistance.h"

//----------------------------------------------------------------------------
NiBoxCapsuleIntersector::NiBoxCapsuleIntersector(const NiBoxBV& kBox,
    const NiCapsuleBV& kCapsule, float fMaxTime, float fDerivativeTimeStep,
    float fPseudodistanceThreshold, int iMaxIterations)
    :
    NiIntersector(fMaxTime, fDerivativeTimeStep, fPseudodistanceThreshold,
        iMaxIterations),
    m_kBox(kBox),
    m_kCapsule(kCapsule)
{
    m_fInvRadiusSqr = 1.0f / (kCapsule.GetRadius() * kCapsule.GetRadius());

    for (int i = 0; i < 3; i++)
        m_afParameter[i] = NI_INFINITY;
}
//----------------------------------------------------------------------------
float NiBoxCapsuleIntersector::Pseudodistance(const NiPoint3& kV0,
    const NiPoint3& kV1, float fTime) const
{
    NiBox kMBox = m_kBox.GetBox();
    kMBox.m_kCenter += fTime * kV0;
    NiSegment kMSegment = m_kCapsule.GetSegment();
    kMSegment.m_kOrigin += fTime * kV1;

    float fS;
    float fPseudodistance = NiSqrDistance::Compute(kMSegment, kMBox, fS,
        m_afParameter[0], m_afParameter[1], m_afParameter[2]) *
        m_fInvRadiusSqr - 1.0f;

    return fPseudodistance;
}
//----------------------------------------------------------------------------
void NiBoxCapsuleIntersector::ComputeContactInformation(
    const NiPoint3& kV0, const NiPoint3& kV1)
{
    m_kContactPoint = m_kBox.GetCenter() + m_fContactTime * kV0;
    int i, aiSign[3];
    for (i = 0; i < 3; i++)
    {
        m_kContactPoint += m_afParameter[i] * m_kBox.GetAxis(i);

        if (m_afParameter[i] == -m_kBox.GetExtent(i))
            aiSign[i] = -1;
        else if (m_afParameter[i] == m_kBox.GetExtent(i))
            aiSign[i] = 1;
        else
            aiSign[i] = 0;
    }

    NiPoint3 kRelVel = kV1 - kV0;
    m_kContactNormal = m_kBox.GetNormal(aiSign[0], aiSign[1], aiSign[2],
        kRelVel);
}
//----------------------------------------------------------------------------
