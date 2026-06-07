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

NiImplementRTTI(NiPhysXTransformDest, NiPhysXRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXTransformDest::NiPhysXTransformDest()
{
    m_pkTarget = 0;
}
//---------------------------------------------------------------------------
NiPhysXTransformDest::NiPhysXTransformDest(NiAVObject* pkTarget,
    NxActor* pkActor, NxActor* pkActorParent) : NiPhysXRigidBodyDest(pkActor, 
    pkActorParent)
{
    m_pkTarget = pkTarget;
}
//---------------------------------------------------------------------------
NiPhysXTransformDest::~NiPhysXTransformDest()
{
    m_pkTarget = 0;
}
//---------------------------------------------------------------------------
void NiPhysXTransformDest::UpdateSceneGraph(const float fT,
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

    NiMatrix3 kRotate;
    NiPoint3 kTranslate;
    GetTransforms(fT, kRotate, kTranslate, kRootTransform);

    m_pkTarget->SetRotate(kRotate);
    m_pkTarget->SetTranslate(kTranslate);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXTransformDest);
//---------------------------------------------------------------------------
void NiPhysXTransformDest::CopyMembers(NiPhysXTransformDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXRigidBodyDest::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkTarget = m_pkTarget;
}
//---------------------------------------------------------------------------
void NiPhysXTransformDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXRigidBodyDest::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXTransformDest* pkClone = (NiPhysXTransformDest*)pkCloneObj;
            
    NiObject* pkClonedTargetObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkTarget, pkClonedTargetObj);
    NiAVObject* pkClonedTarget = (NiAVObject*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkTarget = pkClonedTarget;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXTransformDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXRigidBodyDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXTransformDest::SaveBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkTarget);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXTransformDest);
//---------------------------------------------------------------------------
void NiPhysXTransformDest::LoadBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LoadBinary(kStream);

    m_pkTarget = (NiAVObject*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXTransformDest::LinkObject(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXTransformDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXTransformDest, pkObject));
    if(!NiPhysXRigidBodyDest::IsEqual(pkObject))
        return false;
        
    NiPhysXTransformDest* pkNewObject = (NiPhysXTransformDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget))
        return false;

    return true;
}
//---------------------------------------------------------------------------
