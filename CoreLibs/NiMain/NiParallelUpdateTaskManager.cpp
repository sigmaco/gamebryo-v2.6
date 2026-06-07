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
#include "NiMainPCH.h"

#include "NiParallelUpdateTaskManager.h"

NiImplementRTTI(NiParallelUpdateTaskManager, NiTaskManager);
NiImplementRTTI(NiParallelUpdateTaskManager::SignalTask, NiTask);

NiParallelUpdateTaskManager* 
    NiParallelUpdateTaskManager::ms_pkManager = NULL;

//---------------------------------------------------------------------------
NiParallelUpdateTaskManager::NiParallelUpdateTaskManager(
    unsigned int uiMaxQueueSize) :
    m_uiMaxQueueSize(uiMaxQueueSize),
    m_kUpdateSema(0, uiMaxQueueSize),
    m_kRenderSema(1, 1),
    m_kSignalTask(),
    m_pkThread(NULL),
    m_pkThreadProc(NULL),
    m_bActive(false)
{
    m_kSignalTask.SetStatus(NiTask::COMPLETED);
}
//---------------------------------------------------------------------------
NiParallelUpdateTaskManager::~NiParallelUpdateTaskManager()
{
    NiDelete m_pkThread;
    NiDelete m_pkThreadProc;
}
//---------------------------------------------------------------------------
bool NiParallelUpdateTaskManager::Initialize(unsigned int uiMaxQueueSize)
{
    if (ms_pkManager)
        return true;

    ms_pkManager = NiNew NiParallelUpdateTaskManager(uiMaxQueueSize);

    if (ms_pkManager)
    {
        // Start up the threaded task processing.
        ms_pkManager->DoTasks();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiParallelUpdateTaskManager::Shutdown()
{
    if (ms_pkManager)
    {
        ms_pkManager->Stop();
        NiDelete ms_pkManager;
        ms_pkManager = NULL;
    }
}
//---------------------------------------------------------------------------
bool NiParallelUpdateTaskManager::AddTask_Internal(
    bool bAddingSignalTask,
    NiTask* pkTask, 
    NiTaskManager::TaskPriority ePriority)
{
    // Lock the manager when accessing the task queues.
    m_kManagerLock.Lock();
    NiTPrimitiveQueue<NiTask*> &kTaskQueue = m_akTaskQueues[ePriority];
    
    // Check that we have room to add a task 
    // (The semaphore has a max count, we must not overflow it)
    //
    // we must have room for this task plus the signal task in EndUpdate()
    unsigned int uiMaxSize = m_uiMaxQueueSize - ((bAddingSignalTask) ? 0 : 1);

    if (kTaskQueue.GetSize() < uiMaxSize)
    {
#ifdef _XENON
        PIXSetMarker(D3DCOLOR_XRGB(255,128,0), 
            "NiParallelUpdateTaskManager::");
        PIXSetMarker(D3DCOLOR_XRGB(255,128,0), " AddTask(), added:");
        PIXSetMarker(D3DCOLOR_XRGB(255,128,0), pkTask->GetRTTI()->GetName());
#endif

        kTaskQueue.Add(pkTask);
        pkTask->SetStatus(NiTask::PENDING);
        m_kManagerLock.Unlock();

        // We've added a task. Signal the update semaphore to wake up the 
        // background thread.
        SignalUpdateSema();
        return true;
    }
    else
    {
#ifdef _XENON
        PIXSetMarker(D3DCOLOR_XRGB(255,128,0), 
            "NiParallelUpdateTaskManager::");
        PIXSetMarker(D3DCOLOR_XRGB(255,128,0), 
            " AddTask(), FULL! didn't add:");
        PIXSetMarker(D3DCOLOR_XRGB(255,128,0), pkTask->GetRTTI()->GetName());
#endif
        m_kManagerLock.Unlock();
        return false;
    }
}
//---------------------------------------------------------------------------
NiTaskManager::ManagerStatus NiParallelUpdateTaskManager::DoTasks()
{
    if (m_eStatus == NiTaskManager::RUNNING)
        return m_eStatus;

    m_eStatus = NiTaskManager::RUNNING;

    m_pkThreadProc = NiNew NiParallelUpdateTaskManager::ManagerProc(this);
    NIASSERT(m_pkThreadProc);

    m_pkThread = NiThread::Create(m_pkThreadProc);
    NIASSERT(m_pkThread);

    m_pkThread->SetName("NiParallelUpdateTaskManager");

    // Resume the thread. The update semaphore in the manager will cause
    // the background process to block almost immediately. The addition
    // of tasks will wake up the thread.
    m_pkThread->Resume();
    
    return m_eStatus;
}
//---------------------------------------------------------------------------
bool NiParallelUpdateTaskManager::DoSingleTask(TaskPriority ePriority)
{
    m_kManagerLock.Lock();
    NiTask* pkTask = m_akTaskQueues[ePriority].Remove();
    m_kManagerLock.Unlock();

    if (pkTask)
    {
#ifdef _XENON
        PIXBeginNamedEvent(D3DCOLOR_XRGB(255,128,0), 
            pkTask->GetRTTI()->GetName());
#endif

        pkTask->SetStatus(NiTask::RUNNING);
        pkTask->DoTask();

#ifdef _XENON
        PIXEndNamedEvent();
#endif
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiParallelUpdateTaskManager::Clear()
{
    NIASSERT(m_eStatus == NiTaskManager::STOPPING);

    m_kManagerLock.Lock();

    for (int iPriority = NiTaskManager::IMMEDIATE;
        iPriority >= 0; iPriority--)
    {
        NiTask* pkTask = m_akTaskQueues[iPriority].Remove();
        while (pkTask)
        {
            pkTask->SetStatus(NiTask::ABORTED);
            pkTask->Clear();
            pkTask = m_akTaskQueues[iPriority].Remove();
        }
    }
    m_eStatus = NiTaskManager::STOPPED;

    m_kManagerLock.Unlock();
    return true;
}
//---------------------------------------------------------------------------
NiTaskManager::ManagerStatus NiParallelUpdateTaskManager::Stop()
{
    m_eStatus = NiTaskManager::STOPPING;
    Clear();

    // Signal the update semaphore to allow the thread to complete
    SignalUpdateSema();

    m_pkThread->WaitForCompletion();

    return NiTaskManager::STOPPED;
}
//---------------------------------------------------------------------------
void NiParallelUpdateTaskManager::BeginUpdate()
{
    if (!ms_pkManager)
        return;

    ms_pkManager->m_bActive = true;

    // Acquire the render semaphore so that rendering cannot occur until
    // our fence task clears this semaphore.
    ms_pkManager->WaitRenderSema();
}
//---------------------------------------------------------------------------
void NiParallelUpdateTaskManager::EndUpdate()
{
    if (!ms_pkManager)
        return;

    // Insert the signal task. This task is basically a fence that goes
    // through the pipeline and signals the render semaphore.
    NIVERIFY(ms_pkManager->AddTask_Internal(
        true, &(ms_pkManager->m_kSignalTask), NiTaskManager::LOW));

    ms_pkManager->m_bActive = false;
}
//---------------------------------------------------------------------------
bool NiParallelUpdateTaskManager::SetAffinity(
    const NiProcessorAffinity& kAffinity)
{
    if (m_pkThread)
        return m_pkThread->SetThreadAffinity(kAffinity);
    return false;
}
//---------------------------------------------------------------------------
bool NiParallelUpdateTaskManager::SetPriority(NiThread::Priority ePriority)
{
    if (m_pkThread)
        return m_pkThread->SetPriority(ePriority);
    return false;
}
//---------------------------------------------------------------------------
// ManagerProc functions.
//---------------------------------------------------------------------------
NiParallelUpdateTaskManager::ManagerProc::ManagerProc(
    NiParallelUpdateTaskManager* pkManager) : m_pkManager(pkManager)
{
}
//---------------------------------------------------------------------------
unsigned int NiParallelUpdateTaskManager::ManagerProc::ThreadProcedure(
    NiThread* pkThread)
{
    m_pkThread = pkThread;

    // Loop on the manager until an external source sets it to stop.
    while (m_pkManager->GetStatus() == NiTaskManager::RUNNING)
    {
        // We'll do a task. If there are no tasks in the queues, then the
        // update semaphore will block and we won't spend time iterating
        // over empty queues.
        m_pkManager->WaitUpdateSema();
        int iPriority = NiTaskManager::IMMEDIATE;
        for ( ; iPriority >= 0; iPriority--)
        {
            if (m_pkManager->GetNumTasks(
                (NiTaskManager::TaskPriority)iPriority))
            {
                if (m_pkManager->DoSingleTask(
                    (NiTaskManager::TaskPriority)iPriority))
                {
                    break;
                }
            }
        }
        // We did not process any tasks, so we signal the update semaphore to
        // maintain the correct count.
        if (iPriority < 0)
            m_pkManager->SignalUpdateSema();
    }

    // Stop signal has been received. We'll return an exit code of 0.
    return 0;
}
//---------------------------------------------------------------------------
