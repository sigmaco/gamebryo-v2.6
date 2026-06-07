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

// Precompiled Header
#include "NiUserInterfacePCH.h"

#include "NiUIHotkeyImage.h"
#include "NiUIAtlasMap.h"
#include "NiUIManager.h"

const float NiUIHotkeyImage::ms_fSpacerWidth = 0.25f;

// This makes eliminates a really long switch statement in the 
// "SetKeyboardHotkey" function by specifying the text to go on keys
const char* NiUIHotkeyImage::ms_pcKeyNameMap[
    NiInputKeyboard::KEY_TOTAL_COUNT] = {
    "",
    "Esc",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",             /* - on main keyboard */
    "=",
    "BKSP",          /* backspace */
    "Tab",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "[",
    "]",
    "RET",           /* Enter on main keyboard */
    "CTRL",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    ";",
    "'",
    "`",             /* accent grave */
    "Shift",
    "\\",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    ",",
    ".",             /* . on main keyboard */
    "/",             /* / on main keyboard */
    "Shift",
    "*",             /* * on numeric keypad */
    "Menu",          /* left Alt */
    "Space",
    "CAPS",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "NUMLK",
    "SCRLK",          /* Scroll Lock */
    "7",
    "8",
    "9",
    "-",              /* - on numeric keypad */
    "4",
    "5",
    "6",
    "+",              /* + on numeric keypad */
    "1",
    "2",
    "3",
    "0",
    ".",              /* . on numeric keypad */
    "OEM_102",        /* <> or \| on RT 102-key keyboard (Non-U.S.) */
    "F11",
    "F12",
    "F13",            /*                     (NEC PC98) */
    "F14",            /*                     (NEC PC98) */
    "F15",            /*                     (NEC PC98) */
    "KANA",           /* (Japanese keyboard)            */
    "ABNT_C1",        /* /? on Brazilian keyboard */
    "CONVERT",        /* (Japanese keyboard)            */
    "NOCONVERT",      /* (Japanese keyboard)            */
    "YEN",            /* (Japanese keyboard)            */
    "ABNT_C2",        /* Numpad . on Brazilian keyboard */
    "=",              /* = on numeric keypad (NEC PC98) */
    "PrevTrack",      /* Previous Track 
                            (DIK_CIRCUMFLEX on Japanese keyboard) */
    "@",              /*                     (NEC PC98) */
    ":",              /*                     (NEC PC98) */
    "_",              /*                     (NEC PC98) */
    "KANJI",          /* (Japanese keyboard)            */
    "STOP",           /*                     (NEC PC98) */
    "AX",             /*                     (Japan AX) */
    "UNLABELED",      /*                        (J3100) */
    "NextTrack",      /* Next Track */
    "Enter",          /* Enter on numeric keypad */
    "CTRL",
    "MUTE",           /* Mute */
    "CALC",           /* Calculator */
    "PLAYPAUSE",      /* Play / Pause */
    "MEDIASTOP",      /* Media Stop */
    "VOLUMEDOWN",     /* Volume - */
    "VOLUMEUP",       /* Volume + */
    "WEBHOME",        /* Web home */
    "NUMPADCOMMA",    /* ", on numeric keypad (NEC PC98) */
    "/",              /* / on numeric keypad */
    "SYSRQ",
    "RMENU",          /* right Alt */
    "PAUSE",          /* Pause */
    "HOME",           /* Home on arrow keypad */
    "^\n|",           /* UpArrow on arrow keypad */
    "PgUp",           /* PgUp on arrow keypad */
    "<--",            /* LeftArrow on arrow keypad */
    "-->",            /* RightArrow on arrow keypad */
    "End",            /* End on arrow keypad */
    "|\nV",           /* DownArrow on arrow keypad */
    "PgDn",           /* PgDn on arrow keypad */
    "INS",            /* Insert on arrow keypad */
    "DEL",            /* Delete on arrow keypad */
    "WIN",            /* Left Windows key */
    "WIN",            /* Right Windows key */
    "APPS",           /* AppMenu key */
    "POWER",          /* System Power */
    "SLEEP",          /* System Sleep */
    "WAKE",           /* System Wake */
    "WEBSEARCH",      /* Web Search */
    "WEBFAVORITES",   /* Web Favorites */
    "WEBREFRESH",     /* Web Refresh */
    "WEBSTOP",        /* Web Stop */
    "WEBFORWARD",     /* Web Forward */
    "WEBBACK",        /* Web Back */
    "MYCOMPUTER",     /* My Computer */
    "MAIL",           /* Mail */
    "MEDIASELECT"     /* Media Select */
};

NiImplementRTTI(NiUIHotkeyImage, NiUIBaseElement);

//---------------------------------------------------------------------------
NiUIHotkeyImage::NiUIHotkeyImage() :
    NiUIBaseElement(false)
{
}
//---------------------------------------------------------------------------
NiUIHotkeyImage::~NiUIHotkeyImage()
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kImages.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kImages.SetAt(uiIdx, NULL);
    }
    uiSize = m_kLabels.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kLabels.SetAt(uiIdx, NULL);
    }
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::SetKeyboardHotkey(NiInputKeyboard::KeyCode eKey,
    NiInputKeyboard::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;
    // modifiers alone aren't allowed and the "No Key" is hard to display
    if ((eKey == NiInputKeyboard::KEY_NOKEY) || 
        // Must be a valid key
        (eKey >= NiInputKeyboard::KEY_TOTAL_COUNT))
    {
        return;
    }

    
    const char* pcLabelText = NULL;
    float fWidth = 2.0f;
    NiUIAtlasMap::UIElement eElement = 
        NiUIAtlasMap::KEYBOARD_BUTTON;
    NiUIAtlasMap::TextColor eTextColor = 
        NiUIAtlasMap::TEXT_ON_KEYBOARD_BUTTON;

    bool bModifier = false;

    // First I need to add the modifier if there is one
    // (since these are rendered left to right)
    if (eModifier != NiInputKeyboard::KMOD_NONE)
    {
        bModifier = true;
        eElement = NiUIAtlasMap::KEYBOARD_BUTTON_WIDE;
        
        switch (eModifier)
        {
            case (NiInputKeyboard::KMOD_LCONTROL):
            case (NiInputKeyboard::KMOD_RCONTROL):
                pcLabelText = "CTRL";
                break;
            case (NiInputKeyboard::KMOD_LMENU):
            case (NiInputKeyboard::KMOD_RMENU):
                pcLabelText = "Menu";
                break;
            case (NiInputKeyboard::KMOD_LWIN):
            case (NiInputKeyboard::KMOD_RWIN):
                pcLabelText = "WIN";
                break;
            case (NiInputKeyboard::KMOD_LSHIFT):
            case (NiInputKeyboard::KMOD_RSHIFT):
                pcLabelText = "Shift";
                break;
            case (NiInputKeyboard::KMOD_CAPS_LOCK):
                pcLabelText = "CAPS";
                break;
            default:
                NIASSERT(!"Unknown keyboard modifier!");
                break;
        }

        InsertHotkeyElement(eElement, fWidth, true, pcLabelText, eTextColor);
    }
    
    pcLabelText = NULL;
    fWidth = 1.0f;
    eElement = NiUIAtlasMap::KEYBOARD_BUTTON;
    eTextColor = NiUIAtlasMap::TEXT_ON_KEYBOARD_BUTTON;

    // Now I need to set up the image, text, and bounds of the true button
    
    if (IsDoubleWide(eKey))
    {
        eElement = NiUIAtlasMap::KEYBOARD_BUTTON_WIDE;
        fWidth = 2.0f;
    }
    
    pcLabelText = ms_pcKeyNameMap[eKey];

    InsertHotkeyElement(eElement, fWidth, !bModifier, pcLabelText, eTextColor);
    UpdateRect();
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::SetGamePadAxis(NiInputGamePad::DefaultAxis eAxis, 
    bool bUseBothDirs)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;
    if ((eAxis > NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT) || (eAxis < 0))
        return;

    const char* pcLabelText = NULL;
    float fWidth = 1.0f;
    NiUIAtlasMap::UIElement eElement = 
        NiUIAtlasMap::GAMEPAD_CONTROL_STICK;
    NiUIAtlasMap::TextColor eTextColor = 
        NiUIAtlasMap::TEXT_ON_GAMEPAD_CONTROL_STICK;

    // No modifiers, it just depends on if you have reserved just one dir 
    // of the axis or both.
    switch (eAxis)
    {
        case (NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ):
            if (bUseBothDirs)
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_OMNI;
            else
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_HORIZONTAL;
            pcLabelText = "L";
            break;
        case (NiInputGamePad::NIGP_DEFAULT_LEFT_VERT):
            if (bUseBothDirs)
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_OMNI;
            else
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_VERTICAL;
            pcLabelText = "L";
            break;
        case (NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ):
#if defined(_WII)
            eElement = NiUIAtlasMap::WII_REMOTE_WAGGLE;
#else
            if (bUseBothDirs)
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_OMNI;
            else
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_HORIZONTAL;
#endif
            pcLabelText = "R";
            break;
        case (NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT):
#if defined(_WII)
            eElement = NiUIAtlasMap::WII_REMOTE_WAGGLE;
#else
            if (bUseBothDirs)
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_OMNI;
            else
                eElement = NiUIAtlasMap::GAMEPAD_CONTROL_STICK_VERTICAL;
#endif
            pcLabelText = "R";
            break;
        default:
            NIASSERT(!"Unknown gamepad axis!");
            break;

    }

    InsertHotkeyElement(eElement, fWidth, true, pcLabelText, eTextColor);
    UpdateRect();
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::SetGamePadHotkey(NiInputGamePad::Button eButton, 
    NiInputGamePad::ButtonMask eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;
    if ((eButton <= NiInputGamePad::NIGP_NONE) || 
        (eButton >= NiInputGamePad::NIGP_USER_0))
    {
        return;
    }

    const char* pcLabelText = NULL;
    float fWidth = 1.0f;
    NiUIAtlasMap::UIElement eElement = 
        NiUIAtlasMap::GAMEPAD_DPAD;
    NiUIAtlasMap::TextColor eTextColor = 
        NiUIAtlasMap::TEXT_ON_GAMEPAD_CONTROL_STICK;

    bool bModifier = false;
    bool bButtonSupported = true;
    // Modifier first because this renders left to right
    if (eModifier != NiInputGamePad::NIGP_MASK_NONE)
    {
        bModifier = true;
        switch (eModifier)
        {
            case (NiInputGamePad::NIGP_MASK_LUP):
                eElement = NiUIAtlasMap::GAMEPAD_DPAD;
                pcLabelText = "U";
                break;
            case (NiInputGamePad::NIGP_MASK_LDOWN):
                eElement = NiUIAtlasMap::GAMEPAD_DPAD;
                pcLabelText = "D";
                break;
            case (NiInputGamePad::NIGP_MASK_LLEFT):
                eElement = NiUIAtlasMap::GAMEPAD_DPAD;
                pcLabelText = "L";
                break;
            case (NiInputGamePad::NIGP_MASK_LRIGHT):
                eElement = NiUIAtlasMap::GAMEPAD_DPAD;
                pcLabelText = "R";
                break;
            case (NiInputGamePad::NIGP_MASK_L1):
#if defined(_WII)
                bButtonSupported = false;
#endif
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_L1;
                break;
            case (NiInputGamePad::NIGP_MASK_L2):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_L2;
                break;
            case (NiInputGamePad::NIGP_MASK_R1):
#if defined(_WII)
                bButtonSupported = false;
#endif
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_R1;
                break;
            case (NiInputGamePad::NIGP_MASK_R2):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_R2;
                break;
            case (NiInputGamePad::NIGP_MASK_RUP):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RUP;
                break;
            case (NiInputGamePad::NIGP_MASK_RDOWN):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RDOWN;
                break;
            case (NiInputGamePad::NIGP_MASK_RLEFT):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RLEFT;
                break;
            case (NiInputGamePad::NIGP_MASK_RRIGHT):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RRIGHT;
                break;
            case (NiInputGamePad::NIGP_MASK_START):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_START;
                break;
            case (NiInputGamePad::NIGP_MASK_SELECT):
                eElement = NiUIAtlasMap::GAMEPAD_BUTTON_SELECT;
                break;
            // At present, these two buttons are unknowns.
            case (NiInputGamePad::NIGP_MASK_A):
            case (NiInputGamePad::NIGP_MASK_B):
                break;
            default:
                NIASSERT(!"Unknown gamepad button!");
                break;

        }
        if (bButtonSupported)
            InsertHotkeyElement(eElement, fWidth, true, pcLabelText, eTextColor);
    }

    pcLabelText = NULL;
    fWidth = 1.0f;
    eElement = NiUIAtlasMap::GAMEPAD_DPAD;
    eTextColor = NiUIAtlasMap::TEXT_ON_GAMEPAD_CONTROL_STICK;
    bButtonSupported = true;

    switch (eButton)
    {
        case (NiInputGamePad::NIGP_LUP):
            eElement = NiUIAtlasMap::GAMEPAD_DPAD;
            pcLabelText = "U";
            break;
        case (NiInputGamePad::NIGP_LDOWN):
            eElement = NiUIAtlasMap::GAMEPAD_DPAD;
            pcLabelText = "D";
            break;
        case (NiInputGamePad::NIGP_LLEFT):
            eElement = NiUIAtlasMap::GAMEPAD_DPAD;
            pcLabelText = "L";
            break;
        case (NiInputGamePad::NIGP_LRIGHT):
            eElement = NiUIAtlasMap::GAMEPAD_DPAD;
            pcLabelText = "R";
            break;
        case (NiInputGamePad::NIGP_L1):
#if defined(_WII)
            bButtonSupported = false;
#endif
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_L1;
            break;
        case (NiInputGamePad::NIGP_L2):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_L2;
            break;
        case (NiInputGamePad::NIGP_R1):
#if defined(_WII)
            bButtonSupported = false;
#endif
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_R1;
            break;
        case (NiInputGamePad::NIGP_R2):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_R2;
            break;
        case (NiInputGamePad::NIGP_RUP):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RUP;
            break;
        case (NiInputGamePad::NIGP_RDOWN):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RDOWN;
            break;
        case (NiInputGamePad::NIGP_RLEFT):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RLEFT;
            break;
        case (NiInputGamePad::NIGP_RRIGHT):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_RRIGHT;
            break;
        case (NiInputGamePad::NIGP_START):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_START;
            break;
        case (NiInputGamePad::NIGP_SELECT):
            eElement = NiUIAtlasMap::GAMEPAD_BUTTON_SELECT;
            break;
        // At present, these two buttons are unknowns.
        case (NiInputGamePad::NIGP_A):
        case (NiInputGamePad::NIGP_B):
            break;
        default:
            NIASSERT(!"Unknown gamepad button");
            break;
    }
    if (bButtonSupported)
        InsertHotkeyElement(eElement, fWidth, !bModifier, pcLabelText, eTextColor);
    UpdateRect();
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::SetMouseHotkey(NiInputMouse::Button eButton,
        NiInputMouse::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;
    if ((eButton == NiInputMouse::NIM_NONE) || 
        (eButton > NiInputMouse::NIM_MIDDLE))
    {
        return;
    }

    bool bModifier = false;
    if (eModifier != NiInputMouse::MMOD_NONE)
    {
        bModifier = true;
        const char* pcLabelText = NULL;
        float fModWidth = 1.0f;
        NiUIAtlasMap::UIElement eModElement = 
            NiUIAtlasMap::KEYBOARD_BUTTON_WIDE;
        NiUIAtlasMap::TextColor eTextColor = 
            NiUIAtlasMap::TEXT_ON_KEYBOARD_BUTTON;

        switch (eModifier)
        {
            case (NiInputMouse::MMOD_LCONTROL):
            case (NiInputMouse::MMOD_RCONTROL):
                pcLabelText = "CTRL";
                break;
            case (NiInputMouse::MMOD_LMENU):
            case (NiInputMouse::MMOD_RMENU):
                pcLabelText = "Menu";
                break;
            case (NiInputMouse::MMOD_LWIN):
            case (NiInputMouse::MMOD_RWIN):
                pcLabelText = "WIN";
                break;
            case (NiInputMouse::MMOD_LSHIFT):
            case (NiInputMouse::MMOD_RSHIFT):
                pcLabelText = "Shift";
                break;
            case (NiInputMouse::MMOD_CAPS_LOCK):
                pcLabelText = "CAPS";
                break;
            case (NiInputMouse::MMOD_LEFT):
                eModElement = NiUIAtlasMap::MOUSE_BUTTON_LEFT;
                break;
            case (NiInputMouse::MMOD_RIGHT):
                eModElement = NiUIAtlasMap::MOUSE_BUTTON_RIGHT;;
                break;
            case (NiInputMouse::MMOD_MIDDLE):
                eModElement = NiUIAtlasMap::MOUSE_BUTTON_SCROLL;
                break;
            default:
                NIASSERT(!"Unknown mouse modifier!");
                break;

        }

        if (pcLabelText != NULL)
            fModWidth = 2.0f;
        else
            fModWidth = 1.0f;

        InsertHotkeyElement(eModElement, fModWidth, true, pcLabelText,
            eTextColor);

    }

    float fWidth = 1.0f;
    NiUIAtlasMap::UIElement eElement = 
        NiUIAtlasMap::KEYBOARD_BUTTON_WIDE;

    switch (eButton)
    {
        case (NiInputMouse::NIM_LEFT):
            eElement = NiUIAtlasMap::MOUSE_BUTTON_LEFT;
            break;
        case (NiInputMouse::NIM_RIGHT):
            eElement = NiUIAtlasMap::MOUSE_BUTTON_RIGHT;
            break;
        case (NiInputMouse::NIM_MIDDLE):
            eElement = NiUIAtlasMap::MOUSE_BUTTON_SCROLL;
            break;
        default:
            NIASSERT(!"Unknown mouse button");
            break;
    }
 
    InsertHotkeyElement(eElement, fWidth, !bModifier);
    UpdateRect();
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::SetMouseMotion()
{
    if (NiUIManager::GetUIManager() == NULL)
        return;

    InsertHotkeyElement(NiUIAtlasMap::MOUSE_MOVE, 1.0f);
    UpdateRect();
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::InsertHotkeyElement(NiUIAtlasMap::UIElement eElement,
    float fWidth, bool bInsertSpacerPrior, const char* pcLabelText, 
    NiUIAtlasMap::TextColor eTextColor)
{
    const NiUIAtlasMap* pkMap = NiUIManager::GetUIManager()->GetAtlasMap();

    if (!pkMap)
        return;

    if (m_kImages.GetSize() > 0 && bInsertSpacerPrior)
    {
        // Add a spacer because there are other images already in there
        m_kImages.Add(NULL);
        m_kLabels.Add(NULL);
        m_kImageWidth.Add(ms_fSpacerWidth);
        m_kElementIds.Add(NiUIAtlasMap::NUM_ELEMENTS);
    }

    NiUIImagePtr spImage = NiNew NiUIImage();
    spImage->SetParent(this);
    NiRect<float> kRect = pkMap->GetOuterBounds(eElement);
    NiRect<float> kTextBounds = pkMap->GetInnerBounds(eElement);
    spImage->SetTextureCoords(kRect);
    m_kImages.Add(spImage);

    if (pcLabelText != NULL)
    {
        NiColor kTextColor(pkMap->GetTextColor(eTextColor));

        NiUILabelPtr spLabel = NiNew NiUILabel(pcLabelText, 
            NiColorA(0, 0, 0, 0), kTextColor);
        spLabel->SetParent(this);
        spLabel->SetAlignment(NiUILabel::VERT_MIDDLE, 
            NiUILabel::HORIZ_MIDDLE);
        
        m_kLabels.Add(spLabel);
    }
    else
    {
        m_kLabels.Add(NULL);
    }

    m_kElementIds.Add(eElement);
    m_kImageWidth.Add(fWidth);
}
//---------------------------------------------------------------------------
void NiUIHotkeyImage::UpdateLayout()
{
    // Now I need to propogate the change to my children.
    // This only sets the children up horizontally. 

    unsigned int uiIdx, uiSize;
    float fNumWide;
    uiSize = m_kImageWidth.GetSize();
    if (uiSize == 0)
        return;
    
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    if (!pkManager)
        return;

    const NiUIAtlasMap* pkAtlasMap = pkManager->GetAtlasMap();

    if (!pkAtlasMap)
        return;

    // Get the current dimensions in normalized screen coordinates
    float fWidth = m_kNSCRect.GetWidth();
    float fHeight = m_kNSCRect.GetHeight();

    if (fWidth == 0.0f || fHeight == 0.0f)
        return;

    // Compute the normalized screen coordinate dimensions for a single element
    fNumWide = 0.0f;
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        fNumWide += m_kImageWidth.GetAt(uiIdx);
    }
    float fOneWideInNSC = fWidth / fNumWide;

    float fLeftU = 0.0f;
    float fTopV = 0.0f;

    // Layout the elements horizontally, resizing their dimensions so
    // that they keep their original aspect ratio.
    float fElementLeft = fLeftU;
    NIASSERT(m_kImages.GetSize() == uiSize);
    NIASSERT(m_kLabels.GetSize() == uiSize);
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        // Compute the element's working rectangle
        float fCurrentWidthInElements = m_kImageWidth.GetAt(uiIdx);
        float fCurrentWidthInNSC = fCurrentWidthInElements * fOneWideInNSC;
        NiRect<float> kNSCElementRect;
        kNSCElementRect.m_left = fElementLeft;
        kNSCElementRect.m_right = kNSCElementRect.m_left + fCurrentWidthInNSC;
        kNSCElementRect.m_top = fTopV;
        kNSCElementRect.m_bottom = fTopV + fHeight;

        // Slide the current leftmost edge to be beside this element
        fElementLeft = kNSCElementRect.m_right;

        // If this is just a spacer, continue working with the next elements
        NiUIAtlasMap::UIElement eCurrentElement = m_kElementIds.GetAt(uiIdx);
        if (eCurrentElement == NiUIAtlasMap::NUM_ELEMENTS)
            continue;

        // Compute the Image's rectangle within the element's working
        // rectangle.
        NiRect<float> kNSCImageRect = ComputeAspectRatioPreservingInnerNSCRect(
            eCurrentElement, kNSCElementRect);
        
        // Compute the Label's rectangle within the Image's working rectangle
        NiRect<float> kNSCLabelRect = kNSCImageRect;

        // Set the computed dimensions on the Images & Labels, if they exist
        if (m_kImages.GetAt(uiIdx) != NULL)
        {
            m_kImages.GetAt(uiIdx)->SetOffset(kNSCImageRect.m_left, 
                kNSCImageRect.m_top);
            m_kImages.GetAt(uiIdx)->SetDimensions(kNSCImageRect.GetWidth(), 
                kNSCImageRect.GetHeight());
        }
        if (m_kLabels.GetAt(uiIdx) != NULL)
        {
            m_kLabels.GetAt(uiIdx)->SetOffset(kNSCLabelRect.m_left, 
                kNSCLabelRect.m_top);
            m_kLabels.GetAt(uiIdx)->SetDimensions(kNSCLabelRect.GetWidth(), 
                kNSCLabelRect.GetHeight());
        }
    }
}
//---------------------------------------------------------------------------
unsigned int NiUIHotkeyImage::GetChildElementCount() const
{
    return m_kImages.GetSize() + m_kLabels.GetSize();
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUIHotkeyImage::GetChildElement(unsigned int uiElement) const
{
    if (uiElement < m_kImages.GetSize())
        return m_kImages.GetAt(uiElement);

    uiElement -= m_kImages.GetSize();

    if (uiElement < m_kLabels.GetSize())
        return m_kLabels.GetAt(uiElement);

    return NULL;
}
//---------------------------------------------------------------------------
