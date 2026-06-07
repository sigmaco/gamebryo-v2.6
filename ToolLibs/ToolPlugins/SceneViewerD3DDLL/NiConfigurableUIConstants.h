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

#ifndef NICONFIGURABLEUICONSTANTS_H
#define NICONFIGURABLEUICONSTANTS_H
#include "NiSceneKeyboard.h"
#include "NiSceneMouse.h"
#include "NiSceneKeyboardCommandInfo.h"
#include "NiSceneMouseCommandInfo.h"
#include "NiSceneCommandClasses.h"
#include <NiRefObject.h>

// This class is meant to be a toolkit to aid in parsing ConfigurableUIMap
// files and strings. If you wish to override the default behavior of any of
// the arrays, override the class and the constructor

class NiConfigurableUIConstants : public NiRefObject
{
public:
    NiConfigurableUIConstants();
    ~NiConfigurableUIConstants();

    const char* GetHeader();

    // Sensitivity Enumeration Tools
    const char* GetSensitivityString(unsigned int ui);
    const char* GetSensitivityString(NiDevice::DeviceState m_eType);
    NiDevice::DeviceState GetSensitivity(char* pcString);
    unsigned int GetSensitivityCount();

    // User Interface Enumeration Tools
    const char* GetUITypeString(unsigned int ui);
    const char* GetUITypeString(NiSceneCommandInfo::UserInterfaceType m_eType);
    NiSceneCommandInfo::UserInterfaceType GetUIType(char* pcString);
    unsigned int GetUITypeCount();

    // Virtual KeyTable Enumeration Tools
    const char* GetKeyTableString(unsigned int ui);
    const char* GetKeyTableString(NiSceneKeyboard::KeyCode m_eType);
    NiSceneKeyboard::KeyCode GetKey(char* pcString);
    unsigned int GetKeyTableCount();

    // Virtual Key Modifiers Enumeration Tools
    const char* GetKeyModifierString(unsigned int ui);
    const char* GetKeyModifierString(NiSceneKeyboard::Modifiers m_eType);
    NiSceneKeyboard::Modifiers GetKeyModifier(char* pcString);
    unsigned int GetKeyModifierCount();

    // Mouse Movement Type Enumeration Tools
    const char* GetMouseMovementTypeString(unsigned int ui);
    const char* GetMouseMovementTypeString(
        NiSceneMouseCommandInfo::MouseMovementType m_eType);
    NiSceneMouseCommandInfo::MouseMovementType GetMouseMovementType(
        char* pcString);
    unsigned int GetMouseMovementTypeCount();
    
    // Mouse Modifiers Enumeration Tools
    const char* GetMouseModifierString(unsigned int ui);
    const char* GetMouseModifierString(NiSceneMouse::Modifiers m_eType);
    NiSceneMouse::Modifiers GetMouseModifier(char* pcString);
    unsigned int GetMouseModifierCount();
protected:
    char* m_pcHeader;
    char** m_pcUserInterfaceSensitivityStrings;
    unsigned int m_uiUserInterfaceSensitivityStringsSize;
    char** m_pcUserInterfaceTypeStrings;
    unsigned int m_uiUserInterfaceTypeStringsSize;
    char** m_pcMouseMovementTypeStrings; 
    unsigned int m_uiMouseMovementTypeStringsSize;
    
    char** m_pcVirtualKeyTableStrings;
    unsigned int m_uiVirtualKeyTableStringsSize;
    char** m_pcVirtualKeyModifierStrings;
    unsigned int m_uiVirtualKeyModifierStringsSize;
    char** m_pcMouseModifierStrings;
    unsigned int m_uiMouseModifierStringsSize;
    NiSceneKeyboard::Modifiers* m_peVirtualKeyModifierCodes;
    unsigned int m_uiVirtualKeyModifierCodesSize;
    NiSceneMouse::Modifiers* m_peMouseModifierCodes;
    unsigned int m_uiMouseModifierCodesSize;
};

NiSmartPointer(NiConfigurableUIConstants);

#endif
