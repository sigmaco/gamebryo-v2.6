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

#ifndef NICAMERAROTATECOMMAND_H
#define NICAMERAROTATECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   CameraRotate
// Command ClassID: 2
// Command Purpose: To rotate the camera in its local coordinate system
// Command String Syntax: 
// CAMERA_ROTATE( angleX, angleY, angleZ, bool) ANY-SENSITIVITY

class NiCameraRotateCommand : public NiSceneCommand
{
public:
    NiCameraRotateCommand(NiPoint3 kRotationAnglesInDegrees, bool bAbsolute);
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiCameraRotateCommand();
    NiSceneCommand::NiSceneCommandApplyType GetApplyType();
    bool CanChangeCameraStates();
    static void Register();

    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

protected:
    bool HasAnimationController(NiAVObject* pkObject);
    bool m_bAbsolute;
    NiPoint3 m_kRotationAngles;
    static NiCommandID ms_kID;
};

#endif
