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

#ifndef NICAPSULECAPSULEINTERSECTOR_H
#define NICAPSULECAPSULEINTERSECTOR_H

#include "NiIntersector.h"
#include "NiCapsuleBV.h"

class NICOLLISION_ENTRY NiCapsuleCapsuleIntersector : public NiIntersector
{
public:
    // Construction of an intersector object, which uses a Newton's root
    // finding method applied to a pseudodistance between two moving capsules
    // to determine the first time of contact between the objects.  The time
    // interval of interest is [0,maxTime].  See the documentation in
    // NiIntersector.h that describes the input parameters after maxTime.
    NiCapsuleCapsuleIntersector(const NiCapsuleBV& kCapsule0,
        const NiCapsuleBV& kCapsule1, float fMaxTime,
        float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);

private:
    // A pseudodistance between two capsules is a continuous function that is
    // positive if the objects are separated, zero if the objects are just in
    // contact, or negative if the objects overlap.  The member function
    // computes the pseudodistance for the two objects displaced by the
    // specified velocities at the specified time.  In the case of capsules,
    // the pseudodistance is p = (d^2/(r0+r1)^2 - 1), where d^2 is the squared
    // distance between the capsule segments and r0 and r1 are the radii of
    // the capsules.
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const;

    // If the two objects intersect, whether IT_CONTACT or IT_OVERLAP, this
    // function computes or estimates a contact point and a contact normal at
    // the contact time.
    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1);

    const NiCapsuleBV& m_kCapsule0;
    const NiCapsuleBV& m_kCapsule1;

    // Cache the value 1/(r0+r1)^2 for the pseudodistance evaluations, where
    // r0 and r1 are the radii of the capsules.
    float m_fInvRadiusSumSqr;

    // These are the parameters on the capsule line segments which produce
    // the closest points in the calculation of the squared distance between
    // the segments.  The parameters are needed to compute the contact point
    // for the capsule-capsule intersection.
    mutable float m_fParameter0, m_fParameter1;

private:
    NiCapsuleCapsuleIntersector & operator=( const NiCapsuleCapsuleIntersector & );
};

#endif
