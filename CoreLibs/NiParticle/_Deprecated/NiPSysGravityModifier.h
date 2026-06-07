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

#ifndef NIPSYSGRAVITYMODIFIER_H
#define NIPSYSGRAVITYMODIFIER_H

#include "NiPSysModifier.h"
#include <NiPoint3.h>

class NiAVObject;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSGravityForce.
*/
class NIPARTICLE_ENTRY NiPSysGravityModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    enum ForceType
    {
        FORCE_PLANAR,
        FORCE_SPHERICAL
    };
    NiAVObject* GetGravityObj() const;
    const NiPoint3& GetGravityAxis() const;
    float GetDecay() const;
    float GetStrength() const;
    ForceType GetType() const;
    float GetTurbulence();
    float GetTurbulenceScale();
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysGravityModifier();

    NiAVObject* m_pkGravityObj;
    NiPoint3 m_kGravityAxis;
    float m_fDecay;
    float m_fStrength;
    ForceType m_eType;
    float m_fTurbulence;
    float m_fScale;
};

NiSmartPointer(NiPSysGravityModifier);

/// @endcond

#include "NiPSysGravityModifier.inl"

#endif  // #ifndef NIPSYSGRAVITYMODIFIER_H
