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

#ifndef NICAMERATUMBLECOMMAND_H
#define NICAMERATUMBLECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   CameraTumble
// Command ClassID: 5
// Command Purpose: 
//       To tumble the camera around the scenegraph's centerpoint if relative
//       To tumble the camera around the specified centerpoint if absolute
// Command String Syntax: CAMERA_TUMBLE(angleX, angleY, angleZ,
//                                      centerX, centerY, centerZ, bool)
//                                      ANY_SENSITIVITY

class NiCameraTumbleCommand : public NiSceneCommand
{
public:
    NiCameraTumbleCommand(NiPoint3 kRotationAnglesInDegrees, 
        NiPoint3 kCenterPoint = NiPoint3::ZERO, bool bAbsolute = false);
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiCameraTumbleCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiSceneCommand::NiSceneCommandApplyType GetApplyType();
    bool CanChangeCameraStates();
    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();

protected:
    bool HasAnimationController(NiAVObject* pkObject);
    bool CameraIsChildOfRoot(NiNode* pkScene, NiCamera* pkCamera);
    NiPoint3 GetXAxis(NiCamera* pkCamera);
    NiPoint3 GetYAxis(NiCamera* pkCamera);
    NiPoint3 GetZAxis(NiCamera* pkCamera);
    bool m_bAbsolute;
    NiPoint3 m_kCenterPoint;
    NiPoint3 m_kRotationAngles;
    static NiCommandID ms_kID;
};

#endif