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
#include "NiSPWorkflow.h"
#include "NiStreamProcessor.h"
#include "NiSPAlgorithms.h"

//---------------------------------------------------------------------------
NiTObjectPool<NiSPWorkflow>* NiSPWorkflow::ms_pkWorkflowPool = NULL; 
NiCriticalSection NiSPWorkflow::ms_kWorkflowPoolLock;
//---------------------------------------------------------------------------
NiSPWorkflow::NiSPWorkflow() :
    m_uiId(0),
    m_iCurrentStage(-1),
    m_eStatus(IDLE),
    m_pkCompletionHandler(NULL),
    m_uiRefCount(0),
    m_iPriority(0)
{
}
//---------------------------------------------------------------------------
NiSPWorkflow::~NiSPWorkflow()
{
    Clear();
}
//---------------------------------------------------------------------------
void NiSPWorkflow::InitializePools(NiUInt32 uiWorkflowPoolSize)
{
    ms_kWorkflowPoolLock.Lock();
    NIASSERT(ms_pkWorkflowPool == NULL);
    ms_pkWorkflowPool = NiNew NiTObjectPool<NiSPWorkflow>(uiWorkflowPoolSize);
    ms_kWorkflowPoolLock.Unlock();
}
//---------------------------------------------------------------------------
void NiSPWorkflow::ShutdownPools()
{
    ms_kWorkflowPoolLock.Lock();
    NIASSERT(ms_pkWorkflowPool);
    NiDelete ms_pkWorkflowPool;
    ms_pkWorkflowPool = NULL;
    ms_kWorkflowPoolLock.Unlock();
}
//---------------------------------------------------------------------------
NiSPWorkflowPtr NiSPWorkflow::GetFreeWorkflow()
{
    NIASSERT(ms_pkWorkflowPool && "ms_pkWorkflowPool was NULL!");

    ms_kWorkflowPoolLock.Lock();
    NiSPWorkflowPtr spWorkflow = ms_pkWorkflowPool->GetFreeObject();
    ms_kWorkflowPoolLock.Unlock();

    NIASSERT(spWorkflow && "spWorkflow was NULL!");
    if (spWorkflow)
    {
        spWorkflow->SetStatus(NiSPWorkflow::IDLE);
    }
    return spWorkflow;
}
//---------------------------------------------------------------------------
void NiSPWorkflow::ReleaseWorkflow()
{
    Clear();
    ms_kWorkflowPoolLock.Lock();
    ms_pkWorkflowPool->ReleaseObject(this);
    ms_kWorkflowPoolLock.Unlock();
}
//---------------------------------------------------------------------------
void NiSPWorkflow::Add(NiSPTask* pkTask)
{
    NIASSERT(m_eStatus != RUNNING);
    pkTask->SetWorkflow(this);
    m_kTasks.Add(pkTask);
}
//---------------------------------------------------------------------------
NiSPTask* NiSPWorkflow::AddNewTask(const NiUInt16 uiNumInputs,
     const NiUInt16 uiNumOutputs, const bool bIsSignalTask)
{
    NIASSERT(m_eStatus != RUNNING);

    NiSPTaskPtr spTask = NiSPTask::GetNewTask(uiNumInputs, uiNumOutputs);

    NIASSERT(spTask);

    if (bIsSignalTask)
    {        
        spTask->SetSignalType(NiSPWorkload::SIGNAL_COMPLETION);
        spTask->SetKernel(NiSPTaskImpl::GetSignalKernel());
        spTask->SetSyncData((NiUInt64)GetAtomicUpdateCompleteAddress());
    }

    NiSPWorkflow::Add(spTask);

    return spTask;
}
//---------------------------------------------------------------------------
void NiSPWorkflow::ReplaceAt(NiUInt32 uiTaskIdx, NiSPTask* pkTask)
{
    NIASSERT(m_eStatus != RUNNING);
    m_kTasks.ReplaceAt(uiTaskIdx, pkTask);
}
//---------------------------------------------------------------------------
void NiSPWorkflow::RemoveAt(NiUInt32 uiTaskIdx)
{
    NIASSERT(m_eStatus != RUNNING);
    m_kTasks.RemoveAt(uiTaskIdx);
}
//---------------------------------------------------------------------------
void NiSPWorkflow::SortTasksByStage()
{
    NIASSERT(m_eStatus != RUNNING);
    NiSPAlgorithms::QuickSortPointers< NiTObjectPtrSet<NiSPTaskPtr>,
        NiSPTaskPtr>(&m_kTasks, 0, m_kTasks.GetSize() - 1);
}
//---------------------------------------------------------------------------
bool NiSPWorkflow::Prepare()
{
#if !defined(_WII)
    if (!m_kWorkflowImpl.Prepare())
        return false;
#endif

    // Find completion signal task
    NiUInt32 uiTaskCount = GetSize();
    NiSPTask* pkSignalTask = NULL;
    bool bFoundSignalTask = false;
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        // If this is a sync task then set this workflow's id to 
        // that of its signal task so that the manager can detect 
        // its completion
        NiSPTask* pkTask = GetAt(uiIndex);
        if (pkTask->GetSignalType() == NiSPWorkload::SIGNAL_COMPLETION)
        {
            pkSignalTask = pkTask;
            bFoundSignalTask = true;
            break;
        }
    }

    // If there was no signal kernel then add one manually
    if (!bFoundSignalTask)
    {
        pkSignalTask = AddNewTask(0, 0, true);
    }
    m_uiId = pkSignalTask->GetId();

    // Prepare contained tasks
    uiTaskCount = GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        pkTask->SetSignalId(m_uiId);
        pkTask->Prepare();
    }
    
    // Ensure that the workflow has been given an id
    NIASSERT(m_uiId > 0 && "Workflow signal id is invalid!");

    return true;
}
//---------------------------------------------------------------------------
#if !defined(_PS3)
void NiSPWorkflow::ExecuteSerial()
{
    NIASSERT(GetStatus() == NiSPWorkflow::RUNNING);

    // Verify there are no jobs in the pending job list queue. This queue
    // should go unused while running with a serial execution model.
    NIASSERT(GetNumPendingJobLists() == 0);

    NiUInt32 uiTaskCount = GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        if (!pkTask->IsEnabled())
            continue;

        NiSPTaskImpl& kTaskImpl = pkTask->GetImpl();
#if !defined(_WII)
        NiUInt32 uiListCount = kTaskImpl.GetSize();
        for (NiUInt32 i = 0; i < uiListCount; i++)
        {
            NiSPJobList* pkJobList = kTaskImpl.GetAt(i);
            NiUInt32 uiJobCount = pkJobList->GetJobCount();

            for (NiUInt32 j = 0; j < uiJobCount; j++)
            {
                NiSPWorkload* kWorkload = pkJobList->GetWorkload(j);
                kWorkload->Preload();

                NiSPKernel* pkKernel = kWorkload->GetKernel();
                pkKernel->Execute(*kWorkload);
            }
        }
#else 
        if (kTaskImpl.GetUseLockedCache())
        {
            // Make sure all input streams have made it out to main
            // memory since we will be DMAing from there.
            // Invalidate the cache on the output streams since it 
            // will be out of date (also we don't want any data hanging
            // around in the cache to overwrite the results we DMA into
            // main memory).  Also, store the end edge of output
            // streams since they will be DMA'd in to the LC as well.
            pkTask->PrepareDataForDMAs();

            // Process all workloads in the job list.
            NiUInt32 uiListCount = kTaskImpl.GetSize();
            for (NiUInt32 i = 0; i < uiListCount; i++)
            {
                NiSPJobList* pkJobList = kTaskImpl.GetAt(i);
                NiUInt32 uiJobCount = pkJobList->GetJobCount();

                // All Floodgate workloads on the Wii are created so that, if possible,  
                // subsequent workloads are not contiguous in main memory.  This allows us 
                // to DMA the edges of a subsequent workload's outputs from main memory into 
                // the locked cache (during Preload) without worrying that the results from 
                // the previous workload have not yet made it into main memory.  If there 
                // are more than four workloads, this works in all cases.  With less than 
                // four workloads, subsequent workloads may have to be in contiguous memory, 
                // so we must handle those cases separately. 
                if (uiJobCount > 4)
                {
                    // With more than four workloads, we can always preload the next 
                    // workload prior to completing the DMA out for the current workload.

                    // Start the DMA for the first workload.
                    NiSPWorkload* pkWorkload = pkJobList->GetWorkload(0);
                    pkWorkload->Preload();

                    // Avoid DMAing too far ahead by terminating the loop one iteration early.
                    uiJobCount -= 1;
                    NiUInt32 uiWaitLen = 0;
                    for (NiUInt32 j = 0; j < uiJobCount; j++)
                    {
                        // Start the DMA for the next workload.
                        NiSPWorkload* pkNextWorkload = pkJobList->GetWorkload(j+1);
                        uiWaitLen += pkNextWorkload->Preload();

                        // Wait for all DMAs other than the one we just did to complete.
                        LCQueueWait(uiWaitLen);

                        // Execute the kernel on the current workload.
                        NiSPKernel* pkKernel = pkWorkload->GetKernel();
                        pkKernel->Execute(*pkWorkload);

                        // DMA the results back into main memory.
                        uiWaitLen = pkWorkload->DMAOut();

                        // Get ready for the next workload.
                        pkWorkload = pkNextWorkload;
                    }

                    // Wait for all DMAs to complete other than the last DMA out to main memory.
                    LCQueueWait(uiWaitLen);

                    // Execute the kernel on the final workload.
                    NiSPKernel* pkKernel = pkWorkload->GetKernel();
                    pkKernel->Execute(*pkWorkload);

                    // DMA the results back into main memory.
                    pkWorkload->DMAOut();
                }
                else if (uiJobCount == 1)
                {
                    // With just one workload, we must wait for the DMAs
                    // to complete before executing the kernel.
                    NiSPWorkload* pkWorkload = pkJobList->GetWorkload(0);
                    pkWorkload->Preload();
                    LCQueueWait(0);
                    NiSPKernel* pkKernel = pkWorkload->GetKernel();
                    pkKernel->Execute(*pkWorkload);
                    pkWorkload->DMAOut();
                }
                else if (uiJobCount == 2)
                {
                    // With two workloads, we cannot preload the second workload prior to 
                    // the first workload's results being in main memory since the first 
                    // workload may contain outputs that overlap 32-byte boundaries with 
                    // outputs on the second workload. 
                    NiSPWorkload* pkWorkload0 = pkJobList->GetWorkload(0);
                    pkWorkload0->Preload();
                    LCQueueWait(0);
                    pkWorkload0->GetKernel()->Execute(*pkWorkload0);
                    pkWorkload0->DMAOut();

                    // Wait for the first workload's DMA out to complete before
                    // starting the preload for the second workload.
                    LCQueueWait(0);

                    NiSPWorkload* pkWorkload1 = pkJobList->GetWorkload(1);
                    pkWorkload1->Preload();
                    LCQueueWait(0);
                    pkWorkload1->GetKernel()->Execute(*pkWorkload1);
                    pkWorkload1->DMAOut();
                }
                else if (uiJobCount == 3)
                {
                    // With three workloads, we can preload the second while we
                    // are working on the first, but the third must wait for the 
                    // second to complete.

                    // Start the DMAs for the first and second workloads.
                    NiSPWorkload* pkWorkload0 = pkJobList->GetWorkload(0);
                    pkWorkload0->Preload();
                    NiSPWorkload* pkWorkload1 = pkJobList->GetWorkload(1);
                    NiUInt32 uiWaitLen = pkWorkload1->Preload();

                    // Wait for the first workload's preload to complete, execute it,
                    // and start the DMA back into main memory.
                    LCQueueWait(uiWaitLen);
                    pkWorkload0->GetKernel()->Execute(*pkWorkload0);
                    pkWorkload0->DMAOut();

                    // Wait for the second workload's preload to complete, execute it,
                    // and start the DMA back into main memory.
                    LCQueueWait(0);
                    pkWorkload1->GetKernel()->Execute(*pkWorkload1);
                    pkWorkload1->DMAOut();

                    // Now we have to wait until the second workload is finished
                    // DMAing back into main memory since the third workload
                    // might have outputs that overlap 32-byte boundaries with
                    // outputs on the second workload.
                    NiSPWorkload* pkWorkload2 = pkJobList->GetWorkload(2);
                    LCQueueWait(0);

                    // Now we are safe to work on the third workload.
                    pkWorkload2->Preload();
                    LCQueueWait(0);
                    pkWorkload2->GetKernel()->Execute(*pkWorkload2);
                    pkWorkload2->DMAOut();
                }
                else if (uiJobCount == 4)
                {
                    // With four workloads, we can preload the second while we
                    // are working on the first, and the fourth while we are working
                    // on the third, but the third must wait for the second to complete.

                    // Start the DMAs for the first and second workloads.
                    NiSPWorkload* pkWorkload0 = pkJobList->GetWorkload(0);
                    pkWorkload0->Preload();
                    NiSPWorkload* pkWorkload1 = pkJobList->GetWorkload(1);
                    NiUInt32 uiWaitLen = pkWorkload1->Preload();

                    // Wait for the first workload's preload to complete, execute it,
                    // and start the DMA back into main memory.
                    LCQueueWait(uiWaitLen);
                    pkWorkload0->GetKernel()->Execute(*pkWorkload0);
                    pkWorkload0->DMAOut();

                    // Wait for the second workload's preload to complete, execute it,
                    // and start the DMA back into main memory.
                    LCQueueWait(0);
                    pkWorkload1->GetKernel()->Execute(*pkWorkload1);
                    pkWorkload1->DMAOut();

                    // Now we have to wait until the second workload is finished
                    // DMAing back into main memory since the third workload
                    // might have outputs that overlap 32-byte boundaries with
                    // outputs on the second workload.  However, we can also start
                    // the DMA in for the fourth workload at the same time.
                    NiSPWorkload* pkWorkload2 = pkJobList->GetWorkload(2);
                    pkWorkload2->Preload();
                    NiSPWorkload* pkWorkload3 = pkJobList->GetWorkload(3);
                    uiWaitLen = pkWorkload3->Preload();

                    // Wait for the third workload's preload to complete, execute it,
                    // and start the DMA back into main memory.
                    LCQueueWait(uiWaitLen);
                    pkWorkload2->GetKernel()->Execute(*pkWorkload2);
                    pkWorkload2->DMAOut();

                    // Wait for the fourth workload's preload to complete, execute it,
                    // and start the DMA back into main memory.
                    LCQueueWait(0);
                    pkWorkload3->GetKernel()->Execute(*pkWorkload3);
                    pkWorkload3->DMAOut();
                }

                // Wait for the last DMA out into main memory to complete
                // before continuing.
                LCQueueWait(0);
            }
        }
        else
        {
            // Don't use the locked cache, just do a standard loop & execute.
            NiUInt32 uiListCount = kTaskImpl.GetSize();
            for (NiUInt32 i = 0; i < uiListCount; i++)
            {
                NiSPJobList* pkJobList = kTaskImpl.GetAt(i);
                NiUInt32 uiJobCount = pkJobList->GetJobCount();
                for (NiUInt32 j = 0; j < uiJobCount; j++)
                {
                    NiSPWorkload* kWorkload = pkJobList->GetWorkload(j);
                    NiSPKernel* pkKernel = kWorkload->GetKernel();
                    pkKernel->Execute(*kWorkload);
                }
            }
        }
#endif // !defined(_WII)
    }

    ExecutionComplete();
    SetStatus(NiSPWorkflow::COMPLETED);
}
#endif
//---------------------------------------------------------------------------
void NiSPWorkflow::Reset()
{
    NIASSERT(m_eStatus != RUNNING);

    // Clear all tasks...
    NiInt32 uiIndex = GetSize();
    while (--uiIndex >= 0)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        pkTask->Reset();
    }

    // Reset status
    m_iCurrentStage = -1;
    ClearPendingJobLists();
    SetStatus(NiSPWorkflow::IDLE);
    ResetAtomicUpdateComplete();
}
//---------------------------------------------------------------------------
void NiSPWorkflow::Clear()
{
    NIASSERT(m_eStatus != RUNNING);

    // Clear all tasks...
    NiUInt32 uiIndex = GetSize();
    while (uiIndex)
    {
        --uiIndex;
        RemoveAt(uiIndex);
    }

    m_iCurrentStage = -1;
    ClearPendingJobLists();
    m_uiId = 0;
}
//---------------------------------------------------------------------------
NiSPTask* NiSPWorkflow::FindTask(NiUInt32 uiTaskId)
{
    // Find completion signal task
    NiUInt32 uiTaskCount = GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        if (pkTask->GetId() == uiTaskId)
        {
            return pkTask;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
#if defined(_PS3)
    void NiSPWorkflow::HandleSignal(NiUInt32 uiSignal)
#else
    void NiSPWorkflow::HandleSignal(NiUInt32)
#endif
{
#if defined(_PS3)
    NiSPTask* pkTask = FindTask(uiSignal);
    if (pkTask)
    {
        m_kWorkflowImpl.Execute(pkTask);
    }
#endif
}
//---------------------------------------------------------------------------
void NiSPWorkflow::ExecutionComplete()
{
#if !defined(_WII)
    // Release workflow implementation class
    m_kWorkflowImpl.Release();
#endif

    // Unlock any data streams
    NiUInt32 uiTaskCount = GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        pkTask->Finalize();
    }
}
//---------------------------------------------------------------------------
void NiSPWorkflow::SetPriority(int iPriority)
{
    m_iPriority = iPriority;
}
//---------------------------------------------------------------------------
int NiSPWorkflow::GetPriority() const
{
    return m_iPriority;
}
//---------------------------------------------------------------------------
