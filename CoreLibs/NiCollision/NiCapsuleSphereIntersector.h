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

#ifndef NICAPSULESPHEREINTERSECTOR_H
#define NICAPSULESPHEREINTERSECTOR_H

#include "NiIntersector.h"
#include "NiCapsuleBV.h"
#include "NiSphereBV.h"

class NICOLLISION_ENTRY NiCapsuleSphereIntersector : public NiIntersector
{
public:
    NiCapsuleSphereIntersector(const NiCapsuleBV& kCapsule,
        const NiSphereBV& kSphere, float fMaxTime,
        float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);

private:
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const;

    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1);

    const NiCapsuleBV& m_kCapsule;
    const NiSphereBV& m_kSphere;

    float m_fInvRadiusSumSqr;
    mutable float m_fParameter;

private:
    NiCapsuleSphereIntersector & operator=( const NiCapsuleSphereIntersector & );
};

#endif
