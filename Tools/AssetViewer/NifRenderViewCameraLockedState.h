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


#ifndef NIFRENDERVIEWCAMERALOCKEDSTATE_H
#define NIFRENDERVIEWCAMERALOCKEDSTATE_H


#include "NifRenderViewBufferedUIState.h"


class CNifRenderViewCameraLockedState : public CNifRenderViewBufferedUIState
{
public:
    CNifRenderViewCameraLockedState(CNifRenderView* pkView);

    virtual bool CanExecuteCommand(char* pcCommandID);

protected:
    virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint);
};


#endif // NIFRENDERVIEWCAMERALOCKEDSTATE_H