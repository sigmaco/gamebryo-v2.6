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

#ifndef NICAMERACUTTOCOMMAND_H
#define NICAMERACUTTOCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"
//#include <NiUniversalTypes.h>
// Command Class:   CameraCutTo
// Command Purpose: To change the current camera to the new camera index
// Command String Syntax: CAMERA_CUT_TO() SINGLE

class NiCameraCutToCommand : public NiSceneCommand
{
public:
    NiCameraCutToCommand(int iCameraToJumpTo = 0);
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiCameraCutToCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    unsigned int GetParameterCount();
    char* GetParameterName(unsigned int uiWhichParam) ;
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();
    static void RegisterListener(NiWindowRef kWnd, int iMsgID);

protected:
    int m_iCameraIDToCutTo;
    static NiCommandID ms_kID;
    static NiWindowRef m_kWnd;
    static int m_iMsgID;

};

#endif