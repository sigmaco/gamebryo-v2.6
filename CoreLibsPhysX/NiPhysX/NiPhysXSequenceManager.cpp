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

#include <NiAnimationConstants.h>
#include "NiPhysXSequenceManager.h"
#include "NiPhysXAccumRigidBodyDest.h"
#include "NiPhysXAccumPoseBufferDest.h"
#include "NiPhysXPoseBufferDest.h"
#include "NiPhysXScene.h"

NiImplementRTTI(NiPhysXSequenceManager, NiObject);

//---------------------------------------------------------------------------
NiPhysXSequenceManager::NiPhysXSequenceManager()
{
    m_spControllerManager = 0;
    m_spProp = 0;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::~NiPhysXSequenceManager()
{
    CleanMaps();
    m_spControllerManager = 0;
    m_spProp = 0;
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::SetManager(NiControllerManager* pkCM,
    NiPhysXProp* pkProp)
{
    NIASSERT(pkCM);
    NIASSERT(pkProp);

    // Check for existing controller
    if (m_spProp)
        return false;

    m_spControllerManager = pkCM;
    m_spProp = pkProp;
    
    // Find all the objects controlled by this prop.
    FindTransformDests(pkProp);
    m_kActorActiveCount.Resize(NiTMap<NiUInt8, NiUInt8>::NextPrime(
        pkProp->GetSnapshot()->GetActorCount()));

    // Iterate over the sequence datas in the controller.
    unsigned int uiSeqDataCount = pkCM->GetSequenceDataCount();
    for (unsigned int ui = 0; ui < uiSeqDataCount; ui++)
    {
        NiSequenceData* pkAnimSeqData = pkCM->GetSequenceDataAt(ui);
        if (!pkAnimSeqData)
            continue;

        if (!AddSequence(pkAnimSeqData))
            return false;
    }
    m_kSequences.Resize(NiTMap<NiUInt8, NiUInt8>::NextPrime(uiSeqDataCount));

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::FindTransformDests(NiPhysXProp* pkProp)
{
    // Iterate over all the destinations in the PhysX prop, looking for
    // evaluators that are driving transforms.
    NiUInt32 uiNumDests = pkProp->GetDestinationsCount();
    m_kOriginalDestMap.Resize(NiTMap<NiUInt8, NiUInt8>::NextPrime(uiNumDests));
    for (NiUInt32 ui = 0; ui < uiNumDests; ui++)
    {
        NiPhysXDest* pkDest = pkProp->GetDestinationAt(ui);
            
        // Get the target object
        NiAVObject* pkTarget = 0;
        NiPhysXTransformDest* pkTransformDest = 0;
        if (NiIsKindOf(NiPhysXTransformDest, pkDest))
        {
            pkTransformDest = (NiPhysXTransformDest*)pkDest;
            pkTarget = pkTransformDest->GetTarget();
        }
        else
        {
            continue;
        }
        NIASSERT(pkTarget && pkTransformDest);
        
        // Stick it in the map
        SequenceActorData* pkData = NiNew SequenceActorData;
        pkData->SetDestination(pkTransformDest);
        m_kOriginalDestMap.SetAt(pkTarget, pkData);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::AddSequence(NiSequenceData* pkAnimSeqData)
{
    NIASSERT(pkAnimSeqData);
    NIASSERT(m_spControllerManager);
    NIASSERT(m_spProp);

    if (!m_spControllerManager->IsKnownSequenceData(pkAnimSeqData))
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::AddSequence\n"
            "The specified animation sequence is not registered"
            "with the NiControllerManager object\n");
        return false;
    }

    NiAVObjectPalette* pkObjectPalette =
        m_spControllerManager->GetObjectPalette();
    if (!pkObjectPalette)
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::AddSequence\n"
            "NiControllerManager object does not have an object palette\n");
        return false;
    }

    // Iterate through all the evaluators in the sequence
    // and try to find matching physics controlled object
    const NiFixedString& kTransformCtlrType = 
        NiAnimationConstants::GetTransformCtlrType();
    const char* pcNonAccumSuffix = NiAnimationConstants::GetNonAccumSuffix();
    unsigned int uiNumEvaluators = pkAnimSeqData->GetNumEvaluators();
    for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = pkAnimSeqData->GetEvaluatorAt(ui);
        if (!pkEvaluator)
        {
            continue;
        }

        // We're only interested in transform controllers
        const NiFixedString& kCtlrType = 
            pkEvaluator->GetIDTag().GetCtlrType();
        if (kCtlrType != kTransformCtlrType)
        {
            continue;
        }

        // Find corresponding target
        const NiFixedString& kAVObjectName = 
            pkEvaluator->GetIDTag().GetAVObjectName();
        NiAVObject* pkTarget = pkObjectPalette->GetAVObject(kAVObjectName);
        if (!pkTarget)
        {
            continue;
        }

        // Look for physics control
        SequenceActorData* pkActorData = 0;
        if (m_kOriginalDestMap.GetAt(pkTarget, pkActorData) &&
            !pkActorData->GetSource())
        {
            // If the target is the accum root and has a non-accum child,
            // then we need to attach the source to the non-accum child.
            if (pkTarget == m_spControllerManager->GetAccumRoot())
            {
                // Find the NonAccum node            
                NiNode* pkTargetNode = NiDynamicCast(NiNode, pkTarget);
                NIASSERT(pkTargetNode->GetChildCount() == 1);
                NiUInt32 uiIndex = 0;
                NiAVObject* pkChild = 0;
                do
                {
                    pkChild = pkTargetNode->GetAt(uiIndex++);
                }
                while (!pkChild);
                const char* pcName = pkChild->GetName();
                if (strstr(pcName, pcNonAccumSuffix))
                {
                    pkTarget = pkChild;
                }
            }

            // No source, make one
            pkActorData->SetSource(NiNew NiPhysXDynamicSrc(pkTarget,
                pkActorData->GetDestination()->GetActor()));
            m_spProp->AddSource(pkActorData->GetSource());
            pkActorData->GetSource()->SetActive(true);
            pkActorData->GetSource()->SetInterpolate(true);
            
            // Also set the actor to be kinematic, for now, and
            // put the actor in the map of physics active counts
            NxActor* pkActor = pkActorData->GetDestination()->GetActor();
            NIASSERT(pkActor);
            pkActor->raiseBodyFlag(NX_BF_KINEMATIC);
            m_kActorActiveCount.SetAt(pkActor, 0);
        
            // Deactivate the transform destination and remove it from the
            // prop (and scene)
            m_spProp->DeleteDestination(pkActorData->GetDestination());
            pkActorData->GetDestination()->SetActive(false);
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::MakeSequencePhysical(
    NiSequenceData* pkAnimSeqData)
{
    NIASSERT(pkAnimSeqData);
    NIASSERT(m_spControllerManager);
    NIASSERT(m_spProp);

    // Find the sequence
    SequenceData* pkSeqData;
    if (m_kSequences.GetAt(pkAnimSeqData, pkSeqData))
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::MakeSequencePhysical\n"
            "The specified animation sequence already physical\n");
        return false;
    }

    if (!m_spControllerManager->IsKnownSequenceData(pkAnimSeqData))
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::MakeSequencePhysical\n"
            "The specified animation sequence is not registered"
            "with the NiControllerManager object\n");
        return false;
    }

    NiAVObjectPalette* pkObjectPalette = 
        m_spControllerManager->GetObjectPalette();
    if (!pkObjectPalette)
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::MakeSequencePhysical\n"
            "NiControllerManager object does not have an object palette\n");
        return false;
    }

    NiPoseBinding* pkPoseBinding = m_spControllerManager->GetPoseBinding();
    if (!pkPoseBinding)
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::MakeSequencePhysical\n"
            "NiControllerManager object does not have a pose binding\n");
        return false;
    }

    // Look for the accumulation root and the non-accum node
    NiAVObject* pkAccumRoot = m_spControllerManager->GetAccumRoot();
    NiAVObject* pkNonAccumNode = 0;
    if (pkAccumRoot)
    {
        // Find the NonAccum node            
        NiNode* pkTargetNode = NiDynamicCast(NiNode, pkAccumRoot);
        NIASSERT(pkTargetNode->GetChildCount() == 1);
        NiUInt32 uiIndex = 0;
        NiAVObject* pkChild = 0;
        do
        {
            pkChild = pkTargetNode->GetAt(uiIndex++);
        }
        while (!pkChild);
        const char* pcName = pkChild->GetName();
        const char* pcNonAccumSuffix = 
            NiAnimationConstants::GetNonAccumSuffix();
        if (strstr(pcName, pcNonAccumSuffix))
        {
            pkNonAccumNode = pkChild;
        }
        else
        {
            pkAccumRoot = 0;
        }
    }
    
    // We will need to find both evaluators for accum and non-accum
    NiEvaluator* pkAccumEval = 0;
    NiEvaluator* pkNonAccumEval = 0;
    unsigned int uiAccumEvalIndex = INT_MAX;
    unsigned int uiNonAccumEvalIndex = INT_MAX;
    NiPoseBufferHandle kAccumPBHandleTranslate;
    NiPoseBufferHandle kAccumPBHandleRotate;
    NiPoseBufferHandle kNonAccumPBHandleTranslate;
    NiPoseBufferHandle kNonAccumPBHandleRotate;

    // Create the data for this sequence
    pkSeqData = NiNew SequenceData;
    ActorDataArray& kSeqActorDataArray = pkSeqData->GetActorDataArray();

    // Iterate through all the evaluators in the sequence
    // and try to find matching physics controlled object
    const NiFixedString& kTransformCtlrType = 
        NiAnimationConstants::GetTransformCtlrType();
    unsigned int uiNumEvaluators = pkAnimSeqData->GetNumEvaluators();
    for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = pkAnimSeqData->GetEvaluatorAt(ui);
        if (!pkEvaluator)
        {
            continue;
        }

        // We're only interested in transform controllers
        const NiFixedString& kCtlrType = 
            pkEvaluator->GetIDTag().GetCtlrType();
        if (kCtlrType != kTransformCtlrType)
        {
            continue;
        }

        // Find corresponding target
        const NiFixedString& kAVObjectName = 
            pkEvaluator->GetIDTag().GetAVObjectName();
        NiAVObject* pkTarget = pkObjectPalette->GetAVObject(kAVObjectName);
        if (!pkTarget)
        {
            continue;
        }

        NiPoseBinding::BindInfo* pkBindInfo = pkPoseBinding->GetBindInfo(
            pkEvaluator->GetIDTag());
        if (!pkBindInfo)
        {
            // The evaluator must not have any valid channels. We require
            // at least one valid channel to enable physics on the target.
            continue;
        }

        NiPoseBufferHandle kPBHandleTranslate = pkBindInfo->GetPBHandle(
            PBPOINT3CHANNEL);
        if (!kPBHandleTranslate.IsValid())
        {
            kPBHandleTranslate = 
                pkPoseBinding->AddBinding(pkBindInfo, PBPOINT3CHANNEL);
            NIASSERT(kPBHandleTranslate.IsValid());
        }

        NiPoseBufferHandle kPBHandleRotate = pkBindInfo->GetPBHandle(
            PBROTCHANNEL);
        if (!kPBHandleRotate.IsValid())
        {
            kPBHandleRotate = 
                pkPoseBinding->AddBinding(pkBindInfo, PBROTCHANNEL);
            NIASSERT(kPBHandleRotate.IsValid());
        }

        // Do not process accmulation related nodes until the end
        if (pkTarget == pkAccumRoot)
        {
            pkAccumEval = pkEvaluator;
            uiAccumEvalIndex = ui;
            kAccumPBHandleTranslate = kPBHandleTranslate;
            kAccumPBHandleRotate = kPBHandleRotate;
            continue;
        }
        if (pkTarget == pkNonAccumNode)
        {
            pkNonAccumEval = pkEvaluator;
            uiNonAccumEvalIndex = ui;
            kNonAccumPBHandleTranslate = kPBHandleTranslate;
            kNonAccumPBHandleRotate = kPBHandleRotate;
            continue;
        }

        // Look for physics control
        SequenceActorData *pkOriginalActorData = 0;
        if (m_kOriginalDestMap.GetAt(pkTarget, pkOriginalActorData))
        {
            NiPhysXTransformDest* pkOriginalDest = NiDynamicCast(
                NiPhysXTransformDest, pkOriginalActorData->GetDestination());

            // Create a new pose buffer destination using actor
            // information from the old destination.
            NiPhysXRigidBodyDestPtr spNewDest = NiNew NiPhysXPoseBufferDest(
                pkOriginalDest->GetActor(), pkOriginalDest->GetActorParent(), 
                pkTarget->GetParent(), pkSeqData, ui, kPBHandleTranslate, 
                kPBHandleRotate);

            // Disable the old destination and add the new one. They are
            // inactive right now.
            spNewDest->SetActive(false);
            m_spProp->AddDestination(spNewDest);
            
            // Add this source to the set for this sequence.
            SequenceActorData* pkNewActorData = NiNew SequenceActorData;
            pkNewActorData->SetSource(pkOriginalActorData->GetSource());
            pkNewActorData->SetDestination(spNewDest);
            kSeqActorDataArray.Add(pkNewActorData);
        }
    }
    
    // Add pose buffer targets to match the new pose buffer bindings.
    NIASSERT(m_spControllerManager->GetMultiTargetPoseHandler());
    m_spControllerManager->GetMultiTargetPoseHandler()->
        AddNewTargetsFromPoseBinding();

    // Deal with accumulation nodes
    if (pkAccumEval && pkNonAccumEval)
    {
        // Look for physics control
        SequenceActorData *pkOriginalActorData = 0;
        if (m_kOriginalDestMap.GetAt(pkAccumRoot, pkOriginalActorData))
        {
            NiPhysXTransformDest* pkOriginalDest = NiDynamicCast(
                NiPhysXTransformDest, pkOriginalActorData->GetDestination());

            NiPhysXAccumRigidBodyDest* pkNewAccumDest =
                NiNew NiPhysXAccumPoseBufferDest(
                pkOriginalDest->GetActor(),
                pkOriginalDest->GetActorParent(),
                pkAccumRoot->GetParent(),
                pkSeqData, 
                uiAccumEvalIndex, 
                kAccumPBHandleTranslate, 
                kAccumPBHandleRotate, 
                uiNonAccumEvalIndex, 
                kNonAccumPBHandleTranslate, 
                kNonAccumPBHandleRotate);

            // Set all the accum flags
            bool bX, bY, bZ;
            pkAnimSeqData->GetAccumTranslation(bX, bY, bZ);
            pkNewAccumDest->SetAccumTranslation(bX, bY, bZ);
            pkAnimSeqData->GetAccumRotation(bX, bY, bZ);
            pkNewAccumDest->SetAccumRotation(bX, bY, bZ);
            unsigned char ucAxis;
            bool bNeg;
            pkAnimSeqData->GetAccumFrontFacing(ucAxis, bNeg);
            pkNewAccumDest->SetAccumFrontFacing(ucAxis, bNeg);
            
            // Disable the old destination and add the new one. They are
            // inactive right now.
            pkNewAccumDest->SetActive(false);
            pkNewAccumDest->SetInterpolate(true);
            m_spProp->AddDestination(pkNewAccumDest);

            SequenceActorData* pkNewActorData = NiNew SequenceActorData;
            pkNewActorData->SetSource(pkOriginalActorData->GetSource());
            pkNewActorData->SetDestination(pkNewAccumDest);
            kSeqActorDataArray.Add(pkNewActorData);
            pkSeqData->SetAccumDestination(pkNewAccumDest);
        }
    }
    else if (pkAccumEval || pkNonAccumEval)
    {
        NIASSERT(false &&
            "NiPhysXSequenceManager::MakeSequencePhysical\n"
            "Found only one of NonAccum evaluator and Accum evaluator\n");
    }

    // Add to physics sequences
    pkSeqData->SetActiveAnimSequence(NULL);

    pkAnimSeqData->AddDefaultActivationCallback(this, NULL);
    
    m_kSequences.SetAt(pkAnimSeqData, pkSeqData);

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::RevertPhysicalSequence(
    NiSequenceData* pkAnimSeqData)
{
    NIASSERT(pkAnimSeqData);
    NIASSERT(m_spProp);

    // Find the sequence
    SequenceData* pkSeqData;
    if (!m_kSequences.GetAt(pkAnimSeqData, pkSeqData))
    {
        return false;
    }

    // Remove the activation callbacks
    pkAnimSeqData->RemoveDefaultActivationCallback(this, NULL);
    NiControllerSequence* pkActiveAnimSequence = 
        pkSeqData->GetActiveAnimSequence();
    if (pkActiveAnimSequence)
    {
        pkActiveAnimSequence->RemoveActivationCallback(this);
    }

    // Deactivate physics if necessary
    SetActivity(pkSeqData, NULL);
    NIASSERT(!pkSeqData->IsActive());

    // Remove the destinations from the prop
    ActorDataArray& kSeqActorDataArray = pkSeqData->GetActorDataArray();
    NiUInt32 uiCount = kSeqActorDataArray.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        SequenceActorData* pkActorData = kSeqActorDataArray.GetAt(ui);
        m_spProp->DeleteDestination(pkActorData->GetDestination());
    }

    // Remove the sequence
    m_kSequences.RemoveAt(pkAnimSeqData);
    NiDelete pkSeqData;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::ActivationChanged(NiControllerSequence* pkAnimSeq,
    NiAnimState eState)
{
    // Find the sequence
    SequenceData* pkSeqData = 0;
    NIVERIFY(m_kSequences.GetAt(pkAnimSeq->GetSequenceData(), pkSeqData));

    if (eState == INACTIVE || eState == TRANSSOURCE)
    {
        // Inactive physics sequence. If we're the source of a transition
        // we are inactive because transitions out of physics are
        // intended to return to animation and may be long. We do not want
        // the physics to diverge from the animation while the transition
        // is going on. Also, blend transitions by definition should blend
        // from a fixed frame.
        SetActivity(pkSeqData, NULL);
    }
    else if (eState == TRANSDEST || eState == ANIMATING)
    {
        // If we are the destination of a transition then we always
        // play physics.
        NIASSERT(pkAnimSeq);
        SetActivity(pkSeqData, pkAnimSeq);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXSequenceManager::SetActivity(SequenceData* pkSeqData, 
    NiControllerSequence* pkActiveAnimSeq)
{
    bool bActive = (pkActiveAnimSeq != NULL);
    if (pkSeqData->GetActiveAnimSequence() == pkActiveAnimSeq)
        return false;

    pkSeqData->SetActiveAnimSequence(pkActiveAnimSeq);

    // Clear accum transform data in newly activated sequence.
    if (pkActiveAnimSeq)
    {
        pkActiveAnimSeq->ClearAccumTransformData();
    }

    // Get some scene data to enable actor updating.
    float fT = -1.0f;
    NiTransform kRootTransform;
    float fScalePToW = 1.0f;
    if (bActive && m_spProp)
    {
        NiPhysXScene* pkScene = m_spProp->GetScene();
        if (pkScene)
        {
            fT = pkScene->GetPrevFetchTime();
            kRootTransform = pkScene->GetSceneXform();
            fScalePToW = pkScene->GetScaleFactor();
        }
        else
        {
            kRootTransform.MakeIdentity();
        }
    }

    ActorDataArray& kSeqActorDataArray = pkSeqData->GetActorDataArray();
    NiUInt32 uiCount = kSeqActorDataArray.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        SequenceActorData* pkActorData = kSeqActorDataArray.GetAt(ui);
        pkActorData->GetDestination()->SetActive(bActive);
        if (bActive && fT >= 0.0f)
        {
            // We need to update actors here to flush new state from the
            // kinematically controlled actors into the rigid body
            // destinations. 
            pkActorData->GetDestination()->UpdateFromActors(
                fT, kRootTransform, fScalePToW, true);
        }
        
        NxActor* pkActor = pkActorData->GetDestination()->GetActor();
        NIASSERT(pkActor);
        NiUInt16 usCount = 0;
        NIVERIFY(m_kActorActiveCount.GetAt(pkActor, usCount));
        if (bActive)
        {
            if (usCount == 0)
            {
                pkActor->wakeUp();
                pkActor->clearBodyFlag(NX_BF_KINEMATIC);
                pkActorData->GetSource()->SetActive(false);
            }
            usCount++;
        }
        else
        {
            NIASSERT(usCount - 1 < usCount); // unsigned count > 0
            usCount--;
            if (usCount == 0)
            {
                pkActor->raiseBodyFlag(NX_BF_KINEMATIC);
                pkActorData->GetSource()->SetActive(true);
            }
        }
        m_kActorActiveCount.SetAt(pkActor, usCount);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::CleanMaps()
{
    NiTMapIterator kIter = m_kSequences.GetFirstPos();
    while (kIter)
    {
        NiSequenceData* pkAnimSeqData;
        SequenceData* pkData;
        m_kSequences.GetNext(kIter, pkAnimSeqData, pkData);
        if (pkAnimSeqData)
        {
            pkAnimSeqData->RemoveDefaultActivationCallback(this, NULL);
        }
        if (pkData)
        {
            NiDelete pkData;
        }
    }
    m_kSequences.RemoveAll();

    kIter = m_kOriginalDestMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkObj;
        SequenceActorData* pkData;
        m_kOriginalDestMap.GetNext(kIter, pkObj, pkData);
        if (pkData)
        {
            NiDelete pkData;
        }
    }
    m_kOriginalDestMap.RemoveAll();

    m_kActorActiveCount.RemoveAll();
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceActorData::SequenceActorData()
{
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceActorData::~SequenceActorData()
{
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceData::SequenceData()
{
    m_pkAccumDest = 0;
    m_pkActiveAnimSeq = 0;
    m_uiTotalPoseBufferItems = 0;
}
//---------------------------------------------------------------------------
NiPhysXSequenceManager::SequenceData::~SequenceData()
{
    m_pkAccumDest = 0;
    m_pkActiveAnimSeq = 0;
    m_uiTotalPoseBufferItems = 0;

    NiUInt32 uiCount = m_kActorData.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
        NiDelete m_kActorData.GetAt(ui);
    m_kActorData.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXSequenceManager::SequenceData::UpdateDestPoseBufferDataPointers()
{
    NIASSERT(m_pkActiveAnimSeq);
    NIASSERT(m_pkActiveAnimSeq->GetPoseBuffer());

    const NiPoseBuffer* pkPoseBuffer = m_pkActiveAnimSeq->GetPoseBuffer();
    if (m_uiTotalPoseBufferItems != pkPoseBuffer->GetNumTotalItems())
    {
        NiUInt32 uiCount = m_kActorData.GetSize();
        for (NiUInt32 ui = 0; ui < uiCount; ui++)
        {
            SequenceActorData* pkActorData = m_kActorData.GetAt(ui);
            NiPhysXRigidBodyDest* pkDest = pkActorData->GetDestination();
            if (pkDest == m_pkAccumDest)
            {
                ((NiPhysXAccumPoseBufferDest*)pkDest)->
                    UpdatePoseBufferDataPointers(pkPoseBuffer);
            }
            else
            {
                ((NiPhysXPoseBufferDest*)pkDest)->
                    UpdatePoseBufferDataPointers(pkPoseBuffer);
            }
        }

        m_uiTotalPoseBufferItems = pkPoseBuffer->GetNumTotalItems();
    }
}
//---------------------------------------------------------------------------

