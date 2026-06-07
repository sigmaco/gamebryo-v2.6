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

#ifndef NIPSYSCOLLIDER_H
#define NIPSYSCOLLIDER_H

#include <NiParticleLibType.h>
#include <NiObject.h>

class NiPSysSpawnModifier;
class NiPSysColliderManager;
NiSmartPointer(NiPSysCollider);

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSCollider.
*/
class NIPARTICLE_ENTRY NiPSysCollider : public NiObject
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    float GetBounce() const;
    bool GetSpawnOnCollide() const;
    bool GetDieOnCollide() const;
    NiPSysSpawnModifier* GetSpawnModifier() const;
    NiPSysCollider* GetNext() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysCollider();

    float m_fBounce;
    bool m_bSpawnOnCollide;
    bool m_bDieOnCollide;
    NiPSysSpawnModifier* m_pkSpawnModifier;
    NiPSysColliderManager* m_pkManager;
    NiPSysColliderPtr m_spNext;
};

/// @endcond

#include "NiPSysCollider.inl"

#endif  // #ifndef NIPSYSCOLLIDER_H
