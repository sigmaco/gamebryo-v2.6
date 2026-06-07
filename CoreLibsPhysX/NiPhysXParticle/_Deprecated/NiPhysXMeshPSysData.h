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

#ifndef NIPHYSXMESHPSYSDATA_H
#define NIPHYSXMESHPSYSDATA_H

#include "NiPhysXParticleLibType.h"

#include <NiMeshPSysData.h>

#include "NiPhysXActorDesc.h"

#include <NiPhysX.h>

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. The data it previously
    contained is now found in the NiPhysXPSMeshParticleSystem class.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXMeshPSysData : public NiMeshPSysData
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    /// Destructor.
    virtual ~NiPhysXMeshPSysData();

    /// @cond EMERGENT_INTERNAL

    NiPhysXScenePtr GetScene() const;

    bool GetFillActorPoolsOnLoad();

    bool GetKeepsActorMeshes();

    NiUInt32 GetDefaultActorPoolsSize();

    void GetActorPoolRegion(NiPoint3& kCenter, NiPoint3& kDim);

    NiPhysXActorDesc* GetActorDescAt(NiUInt32 uiIndex) const;

    NiUInt32 GetActorDescCount() const;

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    // For streaming only.
    NiPhysXMeshPSysData();
    
    NiPhysXScenePtr m_spScene;

    bool m_bFillActorPoolsOnLoad;
    bool m_bKeepsActorMeshes;
    NiUInt32 m_uiDefaultActorPoolSize;
    NiPoint3 m_kActorPoolDim;
    NiPoint3 m_kActorPoolCenter;

    NiTObjectArray<NiPhysXActorDescPtr> m_kActorDescriptors;
};

NiSmartPointer(NiPhysXMeshPSysData);

#include "NiPhysXMeshPSysData.inl"

/// @endcond

#endif  // #ifndef NIPHYSXMESHPSYSDATA_H
