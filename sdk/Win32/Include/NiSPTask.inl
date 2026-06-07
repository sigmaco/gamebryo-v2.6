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
inline NiSPTask::Status NiSPTask::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetStatus(NiSPTask::Status eStatus)
{
    m_eStatus = eStatus;
    
    // The status is one of the things cleared by reset, so we need to
    // reset again when reset is requested.
    SetBit(false, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
inline void NiSPTask::LockStreams()
{
    /// Deadlocked here? Prepare has been called without a
    /// Finalize call to finish it. Or something else
    /// has deadlocked on access to the stream arrays.
    m_kStreamsLock.Lock();
}
//---------------------------------------------------------------------------
inline void NiSPTask::UnlockStreams()
{
    m_kStreamsLock.Unlock();
}
//---------------------------------------------------------------------------
inline void NiSPTask::AddInput(NiSPStream* pkInputStream)
{
    NIASSERT(m_kInputStreams.GetSize() < NiSPTaskImpl::MAX_INPUT_COUNT);
    NIASSERT(!pkInputStream->IsFixedInput() ||
        pkInputStream->GetDataSize() < NiSPTaskImpl::MAX_FIXED_INPUT_SIZE);

    LockStreams();

    m_kInputStreams.Add(pkInputStream);
    pkInputStream->AddInput(this);
    SetIsCached(false);

    NiSPDataStream* pkSource = pkInputStream->GetDataSource();
    if (pkSource)
        pkSource->EnableReadFlag();

    UnlockStreams();
}
//---------------------------------------------------------------------------
inline void NiSPTask::AddOutput(NiSPStream* pkOutputStream)
{
    LockStreams();

    m_kOutputStreams.Add(pkOutputStream);
    pkOutputStream->AddOutput(this);
    SetIsCached(false);

    NiSPDataStream* pkSource = pkOutputStream->GetDataSource();
    if (pkSource)
        pkSource->EnableWriteFlag();

    UnlockStreams();
}
//---------------------------------------------------------------------------
inline void NiSPTask::RemoveInput(NiSPStream* pkInputStream)
{
    LockStreams();

    NiInt32 uiLocation = m_kInputStreams.Find(pkInputStream);
    // Another thread may have removed it before we got the lock
    if (uiLocation != -1)
    {
        m_kInputStreams.RemoveAt(uiLocation);
        pkInputStream->RemoveInput(this);
        SetIsCached(false);
    }

    UnlockStreams();
}
//---------------------------------------------------------------------------
inline void NiSPTask::RemoveOutput(NiSPStream* pkOutputStream)
{
    LockStreams();

    NiInt32 uiLocation = m_kOutputStreams.Find(pkOutputStream);
    // Another thread may have removed it before we got the lock
    if (uiLocation != -1)
    {
        m_kOutputStreams.RemoveAt(uiLocation);
        pkOutputStream->RemoveInput(this);
        SetIsCached(false);
    }

    UnlockStreams();
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetKernel(NiSPKernel* pkKernel)
{
    m_pkKernel = pkKernel;
}
//---------------------------------------------------------------------------
inline NiSPKernel* NiSPTask::GetKernel() const
{
    return m_pkKernel;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetInputCount() const
{
    // The use of data from this call should be protected by the
    // m_kStreamsLock lock, lest it be made invalid by another thread.
    return m_kInputStreams.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetOutputCount() const
{
    // The use of data from this call should be protected by the
    // m_kStreamsLock lock, lest it be made invalid by another thread.
    return m_kOutputStreams.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetTotalCount() const
{
    // The use of data from this call should be protected by the
    // m_kStreamsLock lock, lest it be made invalid by another thread.
    return m_kInputStreams.GetSize() + m_kOutputStreams.GetSize();
}
//---------------------------------------------------------------------------
inline NiSPStream* NiSPTask::GetInputAt(NiUInt32 uiIndex) const
{
    // The use of data from this call should be protected by the
    // m_kStreamsLock lock, lest it be made invalid by another thread.
    NIASSERT(uiIndex < m_kInputStreams.GetSize());
    return m_kInputStreams.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiSPStream* NiSPTask::GetOutputAt(NiUInt32 uiIndex) const
{
    // The use of data from this call should be protected by the
    // m_kStreamsLock lock, lest it be made invalid by another thread.
    NIASSERT(uiIndex < m_kOutputStreams.GetSize());
    return m_kOutputStreams.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetSyncData(NiUInt64 uiSyncData)
{
    m_uiSyncData = uiSyncData;
    SetIsCached(false);

    // The sync data is one of the things cleared by reset, so we need to
    // reset again when reset is requested.
    SetBit(false, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
inline NiUInt64 NiSPTask::GetSyncData() const
{
    return m_uiSyncData;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsMarked() const
{
    return GetBit(FLAG_IS_MARKED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsMarked(bool bIsMarked)
{
    SetBit(bIsMarked, FLAG_IS_MARKED);

    // The marked flag is one of the things cleared by reset, so we need to
    // reset again when reset is requested.
    SetBit(false, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
inline unsigned short NiSPTask::GetStage() const
{
    return m_usStage;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetStage(unsigned short usStage)
{
    m_usStage = usStage;

    // The stage is one of the things cleared by reset, so we need to
    // reset again when reset is requested.
    SetBit(false, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsRoot() const
{
    return GetBit(FLAG_IS_ROOT);
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsLeaf() const
{
    return GetBit(FLAG_IS_LEAF);
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsSync() const
{
    return GetBit(FLAG_IS_SYNC);
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsAligned() const
{
    return GetBit(FLAG_IS_ALIGNED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetId(NiUInt32 uiId)
{
    m_uiTaskId = uiId;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetId() const
{
    return m_uiTaskId;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetOptimalBlockCount(NiUInt32 uiBlockCount)
{
    m_uiOptimalBlockCount = uiBlockCount;
    SetIsCached(false);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetOptimalBlockCount()
{
    return m_uiOptimalBlockCount;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetSignalId() const
{
    return m_uiSignalId;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetSignalId(NiUInt32 uiSignalId)
{
    m_uiSignalId = uiSignalId;
    
    // The signal ID is one of the things cleared by reset, so we need to
    // reset again when reset is requested.
    SetBit(false, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsCompacted() const
{
    return GetBit(FLAG_IS_COMPACTED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsCompacted(bool bIsCompacted)
{
    SetBit(bIsCompacted, FLAG_IS_COMPACTED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsCached(bool bCached)
{
    SetBit(bCached, FLAG_IS_CACHED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsCacheable(bool)
{
    // Deprecated
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsCacheable() const
{
    // Deprecated
    return true;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsCached() const
{
    return GetBit(FLAG_IS_CACHED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsDataDecompositionEnabled(bool bEnable)
{
    SetBit(bEnable, FLAG_IS_DATA_DECOMP_ENABLED);
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsDataDecompositionEnabled()
{
    return GetBit(FLAG_IS_DATA_DECOMP_ENABLED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetSignalType(NiSPSignalType eSignalType)
{
    m_eSignalType = eSignalType;
}
//---------------------------------------------------------------------------
inline NiSPSignalType NiSPTask::GetSignalType()
{
    return m_eSignalType;
}
//---------------------------------------------------------------------------
inline NiSPTaskImpl& NiSPTask::GetImpl()
{
    return m_kImpl;
}
//---------------------------------------------------------------------------
inline bool operator<(const NiSPTask& kLhs, const NiSPTask& kRhs)
{
    return kLhs.GetStage() < kRhs.GetStage();
}
//---------------------------------------------------------------------------
inline void NiSPTask::Reset(bool bForceReset)
{
    if (bForceReset)
        SetBit(false, FLAG_IS_CACHED);
    
    if (GetBit(FLAG_IS_CACHED) || GetBit(FLAG_IS_RESET))
    {
        return;
    }
    
    // Clear the NiSPTaskImpl before resetting the state variables because
    // it relies on some of these variables.
    m_kImpl.Clear();

    m_uiSignalId = 0;
    m_uiSliceSize = 0;
    m_eStatus = IDLE;
    SetBit(false, FLAG_IS_ROOT);
    SetBit(false, FLAG_IS_LEAF);
    SetBit(false, FLAG_IS_ALIGNED);
    SetBit(false, FLAG_IS_SYNC);
    SetBit(false, FLAG_IS_MARKED);
    m_eSignalType = NiSPWorkload::SIGNAL_NONE;
    m_uiSyncData = 0;
    m_usStage = 0;

    SetBit(true, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
inline void NiSPTask::Clear(bool bIgnoreCaching)
{
    Reset(bIgnoreCaching);

    NIASSERT(GetStatus() != RUNNING);

    // If the task can not be reused then clear streams
    if (bIgnoreCaching)
    {
        // Lock because we'll be modifying the set of input and output streams 
        // Infinite looping here? Prepare has been called without a
        // Finalize call to finish it. Or something else
        // has deadlocked on access to the stream arrays.
        LockStreams();
        
        // Release all streams
        m_kInputStreams.RemoveAllInput(this);
        m_kOutputStreams.RemoveAllOutput(this);        

        UnlockStreams();
        
        m_pkWorkflow = NULL;
        m_uFlags =
                FLAG_IS_DATA_DECOMP_ENABLED | FLAG_IS_ENABLED | FLAG_IS_RESET;
        m_pkKernel = NULL;
        m_uiOptimalBlockCount = AUTO_BLOCK_COUNT;
    }

}
//---------------------------------------------------------------------------
inline void NiSPTask::StreamPtrSet::RemoveAllInput(NiSPTask* pkTask)
{
    NiUInt32 uiSize = GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiSPStream* pkStream = GetAt(ui);
        pkStream->RemoveInput(pkTask);
    }
    NiTPrimitivePtrSet<NiSPStream*>::RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiSPTask::StreamPtrSet::RemoveAllOutput(NiSPTask* pkTask)
{
    NiUInt32 uiSize = GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiSPStream* pkStream = GetAt(ui);
        pkStream->RemoveOutput(pkTask);
    }
    NiTPrimitivePtrSet<NiSPStream*>::RemoveAll();
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsEnabled() const
{
    return GetBit(FLAG_IS_ENABLED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsEnabled(bool bIsEnabled)
{
    SetBit(bIsEnabled, FLAG_IS_ENABLED);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetWorkflow(NiSPWorkflow* pkWorkflow)
{
    m_pkWorkflow = pkWorkflow;
}
//---------------------------------------------------------------------------
inline NiSPWorkflow* NiSPTask::GetWorkflow() const
{
    return m_pkWorkflow;
}
//---------------------------------------------------------------------------
