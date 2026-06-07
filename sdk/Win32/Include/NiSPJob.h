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

#ifndef NISPJOB_H
#define NISPJOB_H

// The documentation for this class is disabled to work around a problem with
// Doxygen when multiple header files with the same class name exist.

/// @cond

#include <NiSystem.h>
#include <NiTPtrSet.h>
#include "NiSPJobList.h"

class NiSPTask;

/** 
    This class encapsulates a set of NiSPJobLists that collectively represent
    all of the work required to complete a given task.
    
    The class breaks a given NiSPTask down into manageable subproblems which
    are later scheduled for execution.
*/
class NIFLOODGATE_ENTRY NiSPJob : public NiTPrimitivePtrSet<NiSPJobList*>
{
public:
    /// Initializes the job-related object pools.
    static void InitializePools(NiUInt32 uiJobListPoolSize);
    
    /// Shuts down the job-related object pools.
    static void ShutdownPools();

    /// Constructor.
    NiSPJob();
    
    /// Destructor.
    virtual ~NiSPJob();

    /// Initializes the job from the task specified.
    virtual void Initialize(NiSPTask* pkTask);
    
    /// Clears a job and releases job lists back to pool.
    void Clear();

protected:
    /// A type for a pool of job lists.
    typedef NiTObjectPool<NiSPJobList> JobListPool;

    /// Pool of NiSPJobList objects.
    static JobListPool* ms_pkJobListPool;

    /// Critical section for accessing the JobList pool.
    static NiCriticalSection ms_kJobListPoolLock;

    /// The task associated with this NiSPJob.
    NiSPTask* m_pkTask;
};

#include "NiSPJob.inl"

/// @endcond

#endif  // #ifndef NISPJOB_H
