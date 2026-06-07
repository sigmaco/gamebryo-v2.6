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

#ifndef NICAMERAZOOMEXTENTSCOMMAND_H
#define NICAMERAZOOMEXTENTSCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   CameraZoomExtents
// Command Purpose: To zoom the camera to fit the world into view
// Command String Syntax: CAMERA_ZOOM_EXTENTS() SINGLE

class NiCameraZoomExtentsCommand : public NiSceneCommand
{
public:
    NiCameraZoomExtentsCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    NiSceneCommand::NiSceneCommandApplyType GetApplyType();
    bool CanChangeCameraStates();
    static void Register();

    ~NiCameraZoomExtentsCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
protected:
    static NiCommandID ms_kID;
};

#endif