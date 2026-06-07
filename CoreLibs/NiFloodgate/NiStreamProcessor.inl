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
inline NiStreamProcessor* NiStreamProcessor::Get()
{
    return ms_pkInstance;
}
//---------------------------------------------------------------------------
#if !defined(_PS3) && !defined(_WII)
inline bool NiStreamProcessor::GetParallelExecution()
{
    return m_bParallelExecution;
}
//---------------------------------------------------------------------------
inline void NiStreamProcessor::SetDefineWorkerThreadCountFunc(
    DefineWorkerThreadCount pfnCallback)
{
    if (!pfnCallback)
        return;

    ms_pfnDefineWorkerThreadCountFunc = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiStreamProcessor::DefineWorkerThreadCount 
    NiStreamProcessor::GetDefineWorkerThreadCountFunc()
{
    return ms_pfnDefineWorkerThreadCountFunc;
}
//---------------------------------------------------------------------------
inline void NiStreamProcessor::SetDispatchThreadAffinityFunc(
    AssignDispatcherThreadAffinity pfnCallback)
{
    if (!pfnCallback)
        return;

    ms_pfnAssignDispatcherThreadAffinityFunc = pfnCallback;
}
//---------------------------------------------------------------------------
inline NiStreamProcessor::AssignDispatcherThreadAffinity 
    NiStreamProcessor::GetDispatchThreadAffinityFunc()
{
    return ms_pfnAssignDispatcherThreadAffinityFunc;
}
#endif
//---------------------------------------------------------------------------
inline NiStreamProcessor::Status NiStreamProcessor::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
#if !defined(_WII)
inline const NiThread* NiStreamProcessor::GetThread() const
{
    return m_pkSubmitThread;
}
//---------------------------------------------------------------------------
inline bool NiStreamProcessor::SetAffinity(
    const NiProcessorAffinity& kAffinity)
{
    if (m_pkSubmitThread)
        return m_pkSubmitThread->SetThreadAffinity(kAffinity);
    return false;
}
//---------------------------------------------------------------------------
inline bool NiStreamProcessor::SetPriority(NiThread::Priority ePriority)
{
    if (m_pkSubmitThread)
        return m_pkSubmitThread->SetPriority(ePriority);
    return false;
}
//---------------------------------------------------------------------------
inline NiSPThreadPool* NiStreamProcessor::GetThreadPool()
{
    return m_pkThreadPool;
}
#endif
//---------------------------------------------------------------------------
inline bool NiStreamProcessor::IsActive()
{
    return (ms_pkInstance && ms_pkInstance->m_bIsActive);
}
//---------------------------------------------------------------------------
inline bool NiStreamProcessor::IsRunning()
{
    return m_eStatus == RUNNING;
}
//---------------------------------------------------------------------------
inline NiSPWorkflowPtr NiStreamProcessor::GetFreeWorkflow()
{
    return NiSPWorkflow::GetFreeWorkflow();
}
//---------------------------------------------------------------------------
