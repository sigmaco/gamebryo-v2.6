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
#include "NiSystemPCH.h"

#include "NiThread.h"

//---------------------------------------------------------------------------
bool NiThread::SystemCreateThread()
{
    if (!m_pkProcedure)
        return false;

    m_hThread = CreateThread(NULL, m_uiStackSize, 
        ThreadProc, this, CREATE_SUSPENDED, NULL);
    if (m_hThread == 0)
        return false;

    m_ePriority = NORMAL;
    m_eStatus = SUSPENDED;

    // Initialize the thread affinity based on the process affinity
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;

    if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask,
        &systemAffinityMask))
    {
        m_kAffinity.SetAffinityMask(
            static_cast<unsigned int>(processAffinityMask));
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiThread::SystemSetPriority(Priority ePriority)
{
    if (m_ePriority != ePriority)
    {
        int iPriority;

        switch (ePriority)
        {
        case ABOVE_NORMAL:
            iPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case BELOW_NORMAL:
            iPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case HIGHEST:
            iPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case IDLE:
            iPriority = THREAD_PRIORITY_IDLE;
            break;
        case LOWEST:
            iPriority = THREAD_PRIORITY_LOWEST;
            break;
        case NORMAL:
            iPriority = THREAD_PRIORITY_NORMAL;
            break;
        case TIME_CRITICAL:
            iPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
        default:
            return false;
        }
        if (!SetThreadPriority(m_hThread, iPriority))
            return false;

        m_ePriority = ePriority;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiThread::SystemSetAffinity(const NiProcessorAffinity& kAffinity)
{
    DWORD_PTR uiMask = kAffinity.GetAffinityMask();

    // Try to get the process affinity and use it to mask the thread affinity
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;

    if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask,
        &systemAffinityMask))
    {
        uiMask &= processAffinityMask;
    }

    DWORD_PTR uiPrevMask = SetThreadAffinityMask(m_hThread, uiMask);

#if defined(NIDEBUG)
    if (uiPrevMask == 0)
    {
        NiOutputDebugString("NiThread::SystemSetAffinity failed.\n");
        DWORD_PTR uiSystemMask = 0;
        DWORD_PTR uiProcessMask = 0;
        BOOL bRet = GetProcessAffinityMask(GetCurrentProcess(),
            &uiProcessMask, &uiSystemMask);
        if (bRet)
        {
            char acOutput[64];
            NiSprintf(acOutput, 64, "Affinity Mask: 0x%08X\n", uiMask);
            NiOutputDebugString(acOutput);
            NiSprintf(acOutput, 64, "Process Affinity Mask: 0x%08X\n", 
                uiProcessMask);
            NiOutputDebugString(acOutput);
            NiSprintf(acOutput, 64, "System Affinity Mask: 0x%08X\n", 
                uiSystemMask);
            NiOutputDebugString(acOutput);

            if (uiMask & ~uiProcessMask)
            {
                NiOutputDebugString("Affinity mask must be a subset of "
                    "process affinity mask.\n");
            }
        }
    }
#endif

    return (uiPrevMask != 0);
}
//---------------------------------------------------------------------------
int NiThread::SystemSuspend()
{
    if (m_hThread == 0)
        return -1;

    int iRet = SuspendThread(m_hThread);
    if (iRet != -1)
        m_eStatus = SUSPENDED;
    return iRet;
}
//---------------------------------------------------------------------------
int NiThread::SystemResume()
{
    if (m_hThread == 0)
        return -1;

    int iPreviousSuspendCount = ResumeThread(m_hThread);
    switch (iPreviousSuspendCount)
    {
    case -1: 
        break;
    case 0: // fall through
    case 1:
        m_eStatus = RUNNING;
        break;
    default:
        m_eStatus = SUSPENDED;
    }
    return iPreviousSuspendCount;
}
//---------------------------------------------------------------------------
bool NiThread::SystemWaitForCompletion()
{
    if (m_eStatus == RUNNING)
    {
        WaitForSingleObject(m_hThread, INFINITE);

        // Set the status to complete here because it fixes a race
        // condition between the ThreadProc setting COMPLETE and then
        // SystemResume setting RUNNING.  The value might be stale (i.e.
        // the thread is complete, but says it is running) between
        // Resume/WaitForCompletion, but it will be correct after a
        // WaitForCompletion().
        m_eStatus = COMPLETE;

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
