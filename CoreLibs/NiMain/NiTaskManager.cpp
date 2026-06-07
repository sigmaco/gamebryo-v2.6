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

#include "NiTask.h"
#include "NiTaskManager.h"

NiImplementRTTI(NiTaskManager, NiObject);

//---------------------------------------------------------------------------
NiTaskManager::NiTaskManager() : m_eStatus(STOPPED)
{
}
//---------------------------------------------------------------------------
NiTaskManager::~NiTaskManager()
{
}
//---------------------------------------------------------------------------
bool NiTaskManager::AddTask(NiTask* pkTask, TaskPriority ePriority)
{
    // Accquire the lock for thread safety.
    m_kManagerLock.Lock();

    // Add the task and set it to PENDING. Order does not matter since the
    // lock insures that the operation will be atomic.
    m_akTaskQueues[ePriority].Add(pkTask);
    pkTask->SetStatus(NiTask::PENDING);

    // Release and return.
    m_kManagerLock.Unlock();
    return true;
}
//---------------------------------------------------------------------------
NiTaskManager::ManagerStatus NiTaskManager::DoTasks()
{
    // Accquire the lock for thread safety.
    m_kManagerLock.Lock();

    // Set status in case we’re queried by an external thread.
    m_eStatus = RUNNING;

    // Iterate over priority queues.
    for (int i = IMMEDIATE; i >= 0; i--)
    {
        // Try each task in the queue via DoSingleTask.
        unsigned int uiNumTasks = m_akTaskQueues[i].GetSize();
        for (unsigned int ui = 0; ui < uiNumTasks; ui++)
        {
            DoSingleTask((NiTaskManager::TaskPriority)i);
        }
    }

    // Release and return.
    m_eStatus = STOPPED;

    // Release once done with the queues and status.
    m_kManagerLock.Unlock();

    return m_eStatus;
}
//---------------------------------------------------------------------------
bool NiTaskManager::DoSingleTask(TaskPriority ePriority)
{
    // Accquire the lock for thread safety.
    m_kManagerLock.Lock();

    // If there are no tasks at this priority, we return false.
    if (!m_akTaskQueues[ePriority].IsEmpty())
    {
        // Remove and execute task. This will occur atomically.
        NiTask* pkTask = m_akTaskQueues[ePriority].Remove();

        // If the task can't execute, return false.
        if (!pkTask->CanExecute())
        {
            m_akTaskQueues[ePriority].Add(pkTask);
            m_kManagerLock.Unlock();
            return false;
        }

        // We're done with the queues, release the lock.
        m_kManagerLock.Unlock();

        // Execute the task.
        pkTask->SetStatus(NiTask::RUNNING);
        pkTask->DoTask();
        
        return true;
    }

    // Release and return.
    m_kManagerLock.Unlock();
    return false;
}
//---------------------------------------------------------------------------
bool NiTaskManager::Clear()
{
    // We do not want to clear a running manager, and a stopped manager
    // should already be cleared.
    if (m_eStatus != STOPPING)
        return false;

    // Default implementation will simply abort all tasks.
    m_kManagerLock.Lock();
    for (unsigned int ui = 0; ui < NUM_PRIORITIES; ui++)
    {
        NiTask* pkTask = m_akTaskQueues[ui].Remove();
        while (pkTask)
        {
            pkTask->SetStatus(NiTask::ABORTED);
            pkTask = m_akTaskQueues[ui].Remove();
        }
    }

    // Set status, release, and return.
    m_eStatus = STOPPED;
    m_kManagerLock.Unlock();

    return true;
}
//---------------------------------------------------------------------------
