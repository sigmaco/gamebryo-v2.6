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


//---------------------------------------------------------------------------
inline NiAVObjectPalette* NiControllerManager::GetObjectPalette() const
{
    return m_spObjectPalette;
}
//---------------------------------------------------------------------------
inline NiPoseBinding* NiControllerManager::GetPoseBinding() const
{
    return m_spPoseBinding;
}
//---------------------------------------------------------------------------
inline NiPoseBlender* NiControllerManager::GetPoseBlender() const
{
    return m_spPoseBlender;
}
//---------------------------------------------------------------------------
inline NiMultiTargetPoseHandler* 
    NiControllerManager::GetMultiTargetPoseHandler() const
{
    return m_spMTPoseHandler;
}
//---------------------------------------------------------------------------
inline NiSkinningLODController* 
    NiControllerManager::GetSkinningLODController() const
{
    return m_pkLODController;
}
//---------------------------------------------------------------------------
inline unsigned int NiControllerManager::GetSequenceDataCount() const
{
    return m_kSeqDataArray.GetSize();
}
//---------------------------------------------------------------------------
inline NiSequenceData* NiControllerManager::GetSequenceDataByName(
    const NiFixedString& kName) const
{
    NIASSERT(kName.Exists());

    NiSequenceData* pkSeqData;
    if (m_kSeqDataMap.GetAt(kName, pkSeqData))
    {
        return pkSeqData;
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiSequenceData* NiControllerManager::GetSequenceDataAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kSeqDataArray.GetSize());
    return m_kSeqDataArray.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::ActivateSequence(
    const NiFixedString& kSeqDataName,
    int iPriority, float fWeight, float fEaseInTime, 
    NiControllerSequence* pkTimeSyncSeq, 
    float fFrequency, float fStartFrame)
{
    NIASSERT(kSeqDataName.Exists());

    NiSequenceData* pkSeqData;
    if (m_kSeqDataMap.GetAt(kSeqDataName, pkSeqData))
    {
        return ActivateSequence(pkSeqData, iPriority, fWeight, 
            fEaseInTime, pkTimeSyncSeq, fFrequency, fStartFrame, false, false);
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::ActivateSequence(
    NiSequenceData* pkSeqData, 
    int iPriority, float fWeight, float fEaseInTime, 
    NiControllerSequence* pkTimeSyncSeq, 
    float fFrequency, float fStartFrame)
{
    return ActivateSequence(pkSeqData, iPriority, fWeight, 
        fEaseInTime, pkTimeSyncSeq, fFrequency, fStartFrame, false, false);
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::DeactivateSequence(
    const NiFixedString& kSequenceName, float fEaseOutTime)
{
    NIASSERT(kSequenceName.Exists());

    NiControllerSequence* pkControllerSequence = GetSequenceByName(
        kSequenceName);
    if (pkControllerSequence)
    {
        return DeactivateSequence(pkControllerSequence, fEaseOutTime);
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::DeactivateSequence(
    NiControllerSequence* pkSequence, float fEaseOutTime)
{
    NIASSERT(pkSequence && pkSequence->GetOwner() == this);
    return pkSequence->Deactivate(fEaseOutTime, false, false, true);
}
//---------------------------------------------------------------------------
inline void NiControllerManager::DeactivateAll(float fEaseOutTime)
{
    for (unsigned int ui = 0; ui < m_kSequenceArray.GetSize(); ui++)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui);
        if (pkSequence)
        {
            DeactivateSequence(pkSequence, fEaseOutTime);
        }
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiControllerManager::GetSequenceCount() const
{
    return m_kSequenceArray.GetSize();
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::GetSequenceByName(
    const NiFixedString& kName) const
{
    NIASSERT(kName.Exists());

    NiControllerSequence* pkControllerSequence;
    unsigned int uiNumSequences = m_kSequenceArray.GetSize();
    for (unsigned int uiIndex = 0; uiIndex < uiNumSequences; uiIndex++)
    {
        pkControllerSequence = m_kSequenceArray.GetAt(uiIndex);
        if (pkControllerSequence != NULL)
        {
            if (pkControllerSequence->GetName() == kName)
            {
                return pkControllerSequence;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::GetSequenceBySequenceData(
        const NiSequenceData* pkSeqData) const
{
    NIASSERT(pkSeqData);

    NiControllerSequence* pkControllerSequence;
    unsigned int uiNumSequences = m_kSequenceArray.GetSize();
    for (unsigned int uiIndex = 0; uiIndex < uiNumSequences; uiIndex++)
    {
        pkControllerSequence = m_kSequenceArray.GetAt(uiIndex);
        if (pkControllerSequence != NULL)
        {
            if (pkControllerSequence->GetSequenceData() == pkSeqData)
            {
                return pkControllerSequence;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::GetSequenceAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kSequenceArray.GetSize());
    return m_kSequenceArray.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiControllerManager::RemoveAllInactiveSequences(
    bool bIgnoreRefCount)
{
    for (unsigned int ui = m_kSequenceArray.GetSize(); ui > 0; ui--)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui - 1);
        if (pkSequence && pkSequence->GetState() == INACTIVE &&
            (bIgnoreRefCount || pkSequence->GetRefCount() == 1))
        {
            RemoveSequence(pkSequence);
        }
    }
}
//---------------------------------------------------------------------------
inline void NiControllerManager::RemoveAllSequences()
{
    for (unsigned int ui = m_kSequenceArray.GetSize(); ui > 0; ui--)
    {
        NiControllerSequence* pkSequence = m_kSequenceArray.GetAt(ui - 1);
        if (pkSequence)
        {
            RemoveSequence(pkSequence);
        }
    }
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::SetSequenceWeight(
    const NiFixedString& kSequenceName, float fWeight)
{
    NIASSERT(kSequenceName.Exists());

    NiControllerSequence* pkControllerSequence = GetSequenceByName(
        kSequenceName);
    if (pkControllerSequence)
    {
        return SetSequenceWeight(pkControllerSequence, fWeight);
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::SetSequenceWeight(
    NiControllerSequence* pkSequence, float fWeight)
{
    NIASSERT(pkSequence && pkSequence->GetOwner() == this);
    pkSequence->SetWeight(fWeight);
    return true;
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::BlendFromSequence(
    NiControllerSequence* pkSourceSequence,
    NiSequenceData* pkDestSeqData, float fDuration,
    const NiFixedString& kDestTextKey, int iPriority, float fSourceWeight,
    float fDestWeight, NiControllerSequence* pkDestTimeSyncSeq, 
    float fDestFrequency)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this);
    NIASSERT(pkDestSeqData);

    if (pkSourceSequence->GetState() == INACTIVE)
    {
        return NULL;
    }

    return pkSourceSequence->StartBlend(pkDestSeqData, fDuration,
        kDestTextKey, iPriority, fSourceWeight, fDestWeight, 
        pkDestTimeSyncSeq, fDestFrequency);
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::BlendFromSequence(
    NiControllerSequence* pkSourceSequence,
    NiSequenceData* pkDestSeqData, float fDuration, 
    float fDestFrame, int iPriority, float fSourceWeight, 
    float fDestWeight, NiControllerSequence* pkDestTimeSyncSeq, 
    float fDestFrequency)
{
    NIASSERT(pkSourceSequence && pkSourceSequence->GetOwner() == this);
    NIASSERT(pkDestSeqData);

    if (pkSourceSequence->GetState() == INACTIVE)
    {
        return NULL;
    }

    return pkSourceSequence->StartBlend(pkDestSeqData, fDuration, 
        fDestFrame, iPriority, fSourceWeight, fDestWeight, 
        pkDestTimeSyncSeq, fDestFrequency);
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::GetCumulativeAnimations() const
{
    return m_bCumulative;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiControllerManager::GetAccumRootName() const
{
    NIASSERT(m_spMTPoseHandler);
    return m_spMTPoseHandler->GetAccumRootName();
}
//---------------------------------------------------------------------------
inline NiAVObject* NiControllerManager::GetAccumRoot() const
{
    NIASSERT(m_spMTPoseHandler);
    return m_spMTPoseHandler->GetAccumRoot();
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::GetAccumulatedTransform(
    NiQuatTransform& kTransform) const
{
    NIASSERT(m_spMTPoseHandler);
    if (m_bCumulative)
    {
        kTransform = m_spMTPoseHandler->GetAccumulatedTransform();
    }
    return m_bCumulative;
}
//---------------------------------------------------------------------------
inline void NiControllerManager::SetAccumulatedTransform(
    const NiQuatTransform& kTransform)
{
    NIASSERT(m_spMTPoseHandler);
    if (m_bCumulative)
    {
        m_spMTPoseHandler->SetAccumulatedTransform(kTransform);
    }
}
//---------------------------------------------------------------------------
inline void NiControllerManager::ClearCumulativeAnimations()
{
    NIASSERT(m_spMTPoseHandler);
    if (m_bCumulative)
    {
        m_spMTPoseHandler->ClearAccumulatedTransform();
    }
}
//---------------------------------------------------------------------------
inline unsigned short NiControllerManager::GetNumRecycledSequences() const
{
    return (unsigned short)m_kRecycledSequenceArray.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned short NiControllerManager::GetMaxRecycledSequences() const
{
    return m_usMaxRecycledSequences;
}
//---------------------------------------------------------------------------
inline void NiControllerManager::RemoveAllRecycledSequences()
{
    // Flush all recycled sequences by temporarily setting the maximum to 0.
    unsigned short usMaxRecycledSequences = m_usMaxRecycledSequences;
    SetMaxRecycledSequences(0);
    SetMaxRecycledSequences(usMaxRecycledSequences);
}
//---------------------------------------------------------------------------
inline NiControllerSequence* NiControllerManager::ActivateSequence(
    NiSequenceData* pkSeqData, int iPriority, float fWeight, 
    float fEaseInTime, NiControllerSequence* pkTimeSyncSeq, 
    float fFrequency, float fStartFrame, bool bTransition, 
    bool bHoldTransitionFrame)
{
    NIASSERT(pkSeqData);
    // Create and activate new sequence.
    return NiControllerSequence::Activate(this, pkSeqData, iPriority, 
        fWeight, fEaseInTime, pkTimeSyncSeq, fFrequency, fStartFrame, 
        bTransition, bHoldTransitionFrame);
}
//---------------------------------------------------------------------------
inline unsigned int NiControllerManager::GetNextActivationID()
{
    m_uiActivationID++;
    return m_uiActivationID;
}
//---------------------------------------------------------------------------
inline void NiControllerManager::AddSequence(
    NiControllerSequence* pkSequence)
{
    NIASSERT(pkSequence && !pkSequence->GetOwner());

    m_kSequenceArray.AddFirstEmpty(pkSequence);
    pkSequence->SetOwner(this);
}
//---------------------------------------------------------------------------
inline void NiControllerManager::RemoveSequence(
    NiControllerSequence* pkSequence)
{
    NIASSERT(pkSequence && pkSequence->GetOwner() == this);

    // Release the sequence if held by the multitarget pose handler.
    NIASSERT(m_spMTPoseHandler);
    m_spMTPoseHandler->OnRemoveSequence(pkSequence);

    // Try to recycle the sequence before removing it from the array.
    // Don't recycle the sequence if there's an outstanding ref count.
    if (pkSequence->GetRefCount() == 1)
    {
        pkSequence->PrepareForRecycling();
        RecycleSequence(pkSequence);
    }

    // Obtain the source sequence data before removing the sequence.
    NiSequenceDataPtr spSeqData = pkSequence->GetSequenceData();
    pkSequence->SetSequenceData(NULL);

    // Remove the sequence from the array. This may release the last ref count.
    pkSequence->SetOwner(NULL);
    m_kSequenceArray.Remove(pkSequence);

    // Remove the source sequence data, if it's a temporary pose.
    if (spSeqData && spSeqData->GetTempPose())
    {
        RemoveSequenceData(spSeqData);
    }
}
//---------------------------------------------------------------------------
inline bool NiControllerManager::IsKnownSequenceData(
    const NiSequenceData* pkSeqData) const
{
    // This function returns true if pkSeqData was previously added to this
    // manager's NiSequenceData list, and false otherwise. 

    NIASSERT(pkSeqData);

    // Perform the faster look-up first.
    if (GetSequenceDataByName(pkSeqData->GetName()) == pkSeqData)
    {
        return true;
    }

    // Try the slower look-up, in case there are duplicate names.
    unsigned int uiNumSeqDatas = m_kSeqDataArray.GetSize();
    for (unsigned int ui = 0; ui < uiNumSeqDatas; ui++)
    {
        if (m_kSeqDataArray.GetAt(ui) == pkSeqData)
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
inline unsigned short NiControllerManager::GetMaxEvaluators() const
{
    return m_usMaxEvaluators;
}
//---------------------------------------------------------------------------
inline unsigned short NiControllerManager::GetMaxUniqueBasisDatas() 
    const
{
    return m_usMaxUniqueBasisDatas;
}
//---------------------------------------------------------------------------
inline unsigned int NiControllerManager::GetMaxScratchPadBufferSize() const
{
    return m_uiMaxScratchPadBufferSize;
}
//---------------------------------------------------------------------------
