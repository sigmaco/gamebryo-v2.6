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

#ifndef NILOOPEDTHREADPROCEDURE_H
#define NILOOPEDTHREADPROCEDURE_H

#include "NiSystemLibType.h"
#include "NiThreadProcedure.h"

class NISYSTEM_ENTRY NiLoopedThreadProcedure : public NiThreadProcedure
{
    // Returns if the thread should continue looping
    virtual bool LoopedProcedure(void* pvArg);

    // The return value of this ThreadProcedure.
    enum ReturnCode
    {
        THREAD_TERMINATED,
        PROC_TERMINATED
    };
private:
    virtual unsigned int ThreadProcedure(NiThread* pkThread);
};


#endif // NILOOPEDTHREADPROCEDURE_H
