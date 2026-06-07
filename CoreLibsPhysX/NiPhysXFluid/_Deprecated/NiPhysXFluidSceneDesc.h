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

#ifndef NIPHYSXFLUIDSCENEDESC_H
#define NIPHYSXFLUIDSCENEDESC_H

#include "NiPhysXFluidLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXSceneDesc.h"
#include "NiPhysXFluidDesc.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. Its functionality has
    been replaced with the NiPhysXFluidProp class.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidSceneDesc : public NiPhysXSceneDesc
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareStream;
    NiDeclareRTTI;

    /// @endcond

public:
    NiPhysXFluidSceneDesc();
    virtual ~NiPhysXFluidSceneDesc();
    
    // Convert this thing to a NiPhysXFluidProp in the given scene.
    // The scene must be the same one that this is the descriptor for.
    void ConvertToProp(NiPhysXScene* pkScene);

    /// @cond EMERGENT_INTERNAL
        
    // There is no state caching for fluids, because the PhysX API does not
    // support the user removal of particles from a simulation.
    
    virtual bool StreamCanSkip();

    /// @endcond

protected:
    // This class adds a fluid descriptor
    NiTObjectArray<NiPhysXFluidDescPtr> m_kFluids;
};

NiSmartPointer(NiPhysXFluidSceneDesc);

/// @endcond

#include "NiPhysXFluidSceneDesc.inl"

#endif // NIPHYSXFLUIDSCENEDESC_H
