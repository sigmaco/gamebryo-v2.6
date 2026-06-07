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

// NifKeyboardShortcuts.cpp

#include "stdafx.h"
#include "NifKeyboardShortcuts.h"
#include "NiSceneKeyboard.h"

UINT CNifKeyboardShortcuts::ms_uiFlags[KBD_SHORTCUT_COUNT];

//---------------------------------------------------------------------------
CNifKeyboardShortcuts::CNifKeyboardShortcuts()
{
}
//---------------------------------------------------------------------------
CNifKeyboardShortcuts::~CNifKeyboardShortcuts()
{
}
//---------------------------------------------------------------------------
void CNifKeyboardShortcuts::Load()
{

    ms_uiFlags[KBD_CAMERA_MOVE_FORWARD] = NiSceneKeyboard::KEY_W ; 
    ms_uiFlags[KBD_CAMERA_MOVE_BACK]    = NiSceneKeyboard::KEY_S;
    ms_uiFlags[KBD_CAMERA_MOVE_LEFT]    = NiSceneKeyboard::KEY_A;
    ms_uiFlags[KBD_CAMERA_MOVE_RIGHT]   = NiSceneKeyboard::KEY_D;
    ms_uiFlags[KBD_CAMERA_MOVE_UP]      = NiSceneKeyboard::KEY_R;
    ms_uiFlags[KBD_CAMERA_MOVE_DOWN]    = NiSceneKeyboard::KEY_F;
    ms_uiFlags[KBD_CAMERA_PITCH_UP]     = NiSceneKeyboard::KEY_UP;
    ms_uiFlags[KBD_CAMERA_PITCH_DOWN]   = NiSceneKeyboard::KEY_DOWN;
    ms_uiFlags[KBD_CAMERA_YAW_LEFT]     = NiSceneKeyboard::KEY_LEFT;
    ms_uiFlags[KBD_CAMERA_YAW_RIGHT]    = NiSceneKeyboard::KEY_RIGHT;
}
//---------------------------------------------------------------------------
void CNifKeyboardShortcuts::Save()
{
}
//---------------------------------------------------------------------------
UINT CNifKeyboardShortcuts::GetKeyboardShortcut(ShortcutFlag eFlag)
{
    if(eFlag >= KBD_SHORTCUT_COUNT || eFlag < 0)
        return (UINT) -1;
    else
        return ms_uiFlags[eFlag];
}
//---------------------------------------------------------------------------
void CNifKeyboardShortcuts::SetKeyboardShortcut(ShortcutFlag eFlag,
    UINT uiKey)
{
    if(eFlag >= KBD_SHORTCUT_COUNT || eFlag < 0)
        return;
    else
        ms_uiFlags[eFlag] = uiKey;
}
//---------------------------------------------------------------------------
