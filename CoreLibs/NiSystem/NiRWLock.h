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

#ifndef NIRWLOCK_H
#define NIRWLOCK_H

#include "NiSystemLibType.h"
#include "NiRTLib.h"
#include <NiCriticalSection.h>
#include <NiSemaphore.h>

// This include is somewhat circular, but we must include the atomic
// increment and decrement functions.
#include <NiSystem.h>

class NISYSTEM_ENTRY NiRWLock
{
public:
    NiRWLock();
    ~NiRWLock();

    inline void LockRead();
    inline void LockWrite();
    inline void UnlockRead();
    inline void UnlockWrite();
protected:
    volatile NiUInt32 m_uiNumReaders;
#ifdef _PS3
    NiCriticalSection m_kWriteLock;
    sys_lwcond_t m_kNoReadersCond;
#elif defined(_XENON) || defined(WIN32)
    NiCriticalSection m_kWriteLock;
    HANDLE m_hNoReadersEvent;
#elif defined(_WII)
    OSMutex m_kWriteLock;
    OSCond m_kNoReadersCond;
#else
    #error Attempt to create reader-writer lock on undefined platform.
#endif
};

#include "NiRWLock.inl"

#endif // #ifndef NIRWLOCK_H
