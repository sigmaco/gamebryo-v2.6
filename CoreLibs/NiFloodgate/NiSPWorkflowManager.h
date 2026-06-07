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

#ifndef NISPWORKFLOWMANAGER_H
#define NISPWORKFLOWMANAGER_H

#include "NiFloodgateLibType.h"
#include "NiStreamProcessor.h"
#include "NiSystem.h"
#include "NiTPtrSet.h"
#include "NiTFixedStringMap.h"
#include "NiCriticalSection.h"

/**
    The NiSPWorkflowManager class handles the submission and buffering of
    tasks for placement into workflows for execution.
    
    There are two primary reasons for its existence:
    
    (1) On some platforms, specific sized workflows are more efficient than
        arbitrarily sized workflows.

    (2) Under some circumstances, it is convenient for tasks to be buffered
        before submission.
    
    Task groups are used to prevent tasks from appearing in the same workflow.
    Only tasks with the same task group will appear in the same workflow.
    They are useful when an application wishes to delay the submission of
    certain tasks. The delayed tasks can be given their own group ID, and
    they will be placed in the same workflow. Other tasks will be placed in
    other workflows and may be submitted sooner.
    
    Tasks can be marked for delayed execution at the time they are added.
    Delayed tasks must be explicitly forced to submit for execution through
    a call to the FlushTaskGroup function.
    
    @note This class is not thread-safe. Each thread in an application should
        use a separate instance.
*/
class NIFLOODGATE_ENTRY NiSPWorkflowManager : public NiMemObject
{
public:
    /**
        Constructor.
        
        @param uiSubmitThreshold The minimum number of tasks to put in a
            single workflow before submitting the workflow. More tasks may be
            placed in the workflow if they are releated.
        @param eWorkflowPriority The priority to use for submitted workflows.
        @param uiNumTaskGroups The number of task groups for which to allocate
            space. This is an approximate estimate used to size an internal
            map, and should be a prime number for good performance.
    */
    NiSPWorkflowManager(const NiUInt16 uiSubmitThreshold,
        const NiStreamProcessor::Priority eWorkflowPriority =
        NiStreamProcessor::MEDIUM, const NiUInt32 uiNumTaskGroups = 3);

    /// Destructor.
    ~NiSPWorkflowManager();
    
    /**
        Set the submit threshold value.
        
        The submit threashold is the minimum number of tasks a workflow must
        have before it is automatically submitted. A workflow with fewer
        tasks can always be flushed to force submission.
        
        The submit threshold should be set on a per application, or even per
        workflow manager, basis to optimize the trade-off between delaying
        workflow submissions versus having maximally sized workflows. In cases
        where tasks are rapidly submitted to the manager and any given task
        can afford to wait a little on submission, the submit threshold can
        be larger. If tasks come in infrequently or it is important that each
        start as soon as possible, then the submit threshold should be lower.
        
        Note that on some platforms the submit threshold may need to be raised
        to limit the maxium number of workflows outstanding at any given
        moment. A higher threshold means more tasks per workflow and hence
        fewer workflows for a given application.
    */
    void SetSubmitThreshold(const NiUInt16 uiSubmitThreshold);
    
    /// Get the submit threshold value.
    inline NiUInt16 GetSubmitThreshold() const;
    
    /// Set the priority used for submitted workflows.
    void SetWorkflowPriority(const NiStreamProcessor::Priority ePriority);
    
    /// Get the priority used for submitted workflows.
    inline NiStreamProcessor::Priority GetWorkflowPriority() const;
    
    /**
        Adds a task.
        
        If the task group does not yet exist, it will be created. The workflow
        for the task may be submitted, so do not use this function if the task
        must be in the same workflow as other tasks.
        
        @param pkTask The task to add.
        @param uiTaskGroup The group the task belongs to.
        @param bDelaySubmission If true, do not submit the task's workflow
            until the manager is flushed.
        @return The workflow the task has been assigned to. This must be
            assigned to a smart pointer.
    */
    NiSPWorkflowPtr AddTask(NiSPTask* pkTask, const NiUInt32 uiTaskGroup,
        const bool bDelaySubmission);

    /**
        Adds one task of several that should go into a common workflow.

        If the task group does not yet exist, it will be created. This
        function can be called multiple times to add several tasks to the same
        workflow. You must call FinishRelatedTasks to indicate that a set of
        related tasks have all been added.
        
        @param pkTask The task to add.
        @param uiTaskGroup The group the task belongs to.
        @param bDelaySubmission If true, do not submit the task's workflow
            until the manager is flushed.
        @return The workflow the task has been assigned to. This must be
            assigned to a smart pointer.
    */
    NiSPWorkflowPtr AddRelatedTask(NiSPTask* pkTask, 
        const NiUInt32 uiTaskGroup, const bool bDelaySubmission);

    /**
        Finishes off submission of related tasks.
        
        If the workflow is full, it will be submitted or queued for later
        submission.
        
        @param uiTaskGroup The group the task belongs to.
        @param uiTaskGroupMask A mask used in matching up groups. The task
            group with ID KeyID will be flushed if
            (KeyID & uiTaskGroupMask) == (uiTaskGroup & uiTaskGroupMask)
    */
    void FinishRelatedTasks(const NiUInt32 uiTaskGroup,
        const NiUInt32 uiTaskGroupMask);
        
    /**
        Flushes all the tasks for the given group.
        
        Any delayed tasks are submitted, as are any other workflows that have
        not yet been submitted.
        
        @param uiTaskGroup The group to flush.
        @param uiTaskGroupMask A mask used in matching up groups. The task
            group with ID KeyID will be flushed if
            (KeyID & uiTaskGroupMask) == (uiTaskGroup & uiTaskGroupMask)
        @return True if the flush resulted in submission of any workflows.
            False otherwise.
    */
    bool FlushTaskGroup(const NiUInt32 uiTaskGroup,
        const NiUInt32 uiTaskGroupMask);

protected:
    /// Class to hold information about the tasks and workflows for a single
    /// task group.
    class TaskGroupInfo : public NiMemObject
    {
    public:
        /// Constructor.
        TaskGroupInfo(const NiUInt16 uiSubmitThreshold,
            const NiStreamProcessor::Priority eWorkflowPriority);
        
        /// Destructor.
        ~TaskGroupInfo();
        
        /// Current, partially full workflow for immediate tasks.
        NiSPWorkflowPtr m_spImmediateWorkflow;
        
        /// Current, partially full workflow for delayed tasks.
        NiSPWorkflowPtr m_spDelayedWorkflow;
        
        /// Number of tasks currently in immediate submission workflow.
        NiUInt32 m_uiNumTasksImmediate;

        /// Number of tasks currently in delayed submission workflow.
        NiUInt32 m_uiNumTasksDelayed;
        
        /// Set of delayed workflows.
        NiTObjectSet<NiSPWorkflowPtr> m_kDelayedWorkflows;
        
        /// Priority for workflows.
        NiStreamProcessor::Priority m_eWorkflowPriority;
        
        /// Min number of tasks to force submission.
        NiUInt16 m_uiSubmitThreshold;

        /// Adds a task that can begin immediately but cannot be submitted
        /// until related tasks are also added.
        NiSPWorkflowPtr AddRelatedImmediateTask(NiSPTask* pkTask);
        
        /// Closes out addition of related tasks. 
        void FinishRelatedImmediateTasks();

        /// Adds a task that should be delayed until a flush and that must
        /// appear in the same workflow as other tasks.
        NiSPWorkflowPtr AddRelatedDelayedTask(NiSPTask* pkTask);

        /// Closes out addition of related tasks. 
        void FinishRelatedDelayedTasks();

        /// Flushes queues and pending workflows.
        bool Flush();

        /// Flushes immediate workflows that are full.
        bool FlushImmediate();
    };
    
    /// Deletes all task group info from the map.
    void ClearTaskGroupInfoMap();

    /// Gets a task group, creating it if needed.
    TaskGroupInfo* GetTaskGroup(NiUInt32 uiTaskGroup);

    /// Type for storing workflow information.
    typedef NiTMap<NiUInt32, TaskGroupInfo*> TaskGroupInfoMap;
    
    /// Map holding the task group info for all the task groups we know about.
    TaskGroupInfoMap m_kTaskGroupInfoMap;

    /// The priority given to submitted workflows.
    NiStreamProcessor::Priority m_eWorkflowPriority;

    /// The minimum threshold that will result in a workflow being submitted.
    NiUInt16 m_uiSubmitThreshold;
};

#include "NiSPWorkflowManager.inl"

#endif  // #ifndef NISPWORKFLOWMANAGER_H
