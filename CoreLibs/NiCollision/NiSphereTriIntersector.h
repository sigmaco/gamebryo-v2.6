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

#ifndef NISPHERETRIINTERSECTOR_H
#define NISPHERETRIINTERSECTOR_H

#include "NiIntersector.h"
#include "NiSphereBV.h"
#include "NiTrigon.h"

class NICOLLISION_ENTRY NiSphereTriIntersector : public NiIntersector
{
public:
    NiSphereTriIntersector(const NiSphereBV& kSphere, const NiPoint3& kP0,
        const NiPoint3& kP1, const NiPoint3& kP2, float fMaxTime,
        float fDerivativeTimeStep = 0.001f,
        float fPseudodistanceThreshold = 0.0001f, int iMaxIterations = 32);

private:
    virtual float Pseudodistance(const NiPoint3& kV0, const NiPoint3& kV1,
        float fTime) const;

    virtual void ComputeContactInformation(const NiPoint3& kV0,
        const NiPoint3& kV1);

    const NiSphereBV& m_kSphere;
    NiTrigon m_kTri;

    float m_fInvRadiusSqr;
    mutable float m_fParameter0, m_fParameter1;

private:
    NiSphereTriIntersector & operator=( const NiSphereTriIntersector & );
};

#endif
