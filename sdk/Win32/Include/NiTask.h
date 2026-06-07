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

#ifndef NITASK_H
#define NITASK_H

#include "NiObject.h"

class NIMAIN_ENTRY NiTask : public NiObject
{
    NiDeclareRTTI;

public:
    NiTask();
    virtual ~NiTask();

    enum TaskStatus
    {
        IDLE = 0,
        PENDING,
        RUNNING,
        COMPLETED,
        ABORTED
    };
    
    virtual void DoTask() = 0;

    virtual bool CanExecute();

    inline void SetStatus(TaskStatus eStatus);
    TaskStatus GetStatus();

    virtual bool Clear();
    
protected:
    TaskStatus m_eStatus;
};

#include "NiTask.inl"

#endif // NITASK_H
