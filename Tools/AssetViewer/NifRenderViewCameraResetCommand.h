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

// NifRenderViewCameraResetCommand.h

#ifndef NIFRENDERVIEWCAMERARESETCOMMAND_H
#define NIFRENDERVIEWCAMERARESETCOMMAND_H

#include "NifRenderViewUICommand.h"

class CNifRenderView;

class CNifRenderViewCameraResetCommand : public CNifRenderViewUICommand
{    
    public:
        CNifRenderViewCameraResetCommand();
        ~CNifRenderViewCameraResetCommand();
        bool Execute(CNifRenderView* pkView);
        virtual char* GetIDString() {return "CameraReset";};

    protected:
        NiCameraPtr m_spCamera;
        NiNodePtr m_spScene;
        CNifRenderView* m_pkView;
};

#endif  // #ifndef NIFRENDERVIEWCAMERARESETCOMMAND_H
