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

NiImplementRTTI(NiPhysXPoseBufferDest, NiPhysXRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXPoseBufferDest::NiPhysXPoseBufferDest() 
    : m_pkSceneParent(0), m_pkSeqData(0), m_uiEvalIndex(INT_MAX), 
    m_kPBHandleTranslate(), m_kPBHandleRotate(), m_pkPBTranslate(0), 
    m_pkPBRotate(0), m_fLastUpdateTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
NiPhysXPoseBufferDest::NiPhysXPoseBufferDest(NxActor *pkActor, 
    NxActor* pkActorParent, NiNode* pkSceneParent, 
    NiPhysXSequenceManager::SequenceData* pkSequenceData,
    NiUInt32 uiEvalIndex, NiPoseBufferHandle kPBHandleTranslate, 
    NiPoseBufferHandle kPBHandleRotate)
    : NiPhysXRigidBodyDest(pkActor, pkActorParent), 
    m_pkSceneParent(pkSceneParent), m_pkSeqData(pkSequenceData), 
    m_uiEvalIndex(uiEvalIndex), 
    m_kPBHandleTranslate(kPBHandleTranslate), 
    m_kPBHandleRotate(kPBHandleRotate), m_pkPBTranslate(0), 
    m_pkPBRotate(0), m_fLastUpdateTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
NiPhysXPoseBufferDest::~NiPhysXPoseBufferDest()
{
    m_pkSceneParent = 0;
    m_pkSeqData = 0;
    m_pkPBTranslate = 0;
    m_pkPBRotate = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::SetActive(const bool bActive)
{
    NIASSERT(m_pkSeqData);

    m_bActive = bActive;

    NiControllerSequence* pkActiveAnimSeq = 
        m_pkSeqData->GetActiveAnimSequence();
    if (pkActiveAnimSeq)
    {
        // Disable the associated evaluator channels within the active
        // animation sequence so they don't write to the pose buffer.
        NIASSERT(m_uiEvalIndex != INT_MAX);
        pkActiveAnimSeq->DisableEvaluatorChannelAt(
            m_uiEvalIndex, NiEvaluator::EVALPOSINDEX, false);
        pkActiveAnimSeq->DisableEvaluatorChannelAt(
            m_uiEvalIndex, NiEvaluator::EVALROTINDEX, false);

        // Ensure the associated pose buffer locations are validated.
        NiPoseBuffer* pkPoseBuffer = pkActiveAnimSeq->GetPoseBuffer();
        NIASSERT(pkPoseBuffer);
        pkPoseBuffer->SetValid(m_kPBHandleTranslate, true);
        pkPoseBuffer->SetValid(m_kPBHandleRotate, true);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::UpdateSceneGraph(const float fT,
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

    // Compute where PhysX wants the destination for the next frame.
    NiMatrix3 kRotate;
    NiPoint3 kTranslate;
    GetTransforms(fT, kRotate, kTranslate, kRootTransform);
    NiQuaternion kRotateQuat;
    kRotateQuat.FromRotation(kRotate);

    // Ensure the pointers to the pose buffer data items are correct.
    NIASSERT(m_pkSeqData);
    m_pkSeqData->UpdateDestPoseBufferDataPointers();

    // Push the PhysX values into the pose buffer.
    if (m_pkPBTranslate)
    {
        *m_pkPBTranslate = kTranslate;
    }
    if (m_pkPBRotate)
    {
        *m_pkPBRotate = kRotateQuat;
    }
}
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::UpdatePoseBufferDataPointers(
    const NiPoseBuffer* pkPoseBuffer)
{
    NIASSERT(pkPoseBuffer);

    if (m_kPBHandleTranslate.IsValid())
    {
        m_pkPBTranslate = (NiPoint3 *)pkPoseBuffer->GetDataPointer(
            m_kPBHandleTranslate);
    }

    if (m_kPBHandleRotate.IsValid())
    {
        m_pkPBRotate = (NiQuaternion *)pkPoseBuffer->GetDataPointer(
            m_kPBHandleRotate);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiObject* NiPhysXPoseBufferDest::CreateClone(NiCloningProcess&)
{
    NIASSERT(false && "NiPhysXPoseBufferDest should not be cloned.\n");
    return NULL;
}     
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::CopyMembers(NiPhysXPoseBufferDest*,
    NiCloningProcess&)
{
    NIASSERT(false && "NiPhysXPoseBufferDest should not be cloned.\n");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPoseBufferDest::RegisterStreamables(NiStream& kStream)
{
    NIASSERT(false && "NiPhysXPoseBufferDest is a runtime object which "
        "should not be streamed.\n");

    if (!NiPhysXRigidBodyDest::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::SaveBinary(NiStream& kStream)
{
    NIASSERT(false && "NiPhysXPoseBufferDest is a runtime object which "
        "should not be streamed.\n");

    NiPhysXRigidBodyDest::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPoseBufferDest);
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::LoadBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LoadBinary(kStream);

    // NiPhysXPoseBufferDest replaced the deprecated 
    // NiPhysXInterpolatorDest class.
    // Read the NiPhysXInterpolatorDest values, if needed.
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 64))
    {
        // m_pkTarget: Ignore value.
        kStream.ResolveLinkID();
        // m_pkSceneParent: Ignore value
        kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiPhysXPoseBufferDest::LinkObject(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPoseBufferDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(false && "NiPhysXPoseBufferDest is a runtime object which "
        "should not be streamed.\n");

    NIASSERT(NiIsKindOf(NiPhysXPoseBufferDest, pkObject));
    if(!NiPhysXRigidBodyDest::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
