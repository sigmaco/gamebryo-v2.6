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

#ifndef NIOVERDRAWINDICATORCOMMAND_H
#define NIOVERDRAWINDICATORCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"
//#include <NiUniversalTypes.h>

// Command Class:   OverdrawIndicator
// Command ClassID: 15
// Command Purpose: To toggle the OverdrawIndicator mode on or off
// Command String Syntax: OVERDRAW() SINGLE

class NiOverdrawIndicatorCommand : public NiSceneCommand
{
public:
    NiOverdrawIndicatorCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiOverdrawIndicatorCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    static void Register();

    static void RegisterListener(NiWindowRef kWnd, int iMsgID);

    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

    static void CleanUp();

protected:
    void CreateOverdrawIndicator();
    void SetOverdraw(bool bEnable, NiSceneViewer* pkViewer);
    static NiWindowRef m_kWnd;
    static int m_iMsgID;

    NiStencilProperty* GetStencilProperty(NiNode* pkScene);
    
    // support for overdraw metering
    static NiMeshScreenElementsPtr ms_spBlueLevel;
    static NiMeshScreenElementsPtr ms_spGreenLevel;
    static NiMeshScreenElementsPtr ms_spYellowLevel;
    static NiMeshScreenElementsPtr ms_spRedLevel;
    static NiMeshScreenElementsPtr ms_spWarning;
    static NiStencilPropertyPtr ms_spStencil;
    static NiCommandID ms_kID;
};

#endif