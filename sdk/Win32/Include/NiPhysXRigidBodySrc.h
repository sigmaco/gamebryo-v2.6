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

#ifndef NIPHYSXRIGIDBODYSRC_H
#define NIPHYSXRIGIDBODYSRC_H

#include "NiPhysXLibType.h"

#include <NiAVObject.h>

#include "NiPhysXSrc.h"

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
    NiPhysXRigidBodySrc is an abstract base class for sources that extract
    state from a Gamebryo scene graph object and set state of a rigid body
    PhysX actor. 

    Internally, this class stores the Gamebryo source node and the PhysX
    target actor, and the two most recent transformations extracted from
    the node, in PhysX coordinates. In other words, transformations that
    could be used directly to set the pose of the target actor. Also stored
    are the application times for the transformations for use when
    interpolating. 

    This class is abstract because it does not implement
    NiPhysXSrc::UpdateToActors. 
*/
class NIPHYSX_ENTRY NiPhysXRigidBodySrc : public NiPhysXSrc
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareAbstractClone(NiPhysXRigidBodySrc);
    /// @endcond

public:
    /**
        Constructor.
        
        Sets state common to rigid body sources. The internal transformations
        used for interpolation are set to the identity transforms, and the
        extraction times for those transformations are set to be in the past.

        @param pkSource The Gamebryo node from which to extract state.
        @param pkActor The PhysX actor to drive with the state.
    */
    NiPhysXRigidBodySrc(NiAVObject* pkSource, NxActor* pkActor);

    /// Destructor.
    ~NiPhysXRigidBodySrc();

    /// Returns the Gamebryo node that is driving the motion.
    NiAVObject* GetSource() const;

    /// Returns the PhysX actor that this source is pushing state to.
    void SetTarget(NxActor* pkActor);

    /// Set the PhysX actor that this source is pushing state to.
    NxActor* GetTarget() const;
    
    /**
        Reset velocity computations inside the source.
        
        Velocities are computed for use when driving a dynamic actor. After
        this function is called, the source will return zero velocities until
        the source has been updated twice. Calling UpdateFromSceneGraph with
        the bForce argument true also resets the velocities to require one
        additional update before velocities are valid (this function is called
        by NiPhysXScene::UpdatedSources).
    */
    void ResetVelocity();
    
    /**
        Get the time for which this source has internally cached state.
    
        uiIndex is either 0 or 1, indicating which of the two internal values
        is desired. This function is intended for debug purposes. 
    */
    float GetTime(NiUInt32 uiIndex) const;

    /**
        Get the internally cached pose.
        
        uiIndex is either 0 or 1, indicating which of the two internal values
        is desired. This function is intended for debug purposes. 
    */
    NxMat34 GetPose(NiUInt32 uiIndex) const;
    
    /**
        Extract state from the scene graph.
        
        This function extracts the state from the scene graph and uses
        kInvRootTransform to convert it into PhysX's coordinate system.
        The translation is then scaled by fScaleWToP. If fT is less than
        or equal to the most recent stored transformation, and bForce is
        false, then nothing is done. If bForce is true, state is always
        extracted with the given time. The function does nothing if the
        source is not active unless forced. 
    */
    virtual void UpdateFromSceneGraph(const float fT,
        const NiTransform& kInvRootTransform, const float fScaleWToP,
        const bool bForce = false);

    /// @cond EMERGENT_INTERNAL

    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// @endcond

protected:
    NiPhysXRigidBodySrc();
    
    // Provide interpolation functionality for use by sub-classes
    void Interpolate(const float fT, NxMat34& kPose);
    bool CanComputeVelocities() const;
    void ComputeVelocities(const NxMat34& kBodyLocalPose,
        NxVec3& kLinVel, NxVec3& kAngVel);
    void ComputeDynamicData(const float fT, const NxMat34& kBodyLocalPose,
        NxMat34& kBodyWorldPose, NxVec3& kLinVel, NxVec3& kAngVel);
        
    NiAVObject* m_pkSource;
    NxActor* m_pkTarget;
    
    // A rotating buffer of two poses. Data at m_aucIndices[1] is most recent.
    // These are the PhysX global transforms.
    NxMat34 m_akPoses[2];
    float m_afTimes[2];
    unsigned char m_aucIndices[2];
    
    // This holds the number of frames still required before we can compute
    // decent velocity information.
    unsigned char m_ucResetVelocity;

    // This is true if the target was dynamic the last time we set its state,
    // or is true if we have never set the target's state.
    bool m_bForcedUpdate;

    friend class NiPhysXConverter;
};

NiSmartPointer(NiPhysXRigidBodySrc);

#include "NiPhysXRigidBodySrc.inl"

#endif  // #ifndef NIPHYSXRIGIDBODYSRC_H
