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


#ifndef NIPHYSXFLUIDPROPDESC_H
#define NIPHYSXFLUIDPROPDESC_H

#include "NiPhysXFluidLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXPropDesc.h"

#include "NiPhysXSceneDesc.h"
#include "NiPhysXFluidDesc.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>

/**
    NiPhysXFluidPropDesc objects add support for fluid actors to the
    NiPhysXPropDesc class.
    
    They store an internal array of smart pointers to NiPhysXFluidDesc objects.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidPropDesc : public NiPhysXPropDesc
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXFluidPropDesc);
    /// @endcond

public:
    /// Constructor.
    ///
    /// The hash table size is passed to the base class constructor.
    NiPhysXFluidPropDesc(NiUInt32 uiMaterialHashSize = 29);
    
    /// Destructor.
    virtual ~NiPhysXFluidPropDesc();

    /// Return the number of fluids in this prop. 
    NiUInt32 GetNumFluids() const;

    /**
        Return the fluid at the given index.
    
        There is no range checking - the index must be less than the value
        returned by GetNumFluids().
    */
    NiPhysXFluidDescPtr GetFluidDesc(NiUInt32 uiIndex);

    /// Add a fluid to the internal array. 
    void AddFluidDesc(NiPhysXFluidDesc* pkFluidDesc);

    /**
        Remove a fluid at the specified location from the internal array
        and return it.
        
        This fills the gap in the array left by the removed fluid.
    */
    NiPhysXFluidDescPtr RemoveFluidDesc(NiUInt32 uiIndex);

    /**
        @name Content Instantiation Functions
        
        These functions control the creation and removal of PhysX content
        from a scene. They should rarely, if ever, be called directly from
        an application. Rather, they are called by NiPhysXScene functions
        when a prop is added to or removed from the scene.
    */
    /// @{
    
    /**
        Create a PhysX scene based on this descriptor.
        
        The base class function is called first, and then fluids are
        created in the scene.
    
        @param pkScene The scene to instantiate content in.
        @param kXform A transformation to apply to all the PhysX content
            before instantiating.
        @param pkRBCompartment The rigid body compartment to use for the
            actors and joints in the scene.
        @param pkClothCompartment The compartment to use for the cloth
            objects in this descriptor.
        @param pkFluidCompartment The compartment to use for the any fluids
            in this scene (available for derived classes).
        @param bKeepMeshes If true, mesh data is kept after creation, otherwise
            it is deleted. See the documentation for NiPhysXMeshDesc.
        @param uiIndex The index for the state to use when setting poses,
            velocities and other initial conditions.
    */
    virtual bool CreateInScene(NxScene* pkScene, const NxMat34& kXform,
        NxCompartment* pkRBCompartment, NxCompartment* pkClothCompartment,
        NxCompartment* pkFluidCompartment, const bool bKeepMeshes = false,
        const NiUInt32 uiIndex = 0);
    
    /// Remove any fluids from the scene, and then call the base class version.
    virtual void RemoveFromScene(NxScene* pkScene);

    /**
        Reset the fluid and then restore other state to the given scene.

        There is no state caching for fluids, because the memory overhead
        would be large and we see little utility for most users. Instead, this
        funciton removes all the particles from the simulation and resets the
        particle system before calling the base class version of the function.
    */
    virtual void RestoreState(NxScene* pkScene, const NiUInt32 uiIndex,
        const NxMat34& kXform, const bool bKeepMeshes);

    /// @}

    
    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// @endcond
            
protected:
    // This class adds a fluid descriptor
    NiTObjectArray<NiPhysXFluidDescPtr> m_kFluids;
};

NiSmartPointer(NiPhysXFluidPropDesc);

#include "NiPhysXFluidPropDesc.inl"

#endif // NIPHYSXFLUIDPROPDESC_H
