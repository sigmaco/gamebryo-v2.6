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

#ifndef NIPARTICLESYSTEM_H
#define NIPARTICLESYSTEM_H

#include <NiParticleLibType.h>
#include <NiParticles.h>
#include "NiPSysModifier.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSParticleSystem.
*/
class NIPARTICLE_ENTRY NiParticleSystem : public NiParticles
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    void AddModifier(NiPSysModifier* pkModifier);
    NiPSysModifier* GetModifierByName(const NiFixedString& kName);
    unsigned int GetModifierCount() const;
    NiPSysModifier* GetModifierAt(unsigned int uiIndex);
    bool GetWorldSpace() const;
    unsigned short GetMaxNumParticles() const;
    unsigned short GetNumParticles() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiParticleSystem();

    bool m_bWorldSpace;
    NiTPointerList<NiPSysModifierPtr> m_kModifierList;
    float m_fLastTime;
    bool m_bResetSystem;
    bool m_bDynamicBounds;
    NiTransform m_kUnmodifiedWorld;
};

NiSmartPointer(NiParticleSystem);

/// @endcond

#include "NiParticleSystem.inl"

#endif  // #ifndef NIPARTICLESYSTEM_H
