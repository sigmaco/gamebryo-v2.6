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

#ifndef NITHREAD_H
#define NITHREAD_H

#include "NiSystemLibType.h"
#include "NiRTLib.h"
#include "NiProcessorAffinity.h"
#include "NiThreadProcedure.h"
#include "NiMemObject.h"
#include "NiTThreadLocal.h"
#if defined(_PS3)
#include <pthread.h>
#endif

class NISYSTEM_ENTRY NiThread : public NiMemObject
{

public:
    enum
    {
        USEDEF_STACK_SIZE  = 0xffffffff,
#if defined(WIN32) || defined(_XENON)
        DEFAULT_STACK_SIZE = 0      // Let the OS pick the default stack size
#elif defined (_PS3)
        DEFAULT_STACK_SIZE = 0x4000 // 16KB 
#elif defined(_WII)
        DEFAULT_STACK_SIZE = 0x4000 // 16KB 
#endif  //#if defined(WIN32)
    };

    enum Priority
    {
        IDLE = 0,               // Base priority level
        LOWEST,                 // 2 points below normal
        BELOW_NORMAL,           // 1 point below normal
        NORMAL,
        ABOVE_NORMAL,           // 1 point above normal
        HIGHEST,                // 2 points above normal
        TIME_CRITICAL,          // Absolute highest OS priority available
        NUM_PRIORITIES
    };

    enum Status
    {
        RUNNING     = 0,
        SUSPENDED,
        COMPLETE
    };

protected:
    NiThread(NiThreadProcedure* pkProcedure,
        unsigned int uiStackSize = USEDEF_STACK_SIZE);

public:
    virtual ~NiThread();

    static NiThread* Create(NiThreadProcedure* pkProcedure, 
        unsigned int uiStackSize = USEDEF_STACK_SIZE);

    // Get the current NiThread pointer from the current thread context
    static NiThread* GetThread();

    // member access
    inline NiThreadProcedure* GetProcedure() const;

    bool SetPriority(Priority ePriority);
    inline const Priority GetPriority() const;

    inline bool SetThreadAffinity(const NiProcessorAffinity& kAffinity);
    inline const NiProcessorAffinity GetThreadAffinity();

    inline const Status GetStatus() const;
    inline const unsigned int GetReturnValue() const;

    inline void SetName(const char* pcName);
    inline const char* GetName() const;

    int Suspend();
    int Resume();
    bool WaitForCompletion();

    // *** begin Emergent internal use only ***
    inline void SetStackSize(unsigned int uiStackSize);
    inline void SetProcedure(NiThreadProcedure* pkProcedure);

    // *** end Emergent internal use only ***

protected:
    bool SystemCreateThread();
    bool SystemSetPriority(Priority ePriority);
    bool SystemSetAffinity(const NiProcessorAffinity& kAffinity);
    int SystemSuspend();
    int SystemResume();
    bool SystemWaitForCompletion();

    NiProcessorAffinity m_kAffinity;

    unsigned int m_uiStackSize;
    
    NiThreadProcedure* m_pkProcedure;
    
    Priority m_ePriority;
    volatile Status m_eStatus;
    volatile unsigned int m_uiReturnValue;

#if defined(WIN32) || defined(_XENON)
    static DWORD WINAPI ThreadProc(void* pvArg);
    HANDLE m_hThread;
#elif defined(_PS3)
    static void* ThreadProc(void* pvArg);
    pthread_t m_kThreadID;
    pthread_mutex_t m_kMutexID;
#elif defined(_WII)
    static void* ThreadProc(void*);
    bool SystemFreeThreadResources();
    void* m_stack;
    OSThread m_kThread;
#endif  //#if defined(WIN32) || defined(_XENON)

    static NiTThreadLocal<NiThread*> ms_kTLSCurrentThread;

    // This is supplied primarily for debugging to allow for easily 
    // identifying threads. In release builds, it will likely be compiled
    // out to nothing, so DO NOT write code that depends on it being 
    // present.
    char* m_pcName;
};

#include "NiThread.inl"

#endif  //#ifndef NITHREAD_H
