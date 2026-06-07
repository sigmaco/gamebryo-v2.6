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

#ifndef NITHREADPROCEDURE_H
#define NITHREADPROCEDURE_H

#include "NiSystemLibType.h"

#include <NiOS.h>
#include "NiMemObject.h"

class NiThread;

class NISYSTEM_ENTRY NiThreadProcedure : public NiMemObject
{
public:
    NiThreadProcedure();
    virtual ~NiThreadProcedure();

    virtual unsigned int ThreadProcedure(NiThread* pkThread) = 0;
};

#endif  //#ifndef NITHREADPROCEDURE_H
