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

#ifndef NIPSYSEMITTER_H
#define NIPSYSEMITTER_H

#include "NiPSysModifier.h"
#include <NiColor.h>

class NiParticleInfo;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSEmitter.
*/
class NIPARTICLE_ENTRY NiPSysEmitter : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    float GetSpeed() const;
    float GetSpeedVar() const;
    float GetDeclination() const;
    float GetDeclinationVar() const;
    float GetPlanarAngle() const;
    float GetPlanarAngleVar() const;
    NiColorA GetInitialColor() const;
    float GetInitialRadius() const;
    float GetRadiusVar() const;
    float GetLifeSpan() const;
    float GetLifeSpanVar() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysEmitter();

    float m_fSpeed;
    float m_fSpeedVar;
    float m_fDeclination;
    float m_fDeclinationVar;
    float m_fPlanarAngle;
    float m_fPlanarAngleVar;
    NiColorA m_kInitialColor;
    float m_fInitialRadius;
    float m_fRadiusVar;
    float m_fLifeSpan;
    float m_fLifeSpanVar;
};

NiSmartPointer(NiPSysEmitter);

/// @endcond

#include "NiPSysEmitter.inl"

#endif  // #ifndef NIPSYSEMITTER_H
