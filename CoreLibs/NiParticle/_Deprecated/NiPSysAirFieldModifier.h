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

#ifndef NIPSYSAIRFIELDMODIFIER_H
#define NIPSYSAIRFIELDMODIFIER_H

#include "NiPSysFieldModifier.h"
#include <NiPoint3.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSAirFieldForce.
*/
class NIPARTICLE_ENTRY NiPSysAirFieldModifier : public NiPSysFieldModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    const NiPoint3& GetDirection() const;
    void SetDirection(const NiPoint3& kDirection);
    float GetAirFriction() const;
    float GetInheritVelocity() const;
    bool GetInheritRotation() const;
    bool GetComponentOnly() const;
    bool GetEnableSpread() const;
    float GetSpread() const;
    // *** begin Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysAirFieldModifier();

    NiPoint3 m_kDirection;
    NiPoint3 m_kUnitDirection;
    float m_fAirFriction;
    float m_fInheritVelocity;
    bool m_bInheritRotation;
    bool m_bComponentOnly;
    bool m_bEnableSpread;
    float m_fSpread;
};

NiSmartPointer(NiPSysAirFieldModifier);

/// @endcond

#include "NiPSysAirFieldModifier.inl"

#endif  // #ifndef NIPSYSAIRFIELDMODIFIER_H
