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


#ifndef NIPHYSXFLUIDDESC_H
#define NIPHYSXFLUIDDESC_H

#include "NiPhysXFluidLibType.h"

#include <NiMain.h>
#include <NiSystem.h>
#include <NiParticle.h>

#include "NiPhysXFluidEmitterDesc.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#include <fluids/NxFluid.h>

/**
    NiPhysXFluidDesc objects are the Gamebryo descriptor objects for a
    PhysX NxFluid object.
    
    They store a NxFluidDesc object, a pointer to the NxFluid we are
    describing (after it has been instantiated) and additional
    data required to attach a fluid to a Gamebryo scene graph. Streaming
    and cloning is supported. 

    An object of this type is created and streamed when a fluid particle
    system is exported from the art tools. The most common application
    usage of this class will be to modify parameters between the loading
    of a NiPhysXScene and the instantiation of PhysX content using
    NiPhysXScene::CreateSceneFromSnapshot(). See the PhysXFluids demo
    for an example of using this class.
    
    This class may also be used to clone fluid data in tandem with the scene
    graph that is rendering the fluid. 

    This class does not stream userData stored in the NxFluid. Applications
    should derive from this class if explicit handling of userData is required.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXFluidDesc);
    /// @endcond

public:

    /**
        Constructor.

        The fluid description after construction is identical to the PhysX
        default NxFluidDesc. 
    */
    NiPhysXFluidDesc();
    
    /// Destructor.
    virtual ~NiPhysXFluidDesc();

    /**
        Get the PhysX descriptor for the fluid.

        The emitters in the descriptor are not valid; use the functions below
        to access emitters.
    */
    virtual NxFluidDesc GetNxFluidDesc() const;

    /**
        Extract data from the PhysX descriptor for the fluid.

        The emitters in the descriptor are not extracted; use the functions
        below to access emitters. The name member is internally copied and
        accessed thorugh functions described below.
        
        This function has no effect on the emitters or the internal pointer
        to the NxFluid object. Hence, using this function will have no effect
        on a fluid that has already been instantiated. Use GetFluid to get a
        pointer to the NxFluid directly, and then modify that if you would
        like to modify properties on an existing fluid object.
    */
    virtual void SetNxFluidDesc(const NxFluidDesc& kFluidDesc);

    /// Obtain a pointer to the fluid that this descriptor controls.
    ///
    /// This pointer will be NULL before the scene has been instantiated. 
    NxFluid* GetFluid() const;

    /**
        Given an existing fluid system, this creates all the necessary
        information to recreate it.
        
        This function does more than NxFluid::saveToDesc. It also captures
        all the emitters and correctly ties them to shapes. The transformation
        kXform is applied to global pose information to enable saving in
        different coordinate systems. The map of NxShape objects to
        NiPhysXShapeDesc objects is used to find the Gamebryo descriptors
        corresponding to shapes.
    */
    virtual void SetFluid(NxFluid* pkFluid, const NxMat34& kXform,
        NiTMap<NxShape*, NiPhysXShapeDescPtr>& kShapeMap);

    /// Get the emitter array for this fluid.
    ///
    /// Returns a reference to the internal array.
    NiTObjectArray<NiPhysXFluidEmitterDescPtr>& GetEmitters();

    /**
        Set the emitter array for this fluid.

        Copies the given array of smart pointers.
        
        This function has no effect on the emitters of an existing NxFluid
        object. Use GetFluid to get a pointer to the NxFluid directly, and
        then modify that if you would like to modify properties on an existing
        fluid object.
    */
    void SetEmitters(
        const NiTObjectArray<NiPhysXFluidEmitterDescPtr>& kEmitters);
        
    /// Get the name of this fluid.
    NiFixedString GetName() const;

    /// Set the name of this fluid.
    /// 
    /// The name is also set when SetNxFluidDesc or SetFluid are called.
    void SetName(const NiFixedString& kName);

    /// Get the particle system that is to be used to render this fluid.
    NiPSParticleSystemPtr GetParticleSystem() const;

    /**
        Set the particle system that is being used to render this fluid.
        
        The system must be of type NiPhysXPSFluidSystem or
        NiPhysXPSMeshFluidSystem. The particle system pointer is used to
        find the NxParticleData for this fluid. If the particle system's
        particle data contains a non-zero number of particles at the time
        the PhysX fluid is instantiated, then the fluid will be instantiated
        with those particle positions. After initialization, this particle
        system will receive the particle positions for rendering.
    */
    void SetParticleSystem(NiPSParticleSystem* pkPSys);
    
    /// @cond EMERGENT_INTERNAL

    virtual NxFluid* CreateFluid(NxScene* pkScene,
        NxCompartment* pkCompartment, const NxMat34& kXform);
    virtual void RemoveFluid(NxScene* pkScene);
    
    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
        
protected:
    NiFixedString m_kName;
    NxFluidDesc m_kDesc;
    NiTObjectArray<NiPhysXFluidEmitterDescPtr> m_kEmitters;
    
    // Pointer to the PhysX object
    NxFluid* m_pkFluid;
    
    // Particle system object
    NiPSParticleSystem* m_pkPSys;
};

NiSmartPointer(NiPhysXFluidDesc);

#include "NiPhysXFluidDesc.inl"

#endif // NIPHYSXFLUIDDESC_H
