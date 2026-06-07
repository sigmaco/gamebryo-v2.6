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

#ifndef NIPHYSXMESHFLUIDDATA_H
#define NIPHYSXMESHFLUIDDATA_H

#include "NiPhysXFluidLibType.h"

#include <NiMeshPSysData.h>
#include <NiPhysX.h>

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#include <fluids/NxFluid.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. Its functionality has
    been replaced, approximately, with the NiPhysXPSFluidSystem.
*/
class NIPHYSXFLUID_ENTRY NiPhysXMeshFluidData : public NiMeshPSysData
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    virtual ~NiPhysXMeshFluidData();

    /// @cond EMERGENT_INTERNAL

    NxParticleData& GetFluidData();

    NxU32 GetMaxNumParticles();

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    // For streaming only.
    NiPhysXMeshFluidData();

    NxParticleData m_kFluidData;
    NxU32 m_uiMaxParticles;
};

NiSmartPointer(NiPhysXMeshFluidData);

/// @endcond

#include "NiPhysXMeshFluidData.inl"

#endif  // #ifndef NIPHYSXMESHFLUIDDATA_H
