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
#include "NiAnimationPCH.h"

#include "NiControllerManager.h"
#include "NiAnimationConstants.h"
#include <NiCloningProcess.h>
#include <NiString.h>
#include <NiNode.h>
#include <NiSystem.h> 
#include <NiDefaultAVObjectPalette.h>
#include "NiSkinningLODController.h"


NiImplementRTTI(NiControllerManager, NiTimeController);

const float NiControllerManager::SEQUENCE_DATA_FREQUENCY = 
    NiControllerSequence::SEQUENCE_DATA_FREQUENCY;


//---------------------------------------------------------------------------
NiControllerManager* NiControllerManager::FindControllerManager(
    NiAVObject* pkObject)
{
    NIASSERT(pkObject);

    NiControllerManager* pkManager = NiGetController(NiControllerManager,
        pkObject);
    if (pkManager)
    {
        return pkManager;
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                pkManager = FindControllerManager(pkChild);
                if (pkManager)
                {
                    return pkManager;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiAVObject* NiControllerManager::FindAccumRoot(NiAVObject* pkObject)
{
    NIASSERT(pkObject);

    // Check for a child node that is identically named as its parent,
    // but augmented with the " NonAccum" suffix.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        unsigned int uiNumChildren = pkNode->GetArrayCount();
        const NiFixedString& kName = pkObject->GetName();
        if (kName.Exists())
        {
            int iNonAccumLength = 
                NiAnimationConstants::GetNonAccumSuffixLength();
            const char* pcNonAccumSuffix = 
                NiAnimationConstants::GetNonAccumSuffix();
            for (unsigned int ui = 0; ui < uiNumChildren; ui++)
            {
                NiAVObject* pkChild = pkNode->GetAt(ui);
                if (pkChild)
                {
                    const NiFixedString& kChildName = pkChild->GetName();
                    int length = (int)kChildName.GetLength() - iNonAccumLength;
                    if (length >= 0)
                    {
                        const char* pcSuffix = &kChildName[length];
                        if (strcmp(pcSuffix, pcNonAccumSuffix) == 0)
                        {
                            // Found the " NonAccum" suffix.
                            // Compare child name with parent.
                            if (strncmp(kChildName, kName, length) == 0)
                            {
                                return pkObject;
                            }
                        }
                    }
                }
            }
        }

        for (unsigned int ui = 0; ui < uiNumChildren; ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                pkChild = FindAccumRoot(pkChild);
                if (pkChild)
                {
                    return pkChild;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiControllerManager::NiControllerManager(NiAVObject* pkTarget,
    bool bCumulativeAnimations, NiPoseBinding* pkPoseBinding,
    bool bMoveAccumTransformToChild) :
    m_kSeqDataMap(37, false), m_bCumulative(false),
    m_spObjectPalette(NULL), m_spPoseBinding(NULL), m_spPoseBlender(NULL),
    m_spMTPoseHandler(NULL), m_pkLODController(NULL), 
    m_usMaxEvaluators(1), m_usMaxUniqueBasisDatas(0), 
    m_uiMaxScratchPadBufferSize(4),
    m_usMaxRecycledSequences(USHRT_MAX), m_usUnusedPadding(0),
    m_uiActivationID(0)
{
    Init(pkTarget, bCumulativeAnimations, pkPoseBinding, 
        bMoveAccumTransformToChild);
}
//---------------------------------------------------------------------------
NiControllerManager::NiControllerManager() : 
    m_kSeqDataMap(37, false), m_bCumulative(false),
    m_spObjectPalette(NULL), m_spPoseBinding(NULL), m_spPoseBlender(NULL), 
    m_spMTPoseHandler(NULL), m_pkLODController(NULL), 
    m_usMaxEvaluators(1), m_usMaxUniqueBasisDatas(0), 
    m_uiMaxScratchPadBufferSize(4),
    m_usMaxRecycledSequences(USHRT_MAX), m_usUnusedPadding(0),
    m_uiActivationID(0)
{
}
//---------------------------------------------------------------------------
NiControllerManager::~NiControllerManager()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiControllerManager::Update(float fTime)
{
    NIMETRICS_ANIMATION_SCOPETIMER(CM_UPDATE_TIME);

    unsigned int uiMetricsUpdatedSequences = 0;

    if (!GetActive())
    {
        return;
    }

    // Update animation sequences. New sequences may be added while looping
    // as a result of the callback functions.
    int iLOD = m_pkLODController ? m_pkLODController->GetBoneLOD() : 0;
    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            pkSequence->Update(fTime, iLOD, true);
            uiMetricsUpdatedSequences++;
        }
    }

    unsigned int uiNumSequences = m_kSequenceArray.GetEffectiveSize();
    if (uiNumSequences == 0)
    {
        return;
    }

    // Move sequences to the front of the array. Fill in any empty gaps.
    unsigned int uiArraySize = m_kSequenceArray.GetSize();
    unsigned int uiIndex = 0;
    for (unsigned int ui = 0; ui < uiArraySize; ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            if (uiIndex != ui)
            {
                m_kSequenceArray.SetAt(uiIndex, pkSequence);
                m_kSequenceArray.SetAt(ui, NULL);
            }
            uiIndex++;
        }
    }
    NIASSERT(uiIndex == uiNumSequences);

    // Sort the sequences by active/inactive, priority, spinner values,
    // and weight so sequences of greater importance reside at the
    // head of the list. This ordering allows non-contributing sequences 
    // at the tail of the list to be readily excluded from further 
    // consideration.
    bool bSwapped;
    do
    {
        bSwapped = false;
        NiControllerSequence* pkSequence0 = m_kSequenceArray.GetAt(0);
        for (unsigned int ui = 1; ui < uiNumSequences; ui++)
        {
            NiControllerSequence* pkSequence1 = m_kSequenceArray.GetAt(ui);
            if (pkSequence1->IsMoreImportantThan(pkSequence0))
            {
                // Temporarily increment ref count to prevent deletion.
                pkSequence0->IncRefCount();
                m_kSequenceArray.SetAt(ui - 1, pkSequence1);
                m_kSequenceArray.SetAt(ui, pkSequence0);
                pkSequence0->DecRefCount();
                bSwapped = true;
            }
            else
            {
                pkSequence0 = pkSequence1;
            }
        }
    } while (bSwapped);

    // The following code relies upon the sorted order of sequences.

    // Determine which sequences have a possibility of contributing to 
    // the final blended result.
    float fMaxEaseSpinner = 0.0f;
    int iLastFullBodyPriority = -INT_MAX;
    unsigned int uiNumFullBodyPriorities = 0;
    unsigned int uiNumContributingSequences = 0;
    while (uiNumContributingSequences < uiNumSequences)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(
            uiNumContributingSequences);
        if (pkSequence->GetState() == INACTIVE)
        {
            break;
        }

        int iPriority = pkSequence->GetPriority();
        if (iPriority < iLastFullBodyPriority)
        {
            if (fMaxEaseSpinner >= 1.0f || uiNumFullBodyPriorities >= 2)
            {
                break;
            }
        }

        uiNumContributingSequences++;

        NIASSERT(pkSequence->GetPoseBuffer());
        NIASSERT(pkSequence->GetPoseBuffer()->GetPoseBinding());
        NIASSERT(pkSequence->GetPoseBuffer()->GetPoseBinding() == 
            m_spPoseBinding);
        if (pkSequence->GetPoseBuffer()->IsFullBodyPose())
        {
            if (iPriority != iLastFullBodyPriority)
            {
                if (iLastFullBodyPriority == -INT_MAX)
                {
                    fMaxEaseSpinner = pkSequence->GetEaseSpinner();
                }
                iLastFullBodyPriority = pkSequence->GetPriority();
                uiNumFullBodyPriorities++;
            }
        }
    }

    // Check if there's more than one contributing sequence.
    if (uiNumContributingSequences > 1)
    {
        // Compute the final blended result.
        NiControllerSequence* pkSoleSequence;
        NiPoseBuffer* pkFinalPoseBuffer;
        if (m_spPoseBlender->Update(uiNumContributingSequences, iLOD, 
            pkFinalPoseBuffer, pkSoleSequence))
        {
            // Push the final blended result into the scene graph.
            m_spMTPoseHandler->Update(fTime, iLOD, pkFinalPoseBuffer, 
                pkSoleSequence);
        }
    }
    else if (uiNumContributingSequences == 1)
    {
        // Push the contributing pose buffer into the scene graph.
        // By definition, the finalized weight of this sequence is 1.0.
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(0);
        NIASSERT(pkSequence->GetPoseBuffer());
        m_spMTPoseHandler->Update(fTime, iLOD, 
            pkSequence->GetPoseBuffer(), pkSequence);
    }

    // Remove inactive sequences that don't have an outstanding ref count.
    RemoveAllInactiveSequences(false);

    NIMETRICS_ANIMATION_ADDVALUE(UPDATED_SEQUENCES, uiMetricsUpdatedSequences);
}
//---------------------------------------------------------------------------
NiSequenceData* NiControllerManager::AddSequenceDataFromFile(
    const char* pcFilename, unsigned int uiIndex)
{
    // Load the sequence data from file.
    NiSequenceDataPtr spSeqData =
        NiSequenceData::CreateSequenceDataFromFile(pcFilename, uiIndex);
    if (!spSeqData)
    {
        return NULL;
    }

    // Add the sequence data to the manager.
    if (!AddSequenceData(spSeqData))
    {
        return NULL;
    }

    return spSeqData;
}
//---------------------------------------------------------------------------
NiSequenceData* NiControllerManager::AddSequenceDataFromFile(
    const NiStream& kStream, unsigned int uiIndex)
{
    // Load the sequence data from file.
    NiSequenceDataPtr spSeqData =
        NiSequenceData::CreateSequenceDataFromFile(kStream, uiIndex);
    if (!spSeqData)
    {
        return NULL;
    }

    // Add the sequence data to the manager.
    if (!AddSequenceData(spSeqData))
    {
        return NULL;
    }

    return spSeqData;
}
//---------------------------------------------------------------------------
bool NiControllerManager::AddAllSequenceDatasFromFile(const char* pcFilename)
{
    // Load the sequence datas from file.
    NiSequenceDataPointerArray kSeqDatas;
    if (!NiSequenceData::CreateAllSequenceDatasFromFile(
        pcFilename, kSeqDatas))
    {
        return false;
    }

    // Add each sequence data to the manager.
    bool bSuccess = true;
    m_kSeqDataArray.SetSize(m_kSeqDataArray.GetSize() + kSeqDatas.GetSize());
    for (unsigned int ui = 0; ui < kSeqDatas.GetSize(); ui++)
    {
        NiSequenceData* pkSeqData = kSeqDatas.GetAt(ui);
        if (pkSeqData)
        {
            // Add the sequence data to the manager.
            bSuccess = bSuccess && AddSequenceData(pkSeqData);
        }
    }
    kSeqDatas.RemoveAll();

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiControllerManager::AddSequenceData(NiSequenceData* pkSeqData)
{
    NIASSERT(pkSeqData);

    m_kSeqDataArray.AddFirstEmpty(pkSeqData);
    m_kSeqDataMap.SetAt(pkSeqData->GetName(), pkSeqData);

    // Add new pose buffer bindings required by the sequence data.
    NIASSERT(m_spPoseBinding);
    m_spPoseBinding->AddBindings(pkSeqData, this);

    // Set the accum root name in the sequence data, if not set.
    if (!pkSeqData->GetAccumRootName().Exists() && 
        m_spPoseBinding->GetAccumRootName().Exists())
    {
        pkSeqData->SetAccumRootName(m_spPoseBinding->GetAccumRootName());
    }

    // Add pose buffer targets to match the new pose buffer bindings.
    NIASSERT(m_spMTPoseHandler);
    m_spMTPoseHandler->AddNewTargetsFromPoseBinding();

    // Update maximum values for recycled sequences.
    unsigned short usNumEvaluators = 
        (unsigned short)pkSeqData->GetNumEvaluators();
    if (usNumEvaluators > m_usMaxEvaluators)
    {
        m_usMaxEvaluators = usNumEvaluators;
    }

    unsigned int uiNumUniqueBasisDatas;
    unsigned int uiScratchPadSize = pkSeqData->GetScratchPadBufferSize(
        m_bCumulative, m_spObjectPalette, uiNumUniqueBasisDatas);
    if (uiScratchPadSize > m_uiMaxScratchPadBufferSize)
    {
        m_uiMaxScratchPadBufferSize = uiScratchPadSize;
    }

    if (uiNumUniqueBasisDatas > m_usMaxUniqueBasisDatas)
    {
        m_usMaxUniqueBasisDatas = (unsigned short)uiNumUniqueBasisDatas;
    }

    return true;
}
//---------------------------------------------------------------------------
NiSequenceData* NiControllerManager::AddPoseSequenceData(
    const NiSequenceData* pkSeqDataToSynchronize, bool bTempPose, 
    const NiFixedString& kName)
{
    NIASSERT(pkSeqDataToSynchronize);
    NIASSERT(m_spMTPoseHandler);

    NiSequenceData* pkPoseSeqData;
    unsigned int uiNumEvaluators = pkSeqDataToSynchronize->GetNumEvaluators();
    if (kName.Exists())
    {
        pkPoseSeqData = NiNew NiSequenceData(kName, uiNumEvaluators);
    }
    else
    {
        NiString kPoseName("__PoseFrom");
        kPoseName.Concatenate(pkSeqDataToSynchronize->GetName());
        kPoseName.Concatenate("__");
        NiSequenceData* pkExistingSeqData;
        while (m_kSeqDataMap.GetAt(kPoseName, pkExistingSeqData))
        {
            kPoseName.Concatenate("_");
        }
        pkPoseSeqData = NiNew NiSequenceData((const char*)kPoseName, 
            uiNumEvaluators);
    }

    pkPoseSeqData->SetDuration(0.0f);
    if (bTempPose)
    {
        pkPoseSeqData->SetTempPose();
    }

    // Add to sequence data list.
    m_kSeqDataArray.AddFirstEmpty(pkPoseSeqData);
    m_kSeqDataMap.SetAt(pkPoseSeqData->GetName(), pkPoseSeqData);

    // Add evaluators to the pose sequence data and 
    // set their constant values from the scene graph.
    m_spMTPoseHandler->FillPoseSequenceData(pkSeqDataToSynchronize, 
        pkPoseSeqData);

    // Update maximum values for recycled sequences.
    unsigned short usNumEvaluators = 
        (unsigned short)pkPoseSeqData->GetNumEvaluators();
    if (usNumEvaluators > m_usMaxEvaluators)
    {
        m_usMaxEvaluators = usNumEvaluators;
    }

    unsigned int uiNumUniqueBasisDatas;
    unsigned int uiScratchPadSize = pkPoseSeqData->GetScratchPadBufferSize(
        m_bCumulative, m_spObjectPalette, uiNumUniqueBasisDatas);
    if (uiScratchPadSize > m_uiMaxScratchPadBufferSize)
    {
        m_uiMaxScratchPadBufferSize = uiScratchPadSize;
    }

    // No need to update m_usMaxUniqueBasisDatas.
    NIASSERT(uiNumUniqueBasisDatas <= m_usMaxUniqueBasisDatas);

    return pkPoseSeqData;
}
//---------------------------------------------------------------------------
NiSequenceDataPtr NiControllerManager::RemoveSequenceData(
    const NiFixedString& kName)
{
    NIASSERT(kName.Exists());

    NiSequenceData* pkExistingSeqData;
    if (m_kSeqDataMap.GetAt(kName, pkExistingSeqData))
    {
        return RemoveSequenceData(pkExistingSeqData);
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiSequenceDataPtr NiControllerManager::RemoveSequenceData(
    NiSequenceData* pkSeqData)
{
    NIASSERT(pkSeqData);
    NIASSERT(GetSequenceBySequenceData(pkSeqData) == NULL);

    // Remove all default callbacks explicitly assigned to this manager.
    pkSeqData->RemoveDefaultActivationCallback(NULL, this);

    NiSequenceDataPtr spSeqData = pkSeqData;
    if (m_kSeqDataArray.Remove(pkSeqData) != (unsigned int)~0)
    {
        const NiFixedString& kName = spSeqData->GetName();
        if (kName.Exists())
        {
            m_kSeqDataMap.RemoveAt(kName);
        }
        return spSeqData;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiControllerManager::RemoveAllSequenceDatas()
{
    // Remove all default callbacks explicitly assigned to this manager.
    unsigned int uiNumSeqDatas = m_kSeqDataArray.GetSize();
    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        NiSequenceData* pkSeqData = m_kSeqDataArray.GetAt(ui);
        if (pkSeqData)
        {
            pkSeqData->RemoveDefaultActivationCallback(NULL, this);
        }
    }

    m_kSeqDataArray.RemoveAll();
    m_kSeqDataMap.RemoveAll();
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::CrossFade(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSeqDataName, float fDuration, int iPriority, 
    float fDestWeight, const NiFixedString& kDestTimeSyncSeqName, 
    float fDestFrequency)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSeqDataName.Exists());

    NiSequenceData* pkDestSeqData = GetSequenceDataByName(kDestSeqDataName);
    if (pkDestSeqData)
    {
        NiControllerSequence* pkSourceSequence = 
            GetSequenceByName(kSourceSequenceName);
        if (pkSourceSequence)
        {
            NiControllerSequence* pkDestTimeSyncSeq = NULL;
            if (kDestTimeSyncSeqName.Exists())
            {
                pkDestTimeSyncSeq = GetSequenceByName(kDestTimeSyncSeqName);
                if (!pkDestTimeSyncSeq)
                {
                    return NULL;
                }
            }
            return CrossFade(pkSourceSequence, pkDestSeqData, fDuration, 
                iPriority, fDestWeight, pkDestTimeSyncSeq, fDestFrequency);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::CrossFade(
    NiControllerSequence* pkSourceSequence,
    NiSequenceData* pkDestSeqData, float fDuration, int iPriority, 
    float fDestWeight, NiControllerSequence* pkDestTimeSyncSeq, 
    float fDestFrequency)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this);
    NIASSERT(pkDestSeqData);

    if (pkSourceSequence->GetState() == INACTIVE)
    {
        return NULL;
    }

    // Activate destination sequence.
    NiControllerSequence* pkDestSequence = ActivateSequence(
        pkDestSeqData, iPriority, fDestWeight, fDuration,
        pkDestTimeSyncSeq, fDestFrequency, 0.0f, true, false);
    if (!pkDestSequence)
    {
        return NULL;
    }

    // Temporarily deactivate source sequence so we can reactivate
    // it with a different state. Don't process callbacks.
    pkSourceSequence->Deactivate(0.0f, false, false, false);

    // Reactivate source sequence with new state. Don't process callbacks.
    if (!pkSourceSequence->Reactivate(iPriority, 
        pkSourceSequence->GetWeight(), 0.0f, 
        pkSourceSequence->GetTimeSyncSequence(), 
        NiControllerSequence::CURRENT_FRAME, false, false))
    {
        pkDestSequence->Deactivate(0.0f);
        return NULL;
    }

    // Ease out source sequence. Process callbacks.
    pkSourceSequence->Deactivate(fDuration, true, false, true);

    return pkDestSequence;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::BlendFromSequence(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSeqDataName, float fDuration,
    const NiFixedString& kDestTextKey, int iPriority, float fSourceWeight,
    float fDestWeight, const NiFixedString& kDestTimeSyncSeqName, 
    float fDestFrequency)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSeqDataName.Exists());

    NiSequenceData* pkDestSeqData = GetSequenceDataByName(kDestSeqDataName);
    if (pkDestSeqData)
    {
        NiControllerSequence* pkSourceSequence = 
            GetSequenceByName(kSourceSequenceName);
        if (pkSourceSequence && pkSourceSequence->GetState() != INACTIVE)
        {
            NiControllerSequence* pkDestTimeSyncSeq = NULL;
            if (kDestTimeSyncSeqName.Exists())
            {
                pkDestTimeSyncSeq = GetSequenceByName(kDestTimeSyncSeqName);
                if (!pkDestTimeSyncSeq)
                {
                    return NULL;
                }
            }
            return BlendFromSequence(pkSourceSequence, pkDestSeqData, 
                fDuration, kDestTextKey, iPriority, fSourceWeight, 
                fDestWeight, pkDestTimeSyncSeq, fDestFrequency);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::BlendFromSequence(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSeqDataName, float fDuration, 
    float fDestFrame, int iPriority, float fSourceWeight, 
    float fDestWeight, const NiFixedString& kDestTimeSyncSeqName, 
    float fDestFrequency)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSeqDataName.Exists());

    NiSequenceData* pkDestSeqData = GetSequenceDataByName(kDestSeqDataName);
    if (pkDestSeqData)
    {
        NiControllerSequence* pkSourceSequence = 
            GetSequenceByName(kSourceSequenceName);
        if (pkSourceSequence && pkSourceSequence->GetState() != INACTIVE)
        {
            NiControllerSequence* pkDestTimeSyncSeq = NULL;
            if (kDestTimeSyncSeqName.Exists())
            {
                pkDestTimeSyncSeq = GetSequenceByName(kDestTimeSyncSeqName);
                if (!pkDestTimeSyncSeq)
                {
                    return NULL;
                }
            }
            return BlendFromSequence(pkSourceSequence, pkDestSeqData, 
                fDuration, fDestFrame, iPriority, fSourceWeight, 
                fDestWeight, pkDestTimeSyncSeq, fDestFrequency);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopBlendFromSequence(
    const NiFixedString& kSourceSequenceName, 
    const NiFixedString& kDestSequenceName,
    float fEaseOutTime)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    NiControllerSequence* pkDestSequence = 
        GetSequenceByName(kDestSequenceName);
    if (pkDestSequence)
    {
        NiControllerSequence* pkSourceSequence = 
            GetSequenceByName(kSourceSequenceName);
        if (pkSourceSequence)
        {
            return StopBlendFromSequence(pkSourceSequence, 
                pkDestSequence, fEaseOutTime);
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopBlendFromSequence(
    NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fEaseOutTime)
{
    NIASSERT(pkSourceSequence && pkDestSequence);

    if (pkSourceSequence->GetOwner() == this && 
        pkSourceSequence->GetState() != INACTIVE &&
        pkDestSequence->GetOwner() == this &&
        pkDestSequence->GetState() != INACTIVE)
    {
        pkSourceSequence->Deactivate(fEaseOutTime, true, 
            pkSourceSequence->GetState() == TRANSSOURCE, true);
        pkDestSequence->Deactivate(fEaseOutTime, true, 
            pkDestSequence->GetState() == TRANSDEST, true);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::Morph(
    const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSeqDataName, float fDuration, int iPriority,
    float fSourceWeight, float fDestWeight, float fDestFrequency)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSeqDataName.Exists());

    NiSequenceData* pkDestSeqData = GetSequenceDataByName(kDestSeqDataName);
    if (pkDestSeqData)
    {
        NiControllerSequence* pkSourceSequence = 
            GetSequenceByName(kSourceSequenceName);
        if (pkSourceSequence)
        {
            return Morph(pkSourceSequence, pkDestSeqData, 
                fDuration, iPriority, fSourceWeight, fDestWeight, 
                fDestFrequency);
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiControllerSequence* NiControllerManager::Morph(
    NiControllerSequence* pkSourceSequence,
    NiSequenceData* pkDestSeqData, float fDuration, int iPriority,
    float fSourceWeight, float fDestWeight, float fDestFrequency)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this);
    NIASSERT(pkDestSeqData);

    if (pkSourceSequence->GetState() == INACTIVE)
    {
        return NULL;
    }

    return pkSourceSequence->StartMorph(pkDestSeqData, fDuration, 
        iPriority, fSourceWeight, fDestWeight, fDestFrequency);
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopMorph(const NiFixedString& kSourceSequenceName,
    const NiFixedString& kDestSequenceName, float fEaseOutTime)
{
    NIASSERT(kSourceSequenceName.Exists() && kDestSequenceName.Exists());

    NiControllerSequence* pkDestSequence = 
        GetSequenceByName(kDestSequenceName);
    if (pkDestSequence)
    {
        NiControllerSequence* pkSourceSequence = 
            GetSequenceByName(kSourceSequenceName);
        if (pkSourceSequence)
        {
            return StopMorph(pkSourceSequence, pkDestSequence, fEaseOutTime);
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::StopMorph(NiControllerSequence* pkSourceSequence,
    NiControllerSequence* pkDestSequence, float fEaseOutTime)
{
    NIASSERT(pkSourceSequence && pkDestSequence);

    if (pkSourceSequence->GetOwner() == this && 
        pkSourceSequence->GetState() != INACTIVE &&
        pkDestSequence->GetOwner() == this &&
        pkDestSequence->GetState() != INACTIVE)
    {
        pkSourceSequence->Deactivate(fEaseOutTime, true, false, true);
        pkDestSequence->Deactivate(fEaseOutTime, true, false, true);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(
    const NiFixedString& kDestSeqDataName,
    const NiFixedString& kDestTextKey, float fDuration, 
    int iPriority, const NiFixedString& kSeqDataToSynchronize,
    NiControllerSequence*& pkPoseSequence, 
    NiControllerSequence*& pkDestSequence)
{
    // This function is not as efficient as BlendFromSequence in terms 
    // of memory and performance. Consequently, it is recommended that 
    // BlendFromPose be used only when BlendFromSequence is not applicable.

    NIASSERT(kDestSeqDataName.Exists() && kSeqDataToSynchronize.Exists());

    NiSequenceData* pkDestSeqData = GetSequenceDataByName(kDestSeqDataName);
    if (pkDestSeqData)
    {
        NiSequenceData* pkSeqDataToSynchronize = GetSequenceDataByName(
            kSeqDataToSynchronize);
        if (pkSeqDataToSynchronize)
        {
            return BlendFromPose(pkDestSeqData, kDestTextKey, fDuration, 
                iPriority, pkSeqDataToSynchronize, 
                pkPoseSequence, pkDestSequence);
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(
    const NiFixedString& kDestSeqDataName,
    float fDestFrame, float fDuration, int iPriority,
    const NiFixedString& kSeqDataToSynchronize,
    NiControllerSequence*& pkPoseSequence, 
    NiControllerSequence*& pkDestSequence)
{
    // This function is not as efficient as BlendFromSequence in terms 
    // of memory and performance. Consequently, it is recommended that 
    // BlendFromPose be used only when BlendFromSequence is not applicable.

    NIASSERT(kDestSeqDataName.Exists() && kSeqDataToSynchronize.Exists());

    NiSequenceData* pkDestSeqData = GetSequenceDataByName(kDestSeqDataName);
    if (pkDestSeqData)
    {
        NiSequenceData* pkSeqDataToSynchronize = GetSequenceDataByName(
            kSeqDataToSynchronize);
        if (pkSeqDataToSynchronize)
        {
            return BlendFromPose(pkDestSeqData, fDestFrame, fDuration, 
                iPriority, pkSeqDataToSynchronize, 
                pkPoseSequence, pkDestSequence);
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(NiSequenceData* pkDestSeqData,
    const NiFixedString& kDestTextKey, float fDuration, int iPriority,
    NiSequenceData* pkSeqDataToSynchronize,
    NiControllerSequence*& pkPoseSequence, 
    NiControllerSequence*& pkDestSequence)
{
    // This function is not as efficient as BlendFromSequence in terms 
    // of memory and performance. Consequently, it is recommended that 
    // BlendFromPose be used only when BlendFromSequence is not applicable.

    NIASSERT(pkDestSeqData && pkSeqDataToSynchronize);

    // Create sequence data from scene graph pose.
    NiSequenceData* pkPoseSeqData = AddPoseSequenceData(
        pkSeqDataToSynchronize);
    if (!pkPoseSeqData)
    {
        return false;
    }

    // Activate new pose sequence.
    pkPoseSequence = ActivateSequence(pkPoseSeqData, iPriority);
    if (!pkPoseSequence)
    {
        // Remove sequence data of unused temporary pose.
        RemoveSequenceData(pkPoseSeqData);
        return false;
    }

    // Blend from pose to destination sequence.
    pkDestSequence = pkPoseSequence->StartBlend(
        pkDestSeqData, fDuration, kDestTextKey, iPriority, 
        1.0f, 1.0f, NULL, 1.0f);
    if (!pkDestSequence)
    {
        // Sequence data for temporary pose will be removed automatically.
        pkPoseSequence->Deactivate(0.0f, false, false, true);
        pkPoseSequence = NULL;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiControllerManager::BlendFromPose(NiSequenceData* pkDestSeqData,
    float fDestFrame, float fDuration, int iPriority,
    NiSequenceData* pkSeqDataToSynchronize,
    NiControllerSequence*& pkPoseSequence, 
    NiControllerSequence*& pkDestSequence)
{
    // This function is not as efficient as BlendFromSequence in terms 
    // of memory and performance. Consequently, it is recommended that 
    // BlendFromPose be used only when BlendFromSequence is not applicable.

    NIASSERT(pkDestSeqData && pkSeqDataToSynchronize);

    // Create sequence data from scene graph pose.
    NiSequenceData* pkPoseSeqData = AddPoseSequenceData(
        pkSeqDataToSynchronize);
    if (!pkPoseSeqData)
    {
        return false;
    }

    // Activate new pose sequence.
    pkPoseSequence = ActivateSequence(pkPoseSeqData, iPriority);
    if (!pkPoseSequence)
    {
        // Remove sequence data of unused temporary pose.
        RemoveSequenceData(pkPoseSeqData);
        return false;
    }

    // Blend from pose to destination sequence.
    pkDestSequence = pkPoseSequence->StartBlend(
        pkDestSeqData, fDuration, fDestFrame, iPriority, 
        1.0f, 1.0f, NULL, 1.0f);
    if (!pkDestSequence)
    {
        // Sequence data for temporary pose will be removed automatically.
        pkPoseSequence->Deactivate(0.0f, false, false, true);
        pkPoseSequence = NULL;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiControllerManager::SetMaxRecycledSequences(
    unsigned short usMaxRecycledSequences)
{
    m_usMaxRecycledSequences = usMaxRecycledSequences;

    // Remove any excess sequences from the recycled array.
    for (unsigned int ui = m_kRecycledSequenceArray.GetSize(); 
        ui > usMaxRecycledSequences; ui--)
    {
        NiControllerSequence* pkSequence = 
            m_kRecycledSequenceArray.GetAt(ui - 1);
        NIASSERT(pkSequence);
        m_kRecycledSequenceArray.Remove(pkSequence);
    }
}
//---------------------------------------------------------------------------
NiControllerSequencePtr NiControllerManager::GetRecycledSequence()
{
    unsigned int uiNumRecycledSequences = m_kRecycledSequenceArray.GetSize();
    if (uiNumRecycledSequences == 0)
    {
        return NULL;
    }
    // Verify that we'll have the only ref count on this sequence
    // after it's removed from the recycled array.
    NiControllerSequencePtr spSequence = m_kRecycledSequenceArray.GetAt(
        uiNumRecycledSequences - 1);
    NIASSERT(spSequence);
    NIASSERT(spSequence->GetRefCount() == 2);
    m_kRecycledSequenceArray.Remove(spSequence);
    NIASSERT(spSequence->GetRefCount() == 1);
    return spSequence;
}
//---------------------------------------------------------------------------
void NiControllerManager::Init(NiAVObject* pkTarget, 
    bool bCumulativeAnimations, NiPoseBinding* pkPoseBinding,
    bool bMoveAccumTransformToChild)
{
    NIASSERT(pkTarget);
    NIASSERT(!m_spPoseBinding);
    NIASSERT(!m_spPoseBlender);
    NIASSERT(!m_spMTPoseHandler);
    NIASSERT(!m_pkLODController);

    NiTimeController::SetTarget(pkTarget);
    if (!m_spObjectPalette)
    {
        m_spObjectPalette = NiNew NiDefaultAVObjectPalette(pkTarget);
    }

    m_pkLODController = NiSkinningLODController::FindSkinningLODController(
        pkTarget);

    // Search the scene graph branch for the accum root, if needed.
    NiAVObject* pkAccumRoot = NULL;
    NIASSERT(m_bCumulative == false);
    if (bCumulativeAnimations)
    {
        pkAccumRoot = FindAccumRoot((NiAVObject*)GetTarget());
        m_bCumulative = (pkAccumRoot != NULL);
    }

    if (!pkPoseBinding)
    {
        m_spPoseBinding = NiNew NiPoseBinding(this, pkAccumRoot);
    }
    else
    {
        m_spPoseBinding = pkPoseBinding;
    }
    NIASSERT(m_bCumulative == m_spPoseBinding->GetCumulativeAnimations());

    m_spPoseBlender = NiNew NiPoseBlender(this);

    m_spMTPoseHandler = NiNew NiMultiTargetPoseHandler(this, 
        bMoveAccumTransformToChild);
}
//---------------------------------------------------------------------------
void NiControllerManager::Shutdown()
{
    SetMaxRecycledSequences(0);
    RemoveAllSequences();
    RemoveAllSequenceDatas();

    m_spObjectPalette = NULL;
    m_spPoseBinding = NULL;
    m_spPoseBlender = NULL;
    m_spMTPoseHandler = NULL;
    m_pkLODController = NULL;
}
//---------------------------------------------------------------------------
bool NiControllerManager::RecycleSequence(NiControllerSequence* pkSequence)
{
    NIASSERT(pkSequence);
    NIASSERT(pkSequence->GetOwner() == this);
    NIASSERT(pkSequence->GetRefCount() == 1);

    unsigned short usNumRecycledSequences = GetNumRecycledSequences();
    if (usNumRecycledSequences < m_usMaxRecycledSequences)
    {
        m_kRecycledSequenceArray.AddFirstEmpty(pkSequence);
        NIASSERT(pkSequence->GetRefCount() == 2);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiControllerManager::TargetIsRequiredType() const
{
    return NiIsKindOf(NiAVObject, m_pkTarget);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiControllerManager);
//---------------------------------------------------------------------------
void NiControllerManager::CopyMembers(NiControllerManager* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);

    if (m_spObjectPalette)
    {
        pkDest->m_spObjectPalette = (NiAVObjectPalette*)m_spObjectPalette
            ->CreateClone(kCloning);
    }

    // Do not clone sequences in m_kSeqDataArray.

    // Do not copy sequences in m_kRecycledSequenceArray or m_kSequenceArray.

    // Ignore m_spPoseBinding, m_spPoseBlender, m_spMTPoseHandler, 
    // and m_pkLODController.
}
//---------------------------------------------------------------------------
void NiControllerManager::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    NiObject* pkClone = 0;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    if (bCloned)
    {
        NiControllerManager* pkDest = (NiControllerManager*)pkClone;

        if (m_spObjectPalette)
        {
            m_spObjectPalette->ProcessClone(kCloning);
        }

        NiObject* pkCloneTarget = 0;
        bCloned = kCloning.m_pkCloneMap->GetAt(GetTarget(), pkCloneTarget);
        NIASSERT(bCloned && pkCloneTarget);
        // Share the pose binding.
        pkDest->Init((NiAVObject*)pkCloneTarget, m_bCumulative, 
            m_spPoseBinding, false);
        pkDest->SetMaxRecycledSequences(m_usMaxRecycledSequences);

        unsigned int uiNumSeqDatas = m_kSeqDataArray.GetSize();
        for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
        {
            // Skip temporary poses.
            NiSequenceData* pkSeqData = m_kSeqDataArray.GetAt(ui);
            if (pkSeqData && !pkSeqData->GetTempPose())
            {
                // Share original sequence data.
                NIVERIFY(pkDest->AddSequenceData(pkSeqData));
            }
        }
    }

    // Do not process sequences in m_kRecycledSequenceArray 
    // or m_kSequenceArray.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
bool NiControllerManager::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
    {
        return false;
    }

    unsigned int uiNumSeqDatas = m_kSeqDataArray.GetSize();
    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        // Skip temporary poses.
        NiSequenceData* pkSeqData = m_kSeqDataArray.GetAt(ui);
        if (pkSeqData && !pkSeqData->GetTempPose())
        {
            pkSeqData->RegisterStreamables(kStream);
        }
    }

    if (m_spObjectPalette)
    {
        m_spObjectPalette->RegisterStreamables(kStream);
    }

    if (m_spPoseBinding)
    {
        m_spPoseBinding->RegisterStreamables(kStream);
    }

    // Do not register sequences in m_kSequenceArray.

    // Ignore m_spPoseBlender, m_spMTPoseHandler, and m_pkLODController.

    return true;
}
//---------------------------------------------------------------------------
void NiControllerManager::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiBool bCumulative = m_bCumulative;
    NiStreamSaveBinary(kStream, bCumulative);

    // Count number of sequence datas to save.
    unsigned int uiNumToSave = 0;
    unsigned int uiNumSeqDatas = m_kSeqDataArray.GetSize();
    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        // Skip temporary poses.
        NiSequenceData* pkSeqData = m_kSeqDataArray.GetAt(ui);
        if (pkSeqData && !pkSeqData->GetTempPose())
        {
            uiNumToSave++;
        }
    }
    NiStreamSaveBinary(kStream, uiNumToSave);

    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        // Skip temporary poses.
        NiSequenceData* pkSeqData = m_kSeqDataArray.GetAt(ui);
        if (pkSeqData && !pkSeqData->GetTempPose())
        {
            kStream.SaveLinkID(pkSeqData);
        }
    }

    kStream.SaveLinkID(m_spObjectPalette);

    kStream.SaveLinkID(m_spPoseBinding);

    // Do not save sequences in m_kSequenceArray.

    // Ignore m_spPoseBlender, m_spMTPoseHandler, and m_pkLODController.
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiControllerManager);

//---------------------------------------------------------------------------
void NiControllerManager::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiBool bCumulative;
    NiStreamLoadBinary(kStream, bCumulative);
    m_bCumulative = (bCumulative != 0);

    kStream.ReadMultipleLinkIDs(); // m_kSeqDataArray

    if (kStream.GetFileVersion() >= kStream.GetVersion(10, 1, 0, 104))
    {
        m_spObjectPalette = (NiDefaultAVObjectPalette*)
            kStream.ResolveLinkID();
    }

    if (kStream.GetFileVersion() >= NiAnimationConstants::GetPoseVersion())
    {
        m_spPoseBinding = (NiPoseBinding*)kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiControllerManager::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
    
    unsigned int uiNumSeqDatas = kStream.GetNumberOfLinkIDs();
    m_kSeqDataArray.SetSize(uiNumSeqDatas);
    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        NiSequenceData* pkSeqData = (NiSequenceData*)
            kStream.GetObjectFromLinkID();
        NIVERIFY(AddSequenceData(pkSeqData));
    }
}
//---------------------------------------------------------------------------
void NiControllerManager::PostLinkObject(NiStream& kStream)
{
    NiTimeController::PostLinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiControllerManager::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
    {
        return false;
    }

    NiControllerManager* pkManager = (NiControllerManager*) pkObject;

    unsigned int uiNumSeqDatas = m_kSeqDataArray.GetSize();
    if (uiNumSeqDatas != pkManager->m_kSeqDataArray.GetSize())
    {
        return false;
    }

    // Most sequence datas will be shared.
    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        NiSequenceData* pkSeqData0 = m_kSeqDataArray.GetAt(ui);
        NiSequenceData* pkSeqData1 = 
            pkManager->m_kSeqDataArray.GetAt(ui);

        if (pkSeqData0 != pkSeqData1)
        {
            if (pkSeqData0 && pkSeqData1)
            {
                if (!pkSeqData0->IsEqual(pkSeqData1))
                    return false;
            }
            else
            {
                return false;
            }
        }
    }

    if (m_spPoseBinding != pkManager->m_spPoseBinding)
    {
        if (!m_spPoseBinding || 
            !pkManager->m_spPoseBinding ||
            !m_spPoseBinding->IsEqual(pkManager->m_spPoseBinding))
        {
            return false;
        }
    }

    // Ignore sequences in m_kSequenceArray.

    // Ignore m_spPoseBlender, m_spMTPoseHandler, and m_pkLODController.

    return true;
}
//---------------------------------------------------------------------------
