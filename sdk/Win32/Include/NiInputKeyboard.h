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

#ifndef NIINPUTKEYBOARD_H
#define NIINPUTKEYBOARD_H

#include "NiInputDevice.h"

class NIINPUT_ENTRY NiInputKeyboard : public NiInputDevice
{
    NiDeclareRTTI;

public:
    enum KeyCode
    {
        KEY_NOKEY           = 0x00,
        KEY_ESCAPE,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_0,
        KEY_MINUS,      /* - on main keyboard */
        KEY_EQUALS,
        KEY_BACK,       /* backspace */
        KEY_TAB,
        KEY_Q,
        KEY_W,
        KEY_E,
        KEY_R,
        KEY_T,
        KEY_Y,
        KEY_U,
        KEY_I,
        KEY_O,
        KEY_P,
        KEY_LBRACKET,
        KEY_RBRACKET,
        KEY_RETURN,     /* Enter on main keyboard */
        KEY_LCONTROL,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_SEMICOLON,
        KEY_APOSTROPHE,
        KEY_GRAVE,      /* accent grave */
        KEY_LSHIFT,
        KEY_BACKSLASH,
        KEY_Z,
        KEY_X,
        KEY_C,
        KEY_V,
        KEY_B,
        KEY_N,
        KEY_M,
        KEY_COMMA,
        KEY_PERIOD,     /* . on main keyboard */
        KEY_SLASH,      /* / on main keyboard */
        KEY_RSHIFT,
        KEY_MULTIPLY,       /* * on numeric keypad */
        KEY_LMENU,          /* left Alt */
        KEY_SPACE,
        KEY_CAPITAL,
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_NUMLOCK,
        KEY_SCROLL,     /* Scroll Lock */
        KEY_NUMPAD7,
        KEY_NUMPAD8,
        KEY_NUMPAD9,
        KEY_SUBTRACT,   /* - on numeric keypad */
        KEY_NUMPAD4,
        KEY_NUMPAD5,
        KEY_NUMPAD6,
        KEY_ADD,        /* + on numeric keypad */
        KEY_NUMPAD1,
        KEY_NUMPAD2,
        KEY_NUMPAD3,
        KEY_NUMPAD0,
        KEY_DECIMAL,        /* . on numeric keypad */
        KEY_OEM_102,        /* <> or \| on RT 102-key keyboard (Non-U.S.) */
        KEY_F11,
        KEY_F12,
        KEY_F13,            /*                     (NEC PC98) */
        KEY_F14,            /*                     (NEC PC98) */
        KEY_F15,            /*                     (NEC PC98) */
        KEY_KANA,           /* (Japanese keyboard)            */
        KEY_ABNT_C1,        /* /? on Brazilian keyboard */
        KEY_CONVERT,        /* (Japanese keyboard)            */
        KEY_NOCONVERT,      /* (Japanese keyboard)            */
        KEY_YEN,            /* (Japanese keyboard)            */
        KEY_ABNT_C2,        /* Numpad . on Brazilian keyboard */
        KEY_NUMPADEQUALS,   /* = on numeric keypad (NEC PC98) */
        KEY_PREVTRACK,      /* Previous Track 
                                (DIK_CIRCUMFLEX on Japanese keyboard) */
        KEY_AT,             /*                     (NEC PC98) */
        KEY_COLON,          /*                     (NEC PC98) */
        KEY_UNDERLINE,      /*                     (NEC PC98) */
        KEY_KANJI,          /* (Japanese keyboard)            */
        KEY_STOP,           /*                     (NEC PC98) */
        KEY_AX,             /*                     (Japan AX) */
        KEY_UNLABELED,      /*                        (J3100) */
        KEY_NEXTTRACK,      /* Next Track */
        KEY_NUMPADENTER,    /* Enter on numeric keypad */
        KEY_RCONTROL,
        KEY_MUTE,           /* Mute */
        KEY_CALCULATOR,     /* Calculator */
        KEY_PLAYPAUSE,      /* Play / Pause */
        KEY_MEDIASTOP,      /* Media Stop */
        KEY_VOLUMEDOWN,     /* Volume - */
        KEY_VOLUMEUP,       /* Volume + */
        KEY_WEBHOME,        /* Web home */
        KEY_NUMPADCOMMA,    /* , on numeric keypad (NEC PC98) */
        KEY_DIVIDE,         /* / on numeric keypad */
        KEY_SYSRQ,
        KEY_RMENU,          /* right Alt */
        KEY_PAUSE,          /* Pause */
        KEY_HOME,           /* Home on arrow keypad */
        KEY_UP,             /* UpArrow on arrow keypad */
        KEY_PRIOR,          /* PgUp on arrow keypad */
        KEY_LEFT,           /* LeftArrow on arrow keypad */
        KEY_RIGHT,          /* RightArrow on arrow keypad */
        KEY_END,            /* End on arrow keypad */
        KEY_DOWN,           /* DownArrow on arrow keypad */
        KEY_NEXT,           /* PgDn on arrow keypad */
        KEY_INSERT,         /* Insert on arrow keypad */
        KEY_DELETE,         /* Delete on arrow keypad */
        KEY_LWIN,           /* Left Windows key */
        KEY_RWIN,           /* Right Windows key */
        KEY_APPS,           /* AppMenu key */
        KEY_POWER,          /* System Power */
        KEY_SLEEP,          /* System Sleep */
        KEY_WAKE,           /* System Wake */
        KEY_WEBSEARCH,      /* Web Search */
        KEY_WEBFAVORITES,   /* Web Favorites */
        KEY_WEBREFRESH,     /* Web Refresh */
        KEY_WEBSTOP,        /* Web Stop */
        KEY_WEBFORWARD,     /* Web Forward */
        KEY_WEBBACK,        /* Web Back */
        KEY_MYCOMPUTER,     /* My Computer */
        KEY_MAIL,           /* Mail */
        KEY_MEDIASELECT,    /* Media Select */
        //
        KEY_TOTAL_COUNT
    };

    // Modifiers
    enum Modifiers
    { 
        KMOD_NONE      = 0x00000000,
        KMOD_LCONTROL  = 0x00000001,
        KMOD_RCONTROL  = 0x00000002,
        KMOD_LMENU     = 0x00000004,    // Left ALT
        KMOD_RMENU     = 0x00000008,    // Right ALT
        KMOD_LWIN      = 0x00000010,
        KMOD_RWIN      = 0x00000020,
        KMOD_LSHIFT    = 0x00000040,
        KMOD_RSHIFT    = 0x00000080,
        KMOD_CAPS_LOCK = 0x00000100
    };

public:
    // *** begin Emergent internal use only ***
    NiInputKeyboard(NiInputDevice::Description* pkDescription);
    // *** end Emergent internal use only ***

public:
    virtual ~NiInputKeyboard();

    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr UpdateActionMappedDevice(
        NiInputSystem* pkInputSystem);
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    // Query state of key
    inline const bool AnyKeyDown() const;

    inline const bool KeyIsDown(KeyCode eKey, unsigned int& uiModifiers) const;
    inline const bool KeyIsDown(KeyCode eKey) const;

    inline const bool KeyWasPressed(KeyCode eKey, unsigned int& uiModifiers) const;
    inline const bool KeyWasPressed(KeyCode eKey) const;

    inline const bool KeyWasReleased(KeyCode eKey, unsigned int& uiModifiers) const;
    inline const bool KeyWasReleased(KeyCode eKey) const;


    // *** begin Emergent internal use only ***
    inline void RecordKeyPress(KeyCode eKey);
    inline void RecordKeyRelease(KeyCode eKey);

    inline bool GetCapsLockState();
    inline void SetCapsLockState(bool bCapsLock);

    inline unsigned int GetKeyModifierValue(KeyCode eKey);

    virtual void TouchAllInputValues();

    virtual bool IsControlMapped(unsigned int uiControl, 
        unsigned int uiSemantic, bool bUseModifiers, 
        unsigned int uiModifiers, bool bUseRange = false,
        int iRangeLow = 0, int iRangeHigh = 0);
    // *** end Emergent internal use only ***

protected:
    // Caps lock
    bool m_bCapsLock;

    // Keystate
    // 32 * 8 = 256, 1 bit per key
    // AnyKeyDown uses this value as well.
    unsigned char m_aucKeystate[32];
    unsigned char m_aucKeystateLastUpdate[32];

    // Alternative - each key is a button...
    NiInputDevice::Button* m_pkKeys;
};

NiSmartPointer(NiInputKeyboard);

#include "NiInputKeyboard.inl"

#endif  //#ifndef NIINPUTKEYBOARD_H
