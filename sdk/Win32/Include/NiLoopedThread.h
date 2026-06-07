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

#ifndef NILOOPEDTHREAD_H
#define NILOOPEDTHREAD_H

#include "NiSystemLibType.h"
#include "NiSystem.h"
#include "NiThread.h"
#include "NiLoopedThreadProcedure.h"
#include "NiSemaphore.h"

// NiLoopedThread is a thread class that is designed to be re-used
// multiple times, such as an updating or culling thread that will
// be reused each frame in an application.

// Usage notes:
// Like an NiThread, an NiLoopedThread is created suspended and must be
// resumed before it can be used.
//
// Each DoLoop() should be paired with a WaitForLoopCompletion() call
// after it.  DoLoop() cannot be called repeatedly to run the loop
// multiple times without blocking.
//
// NiLoopedThreads can be destroyed by deleting them (via destructor).
// They can also be explicitly stopped via Shutdown().  Both Shutdown()
// and the destructor will wait until the loop procedure has finished
// running and will not interrupt it.  Shutdown() and the destructor
// will block until the loop has terminated (which is not necessarily
// when the thread procedure has terminated (return condition set, status set,
// etc...) Once a thread has stopped, DoLoop() silently does nothing.

class NISYSTEM_ENTRY NiLoopedThread : public NiThread
{
public:
    // constructor protected, use Create(...) function.
    virtual ~NiLoopedThread();

    static NiLoopedThread* Create(NiLoopedThreadProcedure* pkProcedure, 
        unsigned int uiStackSize = USEDEF_STACK_SIZE);

    // Suspend until the current loop finishes.
    bool WaitForLoopCompletion();

    // Run a single iteration of the thread.
    void DoLoop();

    // Terminate thread.  If not in the middle of a loop, then terminate
    // immediately.  If in the middle, terminate after that loop.
    inline void Shutdown();

    // *** begin Emergent internal use only ***
    // Synchronization functions used by NiLoopedThreadProcedure
    inline void WaitStart();
    inline void SignalStart();
    inline void WaitComplete();
    inline void SignalComplete();
    inline void SetLastLoop();
    inline bool GetLastLoop();
    // *** end Emergent internal use only ***
protected:
    NiLoopedThread(NiLoopedThreadProcedure* pkProcedure,
        unsigned int uiStackSize = USEDEF_STACK_SIZE);

    // True if the thread will exit in less than a complete loop iteration.
    volatile bool m_bLastLoop;

    // Complete is 1 if the loop is done; Complete is 0 if in a loop.
    NiSemaphore m_kComplete;
    // Start is 0 if the loop has started; Start is 1 if the loop can start.
    NiSemaphore m_kStart;
};

#include "NiLoopedThread.inl"

#endif // NILOOPEDTHREAD_H
