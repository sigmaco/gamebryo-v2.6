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
#include "NiFloodgatePCH.h"
#include "NiSPTask.h"
#include "NiSPStream.h"
#include "NiSPKernel.h"
#include "NiSPWorkload.h"

//---------------------------------------------------------------------------
const NiUInt32 NiSPTask::AUTO_BLOCK_COUNT = 0xFFFFFFFF;
NiTObjectPool<NiSPTask>* NiSPTask::ms_pkTaskPool = NULL;
NiCriticalSection NiSPTask::ms_kTaskPoolLock;
//---------------------------------------------------------------------------
void NiSPTask::InitializePools(NiUInt32 uiTaskPoolSize)
{
    ms_kTaskPoolLock.Lock();
    NIASSERT(ms_pkTaskPool == NULL);
    ms_pkTaskPool = NiNew NiTObjectPool<NiSPTask>(uiTaskPoolSize);
    ms_kTaskPoolLock.Unlock();
}
//---------------------------------------------------------------------------
void NiSPTask::ShutdownPools()
{
    ms_kTaskPoolLock.Lock();
    NIASSERT(ms_pkTaskPool != NULL);
    ms_pkTaskPool->PurgeAllObjects();
    NiDelete ms_pkTaskPool;
    ms_pkTaskPool = NULL;
    ms_kTaskPoolLock.Unlock();
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiSPTask::GetNewTask(const NiUInt16 uiNumInputs,
    const NiUInt16 uiNumOutputs)
{
    ms_kTaskPoolLock.Lock();
    NiSPTask* pkTask = ms_pkTaskPool->GetFreeObject();
    ms_kTaskPoolLock.Unlock();

#if defined(_WII)
    // Reset the locked cache flag on the Wii in case it
    // was changed the last time this task was used.
    pkTask->GetImpl().SetUseLockedCache(true);
#endif

    NIASSERT(pkTask->m_pkWorkflow == NULL);
    NIASSERT(pkTask->m_uiSignalId == 0);
    NIASSERT(pkTask->m_pkKernel == NULL);
    NIASSERT(pkTask->m_uiSliceSize == 0);
    NIASSERT(pkTask->m_eStatus == IDLE);
    NIASSERT(pkTask->m_uiSyncData == 0);
    NIASSERT(pkTask->m_uiOptimalBlockCount == AUTO_BLOCK_COUNT);
    NIASSERT(pkTask->m_usStage == 0);
    NIASSERT(pkTask->m_uFlags ==
        (FLAG_IS_DATA_DECOMP_ENABLED | FLAG_IS_ENABLED | FLAG_IS_RESET));
    NIASSERT(pkTask->m_eSignalType == NiSPWorkload::SIGNAL_NONE)
    NIASSERT(pkTask->m_uiTaskId == (
        reinterpret_cast<NiUInt64>(pkTask) & 0xFFFFFF));
    NIASSERT(pkTask->GetInputCount() == 0);
    NIASSERT(pkTask->GetOutputCount() == 0);
    
    pkTask->m_kInputStreams.ReallocNoShrink(uiNumInputs);
    pkTask->m_kOutputStreams.ReallocNoShrink(uiNumOutputs);
    
    return pkTask; //NiUInt8
}
//---------------------------------------------------------------------------
void NiSPTask::DeleteThis()
{
    ms_kTaskPoolLock.Lock();
    
    // Only one task can be clearing at any given time.
    Clear(true);
    
    ms_pkTaskPool->ReleaseObject(this);
    ms_kTaskPoolLock.Unlock();
}
//---------------------------------------------------------------------------
NiSPTask::NiSPTask() : 
    m_pkWorkflow(NULL),
    m_uiTaskId(0),
    m_uiSignalId(0),
    m_pkKernel(NULL),
    m_uiSliceSize(0),
    m_eStatus(IDLE),
    m_uiSyncData(0),
    m_uiOptimalBlockCount(AUTO_BLOCK_COUNT),
    m_usStage(0),
    m_uFlags(FLAG_IS_DATA_DECOMP_ENABLED | FLAG_IS_ENABLED | FLAG_IS_RESET),
    m_eSignalType(NiSPWorkload::SIGNAL_NONE)
{
    SetId(static_cast<NiUInt32>(
        reinterpret_cast<NiUInt64>(this) & 0xFFFFFF));
}
//---------------------------------------------------------------------------
NiSPTask::~NiSPTask()
{
}
//---------------------------------------------------------------------------
void NiSPTask::Prepare()
{
    LockStreams();

    if (!IsEnabled())
        return;

    // Lock output streams
    NiUInt32 uiOutputStreamCount = m_kOutputStreams.GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
    {
        NiSPStream* pkStream = m_kOutputStreams.GetAt(uiIndex);
        pkStream->Lock();
    }

    // Lock input streams
    NiUInt32 uiInputStreamCount = m_kInputStreams.GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pkStream = m_kInputStreams.GetAt(uiIndex);
        pkStream->Lock();
    }
    
    if (IsCached())
    {
        return;
    }

    // Minimize memory use if told to do so.
    if (IsCompacted())
    {
        m_kInputStreams.Realloc();
        m_kOutputStreams.Realloc();
    }

    // If there are no streams then this is a sync task
    NiUInt32 uiCount = GetTotalCount();

    NIASSERT(m_pkKernel && "Error: NULL Kernel!");

#if defined(_PS3)
    // If a PPU Kernel was specified then turn this
    // into a PPU Notify signal task
    if (m_pkKernel->IsPPUKernel())
    {
        SetSignalType(NiSPWorkload::SIGNAL_PPU_TASK_NOTIFY);
        SetSyncData(GetId());
        SetIsDataDecompositionEnabled(false);
    }
#endif
    
    SetBit(GetSignalType() != NiSPWorkload::SIGNAL_NONE, FLAG_IS_SYNC);
    
    // Track how many streams are aligned
    NiUInt32 uiMisalignedStreamCount = uiCount;
    
    // Prepare output streams tracking alignment, and slice size
    // Output streams are prepare first so write locks on
    // data streams happen first. Subsequent read locks
    // will promote the locks to read / write locks
    NiUInt32 uiLeafStreamCount = uiOutputStreamCount;
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
    {
        NiSPStream* pStream = m_kOutputStreams.GetAt(uiIndex);
        pStream->Prepare();
        if (pStream->IsDataAligned())
        {
            uiMisalignedStreamCount--;
        }

        if (pStream->GetEffectiveInputSize() == 0)
        {
            uiLeafStreamCount--;
        }
        m_uiSliceSize += pStream->GetStride();
    }
    SetBit(!uiLeafStreamCount, FLAG_IS_LEAF);

    // Prepare input streams tracking alignment, and slice size
    // Lock input streams for read
    NiUInt32 uiRootStreamCount = uiInputStreamCount;
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pStream = m_kInputStreams.GetAt(uiIndex);
        pStream->Prepare();
        if (pStream->IsDataAligned())
        {
            uiMisalignedStreamCount--;
        }
        
        if (pStream->GetEffectiveOutputSize() == 0)
        {
            uiRootStreamCount--;
        }
        
        m_uiSliceSize += pStream->GetStride();
    }
    SetBit(!uiRootStreamCount, FLAG_IS_ROOT);

    // Save alignment
    SetBit(!uiMisalignedStreamCount, FLAG_IS_ALIGNED);
    
    // Initialize the task implementation
    m_kImpl.Initialize(this);

    // Set status to pending
    SetStatus(PENDING);

    // This task has been initialized now
    SetBit(false, FLAG_IS_RESET);
}
//---------------------------------------------------------------------------
void NiSPTask::Finalize()
{
    if (!IsEnabled())
    {
        UnlockStreams();
        return;
    }
    
    NiUInt32 uiInputStreamCount = m_kInputStreams.GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pkStream = m_kInputStreams.GetAt(uiIndex);
        pkStream->Unlock();
    }

    // Prepare output streams tracking alignment, and slice size
    NiUInt32 uiOutputStreamCount = m_kOutputStreams.GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
    {
        NiSPStream* pkStream = m_kOutputStreams.GetAt(uiIndex);
        pkStream->Unlock();
    }

    UnlockStreams();
}
//---------------------------------------------------------------------------
#if defined(_WII)
void NiSPTask::PrepareDataForDMAs()
{
    NiUInt32 uiInputStreamCount = m_kInputStreams.GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pkStream = m_kInputStreams.GetAt(uiIndex);
        if (pkStream->GetData())
        {
            DCStoreRangeNoSync(
                pkStream->GetData(),
                pkStream->GetDataSize());
        }
    }
    NiUInt32 uiOutputStreamCount = m_kOutputStreams.GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
    {
        NiSPStream* pkStream = m_kOutputStreams.GetAt(uiIndex);
        if (pkStream->GetData())
        {
            // Flush outputs since they will be invalid after
            // the DMAs back out into main memory.
            DCFlushRangeNoSync(
                pkStream->GetData(), 
                pkStream->GetDataSize());
        }
    }
    PPCSync();
}
#endif
//---------------------------------------------------------------------------