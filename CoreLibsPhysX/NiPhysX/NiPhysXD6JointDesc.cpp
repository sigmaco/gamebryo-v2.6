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

NiImplementRTTI(NiPhysXD6JointDesc, NiPhysXJointDesc);

//---------------------------------------------------------------------------
NiPhysXD6JointDesc::NiPhysXD6JointDesc() : NiPhysXJointDesc()
{
    m_eType = NX_JOINT_D6;
    
    NxD6JointDesc kDesc;
    kDesc.setToDefault();
    SetD6JointDesc(kDesc);
    
    m_pkJoint = 0;
}
//---------------------------------------------------------------------------
NxD6Joint* NiPhysXD6JointDesc::GetJoint()
{
    if (m_pkJoint)
        return m_pkJoint->isD6Joint();
    
    return 0;
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::GetD6JointDesc(NxD6JointDesc& kD6JointDesc,
    const NxMat34& kXform) const
{
    NiPhysXJointDesc::GetJointDesc(kD6JointDesc, kXform);
        
    kD6JointDesc.xMotion = m_eXMotion;
    kD6JointDesc.yMotion = m_eYMotion;
    kD6JointDesc.zMotion = m_eZMotion;
    kD6JointDesc.swing1Motion = m_eSwing1Motion;
    kD6JointDesc.swing2Motion = m_eSwing2Motion;
    kD6JointDesc.twistMotion = m_eTwistMotion;
    
    kD6JointDesc.linearLimit = m_kLinearLimit;
    kD6JointDesc.swing1Limit = m_kSwing1Limit;
    kD6JointDesc.swing2Limit = m_kSwing2Limit;
    kD6JointDesc.twistLimit = m_kTwistLimit;
 
    kD6JointDesc.xDrive = m_kXDrive;
    kD6JointDesc.yDrive = m_kYDrive;
    kD6JointDesc.zDrive = m_kZDrive;
    kD6JointDesc.swingDrive = m_kSwingDrive;
    kD6JointDesc.twistDrive = m_kTwistDrive;
    kD6JointDesc.slerpDrive = m_kSlerpDrive;
     
    kD6JointDesc.drivePosition = m_kDrivePosition;
    kD6JointDesc.driveOrientation = m_kDriveOrientation;
    kD6JointDesc.driveLinearVelocity = m_kDriveLinearVelocity;
    kD6JointDesc.driveAngularVelocity = m_kDriveAngularVelocity;
     
    kD6JointDesc.projectionMode = m_eProjectionMode;
    kD6JointDesc.projectionDistance = m_fProjectionDistance; 
    kD6JointDesc.projectionAngle = m_fProjectionAngle;
     
    kD6JointDesc.gearRatio = m_fGearRatio;
     
    kD6JointDesc.flags = m_uiFlags;
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::SetD6JointDesc(const NxD6JointDesc& kD6JointDesc)
{
    NiPhysXJointDesc::SetJointDesc(kD6JointDesc);
    
    m_eXMotion = kD6JointDesc.xMotion;
    m_eYMotion = kD6JointDesc.yMotion;
    m_eZMotion = kD6JointDesc.zMotion;
    m_eSwing1Motion = kD6JointDesc.swing1Motion;
    m_eSwing2Motion = kD6JointDesc.swing2Motion;
    m_eTwistMotion = kD6JointDesc.twistMotion;
    
    m_kLinearLimit = kD6JointDesc.linearLimit;
    m_kSwing1Limit = kD6JointDesc.swing1Limit;
    m_kSwing2Limit = kD6JointDesc.swing2Limit;
    m_kTwistLimit = kD6JointDesc.twistLimit;
 
    m_kXDrive = kD6JointDesc.xDrive;
    m_kYDrive = kD6JointDesc.yDrive;
    m_kZDrive = kD6JointDesc.zDrive;
    m_kSwingDrive = kD6JointDesc.swingDrive;
    m_kTwistDrive = kD6JointDesc.twistDrive;
    m_kSlerpDrive = kD6JointDesc.slerpDrive;
     
    m_kDrivePosition = kD6JointDesc.drivePosition;
    m_kDriveOrientation = kD6JointDesc.driveOrientation;
    m_kDriveLinearVelocity = kD6JointDesc.driveLinearVelocity;
    m_kDriveAngularVelocity = kD6JointDesc.driveAngularVelocity;
     
    m_eProjectionMode = kD6JointDesc.projectionMode;
    m_fProjectionDistance = kD6JointDesc.projectionDistance; 
    m_fProjectionAngle = kD6JointDesc.projectionAngle;
     
    m_fGearRatio = kD6JointDesc.gearRatio;
     
    m_uiFlags = kD6JointDesc.flags;
}
//---------------------------------------------------------------------------
NxJoint* NiPhysXD6JointDesc::CreateJoint(NxScene* pkScene,
    const NxMat34& kXform)
{
    NxD6JointDesc kJointDesc;
    GetD6JointDesc(kJointDesc, kXform);
    
    m_pkJoint = pkScene->createJoint(kJointDesc);
    
    if (!m_pkJoint)
        return 0;

    NiPhysXJointDesc::ToJoint(m_pkJoint);

    return m_pkJoint;
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::RemoveJoint(NxScene* pkScene)
{
    if (!m_pkJoint)
        return;
        
    pkScene->releaseJoint(*m_pkJoint);
    m_pkJoint = 0;
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::FromJoint(NxD6Joint* pkJoint,
    NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap, const NxMat34& kXform)
{
    NxD6JointDesc kJointDesc;
    pkJoint->saveToDesc(kJointDesc);
    SetD6JointDesc(kJointDesc);
    
    NiPhysXJointDesc::FromJoint(pkJoint);
    
    // Set actors
    NxActor* pkActor1;
    NxActor* pkActor2;
    NiPhysXActorDescPtr spActor;
    pkJoint->getActors(&pkActor1, &pkActor2);
    if (pkActor1)
    {
        if (kActorMap.GetAt(pkActor1, spActor))
        {
            SetActor(0, spActor);
        }
    }
    else
    {
        m_akLocalNormal[0] = kXform.M * m_akLocalNormal[0];
        m_akLocalAxis[0] = kXform.M * m_akLocalAxis[0];
        m_akLocalAnchor[0] = kXform * m_akLocalAnchor[0];
    }
    if (pkActor2)
    {
        if (kActorMap.GetAt(pkActor2, spActor))
        {
            SetActor(1, spActor);
        }
    }
    else
    {
        m_akLocalNormal[1] = kXform.M * m_akLocalNormal[1];
        m_akLocalAxis[1] = kXform.M * m_akLocalAxis[1];
        m_akLocalAnchor[1] = kXform * m_akLocalAnchor[1];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXD6JointDesc);
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::CopyMembers(NiPhysXD6JointDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXJointDesc::CopyMembers(pkDest, kCloning);
    
    pkDest->m_eXMotion = m_eXMotion;
    pkDest->m_eYMotion = m_eYMotion;
    pkDest->m_eZMotion = m_eZMotion;
    pkDest->m_eSwing1Motion = m_eSwing1Motion;
    pkDest->m_eSwing2Motion = m_eSwing2Motion;
    pkDest->m_eTwistMotion = m_eTwistMotion;
    
    pkDest->m_kLinearLimit = m_kLinearLimit;
    pkDest->m_kSwing1Limit = m_kSwing1Limit;
    pkDest->m_kSwing2Limit = m_kSwing2Limit;
    pkDest->m_kTwistLimit = m_kTwistLimit;
 
    pkDest->m_kXDrive = m_kXDrive;
    pkDest->m_kYDrive = m_kYDrive;
    pkDest->m_kZDrive = m_kZDrive;
    pkDest->m_kSwingDrive = m_kSwingDrive;
    pkDest->m_kTwistDrive = m_kTwistDrive;
    pkDest->m_kSlerpDrive = m_kSlerpDrive;
     
    pkDest->m_kDrivePosition = m_kDrivePosition;
    pkDest->m_kDriveOrientation = m_kDriveOrientation;
    pkDest->m_kDriveLinearVelocity = m_kDriveLinearVelocity;
    pkDest->m_kDriveAngularVelocity = m_kDriveAngularVelocity;
     
    pkDest->m_eProjectionMode = m_eProjectionMode;
    pkDest->m_fProjectionDistance = m_fProjectionDistance; 
    pkDest->m_fProjectionAngle = m_fProjectionAngle;
     
    pkDest->m_fGearRatio = m_fGearRatio;
     
    pkDest->m_uiFlags = m_uiFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXD6JointDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXJointDesc::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXD6JointDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::SaveBinary(NiStream& kStream)
{
    NiPhysXJointDesc::SaveBinary(kStream);
    
    NiStreamSaveEnum(kStream, m_eXMotion);
    NiStreamSaveEnum(kStream, m_eYMotion);
    NiStreamSaveEnum(kStream, m_eZMotion);
    NiStreamSaveEnum(kStream, m_eSwing1Motion);
    NiStreamSaveEnum(kStream, m_eSwing2Motion);
    NiStreamSaveEnum(kStream, m_eTwistMotion);
    
    SaveNxJointLimitSoftDesc(kStream, m_kLinearLimit);
    SaveNxJointLimitSoftDesc(kStream, m_kSwing1Limit);
    SaveNxJointLimitSoftDesc(kStream, m_kSwing2Limit);
    SaveNxJointLimitSoftDesc(kStream, m_kTwistLimit.low);
    SaveNxJointLimitSoftDesc(kStream, m_kTwistLimit.high);
 
    SaveNxJointDriveDesc(kStream, m_kXDrive);
    SaveNxJointDriveDesc(kStream, m_kYDrive);
    SaveNxJointDriveDesc(kStream, m_kZDrive);
    SaveNxJointDriveDesc(kStream, m_kSwingDrive);
    SaveNxJointDriveDesc(kStream, m_kTwistDrive);
    SaveNxJointDriveDesc(kStream, m_kSlerpDrive);

    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kDrivePosition);
    NiPhysXTypes::NxQuatSaveBinary(kStream, m_kDriveOrientation);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kDriveLinearVelocity);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kDriveAngularVelocity);

    NiStreamSaveEnum(kStream, m_eProjectionMode);
    NiStreamSaveBinary(kStream, m_fProjectionDistance); 
    NiStreamSaveBinary(kStream, m_fProjectionAngle);
     
    NiStreamSaveBinary(kStream, m_fGearRatio);
     
    NiStreamSaveBinary(kStream, m_uiFlags);
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::SaveNxJointLimitSoftDesc(NiStream& kStream,
    const NxJointLimitSoftDesc& kDesc)
{
    NiStreamSaveBinary(kStream, kDesc.value);
    NiStreamSaveBinary(kStream, kDesc.restitution);
    NiStreamSaveBinary(kStream, kDesc.spring);
    NiStreamSaveBinary(kStream, kDesc.damping);
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::SaveNxJointDriveDesc(NiStream& kStream,
    const NxJointDriveDesc& kDesc)
{
    NiStreamSaveBinary(kStream, kDesc.driveType);
    NiStreamSaveBinary(kStream, kDesc.spring);
    NiStreamSaveBinary(kStream, kDesc.damping);
    NiStreamSaveBinary(kStream, kDesc.forceLimit);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXD6JointDesc);
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::LoadBinary(NiStream& kStream)
{
    NiPhysXJointDesc::LoadBinary(kStream);
    
    NiStreamLoadEnum(kStream, m_eXMotion);
    NiStreamLoadEnum(kStream, m_eYMotion);
    NiStreamLoadEnum(kStream, m_eZMotion);
    NiStreamLoadEnum(kStream, m_eSwing1Motion);
    NiStreamLoadEnum(kStream, m_eSwing2Motion);
    NiStreamLoadEnum(kStream, m_eTwistMotion);
    
    LoadNxJointLimitSoftDesc(kStream, m_kLinearLimit);
    LoadNxJointLimitSoftDesc(kStream, m_kSwing1Limit);
    LoadNxJointLimitSoftDesc(kStream, m_kSwing2Limit);
    LoadNxJointLimitSoftDesc(kStream, m_kTwistLimit.low);
    LoadNxJointLimitSoftDesc(kStream, m_kTwistLimit.high);
 
    LoadNxJointDriveDesc(kStream, m_kXDrive);
    LoadNxJointDriveDesc(kStream, m_kYDrive);
    LoadNxJointDriveDesc(kStream, m_kZDrive);
    LoadNxJointDriveDesc(kStream, m_kSwingDrive);
    LoadNxJointDriveDesc(kStream, m_kTwistDrive);
    LoadNxJointDriveDesc(kStream, m_kSlerpDrive);

    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kDrivePosition);
    NiPhysXTypes::NxQuatLoadBinary(kStream, m_kDriveOrientation);
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kDriveLinearVelocity);
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kDriveAngularVelocity);

    NiStreamLoadEnum(kStream, m_eProjectionMode);
    NiStreamLoadBinary(kStream, m_fProjectionDistance); 
    NiStreamLoadBinary(kStream, m_fProjectionAngle);
     
    NiStreamLoadBinary(kStream, m_fGearRatio);
     
    NiStreamLoadBinary(kStream, m_uiFlags);
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::LoadNxJointLimitSoftDesc(NiStream& kStream,
    NxJointLimitSoftDesc& kDesc)
{
    NiStreamLoadBinary(kStream, kDesc.value);
    NiStreamLoadBinary(kStream, kDesc.restitution);
    NiStreamLoadBinary(kStream, kDesc.spring);
    NiStreamLoadBinary(kStream, kDesc.damping);
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::LoadNxJointDriveDesc(NiStream& kStream,
    NxJointDriveDesc& kDesc)
{
    NiStreamLoadBinary(kStream, kDesc.driveType);
    NiStreamLoadBinary(kStream, kDesc.spring);
    NiStreamLoadBinary(kStream, kDesc.damping);
    NiStreamLoadBinary(kStream, kDesc.forceLimit);
}
//---------------------------------------------------------------------------
void NiPhysXD6JointDesc::LinkObject(NiStream& kStream)
{
    NiPhysXJointDesc::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXD6JointDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXD6JointDesc, pkObject));
    if(!NiPhysXJointDesc::IsEqual(pkObject))
        return false;
        
    NiPhysXD6JointDesc* pkNewObject = (NiPhysXD6JointDesc*)pkObject;
    
    if (m_eXMotion != pkNewObject->m_eXMotion ||
        m_eYMotion != pkNewObject->m_eYMotion ||
        m_eZMotion != pkNewObject->m_eZMotion ||
        m_eSwing1Motion != pkNewObject->m_eSwing1Motion ||
        m_eSwing2Motion != pkNewObject->m_eSwing2Motion ||
        m_eTwistMotion != pkNewObject->m_eTwistMotion)
        return false;
    
    if (m_kLinearLimit.value != pkNewObject->m_kLinearLimit.value ||
        m_kLinearLimit.restitution != pkNewObject->m_kLinearLimit.restitution||
        m_kLinearLimit.damping != pkNewObject->m_kLinearLimit.damping ||
        m_kLinearLimit.spring != pkNewObject->m_kLinearLimit.spring)
        return false;
        
    if (m_kSwing1Limit.value != pkNewObject->m_kSwing1Limit.value ||
        m_kSwing1Limit.restitution != pkNewObject->m_kSwing1Limit.restitution||
        m_kSwing1Limit.damping != pkNewObject->m_kSwing1Limit.damping ||
        m_kSwing1Limit.spring != pkNewObject->m_kSwing1Limit.spring)
        return false;
        
    if (m_kSwing2Limit.value != pkNewObject->m_kSwing2Limit.value ||
        m_kSwing2Limit.restitution != pkNewObject->m_kSwing2Limit.restitution||
        m_kSwing2Limit.damping != pkNewObject->m_kSwing2Limit.damping ||
        m_kSwing2Limit.spring != pkNewObject->m_kSwing2Limit.spring)
        return false;
        
    if (m_kTwistLimit.low.value != pkNewObject->m_kTwistLimit.low.value ||
        m_kTwistLimit.low.restitution !=
        pkNewObject->m_kTwistLimit.low.restitution ||
        m_kTwistLimit.low.damping != pkNewObject->m_kTwistLimit.low.damping ||
        m_kTwistLimit.low.spring != pkNewObject->m_kTwistLimit.low.spring)
        return false;
    if (m_kTwistLimit.high.value != pkNewObject->m_kTwistLimit.high.value ||
        m_kTwistLimit.high.restitution !=
        pkNewObject->m_kTwistLimit.high.restitution ||
        m_kTwistLimit.high.damping != pkNewObject->m_kTwistLimit.high.damping||
        m_kTwistLimit.high.spring != pkNewObject->m_kTwistLimit.high.spring)
        return false;

    if (m_kXDrive.driveType != pkNewObject->m_kXDrive.driveType ||
        m_kXDrive.forceLimit != pkNewObject->m_kXDrive.forceLimit ||
        m_kXDrive.damping != pkNewObject->m_kXDrive.damping ||
        m_kXDrive.spring != pkNewObject->m_kXDrive.spring)
        return false;
        
    if (m_kYDrive.driveType != pkNewObject->m_kYDrive.driveType ||
        m_kYDrive.forceLimit != pkNewObject->m_kYDrive.forceLimit ||
        m_kYDrive.damping != pkNewObject->m_kYDrive.damping ||
        m_kYDrive.spring != pkNewObject->m_kYDrive.spring)
        return false;
        
    if (m_kZDrive.driveType != pkNewObject->m_kZDrive.driveType ||
        m_kZDrive.forceLimit != pkNewObject->m_kZDrive.forceLimit ||
        m_kZDrive.damping != pkNewObject->m_kZDrive.damping ||
        m_kZDrive.spring != pkNewObject->m_kZDrive.spring)
        return false;
        
    if (m_kSwingDrive.driveType != pkNewObject->m_kSwingDrive.driveType ||
        m_kSwingDrive.forceLimit != pkNewObject->m_kSwingDrive.forceLimit ||
        m_kSwingDrive.damping != pkNewObject->m_kSwingDrive.damping ||
        m_kSwingDrive.spring != pkNewObject->m_kSwingDrive.spring)
        return false;
        
    if (m_kTwistDrive.driveType != pkNewObject->m_kTwistDrive.driveType ||
        m_kTwistDrive.forceLimit != pkNewObject->m_kTwistDrive.forceLimit ||
        m_kTwistDrive.damping != pkNewObject->m_kTwistDrive.damping ||
        m_kTwistDrive.spring != pkNewObject->m_kTwistDrive.spring)
        return false;
        
    if (m_kSlerpDrive.driveType != pkNewObject->m_kSlerpDrive.driveType ||
        m_kSlerpDrive.forceLimit != pkNewObject->m_kSlerpDrive.forceLimit ||
        m_kSlerpDrive.damping != pkNewObject->m_kSlerpDrive.damping ||
        m_kSlerpDrive.spring != pkNewObject->m_kSlerpDrive.spring)
        return false;

    NiPoint3 kNewP(0.0f, 0.0f, 0.0f);
    NiPoint3 kThisP(0.0f, 0.0f, 0.0f);
    NiQuaternion kNewQ(0.0f, 0.0f, 0.0f, 0.0f);
    NiQuaternion kThisQ(0.0f, 0.0f, 0.0f, 0.0f);

    NiPhysXTypes::NxVec3ToNiPoint3(m_kDrivePosition, kThisP);
    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kDrivePosition, kThisP);
    if (kNewP != kThisP)
        return false;

    NiPhysXTypes::NxQuatToNiQuaternion(m_kDriveOrientation, kThisQ);
    NiPhysXTypes::NxQuatToNiQuaternion(pkNewObject->m_kDriveOrientation,
        kThisQ);
    if (kNewQ != kThisQ)
        return false;

    NiPhysXTypes::NxVec3ToNiPoint3(m_kDriveLinearVelocity, kThisP);
    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kDriveLinearVelocity, 
        kThisP);
    if (kNewP != kThisP)
        return false;

    NiPhysXTypes::NxVec3ToNiPoint3(m_kDriveAngularVelocity, kThisP);
    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kDriveAngularVelocity,
        kThisP);
    if (kNewP != kThisP)
        return false;

    if (m_eProjectionMode != pkNewObject->m_eProjectionMode ||
        m_fProjectionDistance != pkNewObject->m_fProjectionDistance ||
        m_fProjectionAngle != pkNewObject->m_fProjectionAngle ||
        m_fGearRatio != pkNewObject->m_fGearRatio ||
        m_uiFlags != pkNewObject->m_uiFlags)
        return false;

    return true;
}
//---------------------------------------------------------------------------
