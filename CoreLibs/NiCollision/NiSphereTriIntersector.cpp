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
#include "NiSphereTriIntersector.h"
#include "NiSqrDistance.h"

//----------------------------------------------------------------------------
NiSphereTriIntersector::NiSphereTriIntersector(const NiSphereBV& kSphere,
    const NiPoint3& kP0, const NiPoint3& kP1, const NiPoint3& kP2,
    float fMaxTime, float fDerivativeTimeStep, float fPseudodistanceThreshold,
    int iMaxIterations)
    :
    NiIntersector(fMaxTime, fDerivativeTimeStep, fPseudodistanceThreshold,
        iMaxIterations),
    m_kSphere(kSphere)
{
    m_kTri.m_kOrigin = kP0;
    m_kTri.m_kEdge0 = kP1 - kP0;
    m_kTri.m_kEdge1 = kP2 - kP0;
    m_fInvRadiusSqr = 1.0f / (kSphere.GetRadius() * kSphere.GetRadius());
    m_fParameter0 = NI_INFINITY;
    m_fParameter1 = NI_INFINITY;
}
//----------------------------------------------------------------------------
float NiSphereTriIntersector::Pseudodistance(const NiPoint3& kV0,
    const NiPoint3& kV1, float fTime) const
{
    NiPoint3 kMCenter = m_kSphere.GetCenter() + fTime * kV0;
    NiTrigon kMTri = m_kTri;
    kMTri.m_kOrigin += fTime * kV1;

    float fPseudodistance = NiSqrDistance::Compute(kMCenter, kMTri,
        m_fParameter0, m_fParameter1) * m_fInvRadiusSqr - 1.0f;

    return fPseudodistance;
}
//----------------------------------------------------------------------------
void NiSphereTriIntersector::ComputeContactInformation(
    const NiPoint3&, const NiPoint3& kV1)
{
    m_kContactPoint = m_kTri.m_kOrigin + m_fParameter0 * m_kTri.m_kEdge0 +
        m_fParameter1 * m_kTri.m_kEdge1;

    m_kContactNormal = -m_kTri.GetNormal();

    if (m_eIntersectionType == IT_CONTACT)
        m_kContactPoint += m_fContactTime * kV1;
}
//----------------------------------------------------------------------------
