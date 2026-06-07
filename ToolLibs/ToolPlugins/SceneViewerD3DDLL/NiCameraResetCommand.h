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

#ifndef NICAMERARESETCOMMAND_H
#define NICAMERARESETCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   CameraReset
// Command ClassID: 4
// Command Purpose: To reset the current camera to its original state
// Command String Syntax: CAMERA_RESET() SINGLE

class NiCameraResetCommand : public NiSceneCommand
{
public:
    NiCameraResetCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    NiSceneCommand::NiSceneCommandApplyType GetApplyType();
    bool CanChangeCameraStates();
    static void Register();

    ~NiCameraResetCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
protected:
    static NiCommandID ms_kID;
};

#endif