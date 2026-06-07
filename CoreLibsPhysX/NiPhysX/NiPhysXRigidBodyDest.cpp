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

NiImplementRTTI(NiPhysXRigidBodyDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXRigidBodyDest::NiPhysXRigidBodyDest()
{
    m_pkActor = 0;
    m_pkActorParent = 0;

    m_bOptimizeSleep = false;
    m_bSleeping = false;
    m_bSleepUpdateDone = false;
    
    m_aucIndices[0] = 0;
    m_aucIndices[1] = 1;
    m_afTimes[0] = -2.0f; // This makes sure interpolation works immediately
    m_afTimes[1] = -1.0f; // Should always call UpdateDestinations before use
    m_akPoses[0].id();
    m_akPoses[1].id();
}
//---------------------------------------------------------------------------
NiPhysXRigidBodyDest::NiPhysXRigidBodyDest(NxActor* pkActor, 
    NxActor* pkActorParent) : NiPhysXDest()
{
    m_pkActor = pkActor;
    m_pkActorParent = pkActorParent;

    m_bOptimizeSleep = false;
    m_bSleeping = false;
    m_bSleepUpdateDone = false;

    m_aucIndices[0] = 0;
    m_aucIndices[1] = 1;
    m_afTimes[0] = -2.0f; // This makes sure interpolation works immediately
    m_afTimes[1] = -1.0f; // Should always call UpdateDestinations before use
    m_akPoses[0].id();
    m_akPoses[1].id();
}
//---------------------------------------------------------------------------
NiPhysXRigidBodyDest::~NiPhysXRigidBodyDest()
{
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::UpdateFromActors(const float fT,
    const NiTransform&, const float fScalePToW,
    const bool bForce)
{
    if (!m_pkActor)
    {
        if (bForce)
        {
            m_afTimes[0] = fT;
            m_afTimes[1] = fT;
        }
        return;
    }

    if ((fT <= m_afTimes[m_aucIndices[1]] || !m_bActive) && !bForce)
        return;
        
    // Optimize for a sleeping actor.
    // This slightly convoluted piece of code makes sure that the update
    // still happens the first time the actor is found to be sleeping.
    if (m_bOptimizeSleep && m_pkActor->isSleeping())
    {
        if (m_bSleeping && !bForce)
        {
            // Update the time for the most recent state, so that
            // interpolation behaves itself.
            m_afTimes[m_aucIndices[1]] = fT;
            return;
        }
        else
        {
            m_bSleeping = true;
            m_bSleepUpdateDone = false;
        }
    }
    else
    {
        m_bSleeping = false;
    }

    // Swap indices.
    unsigned char ucTemp = m_aucIndices[0];
    m_aucIndices[0] = m_aucIndices[1];
    m_aucIndices[1] = ucTemp;
        
    // If we have an actor parent, store the local transform
    if (m_pkActorParent)
    {
        // X(Parent<-Obj) = X(Parent<-Scene) X(Scene<-Obj)
        
        // Get the parent's pose
        NxMat34 kParentPose = m_pkActorParent->getGlobalPose();
        kParentPose.t *= fScalePToW;
        
        // Get the child's pose
        NxMat34 kChildPose = m_pkActor->getGlobalPose();
        kChildPose.t *= fScalePToW;
        
        m_akPoses[m_aucIndices[1]].multiplyInverseRTLeft(
            kParentPose, kChildPose);
    }
    else
    {
        // Extract the PhysX actor transform
        m_akPoses[m_aucIndices[1]] = m_pkActor->getGlobalPose();
        
        // Scale translation
        m_akPoses[m_aucIndices[1]].t *= fScalePToW;
    }

    m_afTimes[m_aucIndices[1]] = fT;
    
    if (bForce)
    {
        m_afTimes[m_aucIndices[0]] = m_afTimes[m_aucIndices[1]];
        m_akPoses[m_aucIndices[0]] = m_akPoses[m_aucIndices[1]];
    }
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::Interpolate(const float fT, NiQuaternion& kRotation,
    NiPoint3& kTranslate)
{
    float fDt = m_afTimes[m_aucIndices[1]] - m_afTimes[m_aucIndices[0]];
    if (NiAbs(fDt) < 1.0e-4f)
    {
        NiPhysXTypes::NxMat34ToNiQuatTransform(
            m_akPoses[m_aucIndices[1]], kRotation, kTranslate);
        return;
    }
    
    float fU = (fT - m_afTimes[m_aucIndices[0]]) / fDt;

    // Translation is easy
    NxVec3 kNxTranslate = m_akPoses[m_aucIndices[0]].t + fU
        * (m_akPoses[m_aucIndices[1]].t - m_akPoses[m_aucIndices[0]].t);
    NiPhysXTypes::NxVec3ToNiPoint3(kNxTranslate, kTranslate);

    // Rotation: We handle this with slerp.
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
    NiPhysXTypes::NxQuatToNiQuaternion(kResQuat, kRotation);
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::GetTransforms(float fTime, NiMatrix3& kRotation,
    NiPoint3& kTranslation, const NiTransform& kRootTransform)
{
    NiPoint3 kPoseTranslate;
    NiQuaternion kPoseRotate;
    
    if (fTime == m_afTimes[m_aucIndices[1]])
    {
        NiPhysXTypes::NxMat34ToNiQuatTransform(m_akPoses[m_aucIndices[1]],
            kPoseRotate, kPoseTranslate);
    }
    else
    {
        Interpolate(fTime, kPoseRotate, kPoseTranslate);
    }
    
    if (m_pkActorParent)
    {
        kPoseRotate.ToRotation(kRotation);
        kTranslation = kPoseTranslate;

        NiAVObject* pkSceneParent = GetSceneGraphParent();
        
        // You hit this assert if the actor has a parent but there is no scene
        // graph parent. This is illegal - the actor parent is supposed to be
        // an actor that is driving the scene graph parent, so there cannot
        // be a actor parent if there is no scene graph parent.
        NIASSERT(pkSceneParent);
        NIASSERT(pkSceneParent->GetWorldScale()!=0.0f);

        kTranslation /= pkSceneParent->GetWorldScale();
    }
    else
    {
        // X(Parent<-Obj) = X(Parent<-World) X(World<-Obj) 
        //                = X(Parent<-World) X(World<-Scene) X(Scene<-Obj)
        
        NiMatrix3 kActorRot;
        kPoseRotate.ToRotation(kActorRot);
        
        // Compute world coordinates: X(World<-Scene) X(Scene<-Obj)
        NiMatrix3 kWorldRotMat = kRootTransform.m_Rotate * kActorRot;
        NiPoint3 kWorldTrans = kRootTransform.m_Translate
            + kRootTransform.m_fScale *
            (kRootTransform.m_Rotate * kPoseTranslate);

        // Look for a parent
        NiAVObject* pkSceneParent = GetSceneGraphParent();
        if (pkSceneParent)
        {
            NiMatrix3 kParentRot = pkSceneParent->GetWorldRotate();

            // Convert into the model space rotation necessary to
            // result in the desired world space rotation.
            kRotation = kParentRot.TransposeTimes(kWorldRotMat);

            // Need to figure out the model space translation
            NIASSERT(pkSceneParent->GetWorldScale()!=0.0f);
            kTranslation =
                (1.0f / pkSceneParent->GetWorldScale())
                * (kParentRot.Inverse()
                * (kWorldTrans - pkSceneParent->GetWorldTranslate()));
        }
        else // No parent is present. Use global coordinates.
        {
            kRotation = kWorldRotMat;
            kTranslation = kWorldTrans;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::CopyMembers(NiPhysXRigidBodyDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXDest::CopyMembers(pkDest, kCloning);

    pkDest->m_bOptimizeSleep = m_bOptimizeSleep;
    pkDest->m_bSleeping = false;
    pkDest->m_bSleepUpdateDone = false;
    for (NiUInt32 ui = 0; ui < 2; ui++)
    {
        pkDest->m_aucIndices[ui] = m_aucIndices[ui];
        pkDest->m_afTimes[ui] = m_afTimes[ui];
        pkDest->m_akPoses[ui] = m_akPoses[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXRigidBodyDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiObject* NiPhysXRigidBodyDest::CreateObject(const char**,
    NiUInt32)
{
    NIASSERT(false &&
        "CreateClass should never be called on NiPhysXRigidBodyDest.");
    return 0;
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXRigidBodyDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXRigidBodyDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXRigidBodyDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;

    NiPhysXRigidBodyDest* pkOther = (NiPhysXRigidBodyDest*)pkObject;
    if (pkOther->m_bOptimizeSleep != m_bOptimizeSleep)
        return false;

    return true;
}
//---------------------------------------------------------------------------
