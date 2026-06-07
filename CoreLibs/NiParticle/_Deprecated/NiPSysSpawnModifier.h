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

#ifndef NIPSYSSPAWNMODIFIER_H
#define NIPSYSSPAWNMODIFIER_H

#include "NiPSysModifier.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSSpawner.
*/
class NIPARTICLE_ENTRY NiPSysSpawnModifier : public NiPSysModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    unsigned short GetNumSpawnGenerations() const;
    float GetPercentageSpawned() const;
    unsigned short GetMinNumToSpawn() const;
    unsigned short GetMaxNumToSpawn() const;
    float GetSpawnSpeedChaos() const;
    float GetSpawnDirChaos() const;
    float GetLifeSpan() const;
    float GetLifeSpanVar() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysSpawnModifier();

    unsigned short m_usNumSpawnGenerations;
    float m_fPercentageSpawned;
    unsigned short m_usMinNumToSpawn;
    unsigned short m_usMaxNumToSpawn;
    float m_fSpawnSpeedChaos;
    float m_fSpawnDirChaos;
    float m_fLifeSpan;
    float m_fLifeSpanVar;
};

/// @endcond

#include "NiPSysSpawnModifier.inl"

#endif  // #ifndef NIPSYSSPAWNMODIFIER_H
