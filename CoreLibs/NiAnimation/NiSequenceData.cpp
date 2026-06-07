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

#include "NiSequenceData.h"
#include "NiAnimationConstants.h"
#include <NiCloningProcess.h>
#include <NiAVObjectPalette.h>
#include "NiTextKeyMatch.h"
#include "NiTimeController.h"
#include "NiInterpController.h"
#include "NiSkinningLODController.h"
#include "NiScratchPad.h"
#include "NiControllerSequence.h"
#include "NiInterpolatorConverter.h"

NiImplementRTTI(NiSequenceData, NiObject);

const float NiSequenceData::INVALID_TIME = -FLT_MAX;
const unsigned int NiSequenceData::INVALID_INDEX = (unsigned int) -1;

//---------------------------------------------------------------------------
NiSequenceData::NiSequenceData(const NiFixedString& kName,
    unsigned int uiMaxEvaluators) : 
    m_uiMaxEvaluators(0), m_uiNumEvaluators(0), m_pspEvaluatorArray(NULL), 
    m_spTextKeys(NULL), m_fDuration(0), 
    m_eCycleType(NiTimeController::LOOP), m_fFrequency(1.0f), 
    m_uiCallbackArraySize(0), m_uiNumCallbacks(0),
    m_pkDefaultActivationCallbacks(NULL), m_uFlags(0), 
    m_spDeprecatedStringPalette(NULL),  m_pkStreamedInterpArray(NULL)
{
    SetName(kName);

    if (uiMaxEvaluators > 0)
    {
        SetMaxEvaluators(uiMaxEvaluators);
    }
}
//---------------------------------------------------------------------------
NiSequenceData::NiSequenceData() : 
    m_uiMaxEvaluators(0), m_uiNumEvaluators(0), m_pspEvaluatorArray(NULL), 
    m_spTextKeys(NULL), m_fDuration(0), 
    m_eCycleType(NiTimeController::LOOP), m_fFrequency(1.0f), 
    m_uiCallbackArraySize(0), m_uiNumCallbacks(0),
    m_pkDefaultActivationCallbacks(NULL), m_uFlags(0), 
    m_spDeprecatedStringPalette(NULL), m_pkStreamedInterpArray(NULL)
{
}
//---------------------------------------------------------------------------
NiSequenceData::~NiSequenceData()
{
    for (unsigned int ui = 0; ui < m_uiNumCallbacks; ui++)
    {
        m_pkDefaultActivationCallbacks[ui].ClearValues();
    }
    NiDelete[] m_pkDefaultActivationCallbacks;
    m_uiCallbackArraySize = 0;
    m_uiNumCallbacks = 0;

    NIASSERT(!m_pkStreamedInterpArray);
    NiDelete[] m_pspEvaluatorArray;
    m_pspEvaluatorArray = NULL;
    m_uiNumEvaluators = 0;
    m_uiMaxEvaluators = 0;

    m_spTextKeys = NULL;
    m_spDeprecatedStringPalette = NULL;
}
//---------------------------------------------------------------------------
NiSequenceDataPtr NiSequenceData::CreateSequenceDataFromFile(
    const char* pcFilename, const NiFixedString& kSequenceName)
{
    NiStream kStream;
    if (!kStream.Load(pcFilename))
    {
        return NULL;
    }

    return CreateSequenceDataFromFile(kStream, kSequenceName);
}
//---------------------------------------------------------------------------
NiSequenceDataPtr NiSequenceData::CreateSequenceDataFromFile(
    const NiStream& kStream, const NiFixedString& kSequenceName)
{
    for (unsigned int i = 0; i < kStream.GetObjectCount(); i++)
    {
        NiSequenceData* pkSeqData = NiDynamicCast(NiSequenceData,
            kStream.GetObjectAt(i));

        if (pkSeqData && pkSeqData->GetName() == kSequenceName)
            return pkSeqData;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiSequenceDataPtr NiSequenceData::CreateSequenceDataFromFile(
    const char* pcFilename, unsigned int uiIndex)
{
    NiStream kStream;
    if (!kStream.Load(pcFilename))
    {
        return NULL;
    }

    return CreateSequenceDataFromFile(kStream, uiIndex);
}
//---------------------------------------------------------------------------
NiSequenceDataPtr NiSequenceData::CreateSequenceDataFromFile(
    const NiStream& kStream, unsigned int uiIndex)
{
    if (uiIndex >= kStream.GetObjectCount())
    {
        return NULL;
    }

    return NiDynamicCast(NiSequenceData, kStream.GetObjectAt(uiIndex));
}
//---------------------------------------------------------------------------
bool NiSequenceData::CreateAllSequenceDatasFromFile(const char* pcFilename,
    NiSequenceDataPointerArray& kSeqDatas)
{
    NiStream kStream;
    if (!kStream.Load(pcFilename))
    {
        return false;
    }

    return CreateAllSequenceDatasFromFile(kStream, kSeqDatas);
}
//---------------------------------------------------------------------------
bool NiSequenceData::CreateAllSequenceDatasFromFile(const NiStream& kStream,
    NiSequenceDataPointerArray& kSeqDatas)
{
    kSeqDatas.RemoveAll();

    for (unsigned int ui = 0; ui < kStream.GetObjectCount(); ui++)
    {
        NiSequenceDataPtr spSeqData = 
            CreateSequenceDataFromFile(kStream, ui);
        if (spSeqData)
        {
            kSeqDatas.Add(spSeqData);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
float NiSequenceData::GetKeyTimeAt(const NiFixedString& kTextKey) const
{
    if ( !kTextKey.Exists() )
    {
        return INVALID_TIME;
    }
    else
    {
        NiTextKeyMatch kMatchObject(kTextKey);
        return GetKeyTimeAt(&kMatchObject);
    }
}
//---------------------------------------------------------------------------
float NiSequenceData::GetKeyTimeAt(NiTextKeyMatch* pkMatchObject) const
{
    if (!pkMatchObject || !m_spTextKeys)
    {
        return INVALID_TIME;
    }

    unsigned int uiNumKeys;
    NiTextKey* pkKeys = m_spTextKeys->GetKeys(uiNumKeys);
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        if ( pkMatchObject->IsKeyMatch(pkKeys[ui].GetText()))
        {
            return pkKeys[ui].GetTime();
        }
    }

    return INVALID_TIME;
}
//---------------------------------------------------------------------------
unsigned int NiSequenceData::AddEvaluator(NiEvaluator* pkEvaluator)
{
    NIASSERT(pkEvaluator);

    m_uiNumEvaluators++;
    if (m_uiNumEvaluators >= m_uiMaxEvaluators)
    {
        SetMaxEvaluators(m_uiNumEvaluators);
    }
    m_pspEvaluatorArray[m_uiNumEvaluators - 1] = pkEvaluator;

    return m_uiNumEvaluators - 1;
}
//---------------------------------------------------------------------------
NiEvaluatorPtr NiSequenceData::RemoveEvaluator(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiNumEvaluators);

    // Cache the evaluator being removed.
    NiEvaluatorPtr spEvaluator = m_pspEvaluatorArray[uiIndex];

    // Compact the array to fill the gap.
    for (unsigned int ui = uiIndex + 1; ui < m_uiNumEvaluators; ui++)
    {
        m_pspEvaluatorArray[ui - 1] = m_pspEvaluatorArray[ui];
    }

    // Decrement the number of evaluators.
    m_uiNumEvaluators--;
    m_pspEvaluatorArray[m_uiNumEvaluators] = NULL;

    return spEvaluator;
}
//---------------------------------------------------------------------------
void NiSequenceData::SetMaxEvaluators(unsigned int uiMax)
{
    NIASSERT(uiMax > 0);

    // Allocate new array.
    NiEvaluatorPtr* pspNewEvalArray = NiNew NiEvaluatorPtr[uiMax];

    // Copy existing data.
    unsigned int uiNumItemsToCopy = (m_uiMaxEvaluators < uiMax) ? 
        m_uiMaxEvaluators : uiMax;
    for (unsigned int ui = 0; ui < uiNumItemsToCopy; ui++)
    {
        pspNewEvalArray[ui] = m_pspEvaluatorArray[ui];
    }

    // Delete existing data.
    NiDelete[] m_pspEvaluatorArray;

    // Update variables.
    m_uiMaxEvaluators = uiMax;
    m_pspEvaluatorArray = pspNewEvalArray;
}
//---------------------------------------------------------------------------
void NiSequenceData::SortEvaluatorsByLOD(
    NiSkinningLODController* pkLODController, NiAVObjectPalette* pkPalette)
{
    NIASSERT(pkLODController);
    NIASSERT(pkPalette);

    unsigned int uiNumLODs = pkLODController->GetNumberOfBoneLODs();
    if (uiNumLODs < 2 || m_uiNumEvaluators < 2)
    {
        return;
    }

    // Create arrays on the stack to store the evaluators and 
    // their associated LODs.
    NiEvaluator** ppkEvaluators = NiStackAlloc(NiEvaluator*, m_uiNumEvaluators);
    NiUInt8* pucLODP1Array = NiStackAlloc(NiUInt8, m_uiNumEvaluators);

    // Copy the evaluators to the temporary arrays.
    unsigned int uiNumEvaluators = 0;
    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = m_pspEvaluatorArray[ui];
        if (!pkEvaluator)
        {
            continue;
        }

        unsigned int uiLODP1 = uiNumLODs;
        if (pkEvaluator->IsTransformEvaluator())
        {
            const NiFixedString& kAVObjectName = 
                pkEvaluator->GetAVObjectName();
            if (kAVObjectName.Exists())
            {
                NiAVObject* pkAVObject = pkPalette->GetAVObject(
                    kAVObjectName);
                if (pkAVObject)
                {
                    uiLODP1 = pkLODController->GetLODForNode(
                        pkAVObject) + 1;
                }
            }
        }

        ppkEvaluators[uiNumEvaluators] = pkEvaluator;
        NIASSERT(uiLODP1 <= 255);
        pucLODP1Array[uiNumEvaluators] = (NiUInt8)uiLODP1;
        uiNumEvaluators++;

        // Increment the ref count so it's not accidentally deleted
        // when we clear the source array.
        pkEvaluator->IncRefCount();
        m_pspEvaluatorArray[ui] = NULL;
    }

    // The source evaluator array has been cleared.
    m_uiNumEvaluators = 0;

    // Loop through each LOD and add the corresponding evaluators
    // to the sequence.
    for (unsigned int uiLODP1 = uiNumLODs; uiLODP1 > 0; uiLODP1--)
    {
        for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
        {
            if (pucLODP1Array[ui] == uiLODP1)
            {
                m_pspEvaluatorArray[m_uiNumEvaluators] = 
                    ppkEvaluators[ui];
                m_uiNumEvaluators++;

                // Decrement the ref count now that it's stored
                // in the sequence.
                ppkEvaluators[ui]->DecRefCount();
                ppkEvaluators[ui] = NULL;
            }
        }
    }

    NiStackFree(ppkEvaluators);
    NiStackFree(pucLODP1Array);
}
//---------------------------------------------------------------------------
void NiSequenceData::SortEvaluatorsByPBChannelType()
{
    if (m_uiNumEvaluators < 2)
    {
        return;
    }

    // Create arrays on the stack to store the evaluators and their
    // associated group (1 through 8).
    NiEvaluator** ppkEvaluators = NiStackAlloc(NiEvaluator*, m_uiNumEvaluators);
    NiUInt8* pucGroupArray = NiStackAlloc(NiUInt8, m_uiNumEvaluators);

    // Copy the evaluators to the temporary arrays.
    unsigned int uiNumEvaluators = 0;
    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = m_pspEvaluatorArray[ui];
        if (!pkEvaluator)
        {
            continue;
        }

        // Group 7 is referenced evaluators.
        unsigned int uiGroup = 7;
        if (pkEvaluator->IsTransformEvaluator())
        {
            NIASSERT(pkEvaluator->GetMaxEvalChannels() == 3);
            if (pkEvaluator->IsEvalChannelInvalid(0) && 
                pkEvaluator->IsEvalChannelInvalid(1) && 
                pkEvaluator->IsEvalChannelInvalid(2))
            {
                // Group 8 is invalid evaluators.
                uiGroup = 8;
            }
            else if (!pkEvaluator->IsReferencedEvaluator())
            {
                // Group 6 is transform evaluators.
                uiGroup = 6;
            }
        }
        else
        {
            NIASSERT(pkEvaluator->GetMaxEvalChannels() == 1);
            if (pkEvaluator->IsEvalChannelInvalid(0))
            {
                // Group 7 is invalid evaluators.
                uiGroup = 7;
            }
            else if (!pkEvaluator->IsReferencedEvaluator())
            {
                NiPoseBufferChannelType ePBChannelType = 
                    pkEvaluator->GetRawEvalPBChannelType(0);
                if (ePBChannelType == PBCOLORCHANNEL)
                {
                    // Group 1 is color evaluators.
                    uiGroup = 1;
                }
                else if (ePBChannelType == PBBOOLCHANNEL)
                {
                    // Group 2 is bool evaluators.
                    uiGroup = 2;
                }
                else if (ePBChannelType == PBFLOATCHANNEL)
                {
                    // Group 3 is float evaluators.
                    uiGroup = 3;
                }
                else if (ePBChannelType == PBPOINT3CHANNEL)
                {
                    // Group 4 is point3 evaluators.
                    uiGroup = 4;
                }
                else
                {
                    NIASSERT(ePBChannelType == PBROTCHANNEL);
                    // Group 5 is rotation evaluators.
                    uiGroup = 5;
                }
            }
        }
        NIASSERT(uiGroup >= 1 && uiGroup <= 8);

        ppkEvaluators[uiNumEvaluators] = pkEvaluator;
        pucGroupArray[uiNumEvaluators] = (NiUInt8)uiGroup;
        uiNumEvaluators++;

        // Increment the ref count so it's not accidentally deleted
        // when we clear the source array.
        pkEvaluator->IncRefCount();
        m_pspEvaluatorArray[ui] = NULL;
    }

    // The source evaluator array has been cleared.
    m_uiNumEvaluators = 0;

    // Loop through each group and add the corresponding evaluators
    // to the sequence.
    for (unsigned int uiGroup = 1; uiGroup <= 8; uiGroup++)
    {
        for (unsigned int ui = 0; ui < uiNumEvaluators; ui++)
        {
            if (pucGroupArray[ui] == uiGroup)
            {
                m_pspEvaluatorArray[m_uiNumEvaluators] = ppkEvaluators[ui];
                m_uiNumEvaluators++;

                // Decrement the ref count now that it's stored
                // in the sequence.
                ppkEvaluators[ui]->DecRefCount();
                ppkEvaluators[ui] = NULL;
            }
        }
    }

    NiStackFree(ppkEvaluators);
    NiStackFree(pucGroupArray);
}
//---------------------------------------------------------------------------
void NiSequenceData::SetCallbackArraySize(unsigned int uiNewArraySize)
{
    NIASSERT(uiNewArraySize > 0 && uiNewArraySize >= m_uiNumCallbacks);

    // Allocate new array.
    ActivationCallbackItem* pkNewArray = 
        NiNew ActivationCallbackItem[uiNewArraySize];

    // Copy existing data.
    for (unsigned int ui = 0; ui < m_uiNumCallbacks; ui++)
    {
        pkNewArray[ui] = m_pkDefaultActivationCallbacks[ui];
    }

    // Delete existing data.
    NiDelete[] m_pkDefaultActivationCallbacks;

    // Update variables.
    m_uiCallbackArraySize = uiNewArraySize;
    m_pkDefaultActivationCallbacks = pkNewArray;
}
//---------------------------------------------------------------------------
bool NiSequenceData::VerifyMatchingMorphKeys(
    NiControllerSequence* pkTimeSyncSeq)
{
    NIASSERT(pkTimeSyncSeq);
    return VerifyMatchingMorphKeys(pkTimeSyncSeq->GetTextKeys());
}
//---------------------------------------------------------------------------
float NiSequenceData::FindCorrespondingMorphFrame(
    NiSequenceData* pkSourceSeqData, float fSourceSeqDataTime)
{
    // This function uses all existing "morph:" key pairs between this
    // sequence data and pkSourceSeqData to map the time in 
    // pkSourceSeqData, fSourceSeqDataTime, to a corresponding time in 
    // this sequence data. fSourceSeqDataTime need not be within the 
    // range of pkSourceSeqData since ComputeScaledTime will be called
    // on it. The time returned is within the range of this sequence.

    // Compute the scaled source sequence data time.
    float fScaledSourceTime = pkSourceSeqData->ComputeScaledTime(
        fSourceSeqDataTime);

    // Find the "morph:" keys in the (first set of text keys of the) source 
    // sequence data surrounding fSourceSeqDataTime.
    // Assumptions: 
    // - keys are in chronological order
    // - at least one "morph:" key exists
    // - in any "combined sequence" (>1 set of _text keys_), the _first_ set
    //   determines the morph values
    // - MORPHING SEQUENCES ARE PURELY LOOPED (not reversed or clamped)
    //   If they are not looped, the morph will only work if, during
    //   the transition, neither sequence hits its endpoint
    const char* pcMorphTextKey = NiAnimationConstants::GetMorphTextKey();
    int iMorphTextKeyLength = NiAnimationConstants::GetMorphTextKeyLength();
    unsigned int uiNumKeys;
    NiTextKey* pkKeys = pkSourceSeqData->GetTextKeys()->GetKeys(uiNumKeys);
    NiTextKey* pkFirstKey = NULL;
    NiTextKey* pkPrevKey = NULL;
    NiTextKey* pkNextKey = NULL;
    NiTextKey* pkLastKey = NULL;
    unsigned int ui;
    for (ui = 0; ui < uiNumKeys; ui++)
    {
        const char* pcKeyText = pkKeys[ui].GetText();
        if (pcKeyText && strncmp(pcKeyText, pcMorphTextKey, 
            iMorphTextKeyLength) == 0)
        {
            pkLastKey = &(pkKeys[ui]);
            if (!pkFirstKey)
                pkFirstKey = pkLastKey;

            if (pkKeys[ui].GetTime() < fScaledSourceTime)
                pkPrevKey = pkLastKey;
            else if (!pkNextKey)
                pkNextKey = pkLastKey;
        }
    }

    // If this assertion is hit, the base sequence does not have
    // any "morph:" text keys.
    NIASSERT(pkNextKey || pkPrevKey);

    bool bCreateNext = false;

    // Find the frame location relative to the base morph frames
    float fSourceRatio;
    if (!pkNextKey || !pkPrevKey)
    {
        // If either prev or next key is not found, supply one based on 
        //   the CycleType attribute of the sequence
        if (!pkNextKey)
        {
            // Assuming LOOP
            float fStartTime = pkPrevKey->GetTime();
            float fEndTime = pkSourceSeqData->m_fDuration + 
                pkFirstKey->GetTime();
            fSourceRatio = (fScaledSourceTime - fStartTime) / 
                (fEndTime - fStartTime);
            pkNextKey = pkFirstKey;
            bCreateNext = true;
        }
        else // (!pkPrevKey)
        {
            // Assuming LOOP
            float fStartTime = pkLastKey->GetTime() - 
                pkSourceSeqData->m_fDuration;
            float fEndTime = pkNextKey->GetTime();
            fSourceRatio = (fScaledSourceTime - fStartTime) / 
                (fEndTime - fStartTime);
            pkPrevKey = pkLastKey;
        }
    }
    else
    {
        float fStartTime = pkPrevKey->GetTime();
        float fEndTime = pkNextKey->GetTime();
        fSourceRatio = (fScaledSourceTime - fStartTime) / 
            (fEndTime - fStartTime);
    }

    // Find the corresponding keys in the Target sequence
    // Assumptions: 
    // - there is exactly one "morph:" key in the Target sequence with the
    //   same name as each key in the base sequence
    // - the corresponding keys are in the same order in both sequences
    // - if the morph extends beyond the length of a sequence (ie loops or 
    //   reverses), both sequences have the same CycleType value
    pkKeys = m_spTextKeys->GetKeys(uiNumKeys);

    bool bPrevFound = (pkPrevKey == NULL); 
    bool bNextFound = (pkNextKey == NULL); 
    NiTextKey* pkTargetPrevKey = NULL;
    NiTextKey* pkTargetNextKey = NULL;
    for (ui = 0; ui < uiNumKeys && !(bPrevFound && bNextFound); ui++)
    {
        if (!bPrevFound && 
            strcmp(pkKeys[ui].GetText(), pkPrevKey->GetText()) == 0)
        {
            pkTargetPrevKey = &(pkKeys[ui]);
            bPrevFound = true;
        }
        if (!bNextFound && 
            strcmp(pkKeys[ui].GetText(), pkNextKey->GetText()) == 0)
        {
            pkTargetNextKey = &(pkKeys[ui]);
            bNextFound = true;
        }
    }

    // If this assertion is hit, the target sequence does not contain a
    // corresponding "morph:" text key.
    NIASSERT(bPrevFound && bNextFound);

    // Find the corresponding frame based on the target morph frames
    float fTargetHi = 0.0f, fTargetLo = 0.0f;

    if (pkTargetPrevKey == pkTargetNextKey)
    {
        fTargetLo = pkTargetPrevKey->GetTime();
        fTargetHi = pkTargetNextKey->GetTime();

        // We are in the situation where only 1 morph tag was found
        if (bCreateNext)
            fTargetHi += m_fDuration;
        else // bCreatePrev implied
            fTargetLo -= m_fDuration;
    }
    else
    {
        if (pkTargetNextKey)
            fTargetHi = pkTargetNextKey->GetTime();
        if (pkTargetPrevKey)
            fTargetLo = pkTargetPrevKey->GetTime();

        // Handle case where tags "cross"
        if (fTargetLo > fTargetHi)
            fTargetHi += m_fDuration;
    }

    float fTargetFrame = fTargetLo + fSourceRatio * (fTargetHi - fTargetLo);

    // Ensure the target frame is within the range of this sequence.
    if (fTargetFrame < 0.0f)
        fTargetFrame += m_fDuration;
    else if (fTargetFrame > m_fDuration)
        fTargetFrame -= m_fDuration;

    return fTargetFrame;
}
//---------------------------------------------------------------------------
float NiSequenceData::ComputeScaledTime(float fTime)
{
    // This function converts the incoming time value to scaled time using 
    // the frequency of the NiSequenceData in combination with its 
    // cycle type. The returned scaled time represents the local time units 
    // found in the animation keys whereby 0.0 is the first frame and 
    // GetDuration is the final frame. 

    // Scale time linearly by frequency value 
    // specified by the application.
    //
    // time' = time * freq
    //
    float fScaledTime = fTime * m_fFrequency;

    // Switch on m_CycleType
    switch (m_eCycleType)
    {
        case NiTimeController::LOOP:
        {
            // Loop -- only need fractional time
            float fSpan = m_fDuration;
            if (fSpan != 0.0f)
            {
                if (fScaledTime != m_fDuration)
                {
                    fScaledTime = NiFmod(fScaledTime, fSpan);
                }

                if (fScaledTime < 0.0f)
                {
                    fScaledTime += fSpan;
                }
            }
            else
            {
                fScaledTime = 0.0f;
            }
            break;
        }
        case NiTimeController::CLAMP:
            // Clamp - do nothing, conditions below will handle clamping.
            break;
        default:
            // Only LOOP and CLAMP cycle types are supported by sequences.
            NIASSERT(false);
            break;
    }

    if (fScaledTime > m_fDuration)
    {
        fScaledTime = m_fDuration;
    }
    else if (fScaledTime < 0.0f)
    {
        fScaledTime = 0.0f;
    }

    return fScaledTime;
}
//---------------------------------------------------------------------------
bool NiSequenceData::CanSyncTo(NiSequenceData* pkTargetSeqData)
{
    if (!pkTargetSeqData)
        return false;

    if (!VerifyMatchingMorphKeys(pkTargetSeqData->m_spTextKeys))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSequenceData::VerifyMatchingMorphKeys(
    NiTextKeyExtraData* pkTimeSyncTextKeys)
{
    // This function determines if this sequence and pkTimeSyncTextKeys have 
    // matching morph key pairs. If so, this sequence may use any sequence 
    // containing pkTimeSyncTextKeys for time synchronization.

    if (!m_spTextKeys || !pkTimeSyncTextKeys)
    {
        return false;
    }

    unsigned int uiNumKeys1, uiNumKeys2;
    NiTextKey* pkKeys1 = pkTimeSyncTextKeys->GetKeys(uiNumKeys1);
    NiTextKey* pkKeys2 = m_spTextKeys->GetKeys(uiNumKeys2);

    // Check that every "morph:" text key in pkTimeSyncTextKeys has an exact
    // match in this sequence.
    const char* pcMorphTextKey = NiAnimationConstants::GetMorphTextKey();
    int iMorphTextKeyLength = NiAnimationConstants::GetMorphTextKeyLength();
    bool bMorphKeysExist1 = false;
    unsigned int ui;
    for (ui = 0; ui < uiNumKeys1; ui++)
    {
        const char* pcText1 = pkKeys1[ui].GetText();
        NIASSERT(pcText1);
        if (strncmp(pcText1, pcMorphTextKey, iMorphTextKeyLength) == 0)
        {
            bMorphKeysExist1 = true;
            bool bFound = false;
            for (unsigned int uj = 0; uj < uiNumKeys2; uj++)
            {
                const char* pcText2 = pkKeys2[uj].GetText();
                NIASSERT(pcText2);
                if (strcmp(pcText1, pcText2) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                return false;
            }
        }
    }
    if (!bMorphKeysExist1)
    {
        return false;
    }

    // Check that every "morph:" text key in this sequence has an exact match
    // in pkTimeSyncTextKeys.
    bool bMorphKeysExist2 = false;
    for (ui = 0; ui < uiNumKeys2; ui++)
    {
        const char* pcText2 = pkKeys2[ui].GetText();
        NIASSERT(pcText2);
        if (strncmp(pcText2, pcMorphTextKey, iMorphTextKeyLength) == 0)
        {
            bMorphKeysExist2 = true;
            bool bFound = false;
            for (unsigned int uj = 0; uj < uiNumKeys1; uj++)
            {
                const char* pcText1 = pkKeys1[uj].GetText();
                NIASSERT(pcText1);
                if (strcmp(pcText2, pcText1) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                return false;
            }
        }
    }
    if (!bMorphKeysExist2)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSequenceData);
//---------------------------------------------------------------------------
void NiSequenceData::CopyMembers(NiSequenceData* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->SetName(m_kName);

    pkDest->SetMaxEvaluators(m_uiMaxEvaluators);
    pkDest->m_uiNumEvaluators = m_uiNumEvaluators;
    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        pkDest->m_pspEvaluatorArray[ui] = 
            (NiEvaluator*)m_pspEvaluatorArray[ui]->
            CreateSharedClone(kCloning); 
    }

    pkDest->m_spTextKeys = m_spTextKeys;

    pkDest->m_fDuration = m_fDuration;
    pkDest->m_eCycleType = m_eCycleType;
    pkDest->m_fFrequency = m_fFrequency;

    pkDest->SetAccumRootName(m_kAccumRootName);
    pkDest->m_uFlags = m_uFlags;

    // Ignore m_kDefaultActivationCallbacks.
    // Ignore m_spDeprecatedStringPalette.
}
//---------------------------------------------------------------------------
unsigned int NiSequenceData::GetScratchPadBufferSize(
    bool bCumulativeAnimations, NiAVObjectPalette* pkPalette, 
    unsigned int& uiNumUniqueBasisDatas) const
{
    // This implementation can match up to MAXUNIQUEBASISDATAS unique 
    // basis data objects. Once this list is filled, any basis data objects
    // not found in the list will be counted once per reference, instead of 
    // once per unique instance. This is okay since most sequences have 
    // at most one unique basis data object.

    NiBSplineBasisData* apkUniqueBasisDatas[MAXUNIQUEBASISDATAS];
    uiNumUniqueBasisDatas = 0;

    unsigned int auiNumBlockItems[SPBMAXSCRATCHPADBLOCKS];
    for (unsigned int ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        auiNumBlockItems[ui] = 0;
    }

    // Compute the total scratch pad buffer size of all evaluators.

    unsigned int uiScratchPadSize = 0;
    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = m_pspEvaluatorArray[ui];
        NIASSERT(pkEvaluator);

        unsigned int uiMaxEvalChannels = pkEvaluator->GetMaxEvalChannels();
        bool bForceAlwaysUpdate = false;
        if (bCumulativeAnimations && uiMaxEvalChannels == 3 && 
            m_kAccumRootName.Exists() && 
            pkEvaluator->GetAVObjectName() == m_kAccumRootName)
        {
            bForceAlwaysUpdate = true;
        }

        bool bAlreadyShared = false;
        for (unsigned int uj = 0; uj < uiMaxEvalChannels; uj++)
        {
            NiUInt32 uiFillSize;
            bool bSharedFillData;
            NiScratchPadBlock eSPBSegmentData;
            NiBSplineBasisData* pkBasisData;
            if (pkEvaluator->GetChannelScratchPadInfo(uj, 
                bForceAlwaysUpdate, pkPalette, uiFillSize, 
                bSharedFillData, eSPBSegmentData, pkBasisData))
            {
                if (!bAlreadyShared)
                {
                    uiScratchPadSize += uiFillSize;
                    if (bSharedFillData)
                    {
                        bAlreadyShared = true;
                    }
                }

                auiNumBlockItems[SPBEVALUATORSPDATA]++;
                auiNumBlockItems[eSPBSegmentData]++;

                if (pkBasisData)
                {
                    // Check if the basis data was referenced by 
                    // a prior evaluator.
                    bool bFound = false;
                    for (unsigned int uk = 0; uk < uiNumUniqueBasisDatas; uk++)
                    {
                        if (pkBasisData == apkUniqueBasisDatas[uk])
                        {
                            bFound = true;
                            break;   // Found a match.
                        }
                    }

                    // Add the new basis data to the array, if there's room.
                    if (!bFound)
                    {
                        if (uiNumUniqueBasisDatas < MAXUNIQUEBASISDATAS)
                        {
                            apkUniqueBasisDatas[uiNumUniqueBasisDatas] = 
                                pkBasisData;
                        }
                        uiNumUniqueBasisDatas++;
                    }
                }
            }
        }
    }

    uiScratchPadSize += auiNumBlockItems[SPBEVALUATORSPDATA] * 
        sizeof(NiEvaluatorSPData);
    uiScratchPadSize += uiNumUniqueBasisDatas * 
        sizeof(NiBSplineBasisData);
    uiScratchPadSize += auiNumBlockItems[SPBLINEARCOLORSEGMENT] * 
        sizeof(NiScratchPad::LinearColorSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBBSPLINECOLORSEGMENT] * 
        sizeof(NiScratchPad::BSplineColorSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBCONSTANTBOOLSEGMENT] * 
        sizeof(NiScratchPad::ConstantBoolSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBCUBICFLOATSEGMENT] * 
        sizeof(NiScratchPad::CubicFloatSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBBSPLINEFLOATSEGMENT] * 
        sizeof(NiScratchPad::BSplineFloatSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBCUBICPOINT3SEGMENT] * 
        sizeof(NiScratchPad::CubicPoint3SegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBBSPLINEPOINT3SEGMENT] * 
        sizeof(NiScratchPad::BSplinePoint3SegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBSLERPROTSEGMENT] * 
        sizeof(NiScratchPad::SlerpRotSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBSQUADROTSEGMENT] * 
        sizeof(NiScratchPad::SquadRotSegmentData);
    uiScratchPadSize += auiNumBlockItems[SPBBSPLINEROTSEGMENT] * 
        sizeof(NiScratchPad::BSplineRotSegmentData);

    NIASSERT(SPBMAXSCRATCHPADBLOCKS == 13);

    return uiScratchPadSize;
}
//---------------------------------------------------------------------------
void NiSequenceData::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    NiObject* pkClone = 0;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    if (bCloned)
    {
        for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
        {
            NIASSERT(m_pspEvaluatorArray[ui]);
            m_pspEvaluatorArray[ui]->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiSequenceData::IDTag::LoadBinary(NiStream& kStream,
    void*& pvDeprecatedExtraData)
{
    // There are 2 representations of ID Tag objects in the supported versions.
    // The old version must be converted to use NiFixedString objects.
    // The "pvDeprecatedExtraData" argument is used to contain the
    // NiStringPalette version. See its comments below for more details.

    // All strings were stored in an NiStringPalette with handles 
    // that indicated the offset into the master character buffer
    // for each string. These strings were not ref-counted.
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        // All NiStringPalette's for a given NiSequenceData should
        // point to the same value. If not, the conversion code will fail.
#ifdef _DEBUG
        void* pvPreviousExtraData = pvDeprecatedExtraData;
#endif

        // Each IDTag::LoadBinary checks agains the results of the previous
        // IDTag's NiStringPalette. Note that IDTags used to own a smart
        // pointer to the NiStringPalette. To save memory, this smart pointer
        // has not been preserved. It is assumed that the stream holds a 
        // pointer and the NiSequenceData holds a pointer.
        pvDeprecatedExtraData = (NiStringPalette*) kStream.ResolveLinkID();

        NIASSERT((pvPreviousExtraData == NULL) ||
            (pvPreviousExtraData == pvDeprecatedExtraData));

        // We need to load in the handles for the NiStringPalette.
        unsigned int uiNameValue = 0;
        unsigned int uiPropertyType = 0;
        unsigned int uiCtlrType = 0;
        unsigned int uiCtlrID = 0;
        unsigned int uiInterpolatorID = 0;

        NiStreamLoadBinary(kStream, uiNameValue);
        NiStreamLoadBinary(kStream, uiPropertyType);
        NiStreamLoadBinary(kStream, uiCtlrType);
        NiStreamLoadBinary(kStream, uiCtlrID);
        NiStreamLoadBinary(kStream, uiInterpolatorID);

        // Because we have no way of knowing if NiStringPalette has been loaded
        // we must save the handles for use later. The NiFixedStrings that will
        // eventually contain the real values are our best candidate. However,
        // they only store character arrays. This leads to two implementation
        // options, convert NiFixedStrings to unsigned int storage or vice 
        // versa. For memory usage, we have chosen to implement the former.
        // Touching the NiFixedString values after this code may result in
        // a segfault.
        // IDTag::HandleDeprecatedExtraData will be called by the sequence
        // once all objects can be resolved.
#ifdef _DEBUG
        NIASSERT(sizeof(NiFixedString) >= sizeof(unsigned int));
        NiFixedString kDefault;
        NIASSERT(m_kAVObjectName == kDefault);
        NIASSERT(m_kPropertyType == kDefault);
        NIASSERT(m_kCtlrType == kDefault);
        NIASSERT(m_kCtlrID == kDefault);
        NIASSERT(m_kInterpolatorID == kDefault);
#endif

        NiMemcpy(&m_kAVObjectName, sizeof(m_kAVObjectName),
            &uiNameValue, sizeof(uiNameValue));
        NiMemcpy(&m_kPropertyType, sizeof(m_kPropertyType),
            &uiPropertyType, sizeof(uiPropertyType));
        NiMemcpy(&m_kCtlrType, sizeof(m_kCtlrType),
            &uiCtlrType, sizeof(uiCtlrType));
        NiMemcpy(&m_kCtlrID, sizeof(m_kCtlrID),
            &uiCtlrID, sizeof(uiCtlrID));
        NiMemcpy(&m_kInterpolatorID, sizeof(m_kInterpolatorID),
            &uiInterpolatorID, sizeof(uiInterpolatorID));
    }
    // This is the current approach of storing the id information in
    // NiFixedStrings
    else
    {
        kStream.LoadFixedString(m_kAVObjectName);
        kStream.LoadFixedString(m_kPropertyType);
        kStream.LoadFixedString(m_kCtlrType);
        kStream.LoadFixedString(m_kCtlrID);
        kStream.LoadFixedString(m_kInterpolatorID);
    }
}
//---------------------------------------------------------------------------
void NiSequenceData::IDTag::HandleDeprecatedExtraData(
    NiStringPalette* pkPalette, 
#ifdef NIDEBUG
    NiStream& kStream)
#else
    NiStream& )
#endif    
{
    // This method is the companion to the conversion code in 
    // IDTag::LoadBinary. Please see the comments for the code above 
    // for implemetation details.

    // Note that this method is only valid when used for data with
    // NiStringPalette's and handles. Calling it on any other occasion
    // will result in bogus information.
    NIASSERT(kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 113));
    NIASSERT(kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1));
    NIASSERT(pkPalette);

    // Previously, we stored the handles in the NiFixedString's memory
    // space, making the NiFixedStrings dangerous to use.
    unsigned int uiAVObjectName;
    unsigned int uiPropertyType;
    unsigned int uiCtlrType;
    unsigned int uiCtlrID;
    unsigned int uiInterpolatorID;

    NiMemcpy(&uiAVObjectName, sizeof(uiAVObjectName),
        &m_kAVObjectName, sizeof(m_kAVObjectName));
    NiMemcpy(&uiPropertyType, sizeof(uiPropertyType),
        &m_kPropertyType, sizeof(m_kPropertyType));
    NiMemcpy(&uiCtlrType, sizeof(uiCtlrType),
        &m_kCtlrType, sizeof(m_kCtlrType));
    NiMemcpy(&uiCtlrID, sizeof(uiCtlrID),
        &m_kCtlrID, sizeof(m_kCtlrID));
    NiMemcpy(&uiInterpolatorID, sizeof(uiInterpolatorID),
        &m_kInterpolatorID, sizeof(m_kInterpolatorID));

    // It is assumed that the NiFixedStrings were in their default
    // constructed forms prior to being clobbered over with the 
    // palette handles. We now return them to that previous state.
    NiFixedString kString;

    NiMemcpy(&m_kAVObjectName, sizeof(NiFixedString),
        &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kPropertyType, sizeof(NiFixedString),
        &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kCtlrType, sizeof(NiFixedString),
        &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kCtlrID, sizeof(NiFixedString),
        &kString, sizeof(NiFixedString));
    NiMemcpy(&m_kInterpolatorID, sizeof(NiFixedString),
        &kString, sizeof(NiFixedString));

    // The handles may be in one of two states: invalid or valid.
    // Invalid handles are 0xffffffff. Since the previous block of
    // code has set the NiFixedStrings to their default, empty state,
    // we only need to set them if the handles are valid.

    if (uiAVObjectName != NiSequenceData::INVALID_INDEX)
    {
        m_kAVObjectName = pkPalette->GetBasePointer() + 
            uiAVObjectName;
    }

    if (uiPropertyType != NiSequenceData::INVALID_INDEX)
    {
        m_kPropertyType = pkPalette->GetBasePointer() + 
            uiPropertyType;
    }

    if (uiCtlrType != NiSequenceData::INVALID_INDEX)
    {
        m_kCtlrType = pkPalette->GetBasePointer() + 
            uiCtlrType;
    }

    if (uiCtlrID != NiSequenceData::INVALID_INDEX)
    {
        m_kCtlrID = pkPalette->GetBasePointer() + 
            uiCtlrID;
    }

    if (uiInterpolatorID != NiSequenceData::INVALID_INDEX)
    {
        m_kInterpolatorID = pkPalette->GetBasePointer() + 
            uiInterpolatorID;
    }
}
//---------------------------------------------------------------------------
bool NiSequenceData::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        NIASSERT(m_pspEvaluatorArray[ui]);
        m_pspEvaluatorArray[ui]->RegisterStreamables(kStream);
    }

    if (m_spTextKeys)
    {
        m_spTextKeys->RegisterStreamables(kStream);
    }

    kStream.RegisterFixedString(m_kName);
    kStream.RegisterFixedString(m_kAccumRootName);
    return true;
}
//---------------------------------------------------------------------------
void NiSequenceData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveFixedString(m_kName);

    NiStreamSaveBinary(kStream, m_uiNumEvaluators);

    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        kStream.SaveLinkID(m_pspEvaluatorArray[ui]);
    }

    kStream.SaveLinkID(m_spTextKeys);

    NiStreamSaveBinary(kStream, m_fDuration);
    NiStreamSaveEnum(kStream, m_eCycleType);
    NiStreamSaveBinary(kStream, m_fFrequency);

    kStream.SaveFixedString(m_kAccumRootName);
    NiStreamSaveBinary(kStream, m_uFlags);

    // Ignore m_kDefaultActivationCallbacks.
    // Ignore m_spDeprecatedStringPalette.
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSequenceData);
//---------------------------------------------------------------------------
void NiSequenceData::LoadBinary(NiStream& kStream)
{
    NIASSERT(kStream.GetFileVersion() >= NiStream::GetVersion(10, 2, 0, 0));

    // If needed, load the older NiControllerSequence object. This is how 
    // sequences were saved prior to the NiSequenceData object.
    if (kStream.GetFileVersion() < NiAnimationConstants::GetPoseVersion())
    {
        LoadPreSequenceDataBinary(kStream);
        return;
    }

    // Load the NiSequenceData object.

    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kName);

    if (kStream.GetFileVersion() < NiAnimationConstants::GetScratchPadVersion())
    {
        NIASSERT(!m_pkStreamedInterpArray);
        m_pkStreamedInterpArray = NiNew StreamedInterpArray();

        unsigned int uiArraySize;
        NiStreamLoadBinary(kStream, uiArraySize);
        m_pkStreamedInterpArray->m_uiArraySize = uiArraySize;

        unsigned int uiArrayGrowBy;
        NiStreamLoadBinary(kStream, uiArrayGrowBy);

        m_pkStreamedInterpArray->m_pspInterpArray = 
            NiNew NiInterpolatorPtr[uiArraySize];
        m_pkStreamedInterpArray->m_pkIDTagArray = 
            NiNew IDTag[uiArraySize];

        // The old string palette conversion code exists so 
        // that the older NiStringPalette/handle data can be
        // converted to NiFixedString's for the IDTag's.
        // Please see IDTag::LoadBinary for more information
        void* pvOldStringPaletteAddress = NULL;

        for (unsigned int ui = 0; ui < uiArraySize; ui++)
        {
            m_pkStreamedInterpArray->m_pspInterpArray[ui] = 
                (NiInterpolator*) kStream.ResolveLinkID();
            // Interp controllers were stored in versions before the conversion
            // from NiControllerSequence to NiSequenceData streamed objects.
            // Ignore the interp controllers here.
            if (kStream.GetFileVersion() < NiAnimationConstants::GetPoseVersion())
            {
                (NiInterpController*) kStream.ResolveLinkID();
            }
            m_pkStreamedInterpArray->m_pkIDTagArray[ui].LoadBinary(kStream,
                pvOldStringPaletteAddress);
        }
    }
    else
    {
        NiStreamLoadBinary(kStream, m_uiNumEvaluators);

        SetMaxEvaluators(m_uiNumEvaluators);
        for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
        {
            m_pspEvaluatorArray[ui] = (NiEvaluator*)kStream.ResolveLinkID();
        }
    }

    kStream.ReadLinkID();   // m_spTextKeys

    NiStreamLoadBinary(kStream, m_fDuration);
    NiStreamLoadEnum(kStream, m_eCycleType);
    NiStreamLoadBinary(kStream, m_fFrequency);

    kStream.LoadFixedString(m_kAccumRootName);
    NiStreamLoadBinary(kStream, m_uFlags);

    if (kStream.GetFileVersion() < NiAnimationConstants::GetScratchPadVersion())
    {
        // FLAG_HAS_SOLE_OWNER (0X8000) has been deprecated and removed.
        m_uFlags &= ~0X8000;
    }
}
//---------------------------------------------------------------------------
void NiSequenceData::LoadPreSequenceDataBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    // Conversion code.
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kName);
    }
    else
    {
        kStream.LoadFixedString(m_kName);
    }

    NIASSERT(!m_pkStreamedInterpArray);
    m_pkStreamedInterpArray = NiNew StreamedInterpArray();

    unsigned int uiArraySize;
    NiStreamLoadBinary(kStream, uiArraySize);
    m_pkStreamedInterpArray->m_uiArraySize = uiArraySize;

    unsigned int uiArrayGrowBy;
    NiStreamLoadBinary(kStream, uiArrayGrowBy);

    m_pkStreamedInterpArray->m_pspInterpArray = 
        NiNew NiInterpolatorPtr[uiArraySize];
    m_pkStreamedInterpArray->m_pkIDTagArray = 
        NiNew IDTag[uiArraySize];

    // The old string palette conversion code exists so 
    // that the older NiStringPalette/handle data can be
    // converted to NiFixedString's for the IDTag's.
    // Please see IDTag::LoadBinary for more information
    void* pvOldStringPaletteAddress = NULL;

    for (unsigned int ui = 0; ui < uiArraySize; ui++)
    {
        m_pkStreamedInterpArray->m_pspInterpArray[ui] = 
            (NiInterpolator*) kStream.ResolveLinkID();
        // Interp controllers were stored in versions before the conversion
        // from NiControllerSequence to NiSequenceData streamed objects.
        // Ignore the interp controllers here.
        if (kStream.GetFileVersion() < NiAnimationConstants::GetPoseVersion())
        {
            (NiInterpController*) kStream.ResolveLinkID();
        }
        m_pkStreamedInterpArray->m_pkIDTagArray[ui].LoadBinary(kStream,
            pvOldStringPaletteAddress);
    }

    float fSeqWeight;
    NiStreamLoadBinary(kStream, fSeqWeight);    // Ignore value.

    kStream.ReadLinkID();   // m_spTextKeys

    NiStreamLoadEnum(kStream, m_eCycleType);
    NiStreamLoadBinary(kStream, m_fFrequency);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 1))
    {
        // Phase is no longer supported and is thrown out.
        float fPhase;
        NiStreamLoadBinary(kStream, fPhase);
    }

    float fBeginKeyTime;
    NiStreamLoadBinary(kStream, fBeginKeyTime);
    NIASSERT(fBeginKeyTime == 0.0f);
    float fEndKeyTime;
    NiStreamLoadBinary(kStream, fEndKeyTime);
    m_fDuration = fEndKeyTime - fBeginKeyTime;

    kStream.ReadLinkID();   // pkOwner: Ignore value.
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kAccumRootName);      
    }
    else
    {
        kStream.LoadFixedString(m_kAccumRootName);
    }
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 8))
    {
        NiStreamLoadBinary(kStream, m_uFlags);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 113) &&
        kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        // The old string palette conversion code exists so 
        // that the older NiStringPalette/handle data can be
        // converted to NiFixedString's for the IDTag's.
        // Please see IDTag::LoadBinary for more information
        
        NiStringPalette* pkPalette = 
            (NiStringPalette*) kStream.ResolveLinkID();
        NIASSERT(pvOldStringPaletteAddress == pkPalette);
        m_spDeprecatedStringPalette = pkPalette;
    }
    else
    {
        m_spDeprecatedStringPalette = NULL;
    }
}
//---------------------------------------------------------------------------
void NiSequenceData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_spTextKeys = (NiTextKeyExtraData*) kStream.GetObjectFromLinkID();

    // The controller manager (i.e. owner) of an NiControllerSequence 
    // object was stored before the conversion to NiSequenceData 
    // streamed objects. Ignore the owner here.
    if (kStream.GetFileVersion() < NiAnimationConstants::GetPoseVersion())
    {
        kStream.GetObjectFromLinkID();
    }
}
//---------------------------------------------------------------------------
void NiSequenceData::PostLinkObject(NiStream& kStream)
{
    NiObject::PostLinkObject(kStream);

    if (m_spDeprecatedStringPalette)
    {
        // The old string palette conversion code exists so 
        // that the older NiStringPalette/handle data can be
        // converted to NiFixedString's for the IDTag's.
        // Please see IDTag::HandleDeprecatedExtraData for more information
        
        unsigned int uiArraySize = m_pkStreamedInterpArray->m_uiArraySize;
        for (unsigned int ui = 0; ui < uiArraySize; ui++)
        {
            m_pkStreamedInterpArray->m_pkIDTagArray[ui].
                HandleDeprecatedExtraData(m_spDeprecatedStringPalette, 
                kStream);
        }

        // The NiStringPalette is no longer needed.
        m_spDeprecatedStringPalette = NULL;
    }

    // Convert from interpolators to evaluators, if needed.
    if (kStream.GetFileVersion() < NiAnimationConstants::GetScratchPadVersion())
    {
        // Create the evaluator array based on the size of 
        // the interpolator list.
        NIASSERT(m_pkStreamedInterpArray);
        unsigned int uiArraySize = m_pkStreamedInterpArray->m_uiArraySize;
        if (uiArraySize > 0)
        {
            SetMaxEvaluators(uiArraySize);

            // Assign the streamed interpolator data to local variables.
            NiInterpolatorPtr* pspInterpArray = 
                m_pkStreamedInterpArray->m_pspInterpArray;
            IDTag* pkIDTagArray = m_pkStreamedInterpArray->m_pkIDTagArray;

            // Construct equivalent evaluators.
            m_uiNumEvaluators = 0;
            for (unsigned int ui = 0; ui < uiArraySize; ui++)
            {
                if (pspInterpArray[ui])
                {
                    // Convert the interpolator to an evaluator. 
                    // Request that the interpolator be collapsed first 
                    // so all posed interpolators can be converted to 
                    // const evaluators.
                    const IDTag* pkIDTag = &pkIDTagArray[ui];
                    NiEvaluator::IDTag kIDTag(pkIDTag->GetAVObjectName(),
                        pkIDTag->GetPropertyType(), pkIDTag->GetCtlrType(), 
                        pkIDTag->GetCtlrID(), pkIDTag->GetInterpolatorID());
                    m_pspEvaluatorArray[m_uiNumEvaluators] = 
                        NiInterpolatorConverter::Convert(
                        pspInterpArray[ui], kIDTag, true);
                    if (m_pspEvaluatorArray[m_uiNumEvaluators])
                    {
                        m_uiNumEvaluators++;
                    }
                    else
                    {
                        NIASSERT(false);
                    }
                }
            }

            // Sort the evaluators by channel type. This should improve runtime 
            // performance as this ordering more closely matches that found in 
            // the pose buffers.
            SortEvaluatorsByPBChannelType();
        }

        // Delete the streamed interpolator data now that 
        // equivalent evaluators have been created.
        NiDelete[] m_pkStreamedInterpArray->m_pspInterpArray;
        m_pkStreamedInterpArray->m_pspInterpArray = NULL;
        NiDelete[] m_pkStreamedInterpArray->m_pkIDTagArray;
        m_pkStreamedInterpArray->m_pkIDTagArray = NULL;
        NiDelete(m_pkStreamedInterpArray);
        m_pkStreamedInterpArray = NULL;
    }
}
//---------------------------------------------------------------------------
bool NiSequenceData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiSequenceData* pkDest = (NiSequenceData*) pkObject;

    if (m_kName != pkDest->m_kName)
    {
        return false;
    }

    if (m_uiNumEvaluators != pkDest->m_uiNumEvaluators)
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_uiNumEvaluators; ui++)
    {
        if (!m_pspEvaluatorArray[ui]->IsEqual(
            pkDest->m_pspEvaluatorArray[ui]))
        {
            return false;
        }
    }

    if ((m_spTextKeys && !pkDest->m_spTextKeys) ||
        (!m_spTextKeys && pkDest->m_spTextKeys) ||
        (m_spTextKeys && pkDest->m_spTextKeys &&
            !m_spTextKeys->IsEqual(pkDest->m_spTextKeys)))
    {
        return false;
    }

    if (m_fDuration != pkDest->m_fDuration ||
        m_eCycleType != pkDest->m_eCycleType ||
        m_fFrequency != pkDest->m_fFrequency)
    {
        return false;
    }

    if (m_kAccumRootName != pkDest->m_kAccumRootName)
    {
        return false;
    }
    
    if (m_uFlags != pkDest->m_uFlags)
    {
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiSequenceData::ActivationCallback::~ActivationCallback()
{
}
//---------------------------------------------------------------------------
