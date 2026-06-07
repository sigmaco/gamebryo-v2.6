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


#ifndef NIPHYSXBODYDESC_H
#define NIPHYSXBODYDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

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
    NiPhysXBodyDesc objects are for caching snapshots of NxBodyDesc objects,
    used for streaming, cloning and restoring simulation state.
    
    Applications may need to use this class to manipulate the body state
    between streaming and creating the PhysX content. It is a sub-part
    of NiPhysXActorDesc, and hence may be needed when creating such objects. 

    Internally, this class is internally a wrapper for an NxBodyDesc object
    and arrays of initial velocities. 
*/
class NIPHYSX_ENTRY NiPhysXBodyDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareClone(NiPhysXBodyDesc);
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareViewerStrings;
    /// @endcond

public:
    /// The new object has a default NxBodyDesc and no stored velocity states.
    NiPhysXBodyDesc();
    
    /// Destructor frees all memory.
    virtual ~NiPhysXBodyDesc();
    
    /**
        Pushes the cached state into a PhysX body descriptor, which can
        subsequently be used as part of an actor descriptor or modified
        and used with the SetFromBodyDesc function.

        @param kBodyDesc The body descriptor to copy state to.
        @param uiIndex The index of the initial velocity set to use.
        @param kXform A rotation transform to apply to the velocities.
    */
    virtual void ToBodyDesc(NxBodyDesc& kBodyDesc, const NiUInt32 uiIndex,
        const NxMat33& kXform) const;

    /**
        Captures state from a PhysX body descriptor.

        The velocities in the descriptor are always stored at index 0.
        The kXform argument is a rotation to apply to the velocities before
        storage.
    */
    virtual void SetFromBodyDesc(const NxBodyDesc& kBodyDesc,
        const NxMat33& kXform);


    /**
        @name Initial State Management
        
        This object can store multiple sets of initial velocities for the
        actor. Any one of these may be used when creating an actor or an
        actor may be set to have one of these velocities on demand. The body
        is always assumed to have the same number of states as the
        NiPhysXActorDesc to which it is attached. Therefore, any call to
        NiPhysXActorDesc::SetPose shoudl have a matching call to
        NiPhysXBodyDesc::SetVelocities if the actor is dynamic.
        
        If no velocities are provided, the actor is assumed to have zero
        angular and linear velocity.
    */
    /// @{
    
    /// Get the velocities stored at a given index.
    void GetVelocities(NxVec3& kLinearVelocity, NxVec3& kAngularVelocity,
        const NiUInt32 uiIndex) const;

    /**
        Set the velocities at a given index.
        
        The given transform is appied to the velocities before storage.
        More space is allocated when the requested index is beyond the
        number already stored. All the newly allocated states up to the
        given index, if not already set, are filled with zero vectors.
    **/
    void SetVelocities(const NxVec3& kLinearVelocity,
        const NxVec3& kAngularVelocity, const NiUInt32 uiIndex,
        const NxMat33& kXform);

    /**
        Add new velocities taken from the actor's current velocity.
        
        The actor's current velocities are extracted, multiplied by the given
        rotation, and stored in the next unused state index. The index for
        the newly stored state is returned. The actor must have been
        created or set for this function to do anything.
    */
    virtual NiUInt32 AddState(NxActor* pkActor, const NxMat33& kXform);

    /**
        Restore the actor's velocities to those stored in the given index.
        
        Applications should rarely, if ever, use this function. It is called
        through NiPhysXActorDesc::RestoreState.
        
        The stored pose is first multiplied by the given rotation.
    */
    virtual void RestoreState(NxActor* pkActor, const NiUInt32 uiIndex,
        const NxMat33& kXform);

    /// Remove all the stored states starting at and beyond the given index.
    virtual void RemoveStatesBeyond(const NiUInt32 uiState);

    /// Remove the stored state at the given index, and shuffle down other
    /// states to fill any hole in stored state indexes.
    virtual void RemoveState(const NiUInt32 uiState);

    /// @}

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    /// All the body values are stored in the PhysX descriptor.
    NxBodyDesc m_kBodyDesc;
    
    /// Stored velocities.
    NiUInt32 m_uiNumVels;
    NxVec3* m_pkLinVels;
    NxVec3* m_pkAngVels;
};

NiSmartPointer(NiPhysXBodyDesc);

#include "NiPhysXBodyDesc.inl"

#endif // NIPHYSXBODYDESC_H

