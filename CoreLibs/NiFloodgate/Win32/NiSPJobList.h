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

#ifndef NISPJOBLIST_H
#define NISPJOBLIST_H

// The documentation for this class is disabled to work around a problem with
// Doxygen when multiple header files with the same class name exist.

/// @cond

#include <NiSystem.h>
#include <NiTArray.h>
#include <NiTPool.h>

#include "NiSPWorkload.h"

class NiSPTask;
class NiSPWorkflow;

/// This class manages the creation of a JobList consisting of one or more
/// NiSPJob objects.
class NIFLOODGATE_ENTRY NiSPJobList : public NiMemObject
{
public:
    /// Constructor.
    NiSPJobList(NiUInt32 uiJobCountEstimate = 1);
    
    /// Destructor.
    virtual ~NiSPJobList();

    /**
        Initializes the job list from data in the task.
        
        @param pkTask The task to initialize
        @return True if all data was processed, false if an additional job
            list is needed.
    */
    bool Initialize(NiSPTask* pkTask);

    /// Returns the number of jobs to execute.
    NiUInt32 GetJobCount();

    /// Gets the workload at the index specified.
    NiSPWorkload* GetWorkload(NiUInt32 uiIndex);
        
    /// Sets job list completion status.
    void SetIsFinished(bool bIsFinished);
    
    /// Determines if the job list is finished.
    bool IsFinished() const;

    /// Sets the workflow this joblist belongs too.
    void SetWorkflow(NiSPWorkflow* pWorkflow);

    /// Gets the workflow this joblist belongs too.
    NiSPWorkflow* GetWorkflow();

    /// Sets the workflow ID this joblist belongs too.
    void SetWorkflowID(NiUInt32 uiWorkflowID);

    /// Gets the workflow ID this joblist belongs too.
    NiUInt32 GetWorkflowID();

    /// Sets the workflow stage this joblist belongs too.
    void SetStage(NiUInt32 uiStage);

    /// Gets the workflow stage this joblist belongs too.
    NiUInt32 GetStage();

    /// @cond EMERGENT_INTERNAL 

    /// Initializes the job list related object pools.
    static void InitializePools(NiUInt32 uiWorkloadPoolSize);
    
    /// Shuts down the job list related object pools.
    static void ShutdownPools();
    /// @endcond

protected:
    typedef NiTObjectPool<NiSPWorkload> WorkloadPool;

    /// Pool of NiSPWorkload objects.
    static WorkloadPool* ms_pkWorkloadPool;

    /// Critical section for accessing the Workload pool.
    static NiCriticalSection ms_kWorkloadPoolLock;

    /// Maximum number of workloads we can support, which matches the size
    /// of the m_uiJobCount variable.
    static NiUInt32 MAX_JOB_COUNT;

    /// Builds a single job.
    void Build(NiUInt32 uiJobIndex, NiSPTask* pkTasks);

    /// Workloads.
    NiTPrimitiveArray<NiSPWorkload*> m_kWorkloads;
    
    /// The workflow this joblist belongs to
    NiSPWorkflow* m_pkWorkflow;

    /// The workflow this joblist belongs to
    NiUInt32 m_uiWorkflowId;

    /// The stage which this joblist is in
    NiUInt32 m_uiStage;

    /// Number of active workloads.
    NiUInt16 m_uiJobCount;

    /// Indicates if all work is done.
    bool m_bIsFinished;          
};

#include "NiSPJobList.inl"

/// @endcond

#endif  // #ifndef NISPJOBLIST_H
