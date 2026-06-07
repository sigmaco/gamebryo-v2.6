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

#ifndef NIPHYSXPSYSDATA_H
#define NIPHYSXPSYSDATA_H

#include "NiPhysXParticleLibType.h"

#include <NiMeshPSysData.h>
#include <NiPhysX.h>

#include "NiPhysXActorDesc.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. The data it previously
    held is now owned by NiPhysXPSParticleSystem objects.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXPSysData : public NiPSysData
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    /// Destructor.
    virtual ~NiPhysXPSysData();

    /// @cond EMERGENT_INTERNAL

    NiPhysXScenePtr GetScene() const;

    bool GetFillActorPoolOnLoad();

    bool GetKeepsActorMeshes();

    NiUInt32 GetDefaultActorPoolSize();

    void GetActorPoolRegion(NiPoint3& kCenter, NiPoint3& kDim);

    NiPhysXActorDesc* GetActorDesc() const;

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    // For streaming only.
    NiPhysXPSysData();
      
    NiPhysXScenePtr m_spScene;

    bool m_bFillActorPoolOnLoad;
    bool m_bKeepsActorMeshes;
    NiUInt32 m_uiDefaultActorPoolSize;

    NiPoint3 m_kActorPoolCenter;
    NiPoint3 m_kActorPoolDim;

    NiPhysXActorDescPtr m_spActorDescriptor;
};

NiSmartPointer(NiPhysXPSysData);

#include "NiPhysXPSysData.inl"

/// @endcond

#endif  // #ifndef NIPHYSXPSYSDATA_H
