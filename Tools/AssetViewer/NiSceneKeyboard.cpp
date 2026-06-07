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

// NiSceneKeyboard.cpp

#include "stdafx.h"
#include "NiSceneKeyboard.h"

NiSceneKeyboard::KeyCode* NiSceneKeyboard::ms_peVirtualKeyTable = 0;

//---------------------------------------------------------------------------
NiSceneKeyboard::NiSceneKeyboard() : NiDevice(256, false)
{
    memset(m_acKeyboardState, 0, 16);

    BYTE aucKeyState[256];
    GetKeyboardState((LPBYTE)&aucKeyState);   
    m_bCapsLockDown = (aucKeyState[VK_CAPITAL] != 0);

    if (ms_peVirtualKeyTable == 0)
    {
        ms_peVirtualKeyTable = new KeyCode[256];
        memset(ms_peVirtualKeyTable, 0, 256 * sizeof(KeyCode));

        ms_peVirtualKeyTable[0x08] = KEY_BACK_SPACE;    // VK_BACK
        ms_peVirtualKeyTable[0x09] = KEY_TAB;
        ms_peVirtualKeyTable[0x0D] = KEY_RETURN;
        ms_peVirtualKeyTable[0x10] = KEY_SHIFT;         
        ms_peVirtualKeyTable[0x11] = KEY_CONTROL;        
        ms_peVirtualKeyTable[0x12] = KEY_MENU;          // Alt on PC,
                                                        // Command on Mac
        ms_peVirtualKeyTable[0x13] = KEY_PAUSE;
        ms_peVirtualKeyTable[0x14] = KEY_CAPS_LOCK;     // VK_CAPITAL
        ms_peVirtualKeyTable[0x1B] = KEY_ESCAPE;        // VK_ESCAPE
        ms_peVirtualKeyTable[0x20] = KEY_SPACE;
        ms_peVirtualKeyTable[0x21] = KEY_PAGE_UP;       // VK_PRIOR
        ms_peVirtualKeyTable[0x22] = KEY_PAGE_DOWN;     // VK_NEXT
        ms_peVirtualKeyTable[0x23] = KEY_END;
        ms_peVirtualKeyTable[0x24] = KEY_HOME;
        ms_peVirtualKeyTable[0x25] = KEY_LEFT;
        ms_peVirtualKeyTable[0x26] = KEY_UP;
        ms_peVirtualKeyTable[0x27] = KEY_RIGHT;
        ms_peVirtualKeyTable[0x28] = KEY_DOWN;
        ms_peVirtualKeyTable[0x2C] = KEY_PRINTSCREEN;   // VK_SNAPSHOT
        ms_peVirtualKeyTable[0x2D] = KEY_INSERT;
        ms_peVirtualKeyTable[0x2E] = KEY_DELETE;
        ms_peVirtualKeyTable[0x30] = KEY_0;
        ms_peVirtualKeyTable[0x31] = KEY_1;
        ms_peVirtualKeyTable[0x32] = KEY_2;
        ms_peVirtualKeyTable[0x33] = KEY_3;
        ms_peVirtualKeyTable[0x34] = KEY_4;
        ms_peVirtualKeyTable[0x35] = KEY_5;
        ms_peVirtualKeyTable[0x36] = KEY_6;
        ms_peVirtualKeyTable[0x37] = KEY_7;
        ms_peVirtualKeyTable[0x38] = KEY_8;
        ms_peVirtualKeyTable[0x39] = KEY_9;
        ms_peVirtualKeyTable[0x41] = KEY_A;
        ms_peVirtualKeyTable[0x42] = KEY_B;
        ms_peVirtualKeyTable[0x43] = KEY_C;
        ms_peVirtualKeyTable[0x44] = KEY_D;
        ms_peVirtualKeyTable[0x45] = KEY_E;
        ms_peVirtualKeyTable[0x46] = KEY_F;
        ms_peVirtualKeyTable[0x47] = KEY_G;
        ms_peVirtualKeyTable[0x48] = KEY_H;
        ms_peVirtualKeyTable[0x49] = KEY_I;
        ms_peVirtualKeyTable[0x4A] = KEY_J;
        ms_peVirtualKeyTable[0x4B] = KEY_K;
        ms_peVirtualKeyTable[0x4C] = KEY_L;
        ms_peVirtualKeyTable[0x4D] = KEY_M;
        ms_peVirtualKeyTable[0x4E] = KEY_N;
        ms_peVirtualKeyTable[0x4F] = KEY_O;
        ms_peVirtualKeyTable[0x50] = KEY_P;
        ms_peVirtualKeyTable[0x51] = KEY_Q;
        ms_peVirtualKeyTable[0x52] = KEY_R;
        ms_peVirtualKeyTable[0x53] = KEY_S;
        ms_peVirtualKeyTable[0x54] = KEY_T;
        ms_peVirtualKeyTable[0x55] = KEY_U;
        ms_peVirtualKeyTable[0x56] = KEY_V;
        ms_peVirtualKeyTable[0x57] = KEY_W;
        ms_peVirtualKeyTable[0x58] = KEY_X;
        ms_peVirtualKeyTable[0x59] = KEY_Y;
        ms_peVirtualKeyTable[0x5A] = KEY_Z;
        ms_peVirtualKeyTable[0x5B] = KEY_LWIN;          // Win on PC
                                                        // Option on Mac
        ms_peVirtualKeyTable[0x5C] = KEY_RWIN;          // Win on PC
                                                        // Option on Mac
        ms_peVirtualKeyTable[0x5D] = KEY_APPS;          // Meta key
                                                        // only on MS keyboard
        ms_peVirtualKeyTable[0x60] = KEY_NUMPAD0;
        ms_peVirtualKeyTable[0x61] = KEY_NUMPAD1;
        ms_peVirtualKeyTable[0x62] = KEY_NUMPAD2;
        ms_peVirtualKeyTable[0x63] = KEY_NUMPAD3;
        ms_peVirtualKeyTable[0x64] = KEY_NUMPAD4;
        ms_peVirtualKeyTable[0x65] = KEY_NUMPAD5;
        ms_peVirtualKeyTable[0x66] = KEY_NUMPAD6;
        ms_peVirtualKeyTable[0x67] = KEY_NUMPAD7;
        ms_peVirtualKeyTable[0x68] = KEY_NUMPAD8;
        ms_peVirtualKeyTable[0x69] = KEY_NUMPAD9;
        ms_peVirtualKeyTable[0x6A] = KEY_MULTIPLY;
        ms_peVirtualKeyTable[0x6B] = KEY_ADD;
        ms_peVirtualKeyTable[0x6D] = KEY_SUBTRACT;
        ms_peVirtualKeyTable[0x6E] = KEY_DECIMAL;
        ms_peVirtualKeyTable[0x6F] = KEY_DIVIDE;
        ms_peVirtualKeyTable[0x70] = KEY_F1;
        ms_peVirtualKeyTable[0x71] = KEY_F2;
        ms_peVirtualKeyTable[0x72] = KEY_F3;
        ms_peVirtualKeyTable[0x73] = KEY_F4;
        ms_peVirtualKeyTable[0x74] = KEY_F5;
        ms_peVirtualKeyTable[0x75] = KEY_F6;
        ms_peVirtualKeyTable[0x76] = KEY_F7;
        ms_peVirtualKeyTable[0x77] = KEY_F8;
        ms_peVirtualKeyTable[0x78] = KEY_F9;
        ms_peVirtualKeyTable[0x79] = KEY_F10;
        ms_peVirtualKeyTable[0x7A] = KEY_F11;
        ms_peVirtualKeyTable[0x7B] = KEY_F12;
        ms_peVirtualKeyTable[0x90] = KEY_NUMLOCK;       // VK_NUMLOCK
        ms_peVirtualKeyTable[0x91] = KEY_SCROLL_LOCK;   // VK_OEM_SCROLL
        ms_peVirtualKeyTable[0xBA] = KEY_SEMICOLON;     // VK_OEM_1 
        ms_peVirtualKeyTable[0xBB] = KEY_EQUALS;        // VK_OEM_PLUS 
        ms_peVirtualKeyTable[0xBC] = KEY_COMMA;         // VK_OEM_COMMA 
        ms_peVirtualKeyTable[0xBD] = KEY_MINUS;         // VK_OEM_MINUS 
        ms_peVirtualKeyTable[0xBE] = KEY_PERIOD;        // VK_OEM_PERIOD 
        ms_peVirtualKeyTable[0xBF] = KEY_SLASH;         // VK_OEM_2 
        ms_peVirtualKeyTable[0xC0] = KEY_BACK_QUOTE;    // VK_OEM_3 
        ms_peVirtualKeyTable[0xDB] = KEY_OPEN_BRACKET;  // VK_OEM_4 
        ms_peVirtualKeyTable[0xDC] = KEY_BACK_SLASH;    // VK_OEM_5 
        ms_peVirtualKeyTable[0xDD] = KEY_CLOSE_BRACKET; // VK_OEM_6 
        ms_peVirtualKeyTable[0xDE] = KEY_QUOTE;         // VK_OEM_7  
    }
}
//---------------------------------------------------------------------------
NiSceneKeyboard::~NiSceneKeyboard()
{
    if (ms_peVirtualKeyTable)
        delete [] ms_peVirtualKeyTable;
    ms_peVirtualKeyTable = 0;
}
//---------------------------------------------------------------------------
NiSceneKeyboard::KeyCode NiSceneKeyboard::NiTranslateKeyCode(
    unsigned char ucInput)
{
    NIASSERT(ms_peVirtualKeyTable);
    return ms_peVirtualKeyTable[ucInput];
}
//---------------------------------------------------------------------------
long NiSceneKeyboard::GetModifiers()
{
    long lResult = KMOD_NONE;
    if (KeyIsDown(KEY_CONTROL))
        lResult |= KMOD_CONTROL;
    if (KeyIsDown(KEY_MENU))
        lResult |= KMOD_MENU;
    if (KeyIsDown(KEY_LWIN) || KeyIsDown(KEY_RWIN))
        lResult |= KMOD_WIN;
    if (KeyIsDown(KEY_SHIFT))
        lResult |= KMOD_SHIFT;
    if (m_bCapsLockDown)
        lResult |= KMOD_CAPS_LOCK;
    return lResult;
}
//---------------------------------------------------------------------------
bool NiSceneKeyboard::KeyIsDown(KeyCode eKey)
{
    if (eKey == KEY_NOKEY)
        return false;
    if (GetDeviceState(eKey) == NiDevice::PRESSED || 
        GetDeviceState(eKey) == NiDevice::CONTINUOUS_PRESSED)
        return true;
    /*if ((m_acKeyboardState[eKey >> 3] >> (eKey & 7)) & 1)
        return true;*/
    else
    {
        if (eKey == KEY_CAPS_LOCK)
            return m_bCapsLockDown;
        else
            return false;
    }
}
//---------------------------------------------------------------------------
void NiSceneKeyboard::RecordKeyPress(KeyCode eKey)
{
    m_acKeyboardState[eKey >> 3] |= 1 << (eKey & 7);
    DeviceStateChange(NiDevice::PRESSED_EVENT, eKey);
}
//---------------------------------------------------------------------------
void NiSceneKeyboard::RecordKeyRelease(KeyCode eKey)
{
    m_acKeyboardState[eKey >> 3] &= ~(1 << (eKey & 7));
    if (eKey == KEY_CAPS_LOCK)
        m_bCapsLockDown = !m_bCapsLockDown;
    DeviceStateChange(NiDevice::RELEASED_EVENT, eKey);
}
//---------------------------------------------------------------------------
NiSceneKeyboard::KeyCode* NiSceneKeyboard::GetVirtualKeyTable()
{
    return ms_peVirtualKeyTable;
}
//---------------------------------------------------------------------------
