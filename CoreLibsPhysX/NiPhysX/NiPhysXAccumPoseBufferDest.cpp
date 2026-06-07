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

NiImplementRTTI(NiPhysXAccumPoseBufferDest, NiPhysXAccumRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXAccumPoseBufferDest::NiPhysXAccumPoseBufferDest() 
    : m_pkSceneParent(0), m_pkSeqData(0), 
    m_uiAccumEvalIndex(INT_MAX), m_kAccumPBHandleTranslate(), 
    m_kAccumPBHandleRotate(), m_pkAccumPBTranslate(0), m_pkAccumPBRotate(0),
    m_uiNonAccumEvalIndex(INT_MAX), m_kNonAccumPBHandleTranslate(), 
    m_kNonAccumPBHandleRotate(), m_pkNonAccumPBTranslate(0), 
    m_pkNonAccumPBRotate(0), m_fLastUpdateTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
NiPhysXAccumPoseBufferDest::NiPhysXAccumPoseBufferDest(
    NxActor *pkActor, NxActor* pkActorParent, NiNode* pkSceneParent, 
    NiPhysXSequenceManager::SequenceData* pkSeqData,
    NiUInt32 uiAccumEvalIndex, 
    NiPoseBufferHandle kAccumPBHandleTranslate, 
    NiPoseBufferHandle kAccumPBHandleRotate, 
    NiUInt32 uiNonAccumEvalIndex, 
    NiPoseBufferHandle kNonAccumPBHandleTranslate, 
    NiPoseBufferHandle kNonAccumPBHandleRotate)
    : NiPhysXAccumRigidBodyDest(pkActor, pkActorParent),
    m_pkSceneParent(pkSceneParent), m_pkSeqData(pkSeqData), 
    m_uiAccumEvalIndex(uiAccumEvalIndex), 
    m_kAccumPBHandleTranslate(kAccumPBHandleTranslate), 
    m_kAccumPBHandleRotate(kAccumPBHandleRotate), 
    m_pkAccumPBTranslate(0), m_pkAccumPBRotate(0),
    m_uiNonAccumEvalIndex(uiNonAccumEvalIndex), 
    m_kNonAccumPBHandleTranslate(kNonAccumPBHandleTranslate), 
    m_kNonAccumPBHandleRotate(kNonAccumPBHandleRotate), 
    m_pkNonAccumPBTranslate(0), m_pkNonAccumPBRotate(0), 
    m_fLastUpdateTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
NiPhysXAccumPoseBufferDest::~NiPhysXAccumPoseBufferDest()
{
    m_pkSceneParent = 0;
    m_pkSeqData = 0;
    m_pkAccumPBTranslate = 0;
    m_pkAccumPBRotate = 0;
    m_pkNonAccumPBTranslate = 0;
    m_pkNonAccumPBRotate = 0;
}
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::SetActive(const bool bActive)
{
    NIASSERT(m_pkSeqData);

    m_bActive = bActive;

    NiControllerSequence* pkActiveAnimSeq = 
        m_pkSeqData->GetActiveAnimSequence();
    if (pkActiveAnimSeq)
    {
        // Disable the associated evaluator channels within the active
        // animation sequence so they don't write to the pose buffer.
        NIASSERT(m_uiAccumEvalIndex != INT_MAX);
        pkActiveAnimSeq->DisableEvaluatorChannelAt(
            m_uiAccumEvalIndex, NiEvaluator::EVALPOSINDEX, false);
        pkActiveAnimSeq->DisableEvaluatorChannelAt(
            m_uiAccumEvalIndex, NiEvaluator::EVALROTINDEX, false);

        NIASSERT(m_uiNonAccumEvalIndex != INT_MAX);
        pkActiveAnimSeq->DisableEvaluatorChannelAt(
            m_uiNonAccumEvalIndex, NiEvaluator::EVALPOSINDEX, false);
        pkActiveAnimSeq->DisableEvaluatorChannelAt(
            m_uiNonAccumEvalIndex, NiEvaluator::EVALROTINDEX, false);

        // Ensure the associated pose buffer locations are validated.
        NiPoseBuffer* pkPoseBuffer = pkActiveAnimSeq->GetPoseBuffer();
        NIASSERT(pkPoseBuffer);
        pkPoseBuffer->SetValid(m_kAccumPBHandleTranslate, true);
        pkPoseBuffer->SetValid(m_kAccumPBHandleRotate, true);
        pkPoseBuffer->SetValid(m_kNonAccumPBHandleTranslate, true);
        pkPoseBuffer->SetValid(m_kNonAccumPBHandleRotate, true);
        // Also validate the accum delta locations.
        pkPoseBuffer->SetValid(NiPoseBufferHandle(PBPOINT3CHANNEL, 1), true);
        pkPoseBuffer->SetValid(NiPoseBufferHandle(PBROTCHANNEL, 1), true);
    }
}
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::UpdateSceneGraph(const float fT,
    const NiTransform& kRootTransform, const float,
    const bool bForce)
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

    // Don't update the pose buffer (unless forced) if we've 
    // already been updated at this time.
    if (fT == m_fLastUpdateTime && !bForce)
    {
        return;
    }
    m_fLastUpdateTime = fT;

    // Compute where PhysX wants the character for the next frame.
    NiQuaternion kAccumRotate;
    NiPoint3 kAccumTranslate;
    NiQuaternion kNonAccumRotate;
    NiPoint3 kNonAccumTranslate;
    GetAccumTransforms(fT, kAccumRotate, kAccumTranslate, kNonAccumRotate,
        kNonAccumTranslate, kRootTransform);

    // Get the active animation sequence and associated controller manager.
    NiControllerSequence* pkActiveAnimSeq = 
        m_pkSeqData->GetActiveAnimSequence();
    NIASSERT(pkActiveAnimSeq);

    NiControllerManager* pkControllerManager = pkActiveAnimSeq->GetOwner();
    NIASSERT(pkControllerManager);

    // Get the last accum transform from the character. 
    // The accum delta is relative to this transform.
    NiQuatTransform kLastAccumTransform;
    pkControllerManager->GetAccumulatedTransform(kLastAccumTransform);

    // Compute the accum delta.
    NiQuatTransform kAccumDelta;
    NiQuatTransform kAccumTransform(kAccumTranslate, kAccumRotate, 1.0f);
    if (kLastAccumTransform.IsTransformInvalid())
    {
        // Invalid last transform: use the accum transform as the accum delta.
        kAccumDelta = kAccumTransform;
    }
    else
    {
        // Compute the delta between the last and desired transforms.
        NiQuatTransform kLastAccumInv;
        kLastAccumTransform.Invert(kLastAccumInv);
        kAccumDelta = kLastAccumInv * kAccumTransform;
        if (kLastAccumInv.IsRotateValid())
        {
            NiMatrix3 kRefRot;
            kLastAccumInv.GetRotate().ToRotation(kRefRot);
            kAccumDelta.SetTranslate(kRefRot * kAccumDelta.GetTranslate());
        }
    }

    // Ensure the pointers to the pose buffer data items are correct.
    NIASSERT(m_pkSeqData);
    m_pkSeqData->UpdateDestPoseBufferDataPointers();

    // Push the PhysX values into the pose buffer.
    NIASSERT(pkActiveAnimSeq->GetPoseBuffer());
    if (m_pkAccumPBTranslate)
    {
        // Write accum translation.
        NIASSERT(kAccumTranslate.x != -NI_INFINITY);
        *m_pkAccumPBTranslate = kAccumTranslate;

        // Write accum translation delta.
        NIASSERT((m_pkAccumPBTranslate + 1) == 
            (NiPoint3*)pkActiveAnimSeq->GetPoseBuffer()->
            GetDataPointer(NiPoseBufferHandle(PBPOINT3CHANNEL, 1)));
        NIASSERT(kAccumDelta.IsTranslateValid());
        *(m_pkAccumPBTranslate + 1) = kAccumDelta.GetTranslate();
    }

    if (m_pkAccumPBRotate)
    {
        // Write accum rotation.
        NIASSERT(kAccumRotate.GetX() != -NI_INFINITY);
        *m_pkAccumPBRotate = kAccumRotate;

        // Write accum rotation delta.
        NIASSERT((m_pkAccumPBRotate + 1) == 
            (NiQuaternion*)pkActiveAnimSeq->GetPoseBuffer()->
            GetDataPointer(NiPoseBufferHandle(PBROTCHANNEL, 1)));
        NIASSERT(kAccumDelta.IsRotateValid());
        *(m_pkAccumPBRotate + 1) = kAccumDelta.GetRotate();
    }

    if (m_pkNonAccumPBTranslate)
    {
        // Write non-accum translation.
        *m_pkNonAccumPBTranslate = kNonAccumTranslate;
    }

    if (m_pkNonAccumPBRotate)
    {
        // Write non-accum rotation.
        *m_pkNonAccumPBRotate = kNonAccumRotate;
    }
}
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::UpdatePoseBufferDataPointers(
    const NiPoseBuffer* pkPoseBuffer)
{
    NIASSERT(pkPoseBuffer);

    if (m_kAccumPBHandleTranslate.IsValid())
    {
        m_pkAccumPBTranslate = (NiPoint3 *)pkPoseBuffer->GetDataPointer(
            m_kAccumPBHandleTranslate);
    }

    if (m_kAccumPBHandleRotate.IsValid())
    {
        m_pkAccumPBRotate = (NiQuaternion *)pkPoseBuffer->GetDataPointer(
            m_kAccumPBHandleRotate);
    }

    if (m_kNonAccumPBHandleTranslate.IsValid())
    {
        m_pkNonAccumPBTranslate = (NiPoint3 *)pkPoseBuffer->GetDataPointer(
            m_kNonAccumPBHandleTranslate);
    }

    if (m_kNonAccumPBHandleRotate.IsValid())
    {
        m_pkNonAccumPBRotate = (NiQuaternion *)pkPoseBuffer->GetDataPointer(
            m_kNonAccumPBHandleRotate);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiObject* NiPhysXAccumPoseBufferDest::CreateClone(NiCloningProcess&)
{
    NIASSERT(false && "NiPhysXAccumPoseBufferDest should not be cloned.\n");
    return NULL;
}     
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::CopyMembers(
    NiPhysXAccumPoseBufferDest*, NiCloningProcess&)
{
    NIASSERT(false && "NiPhysXAccumPoseBufferDest should not be cloned.\n");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXAccumPoseBufferDest::RegisterStreamables(NiStream& kStream)
{
    NIASSERT(false && "NiPhysXAccumPoseBufferDest is a runtime object which "
        "should not be streamed.\n");

    if (!NiPhysXAccumRigidBodyDest::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::SaveBinary(NiStream& kStream)
{
    NIASSERT(false && "NiPhysXAccumPoseBufferDest is a runtime object which "
        "should not be streamed.\n");

    NiPhysXAccumRigidBodyDest::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXAccumPoseBufferDest);
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::LoadBinary(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LoadBinary(kStream);

    // NiPhysXAccumPoseBufferDest replaced the deprecated 
    // NiPhysXAccumInterpolatorDest class. 
    // Read the NiPhysXAccumInterpolatorDest values, if needed.
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 64))
    {
        // m_pkAccumTarget: Ignore value.
        kStream.ResolveLinkID();
        // m_pkNonAccumTarget: Ignore value.
        kStream.ResolveLinkID();
        // m_pkBlendInterp: Ignore value.
        kStream.ResolveLinkID();
        // m_pkSceneParent: Ignore value.
        kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiPhysXAccumPoseBufferDest::LinkObject(NiStream& kStream)
{
    NiPhysXAccumRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXAccumPoseBufferDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(false && "NiPhysXAccumPoseBufferDest is a runtime object which "
        "should not be streamed.\n");

    NIASSERT(NiIsKindOf(NiPhysXAccumPoseBufferDest, pkObject));
    if(!NiPhysXAccumRigidBodyDest::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
