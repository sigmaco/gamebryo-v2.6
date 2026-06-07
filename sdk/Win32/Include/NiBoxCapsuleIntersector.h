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

#ifndef NIBOXCAPSULEINTERSECTOR_H
#define NIBOXCAPSULEINTERSECTOR_H

#include "NiIntersector.h"
#include "NiBoxBV.h"
#include "NiCapsuleBV.h"

class NICOLLISION_ENTRY NiBoxCapsuleIntersector : public NiIntersector
{
public:
    NiBoxCapsuleIntersector(const NiBoxBV& kBox, const NiCapsuleBV& kCapsule,
        float fMaxTime, float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);

private:
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const;

    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1);

    const NiBoxBV& m_kBox;
    const NiCapsuleBV& m_kCapsule;
    float m_fInvRadiusSqr;
    mutable float m_afParameter[3];

private:
    NiBoxCapsuleIntersector & operator=( const NiBoxCapsuleIntersector & );
};

#endif
