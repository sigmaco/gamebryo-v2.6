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
#include "NiBoxSphereIntersector.h"
#include "NiSqrDistance.h"

//----------------------------------------------------------------------------
NiBoxSphereIntersector::NiBoxSphereIntersector(const NiBoxBV& kBox,
    const NiSphereBV& kSphere, float fMaxTime, float fDerivativeTimeStep,
    float fPseudodistanceThreshold, int iMaxIterations)
    :
    NiIntersector(fMaxTime, fDerivativeTimeStep, fPseudodistanceThreshold,
        iMaxIterations),
    m_kBox(kBox),
    m_kSphere(kSphere)
{
    m_fInvRadiusSqr = 1.0f / (kSphere.GetRadius() * kSphere.GetRadius());

    for (int i = 0; i < 3; i++)
        m_afParameter[i] = NI_INFINITY;
}
//----------------------------------------------------------------------------
float NiBoxSphereIntersector::Pseudodistance(const NiPoint3& kV0,
    const NiPoint3& kV1, float fTime) const
{
    NiBox kMBox = m_kBox.GetBox();
    kMBox.m_kCenter += fTime * kV0;
    NiPoint3 kMCenter = m_kSphere.GetCenter() + fTime * kV1;

    float fPseudodistance = NiSqrDistance::Compute(kMCenter, kMBox,
        m_afParameter[0], m_afParameter[1], m_afParameter[2]) *
        m_fInvRadiusSqr - 1.0f;

    return fPseudodistance;
}
//----------------------------------------------------------------------------
void NiBoxSphereIntersector::ComputeContactInformation(
    const NiPoint3& kV0, const NiPoint3& kV1)
{
    if (m_eIntersectionType == IT_CONTACT)
    {
        m_kContactPoint = m_kBox.GetCenter() + m_fContactTime * kV0;
        for (int i = 0; i < 3; i++)
            m_kContactPoint += m_afParameter[i] * m_kBox.GetAxis(i);

        // Use the direction vector from the sphere center to the closest
        // point for the normal vector.
        NiPoint3 kMCenter = m_kSphere.GetCenter() + m_fContactTime * kV1;
        m_kContactNormal = kMCenter - m_kContactPoint;
        m_kContactNormal.Unitize();
    }
    else  // m_eIntersectionType == IT_OVERLAP
    {
        float fEpsilon = 1e-05f;
        float fDeltaVSqr = kV1.SqrLength() - kV0.SqrLength();

        if (fDeltaVSqr < -fEpsilon)
        {
            // The box is moving faster than the sphere.  Choose the
            // contact point on the sphere.
            NiPoint3 kV0Unit = kV0;
            kV0Unit.Unitize();
            NiPoint3 kNormal1 = (m_kBox.GetCenter() -
                (2.0f * m_kSphere.GetRadius()) * kV0Unit) -
                m_kSphere.GetCenter();
            kNormal1.Unitize();
            m_kContactPoint = m_kSphere.GetCenter() +
                kNormal1 * m_kSphere.GetRadius();
            m_kContactNormal = -kNormal1;
            return;
        }

        if (fDeltaVSqr > fEpsilon) 
        {
            // The sphere is moving faster than the box.  Choose the
            // contact point on the box.
            m_kContactPoint = m_kBox.GetCenter();
            for (int i = 0; i < 3; i++)
                m_kContactPoint += m_afParameter[i] * m_kBox.GetAxis(i);
        }
        else
        {
            // The box and sphere are moving with the same velocity.  Choose
            // the contact point to be the average of the centers.
            m_kContactPoint = (m_kSphere.GetCenter() + m_kBox.GetCenter())
                * 0.5f;
        }

        // Use the direction vector from the sphere center to the closest
        // point for the normal vector.
        m_kContactNormal =  m_kSphere.GetCenter() - m_kContactPoint;
        m_kContactNormal.Unitize();
    }
}
//----------------------------------------------------------------------------
