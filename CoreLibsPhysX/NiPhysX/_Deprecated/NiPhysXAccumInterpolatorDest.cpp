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
#include "NiPhysXAccumInterpolatorDest.h"

NiImplementRTTI(NiPhysXAccumInterpolatorDest, NiPhysXAccumRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXAccumInterpolatorDest::NiPhysXAccumInterpolatorDest()
{
    m_pkAccumTarget = 0;
    m_pkNonAccumTarget = 0;
    m_pkControllerManager = 0;
    m_pkSceneParent = 0;
    m_pkActiveAnimSeq = 0;
}
//---------------------------------------------------------------------------
NiPhysXAccumInterpolatorDest::NiPhysXAccumInterpolatorDest(
    NiInterpolator* pkAccumTarget, NiInterpolator* pkNonAccumTarget,
    NiControllerManager* pkControllerManager,
    NiNode* pkSceneParent, NxActor *pkActor, NxActor* pkActorParent)
    : NiPhysXAccumRigidBodyDest(pkActor, pkActorParent)
{
    m_pkAccumTarget = pkAccumTarget;
    m_pkNonAccumTarget = pkNonAccumTarget;
    m_pkControllerManager = pkControllerManager;
    m_pkSceneParent = pkSceneParent;
    m_pkActiveAnimSeq = 0;
}
//---------------------------------------------------------------------------
NiPhysXAccumInterpolatorDest::~NiPhysXAccumInterpolatorDest()
{
    m_pkAccumTarget = 0;
    m_pkNonAccumTarget = 0;
    m_pkControllerManager = 0;
    m_pkSceneParent = 0;
    m_pkActiveAnimSeq = 0;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiPhysXAccumInterpolatorDest::GetActiveAnimSequence() 
    const
{
    NIASSERT(GetActive() || !m_pkActiveAnimSeq);
    NIASSERT(!GetActive() || m_pkActiveAnimSeq);
    return m_pkActiveAnimSeq;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::SetActiveAnimSequence(
    NiControllerSequence* pkActiveAnimSeq)
{
    m_pkActiveAnimSeq = pkActiveAnimSeq;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::UpdateSceneGraph(const float fT,
    const NiTransform& kRootTransform, const float,
    const bool)
{
    if (!m_bActive || !m_pkActiveAnimSeq)
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

    // Inform the active sequence as to where the character is currently 
    // located since the accumulation delta must be relative to that location.
    NiQuatTransform kAccumTransform;
    if (m_pkControllerManager)
    {
        m_pkControllerManager->GetAccumulatedTransform(kAccumTransform);
    }
    if (kAccumTransform.IsTransformInvalid())
    {
        kAccumTransform.SetTranslate(NiPoint3::ZERO);
        kAccumTransform.SetRotate(NiQuaternion::IDENTITY);
        kAccumTransform.SetScale(1.0f);
    }
    m_pkActiveAnimSeq->SetLastAccumTransform(kAccumTransform);

    // Compute where PhysX wants the character for the next frame.
    NiQuaternion kAccumRotate;
    NiPoint3 kAccumTranslate;
    NiQuaternion kNonAccumRotate;
    NiPoint3 kNonAccumTranslate;
    GetAccumTransforms(fT, kAccumRotate, kAccumTranslate, kNonAccumRotate,
        kNonAccumTranslate, kRootTransform);

    // Push the PhysX values into the target interpolators.
    NiQuatTransform kAccumLocal(kAccumTranslate, kAccumRotate, 1.0f);
    if (NiIsKindOf(NiTransformInterpolator, m_pkAccumTarget))
    {
        NiTransformInterpolator* pkTarget =
            (NiTransformInterpolator*)m_pkAccumTarget;
        pkTarget->SetPoseRotate(kAccumLocal.GetRotate());
        pkTarget->SetPoseTranslate(kAccumLocal.GetTranslate());
    }
    else if (NiIsKindOf(NiBSplineTransformInterpolator, m_pkAccumTarget))
    {
        NiBSplineTransformInterpolator* pkTarget =
            (NiBSplineTransformInterpolator*)m_pkAccumTarget;
        pkTarget->SetPoseRotate(kAccumLocal.GetRotate());
        pkTarget->SetPoseTranslate(kAccumLocal.GetTranslate());
    }
    else
    {
        // You hit this if you passed in an interpolator that does not
        // support SetPose functions.
        NIASSERT(false && "Invalid interpolator type");
    }

    if (NiIsKindOf(NiTransformInterpolator, m_pkNonAccumTarget))
    {
        NiTransformInterpolator* pkTarget =
            (NiTransformInterpolator*)m_pkNonAccumTarget;
        pkTarget->SetPoseRotate(kNonAccumRotate);
        pkTarget->SetPoseTranslate(kNonAccumTranslate);
    }
    else if (NiIsKindOf(NiBSplineTransformInterpolator,
        m_pkNonAccumTarget))
    {
        NiBSplineTransformInterpolator* pkTarget =
            (NiBSplineTransformInterpolator*)m_pkNonAccumTarget;
        pkTarget->SetPoseRotate(kNonAccumRotate);
        pkTarget->SetPoseTranslate(kNonAccumTranslate);
    }
    else
    {
        // You hit this if you passed in an interpolator that does not
        // support SetPose functions.
        NIASSERT(false && "Invalid interpolator type");
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXAccumInterpolatorDest);
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::CopyMembers(
    NiPhysXAccumInterpolatorDest* pkDest, NiCloningProcess& kCloning)
{
    NiPhysXAccumRigidBodyDest::CopyMembers(pkDest, kCloning);

    pkDest->m_pkAccumTarget = m_pkAccumTarget;
    pkDest->m_pkNonAccumTarget = m_pkNonAccumTarget;
    pkDest->m_pkControllerManager = m_pkControllerManager;
    pkDest->m_pkSceneParent = m_pkSceneParent;
    pkDest->m_pkActiveAnimSeq = m_pkActiveAnimSeq;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXAccumRigidBodyDest::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXAccumInterpolatorDest* pkClone =
        (NiPhysXAccumInterpolatorDest*)pkCloneObj;

    NiObject* pkClonedTargetObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkAccumTarget, pkClonedTargetObj);
    NiInterpolator* pkClonedTarget = (NiInterpolator*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkAccumTarget = pkClonedTarget;
    }

    pkClonedTargetObj = 0;
    bCloned =
        kCloning.m_pkCloneMap->GetAt(m_pkNonAccumTarget, pkClonedTargetObj);
    pkClonedTarget = (NiInterpolator*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkNonAccumTarget = pkClonedTarget;
    }

    pkClonedTargetObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkControllerManager,
        pkClonedTargetObj);
    NiControllerManager* pkClonedControllerManager =
        (NiControllerManager*)pkClonedTargetObj;
    if (bCloned)
    {
        pkClone->m_pkControllerManager = pkClonedControllerManager;
    }

    NiObject* pkClonedParentObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkSceneParent, pkClonedParentObj);
    NiNode* pkClonedParent = (NiNode*)pkClonedParentObj;
    if (bCloned)
    {
        pkClone->m_pkSceneParent = pkClonedParent;
    }

    pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkActiveAnimSeq, pkCloneObj);
    NiControllerSequence* pkClonedActiveAnimSeq =
        (NiControllerSequence*)pkCloneObj;
    if (bCloned)
    {
        pkClone->m_pkActiveAnimSeq = pkClonedActiveAnimSeq;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXAccumInterpolatorDest::RegisterStreamables(NiStream& kStream)
{
    NIASSERT(false && "NiPhysXAccumInterpolatorDest is deprecated.\n");

    if (!NiPhysXAccumRigidBodyDest::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::SaveBinary(NiStream& kStream)
{
    NIASSERT(false && "NiPhysXAccumInterpolatorDest is deprecated.\n");

    NiPhysXAccumRigidBodyDest::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXAccumInterpolatorDest);
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::LoadBinary(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 64))
    {
        m_pkAccumTarget = (NiInterpolator*)kStream.ResolveLinkID();
        m_pkNonAccumTarget = (NiInterpolator*)kStream.ResolveLinkID();
        // Use of the blend interpolator to access the accum transform value
        // was changed when the NiSequenceData class was introduced.
        // Now, the accum transform is accessed via the controller manager.
        //
        // m_pkBlendInterp: Ignore value.
        kStream.ResolveLinkID();
        m_pkSceneParent = (NiNode*)kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::LinkObject(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXAccumInterpolatorDest::PostLinkObject(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::PostLinkObject(kStream);

    // Find the controller manager.
    // Use of the blend interpolator to access the accum transform value
    // was changed when the NiSequenceData class was introduced.
    // Now, the accum transform is accessed via the controller manager.
    if (m_pkSceneParent)
    {
        m_pkControllerManager = NiGetController(NiControllerManager,
            m_pkSceneParent);
        if (!m_pkControllerManager)
        {
            for (unsigned int ui = 0; 
                ui < m_pkSceneParent->GetArrayCount(); ui++)
            {
                NiAVObject* pkChild = m_pkSceneParent->GetAt(ui);
                if (pkChild)
                {
                    m_pkControllerManager = NiGetController(
                        NiControllerManager, pkChild);
                    if (m_pkControllerManager)
                    {
                        break;
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiPhysXAccumInterpolatorDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(false && "NiPhysXAccumInterpolatorDest is deprecated.\n");

    NIASSERT(NiIsKindOf(NiPhysXAccumInterpolatorDest, pkObject));
    if(!NiPhysXAccumRigidBodyDest::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
