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

#ifndef NIINITOPTIONS_H
#define NIINITOPTIONS_H

#include "NiSystemLibType.h"
#include "NiMemManager.h"
#include "NiUniversalTypes.h"

class NISYSTEM_ENTRY NiInitOptions
{
public:
    NiInitOptions();
    NiInitOptions(NiAllocator* pkAllocator);

    ~NiInitOptions();

    NiAllocator* GetAllocator() const;

#if !defined (_PS3)
    /// Sets a callback method for Floodgate to use when determining the number
    /// of worker threads to create.
    void SetFloodgateDefineWorkerThreadCountFunc(void* pfnCallback);

    /// Gets a callback method for Floodgate to use when determining the number
    /// of worker threads to create.
    const void* GetFloodgateDefineWorkerThreadCountFunc();

    /// Sets a callback method for Floodgate to use when determining the 
    /// thread affinity of the dispatcher thread.
    void SetDispatchThreadAffinityFunc(void* pfnCallback);

    /// Gets a callback method for Floodgate to use when determining the 
    /// processor affinity of the dispatcher thread.
    const void* GetDispatchThreadAffinityFunc();

    /// Sets a callback method for Floodgate to use when determining the 
    /// processor affinity of the worker threads.
    void SetWorkerThreadAffinityFunc(void* pfnCallback);

    /// Gets a callback method for Floodgate to use when determining the 
    /// processor affinity of the worker threads.
    const void* GetWorkerThreadAffinityFunc();

    /**
        Sets whether or not Floodgate should operate with a parallel execution
        model. Floodgate will operate with a parallel execution model by 
        default. In general parallel execution should only turned on when
        operating on a single core machine. 
    */
    void SetFloodgateParallelExecution(bool bParallelExecution);

    /// Returns whether or not Floodgate is using a parallel execution model.
    const bool GetParallelExecution(); 
#endif

private:
    NiAllocator* m_pkAllocator;

    bool m_bAllocatedInternally;

#if !defined (_PS3)
    void* m_pfnDefineWorkerThreadCountFunc;
    
    void* m_pfnAssignDispatcherThreadAffinityFunc;
    
    void* m_pfnAssignWorkerThreadAffinityFunc;

    bool m_bParallelExecution;
#endif

#if defined (_PS3)
public:
    /// Indicates if the SPU Print Server should be started
    bool GetInitSpuPrintServer() const;
    
    /// Specifies if the SPU Print Server should be started
    void SetInitSpuPrintServer(bool bInitSpuPrintServer);

    /// Gets the number of SPUs to use
    unsigned int GetSpuCount() const;
    
    /// Specifies the number of SPUS to use
    void SetSpuCount(unsigned int uiSpuCount);

    /// Gets the number of job workloads to reserve on the SPURS instance
    unsigned int GetJobWorkloadCount() const;
    
    /// Sets the number of job workloads to reserve on the SPURS instance
    void SetJobWorkloadCount(NiUInt32 uiJobWorkloadCount);

    // *** begin Emergent internal use only ***

    /// Gets the number of task workloads to reserve on the SPURS instance
    unsigned int GetTaskWorkloadCount() const;

    /// Sets the number of task workloads to reserve on the SPURS instance
    void SetTaskWorkloadCount(NiUInt32 uiTaskWorkloadCount);

    /// Gets the number of records per SPU to capture in the trace buffer.
    /// A return value of 0 indicates that the trace capture is disabled.
    NiUInt32 GetSpursTraceSize() const;

    /// Sets the number of records per SPU to capture in the trace buffer.
    /// Each record takes 16 bytes. Specifying 0 will disable the trace
    /// capture.
    void SetSpursTraceSize(NiUInt32 uiRecordsPerSPU);

    // *** end Emergent internal use only ***
private:
    bool m_bInitSpuPrintServer;
    NiUInt32 m_uiSpuCount;
    NiUInt32 m_uiJobWorkloadCount;
    NiUInt32 m_uiTaskWorkloadCount;
    NiUInt32 m_uiRecordsPerSPU;
#endif
};

#endif // #ifndef NIINITOPTIONS_H
