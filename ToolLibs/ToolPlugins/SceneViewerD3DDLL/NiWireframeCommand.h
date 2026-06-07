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

#ifndef NIWIREFRAMECOMMAND_H
#define NIWIREFRAMECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"
//#include <NiUniversalTypes.h>
// Command Class:   Wireframe
// Command ClassID: 6
// Command Purpose: To toggle the wireframe mode on or off
// Command String Syntax: WIREFRAME() SINGLE

class NiWireframeCommand : public NiSceneCommand
{
public:
    NiWireframeCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiWireframeCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();

    static void RegisterListener(NiWindowRef kWnd, int iMsgID);

protected:
    NiWireframeProperty* GetWireframeProperty(NiNode* pkScene);
    static NiCommandID ms_kID;
    static NiWindowRef ms_kWnd;
    static int ms_iMsgID;

};

#endif