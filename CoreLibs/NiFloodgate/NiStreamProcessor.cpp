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

//---------------------------------------------------------------------------
#include "NiFloodgatePCH.h"
#include "NiStreamProcessor.h"
#include "NiFloodgateSDM.h"
//---------------------------------------------------------------------------
NiStreamProcessor* NiStreamProcessor::ms_pkInstance = NULL;
NiThread::Priority NiStreamProcessor::ms_eThreadPriority = 
    NiThread::HIGHEST;

#if !defined(_PS3) && !defined(_WII)
NiStreamProcessor::DefineWorkerThreadCount 
    NiStreamProcessor::ms_pfnDefineWorkerThreadCountFunc = 
    NiStreamProcessor::DefineWorkerThreadCountDefaultFunc;

NiStreamProcessor::AssignDispatcherThreadAffinity 
    NiStreamProcessor::ms_pfnAssignDispatcherThreadAffinityFunc = 
    NiStreamProcessor::AssignDispatcherThreadAffinityDefaultFunc;
#endif

//---------------------------------------------------------------------------
NiStreamProcessor::NiStreamProcessor(NiUInt32 uiMaxQueueSize) :
    m_eStatus(STOPPED),
    m_uiMaxQueueSize(uiMaxQueueSize),
#if !defined(_WII)
    m_pkThreadPool(NULL),
    m_pkSubmitThread(NULL),
    m_pkSubmitThreadProc(NULL),
    m_kSubmitSemaphore(0, 
        NiStreamProcessor::NUM_PRIORITIES * uiMaxQueueSize + 1),
    m_pkEventQueue(NULL),
#endif
    m_bIsActive(false)
{
#if !defined(_PS3) && !defined(_WII)
    m_bParallelExecution = true;
#endif
}
//---------------------------------------------------------------------------
NiStreamProcessor::~NiStreamProcessor()
{
}
//---------------------------------------------------------------------------
void NiStreamProcessor::InitializePools()
{
    NiSPTask::InitializePools(128);
    NiSPTaskImpl::InitializePools(128);
    NiSPWorkflow::InitializePools(128);
}
//---------------------------------------------------------------------------
void NiStreamProcessor::ShutdownPools()
{
    NiSPWorkflow::ShutdownPools();
    NiSPTaskImpl::ShutdownPools();
    NiSPTask::ShutdownPools();
}
//---------------------------------------------------------------------------
void NiStreamProcessor::Create(NiUInt32 uiMaxQueueSize)
{
    // Create instance
    ms_pkInstance = NiNew NiStreamProcessor(uiMaxQueueSize);

#if !defined(_PS3) && !defined(_WII)
    const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
    ms_pkInstance->SetParallelExecution(
        ((NiInitOptions*)pkInitOptions)->GetParallelExecution());
#endif

    NIASSERT(ms_pkInstance);
}
//---------------------------------------------------------------------------
void NiStreamProcessor::Destroy()
{
    // Delete instance
    NIASSERT(ms_pkInstance);
    NiDelete ms_pkInstance;
    ms_pkInstance = NULL;
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::Initialize()
{
    if (ms_pkInstance)
    {
        ms_pkInstance->InitializeInternal();
        ms_pkInstance->Start();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::Shutdown()
{
    // Stop the manager and wait for background thread to complete
    ms_pkInstance->Stop();

    // Perform internal shutdown
    ms_pkInstance->ShutdownInternal();

    return true;
}
//---------------------------------------------------------------------------
#if !defined(_PS3) && !defined(_WII)
//---------------------------------------------------------------------------
bool NiStreamProcessor::ExecuteOneWorkflowSerial(NiSPWorkflowPtr spWorkflow)
{
    // Ensure that we were not given a workflow that is currently being
    // executed. Since we are operating with a serial execution model 
    // this should always be the case.
    NIASSERT(spWorkflow->GetStatus() == NiSPWorkflow::PENDING);

    // Attempt to prepare the task
    if (spWorkflow->Prepare())
    {
        // Determine dependencies and 
        // execution order of contained tasks
        m_kAnalyzer.Analyze(spWorkflow);

        // Set workflow id on manager hash table
        NIASSERT(spWorkflow->GetId() != 0)

        // Update status and execute
        spWorkflow->SetStatus(NiSPWorkflow::RUNNING);

        spWorkflow->ExecuteSerial();

        // Ensure that the workflow completed. Since we are operating
        // with a serial execution model this should always be the case.
        NIASSERT(spWorkflow->GetStatus() == NiSPWorkflow::COMPLETED);

        return true;
    }

    return false;
}
#endif
//---------------------------------------------------------------------------
bool NiStreamProcessor::Poll(const NiSPWorkflow* pkWorkflow)
{
    NIASSERT(pkWorkflow);

    NiSPWorkflow::Status eStatus = pkWorkflow->GetStatus();
    if (eStatus == NiSPWorkflow::COMPLETED || eStatus == NiSPWorkflow::ABORTED)
    {
        // Memory synch on consoles.  Win32 handles it in the compiler via
        // the "volatile" keyword.  Wii doesn't need to sync since we are
        // always in serial execution mode.
#if defined(_XENON) || defined(_PS3)
        // We read a volatile flag that indicated the workflow was
        // complete. We need a memory barrier to insure that any reads
        // of the results are at least as current as that flag.
        __lwsync();
#endif
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
bool NiStreamProcessor::Wait(const NiSPWorkflow* pkWorkflow, 
    NiUInt32 uiTimeout)
{
    // Poll the workflow.
    bool bDone = Poll(pkWorkflow);

    // If we're finished or the timeout is zero, return immediately without
    // executing any timeout logic or yielding the thread.
    if (bDone || !uiTimeout)
        return bDone;

    bool bInfinite = false;
    if (uiTimeout == 0xFFFFFFFF)
        bInfinite = true;

    NiUInt64 uiTimeoutTicks = 0;
    if (!bInfinite)
    {
        // Get an approximate number of ticks until the timeout occurs. Note
        // that we multiply by the frequency before dividing for increased 
        // precision since some platforms have performance counters in the 
        // 1-10 MHz range.
        NiUInt64 uiTicks = 
            ((NiUInt64)NiSystemDesc::GetSystemDesc().
            GetPerformanceCounterHz() * uiTimeout) / 1000000;

        // Determine the number of ticks returned from NiGetPerformanceCounter
        // when the timeout expires. Note that we scale the input argument in
        // microseconds to the performance counter's frequency.
        uiTimeoutTicks = NiGetPerformanceCounter() + uiTicks;
    }
    
    // Wait until our workflow is done or timeout has occurred
    while (bInfinite || (NiGetPerformanceCounter() < uiTimeoutTicks))
    {
        if (Poll(pkWorkflow))
            return true;
        NiYieldThread();
    }
    return false;
}
//---------------------------------------------------------------------------
NiStreamProcessor::Status NiStreamProcessor::Start()
{
    if (m_eStatus == NiStreamProcessor::RUNNING)
        return m_eStatus;

#if !defined(_WII)
    m_eStatus = NiStreamProcessor::RUNNING;

#if !defined(_PS3)
    if (!GetParallelExecution())
    {
        NIASSERT(m_pkSubmitThreadProc == NULL);
        NIASSERT(m_pkSubmitThread == NULL);

        m_eStatus = StartInternal();
        return m_eStatus;
    }
#endif

    m_pkSubmitThreadProc = NiNew NiStreamProcessor::ManagerProc(this);
    NIASSERT(m_pkSubmitThreadProc);

    m_pkSubmitThread = NiThread::Create(m_pkSubmitThreadProc);
    m_pkSubmitThread->SetPriority(ms_eThreadPriority);

#if !defined(_PS3)
    NiProcessorAffinity kAffinity;
    ms_pfnAssignDispatcherThreadAffinityFunc(kAffinity);
    m_pkSubmitThread->SetThreadAffinity(kAffinity);
#endif

    NIASSERT(m_pkSubmitThread);
    m_pkSubmitThread->SetName("Floodgate: Submit Thread");
    m_pkSubmitThread->Resume();
#endif
    
    m_eStatus = StartInternal();

    return m_eStatus;
}
//---------------------------------------------------------------------------
NiStreamProcessor::Status NiStreamProcessor::Stop()
{
    // Signal that we are stopping
    m_kManagerLock.Lock();
    m_eStatus = NiStreamProcessor::STOPPING;
    m_kManagerLock.Unlock();

#if defined(_WII)
    m_eStatus = StopInternal();
#else
    // Wake up the manager thread and allow it to come to a complete stop
#if defined(_PS3)
    // Submit a dummy workflow (NULL) to wake up the manager thread
    Submit(NULL, IMMEDIATE);
#else
    // Signal the update and poll semaphore to allow the thread to complete
    m_kSubmitSemaphore.Signal();
#endif

    if (m_pkSubmitThreadProc || m_pkSubmitThread)
    {
        m_pkSubmitThread->WaitForCompletion();

        // Stop the internal (platform-specific) systems
        m_kManagerLock.Lock();
        m_eStatus = StopInternal();
        
        NiDelete m_pkSubmitThread;
        NiDelete m_pkSubmitThreadProc;
        m_pkSubmitThread = 0;
        m_pkSubmitThreadProc = 0;
        m_kManagerLock.Unlock();
    }
#endif

    return m_eStatus;
}
//---------------------------------------------------------------------------
void NiStreamProcessor::Clear(NiSPWorkflow* pkWorkflow)
{
    m_kManagerLock.Lock();

    pkWorkflow->Reset();
    pkWorkflow->SetStatus(NiSPWorkflow::IDLE);

    m_kManagerLock.Unlock();
}
//---------------------------------------------------------------------------
const NiUInt32 NiStreamProcessor::GetActiveWorkflowCount() const
{
#if defined(_WII)
    return 0;
#elif defined(_PS3)
    return m_pkSubmitThreadProc->GetActiveWorkflowCount();
#else
    if (m_bParallelExecution)
        return m_pkSubmitThreadProc->GetActiveWorkflowCount();
    return 0;
#endif
}
//---------------------------------------------------------------------------
#if !defined(_WII)
//---------------------------------------------------------------------------
// ManagerProc functions.
//---------------------------------------------------------------------------
NiStreamProcessor::ManagerProc::ManagerProc(NiStreamProcessor* pkManager) : 
    m_pkManager(pkManager),
    m_uiActiveWorkflowCount(0)
{
}
//---------------------------------------------------------------------------
const NiUInt32 NiStreamProcessor::ManagerProc::GetActiveWorkflowCount() const
{
    return m_uiActiveWorkflowCount;
}
//---------------------------------------------------------------------------
const NiUInt32 NiStreamProcessor::ManagerProc::GetActiveWorkflowQueueCount() 
    const
{
    NiUInt32 uiActiveCount = 0;
    for (NiInt32 iPriority = IMMEDIATE; iPriority >= 0; iPriority--)
    {
        uiActiveCount += m_akWorkflowQueues[iPriority].GetSize();
    }

    return uiActiveCount;
}
//---------------------------------------------------------------------------
#endif
