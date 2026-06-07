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

#ifndef NISPWORKFLOW_H
#define NISPWORKFLOW_H

#include "NiFloodgateLibType.h"

#include <NiSystem.h>
#include <NiSmartPointer.h>
#include <NiTPtrSet.h>
#include <NiTPool.h>

#include "NiSPKernel.h"
#include "NiSPTask.h"
#include "NiSPThreadPool.h"

#if !defined(_WII)
#include "NiSPWorkflowImpl.h"
#endif

class NiSPWorkflow;
NiSmartPointer(NiSPWorkflow);

/**
    Wrapper around an acyclic directed graph of NiSPTask objects.
    
    Each NiSPTask represents a node of this graph while each NiSPStream that
    is bound to 2 or more NiSPTask objects represents an edge of the graph.
*/
class NIFLOODGATE_ENTRY NiSPWorkflow : public NiMemObject
{
public:

    /// Workflow status.
    enum Status
    {
        /// The workflow is not currently in use
        IDLE = 0,
        
        /// The workflow is ready for execution but has not yet been started
        PENDING,
        
        /// The workflow is in currently executing 
        RUNNING,
        
        /// The workflow has completed successfully
        COMPLETED,
        
        /// The workflow was aborted
        ABORTED
    };

    /// Objects derived from this class are used to process workflows
    /// upon completion. Applications should not use this class.
    class CompletionHandler : public NiMemObject
    {
    public:
        
        /// @cond EMERGENT_INTERNAL

        virtual ~CompletionHandler(){}
        virtual void OnCompletion(NiSPWorkflow* pkWorkflow) = 0; 
        
        /// @endcond
    };


    /** @name Commonly Used Functions
    
        While there are many functions for this class, only a few are
        regularly called by applications. The other functionality is
        used by the NiStreamProcessor class and related classes to
        manage workflows.
    */
    /// @{
    /**
        Gets a workflow from the object pool.
        
        This function should always be used to obtain a new workflow.
    */
    static NiSPWorkflowPtr GetFreeWorkflow();

    /**
        Adds a task to the workflow.

        @param pkTask Task to add to the workflow.
    */
    void Add(NiSPTask* pkTask);

    /**
        Obtains a new task from the task pool and adds it to the workflow.

        The returned task should be stored in a smart pointer if it should
        live beyond the life of the workflow.
        
        @param uiNumInputs A hint as to the number of input streams the
             task will have.
        @param uiNumOutputs A hint as to the number of output streams the
             task will have.
        @param bIsSignalTask Set this true if the task is to be a signal task
            that is used to indicate that a workflow is finished. Applications
            will almost never specify this - it is handled automatically.
    **/
    NiSPTask* AddNewTask(const NiUInt16 uiNumInputs = 1,
        const NiUInt16 uiNumOutputs = 1, const bool bIsSignalTask = false);

    /// Gets the number of tasks associated with the workflow.
    inline NiUInt32 GetSize() const;

    /// Gets the task at the specified index.
    inline NiSPTask* GetAt(NiUInt32 uiTaskIdx) const;

    /// Gets the status of a workflow.
    inline Status GetStatus() const;

    /// Clears the internal state of the workflow.
    void Clear();

    /// @}

    /** @name Additional Functions
    
        Most of the functionality of this class is accessed by the 
        NiStreamProcessor and related classes as part of workflow execution.
    */
    /// @{
    /// Gets the id for a workflow.
    inline NiUInt32 GetId() const;

    /// Gets the current stage being executed on other processing elements.
    inline NiInt32 GetCurrentStage() const;
    
    /// Sets the current stage being executed on other processing elements.
    inline void SetCurrentStage(NiInt32 iStage);

    /// Adds a job list pending on other processing elements.
    inline void AddPendingJobList(NiSPJobList* pkJobList);
    
    /// Removes the job list pending on other processing elements.
    inline void RemovePendingJobList(NiSPJobList* pkJobList);
    
    /// Clears the pending job lists.
    inline void ClearPendingJobLists();
    
    /// Gets the number of pending job lists.
    inline NiUInt32 GetNumPendingJobLists() const;
    
    /// Gets the pending job list at the index specified.
    inline const NiSPJobList* GetPendingJobList(NiUInt32 uiIndex) const;

    /// Finds a task in the workflow.
    NiSPTask* FindTask(NiUInt32 uiTaskId);

    /// Resets the workflow for new execution clearing any per execution info.
    void Reset();

    /// Handles an incoming signal that may require special processing.
    virtual void HandleSignal(NiUInt32 uiSignal);

    /// Sets the status of a workflow.
    void SetStatus(Status eStatus);

    /**
        Prepares the workflow for scheduling and execution.

        @return True if scheduling was successful, false if scheduling should
            be tried again later.
    */
    bool Prepare();
    
    /// Executes the workflow.
    inline void Execute();

#if !defined(_PS3)
    /// Serially executes the workflow.
    ///
    /// @ps3_begin Not available on PS3. @ps3_end
    void ExecuteSerial();
#endif

    /// Performs platform-specific completion of a workflow.
    void ExecutionComplete();

    /// Sets the completion handler for the workflow.
    inline void SetCompletionHandler(CompletionHandler* kHandler);

    /// Indicates if the workflow has completed execution.
    inline bool IsAtomicUpdateComplete() const;

    /// Resets the flag that tracks completion.
    inline void ResetAtomicUpdateComplete();

    /// Gets the memory address of the completion flag.
    inline NiUInt32* GetAtomicUpdateCompleteAddress();
    
    /// Sorts the tasks by their internal stage value.
    void SortTasksByStage();

    /// @}

    /// @cond EMERGENT_INTERNAL

    /**
        Initializes object pools.
        
        Called by NiStreamProcessor::InitializePools.

        @param uiWorkflowPoolSize Number of workflows to be allocated in pool.
    */
    static void InitializePools(NiUInt32 uiWorkflowPoolSize);

    /**
        Shuts down object pools.
        
        Called by NiStreamProcessor::ShutdownPools.
    */
    static void ShutdownPools();   

    /// Increments the count of references to this object.
    void IncRefCount();

    /**
        Increments the count of references to this object.
        
        If the ref count goes to 0, the object is placed back in the pool.
    */
    void DecRefCount();

    /**
        Sets the priority of this workflow.
        
        See NiStreamProcessor::Priority.
    */
    void SetPriority(int iPriority);

    /**
        Gets the priority of this workflow.
        
        See NiStreamProcessor::Priority.
    */
    int GetPriority() const;

    /// @endcond

protected:
    
    /// Replaces the task at the specified index with a different task.
    void ReplaceAt(NiUInt32 uiTaskIdx, NiSPTask* pkTask);

    /// Removes the task at the specified index.
    void RemoveAt(NiUInt32 uiTaskIdx);

    /// Allow NiTPool access to protected constructors.
    friend class NiTNewInterface<NiSPWorkflow>;

    /// Protected constructor used internally by object pool.
    NiSPWorkflow();

    /// Virtual destructor.
    virtual ~NiSPWorkflow();

    /// Releases the workflow back to the pool.
    void ReleaseWorkflow();

    /// NiSPWorkflow object pool.
    static NiTObjectPool<NiSPWorkflow>* ms_pkWorkflowPool;       

    /// Critical section for accessing the workflow pool.
    static NiCriticalSection ms_kWorkflowPoolLock;

    /// List of tasks associated with this workflow.
    NiTObjectPtrSet<NiSPTaskPtr> m_kTasks;

    /// Job lists that are pending in other threads.
    NiTPrimitiveSet<NiSPJobList*> m_kPendingJobLists; 
    
#if !defined(_WII)
    /// Private workflow implementation.
    NiSPWorkflowImpl m_kWorkflowImpl;   
#endif
    
    /// ID of workflow instance.
    NiUInt32 m_uiId;             
    
    /// The current stage in flight in other threads.
    volatile NiInt32 m_iCurrentStage;        
    
    /// Status of workflow.
    volatile Status m_eStatus;                
    
    /// Completion handler for the workflow.
    CompletionHandler* m_pkCompletionHandler;

    /// Flag used to trigger completion of the workflow.
    NiUInt32 m_uiAtomicUpdateFlag;        

private:
    /// Number of NiPointer references to this object.
    NiUInt32 m_uiRefCount;

    /// The priority of the workflow.
    int m_iPriority;
};

#include "NiSPWorkflow.inl"

#endif  // #ifndef NISPWORKFLOW_H
