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

#ifndef NIPARALLELUPDATETASKMANAGER_H
#define NIPARALLELUPDATETASKMANAGER_H

#include <NiCriticalSection.h>
#include <NiSemaphore.h>
#include <NiTask.h>
#include <NiTaskManager.h>
#include <NiThread.h>
#include <NiThreadProcedure.h>

class NIMAIN_ENTRY NiParallelUpdateTaskManager : public NiTaskManager
{
    NiDeclareRTTI;

    // Constructor and destructor are protected. They should always be
    // accessed via the Initialize and terminate methods. Those methods
    // are safe to call repeatedly and insure the singleton nature of this
    // class.
protected:
    NiParallelUpdateTaskManager(unsigned int uiMaxQueueSize);
    virtual ~NiParallelUpdateTaskManager();

public:
    // Initialize creates the singleton, 
    // Terminate stops the manager and deletes the singleton instance. 
    static bool Initialize(unsigned int uiMaxQueueSize = INT_MAX);
    static void Shutdown();

    // Retrieve a pointer to the singleton instance.
    inline static NiParallelUpdateTaskManager* Get();

    // Standard NiTaskManager methods that are overridden. Users should never
    // need to call these directly as they are called from the constructor
    // and destructor.
    virtual bool AddTask(NiTask* pkTask, TaskPriority ePriority = MEDIUM);
    
    virtual ManagerStatus DoTasks();
    virtual bool DoSingleTask(TaskPriority ePriority);
    virtual bool Clear();
    virtual ManagerStatus Stop();

    // Returns true if the manager exists and you are between a pair of calls
    // to BeginUpdate and EndUpdate.
    static bool IsActive();

    // These methods should bracket the call to update the scene graph in the
    // application. The begin call sets the manager to active and waits on the
    // render semaphore. The end update call sets the manager to inactive and
    // inserts a task to signal the render semaphore when it completes. Once
    // this task clears, all morph controllers have been updated.
    static void BeginUpdate();
    static void EndUpdate();

    // Signal and wait on the render semaphore. Applications should use these 
    // functions to bracket their rendering calls insuring that all background
    // morphing tasks have occurred.
    static int WaitRenderSema();
    inline static int SignalRenderSema();

    // Set the processor affinity for the background process.
    bool SetAffinity(const NiProcessorAffinity& kAffinity);

    // Set the thread priority for the background process.
    bool SetPriority(NiThread::Priority);

    // Get a pointer to the thread for reading current affinity or priority.
    inline const NiThread* GetThread();

protected:

    // Some syncrhonization will occur via an updated semaphore that can be 
    // modified only through protected methods. The ManagerProc will be a 
    // friend class to allow this to occur.
    friend class ManagerProc;

    class ManagerProc : public NiThreadProcedure
    {
    public:
        ManagerProc(NiParallelUpdateTaskManager* pkManager);

        virtual unsigned int ThreadProcedure(NiThread* pkThread);
        
    protected:
        NiThread* m_pkThread;
        NiParallelUpdateTaskManager* m_pkManager;
    };

    class SignalTask : public NiTask
    {
        NiDeclareRTTI;
    public:
        void DoTask()
        {
            NiParallelUpdateTaskManager::Get()->SignalRenderSema();
            m_eStatus = NiTask::COMPLETED;
        }
    };

    virtual bool AddTask_Internal(
        bool bAddingSignalTask, NiTask* pkTask, 
        TaskPriority ePriority = MEDIUM);

    // The update semaphore carries a count of the number of tasks stored in
    // the manager at any given moment. The background process uses this 
    // semaphore to stall execution when there are not any tasks to execute.
    inline int SignalUpdateSema();
    inline int WaitUpdateSema();

    unsigned int m_uiMaxQueueSize;

    NiSemaphore m_kUpdateSema;
    NiSemaphore m_kRenderSema;

    SignalTask m_kSignalTask;

    NiThread* m_pkThread;
    ManagerProc* m_pkThreadProc;

    volatile bool m_bActive;

    static NiParallelUpdateTaskManager* ms_pkManager;
};

#include "NiParallelUpdateTaskManager.inl"

#endif // NIPARALLELUPDATETASKMANAGER_H
