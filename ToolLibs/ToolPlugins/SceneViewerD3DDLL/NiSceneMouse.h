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

#ifndef NISCENEMOUSE_H
#define NISCENEMOUSE_H

#include "NiDevice.h"
#include <NiUniversalTypes.h>
#include "NiSceneKeyboard.h"

class NiSceneMouse : public NiDevice
{
public:
    // construction
    NiSceneMouse();
    
    // to be used as the input side index of the device 
    // mapping array
    enum Button
    {
        SCENEMOUSE_NOBUTTON,
        SCENEMOUSE_LBUTTON,
        SCENEMOUSE_MBUTTON,
        SCENEMOUSE_RBUTTON,
        SCENEMOUSE_WHEEL,
        SCENEMOUSE_ENDOFLIST
    };

    // Emergent reserves modifiers in the lower 16 bits - an application can
    // add its own modifiers in the upper 16 bits
    enum Modifiers
    { 
        MMOD_NONE    = 0x00000000,
        MMOD_CONTROL = 0x00000001,
        MMOD_MENU    = 0x00000002,
        MMOD_WIN     = 0x00000004,
        MMOD_SHIFT   = 0x00000008,
        MMOD_LEFT    = 0x00000010,
        MMOD_MIDDLE  = 0x00000020,
        MMOD_RIGHT   = 0x00000040,
        MMOD_WHEEL   = 0x00000080
    };

    // Query state of mouse
    bool ButtonIsDown(Button eButton);
    void GetMouseLocation(float* pfHor, float* pfVer, NiWindowRef pkWnd);
    void GetMouseLocation(int* piHor, int* piVer, NiWindowRef pkWnd);
    short GetWheelDelta();
    void GetCurrentMouseLocation(int& iX, int& iY);
    void SetKeyboard(NiSceneKeyboard* pkKeyboard);
    void SetFullscreenMode(bool bFullscreen);
    void GetPreviousMouseLocation(int& iHor, int& iVer);
    void ResetPreviousMouseLocation();
    void UpdatePreviousMousePosition(int iHor, int iVer);
    // *** begin Emergent internal use only ***

    // Keep track of state of mouse
    void RecordButtonPress(Button eButton);
    void RecordButtonRelease(Button eButton);
    void RecordMouseMove(int iX, int iY);
    void RecordWheelMoved(short sDelta);
    // create NI modifiers from platform-specific modifiers
    long GetModifiers();
    unsigned int GetMouseButtons();
    // *** end Emergent internal use only ***
    void UpdateDevice();

protected:
    // state of mouse buttons
    bool m_abMouseState[4];
    int m_iPreviousMouseHor;
    int m_iPreviousMouseVer;
    int m_iCurrentMouseHor;
    int m_iCurrentMouseVer;
    NiSceneKeyboard* m_pkKeyboard;
    bool m_bFullscreen;
    short m_sWheelDelta;
};

#endif

