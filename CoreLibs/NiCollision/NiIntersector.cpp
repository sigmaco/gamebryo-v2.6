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
#include "NiIntersector.h"

//----------------------------------------------------------------------------
NiIntersector::NiIntersector(float fMaxTime, float fDerivativeTimeStep,
    float fPseudodistanceThreshold, int iMaxIterations)
    :
    m_kContactPoint(NI_INFINITY, NI_INFINITY, NI_INFINITY),
    m_kContactNormal(NI_INFINITY, NI_INFINITY, NI_INFINITY)
{
    m_fMaxTime = (fMaxTime > 0.0f ? fMaxTime : FLT_EPSILON);

    m_fMaxTime = fMaxTime;
    m_fDerivativeTimeStep = fDerivativeTimeStep;
    m_fInvDerivativeTimeStep = 1.0f / fDerivativeTimeStep;
    m_fPseudodistanceThreshold = fPseudodistanceThreshold;
    m_iMaxIterations = iMaxIterations;
    m_eIntersectionType = IT_INVALID;
    m_fContactTime = NI_INFINITY;
}
//----------------------------------------------------------------------------
NiIntersector::~NiIntersector()
{
    // virtual destructor defined in case derived classes need virtual
    // destruction
}
//----------------------------------------------------------------------------
void NiIntersector::Test(const NiPoint3& kV0, const NiPoint3& kV1)
{
    Find(kV0, kV1);
}
//----------------------------------------------------------------------------
void NiIntersector::Find(const NiPoint3& kV0, const NiPoint3& kV1)
{
    // Analyze the initial configuration of the objects.
    float fT0 = 0.0f;
    float fF0 = Pseudodistance(kV0, kV1, 0.0f);

    if (fF0 <= -m_fPseudodistanceThreshold)
    {
        // Objects are (significantly) overlapping.
        m_eIntersectionType = IT_OVERLAP;
        m_fContactTime = 0.0f;
        ComputeContactInformation(kV0, kV1);
        return;
    }

    if (fF0 <= FLT_EPSILON)
    {
        // Objects are (nearly) in tangential contact.
        m_eIntersectionType = IT_CONTACT;
        m_fContactTime = 0.0f;
        ComputeContactInformation(kV0, kV1);
        return;
    }

    // The objects are not currently in contact or overlapping.  If the
    // relative velocity between them is zero, they cannot intersect at a
    // later time.
    if (kV0 == kV1)
    {
        m_eIntersectionType = IT_EMPTY;
        m_fContactTime = NI_INFINITY;
        return;
    }

    // Estimate the derivative F'(t0).
    float fT1 = fT0 - m_fDerivativeTimeStep;
    float fF1 = Pseudodistance(kV0, kV1, fT1);
    float fFDer0 = (fF0 - fF1) * m_fInvDerivativeTimeStep;
    if (fFDer0 >= 0.0f)
    {
        // The objects are moving apart.
        m_eIntersectionType = IT_EMPTY;
        m_fContactTime = NI_INFINITY;
        return;
    }

    // Check if the objects are not intersecting, yet still moving
    // towards each other at maximum time.  If this is the case, the
    // objects do not intersect on the specified time interval.
    float fF2 = Pseudodistance(kV0, kV1, m_fMaxTime);
    if (fF2 > 0.0f)
    {
        // Estimate the derivative F'(tmax).
        fT1 = m_fMaxTime - m_fDerivativeTimeStep;
        fF1 = Pseudodistance(kV0, kV1, fT1);
        float fFDer1 = (fF2 - fF1) * m_fInvDerivativeTimeStep;
        if (fFDer1 < 0.0f)
        {
            // The objects are moving towards each other and do not
            // intersect during the specified time interval.
            m_eIntersectionType = IT_EMPTY;
            m_fContactTime = NI_INFINITY;
            return;
        }
    }

    // This looks like Newton's Method for root finding, but since the
    // derivative is estimated, this is really the Secant Method.
    for (int i = 1; i <= m_iMaxIterations; i++)
    {
        fT0 -= fF0 / fFDer0;
        if (fT0 > m_fMaxTime)
        {
            // The objects do not intersect during the specified time
            // interval.
            m_eIntersectionType = IT_EMPTY;
            m_fContactTime = NI_INFINITY;
            return;
        }

        fF0 = Pseudodistance(kV0, kV1, fT0);
        if (fF0 <= m_fPseudodistanceThreshold)
        {
            m_eIntersectionType = IT_CONTACT;
            m_fContactTime = fT0;
            ComputeContactInformation(kV0, kV1);
            return;
        }

        fT1 = fT0 - m_fDerivativeTimeStep;
        fF1 = Pseudodistance(kV0, kV1, fT1);
        fFDer0 = (fF0 - fF1) * m_fInvDerivativeTimeStep;
        if (fFDer0 >= 0.0f)
        {
            // The objects are moving apart.
            m_eIntersectionType = IT_EMPTY;
            m_fContactTime = NI_INFINITY;
            return;
        }
    }

    // Newton's method failed to converge, but we already tested earlier if
    // the objects were moving apart or not intersecting during the specified
    // time interval.  To reach here, the number of iterations was not
    // large enough for the desired pseudodistance threshold.  Most likely
    // this occurs when the relative speed is very large and the time step
    // for the derivative estimation needs to be smaller.
    m_eIntersectionType = IT_CONTACT;
    m_fContactTime = fT0;
    ComputeContactInformation(kV0, kV1);
}
//----------------------------------------------------------------------------
