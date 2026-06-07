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
#include "NiLoopedThreadProcedure.h"
#include "NiLoopedThread.h"

//---------------------------------------------------------------------------
bool NiLoopedThreadProcedure::LoopedProcedure(void*)
{
    // default behavior: do nothing once
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiLoopedThreadProcedure::ThreadProcedure(NiThread* pkThread)
{
    NiLoopedThread* pkLoopedThread = static_cast<NiLoopedThread*>(pkThread);

    pkLoopedThread->WaitStart();
    while(!pkLoopedThread->GetLastLoop())
    {
        if (!LoopedProcedure(pkThread))
        {
            // If return value is false, then the thread explicitly
            // terminates itself.
            pkLoopedThread->SignalComplete();
            return PROC_TERMINATED;
        }

        pkLoopedThread->SignalComplete();
        pkLoopedThread->WaitStart();
    }

    // If last loop was set, then thread is terminated by some controlling 
    // thread.
    pkLoopedThread->SignalComplete();
    return THREAD_TERMINATED;
}
//---------------------------------------------------------------------------
