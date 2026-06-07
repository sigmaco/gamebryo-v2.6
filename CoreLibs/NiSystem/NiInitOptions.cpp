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

// Precompiled Header
#include "NiSystemPCH.h"

#include "NiInitOptions.h"
#include "NiMemTracker.h"
#include "NiStandardAllocator.h"

//---------------------------------------------------------------------------
NiInitOptions::NiInitOptions()
{
#ifdef NI_MEMORY_DEBUGGER
    m_pkAllocator = NiExternalNew NiMemTracker(
        NiExternalNew NiStandardAllocator());
#else
    m_pkAllocator = NiExternalNew NiStandardAllocator();
#endif
    m_bAllocatedInternally = true;

#if defined (_PS3)
    m_bInitSpuPrintServer = true;
    m_uiSpuCount = 5;
    m_uiJobWorkloadCount = 14;
    m_uiTaskWorkloadCount = 2;
    m_uiRecordsPerSPU = 0;
#else
    m_bParallelExecution = true;
    m_pfnDefineWorkerThreadCountFunc = NULL;
    m_pfnAssignDispatcherThreadAffinityFunc = NULL;
    m_pfnAssignWorkerThreadAffinityFunc = NULL;
#endif

}
//---------------------------------------------------------------------------
NiInitOptions::NiInitOptions(NiAllocator* pkAllocator) : 
    m_pkAllocator(pkAllocator), m_bAllocatedInternally(false)
{
    NIASSERT(m_pkAllocator);

#if defined (_PS3)
    m_bInitSpuPrintServer = true;
    m_uiSpuCount = 5;
    m_uiJobWorkloadCount = 14;
    m_uiTaskWorkloadCount = 2;
    m_uiRecordsPerSPU = 0;
#else
    m_bParallelExecution = true;
    m_pfnDefineWorkerThreadCountFunc = NULL;
    m_pfnAssignDispatcherThreadAffinityFunc = NULL;
    m_pfnAssignWorkerThreadAffinityFunc = NULL;
#endif

}
//---------------------------------------------------------------------------
NiInitOptions::~NiInitOptions()
{
    if (m_bAllocatedInternally)
        NiExternalDelete m_pkAllocator;
}
//---------------------------------------------------------------------------
NiAllocator* NiInitOptions::GetAllocator() const
{
    return m_pkAllocator;
}
//---------------------------------------------------------------------------
#if !defined(_PS3)
void NiInitOptions::SetFloodgateDefineWorkerThreadCountFunc(void* pfnCallback)
{
    NIASSERT(pfnCallback);
    m_pfnDefineWorkerThreadCountFunc = pfnCallback;
}
//---------------------------------------------------------------------------
const void* NiInitOptions::GetFloodgateDefineWorkerThreadCountFunc()
{
    return m_pfnDefineWorkerThreadCountFunc;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetDispatchThreadAffinityFunc(void* pfnCallback)
{
    NIASSERT(pfnCallback);
    m_pfnAssignDispatcherThreadAffinityFunc = pfnCallback;
}
//---------------------------------------------------------------------------
const void* NiInitOptions::GetDispatchThreadAffinityFunc()
{
    return m_pfnAssignDispatcherThreadAffinityFunc;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetWorkerThreadAffinityFunc(void* pfnCallback)
{
    NIASSERT(pfnCallback);
    m_pfnAssignWorkerThreadAffinityFunc = pfnCallback;
}
//---------------------------------------------------------------------------
const void* NiInitOptions::GetWorkerThreadAffinityFunc()
{
    return m_pfnAssignWorkerThreadAffinityFunc;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetFloodgateParallelExecution(bool bParallelExecution)
{
    m_bParallelExecution = bParallelExecution;
}
//---------------------------------------------------------------------------
const bool NiInitOptions::GetParallelExecution()
{
    return m_bParallelExecution;
}
#endif
//---------------------------------------------------------------------------
#if defined (_PS3)
bool NiInitOptions::GetInitSpuPrintServer() const
{
    return m_bInitSpuPrintServer;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetInitSpuPrintServer(bool bInitSpuPrintServer)
{
    m_bInitSpuPrintServer = bInitSpuPrintServer;
}
//---------------------------------------------------------------------------
NiUInt32  NiInitOptions::GetSpuCount() const
{
    return m_uiSpuCount;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetSpuCount(NiUInt32 uiSpuCount)
{
    m_uiSpuCount = uiSpuCount;
}
//---------------------------------------------------------------------------
NiUInt32  NiInitOptions::GetJobWorkloadCount() const
{
    return m_uiJobWorkloadCount;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetJobWorkloadCount(NiUInt32 uiJobWorkloadCount)
{
    m_uiJobWorkloadCount = uiJobWorkloadCount;
}
//---------------------------------------------------------------------------
NiUInt32 NiInitOptions::GetTaskWorkloadCount() const
{
    return m_uiTaskWorkloadCount;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetTaskWorkloadCount(NiUInt32 uiTaskWorkloadCount)
{
    m_uiTaskWorkloadCount = uiTaskWorkloadCount;
}
//---------------------------------------------------------------------------
NiUInt32 NiInitOptions::GetSpursTraceSize() const
{
    return m_uiRecordsPerSPU;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetSpursTraceSize(NiUInt32 uiRecordsPerSPU)
{
    m_uiRecordsPerSPU = uiRecordsPerSPU;
}
#endif
//---------------------------------------------------------------------------

