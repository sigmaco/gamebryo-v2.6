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
#include "NiCapsuleSphereIntersector.h"
#include "NiSqrDistance.h"

//----------------------------------------------------------------------------
NiCapsuleSphereIntersector::NiCapsuleSphereIntersector(
    const NiCapsuleBV& kCapsule, const NiSphereBV& kSphere,
    float fMaxTime, float fDerivativeTimeStep, float fPseudodistanceThreshold,
    int iMaxIterations)
    :
    NiIntersector(fMaxTime, fDerivativeTimeStep, fPseudodistanceThreshold,
        iMaxIterations),
    m_kCapsule(kCapsule),
    m_kSphere(kSphere)
{
    float fRadiusSum = kCapsule.GetRadius() + kSphere.GetRadius();
    m_fInvRadiusSumSqr = 1.0f / (fRadiusSum * fRadiusSum);

    m_fParameter = NI_INFINITY;
}
//----------------------------------------------------------------------------
float NiCapsuleSphereIntersector::Pseudodistance(const NiPoint3& kV0,
    const NiPoint3& kV1, float fTime) const
{
    NiSegment kMSegment = m_kCapsule.GetSegment();
    kMSegment.m_kOrigin += fTime * kV0;
    NiPoint3 kMCenter = m_kSphere.GetCenter() + fTime * kV1;

    float fPseudodistance = NiSqrDistance::Compute(kMCenter, kMSegment,
        m_fParameter) * m_fInvRadiusSumSqr - 1.0f;

    return fPseudodistance;
}
//----------------------------------------------------------------------------
void NiCapsuleSphereIntersector::ComputeContactInformation(
    const NiPoint3& kV0, const NiPoint3& kV1)
{
    NiPoint3 kSeg = m_kCapsule.GetSegment().GetPoint(m_fParameter);
    NiPoint3 kCen = m_kSphere.GetCenter();

    if (m_eIntersectionType == IT_CONTACT)
    {
        kSeg += m_fContactTime * kV0;
        kCen += m_fContactTime * kV1;
        m_kContactNormal = kCen - kSeg;
        m_kContactNormal.Unitize();
        m_kContactPoint = kSeg + m_kCapsule.GetRadius() * m_kContactNormal;
    }
    else // m_eIntersectionType == IT_OVERLAP
    {
        // Capsule and sphere are initially intersecting.  Approximate the
        // intersection by the midpoint of the line segment connecting the
        // closest point on the capsule axis to the sphere center.
        // points on the capsule axes.
        m_kContactPoint = 0.5f * (kSeg + kCen);

        if (kSeg != kCen)
        {
            m_kContactNormal = kCen - kSeg;
            m_kContactNormal.Unitize();
        }
        else
        {
            m_kContactNormal = NiPoint3::UNIT_Z;
        }
    }
}
//----------------------------------------------------------------------------
