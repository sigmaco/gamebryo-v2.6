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

// Precompiled Header
#include "NiPhysXPCH.h"

#include "NiPhysX.h"

NiImplementRTTI(NiPhysXRigidBodySrc, NiPhysXSrc);

//---------------------------------------------------------------------------
NiPhysXRigidBodySrc::NiPhysXRigidBodySrc()
{
    m_pkSource = 0;
    m_pkTarget = 0;
    
    m_aucIndices[0] = 0;
    m_aucIndices[1] = 1;
    m_afTimes[0] = -2.0f; // This makes sure interpolation works immediately
    m_afTimes[1] = -1.0f; // Should always call UpdateSources before use
    m_akPoses[0].id();
    m_akPoses[1].id();
    
    m_ucResetVelocity = 2;
    m_bForcedUpdate = false;
}
//---------------------------------------------------------------------------
NiPhysXRigidBodySrc::NiPhysXRigidBodySrc(NiAVObject* pkSource,
    NxActor* pkTarget) : NiPhysXSrc()
{
    m_pkSource = pkSource;
    m_pkTarget = pkTarget;
    
    m_aucIndices[0] = 0;
    m_aucIndices[1] = 1;
    m_afTimes[0] = -2.0f; // This makes sure interpolation works immediately
    m_afTimes[1] = -1.0f; // Should always call UpdateSources before use
    m_akPoses[0].id();
    m_akPoses[1].id();
    
    m_ucResetVelocity = 2;
    m_bForcedUpdate = false;
}
//---------------------------------------------------------------------------
NiPhysXRigidBodySrc::~NiPhysXRigidBodySrc()
{
    m_pkSource = 0;
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::UpdateFromSceneGraph(const float fT,
    const NiTransform& kInvRootTransform, const float fScaleWToP,
    const bool bForce)
{
    if ((fT <= m_afTimes[m_aucIndices[1]] || !m_bActive) && !bForce)
        return;

    // Swap indices.
    unsigned char ucTemp = m_aucIndices[0];
    m_aucIndices[0] = m_aucIndices[1];
    m_aucIndices[1] = ucTemp;
        
    // Extract the Gamebryo node transform and put it in the PhysX coordinate
    // system
    const NiTransform kSrcXform = m_pkSource->GetWorldTransform();
    NiTransform kPhysXXform = kInvRootTransform * kSrcXform;
    kPhysXXform.m_Translate *= fScaleWToP;
    
    NiPhysXTypes::NiTransformToNxMat34(kPhysXXform.m_Rotate,
        kPhysXXform.m_Translate, m_akPoses[m_aucIndices[1]]);
    m_afTimes[m_aucIndices[1]] = fT;

    // Decrement velocity reset each time we call
    if (m_ucResetVelocity)
    {
        m_ucResetVelocity--;
    }
    
    if (bForce)
    {
        m_afTimes[m_aucIndices[0]] = m_afTimes[m_aucIndices[1]];
        m_akPoses[m_aucIndices[0]] = m_akPoses[m_aucIndices[1]];
        m_ucResetVelocity = 1;
        m_bForcedUpdate = true;
    }
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::Interpolate(const float fT, NxMat34& kPose)
{
    float fDt = m_afTimes[m_aucIndices[1]] - m_afTimes[m_aucIndices[0]];
    if (NiAbs(fDt) < 1.0e-4f)
    {
        kPose = m_akPoses[m_aucIndices[1]];
        return;
    }
    
    float fU = (fT - m_afTimes[m_aucIndices[0]]) / fDt;

    // Translation is easy
    kPose.t = m_akPoses[m_aucIndices[0]].t + fU
        * (m_akPoses[m_aucIndices[1]].t - m_akPoses[m_aucIndices[0]].t);

    // Rotation: We handle this with slerp. ASSUMPTION: PhysX's slerp handles
    // extrapolation.
#ifdef _PS3
    NxQuat kQuat0;
    kQuat0.setWXYZ(0,0,0,0);
    kQuat0 = NxQuat(m_akPoses[m_aucIndices[0]].M);
    NxQuat kQuat1;
    kQuat1.setWXYZ(0,0,0,0);
    kQuat1 = NxQuat(m_akPoses[m_aucIndices[1]].M);
#else
    NxQuat kQuat0(m_akPoses[m_aucIndices[0]].M);
    NxQuat kQuat1(m_akPoses[m_aucIndices[1]].M);
#endif
    NxQuat kResQuat;
    kResQuat.slerp(fU, kQuat0, kQuat1);
    kPose.M.fromQuat(kResQuat);
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::ComputeVelocities(const NxMat34& kBodyLocalPose,
    NxVec3& kLinVel, NxVec3& kAngVel)
{
    float fDt = m_afTimes[m_aucIndices[1]] - m_afTimes[m_aucIndices[0]];
    NIASSERT(m_ucResetVelocity == 0 && NiAbs(fDt) > 1.0e-4)
    
    float fInvDt = 1.0f / fDt;

    // We need the translations to be the world coordinates of the COM
    NxVec3 kCOM0 = m_akPoses[m_aucIndices[0]] * kBodyLocalPose.t;
    NxVec3 kCOM1 = m_akPoses[m_aucIndices[1]] * kBodyLocalPose.t;

    // Compute the linear velocity of the COM, in world coordinates
    kLinVel = fInvDt * (kCOM1 - kCOM0);

    // Compute the angular velocity, by finding the quaternion that
    // rotates one state to the next, converting to an axis and angle,
    // and dividing the angle by the time it took to make the move.
    // This quantity is in world coordinates.
    NxMat33 kIncrRot;
    kIncrRot.multiplyTransposeRight(m_akPoses[m_aucIndices[1]].M,
        m_akPoses[m_aucIndices[0]].M);
#ifdef _PS3
    NxQuat kIncrQuat;
    kIncrQuat.setWXYZ(0,0,0,0);
    kIncrQuat = NxQuat(kIncrRot);
#else
    NxQuat kIncrQuat(kIncrRot);
#endif
    NxReal kAngle;
    NxVec3 kAxis;
    kIncrQuat.getAngleAxis(kAngle, kAxis);
    if (kAngle > 0.0f)
        kAngVel = fInvDt * kAngle * NI_PI / 180.0f * kAxis;
    else
        kAngVel.zero();
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::ComputeDynamicData(const float fT,
    const NxMat34& kBodyLocalPose, NxMat34& kBodyWorldPose, NxVec3& kLinVel,
    NxVec3& kAngVel)
{
    // Take the most recent data we have stored, and the velocity
    // we have stored, and place the actor so that, with that velocity,
    // and the given time, it will end up passing through the
    // target pose at the desired time.

    float fDt = m_afTimes[m_aucIndices[1]] - m_afTimes[m_aucIndices[0]];
    NIASSERT(m_ucResetVelocity == 0 && fDt > 1.0e-4)
    
    float fInvDt = 1.0f / fDt;

    // Figure out the body pose we are aiming for
    NxMat34 kBodyPose1 = m_akPoses[m_aucIndices[1]] * kBodyLocalPose;
    NxMat34 kBodyPose0 = m_akPoses[m_aucIndices[0]] * kBodyLocalPose;

    // Compute the linear velocity of the COM, in world coordinates
    kLinVel = fInvDt * (kBodyPose1.t - kBodyPose0.t);

    // Compute the angular velocity, by finding the quaternion that
    // rotates one state to the next, converting to an axis and angle,
    // and dividing the angle by the time it took to make the move.
    // This quantity is in world coordinates.
    NxMat33 kIncrRot;
    kIncrRot.multiplyTransposeRight(m_akPoses[m_aucIndices[1]].M,
        m_akPoses[m_aucIndices[0]].M);

#ifdef _PS3
    NxQuat kIncrQuat;
    kIncrQuat.setWXYZ(0,0,0,0);
    kIncrQuat = NxQuat(kIncrRot);
#else
    NxQuat kIncrQuat(kIncrRot);
#endif
    NxReal kAngle;
    NxVec3 kAxis;
    kIncrQuat.getAngleAxis(kAngle, kAxis);
    if (kAngle > 0.0f)
        kAngVel = fInvDt * kAngle * NI_PI / 180.0f * kAxis;
    else
        kAngVel.zero();

    // kBodyTargetPose is where we want to be at time fT.
    float fU = fT - m_afTimes[m_aucIndices[1]];
    kBodyWorldPose.t = kBodyPose1.t + fU * kLinVel;

    // Rotation: We handle this with slerp. ASSUMPTION: PhysX's slerp
    // handles extrapolation.
    fU = (fT - m_afTimes[m_aucIndices[0]]) * fInvDt;
#ifdef _PS3
    NxQuat kQuat0;
    kQuat0.setWXYZ(0,0,0,0);
    kQuat0 = NxQuat(kBodyPose0.M);
    NxQuat kQuat1;
    kQuat1.setWXYZ(0,0,0,0);
    kQuat1 = NxQuat(kBodyPose1.M);
#else
    NxQuat kQuat0(kBodyPose0.M);
    NxQuat kQuat1(kBodyPose1.M);
#endif
    NxQuat kResQuat;
    kResQuat.slerp(fU, kQuat0, kQuat1);
    kBodyWorldPose.M.fromQuat(kResQuat);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::CopyMembers(NiPhysXRigidBodySrc* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXSrc::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkSource = m_pkSource;

    for (NiUInt32 ui = 0; ui < 2; ui++)
    {
        pkDest->m_aucIndices[ui] = m_aucIndices[ui];
        pkDest->m_afTimes[ui] = m_afTimes[ui];
        pkDest->m_akPoses[ui] = m_akPoses[ui];
    }
    
    pkDest->m_ucResetVelocity = m_ucResetVelocity;
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXSrc::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    
    NiPhysXRigidBodySrc* pkClone = (NiPhysXRigidBodySrc*)pkCloneObj;
    
    NiObject* pkClonedSourceObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkSource, pkClonedSourceObj);
    NiAVObject* pkClonedSource = (NiAVObject*) pkClonedSourceObj;
    if (bCloned)
    {
        pkClone->m_pkSource = pkClonedSource;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXRigidBodySrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXSrc::RegisterStreamables(kStream))
        return false;

    m_pkSource->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::SaveBinary(NiStream& kStream)
{
    NiPhysXSrc::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkSource);
}
//---------------------------------------------------------------------------
NiObject* NiPhysXRigidBodySrc::CreateObject(const char**,
    NiUInt32)
{
    NIASSERT(false &&
        "CreateClass should never be called on NiPhysXRigidBodySrc.");
    return 0;
}//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::LoadBinary(NiStream& kStream)
{
    NiPhysXSrc::LoadBinary(kStream);

    m_pkSource = (NiAVObject*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodySrc::LinkObject(NiStream& kStream)
{
    NiPhysXSrc::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXRigidBodySrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXRigidBodySrc, pkObject));
    if(!NiPhysXSrc::IsEqual(pkObject))
        return false;
        
    NiPhysXRigidBodySrc* pkNewObject = (NiPhysXRigidBodySrc*)pkObject;

    if (!m_pkSource->IsEqual(pkNewObject->m_pkSource))
        return false;

    return true;
}
//---------------------------------------------------------------------------
