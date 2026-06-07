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

#ifndef NIPSYSAGEDEATHMODIFIER_H
#define NIPSYSAGEDEATHMODIFIER_H

#include "NiPSysModifier.h"

class NiPSysSpawnModifier;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSParticleSystem and NiPSSimulatorFinalStep.
*/
class NIPARTICLE_ENTRY NiPSysAgeDeathModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    bool GetSpawnOnDeath() const;
    NiPSysSpawnModifier* GetSpawnModifier() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysAgeDeathModifier();

    bool m_bSpawnOnDeath;
    NiPSysSpawnModifier* m_pkSpawnModifier;
};

NiSmartPointer(NiPSysAgeDeathModifier);

/// @endcond

#include "NiPSysAgeDeathModifier.inl"

#endif  // #ifndef NIPSYSAGEDEATHMODIFIER_H
