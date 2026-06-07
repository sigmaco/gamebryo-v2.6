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

#ifndef NISCENEEVENTHANDLER_H
#define NISCENEEVENTHANDLER_H

#include "NiSceneViewer.h"
#include "NiUIMap.h"

class NiSceneEventHandler : public NiMemObject
{
public:
    NiSceneEventHandler();
    ~NiSceneEventHandler();
    void SetSceneViewer(NiSceneViewer* pkViewer);
    NiSceneViewer* GetSceneViewer();
    void InvalidateUI();
    bool HasValidUIMap();
    void SetupUI(NiUIMap* pkMap);
    NiUIMap* GetUI();
    bool IsValid();
    void GenerateIdleCommands();

    bool IsKeyboardActive();
    bool IsMouseActive();

    void SetKeyboard(NiSceneKeyboard* pkKeyboard);
    void SetMouse(NiSceneMouse* pkMouse);
   
protected:
    NiSceneViewer* m_pkViewer;
    NiUIMap* m_pkUIMap;
    NiSceneKeyboard* m_pkKeyboard;
    NiSceneMouse* m_pkMouse;
    bool m_bUseKeyboard;
    bool m_bUseMouse;
    long m_lLastMouseModifiers;
    long m_lLastKeyboardModifiers;
    int  m_iLastXPos;
    int  m_iLastYPos;       
};

#endif //NISCENEEVENTHANDLER_H