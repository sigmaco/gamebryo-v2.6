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

#ifndef NIINPUTDI8KEYBOARD_H
#define NIINPUTDI8KEYBOARD_H

#include "NiInputKeyboard.h"
#include "NiDI8Headers.h"

class NIINPUT_ENTRY NiInputDI8Keyboard : public NiInputKeyboard
{
    NiDeclareRTTI;

    enum
    {
        NIDI8KB_NUM_KEYS = 256
    };

public:
    NiInputDI8Keyboard(NiInputDevice::Description* pkDescription,
        LPDIRECTINPUTDEVICE8 pkDIDevice, unsigned int uiUsage);
    virtual ~NiInputDI8Keyboard();

    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    // *** begin Emergent internal use only ***
    inline const LPDIRECTINPUTDEVICE8 GetDIDevice();
    // *** end Emergent internal use only ***

protected:
    void UpdateKeymap();
    void FillMapping();
    const char* KeyCodeToString(unsigned int uiDIKeyCode);

    unsigned int m_uiBufferSize;

    LPDIRECTINPUTDEVICE8 m_pkDIDevice;
    KeyCode m_aeDIToNIIMapping[NIDI8KB_NUM_KEYS];
};

NiSmartPointer(NiInputDI8Keyboard);

#include "NiInputDI8Keyboard.inl"

#endif  //#ifndef NIINPUTDI8KEYBOARD_H
