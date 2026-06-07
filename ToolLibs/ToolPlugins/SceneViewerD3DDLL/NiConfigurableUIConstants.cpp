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

#include "stdafx.h"  
#include "NiConfigurableUIConstants.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
NiConfigurableUIConstants::NiConfigurableUIConstants()
{
    char* pcStr = "[CONFIGURABLE USER-INTERFACE MAP]";
    size_t stLen = strlen(pcStr) + 1;
    m_pcHeader = NiAlloc(char, stLen);
    NiStrcpy(m_pcHeader, stLen, pcStr);
    m_pcUserInterfaceTypeStrings = NiAlloc(char*, 3);
    m_pcUserInterfaceTypeStrings[0] = "[MOUSE]";
    m_pcUserInterfaceTypeStrings[1] = "[KEYBOARD]";
    m_pcUserInterfaceTypeStrings[2] = "[GAMEPAD]";

    m_pcUserInterfaceSensitivityStrings = NiAlloc(char*, 5);
    m_pcUserInterfaceSensitivityStrings[0] = "NOT_PRESSED";
    m_pcUserInterfaceSensitivityStrings[1] = "SINGLE";
    m_pcUserInterfaceSensitivityStrings[2] = "CONTINUOUS";
    m_pcUserInterfaceSensitivityStrings[3] = "RELEASED";
    m_pcUserInterfaceSensitivityStrings[4] = "DOUBLE-CLICK";

    m_pcMouseMovementTypeStrings = NiAlloc(char*, 2); 
    m_pcMouseMovementTypeStrings[0] = "F/B";    
    m_pcMouseMovementTypeStrings[1] = "L/R";    
    
    m_pcVirtualKeyTableStrings = 0;
    m_pcVirtualKeyModifierStrings =0;
    m_pcMouseModifierStrings =0;
    m_peVirtualKeyModifierCodes = 0;
    m_peMouseModifierCodes = 0;

    m_uiUserInterfaceSensitivityStringsSize = 5;
    m_uiUserInterfaceTypeStringsSize = 3;
    m_uiMouseMovementTypeStringsSize = 2;

    m_uiVirtualKeyTableStringsSize = 0;
    m_uiVirtualKeyModifierStringsSize = 0;
    m_uiMouseModifierStringsSize = 0;
    m_uiVirtualKeyModifierCodesSize = 0;
    m_uiMouseModifierCodesSize = 0;

    if (m_pcVirtualKeyTableStrings == 0)
    {
        m_uiVirtualKeyTableStringsSize = 256;
        m_pcVirtualKeyTableStrings = NiAlloc(char*, 256);
        memset(m_pcVirtualKeyTableStrings, 0, 256 * sizeof(char*));
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NOKEY]         = 
            "NONE\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_BACK_SPACE]    = 
            "BS\0"; // VK_BACK
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_TAB]           = 
            "TAB\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_RETURN]        = 
            "RET\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_ESCAPE]        = 
            "ESC\0"; // VK_ESCAPE = 0x1B
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_CAPS_LOCK]     = 
            "CAP\0"; // VK_CAPITAL

        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_SHIFT]         = 
            "SHIFT\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_CONTROL]       = 
            "CTL\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_MENU]          = 
            "ALT\0"; 
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_LWIN]          = 
            "LWIN\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_RWIN]          = 
            "RWIN\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_APPS]          = 
            "META\0"; // Meta key
        // Windows 2000 only
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_SEMICOLON]     = 
            ";"; // VK_OEM_1 = 0xBA
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_EQUALS]        = 
            "="; // VK_OEM_PLUS = 0xBB
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_COMMA]         = 
            ","; // VK_OEM_COMMA = 0xBC
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_MINUS]         = 
            "-"; // VK_OEM_MINUS = 0xBD
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_PERIOD]        = 
            "."; // VK_OEM_PERIOD = 0xBE
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_SLASH]         = 
            "/"; // VK_OEM_2 = 0xBF
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_BACK_QUOTE]    = 
            "'"; // VK_OEM_3 = 0xC0
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_OPEN_BRACKET]  = 
            "["; // VK_OEM_4 = 0xDB
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_BACK_SLASH]    = 
            "\\"; // VK_OEM_5 = 0xDC
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_CLOSE_BRACKET] = 
            "]"; // VK_OEM_6 = 0xDD
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_QUOTE]         = 
            "\""; // VK_OEM_7 = 0xDE
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_SPACE]         = 
            "SPACE\0";

        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_PAGE_UP]       = 
            "PGUP\0"; // VK_PRIOR
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_PAGE_DOWN]     = 
            "PGDOWN\0"; // VK_NEXT
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_END]           = 
            "END\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_HOME]          = 
            "HOME\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_LEFT]          = 
            "LEFT\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_UP]            = 
            "UP\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_RIGHT ]        = 
            "RIGHT\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_DOWN]          = 
            "DOWN\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_INSERT]        = 
            "INS\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_DELETE ]       = 
            "DEL\0";

        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_0]             = 
            "0\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_1]             = 
            "1\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_2]             = 
            "2\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_3]             = 
            "3\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_4]             = 
            "4\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_5]             = 
            "5\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_6]             = 
            "6\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_7]             = 
            "7\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_8]             = 
            "8\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_9]             = 
            "9\0";

        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_A]             = 
            "A\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_B]             = 
            "B\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_C]             = 
            "C\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_D]             = 
            "D\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_E]             = 
            "E\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F]             = 
            "F\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_G]             = 
            "G\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_H]             = 
            "H\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_I]             = 
            "I\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_J]             = 
            "J\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_K]             = 
            "K\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_L]             = 
            "L\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_M]             = 
            "M\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_N]             = 
            "N\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_O]             = 
            "O\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_P]             = 
            "P\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_Q]             = 
            "Q\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_R]             = 
            "R\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_S]             = 
            "S\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_T]             = 
            "T\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_U]             = 
            "U\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_V]             = 
            "V\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_W]             = 
            "W\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_X]             = 
            "X\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_Y]             = 
            "Y\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_Z]             = 
            "Z\0";
        
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMLOCK]       = 
            "NUM\0"; // VK_NUMLOCK = 0x90
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD0]       = 
            "NP0\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD1]       = 
            "NP1\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD2]       = 
            "NP2\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD3]       = 
            "NP3\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD4]       = 
            "NP4\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD5]       = 
            "NP5\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD6]       = 
            "NP6\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD7]       = 
            "NP7\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD8]       = 
            "NP8\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_NUMPAD9]       = 
            "NP9\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_MULTIPLY]      = 
            "MUL\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_ADD]           = 
            "PLS\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_SUBTRACT]      = 
            "SUB\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_DECIMAL]       = 
            "DEC\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_DIVIDE]        = 
            "DIV\0";

        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F1]            = 
            "F1\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F2]            = 
            "F2\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F3]            = 
            "F3\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F4]            = 
            "F4\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F5]            = 
            "F5\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F6]            = 
            "F6\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F7]            = 
            "F7\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F8]            = 
            "F8\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F9]            = 
            "F9\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F10]           = 
            "F10\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F11]           = 
            "F11\0";
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_F12]           = 
            "F12\0";

        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_PRINTSCREEN]   = 
            "PRNT\0"; // VK_SNAPSHOT
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_SCROLL_LOCK]   = 
            "SCROLL\0"; // VK_OEM_SCROLL = 0x91
        m_pcVirtualKeyTableStrings[NiSceneKeyboard::KEY_PAUSE]         = 
            "PAUSE\0";
    }


    if (m_pcVirtualKeyModifierStrings == 0)
    {
        m_uiVirtualKeyModifierStringsSize = 6;
        m_pcVirtualKeyModifierStrings = NiAlloc(char*, 8);
        memset(m_pcVirtualKeyModifierStrings, 0, 8 * sizeof(char*));
        m_pcVirtualKeyModifierStrings[0] = "\0";
        m_pcVirtualKeyModifierStrings[1] = "CTL\0";
        m_pcVirtualKeyModifierStrings[2] = "ALT\0";
        m_pcVirtualKeyModifierStrings[3] = "WIN\0";
        m_pcVirtualKeyModifierStrings[4] = "SHIFT\0";
        m_pcVirtualKeyModifierStrings[5] = "CAP\0";
    }

    if (m_peVirtualKeyModifierCodes == 0)
    {
        m_uiVirtualKeyModifierCodesSize = 6;
        m_peVirtualKeyModifierCodes = NiAlloc(
            NiSceneKeyboard::Modifiers, 8);
        memset(m_peVirtualKeyModifierCodes, 0, 8 * sizeof(char*));
        m_peVirtualKeyModifierCodes[0] = NiSceneKeyboard::KMOD_NONE;
        m_peVirtualKeyModifierCodes[1] = NiSceneKeyboard::KMOD_CONTROL;
        m_peVirtualKeyModifierCodes[2] = NiSceneKeyboard::KMOD_MENU;
        m_peVirtualKeyModifierCodes[3] = NiSceneKeyboard::KMOD_WIN;
        m_peVirtualKeyModifierCodes[4] = NiSceneKeyboard::KMOD_SHIFT;
        m_peVirtualKeyModifierCodes[5] = NiSceneKeyboard::KMOD_CAPS_LOCK;
    }

    if (m_pcMouseModifierStrings==0)
    {      
        m_uiMouseModifierStringsSize = 9;
        m_pcMouseModifierStrings = NiAlloc(char*, 9);
        memset(m_pcMouseModifierStrings, 0, 9 * sizeof(char*));
        m_pcMouseModifierStrings[0]     = "NONE\0";
        m_pcMouseModifierStrings[1]     = "CTL\0";
        m_pcMouseModifierStrings[2]     = "ALT\0";
        m_pcMouseModifierStrings[3]     = "WIN\0"; 
        m_pcMouseModifierStrings[4]     = "SHIFT\0";
        m_pcMouseModifierStrings[5]     = "L_BUTTON\0";
        m_pcMouseModifierStrings[6]     = "R_BUTTON\0";
        m_pcMouseModifierStrings[7]     = "M_BUTTON\0";
        m_pcMouseModifierStrings[8]     = "WHEEL\0";
    }


    if (m_peMouseModifierCodes == 0)
    {
        m_uiMouseModifierCodesSize = 9;
        m_peMouseModifierCodes = NiAlloc(NiSceneMouse::Modifiers, 9);
        memset(m_peMouseModifierCodes, 0, 9 * sizeof(char*));

        m_peMouseModifierCodes[0] = NiSceneMouse::MMOD_NONE;
        m_peMouseModifierCodes[1] = NiSceneMouse::MMOD_CONTROL;
        m_peMouseModifierCodes[2] = NiSceneMouse::MMOD_MENU;
        m_peMouseModifierCodes[3] = NiSceneMouse::MMOD_WIN;
        m_peMouseModifierCodes[4] = NiSceneMouse::MMOD_SHIFT;
        m_peMouseModifierCodes[5] = NiSceneMouse::MMOD_LEFT;
        m_peMouseModifierCodes[6] = NiSceneMouse::MMOD_RIGHT;
        m_peMouseModifierCodes[7] = NiSceneMouse::MMOD_MIDDLE;
        m_peMouseModifierCodes[8] = NiSceneMouse::MMOD_WHEEL;

    }

    
}
//---------------------------------------------------------------------------
NiConfigurableUIConstants::~NiConfigurableUIConstants()
{
    NiFree(m_pcMouseModifierStrings);
    m_pcMouseModifierStrings = NULL;
    NiFree(m_pcVirtualKeyModifierStrings);
    m_pcVirtualKeyModifierStrings = NULL;
    NiFree(m_pcVirtualKeyTableStrings);
    m_pcVirtualKeyTableStrings = NULL;
    NiFree(m_peVirtualKeyModifierCodes);
    m_peVirtualKeyModifierCodes = NULL;
    NiFree(m_peMouseModifierCodes);
    m_peMouseModifierCodes = NULL;
    
    NiFree(m_pcHeader);
    NiFree(m_pcUserInterfaceTypeStrings);
    NiFree(m_pcUserInterfaceSensitivityStrings);
    NiFree(m_pcMouseMovementTypeStrings); 
}
//---------------------------------------------------------------------------
// Sensitivity Enumeration Tools
const char* NiConfigurableUIConstants::GetSensitivityString(unsigned int ui)
{
    if (GetSensitivityCount() <= ui)
        return NULL;
    else
        return m_pcUserInterfaceSensitivityStrings[ui];
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetSensitivityString(
    NiDevice::DeviceState eType)
{
    if (eType != NiDevice::DEVICE_ERROR)
        return GetSensitivityString((unsigned int)eType);
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiDevice::DeviceState NiConfigurableUIConstants::GetSensitivity(
    char* pcString)
{
    for (int i = 0; i < (int)GetSensitivityCount(); i++)
    {
        if (NiStricmp(GetSensitivityString(i), pcString) == 0)
            return (NiDevice::DeviceState)i;
    }

    return NiDevice::DEVICE_ERROR;
}
//---------------------------------------------------------------------------
unsigned int NiConfigurableUIConstants::GetSensitivityCount()
{
    return m_uiUserInterfaceSensitivityStringsSize;
}
//---------------------------------------------------------------------------
// User Interface Enumeration Tools
const char* NiConfigurableUIConstants::GetUITypeString(unsigned int ui)
{
    if (GetMouseModifierCount() <= ui)
        return NULL;
    else
        return m_pcUserInterfaceTypeStrings[ui];   
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetUITypeString(
    NiSceneCommandInfo::UserInterfaceType eType)
{
    if (eType != NiSceneCommandInfo::UI_TYPE_NONE)
        return GetUITypeString((unsigned int)eType);
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiSceneCommandInfo::UserInterfaceType NiConfigurableUIConstants::GetUIType(
    char* pcString)
{
    for (int i = 0; i < (int) GetUITypeCount(); i++)
    {
        if (NiStricmp(GetUITypeString(i), pcString) == 0)
            return (NiSceneCommandInfo::UserInterfaceType)i;
    }

    return NiSceneCommandInfo::UI_TYPE_NONE;
}
//---------------------------------------------------------------------------
unsigned int NiConfigurableUIConstants::GetUITypeCount()
{
    return m_uiUserInterfaceTypeStringsSize;
}
//---------------------------------------------------------------------------
// Virtual KeyTable Enumeration Tools
const char* NiConfigurableUIConstants::GetKeyTableString(unsigned int ui)
{
    if (GetKeyTableCount() <= ui)
        return NULL;
    else
        return m_pcVirtualKeyTableStrings[ui];
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetKeyTableString(
    NiSceneKeyboard::KeyCode eType)
{
    NiSceneKeyboard::KeyCode* pkKeyTable = 
        NiSceneKeyboard::GetVirtualKeyTable();
    unsigned int ui = pkKeyTable[(int)eType];
    return GetKeyTableString(ui);
}
//---------------------------------------------------------------------------
NiSceneKeyboard::KeyCode NiConfigurableUIConstants::GetKey(char* pcString)
{
    NiSceneKeyboard::KeyCode* pkKeyTable = 
        NiSceneKeyboard::GetVirtualKeyTable();
    for (unsigned int ui = 0; ui < GetKeyTableCount(); ui++)
    {
        const char* pcKey = GetKeyTableString(ui);
        if (pcKey != NULL && NiStricmp(pcKey, pcString) == 0)
            return pkKeyTable[ui];
    }
    return pkKeyTable[0];
}
//---------------------------------------------------------------------------
unsigned int NiConfigurableUIConstants::GetKeyTableCount()
{
    return m_uiVirtualKeyTableStringsSize;
}
//---------------------------------------------------------------------------
// Virtual Key Modifiers Enumeration Tools
const char* NiConfigurableUIConstants::GetKeyModifierString(unsigned int ui)
{
    if (GetKeyModifierCount() <= ui)
        return NULL;
    else
        return m_pcVirtualKeyModifierStrings[ui];    
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetKeyModifierString(
    NiSceneKeyboard::Modifiers eType)
{
    unsigned int ui = 0;
    switch(eType)
    {
    default:
    case NiSceneKeyboard::KMOD_NONE:
        ui = 0;
        break;
    case NiSceneKeyboard::KMOD_CONTROL:
        ui = 1;
        break;
    case NiSceneKeyboard::KMOD_MENU:   
        ui = 2;
        break;
    case NiSceneKeyboard::KMOD_WIN:    
        ui = 3;
        break;
    case NiSceneKeyboard::KMOD_SHIFT:  
        ui = 4;
        break;
    case NiSceneKeyboard::KMOD_CAPS_LOCK:
        ui = 5;
        break;
    }
    return GetKeyModifierString(ui);
}
//---------------------------------------------------------------------------
NiSceneKeyboard::Modifiers NiConfigurableUIConstants::GetKeyModifier(
    char* pcString)
{
    for (int i = 0; i < (int) GetKeyModifierCount(); i++)
    {
        if (NiStricmp(GetKeyModifierString(i), pcString) == 0)
            return m_peVirtualKeyModifierCodes[i];
    }

    return NiSceneKeyboard::KMOD_NONE;
}
//---------------------------------------------------------------------------
unsigned int NiConfigurableUIConstants::GetKeyModifierCount()
{
    return m_uiVirtualKeyModifierCodesSize;
}
//---------------------------------------------------------------------------
// Mouse Movement Type Enumeration Tools
const char* NiConfigurableUIConstants::GetMouseMovementTypeString(
    unsigned int ui)
{ 
    if (GetMouseMovementTypeCount() <= ui)
        return NULL;
    else
        return m_pcMouseMovementTypeStrings[ui];    
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetMouseMovementTypeString(
    NiSceneMouseCommandInfo::MouseMovementType eType)
{ 
    if (eType != NiSceneMouseCommandInfo::MOUSE_MOVEMENT_NONE)
        return GetMouseMovementTypeString((int)eType);
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiSceneMouseCommandInfo::MouseMovementType 
    NiConfigurableUIConstants::GetMouseMovementType(char* pcString)
{ 
    for (int i = 0; i < (int) GetMouseMovementTypeCount(); i++)
    {
        if (NiStricmp(GetMouseMovementTypeString(i), pcString) == 0)
            return (NiSceneMouseCommandInfo::MouseMovementType)i;
    }
    
    return NiSceneMouseCommandInfo::MOUSE_MOVEMENT_NONE;
}
//---------------------------------------------------------------------------
unsigned int NiConfigurableUIConstants::GetMouseMovementTypeCount()
{ 
    return m_uiMouseMovementTypeStringsSize;
}
//---------------------------------------------------------------------------
// Mouse Modifiers Enumeration Tools
const char* NiConfigurableUIConstants::GetMouseModifierString(unsigned int ui)
{ 
    if (GetMouseModifierCount() <= ui)
        return NULL;
    else
        return m_pcMouseModifierStrings[ui];        
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetMouseModifierString(
    NiSceneMouse::Modifiers eType)
{ 
    unsigned int ui = 0;
    switch(eType)
    {
    default:
    case NiSceneMouse::MMOD_NONE:
        ui = 0;
        break;
    case NiSceneMouse::MMOD_CONTROL:
        ui = 1;
        break;
    case NiSceneMouse::MMOD_MENU:   
        ui = 2;
        break;
    case NiSceneMouse::MMOD_WIN:    
        ui = 3;
        break;
    case NiSceneMouse::MMOD_SHIFT:  
        ui = 4;
        break;
    case NiSceneMouse::MMOD_LEFT:
        ui = 5;
        break;
    case NiSceneMouse::MMOD_RIGHT:
        ui = 6;
        break;
    case NiSceneMouse::MMOD_MIDDLE:
        ui = 7;
        break;
    case NiSceneMouse::MMOD_WHEEL:
        ui = 8;
        break;
    }
    return GetMouseModifierString(ui);
}
//---------------------------------------------------------------------------
NiSceneMouse::Modifiers NiConfigurableUIConstants::GetMouseModifier(
    char* pcString)
{ 
    for (int i = 0; i < (int) GetMouseModifierCount(); i++)
    {
        if (NiStricmp(GetMouseModifierString(i), pcString) == 0)
            return m_peMouseModifierCodes[i];
    }

    return NiSceneMouse::MMOD_NONE;
}
//---------------------------------------------------------------------------
unsigned int NiConfigurableUIConstants::GetMouseModifierCount()
{ 
    return m_uiMouseModifierStringsSize;
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIConstants::GetHeader()
{
    return m_pcHeader;
}
//---------------------------------------------------------------------------
