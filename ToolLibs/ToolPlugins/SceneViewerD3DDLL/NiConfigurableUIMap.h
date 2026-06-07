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

#ifndef NICONFIGURABLEUIMAP_H
#define NICONFIGURABLEUIMAP_H

#include "NiUIMap.h"
#include <NiMain.h>
#include "NiSceneCommandInfo.h"
#include "NiSceneCommandList.h"
#include "NiSceneCommandFactory.h"

class NiConfigurableUIMap : public NiUIMap
{
public:
    NiConfigurableUIMap();
    ~NiConfigurableUIMap();

    NiSceneCommandQueue* CreateMouseCommands();
    NiSceneCommandQueue* CreateKeyboardCommands();
    NiSceneCommandQueue* CreateExternalCommands();

    void SendCommand(NiSceneCommand* pkCommand);

    NiSceneCommandList* GetKeyboardList();
    NiSceneCommandList* GetMouseList();

    void SetName(const char* pcName);
    const char* GetName();
    
    NiSceneCommandFactoryPtr GetCommandFactory();
    void SetCommandFactory(NiSceneCommandFactoryPtr spFactory);

    void SetKeyboard(NiSceneKeyboard* pkKeyboard);
    void SetMouse(NiSceneMouse* pkMouse);

    void SetDeadzone(float fDeadZonePercentage);
    float GetDeadzone();

protected:       
    bool MouseButtonsMatchStates(unsigned int uiMouseButtons, 
        NiDevice::DeviceState eState);
    int  GetMaxLevelForMouseButtons(unsigned int uiMouseButtons);
    void SetLevelForMouseButtons(unsigned int uiMouseButtons, int iLevel);
    
   
    char* m_pcName;
    int* m_aiLastPriorityLevelKeyboard;
    int* m_aiLastPriorityLevelMouse;

    unsigned int m_uiPriorityListSizeKeyboard;
    unsigned int m_uiPriorityListSizeMouse;

    float m_fDeadZonePercentage;

    int m_iLowestPriorityValue;

    NiSceneCommandFactoryPtr m_pkCommandFactory;
    
    NiSceneCommandList* m_pkKeyboardMap;
    NiSceneCommandList* m_pkMouseMap;
    NiSceneCommandQueue* m_pkExternalMap;
};
NiSmartPointer(NiConfigurableUIMap);
#endif