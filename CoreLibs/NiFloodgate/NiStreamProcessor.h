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

#ifndef NISTREAMPROCESSOR_H
#define NISTREAMPROCESSOR_H

#include <NiSystem.h>
#include <NiTPointerMap.h>
#include <NiProcessorAffinity.h>
#include <NiThread.h>
#include <NiSemaphore.h>
#include <NiTQueue.h>

#include "NiFloodgateSDM.h"
#include "NiSPTask.h"
#include "NiSPWorkflow.h"
#include "NiSPThreadPool.h"
#include "NiSPAnalyzer.h"

static NiFloodgateSDM NiFloodgateSDMObject;

class NiFloodgateEventQueue;

#if defined(WIN32)
/// Replacement for API for interlocked singly linked lists,
/// which are unavailable before WinXP.
struct __declspec(align(MEMORY_ALLOCATION_ALIGNMENT)) SLIST_ENTRY_GB
{
    SLIST_ENTRY_GB* Next;

    // pad out aligned structure to avoid compiler warning C4324
    NiUInt8 m_auiPadding[MEMORY_ALLOCATION_ALIGNMENT - 
        sizeof(SLIST_ENTRY_GB*)];
};
typedef SLIST_ENTRY_GB* PSLIST_ENTRY_GB;
typedef SLIST_ENTRY_GB SLIST_HEADER_GB;
typedef SLIST_HEADER_GB* PSLIST_HEADER_GB;

void InitializeSListHead_GB(PSLIST_HEADER_GB ListHead);
PSLIST_ENTRY_GB InterlockedPopEntrySList_GB(PSLIST_HEADER_GB pkListHead);
PSLIST_ENTRY_GB InterlockedPushEntrySList_GB(
    PSLIST_HEADER_GB pkListHead, 
    PSLIST_ENTRY_GB pkListEntry);

#elif defined(_XENON)
/// Xenon always supports this API
typedef SLIST_HEADER SLIST_HEADER_GB;
typedef SLIST_ENTRY SLIST_ENTRY_GB;

#endif

/// Main interface for working with the Floodgate stream processing system.
class NIFLOODGATE_ENTRY NiStreamProcessor : public NiMemObject
{
public:
    /// Workflow dispatch priority hints. Higher priority is not a 
    /// guarantee that the workflow will be executed first. 
    enum Priority
    {
        LOW = 0,
        MEDIUM,
        HIGH,
        IMMEDIATE,
        NUM_PRIORITIES
    };
    
    /// Gets the single managed instance.
    static NiStreamProcessor* Get();

    /// Destructor.
    ~NiStreamProcessor();

#if !defined(_PS3) && !defined(_WII)

    // Affinity functions.

    /// Returns whether or not Floodgate is using a parallel execution model.
    inline bool GetParallelExecution(); 

    /**
        Sets whether or not Floodgate should operate with a parallel execution
        model. Floodgate will operate with a parallel execution model by 
        default. In general serial execution should only turned on when
        operating on a single core machine. 

        @param bParallelExecution Boolean defining if parallel execution should
        be used.
        @return Returns if the execution model was set.
    */
    bool SetParallelExecution(bool bParallelExecution);

    /**
        Adjust the number of worker threads used. Specifying a worker thread
        count of zero will also shutdown the dispatcher thread and cause Floodgate
        to operate in serial mode.

        @param uiWorkerThreadCount NiUInt32 defining the number of desired
        worker threads.
        @return Returns if the worker thread count was successfully changed.
    */
    bool SetWorkerThreadCount(NiUInt32 uiWorkerThreadCount);

    /**
        Call-back function signature for determining the number of worker 
        threads to be used by Floodgate.
    */
    typedef NiUInt32 (*DefineWorkerThreadCount)();

    /**
        Sets a callback method for Floodgate to use when determining the number
        of worker threads to create.
    */
    static void SetDefineWorkerThreadCountFunc(
        DefineWorkerThreadCount pfnCallback);

    /**
        Gets a callback method for Floodgate to use when determining the number
        of worker threads to create.
    */
    static DefineWorkerThreadCount GetDefineWorkerThreadCountFunc();

    /// The default DefineWorkerThreadCount.
    static NiUInt32 DefineWorkerThreadCountDefaultFunc();

    /**
        Call-back function signature for assigning the processor affinity for
        the dispatcher thread.

        @param kProcessorAffinity Input/Output. The NiProcessorAffinity that
        will be configured by this function. 
    */
    typedef void (*AssignDispatcherThreadAffinity)(
        NiProcessorAffinity& kProcessorAffinity);

    /**
        Sets a callback method for Floodgate to use when determining the 
        thread affinity of the dispatcher thread.
    */
    static void SetDispatchThreadAffinityFunc(
        AssignDispatcherThreadAffinity pfnCallback);

    /**
        Gets a callback method for Floodgate to use when determining the 
        processor affinity of the dispatcher thread.
    */
    static AssignDispatcherThreadAffinity GetDispatchThreadAffinityFunc();

    /// The default AssignDispatcherThreadAffinity.
    static void AssignDispatcherThreadAffinityDefaultFunc(
        NiProcessorAffinity& kProcessorAffinity);
#endif



#if !defined(_WII)
    /// Returns the number of active worker threads.
    NiUInt32 GetWorkerThreadCount();

    /// Sets processor affinity.
    inline bool SetAffinity(const NiProcessorAffinity& kAffinity);

    /// Sets thread priority.
    inline bool SetPriority(NiThread::Priority ePriority);
#endif

    /**
        Gets a workflow from the object pool.
        
        Calls NiSPWorkflow::GetFreeWorkflow.
    */
    static NiSPWorkflowPtr GetFreeWorkflow();
    
    /// Clears the workflow queue.
    void Clear();

    /// Clears the given workflow from the queue.
    void Clear(NiSPWorkflow* pkWorkflow);

    /// Determines if a workflow has completed.
    bool Poll(const NiSPWorkflow* pkWorkflow);

    /**
        Waits for a workflow to complete with optional timeout.
        
        @param pkWorkflow Workflow to check for completion.
        @param uiTimeout Approximate timeout in microseconds. A timeout of 
            zero will return immediately. The default timeout of 0xFFFFFFFF 
            specifies an infinite wait. Note that since this function will
            attempt to yield the thread, the granularity of the timeout may be
            much larger than microseconds depending on the underlying speed 
            of thread context switching per platform. Applications should not
            rely on this value for any form of accurate timing.
        @return Returns true if task completed; false if timeout expired.
    */
    bool Wait(const NiSPWorkflow* pkWorkflow, NiUInt32 uiTimeout = 0xFFFFFFFF);

    /**
        Submits a workflow to be executed by the manager.

        @param pkWorkflow Workflow to submit for execution.
        @param ePriority Execution priority for the workflow.
        @return Returns true if the workflow was added; false if the 
            queue was full and the workflow could not be added. The queue size
            should be increased if this condition is detected.
    */
    bool Submit(NiSPWorkflow* pkWorkflow, Priority ePriority);

    /// @cond EMERGENT_INTERNAL 

    enum Status
    {
        STOPPED,
        STOPPING,
        RUNNING
    };

    /// This value controls the priority of the scheduler thread (and the
    /// receive thread on PS3).
    static NiThread::Priority ms_eThreadPriority;
    
    /// Starts the manager.
    Status Start();

    /// Stops the manager.
    Status Stop();

    /// Gets the status of the manager.
    Status GetStatus() const;

    /// Indicates if manager is in running state.
    bool IsRunning();

    /// Returns true if the manager exists and active.
    static bool IsActive();

#if !defined(_WII)
    /// Gets the background thread.
    const NiThread* GetThread() const;

    /// Gets the thread pool.
    NiSPThreadPool* GetThreadPool();
#endif

    /// Gets the active number of workflows currenty in the submission queues.
    const NiUInt32 GetActiveWorkflowCount() const;    

    /// Initializes object pools.
    static void InitializePools();

    /// Shuts down object pools.
    static void ShutdownPools();

    /// Creates the NiStreamProcessor singleton.
    static void Create(NiUInt32 uiMaxQueueSize);

    /// Destroys the NiStreamProcessor singleton.
    static void Destroy();

    /// Initializes the streaming task manager. Called by NiFloodgateSDM.
    static bool Initialize();

    /// Shuts down the streaming task manager. Called by NiFloodgateSDM.
    static bool Shutdown();

#if defined(WIN32) || defined(_XENON)
    /** 
        Submits a workflow for finalization. 
        This function should only be called by NiSPThreadPool after all 
        work on a Floodgate workflow has been completed.
        @param pkWorkflow Completed workflow requiring finalization.
     */
    void SubmitForFinalization(NiSPWorkflow* pkWorkflow);
#endif
    
    /// @endcond 
    
protected:
#if !defined(_WII)
    /// Manager procedure used to manage the execution and completion of
    /// NiSPWorkflows on a background thread.
    class ManagerProc : public NiThreadProcedure
    {
    public:
        ManagerProc(NiStreamProcessor* pkManager);
        virtual unsigned int ThreadProcedure(NiThread* pkThread);
        const NiUInt32 GetActiveWorkflowCount() const;
        const NiUInt32 GetActiveWorkflowQueueCount() const;

    protected:
        typedef NiTObjectQueue<NiSPWorkflowPtr> NiSPWorkflowQueue;
        NiSPWorkflowQueue m_akWorkflowQueues[NUM_PRIORITIES];

        NiThread* m_pkThread;
        NiStreamProcessor* m_pkManager;
        NiUInt32 m_uiActiveWorkflowCount;
    };
#endif

    /// Protected constructor.
    NiStreamProcessor(NiUInt32 uiMaxQueueSize);    

    /// Low level Initialization.
    bool InitializeInternal();

    /// Low level for Start().
    NiStreamProcessor::Status StartInternal();

    /// Low level for Stop().
    NiStreamProcessor::Status StopInternal();

    /// Low level receive.
    void Receive();

    /// Low level for Shutdown().
    bool ShutdownInternal();
    
    /// Executes a workflow.
    bool ExecuteOneWorkflow(NiSPWorkflowPtr spWorkflow);

    /// Singleton instance.
    static NiStreamProcessor* ms_pkInstance;   

#if !defined(_PS3) && !defined(_WII)
    /// Serial execution of a workflow.
    bool ExecuteOneWorkflowSerial(NiSPWorkflowPtr spWorkflow);  

    /// Function pointer to the active DefineWorkerThreadCount function.
    static DefineWorkerThreadCount ms_pfnDefineWorkerThreadCountFunc;

    /// Function pointer to the active AssignDispatcherThreadAffinity function.
    static AssignDispatcherThreadAffinity 
        ms_pfnAssignDispatcherThreadAffinityFunc;
#endif

    /// Type for a lookup table of workflows, implemented as an array because
    /// iteration is one of the primary operations, the number of elements is
    /// likely to be small, and only some platforms use it.
    typedef NiTObjectArray<NiSPWorkflowPtr> NiSPWorkflowPtrMap;

    // Declare the atomic SLIST items first. They are aligned and generate
    // warnings if previous data members disrupt that alignment and 
    // introduce padding to the class.
#if defined(WIN32) || defined(_XENON)
    /// Free entries for the finalization queue.
    SLIST_HEADER_GB m_kFreeFinalizationEntries;

    /// Workflows requiring finalization.
    SLIST_HEADER_GB m_kFinalizationStack;

    class FinalizationEntry : public NiMemObject
    {
    public:
        FinalizationEntry() {m_kListEntry.Next = NULL; m_pkWorkflow = NULL;};
        SLIST_ENTRY_GB m_kListEntry;
        NiSPWorkflow* m_pkWorkflow;
    };
#endif

    /// Lookup table for workflows.
    NiSPWorkflowPtrMap m_kWorkflows;        

    /// Serializes access to queues.
    NiCriticalSection m_kManagerLock;       

    /// Status of NiStreamProcess.
    Status m_eStatus;                       

    /// Max number of workflows per queue.
    NiUInt32 m_uiMaxQueueSize;              

#if !defined(_WII)
    /// Thread pool.
    NiSPThreadPool* m_pkThreadPool;         
    
    /// Workflow submission and completion thread.
    NiThread* m_pkSubmitThread;             

    /// Workflow submission thread procedure.
    ManagerProc* m_pkSubmitThreadProc;      

    /// Manages execution of workflow submission to the manager thread.
    NiSemaphore m_kSubmitSemaphore;

    /// Event queue from main thread to manager thread.
    NiFloodgateEventQueue* m_pkEventQueue;
#endif

    /// Task dependency analyzer.
    NiSPAnalyzer m_kAnalyzer;   

    /// Indicates if the stream processor is active.
    bool m_bIsActive;

#if defined(_PS3)
    friend class PPUTaskProc;

    static const unsigned int NUM_PPU_TASK_THREADS = 2;
    NiThread* m_pkPPUTaskThread[NUM_PPU_TASK_THREADS]; 
    NiThreadProcedure* m_pkPPUTaskThreadProc[NUM_PPU_TASK_THREADS];

    void FinishAvailableCompletedWorkflows();
#elif !defined(_WII)
    /// Indicates if the stream processor is operating in parallel execution
    /// mode (default on non-Wii platforms).
    bool m_bParallelExecution;
#endif

};

#include "NiStreamProcessor.inl"

#endif  // #ifndef NISTREAMPROCESSOR_H
