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

#ifndef NIMESHPSYSDATA_H
#define NIMESHPSYSDATA_H

#include "NiPSysData.h"
#include <NiNode.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSMeshParticleSystem.
*/
class NIPARTICLE_ENTRY NiMeshPSysData : public NiPSysData
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    bool GetFillPoolsOnLoad();
    unsigned int GetDefaultPoolsSize();
    unsigned int GetNumGenerations();
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiMeshPSysData();

    NiNodePtr m_spParticleMeshes;
    bool m_bFillPoolsOnLoad;
    unsigned int m_uiDefaultPoolSize;
    unsigned int m_uiNumGenerations;
};

NiSmartPointer(NiMeshPSysData);

/// @endcond

#include "NiMeshPSysData.inl"

#endif  // #ifndef NIMESHPSYSDATA_H
