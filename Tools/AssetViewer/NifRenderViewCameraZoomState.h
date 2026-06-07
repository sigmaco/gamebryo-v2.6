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

// NifRenderViewCameraZoomState.h

#ifndef NIFRENDERVIEWCAMERAZOOMSTATE_H
#define NIFRENDERVIEWCAMERAZOOMSTATE_H

#include "NifRenderViewCameraTranslationState.h"
#include "NifCameraList.h"

class CNifRenderViewCameraZoomState : 
    public CNifRenderViewCameraTranslationState
{
    public:
        CNifRenderViewCameraZoomState(CNifRenderView* pkView);
        ~CNifRenderViewCameraZoomState();
        virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint);

    protected:
        virtual void UpdateDevices();

};

#endif  // #ifndef NIFRENDERVIEWCAMERAZOOMSTATE_H
