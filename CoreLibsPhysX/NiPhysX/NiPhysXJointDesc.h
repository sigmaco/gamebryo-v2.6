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


#ifndef NIPHYSXJOINTDESC_H
#define NIPHYSXJOINTDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>
#include <NiPhysXTypes.h>

#include "NiPhysXActorDesc.h"

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
#include <NxMat34.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXJointDesc objects are for caching snapshots of NxJointDesc objects,
    used for streaming and cloning simulation state.
    
    Applications will typically only touch these objects to modify PhysX
    content between streaming and instantiation, or to add cloning
    functionality to content created in code.

    The class stores a smart pointer to the actors involved in the joint and
    the state necessary to recreate the joint. Specific joint types should be
    derived from this class, as NiPhysXD6JointDescPtr is. 

    @note Extending Joint Descriptors: If an application creates new joint
        sub-types, the CreateFromJoint function of this class must be modified
        to create the correct type of joint.
*/
class NIPHYSX_ENTRY NiPhysXJointDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXJointDesc);
    /// @endcond

public:
    /**
        Extract from a joint all the state necessary to re-create that joint,
        and store actor pointers for future reference.
        
        The returned object will be of a derived class specific to the joint
        type that is being saved. At this time, the only type of supported
        joint is D6 (arbitrary six-degree-of-freedom joints). The map argument
        stores the mapping of NxActor objects onto their NiPhysX descriptors.
        It is required to support streaming pointers to the actors. The
        transformation, kXform, is applied to the axes and anchor point of
        connections to the world frame (NULL actor).
    */
    static NiPhysXJointDesc* CreateFromJoint(NxJoint* pkJoint,
        NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap,
        const NxMat34& kXform = NiPhysXTypes::NXMAT34_ID);

    /// Get an actor involved in this joint.
    ///
    /// The uiIndex should be either 0 or 1. 
    NiPhysXActorDescPtr GetActor(const NiUInt32 uiIndex);

    /// Set an actor involved in this joint.
    ///
    /// The uiIndex should be either 0 or 1. 
    void SetActor(const NiUInt32 uiIndex, NiPhysXActorDesc* kActorDesc);

    /// Get the limit point associated with this joint. 
    NxVec3 GetLimitPoint() const;

    /// Set the limit point associated with this joint. 
    void SetLimitPoint(const NxVec3& kLimitPt);

    /// Returns the number of limit planes associated with this joint. 
    NiUInt32 GetNumLimits() const;

    /// Get limit plane information for this joint.
    ///
    /// The index must be less than the value returned by GetNumLimits.
    void GetLimitPlane(const NiUInt32 uiIndex, NxVec3& kNorm,  NxReal& kD, 
        NxReal& kR);
    
    /// Add limit plane information for this joint.
    ///
    /// Space is internally allocated to accomodate the index. 
    void SetLimitPlane(const NiUInt32 uiIndex, const NxVec3& kNorm,
        const NxReal& kD, const NxReal& kR);

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
    
protected:
    NiPhysXJointDesc();
    virtual ~NiPhysXJointDesc();

    virtual void SetJointDesc(const NxJointDesc& kJointDesc);
    virtual void GetJointDesc(NxJointDesc& kJointDesc,
        const NxMat34& kXform) const;

    virtual void FromJoint(NxJoint* pkJoint);
    virtual void ToJoint(NxJoint* pkJoint);

    NxJointType m_eType;

    NiFixedString m_kJointName;
    NiPhysXActorDescPtr m_aspActors[2];
    NxVec3 m_akLocalNormal[2];
    NxVec3 m_akLocalAxis[2];
    NxVec3 m_akLocalAnchor[2];
    NxReal m_fMaxForce;
    NxReal m_fMaxTorque;
    NxReal m_fSolverExtrapolationFactor;
    NxU32 m_uiUseAccelerationSpring;
    NxU32 m_uiJointFlags;
    
    NxVec3 m_kLimitPoint;
    NiUInt32 m_uiNumLimits;
    NxVec3* m_pkLimitPlaneNorms;
    NxReal* m_pkLimitPlaneDs;
    NxReal* m_pkLimitPlaneRs;
};

NiSmartPointer(NiPhysXJointDesc);

#endif // #ifdef NIPHYSXJOINTDESC_H

