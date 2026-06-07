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
#include "NiSystemPCH.h"

#include "NiThread.h"
#include "NiVersion.h"

NiTThreadLocal<NiThread*> NiThread::ms_kTLSCurrentThread;
//---------------------------------------------------------------------------
NiThread::NiThread(NiThreadProcedure* pkProcedure, unsigned int uiStackSize):
    m_eStatus(SUSPENDED),
    m_uiReturnValue(0xFFFFFFFF),
#if defined(WIN32) || defined(_XENON)
    m_hThread(0), 
#elif defined(_WII)
    m_stack(0),
#endif  
    m_pcName(0)
{
    SetStackSize(uiStackSize);
    NIASSERT(pkProcedure);
    SetProcedure(pkProcedure);
}
//---------------------------------------------------------------------------
NiThread::~NiThread()
{
    WaitForCompletion();

    m_pkProcedure = 0;
#if defined(WIN32) || defined(_XENON)
    if (m_hThread)
        CloseHandle(m_hThread);
    m_hThread = 0;
#elif defined(_PS3)
    pthread_detach(m_kThreadID);
    pthread_mutex_destroy(&m_kMutexID);
#elif defined(_WII)
    SystemFreeThreadResources();
#endif  

    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
NiThread* NiThread::Create(NiThreadProcedure* pkProcedure, 
    unsigned int uiStackSize)
{
    NiThread* pkThread = NiNew NiThread(pkProcedure, uiStackSize);
    if (pkThread)
    {
        if (!pkThread->SystemCreateThread())
        {
            NiDelete pkThread;
            pkThread = 0;
        }
    }
    return pkThread;
}
//---------------------------------------------------------------------------
bool NiThread::SetPriority(NiThread::Priority ePriority)
{
    return SystemSetPriority(ePriority);
}
//---------------------------------------------------------------------------
int NiThread::Suspend()
{
    return SystemSuspend();
}
//---------------------------------------------------------------------------
int NiThread::Resume()
{
    return SystemResume();
}
//---------------------------------------------------------------------------
bool NiThread::WaitForCompletion()
{
    return SystemWaitForCompletion();
}
//---------------------------------------------------------------------------
#if defined (WIN32) || defined (_XENON)
DWORD WINAPI NiThread::ThreadProc(void* pvArg)
{
    NiThread* pkThread = (NiThread*)pvArg;
    ms_kTLSCurrentThread = pkThread;
    // Verify that the thread local storage is what we expect
    NIASSERT(ms_kTLSCurrentThread == pkThread);

    pkThread->m_uiReturnValue = pkThread->GetProcedure()->ThreadProcedure(
        pkThread);
    // NOTE: the setting of m_eStatus here is not entirely thread-safe
    // with the checking of it in SystemWaitForCompletion().  However,
    // because the return value has already been set, the only side-effect
    // is a potentially stale return value in SystemWaitForCompletion().
    unsigned int uiReturn = pkThread->m_uiReturnValue;
    pkThread->m_eStatus = COMPLETE;
    return uiReturn;
}
//---------------------------------------------------------------------------
#elif defined (_PS3)
void* NiThread::ThreadProc(void* pvArg)
{
    NiThread* pkThread = (NiThread*)pvArg;
    ms_kTLSCurrentThread = pkThread;
    // Verify that the thread local storage is what we expect
    NIASSERT(ms_kTLSCurrentThread == pkThread);

    NIVERIFY(pthread_mutex_lock(&pkThread->m_kMutexID) == 0);

    pkThread->m_uiReturnValue = pkThread->GetProcedure()->ThreadProcedure(
        pkThread);

    // NOTE: the setting of m_eStatus here is not entirely thread-safe
    // with the checking of it in SystemWaitForCompletion().  However,
    // because the return value has already been set, the only side-effect
    // is a potentially stale return value in SystemWaitForCompletion().
    pkThread->m_eStatus = COMPLETE;
    pthread_mutex_unlock(&pkThread->m_kMutexID);

    // Note that pthread_exit is implicitly called when this method returns.
    return NULL;
}
//---------------------------------------------------------------------------
#elif defined(_WII)
void* NiThread::ThreadProc(void* pvArg)
{
    NiThread* pkThread = (NiThread*)pvArg;
    ms_kTLSCurrentThread = pkThread;
    // Verify that the thread local storage is what we expect
    NIASSERT(ms_kTLSCurrentThread == pkThread);

    pkThread->m_uiReturnValue = pkThread->GetProcedure()->ThreadProcedure(
        pkThread);

    // NOTE: the setting of m_eStatus here is not entirely thread-safe
    // with the checking of it in SystemWaitForCompletion().  However,
    // because the return value has already been set, the only side-effect
    // is a potentially stale return value in SystemWaitForCompletion().
    pkThread->m_eStatus = COMPLETE;
    return 0;
}
#endif
//---------------------------------------------------------------------------
