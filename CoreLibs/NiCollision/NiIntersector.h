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

#ifndef NIINTERSECTOR_H
#define NIINTERSECTOR_H

#include "NiCollisionLibType.h"
#include "NiPoint3.h"

class NICOLLISION_ENTRY NiIntersector : public NiMemObject
{
public:
    // Construction of an intersector object, which uses a Newton's root
    // finding method applied to a pseudodistance between two moving objects
    // to determine the first time of contact between the objects.  The time
    // interval of interest is [0,maxTime].  The pseudodistance is F(t), and
    // is deemed to be close enough to zero when |F(t)| <= e, where e is
    // pseudodistanceThreshold.  Newton's method requires an evaluation of
    // the derivative, F'(t).  We instead estimate the derivative by a
    // backward finite difference:  F'(t) = (F(t) - F(t-h))/h.  The step size
    // h is derivativeTimeStep.  The value maxIterations is the maximum
    // number of iterations that is allowed in Newton's method to guarantee
    // a bounded amount of time spent searching for the contact time.
    //
    // The default values are reasonable for most situations.  For very fast
    // moving objects, you might want to increase maxIterations (at an
    // increase in computation time) or decrease pseudodistanceThreshold (at
    // a decrease in accuracy for the contact time).
    NiIntersector(float fMaxTime, float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);
    virtual ~NiIntersector();

    // Intersection queries.  The Test query defaults to calling Find.  If
    // a derived class has a Test algorithm that is more efficient to call
    // than Find, it should override Test.  If a derived class has a Find
    // algorithm that is faster than the Newton's root finder used in the
    // base class, it should override Find.
    virtual void Test(const NiPoint3& kV0, const NiPoint3& kV1);
    virtual void Find(const NiPoint3& kV0, const NiPoint3& kV1);

    enum IntersectionType
    {
        // A Test or Find query was not called yet, so the contact time,
        // point, and normal are invalid.
        IT_INVALID,

        // The objects are separated, so the contact set is the empty set.
        IT_EMPTY,

        // The objects are just touching, but not interpenetrating.
        IT_CONTACT,

        // The objects are overlapping.
        IT_OVERLAP
    };

    IntersectionType GetIntersectionType() const;

    // The first time of contact for IT_CONTACT or zero for IT_OVERLAP.
    inline float GetContactTime() const;

    // The contact point for IT_CONTACT or the estimate of a point of
    // intersection for IT_OVERLAP.
    inline const NiPoint3& GetContactPoint() const;

    // The contact normal for object0.  The contact normal for object1 is
    // the negative of this.
    inline const NiPoint3& GetContactNormal() const;

protected:
    // A pseudodistance between two objects is a continuous function that is
    // positive if the objects are separated, zero if the objects are just in
    // contact, or negative if the objects overlap.  The member function
    // computes the pseudodistance for the two objects displaced by the
    // specified velocities at the specified time.  Each derived class must
    // override this function.
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const = 0;

    // If the two objects intersect, whether IT_CONTACT or IT_OVERLAP, the
    // derived class must compute or estimate a contact point and a contact
    // normal at the contact time.
    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1) = 0;

    // The interval of interest for the intersection query is [0,maxTime].
    float m_fMaxTime;

    // The derivative of the pseudodistance is estimated using a backward
    // finite difference:  F'(t) = (F(t) - F(t-h))/h.  The value h is the
    // derivativeTimeStep.  The reciprocal 1/h is stored to avoid dividing
    // by h multiple times when multiple derivative estimates are needed.
    float m_fDerivativeTimeStep, m_fInvDerivativeTimeStep;

    // A Newton's root finding method is used to determine the time of
    // contact as a root to F(t) = 0, where F is the pseudodistance function.
    // The values of F are considered close enough to zero when |F| <= e,
    // where e is the pseudodistanceThreshold.
    float m_fPseudodistanceThreshold;

    // At most maxIterations are performed in the root finding to guarantee
    // a bounded amount of time spent in determining the contact time.  The
    // iterates are t[0] = 0, and
    //     t[i+1] = t[i] - F(t[i])/F'(t[i]), i >= 0
    // The derivative is estimated by F'(t) = (F(t) - F(t-h))/h, so the
    // numberical method is actually the Secant root finding method.
    int m_iMaxIterations;

    // Information about the contact time, point, normal, and what type of
    // intersection occurs (none, just in contact, overlapping).
    IntersectionType m_eIntersectionType;
    float m_fContactTime;
    NiPoint3 m_kContactPoint, m_kContactNormal;
};

#include "NiIntersector.inl"

#endif
