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

#ifndef NICAMERATRANSLATECOMMAND_H
#define NICAMERATRANSLATECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   CameraTranslate
// Command ClassID: 1
// Command Purpose: To translate the camera in its local coordinate system
//                  using its local axes
// Command String Syntax: CAMERA_TRANSLATE( locX, locY, locZ, bool)
// ANY_SENSITIVITY

class NiCameraTranslateCommand : public NiSceneCommand
{
public:
    NiCameraTranslateCommand(NiPoint3 pkTranslateVector, bool bAbsolute);
    ~NiCameraTranslateCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();

protected:
    bool HasAnimationController(NiAVObject* pkObject);
    NiPoint3 GetXAxis(NiCamera* pkCamera);
    NiPoint3 GetYAxis(NiCamera* pkCamera);
    NiPoint3 GetZAxis(NiCamera* pkCamera);
    bool m_bAbsolute;
    NiPoint3 m_kTranslateVector;
    static NiCommandID ms_kID;
};

#endif
