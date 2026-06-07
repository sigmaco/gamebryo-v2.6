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

// NifKeyboardShortcuts.h

#ifndef NIFKEYBOARDSHORTCUTS_H
#define NIFKEYBOARDSHORTCUTS_H

class CNifKeyboardShortcuts
{
    public:
        enum ShortcutFlag 
        {
            KBD_CAMERA_MOVE_FORWARD = 0,
            KBD_CAMERA_MOVE_BACK,
            KBD_CAMERA_MOVE_LEFT,
            KBD_CAMERA_MOVE_RIGHT,
            KBD_CAMERA_MOVE_UP,
            KBD_CAMERA_MOVE_DOWN,
            KBD_CAMERA_PITCH_UP,
            KBD_CAMERA_PITCH_DOWN,
            KBD_CAMERA_YAW_LEFT,
            KBD_CAMERA_YAW_RIGHT,
            KBD_SHORTCUT_COUNT
        };

        static void Load();
        static void Save();

        CNifKeyboardShortcuts();
        ~CNifKeyboardShortcuts();
        UINT GetKeyboardShortcut(ShortcutFlag eFlag);
        void SetKeyboardShortcut(ShortcutFlag eFlag, UINT uiKey);

    protected:
        static UINT ms_uiFlags[KBD_SHORTCUT_COUNT];
};

#endif  // #ifndef NIFKEYBOARDSHORTCUTS_H
