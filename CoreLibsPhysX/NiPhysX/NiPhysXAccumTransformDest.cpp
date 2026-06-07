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

NiImplementRTTI(NiPhysXAccumTransformDest, NiPhysXAccumRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXAccumTransformDest::NiPhysXAccumTransformDest()
{
    m_pkAccumTarget = 0;
    m_pkNonAccumTarget = 0;
}
//---------------------------------------------------------------------------
NiPhysXAccumTransformDest::NiPhysXAccumTransformDest(
    NiAVObject* pkAccumTarget, NiAVObject* pkNonAccumTarget,
    NxActor* pkActor, NxActor* pkActorParent)
    : NiPhysXAccumRigidBodyDest(pkActor, pkActorParent)
{
    m_pkAccumTarget = pkAccumTarget;
    m_pkNonAccumTarget = pkNonAccumTarget;
}
//---------------------------------------------------------------------------
NiPhysXAccumTransformDest::~NiPhysXAccumTransformDest()
{
    m_pkAccumTarget = 0;
    m_pkNonAccumTarget = 0;
}
//---------------------------------------------------------------------------
void NiPhysXAccumTransformDest::UpdateSceneGraph(const float fT,
    const NiTransform& kRootTransform, const float,
    const bool)
{
    if (!m_bActive)
        return;

    if (m_bOptimizeSleep && m_bSleeping)
    {
        if (m_bSleepUpdateDone)
        {
            return;
        }
        else
        {
            m_bSleepUpdateDone = true;
        }
    }

    NiQuaternion kAccumRotate;
    NiPoint3 kAccumTranslate;
    NiQuaternion kNonAccumRotate;
    NiPoint3 kNonAccumTranslate;
    
    // NiPhysXAccumRigidBodyDest defines this function for computing the
    // overall transform and splitting it into its component pieces.
    GetAccumTransforms(fT, kAccumRotate, kAccumTranslate,
        kNonAccumRotate, kNonAccumTranslate, kRootTransform);
        
    // Simply push the components to the nodes.
    m_pkAccumTarget->SetRotate(kAccumRotate);
    m_pkAccumTarget->SetTranslate(kAccumTranslate);
    m_pkNonAccumTarget->SetRotate(kNonAccumRotate);
    m_pkNonAccumTarget->SetTranslate(kNonAccumTranslate);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXAccumTransformDest);
//---------------------------------------------------------------------------
void NiPhysXAccumTransformDest::CopyMembers(NiPhysXAccumTransformDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXAccumRigidBodyDest::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkAccumTarget = m_pkAccumTarget;
    pkDest->m_pkNonAccumTarget = m_pkNonAccumTarget;
}
//---------------------------------------------------------------------------
void NiPhysXAccumTransformDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXAccumRigidBodyDest::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXAccumTransformDest* pkClone =
        (NiPhysXAccumTransformDest*)pkCloneObj;
            
    NiObject* pkClonedTargetObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkAccumTarget, pkClonedTargetObj);
    NiAVObject* pkClonedTarget = (NiAVObject*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkAccumTarget = pkClonedTarget;
    }

    pkClonedTargetObj = 0;
    bCloned =
        kCloning.m_pkCloneMap->GetAt(m_pkNonAccumTarget, pkClonedTargetObj);
    pkClonedTarget = (NiAVObject*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkNonAccumTarget = pkClonedTarget;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXAccumTransformDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXAccumRigidBodyDest::RegisterStreamables(kStream))
        return false;

    m_pkAccumTarget->RegisterStreamables(kStream);
    m_pkNonAccumTarget->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXAccumTransformDest::SaveBinary(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkAccumTarget);
    kStream.SaveLinkID(m_pkNonAccumTarget);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXAccumTransformDest);
//---------------------------------------------------------------------------
void NiPhysXAccumTransformDest::LoadBinary(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LoadBinary(kStream);

    m_pkAccumTarget = (NiAVObject*)kStream.ResolveLinkID();
    m_pkNonAccumTarget = (NiAVObject*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXAccumTransformDest::LinkObject(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXAccumTransformDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXAccumTransformDest, pkObject));
    if(!NiPhysXAccumRigidBodyDest::IsEqual(pkObject))
        return false;
        
    NiPhysXAccumTransformDest* pkNewObject =
        (NiPhysXAccumTransformDest*)pkObject;

    if (!m_pkAccumTarget->IsEqual(pkNewObject->m_pkAccumTarget))
        return false;

    if (!m_pkNonAccumTarget->IsEqual(pkNewObject->m_pkNonAccumTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
