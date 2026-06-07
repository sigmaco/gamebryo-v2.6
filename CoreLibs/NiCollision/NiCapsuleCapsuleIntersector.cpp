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
#include "NiCapsuleCapsuleIntersector.h"
#include "NiSqrDistance.h"

//----------------------------------------------------------------------------
NiCapsuleCapsuleIntersector::NiCapsuleCapsuleIntersector(
    const NiCapsuleBV& kCapsule0, const NiCapsuleBV& kCapsule1,
    float fMaxTime, float fDerivativeTimeStep, float fPseudodistanceThreshold,
    int iMaxIterations)
    :
    NiIntersector(fMaxTime, fDerivativeTimeStep, fPseudodistanceThreshold,
        iMaxIterations),
    m_kCapsule0(kCapsule0),
    m_kCapsule1(kCapsule1)
{
    float fRadiusSum = kCapsule0.GetRadius() + kCapsule1.GetRadius();
    m_fInvRadiusSumSqr = 1.0f / (fRadiusSum * fRadiusSum);

    m_fParameter0 = NI_INFINITY;
    m_fParameter1 = NI_INFINITY;
}
//----------------------------------------------------------------------------
float NiCapsuleCapsuleIntersector::Pseudodistance(const NiPoint3& kV0,
    const NiPoint3& kV1, float fTime) const
{
    NiSegment kSegment0 = m_kCapsule0.GetSegment();
    NiSegment kSegment1 = m_kCapsule1.GetSegment();
    kSegment0.m_kOrigin += fTime * kV0;
    kSegment1.m_kOrigin += fTime * kV1;

    float fPseudodistance = NiSqrDistance::Compute(kSegment0, kSegment1,
        m_fParameter0, m_fParameter1) * m_fInvRadiusSumSqr - 1.0f;

    return fPseudodistance;
}
//----------------------------------------------------------------------------
void NiCapsuleCapsuleIntersector::ComputeContactInformation(
    const NiPoint3& kV0, const NiPoint3& kV1)
{
    NiPoint3 kSeg0 = m_kCapsule0.GetSegment().GetPoint(m_fParameter0);
    NiPoint3 kSeg1 = m_kCapsule1.GetSegment().GetPoint(m_fParameter1);

    if (m_eIntersectionType == IT_CONTACT)
    {
        kSeg0 += m_fContactTime * kV0;
        kSeg1 += m_fContactTime * kV1;
        m_kContactNormal = kSeg1 - kSeg0;
        m_kContactNormal.Unitize();
        m_kContactPoint = kSeg0 + m_kCapsule0.GetRadius() * m_kContactNormal;
    }
    else // m_eIntersectionType == IT_OVERLAP
    {
        // Capsules are initially intersecting.  Approximate the intersection
        // by the midpoint of the line segment connecting the two closest
        // points on the capsule axes.
        m_kContactPoint = 0.5f * (kSeg0 + kSeg1);

        if (kSeg1 != kSeg0)
        {
            m_kContactNormal = kSeg1 - kSeg0;
            m_kContactNormal.Unitize();
        }
        else
        {
            m_kContactNormal = NiPoint3::UNIT_Z;
        }
    }
}
//----------------------------------------------------------------------------
