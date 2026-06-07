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

#ifndef NIPSYSDATA_H
#define NIPSYSDATA_H

#include <NiParticleLibType.h>
#include <NiParticlesData.h>
#include "NiParticleInfo.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSParticleSystem.
*/
class NIPARTICLE_ENTRY NiPSysData : public NiParticlesData
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    virtual ~NiPSysData();

    // *** begin Emergent internal use only ***
    NiParticleInfo* GetParticleInfo();
    const NiParticleInfo* GetParticleInfo() const;
    float* GetRotationSpeeds();
    const float* GetRotationSpeeds() const;
    unsigned short GetMaxNumParticles() const;
    unsigned short GetNumParticles() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysData();

    NiParticleInfo* m_pkParticleInfo;
    float* m_pfRotationSpeeds;
    unsigned short m_usNumAddedParticles;
    unsigned short m_usAddedParticlesBase;
};

NiSmartPointer(NiPSysData);

/// @endcond

#include "NiPSysData.inl"

#endif  // #ifndef NIPSYSDATA_H
