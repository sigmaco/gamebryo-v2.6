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

#ifndef NICAMERAPOINTTRANSLATECOMMAND_H
#define NICAMERAPOINTTRANSLATECOMMAND_H

#include "NiSceneCommand.h"
#include "NiSceneViewer.h"

// Command Class:   CameraPointTranslate
// Command Purpose: To translate the camera in its local coordinate system
//                  using its local axes to the point specified by the ray
// Command String Syntax: CAMERA_POINT_TRANSLATE( locX, locY) ANY_SENSITIVITY

class NiCameraPointTranslateCommand : 
    public NiSceneCommand
{
public:
    NiCameraPointTranslateCommand(NiPoint2 kPoint);
    ~NiCameraPointTranslateCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();

protected:
    NiPoint2 m_kWindowPt;
    static NiCommandID ms_kID;
};

#endif