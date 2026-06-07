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

#if !defined(_WII)
#ifndef NISPTHREADPOOL_H
#define NISPTHREADPOOL_H

#include <NiSystem.h>
#include <NiLoopedThread.h>
#include <NiTArray.h>
#include <NiTQueue.h>
#include "NiFloodgateLibType.h"

class NiSPTask;
class NiSPWorkflow;
class NiThread;
class NiSPJobList;

/**
    Maintains a pool of NiLoopedThread objects used for executing tasks, each
    on a separate thread.
*/
class NIFLOODGATE_ENTRY NiSPThreadPool : public NiMemObject
{
public:

#if !defined(_PS3)
    /**
        Call-back function signature for assigning the processor affinity for
        a worker thread.

        @param uiWorkerThreadID Input. The ID of the worker thread to assign 
            a processor affinity too. This value will range between 0 and
            uiMaxWorkerThreads - 1.
        @param uiMaxWorkerThreads Input. The maximum number of worker threads
            that will be available.
        @param kProcessorAffinity Input/Output. The NiProcessorAffinity that
            will be configured by this function. 
    */
    typedef void (*AssignWorkerThreadAffinity)(
        NiUInt32 uiWorkerThreadID, NiUInt32 uiMaxWorkerThreads,
        NiProcessorAffinity& kProcessorAffinity);

    /// Sets the 'AssignWorkerThreadAffinity' callback function.
    static void SetWorkerThreadAffinityFunc(
        AssignWorkerThreadAffinity pfnCallback);

    /// Returns the 'AssignWorkerThreadAffinity' callback function.
    static AssignWorkerThreadAffinity GetWorkerThreadAffinityFunc();

    /// The default 'AssignWorkerThreadAffinity' callback function.
    static void AssignWorkerThreadAffinityDefaultFunc(
        NiUInt32 uiWorkerThreadID, NiUInt32 uiMaxWorkerThreads,
        NiProcessorAffinity& kProcessorAffinity);

    /// Block until all the worker threads to complete their assigned work.
    bool CompleteActiveWork();

    /// Change the number of worker threads used by the thread pool.
    bool SetWorkerThreadCount(NiUInt32 uiWorkerThreadCount);
#endif

    /// Constructor: takes the maximum number of threads to use for Floodgate.
    NiSPThreadPool(unsigned int uiMaxThreads);
    
    /// Destructor.
    virtual ~NiSPThreadPool();

    /// Initializes object pools.
    void Initialize(); 
    
    /// Shuts down object pools.
    void Shutdown();

    /// Clears the thread.
    void Clear();

    /// Executes a workflow.
    void ExecuteWorkflow(NiSPWorkflow* pkWorkflow);

    /// Return the number of worker threads used by thread pool.
    NiUInt32 GetWorkerThreadCount();

protected:

    class WorkerProc : public NiThreadProcedure
    {
    public:
        virtual unsigned int ThreadProcedure(NiThread* pkThread);
        void SetPool(NiSPThreadPool *pkPool)
        {
            m_pkPool = pkPool;
        }
        void SignalShutdown() {m_bShutdown = true;}
        NiUInt32 GetThreadID() {return m_uiThreadID; }
        void SetThreadID(NiUInt32 uiNewThreadID) 
        {m_uiThreadID = uiNewThreadID; }

    protected:
        NiSPThreadPool *  m_pkPool;
        volatile NiUInt32 m_uiThreadID;
        volatile bool     m_bShutdown;
    };

public:

    /// @cond EMERGENT_INTERNAL 

    /**
        Enqueues a job list.

        @param pkJobList Job list to add to the queue.
    */
    void Enqueue(NiSPJobList* pkJobList);

    /** 
        Dequeues a job list. 

        @param uiWorkerThreadID ID of the worker thread requesting a
            NiSPJobList. This may be ignored by some platforms.
    */
    NiSPJobList* Dequeue(WorkerProc* pkWorkerProc);

    /// @endcond

protected:

    typedef NiTPrimitiveArray<NiThread*> ThreadArray;
    typedef NiTPrimitiveArray<WorkerProc*> WorkerProcArray;
    
#if !defined(_PS3)
    /// Function pointer to callback used to define processor affinity for
    /// worker threads.
    static AssignWorkerThreadAffinity ms_pfnAssignWorkerThreadAffinityFunc;

    /// Dynamically creates a new worker thread. 
    void CreateNewWorkerThread();

    /// Dynamically destroys the last worker thread.
    void DestroyLastWorkerThread();
#endif


    /// Critical section.
    NiCriticalSection m_kLock;
    
    /// Procedures for working threads.
    WorkerProcArray* m_apkWorkerProcs;

    /// Array of worker threads.
    ThreadArray* m_apkWorkerThreads;

    /// Max number of worker threads.
    unsigned int m_uiMaxWorkerThreads;

    /// Queue of NiSPJobLists to execute.
    NiTPrimitiveQueue<NiSPJobList*> m_kWorkQueue;

    /// Semaphore to control queue access.
    NiSemaphore m_kWorkQueueSemaphore;

    /// Semaphore to control pausing the worker threads
    NiSemaphore m_kThreadPauseShutdownSemaphore;
    
    NiSemaphore m_kShutdownThreadAssignedSemaphore;

    /// Counter to specify the number of worker threads actively paused.
    NiUInt32 m_uiPausedThreads;

    /// List of what workflow each worker thread is working on
    NiUInt32* m_pCurrentWorkflowList;

    /// List of what stage each worker thread is working on
    NiUInt32* m_pCurrentStageList;

    /// ID of the desired worker thread to shutdown. Only used when
    /// worker threads are paused.
    volatile NiInt32 m_iWorkerThreadShutdown;

    /// Flag to tell the work threads to temporarily stop execution
    /// and wait on m_kThreadPauseShutdownSemaphore.
    volatile bool m_bPauseWorkerThreads;
};

#include "NiSPThreadPool.inl"

#endif  // #ifndef NISPTHREADPOOL_H
#endif // !defined(_WII)
