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
inline bool NiLoopedThread::GetLastLoop()
{
    return m_bLastLoop;
}
//---------------------------------------------------------------------------
inline void NiLoopedThread::SetLastLoop()
{
    m_bLastLoop = true;
}
//---------------------------------------------------------------------------
inline void NiLoopedThread::WaitStart()
{
    m_kStart.Wait();
}
//---------------------------------------------------------------------------
inline void NiLoopedThread::SignalStart()
{
    m_kStart.Signal();
}
//---------------------------------------------------------------------------
inline void NiLoopedThread::WaitComplete()
{
    m_kComplete.Wait();
}
//---------------------------------------------------------------------------
inline void NiLoopedThread::SignalComplete()
{
    m_kComplete.Signal();
}
//---------------------------------------------------------------------------
inline void NiLoopedThread::Shutdown()
{
    // SetLastLoop() is not threadsafe to be called in the middle of a loop,
    // because "DoLoop(); Shutdown();" might not iterate if SetLastLoop()
    // gets set before the loop checks it.
    WaitForLoopCompletion();
    SetLastLoop();

    // Now that we're at the end, signal start to finish the thread
    SignalStart();
}
//---------------------------------------------------------------------------
