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

#ifndef NISCENEKEYBOARD_H
#define NISCENEKEYBOARD_H

#include "NiDevice.h"
class NiSceneKeyboard : public NiDevice
{
public:
    // construction
    NiSceneKeyboard();

    // destruction
    ~NiSceneKeyboard();

    enum KeyCode
    {
        KEY_NOKEY         = 0x00,
        KEY_BACK_SPACE    = 0x08, // VK_BACK
        KEY_TAB           = 0x09,
        KEY_RETURN        = 0x0D,
        KEY_ESCAPE        = 0x0A, // VK_ESCAPE = 0x1B
        KEY_CAPS_LOCK     = 0x14, // VK_CAPITAL

        KEY_SHIFT         = 0x10,
        KEY_CONTROL       = 0x11,
        KEY_MENU          = 0x12, 
        KEY_LWIN          = 0x5B, 
        KEY_RWIN          = 0x5C, 
        KEY_APPS          = 0x5D, // Meta key, only on MS keyboard

                                  // Windows 2000 only
        KEY_SEMICOLON     = 0xBA, // VK_OEM_1 = 0xBA
        KEY_EQUALS        = 0xBB, // VK_OEM_PLUS = 0xBB
        KEY_COMMA         = 0xBC, // VK_OEM_COMMA = 0xBC
        KEY_MINUS         = 0xBD, // VK_OEM_MINUS = 0xBD
        KEY_PERIOD        = 0xBE, // VK_OEM_PERIOD = 0xBE
        KEY_SLASH         = 0xBF, // VK_OEM_2 = 0xBF
        KEY_BACK_QUOTE    = 0xC0, // VK_OEM_3 = 0xC0
        KEY_OPEN_BRACKET  = 0xDB, // VK_OEM_4 = 0xDB
        KEY_BACK_SLASH    = 0xDC, // VK_OEM_5 = 0xDC
        KEY_CLOSE_BRACKET = 0xDD, // VK_OEM_6 = 0xDD
        KEY_QUOTE         = 0xDE, // VK_OEM_7 = 0xDE
        KEY_SPACE         = 0x20,

        KEY_PAGE_UP       = 0x21, // VK_PRIOR
        KEY_PAGE_DOWN     = 0x22, // VK_NEXT
        KEY_END           = 0x23,
        KEY_HOME          = 0x24,
        KEY_LEFT          = 0x25,
        KEY_UP            = 0x26,
        KEY_RIGHT         = 0x27,
        KEY_DOWN          = 0x28,
        KEY_INSERT        = 0x2D,
        KEY_DELETE        = 0x2E,

        KEY_0             = 0x30,
        KEY_1             = 0x31,
        KEY_2             = 0x32,
        KEY_3             = 0x33,
        KEY_4             = 0x34,
        KEY_5             = 0x35,
        KEY_6             = 0x36,
        KEY_7             = 0x37,
        KEY_8             = 0x38,
        KEY_9             = 0x39,

        KEY_A             = 0x41,
        KEY_B             = 0x42,
        KEY_C             = 0x43,
        KEY_D             = 0x44,
        KEY_E             = 0x45,
        KEY_F             = 0x46,
        KEY_G             = 0x47,
        KEY_H             = 0x48,
        KEY_I             = 0x49,
        KEY_J             = 0x4A,
        KEY_K             = 0x4B,
        KEY_L             = 0x4C,
        KEY_M             = 0x4D,
        KEY_N             = 0x4E,
        KEY_O             = 0x4F,
        KEY_P             = 0x50,
        KEY_Q             = 0x51,
        KEY_R             = 0x52,
        KEY_S             = 0x53,
        KEY_T             = 0x54,
        KEY_U             = 0x55,
        KEY_V             = 0x56,
        KEY_W             = 0x57,
        KEY_X             = 0x58,
        KEY_Y             = 0x59,
        KEY_Z             = 0x5A,
        
        KEY_NUMLOCK       = 0x90, // VK_NUMLOCK = 0x90
        KEY_NUMPAD0       = 0x60,
        KEY_NUMPAD1       = 0x61,
        KEY_NUMPAD2       = 0x62,
        KEY_NUMPAD3       = 0x63,
        KEY_NUMPAD4       = 0x64,
        KEY_NUMPAD5       = 0x65,
        KEY_NUMPAD6       = 0x66,
        KEY_NUMPAD7       = 0x67,
        KEY_NUMPAD8       = 0x68,
        KEY_NUMPAD9       = 0x69,
        KEY_MULTIPLY      = 0x6A,
        KEY_ADD           = 0x6B,
        KEY_SUBTRACT      = 0x6D,
        KEY_DECIMAL       = 0x6E,
        KEY_DIVIDE        = 0x6F,

        KEY_F1            = 0x70,
        KEY_F2            = 0x71,
        KEY_F3            = 0x72,
        KEY_F4            = 0x73,
        KEY_F5            = 0x74,
        KEY_F6            = 0x75,
        KEY_F7            = 0x76,
        KEY_F8            = 0x77,
        KEY_F9            = 0x78,
        KEY_F10           = 0x79,
        KEY_F11           = 0x7A,
        KEY_F12           = 0x7B,

        KEY_PRINTSCREEN   = 0x2C, // VK_SNAPSHOT
        KEY_SCROLL_LOCK   = 0x8E, // VK_OEM_SCROLL = 0x91
        KEY_PAUSE         = 0x13
    };

    // Emergent reserves modifiers in the lower 16 bits - an application can
    // add its own modifiers in the upper 16 bits
    enum Modifiers
    { 
        KMOD_NONE      = 0x00000000,
        KMOD_CONTROL   = 0x00000001,
        KMOD_MENU      = 0x00000002,
        KMOD_WIN       = 0x00000004,
        KMOD_SHIFT     = 0x00000008,
        KMOD_CAPS_LOCK = 0x00000010
    };

    // Transform from platform-dependent key code to NI key code
    KeyCode NiTranslateKeyCode(unsigned char ucInput);

    // Query state of key
    bool KeyIsDown(KeyCode eKey);
    
    // *** begin Emergent internal use only ***

    // Keep track of state of keyboard
    void RecordKeyPress(KeyCode eKey);
    void RecordKeyRelease(KeyCode eKey);

    // create NI modifiers from platform-specific modifiers
    long GetModifiers();
    static KeyCode* GetVirtualKeyTable();
    // *** end Emergent internal use only ***

protected:
    // condition of locking Caps Lock key
    //(We will not worry about Num Lock or Scroll Lock)
    bool m_bCapsLockDown;

    // state of keyboard - 1 bit for each key
    char m_acKeyboardState[16];
    
    // Virtual key table - different for each platform
    static KeyCode* ms_peVirtualKeyTable;
    static bool ms_pbVirtualKeyTable;
};

#endif

