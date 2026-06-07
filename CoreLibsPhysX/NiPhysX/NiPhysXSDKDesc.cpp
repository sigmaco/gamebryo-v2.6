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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiImplementRTTI(NiPhysXSDKDesc, NiObject);

//---------------------------------------------------------------------------
void NiPhysXSDKDesc::FromSDK()
{
    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    
    pkManager->WaitSDKLock();
    for (unsigned int i = 0; i < NX_PARAMS_NUM_VALUES; i++)
    {
        // These values are deprecated.
        if ((i >= 30 && i <= 32) ||
            (i >= 23 && i <= 26) ||
            (i >= 15 && i <= 21))
        {
            continue;
        }

        m_afSDKParams[i] =
            pkManager->m_pkPhysXSDK->getParameter((NxParameter)i);
    }
    pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::ToSDK() const
{
    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    
    pkManager->WaitSDKLock();
    for (unsigned int i = 0; i < NX_PARAMS_NUM_VALUES; i++)
    {
        // These values are deprecated.
        if ((i >= 30 && i <= 32) ||
            (i >= 23 && i <= 26) ||
            (i >= 15 && i <= 21))
        {
            continue;
        }
        pkManager->m_pkPhysXSDK->setParameter((NxParameter)i, 
            m_afSDKParams[i]);
    }
    pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
NxReal NiPhysXSDKDesc::GetParameter(const NxParameter kIndex) const
{
    return m_afSDKParams[kIndex];
}
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::SetParameter(const NxParameter kIndex, const NxReal kVal)
{
    m_afSDKParams[kIndex] = kVal;
}
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::SetSDKDesc(const NxPhysicsSDKDesc& kSDKDesc)
{
    m_kSDKDescriptor = kSDKDesc;
}
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::GetSDKDesc(NxPhysicsSDKDesc& kSDKDesc) const
{
    kSDKDesc = m_kSDKDescriptor;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXSDKDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_afSDKParams[NX_PENALTY_FORCE]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_SKIN_WIDTH]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_DEFAULT_SLEEP_LIN_VEL_SQUARED]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_DEFAULT_SLEEP_ANG_VEL_SQUARED]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_BOUNCE_THRESHOLD]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_DYN_FRICT_SCALING]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_STA_FRICT_SCALING]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_MAX_ANGULAR_VELOCITY]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_CONTINUOUS_CD]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZATION_SCALE]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_WORLD_AXES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_AXES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_MASS_AXES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_LIN_VELOCITY]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_ANG_VELOCITY]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_JOINT_GROUPS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_JOINT_LOCAL_AXES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_JOINT_WORLD_AXES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_JOINT_LIMITS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_POINT]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_NORMAL]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_ERROR]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_FORCE]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_ACTOR_AXES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_AABBS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_SHAPES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_AXES]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_COMPOUNDS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_VNORMALS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_FNORMALS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_EDGES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_SPHERES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_STATIC]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_DYNAMIC]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_FREE]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_CCD]);    
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_SKELETONS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_EMITTERS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_POSITION]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_VELOCITY]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_KERNEL_RADIUS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_BOUNDS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_PACKETS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_MOTION_LIMIT]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_DYN_COLLISION]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_DRAINS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_COLLISIONS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_SELFCOLLISIONS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_WORKPACKETS]);
    NiStreamSaveBinary(kStream,
         m_afSDKParams[NX_VISUALIZE_CLOTH_SLEEP]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_ADAPTIVE_FORCE]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_COLL_VETO_JOINTED]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_TRIGGER_TRIGGER_CALLBACK]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_SELECT_HW_ALGO]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_ACTIVE_VERTICES]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_CCD_EPSILON]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_SOLVER_CONVERGENCE_THRESHOLD]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_BBOX_NOISE_LEVEL]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_IMPLICIT_SWEEP_CACHE_SIZE]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_STC_COLLISION]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_MESH_PACKETS]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_VISUALIZE_CLOTH_MESH]);
    NiStreamSaveBinary(kStream, m_afSDKParams[NX_DEFAULT_SLEEP_ENERGY]);

    // New in PhysX 2.7.2
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_PACKET_DATA]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_SLEEP_VERTEX]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_TEARABLE_VERTICES]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_TEARING]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_ATTACHMENT]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_CLOTH_VALIDBOUNDS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_MESH]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_COLLISIONS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_WORKPACKETS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_SLEEP]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_TEARING]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_ATTACHMENT]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_SOFTBODY_VALIDBOUNDS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_CONSTANT_FLUID_MAX_PACKETS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FORCE_FIELDS]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_ASYNCHRONOUS_MESH_CREATION]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON]);
    NiStreamSaveBinary(kStream,
        m_afSDKParams[NX_IMPROVED_SPRING_SOLVER]);

    NiStreamSaveBinary(kStream, m_kSDKDescriptor.hwPageSize);
    NiStreamSaveBinary(kStream, m_kSDKDescriptor.hwPageMax);
    NiStreamSaveBinary(kStream, m_kSDKDescriptor.hwConvexMax);
    NiStreamSaveBinary(kStream, m_kSDKDescriptor.flags);
    NiStreamSaveBinary(kStream, m_kSDKDescriptor.cookerThreadMask);
}
//---------------------------------------------------------------------------
bool NiPhysXSDKDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXSDKDesc);
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_PENALTY_FORCE]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_SKIN_WIDTH]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_DEFAULT_SLEEP_LIN_VEL_SQUARED]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_DEFAULT_SLEEP_ANG_VEL_SQUARED]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_BOUNCE_THRESHOLD]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_DYN_FRICT_SCALING]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_STA_FRICT_SCALING]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_MAX_ANGULAR_VELOCITY]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_CONTINUOUS_CD]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZATION_SCALE]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_WORLD_AXES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_AXES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_MASS_AXES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_LIN_VELOCITY]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_ANG_VELOCITY]);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 0))
    {
        NxReal dummy;
        // For the deprecated NX_VISUALIZE_BODY_LIN_MOMENTUM
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_ANG_MOMENTUM
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_LIN_ACCEL
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_ANG_ACCEL
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_LIN_FORCE
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_ANG_FORCE
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_REDUCED
        NiStreamLoadBinary(kStream, dummy);
    }
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_BODY_JOINT_GROUPS]);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 0))
    {
        NxReal dummy;
        // For the deprecated NX_VISUALIZE_BODY_CONTACT_LIST
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_JOINT_LIST
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_DAMPING
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_BODY_SLEEP
        NiStreamLoadBinary(kStream, dummy);
    }
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_JOINT_LOCAL_AXES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_JOINT_WORLD_AXES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_JOINT_LIMITS]);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 0))
    {
        NxReal dummy;
        // For the deprecated NX_VISUALIZE_JOINT_ERROR
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_JOINT_FORCE
        NiStreamLoadBinary(kStream, dummy);
        // For the deprecated NX_VISUALIZE_JOINT_REDUCED
        NiStreamLoadBinary(kStream, dummy);
    }
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_POINT]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_NORMAL]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_ERROR]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_CONTACT_FORCE]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_ACTOR_AXES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_AABBS]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_SHAPES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_AXES]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_COMPOUNDS]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_VNORMALS]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_FNORMALS]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_EDGES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_SPHERES]);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 0))
    {
        NxReal fDummy;
        NiStreamLoadBinary(kStream, fDummy);
    }
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_STATIC]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_DYNAMIC]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_FREE]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_COLLISION_CCD]);    
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_COLLISION_SKELETONS]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_EMITTERS]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_POSITION]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_VELOCITY]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_KERNEL_RADIUS]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_BOUNDS]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_PACKETS]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_MOTION_LIMIT]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_VISUALIZE_FLUID_DYN_COLLISION]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_FLUID_DRAINS]);
    if (kStream.GetFileVersion() > NiStream::GetVersion(20, 2, 0, 7))
    {
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_COLLISIONS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_SELFCOLLISIONS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_WORKPACKETS]);
    }
    if (kStream.GetFileVersion() > NiStream::GetVersion(20, 3, 0, 0))
    {
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_SLEEP]);
    }
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_ADAPTIVE_FORCE]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_COLL_VETO_JOINTED]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_TRIGGER_TRIGGER_CALLBACK]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_SELECT_HW_ALGO]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_ACTIVE_VERTICES]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_CCD_EPSILON]);
    NiStreamLoadBinary(kStream,
        m_afSDKParams[NX_SOLVER_CONVERGENCE_THRESHOLD]);
    NiStreamLoadBinary(kStream, m_afSDKParams[NX_BBOX_NOISE_LEVEL]);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 0))
    {
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_IMPLICIT_SWEEP_CACHE_SIZE]);
    }
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 1) &&
        kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 6))
    {
        NxReal fDummy;
        NiStreamLoadBinary(kStream, fDummy);
    }
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 6))
    {
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_FLUID_STC_COLLISION]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_FLUID_MESH_PACKETS]);
        NiStreamLoadBinary(kStream, m_afSDKParams[NX_VISUALIZE_CLOTH_MESH]);
        NiStreamLoadBinary(kStream, m_afSDKParams[NX_DEFAULT_SLEEP_ENERGY]);
    }
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_FLUID_PACKET_DATA]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_SLEEP_VERTEX]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_TEARABLE_VERTICES]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_TEARING]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_ATTACHMENT]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_CLOTH_VALIDBOUNDS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_MESH]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_COLLISIONS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_WORKPACKETS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_SLEEP]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_TEARING]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_ATTACHMENT]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_SOFTBODY_VALIDBOUNDS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_CONSTANT_FLUID_MAX_PACKETS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_VISUALIZE_FORCE_FIELDS]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_ASYNCHRONOUS_MESH_CREATION]);
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON]);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 5, 0, 3))
    {
        NiStreamLoadBinary(kStream,
            m_afSDKParams[NX_IMPROVED_SPRING_SOLVER]);
    }

    if (kStream.GetFileVersion() > NiStream::GetVersion(20, 2, 0, 7))
    {
        NiStreamLoadBinary(kStream, m_kSDKDescriptor.hwPageSize);
        NiStreamLoadBinary(kStream, m_kSDKDescriptor.hwPageMax);
        NiStreamLoadBinary(kStream, m_kSDKDescriptor.hwConvexMax);
        NiStreamLoadBinary(kStream, m_kSDKDescriptor.flags);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kSDKDescriptor.cookerThreadMask);
    }
}
//---------------------------------------------------------------------------
void NiPhysXSDKDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXSDKDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXSDKDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXSDKDesc* pkNewObject = (NiPhysXSDKDesc*)pkObject;

    for (unsigned int i = 0; i < NX_PARAMS_NUM_VALUES; i++)
    {
        if (pkNewObject->m_afSDKParams[i] != m_afSDKParams[i])
            return false;
    }

    if (m_kSDKDescriptor.hwPageSize !=
        pkNewObject->m_kSDKDescriptor.hwPageSize ||
        m_kSDKDescriptor.hwPageMax !=
        pkNewObject->m_kSDKDescriptor.hwPageMax ||
        m_kSDKDescriptor.hwConvexMax !=
        pkNewObject->m_kSDKDescriptor.hwConvexMax ||
        m_kSDKDescriptor.flags !=
        pkNewObject->m_kSDKDescriptor.flags)
        return false;

    return true;
}
//---------------------------------------------------------------------------
