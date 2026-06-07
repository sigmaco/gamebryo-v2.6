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
#include "NiLoopedThread.h"

//---------------------------------------------------------------------------
NiLoopedThread::~NiLoopedThread()
{
    Shutdown();
    WaitForCompletion();
}
//---------------------------------------------------------------------------
NiLoopedThread* NiLoopedThread::Create(NiLoopedThreadProcedure* pkProcedure, 
    unsigned int uiStackSize)
{
    NiLoopedThread* pkThread = NiNew NiLoopedThread(pkProcedure, 
        uiStackSize);

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
NiLoopedThread::NiLoopedThread(NiLoopedThreadProcedure* pkProcedure,
    unsigned int uiStackSize) : 
    NiThread(pkProcedure, uiStackSize),
    m_bLastLoop(false),
    m_kComplete(0), m_kStart(0)
    
{
    // Initialize semaphores correctly
    SignalComplete();

    // m_kStart now has value 0 (max 1)
    // m_kComplete now has value 1 (max 1)
}
//---------------------------------------------------------------------------
void NiLoopedThread::DoLoop()
{
    // Set complete outside of the threadproc to avoid race condition
    // with WaitForLoopCondition()'s WaitComplete()/SignalComplete().
    WaitComplete();
    SignalStart();
}
//---------------------------------------------------------------------------
bool NiLoopedThread::WaitForLoopCompletion()
{
    // If in the middle of a loop, wait will block.  If not, it will
    // blow right through the wait/signal.
    WaitComplete();
    SignalComplete();
    return true;
}
//---------------------------------------------------------------------------
