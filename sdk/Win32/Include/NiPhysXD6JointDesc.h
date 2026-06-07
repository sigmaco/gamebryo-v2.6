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


#ifndef NIPHYSXD6JOINTDESC_H
#define NIPHYSXD6JOINTDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXJointDesc.h"

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
    NiPhysXD6JointDesc objects are for caching snapshots of NxD6JointDesc
    objects, used for streaming and cloning.
    
    Applications will typically only access these objects to modify PhysX
    content between loading from a NIF file and instantiating the PhysX
    content, or when cloning behavior is desired for content created in code.

    The class stores a pointer to the actors involved in the joint and the
    state necessary to recreate the joint. 
*/
class NIPHYSX_ENTRY NiPhysXD6JointDesc : public NiPhysXJointDesc
{
    /// @cond EMERGENT_INTERNAL
    
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXD6JointDesc);

    /// @endcond

public:
    /// Constructor.
    ///
    /// The new object is initialized the same as an NxD6JointDesc object. 
    NiPhysXD6JointDesc();
    
    /**
        Creates a PhysX joint from the descriptor.
        
        The joint will be part of the given pkScene argument. The
        transformation argument is used to transform the axes and anchor of
        any connection to the global coordinate frame (if one of the joint
        actors is NULL).
    */
    virtual NxJoint* CreateJoint(NxScene* pkScene,
        const NxMat34& kXform = NiPhysXTypes::NXMAT34_ID);

    /// Remove the joint associated with this descriptor from the given scene. 
    virtual void RemoveJoint(NxScene* pkScene);
    
    /**
        Extract from a joint all the state necessary to re-create that joint,
        and store actor pointers for future reference.
        
        The map argument stores the mapping of NxActors onto their NiPhysX
        descriptors. It is required to support streaming the actor pointers.
        The transformation, kXform, is applied to the axes and anchor point of
        connections to the world frame (NULL actor). 
    */
    virtual void FromJoint(NxD6Joint* pkJoint,
        NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap,
        const NxMat34& kXform = NiPhysXTypes::NXMAT34_ID);

    // Don't hide the parent class version of this function.
    using NiPhysXJointDesc::FromJoint;
        
    /**
        Set the contents of this descriptor from the given NxD6JointDesc.
        
        This does not set the actors. Use the NiPhysXJointDesc::SetActor
        function to set actors. 
    */
    virtual void SetD6JointDesc(const NxD6JointDesc& kJointDesc);

    /**
        Gets the contents of this descriptor and puts them into the given
        NxD6JointDesc.
        
        This does not get the actors. Use the NiPhysXJointDesc::GetActor
        function to get actors. The transformation argument is used to
        transform the axes and anchor of any connection to the global
        coordinate frame (if one of the joint actors is NULL).
    */
    virtual void GetD6JointDesc(NxD6JointDesc& kJointDesc,
        const NxMat34& kXform = NiPhysXTypes::NXMAT34_ID) const;
    
    /**
        Get the PhysX joint that this descriptor is responsible for, or NULL
        if no joint has been created from the descriptor.
    */
    NxD6Joint* GetJoint();

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:        
    void SaveNxJointLimitSoftDesc(NiStream& kStream,
        const NxJointLimitSoftDesc& kDesc);
    void SaveNxJointDriveDesc(NiStream& kStream,
        const NxJointDriveDesc& kDesc);
    void LoadNxJointLimitSoftDesc(NiStream& kStream,
        NxJointLimitSoftDesc& kDesc);
    void LoadNxJointDriveDesc(NiStream& kStream,
        NxJointDriveDesc& kDesc);

    NxD6JointMotion m_eXMotion;
    NxD6JointMotion m_eYMotion;
    NxD6JointMotion m_eZMotion;
    NxD6JointMotion m_eSwing1Motion;
    NxD6JointMotion m_eSwing2Motion;
    NxD6JointMotion m_eTwistMotion;
    
    NxJointLimitSoftDesc m_kLinearLimit;
    NxJointLimitSoftDesc m_kSwing1Limit;
    NxJointLimitSoftDesc m_kSwing2Limit;
    NxJointLimitSoftPairDesc m_kTwistLimit;
 
    NxJointDriveDesc  m_kXDrive;
    NxJointDriveDesc  m_kYDrive;
    NxJointDriveDesc  m_kZDrive;
    NxJointDriveDesc  m_kSwingDrive;
    NxJointDriveDesc  m_kTwistDrive;
    NxJointDriveDesc  m_kSlerpDrive;
     
    NxVec3 m_kDrivePosition;
    NxQuat m_kDriveOrientation;
    NxVec3 m_kDriveLinearVelocity;
    NxVec3 m_kDriveAngularVelocity;
     
    NxJointProjectionMode m_eProjectionMode;
    NxReal m_fProjectionDistance; 
    NxReal m_fProjectionAngle;
     
    NxReal  m_fGearRatio;
     
    NxU32  m_uiFlags;

    NxJoint* m_pkJoint;
};

NiSmartPointer(NiPhysXD6JointDesc);

#endif // #ifdef NIPHYSXD6JOINTDESC_H

