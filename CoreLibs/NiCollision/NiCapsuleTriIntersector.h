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

#ifndef NICAPSULETRIINTERSECTOR_H
#define NICAPSULETRIINTERSECTOR_H

#include "NiIntersector.h"
#include "NiCapsuleBV.h"
#include "NiTrigon.h"

class NICOLLISION_ENTRY NiCapsuleTriIntersector : public NiIntersector
{
public:
    NiCapsuleTriIntersector(const NiCapsuleBV& kCapsule, const NiPoint3& kP0,
        const NiPoint3& kP1, const NiPoint3& kP2, float fMaxTime,
        float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);

private:
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const;

    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1);

    const NiCapsuleBV& m_kCapsule;
    NiTrigon m_kTri;

    float m_fInvRadiusSqr;
    mutable float m_fParameter0, m_fParameter1;

private:
    NiCapsuleTriIntersector & operator=( const NiCapsuleTriIntersector & );
};

#endif
