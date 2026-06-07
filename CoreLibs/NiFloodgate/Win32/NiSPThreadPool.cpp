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
#include "NiSPThreadPool.h"
#include "NiSPWorkflow.h"
#include "NiSPTask.h"
#include "NiSPAnalyzer.h"
#define MAX_QUEUESIZE 128

NiSPThreadPool::AssignWorkerThreadAffinity 
    NiSPThreadPool::ms_pfnAssignWorkerThreadAffinityFunc = 
    NiSPThreadPool::AssignWorkerThreadAffinityDefaultFunc;

//---------------------------------------------------------------------------
void NiSPThreadPool::AssignWorkerThreadAffinityDefaultFunc(
    NiUInt32, NiUInt32,
    NiProcessorAffinity& kProcessorAffinity)
{    
    kProcessorAffinity.SetIdealProcessor(
        NiProcessorAffinity::PROCESSOR_DONT_CARE);

    kProcessorAffinity.SetAffinityMask(
        (unsigned int)NiProcessorAffinity::PROCESSOR_DONT_CARE);
}
//---------------------------------------------------------------------------
void NiSPThreadPool::Initialize()
{
    m_apkWorkerThreads = NiNew ThreadArray(m_uiMaxWorkerThreads);
    NIASSERT(m_apkWorkerThreads);

    m_apkWorkerProcs = NiNew WorkerProcArray(m_uiMaxWorkerThreads);
    NIASSERT(m_apkWorkerProcs);

    m_pCurrentWorkflowList = NiAlloc(NiUInt32, m_uiMaxWorkerThreads);
    m_pCurrentStageList    = NiAlloc(NiUInt32, m_uiMaxWorkerThreads);

    // Create update threads and resume them.
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        WorkerProc* pkWorkerProc = NiNew WorkerProc();
        pkWorkerProc->SetPool(this);
        pkWorkerProc->SetThreadID(i);
        m_apkWorkerProcs->SetAt(i, pkWorkerProc);

        NiThread* pkThread = NiThread::Create(pkWorkerProc);
        pkThread->SetPriority(NiThread::HIGHEST);

        NiProcessorAffinity kAffinity;
        ms_pfnAssignWorkerThreadAffinityFunc(
            i, m_uiMaxWorkerThreads, kAffinity);

        pkThread->Resume();
        m_apkWorkerThreads->Add(pkThread);
    }
}
//---------------------------------------------------------------------------
void NiSPThreadPool::Shutdown()
{
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        NIASSERT(m_apkWorkerProcs->GetAt(i));
        m_apkWorkerProcs->GetAt(i)->SignalShutdown();
    }

    // All threads have shutdown signal. Signal the semaphore to wake them
    // up and have them exit.
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        m_kWorkQueueSemaphore.Signal();
    }
    
    // Send one more signal to the work queue semaphore. We're shutting down
    // so it won't matter if it's extraneous.
    m_kWorkQueueSemaphore.Signal();

    NiFree(m_pCurrentWorkflowList);
    NiFree(m_pCurrentStageList);
}
//---------------------------------------------------------------------------
NiSPThreadPool::NiSPThreadPool(unsigned int uiMaxThreads) 
    : m_uiMaxWorkerThreads(uiMaxThreads)
    , m_kWorkQueue(MAX_QUEUESIZE)
    , m_kWorkQueueSemaphore(0, MAX_QUEUESIZE)
    , m_uiPausedThreads(0)
    , m_pCurrentWorkflowList(NULL)
    , m_pCurrentStageList(NULL)
    , m_bPauseWorkerThreads(false)
    , m_iWorkerThreadShutdown(-1)
{
}
//---------------------------------------------------------------------------
NiSPThreadPool::~NiSPThreadPool()
{
    // NiThread dtor waits for the completion we signaled in Shutdown.
    unsigned int uiSizeToDelete = m_uiMaxWorkerThreads;
    for (unsigned int i = 0; i < uiSizeToDelete; i++)
    {
        NiDelete m_apkWorkerThreads->GetAt(i);
    }

    uiSizeToDelete = m_apkWorkerProcs->GetSize();
    for (unsigned int i = 0; i < uiSizeToDelete; i++)
    {
        NiDelete m_apkWorkerProcs->GetAt(i);
    }
    
    NiDelete m_apkWorkerThreads;
    NiDelete m_apkWorkerProcs;
}
//---------------------------------------------------------------------------
NiUInt32 NiSPThreadPool::GetWorkerThreadCount()
{
    return m_uiMaxWorkerThreads;
}
//---------------------------------------------------------------------------
void NiSPThreadPool::DestroyLastWorkerThread()
{
    // Wait for all the threads to pause
    m_bPauseWorkerThreads = true;
    while (m_uiPausedThreads != m_uiMaxWorkerThreads)
    {
        if (m_kWorkQueueSemaphore.GetCount() == 0)
            m_kWorkQueueSemaphore.Signal();
    }

    // At this point we know all the worker threads are effectively paused
    // on a m_kDynamicThreadShutdownSemaphore.Wait()

    // Identify the thread we want to shutdown/destroy. This is always the 
    // last worker thread.
    m_iWorkerThreadShutdown = m_uiMaxWorkerThreads-1;

    // Remove the last threads from all the associated arrays.
    WorkerProc* pkRemovedWorkerProc = m_apkWorkerProcs->RemoveEnd();
    NIASSERT(pkRemovedWorkerProc);

    NiThread* pkRemovedThread = m_apkWorkerThreads->RemoveEnd();
    NIASSERT(pkRemovedThread);

    for (NiUInt32 ui = 0; ui < m_uiMaxWorkerThreads; ui++)
    {
        NIASSERT(m_pCurrentWorkflowList[ui] == 0);
        NIASSERT(m_pCurrentStageList[ui] == 0);
    }
    m_uiMaxWorkerThreads--;

    m_pCurrentWorkflowList = (NiUInt32*)NiRealloc(m_pCurrentWorkflowList, 
        sizeof(NiUInt32) * m_uiMaxWorkerThreads);

    m_pCurrentStageList = (NiUInt32*)NiRealloc(m_pCurrentStageList, 
        sizeof(NiUInt32) * m_uiMaxWorkerThreads);

    // Resume all worker threads.
    m_bPauseWorkerThreads = false;
    while (m_uiPausedThreads > 0)
    {
        if (m_kThreadPauseShutdownSemaphore.GetCount() == 0)
            m_kThreadPauseShutdownSemaphore.Signal();
    }

    // Clean up and extra signals that may have occurred.
    while (m_kThreadPauseShutdownSemaphore.GetCount() > 0)
        m_kThreadPauseShutdownSemaphore.Wait();

    // Wait for thread we shutdown to signal that it is shutting down.
    m_kShutdownThreadAssignedSemaphore.Wait();

    m_iWorkerThreadShutdown = -1;

    // Wait for thread to complete.
    while (pkRemovedThread->GetStatus() != NiThread::COMPLETE)
        NiYieldThread();

    // Finally delete the thread and it's workerproc now that we know the 
    // thread as completed.
    NiDelete pkRemovedWorkerProc;
    NiDelete pkRemovedThread;
}
//---------------------------------------------------------------------------
void NiSPThreadPool::CreateNewWorkerThread()
{
    // Wait for all the threads to pause
    m_bPauseWorkerThreads = true;
    while (m_uiPausedThreads != m_uiMaxWorkerThreads)
    {
        if (m_kWorkQueueSemaphore.GetCount() == 0)
            m_kWorkQueueSemaphore.Signal();
    }

    // Allocate the additional space required for the new worker thread.
    m_pCurrentWorkflowList = (NiUInt32*)NiRealloc(m_pCurrentWorkflowList, 
        sizeof(NiUInt32) * (m_uiMaxWorkerThreads + 1));

    m_pCurrentStageList = (NiUInt32*)NiRealloc(m_pCurrentStageList, 
        sizeof(NiUInt32) * (m_uiMaxWorkerThreads + 1));

    m_pCurrentWorkflowList[m_uiMaxWorkerThreads] = 0;
    m_pCurrentStageList[m_uiMaxWorkerThreads] = 0;

    WorkerProc* pkWorkerProc = NiNew WorkerProc();
    pkWorkerProc->SetPool(this);
    pkWorkerProc->SetThreadID(m_uiMaxWorkerThreads);
    NIASSERT(m_apkWorkerProcs->GetSize() == m_uiMaxWorkerThreads);
    m_apkWorkerProcs->SetAtGrow(m_uiMaxWorkerThreads, pkWorkerProc);

    NiThread* pkThread = NiThread::Create(pkWorkerProc);
    pkThread->SetPriority(NiThread::HIGHEST);

    NiProcessorAffinity kAffinity;
    ms_pfnAssignWorkerThreadAffinityFunc(
        m_uiMaxWorkerThreads, m_uiMaxWorkerThreads + 1, kAffinity);

    pkThread->SetThreadAffinity(kAffinity);
    NIASSERT(m_apkWorkerThreads->GetSize() == m_uiMaxWorkerThreads);
    m_apkWorkerThreads->SetAtGrow(m_uiMaxWorkerThreads, pkThread);
    pkThread->Resume();

    m_uiMaxWorkerThreads++;

    m_bPauseWorkerThreads = false;

    // Resume all worker threads.
    while (m_uiPausedThreads > 0)
    {
        if (m_kThreadPauseShutdownSemaphore.GetCount() == 0)
            m_kThreadPauseShutdownSemaphore.Signal();
    }

    // Clean up and extra signals that may have occurred.
    while (m_kThreadPauseShutdownSemaphore.GetCount() > 0)
        m_kThreadPauseShutdownSemaphore.Wait();
}
//---------------------------------------------------------------------------
bool NiSPThreadPool::SetWorkerThreadCount(NiUInt32 uiWorkerThreadCount)
{
    if (uiWorkerThreadCount == 0)
        return false;

    if (uiWorkerThreadCount == m_uiMaxWorkerThreads)
        return true;

    if (uiWorkerThreadCount < m_uiMaxWorkerThreads)
    {
        while (m_uiMaxWorkerThreads != uiWorkerThreadCount)
            DestroyLastWorkerThread();
    }
    else
    {
        while (m_uiMaxWorkerThreads != uiWorkerThreadCount)
            CreateNewWorkerThread();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSPThreadPool::CompleteActiveWork()
{
    // Block until all the worker threads to complete their assigned work.
    bool bDone = false;

    while (!bDone)
    {
        NiYieldThread();

        m_kLock.Lock();      
        if (m_kWorkQueue.GetSize() == 0)
        {
            bDone = true;

            // Check to see if any worker threads have an assigned workflow.
            for (NiUInt32 ui = 0; ui < m_uiMaxWorkerThreads; ui++)
            {
                if (m_pCurrentWorkflowList[ui] != 0)
                {
                    // A worker thread is still performing work.
                    bDone = false;
                    break;
                }
            }
        }
        m_kLock.Unlock();
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSPThreadPool::ExecuteWorkflow(NiSPWorkflow* pkWorkflow)
{
    NIASSERT(pkWorkflow->GetStatus() == NiSPWorkflow::RUNNING);

    // Ensure that the provided workflow does not contain any pending joblists.
    // Hitting this assert indicates that the thread pool stop executing a 
    // workflow prematurely.
    NIASSERT(pkWorkflow->GetNumPendingJobLists() == 0);

    NiUInt32 uiTaskCount = pkWorkflow->GetSize();
    NiUInt32 uiIndex = 0;

    // Traverse the tasks until they are in a subsequent stage. Submit them
    // to the thread pool.
    for (; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = pkWorkflow->GetAt(uiIndex);
        if (!pkTask->IsEnabled())
            continue;

        NIASSERT(!(pkTask->IsSync() &&
            pkTask->GetStage() != NiSPAnalyzer::MAX_STAGE));

        // Submit for the current stage.
        NiSPTaskImpl& kTaskImpl = pkTask->GetImpl();
        NiUInt32 uiListCount = kTaskImpl.GetSize();
        for (NiUInt32 i = 0; i < uiListCount; i++)
        {
            NiSPJobList* pkJobList = kTaskImpl.GetAt(i);
            pkJobList->SetIsFinished(false);

            // Record information about the workflow the joblist belongs to
            // into the joblist itself. This helps the worker thread determine
            // whether the joblist can be executed or not.
            pkJobList->SetWorkflow(pkWorkflow);
            pkJobList->SetWorkflowID(pkWorkflow->GetId());
            pkJobList->SetStage(pkTask->GetStage());

            Enqueue(pkJobList);
            pkWorkflow->AddPendingJobList(pkJobList);
        }
    }
}
//---------------------------------------------------------------------------
void NiSPThreadPool::Enqueue(NiSPJobList* pkJobList)
{
    // Spin yield if there's no room in the queue. Let the worker threads
    // peel some work off.
    while (m_kWorkQueue.GetSize() >= MAX_QUEUESIZE)
    {
        NiYieldThread();
    }

    // Lock the manager when accessing the task queue.
    m_kLock.Lock();

    // Get queue and check for available space
    if (m_kWorkQueue.GetSize() < MAX_QUEUESIZE)
    {
        m_kWorkQueue.Add(pkJobList);
    }
    else
    {
        NILOG("Error: Queue full.");
    }
    // Unlock
    m_kLock.Unlock();

    // Signal the update semaphore to wake up the processing thread.
    m_kWorkQueueSemaphore.Signal();
}

//---------------------------------------------------------------------------
NiSPJobList* NiSPThreadPool::Dequeue(WorkerProc* pkWorkerProc)
{
    m_kWorkQueueSemaphore.Wait();

    m_kLock.Lock();
    NiSPJobList* pkJobList = m_kWorkQueue.Remove();
    if (pkJobList)
    {
        NIASSERT(pkWorkerProc->GetThreadID() >= 0);
        NIASSERT(pkWorkerProc->GetThreadID() < m_uiMaxWorkerThreads);
        m_pCurrentWorkflowList[pkWorkerProc->GetThreadID()] = pkJobList->GetWorkflowID();
        m_pCurrentStageList[pkWorkerProc->GetThreadID()] = pkJobList->GetStage();
    }
    m_kLock.Unlock();

    if (pkJobList)
    {
        NiUInt32 i;
        bool bWait = true;

        while (bWait)
        {
            for (i = 0; i < m_uiMaxWorkerThreads; i++)
            {
                // Note: A critical section lock is not needed around the 
                // following branch since a workflow's joblists are guaranteed 
                // to be sorted by their stage. This ensures that a
                // workflow's job lists will be stored in the queue in 
                // monotonically increasing order. 

                // We will actually examine our own job here but that is fine
                // since the stage check will fail as we are on the same stage
                // as ourself, not an earlier one.

                if (pkJobList->GetWorkflowID() == m_pCurrentWorkflowList[i] &&
                    pkJobList->GetStage() > m_pCurrentStageList[i])
                {
                    break; // break out of for loop
                }

            }

            if (i == m_uiMaxWorkerThreads)
            {
                bWait = false;
            }
            else
            {
                // If we are dependent on a job that is being worked on then we
                // naively yield ourselves until we are able to run.
                NiYieldThread();
            }
        }
    }

    return pkJobList;
}

//---------------------------------------------------------------------------
// WorkerProc functions.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
unsigned int NiSPThreadPool::WorkerProc::ThreadProcedure(NiThread*)
{
    m_bShutdown = false;
    while (!m_bShutdown)
    {
        // Check to see if the worker threads should be paused. This only 
        // happens when dynamically creating/destroying worker threads.
        if (m_pkPool->m_bPauseWorkerThreads)
        {
            // Ensure that this thread is not still executing a workflow
            NIASSERT(m_pkPool->m_pCurrentWorkflowList[m_uiThreadID] == 0);            

            // Indicate that a worker thread is now paused.
            NiAtomicIncrement(m_pkPool->m_uiPausedThreads);

            // Wait for the signal to resume the thread.
            m_pkPool->m_kThreadPauseShutdownSemaphore.Wait();

            // Indicate that a worker thread is no longer paused.
            NiAtomicDecrement(m_pkPool->m_uiPausedThreads);

            if (m_pkPool->m_iWorkerThreadShutdown == (int)m_uiThreadID)
            {
                // If this thread has been designated for shutdown, signal
                // that we acknowledge this and break out of the loop and shutdown.
                m_pkPool->m_kShutdownThreadAssignedSemaphore.Signal();
                break;
            }
        }
        else
        {
            // Block waiting for work.
            NiSPJobList* pkWork = m_pkPool->Dequeue(this);
            if (pkWork)
            {
                // Process work
                NiUInt32 uiJobCount = pkWork->GetJobCount();
                for (NiUInt32 j = 0; j < uiJobCount; j++)
                {
                    NiSPWorkload* pkWorkload = pkWork->GetWorkload(j);
                    NiSPKernel* pkKernel = pkWorkload->GetKernel();
                    pkKernel->Execute(*pkWorkload);
                }
                pkWork->SetIsFinished(true);

                // If we just executed the final stage then we can mark the 
                // workflow as completed.
                if (pkWork->GetStage() == NiSPAnalyzer::MAX_STAGE)
                {
                    NiStreamProcessor::Get()->
                        SubmitForFinalization(
                        ((NiSPWorkflow *)(pkWork->GetWorkflow())));
                }

                // Invalid workflow id
                m_pkPool->m_pCurrentWorkflowList[m_uiThreadID] = 0;

                // Set the stage ID to 0. This is added safety that the workflow
                // & stage branch in NiSPThreadPool::Dequeue() never results in a
                // race condition.
                m_pkPool->m_pCurrentStageList[m_uiThreadID] = 0; 
            }
        }
    }
    
    return 0;
}
//---------------------------------------------------------------------------

