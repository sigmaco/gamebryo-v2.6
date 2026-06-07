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


#ifndef NIPHYSXFLUIDEMITTERDESC_H
#define NIPHYSXFLUIDEMITTERDESC_H

#include "NiPhysXFluidLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXActorDesc.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#include <fluids/NxFluid.h>

/**
    NiPhysXFluidEmitterDesc objects are the Gamebryo descriptor objects for
    a PhysX NxFluidEmitter object.
    
    Each instance stores a NxFluidEmitterDesc object, a pointer to the
    NxFluidEmitter it is describing (after it has been instantiated), and a
    pointer to a NiPhysXActorDesc object if the emitter is attached to an
    actor's frame. 

    An object of this type is created and streamed for each emitter when a
    fluid particle system is exported from the art tools. The most common
    application usage of this class is to modify parameters between
    the loading of a NiPhysXScene and the instantiation of PhysX content
    using NiPhysXScene::CreateSceneFromSnapshot(). This class can also be
    used to clone fluid data in tandem with the scene graph that is rendering
    the fluid. 

    This class does not stream userData stored in the NxFluidEmitter.
    Applications should derive from this class if explicit handling of
    userData is required. 
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidEmitterDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXFluidEmitterDesc);
    /// @endcond
    
public:

    /**
        Constructor.
        
        The emitter description after construction is identical to the
        PhysX default NxFluidEmitterDesc. 
    */
    NiPhysXFluidEmitterDesc();
    
    /// Destructor.
    ///
    /// Releases all memory, and sets smart pointers to 0.
    virtual ~NiPhysXFluidEmitterDesc();

    /**
       Get the PhysX descriptor for the emitter.
       
       The frame actor in the descriptor is not valid; use the functions
       below to access the frame actor. The userData member is blindly
       copied by this class. 
    */
    virtual NxFluidEmitterDesc GetNxFluidEmitterDesc() const;

    /**
       Set the PhysX descriptor for the emitter.
       
       The frame actor in the descriptor is not copied; use the functions
       below to access the frame actor. The userData member is blindly
       copied by this class. The name member is internally copied and
       accessed through functions described below. These functions have
       no effect on the internal pointer to the NxFluidEmitter object.
       Hence, modifications to this data will have no effect on an emitter
       that has already been instantiated.
       
       In cases where you wish to modify an existing emitter, obtain a pointer
       to it using GetEmitter and modify the pointer's target directly.
    */
    virtual void SetNxFluidEmitterDesc(const NxFluidEmitterDesc& kEmitterDesc);

    /// Obtain a pointer to the emitter that this descriptor controls.
    ///
    /// This pointer will be NULL before the scene has been instantiated. 
    NxFluidEmitter* GetEmitter() const;

    /**
        Given an existing emitter, this function saves all the necessary
        information to recreate it.
        
        This function does more than NxFluidEmitter::saveToDesc. It also
        correctly ties the emitter to the Gamebryo descriptor for the frame
        shape. The transformation kXform is applied to global pose information
        to enable saving in different coordinate systems. The map of NxShape
        objects to NiPhysXShapeDesc objects is used to find the Gamebryo
        descriptors corresponding to actors. 
    */
    virtual void SetEmitter(NxFluidEmitter* pkEmitter, const NxMat34& kXform,
        NiTMap<NxShape*, NiPhysXShapeDescPtr>& kShapeMap);

    /// Get the name of the emitter.
    NiFixedString GetName() const;
    
    /**
        Set the name of this fluid emitter.
        
        The name is also set when SetNxFluidEmitterDesc or SetEmitter are
        called. 
    */
    void SetName(const NiFixedString& kName);

    /**
        Get the frame reference shape that is used to set the pose of this
        emitter during simulation.
        
        The return value is a smart pointer. The return may be null if the
        emitter is static (attached to the world frame).
    */
    NiPhysXShapeDescPtr GetFrameShape() const;
    
    /**
        Set the frame reference shape that is used to set the pose of this
        emitter during simulation.
        
        The value will be stored in a smart pointer, so applications must not
        explicitly free the pointer. Set the value to null for an emitter
        that is static.
    */
    void SetFrameShape(NiPhysXShapeDesc* pkShapeDesc);

    /// @cond EMERGENT_INTERNAL

    virtual NxFluidEmitter* CreateEmitter(NxFluid* pkFluid,
        const NxMat34& kXform);
    virtual void RemoveEmitter(NxFluid* pkFluid);
    
    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
        
protected:
    virtual void ToFluidEmitterDesc(
        NxFluidEmitterDesc& kEmitterDesc, const NxMat34& kXform);
    
    // Constant through the life of an fluid
    NxFluidEmitterDesc m_kDesc;
    NiPhysXShapeDescPtr m_spFrameShape;
    NiFixedString m_kName;
    
    // Pointer to the PhysX objects.
    NxFluidEmitter* m_pkEmitter;

    // Legacy loading code
    NiPhysXActorDescPtr m_spDeprecatedFrameActor;
};

NiSmartPointer(NiPhysXFluidEmitterDesc);

#include "NiPhysXFluidEmitterDesc.inl"

#endif // #ifdef NIPHYSXFLUIDEMITTERDESC_H
