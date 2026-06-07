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

#ifndef NIPHYSXRIGIDBODYDEST_H
#define NIPHYSXRIGIDBODYDEST_H

#include "NiPhysXLibType.h"

#include <NiAVObject.h>
#include <NiPoint3.h>
#include <NiQuaternion.h>
#include <NiColor.h>
#include <NiPhysXTypes.h>

#include "NiPhysXDest.h"

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
    NiPhysXRigidBodyDest is an abstract base class for destinations that
    extract and set rigid body state (rotation and translation). 
    
    A destination in the Gamebryo-PhysX package is a link between a PhysX
    actor that is producing state, and a Gamebryo scene graph object that
    is being driven by the physics. 

    Internally, this class stores a PhysX actor, it actor parent if it has one,
    and the two most recent transformations extracted from that actor. Also
    stored are the application times for the transformations for use when
    interpolating.

    The actor parent is a PhysX actor object that corresponds to the
    scene graph parent of this destination's target node.

    If the actor parent is known, the stored transformation state is the
    relative transformation between the actor and its parent (used for
    jointed objects). Otherwise global state is stored. All stored states
    are pre-scaled by the Gamebryo-PhysX relative scale.

    A protected member function, GetTransforms, is used by the
    UpdateSceneGraph to compute the Gamebryo scene graph transform for a
    node or interpolator. The extracted transformation depends on the
    presence of the various parent pointers. All computations are designed
    to get the best estimate for the local transformation from the actor's
    coordinate system to its parents, as required by Gamebryo. 

    First, the actor parent is checked. If pkActorParent is non-NULL, then
    the stored transform is the parent actor's inverse transform composed
    with the actor's transform. 

    In the absence of a parent actor, the protected function
    NiPhysXRigidBodyDest::GetSceneGraphParent() is called to find the scene
    graph parent of the Gamebryo node the class is driving. Derived classes
    must implement this function. If the returned parent is non-NULL, then
    the stored transform is the parent node's inverse transform composed with
    the actor's Gamebryo global transform, which is computed by composing
    kRootTransform with the actor's pose. 

    If neither parent is available, then the Gamebryo global transformation
    is used, which is computed by composing kRootTransform with the actor's
    pose. 

    This class is abstract because it does not implement
    NiPhysXDest::UpdateSceneGraph or NiPhysXRigidBodyDest::GetSceneGraphParent.
*/
class NIPHYSX_ENTRY NiPhysXRigidBodyDest : public NiPhysXDest
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareAbstractClone(NiPhysXRigidBodyDest);
    /// @endcond

public:
    /**
        Constructor.
        
        The constructor sets state common to rigid body destinations. The
        internal transformations used for interpolation are set to the identity
        transforms, and the extraction times for those transformations are set
        to be in the past. 

        @param pkActor The PhysX actor from which to extract state.
        @param pkActorParent The parent of pkActor. It must be some actor
            in the PhysX scene that has the same pose as the controlled
            node's parent in the scene graph. For example, if pkActor is
            representing a character's lower arm, pkActorParent should be
            the actor for the upper arm, presuming the arms have a
            parent-child relationship in the Gamebryo scene graph. This value
            will be NULL for objects not connected to others in a hierarchical
            linkage. 
    */
    NiPhysXRigidBodyDest(NxActor* pkActor, NxActor* pkActorParent);

    /// Destructor does nothing.
    ~NiPhysXRigidBodyDest();

    /// Returns the PhysX actor the we are extracting state from. 
    NxActor* GetActor() const;

    /// Sets the PhysX actor the we are extracting state from.
    void SetActor(NxActor* pkActor);

    /// Get the parent actor.
    ///
    /// See the documentation above.
    NxActor* GetActorParent() const;

    /// Set the parent actor.
    ///
    /// See the documentation above.
    void SetActorParent(NxActor* pkParent);

    /**
        Get the sleep optimization flag.
        
        An optimization is possible if an actor is sleeping and its
        state does not need to be pushed to the Gamebryo scene graph.
        In order to use this optimization the global transformation for
        the object in the Gamebryo scene graph must also be unchanging.
        This is typically not the case for anything involved in an animated
        hierarchy. For instance, rag-doll bones cannot be sleep optimized
        because their parents in the skeleton may still be moving. This flag
        is false by default. 
    */
    bool GetOptimizeSleep() const;
    
    /// Set the sleep optimization flag.
    ///
    /// See GetSleepOptimization, above.
    void SetOptimizeSleep(const bool bOptimize);
    
    /**
        Get the times stored for the internal state.
        
        This function is intended for debugging purposes only. The index is
        either 0 or 1, and refers to the internal buffer entries. 
    */
    float GetTime(NiUInt32 uiIndex) const;

    /**
        Get the pose stored for the internal state.
        
        This function is intended for debugging purposes only. The index is
        either 0 or 1, and refers to the internal buffer entries. 
    */
    const NxMat34& GetPose(NiUInt32 uiIndex) const;

    /// Returns true if the actor is currently sleeping (for debug).
    bool GetSleeping() const;
    
    /**
        Extract the state from the actor and store it.
        
        If fT is less than or equal to the most recent stored transformation,
        and bForce is false, then nothing is done. If bForce is true, state
        is always extracted with the given time. If the actor is sleeping
        and the sleeping optimization flag is true, then this function
        operates at most once more (to extract the state for the frame on
        which the actor was put to sleep). 

        @param fT The simulation time for which we are extracting.
        @param kRootTransform The global transformation applied to the
            PhysX scene, which is not used by this function.
        @param fScalePToW A scale factor to apply to all positions
            before they are stored. 
        @param bForce If true, the update alwasy happens and the internal
            entries for interpolation are both set to have the extracted state.
    */
    virtual void UpdateFromActors(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);
        
protected:
    NiPhysXRigidBodyDest();
    
    void GetTransforms(float fTime, NiMatrix3& kRotation,
        NiPoint3& kTranslation, const NiTransform& kRootTransform);
    
    // We need the parent of the controlled scene graph node to
    // compute the local transformation, but what that is depends on
    // the type of thing we are driving. So it's virtual.
    virtual NiNode* GetSceneGraphParent() const = 0;
    
    // Provide interpolation functionality for use by sub-classes
    void Interpolate(const float fT, NiQuaternion& kRotation,
        NiPoint3& kTranslate);
    
    NxActor* m_pkActor;    
    NxActor* m_pkActorParent;
    
    // A rotating buffer of two transforms. For actors with no scene graph
    // actor parent, these are world transforms. If there is a parent, then
    // these are local transforms to the parent.
    unsigned char m_aucIndices[2];
    float m_afTimes[2];
    NxMat34 m_akPoses[2];

    /// If true, optimize state transfer for sleeping actors.
    bool m_bOptimizeSleep;
    
    /// If true, we are already known to be sleeping.
    bool m_bSleeping;    
    
    /// If true, we have done the necessary sleep update and don't need to do
    /// it again. This variable is set to false when m_bSleeping is first set
    /// to true, and must be set to true by the code that updates the scene.
    bool m_bSleepUpdateDone;
};

NiSmartPointer(NiPhysXRigidBodyDest);

#include "NiPhysXRigidBodyDest.inl"

#endif  // #ifndef NIPHYSXRIGIDBODYDEST_H
