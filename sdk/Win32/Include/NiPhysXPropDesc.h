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


#ifndef NIPHYSXPROPDESC_H
#define NIPHYSXPROPDESC_H

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXActorDesc.h"
#include "NiPhysXBodyDesc.h"
#include "NiPhysXClothDesc.h"
#include "NiPhysXD6JointDesc.h"
#include "NiPhysXMaterialDesc.h"
#include "NiPhysXShapeDesc.h"

//#include "NiPhysXProp.h"
class NiPhysxProp;

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXPropDesc objects are for caching the data required to create
    PhysX objects and attach them to a scene graph.
    
    In addition, they can store multiple copies of pose and velocity
    information. They are the objects streamed to save or load PhysX actors
    (rigid, cloth and fluid) and are the objects stored as snapshots in
    NiPhysXProp objects. Instances of this class are typically created in
    art tools, or modified after loading from a stream but before
    instantiating the PhysX.
    
    NiPhysXPropDesc also implement cloning behavior for PhysX content.
*/
class NIPHYSX_ENTRY NiPhysXPropDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXPropDesc);
    NiDeclareFlags(NiUInt8);
    /// @endcond

public:
    /**
        Constructor.
        
        @param uiMaterialHashSize The size of the hash table used to store
            materials. It should be a prime number that is greater than the
            total number of materials needed for the prop, or could be smaller
            if memory consumption is more important that speed.
        @param uiStateMapSize The size of the hash table used to map saved
            state indexes onto names. It should be a prime number that is
            greater than the number of saved states, or smaller if memory is
            of concern.
    */
    NiPhysXPropDesc(NiUInt32 uiMaterialHashSize = 7,
        NiUInt32 uiStateMapSize = 3);
    
    /// Destructor.
    virtual ~NiPhysXPropDesc();

    /// Get the flag controlling hardware targeting.
    ///
    /// If true, the creator of the prop expected hardware simulation.
    bool GetUseHardware() const;
    
    /// Set the flag indicating that simulation should occur on hardware.
    void SetUseHardware(const bool bHardware);

    /**
        @name Compartment Management Functions
        
        These functions allow you to specify which simulation compartment
        the contents of this descriptor should be created in. The identifier
        is the one used with the NiPhysXScene object when the compartment
        was registered, or 0 if the contents should be created in the
        primary scene.
        
        See the PhysX documentation for the rules for creating
        different types of content in compartments.
    */
    
    /// @{

    /// Get the rigid body compartment ID.
    NiUInt32 GetRBCompartmentID() const;

    /// Set the rigid body compartment ID.
    void SetRBCompartmentID(const NiUInt32 uiID);

    /// Get the cloth compartment ID.
    NiUInt32 GetClothCompartmentID() const;

    /// Set the cloth compartment ID.
    void SetClothCompartmentID(const NiUInt32 uiID);

    /// Get the fluid compartment ID.
    NiUInt32 GetFluidCompartmentID() const;

    /// Set the fluid compartment ID.
    void SetFluidCompartmentID(const NiUInt32 uiID);
    /// @}

    /**
        @name Actor Array Funtions
        
        These functions provide access to the array of actors stored in
        this object. Applications may wish to access these to modify the
        actor descriptions before the prop is instantiated in a scene, or
        when creating a prop in code.
        
        Note that the internal array stores smart pointers to the objects.
    */
    /// @{

    /// Returns the number of actors in the array.
    NiUInt32 GetActorCount() const;
    
    /// Returns the actor at a given index in the array.
    NiPhysXActorDescPtr GetActorAt(NiUInt32 uiIndex);

    /**
        Add an actor descriptor to the array.
        
        @return The actor's location in the array.
    */
    NiUInt32 AddActor(NiPhysXActorDesc* pkActorDesc);
    
    /**
        Remove the actor descriptor at the given index.
        
        Returns the smart pointer at the given index, and has
        NiTArray::RemoveAtAndFill semantics. 
    */
    NiPhysXActorDescPtr RemoveActorAt(NiUInt32 uiIndex);
    /// @}

    /**
        @name Joint Array Funtions
        
        These functions provide access to the array of joints stored in
        this object. Applications may wish to access these to modify the
        joint descriptions before the prop is instantiated in a scene, or
        when creating a prop in code.
        
        Note that the internal array stores smart pointers to the objects.
    */
    /// @{

    /// Returns the number of joint descriptors in the array.
    NiUInt32 GetJointCount() const;

    /// Returns the joint at a given index in the array.
    NiPhysXJointDescPtr GetJointAt(NiUInt32 uiIndex);

    /**
        Add a joint descriptor to the array.
        
        @return The joint's location in the array.
    */
    NiUInt32 AddJoint(NiPhysXJointDesc* pkJointDesc);
    
    /**
        Remove the joint descriptor at the given index.
        
        Returns the smart pointer at the given index, and has
        NiTArray::RemoveAtAndFill semantics. 
    */
    NiPhysXJointDescPtr RemoveJointAt(NiUInt32 uiIndex);
    /// @}

    /**
        @name Cloth Array Funtions
        
        These functions provide access to the array of cloth descriptors
        stored in this object. Applications may wish to access these to modify
        the cloth descriptions before the prop is instantiated in a scene, or
        when creating a prop in code.
        
        Note that the internal array stores smart pointers to the objects.
    */
    /// @{

    /// Returns the number of cloth descriptors in the array.
    NiUInt32 GetClothCount() const;

    /// Returns the cloth descriptor at a given index in the array.
    NiPhysXClothDescPtr GetClothAt(NiUInt32 uiIndex);

    /**
        Add a cloth descriptor to the array.
        
        @return The cloth's location in the array.
    */
    NiUInt32 AddCloth(NiPhysXClothDesc* pkClothDesc);
    
    /**
        Remove the cloth descriptor at the given index.
        
        Returns the smart pointer at the given index, and has
        NiTArray::RemoveAtAndFill semantics. 
    */
    NiPhysXClothDescPtr RemoveClothAt(NiUInt32 uiIndex);
    /// @}

    /**
        @name Material Map Functions
        
        These functions provide access to the internal map of materials.
        The map is keyed by the material index and the value is an
        NiPhysXMaterialDesc smart pointer.
    */
    
    /// @{
        
    /**
        Add an additional material to the map.
        
        AddMaterial first searches for a material matching kMaterial in the
        map. If found, it returns the corresponding material index which can
        be used in shapes to specify their materials. If not found, it adds
        the material in the first empty slot greater than or equal to
        usBaseIndex.
    */
    NxMaterialIndex AddMaterial(NxMaterialDesc& kMaterial,
        const NxMaterialIndex usBaseIndex = 0);

    /**
        Set the material at a specific index.
        
        SetMaterialAt inserts the given material descriptor
        at the specified index. It returns the material descriptor that
        already existed at that index, or 0 if none.
    */
    NiPhysXMaterialDescPtr SetMaterialAt(NxMaterialDesc& kMaterial,
        const NxMaterialIndex usIndex);

    /**
        Obtain the material for a specific index.
        
        GetMaterial returns the material at the given index, or 0 if there
        is no material with that index.
    */
    NiPhysXMaterialDescPtr GetMaterial(const NxMaterialIndex usIndex);
    
    /// Get an iterator initialized to the first element in the map.
    NiTMapIterator GetFirstMaterial();

    /// Obtain the next material when iterating through the map.
    void GetNextMaterial(NiTMapIterator& kPos, NxMaterialIndex& usIndex,
        NiPhysXMaterialDescPtr& spMaterial);

    /**
        Remove a material.
        
        RemoveMaterial removes the material with the given index, returning
        true if it was found in the map. The remove decrements the smart
        pointer on the material descriptor at that index, which does not
        necessarily delete it.
    */
    bool RemoveMaterial(const NxMaterialIndex usIndex);
    
    /// @}

    /**
        @name State Management Functions
        
        The actors and cloth objects stored within the prop each may contain
        multiple sets of initial conditions. These functions provide
        convenience in accessing those states, including a mapping from
        state indexes to state names.
        
        Every actor and cloth piece in the prop must have the same number of
        stored states.
    */
    /// @{
    
    /// Get the number of states stored in each actor or cloth object.
    NiUInt32 GetNumStates() const;

    /// Set the number of states stored in this prop.
    ///
    /// The same number of states must be stored in each actor or cloth object.
    void SetNumStates(const NiUInt32 uiNumStates);

    /// Return the index that is used for a particular state name.
    ///
    /// If the name does not match any state, 0 is returned.
    NiUInt32 GetStateIndex(const NiFixedString& kName);

    /// Set the name to use for a particular state.
    ///
    /// Returns false if the name is already in use.
    bool SetStateName(NiUInt32 uiIndex, NiFixedString& kName);

    /// Get the first named state.
    NiTMapIterator GetFirstStateName();

    /// Get the next named state.
    void GetNextStateName(NiTMapIterator& kPos, NiFixedString& kName,
        NiUInt32& uiIndex);
        
    /**
        Remove a state name.
        
        This just removes the name from the map of names to indexes, it does
        not remove the state itself.
    */
    bool RemoveStateName(NiFixedString& kName);

    /**
        Adds a new set of state from the scene, and return the index of the
        new state.
        
        The given transform is applied to all the state before it is saved.
        The name is added to the mapping from state indexes to names.
    */
    virtual NiUInt32 AddState(NxScene* pkPhysXScene, 
        const NxMat34& kXform, NiFixedString& kName);

    /**
        Restore state to the given scene.

        The index tells which of the stored states to restore from. The
        transformation is applied to all state before it is pushed into the
        actors and cloth. The keep meshes flag is needed when cloth is
        restored. It will only be used if the cloth was, for some reason,
        previously not in existence. See NiPhysXMeshDesc for the meaning of
        the argument.
    */
    virtual void RestoreState(NxScene* pkScene, const NiUInt32 uiIndex,
        const NxMat34& kXform, const bool bKeepMeshes);

    /**
        Remove a single stored state.
        
        All subsequent states are shuffled down, so indexes and state name
        mappings will change. The state name remapping is performed internally.
    */
    virtual void RemoveState(NiUInt32 uiState);

    /// @}

    /**
        @name State Keep/Delete Functions
        
        These functions control what happens to saved states once
        they have been used to create PhysX content. When these flags are
        disabled, state is deleted after the actors, cloth, etc are
        created in the PhysX scene. This will prevent subsequent attempts
        to restore the PhysX system to its initial conditions, and will
        prevent cloning from functioning correctly for the prop. Note
        that the prop can still be cloned before PhysX content is
        instantiated, regardless of these settings.
            
        The default behavior is to keep only the default state (index 0).
    */
    
    /// @{

    /// Set the prop to keep all states, or not.
    void SetKeepAllStates(const bool bKeepAllStates);
    
    /// Get whether or not the prop will keep all states.
    bool GetKeepAllStates() const;
    
    /// Set the prop to keep only the first state (regardless, of this value,
    /// all states will be kept if SetKeepAllStates was set true).
    void SetKeepDefaultState(const bool bKeepDefaultState);

    /// Get whether or not the prop will keep the default state.
    bool GetKeepDefaultState() const;
    
    /// }@

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
        
        The actors, joints and materials in this descriptor are all added
        into the existing scene.
    
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
        NxCompartment* pkFluidCompartment,
        const bool bKeepMeshes = false, const NiUInt32 uiIndex = 0);
    
    /// Remove any actors, joints and materials that this descriptor knows
    /// about from the given PhysX scene.
    virtual void RemoveFromScene(NxScene* pkScene);

    /// @}
    
    
    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// @endcond
            
protected:
    /// Flags for internal use
    enum {
        KEEP_ALL_STATES_MASK = 0x01, /// Keep all states after creation.
        KEEP_DEFAULT_STATE_MASK = 0x02, /// Keep default state after creation.
        TARGET_HARDWARE_MASK = 0x04 /// Is the prop targeted at hardware?
    };

    // Updates the actor pointers in NiPhysXSrc and NiPhysXDest objects.
    virtual void PushToSrcDest() const;
    
    /// Remove states as indicated by the internal flags.
    virtual void RemoveStates();

    // Components of a scene
    NiTObjectArray<NiPhysXActorDescPtr> m_kActors;
    NiTObjectArray<NiPhysXJointDescPtr> m_kJoints;
    NiTObjectArray<NiPhysXClothDescPtr> m_kClothes;
    NiTMap<NxMaterialIndex, NiPhysXMaterialDescPtr> m_kMaterials;
    
    // The compartment to create this prop in
    NiUInt32 m_uiRBCompartmentID;
    NiUInt32 m_uiClothCompartmentID;
    NiUInt32 m_uiFluidCompartmentID;
    
    // The number of states we have for restoration
    NiUInt32 m_uiNumStates;
    
    // Map from state names to state indexes
    NiTFixedStringMap<NiUInt32> m_kStateNameMap;
};

#include "NiPhysXPropDesc.inl"

#endif // NIPHYSXPROPDESC_H
