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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiInputPCH.h"

#include "NiInputDI8Keyboard.h"
#include "NiDI8InputSystem.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiInputDI8Keyboard, NiInputKeyboard);
//---------------------------------------------------------------------------
NiInputDI8Keyboard::NiInputDI8Keyboard(
    NiInputDevice::Description* pkDescription, 
    LPDIRECTINPUTDEVICE8 pkDIDevice, unsigned int uiUsage) :
    NiInputKeyboard(pkDescription),
    m_uiBufferSize(64),
    m_pkDIDevice(pkDIDevice)
{
    if (!m_pkDIDevice)
        return;

    FillMapping();

    // Need to set the keyboard data format and the cooperative mode.
    HRESULT hr;
    hr = m_pkDIDevice->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_DEVICEINITFAIL;
        return;
    }

    DIPROPDWORD kDIPropDW;
    kDIPropDW.diph.dwSize = sizeof(DIPROPDWORD);
    kDIPropDW.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    kDIPropDW.diph.dwObj = 0;
    kDIPropDW.diph.dwHow = DIPH_DEVICE;
    kDIPropDW.dwData = m_uiBufferSize;

    hr = m_pkDIDevice->SetProperty(DIPROP_BUFFERSIZE, &kDIPropDW.diph);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_DEVICEINITFAIL;
        return;
    }

    DWORD dwFlags = 0;

    if (uiUsage & NiInputSystem::FOREGROUND)
        dwFlags |= DISCL_FOREGROUND;
    if (uiUsage & NiInputSystem::BACKGROUND)
        dwFlags |= DISCL_BACKGROUND;
    if (uiUsage & NiInputSystem::EXCLUSIVE)
        dwFlags |= DISCL_EXCLUSIVE;
    if (uiUsage & NiInputSystem::NONEXCLUSIVE)
        dwFlags |= DISCL_NONEXCLUSIVE;

    hr = m_pkDIDevice->SetCooperativeLevel(
        NiDI8InputSystem::GetOwnerWindow(), dwFlags);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_DEVICEINITFAIL;
        return;
    }

    hr = m_pkDIDevice->Acquire();
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_DEVICELOST;
    }
    else
    {
        UpdateKeymap();
    }
}
//---------------------------------------------------------------------------
NiInputDI8Keyboard::~NiInputDI8Keyboard()
{
    if (m_pkDIDevice)
    {
        m_pkDIDevice->Unacquire();
        m_pkDIDevice->Release();
    }
    m_pkDIDevice = 0;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8Keyboard::UpdateDevice()
{
    m_eLastError = NIIERR_OK;

    NiInputKeyboard::UpdateDevice();

    if (!m_pkDIDevice)
    {
        m_eLastError = NIIERR_INVALIDDEVICE;
        return m_eLastError;
    }

    // Make a reasonably sized buffer on the stack to store all the
    // keyboard events since the last time UpdateDevice was called.
    const unsigned int uiBufferSize = 16;
    DWORD dwElements;
    DIDEVICEOBJECTDATA kDIDOD[uiBufferSize];
    HRESULT hr;

    do
    {
        // passed in as buffer size, returned filled with number of elements
        dwElements = uiBufferSize;

        hr = m_pkDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), kDIDOD, 
            &dwElements, 0);
        if (FAILED(hr))
        {
            if ((hr == DIERR_INPUTLOST) ||
                (hr == DIERR_NOTACQUIRED) ||
                (hr == DIERR_OTHERAPPHASPRIO))
            {
                HandleInsertion();
                m_eLastError = NIIERR_DEVICELOST;
            }
            else
            {
                m_eLastError = NIIERR_DEVICEERROR;
            }

            return m_eLastError;
        }

        for (unsigned int i = 0; i < dwElements; i++)
        {
            // Only the low byte of dwData is significant.
            if (kDIDOD[i].dwData & 0x80)
            {
                RecordKeyPress(m_aeDIToNIIMapping[kDIDOD[i].dwOfs]);
            }
            else
            {
                RecordKeyRelease(m_aeDIToNIIMapping[kDIDOD[i].dwOfs]);
            }
        }
    }
    while (dwElements != 0);

    m_eLastError = NIIERR_OK;
    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8Keyboard::HandleRemoval()
{
    m_pkDIDevice->Unacquire();

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8Keyboard::HandleInsertion()
{
    HRESULT hr = m_pkDIDevice->Acquire();
    if (FAILED(hr))
    {
        if ((hr == DIERR_INPUTLOST) ||
            (hr == DIERR_NOTACQUIRED) ||
            (hr == DIERR_OTHERAPPHASPRIO))
        {
            m_eLastError = NIIERR_DEVICELOST;
        }
        else
        {
            m_eLastError = NIIERR_DEVICEERROR;
        }
    }
    else
    {
        UpdateKeymap();
        m_eLastError = NIIERR_OK;
    }
    return m_eLastError;
}
//---------------------------------------------------------------------------
void NiInputDI8Keyboard::UpdateKeymap()
{
    BYTE pcKeymap[256];
    HRESULT hr = m_pkDIDevice->GetDeviceState(sizeof(pcKeymap), pcKeymap);
    if (FAILED(hr))
    {
        if ((hr == DIERR_INPUTLOST) ||
            (hr == DIERR_NOTACQUIRED) ||
            (hr == DIERR_OTHERAPPHASPRIO))
        {
            m_eLastError = NIIERR_DEVICELOST;
        }
        else
        {
            m_eLastError = NIIERR_DEVICEERROR;
        }
    }

    m_bCapsLock = false;
    m_uiModifiers = 0;
    for (unsigned int i = 0; i < 256; i++)
    {
        NiInputKeyboard::KeyCode eCode = m_aeDIToNIIMapping[i];
        if (eCode == NiInputKeyboard::KEY_NOKEY)
            continue;

        if (pcKeymap[i] & 0x80)
        {
            RecordKeyPress(eCode);
        }
        else
        {
            RecordKeyRelease(eCode);
        }
    }
}
//---------------------------------------------------------------------------
void NiInputDI8Keyboard::FillMapping()
{
    memset((void*)m_aeDIToNIIMapping, 0, sizeof(m_aeDIToNIIMapping));

    m_aeDIToNIIMapping[DIK_ESCAPE] = KEY_ESCAPE;
    m_aeDIToNIIMapping[DIK_1] = KEY_1;
    m_aeDIToNIIMapping[DIK_2] = KEY_2;
    m_aeDIToNIIMapping[DIK_3] = KEY_3;
    m_aeDIToNIIMapping[DIK_4] = KEY_4;
    m_aeDIToNIIMapping[DIK_5] = KEY_5;
    m_aeDIToNIIMapping[DIK_6] = KEY_6;
    m_aeDIToNIIMapping[DIK_7] = KEY_7;
    m_aeDIToNIIMapping[DIK_8] = KEY_8;
    m_aeDIToNIIMapping[DIK_9] = KEY_9;
    m_aeDIToNIIMapping[DIK_0] = KEY_0;
    m_aeDIToNIIMapping[DIK_MINUS] = KEY_MINUS;
    m_aeDIToNIIMapping[DIK_EQUALS] = KEY_EQUALS;
    m_aeDIToNIIMapping[DIK_BACK] = KEY_BACK;
    m_aeDIToNIIMapping[DIK_TAB] = KEY_TAB;
    m_aeDIToNIIMapping[DIK_Q] = KEY_Q;
    m_aeDIToNIIMapping[DIK_W] = KEY_W;
    m_aeDIToNIIMapping[DIK_E] = KEY_E;
    m_aeDIToNIIMapping[DIK_R] = KEY_R;
    m_aeDIToNIIMapping[DIK_T] = KEY_T;
    m_aeDIToNIIMapping[DIK_Y] = KEY_Y;
    m_aeDIToNIIMapping[DIK_U] = KEY_U;
    m_aeDIToNIIMapping[DIK_I] = KEY_I;
    m_aeDIToNIIMapping[DIK_O] = KEY_O;
    m_aeDIToNIIMapping[DIK_P] = KEY_P;
    m_aeDIToNIIMapping[DIK_LBRACKET] = KEY_LBRACKET;
    m_aeDIToNIIMapping[DIK_RBRACKET] = KEY_RBRACKET;
    m_aeDIToNIIMapping[DIK_RETURN] = KEY_RETURN;
    m_aeDIToNIIMapping[DIK_LCONTROL] = KEY_LCONTROL;
    m_aeDIToNIIMapping[DIK_A] = KEY_A;
    m_aeDIToNIIMapping[DIK_S] = KEY_S;
    m_aeDIToNIIMapping[DIK_D] = KEY_D;
    m_aeDIToNIIMapping[DIK_F] = KEY_F;
    m_aeDIToNIIMapping[DIK_G] = KEY_G;
    m_aeDIToNIIMapping[DIK_H] = KEY_H;
    m_aeDIToNIIMapping[DIK_J] = KEY_J;
    m_aeDIToNIIMapping[DIK_K] = KEY_K;
    m_aeDIToNIIMapping[DIK_L] = KEY_L;
    m_aeDIToNIIMapping[DIK_SEMICOLON] = KEY_SEMICOLON;
    m_aeDIToNIIMapping[DIK_APOSTROPHE] = KEY_APOSTROPHE;
    m_aeDIToNIIMapping[DIK_GRAVE] = KEY_GRAVE;
    m_aeDIToNIIMapping[DIK_LSHIFT] = KEY_LSHIFT;
    m_aeDIToNIIMapping[DIK_BACKSLASH] = KEY_BACKSLASH;
    m_aeDIToNIIMapping[DIK_Z] = KEY_Z;
    m_aeDIToNIIMapping[DIK_X] = KEY_X;
    m_aeDIToNIIMapping[DIK_C] = KEY_C;
    m_aeDIToNIIMapping[DIK_V] = KEY_V;
    m_aeDIToNIIMapping[DIK_B] = KEY_B;
    m_aeDIToNIIMapping[DIK_N] = KEY_N;
    m_aeDIToNIIMapping[DIK_M] = KEY_M;
    m_aeDIToNIIMapping[DIK_COMMA] = KEY_COMMA;
    m_aeDIToNIIMapping[DIK_PERIOD] = KEY_PERIOD;
    m_aeDIToNIIMapping[DIK_SLASH] = KEY_SLASH;
    m_aeDIToNIIMapping[DIK_RSHIFT] = KEY_RSHIFT;
    m_aeDIToNIIMapping[DIK_MULTIPLY] = KEY_MULTIPLY;
    m_aeDIToNIIMapping[DIK_LMENU] = KEY_LMENU;
    m_aeDIToNIIMapping[DIK_SPACE] = KEY_SPACE;
    m_aeDIToNIIMapping[DIK_CAPITAL] = KEY_CAPITAL;
    m_aeDIToNIIMapping[DIK_F1] = KEY_F1;
    m_aeDIToNIIMapping[DIK_F2] = KEY_F2;
    m_aeDIToNIIMapping[DIK_F3] = KEY_F3;
    m_aeDIToNIIMapping[DIK_F4] = KEY_F4;
    m_aeDIToNIIMapping[DIK_F5] = KEY_F5;
    m_aeDIToNIIMapping[DIK_F6] = KEY_F6;
    m_aeDIToNIIMapping[DIK_F7] = KEY_F7;
    m_aeDIToNIIMapping[DIK_F8] = KEY_F8;
    m_aeDIToNIIMapping[DIK_F9] = KEY_F9;
    m_aeDIToNIIMapping[DIK_F10] = KEY_F10;
    m_aeDIToNIIMapping[DIK_NUMLOCK] = KEY_NUMLOCK;
    m_aeDIToNIIMapping[DIK_SCROLL] = KEY_SCROLL;
    m_aeDIToNIIMapping[DIK_NUMPAD7] = KEY_NUMPAD7;
    m_aeDIToNIIMapping[DIK_NUMPAD8] = KEY_NUMPAD8;
    m_aeDIToNIIMapping[DIK_NUMPAD9] = KEY_NUMPAD9;
    m_aeDIToNIIMapping[DIK_SUBTRACT] = KEY_SUBTRACT;
    m_aeDIToNIIMapping[DIK_NUMPAD4] = KEY_NUMPAD4;
    m_aeDIToNIIMapping[DIK_NUMPAD5] = KEY_NUMPAD5;
    m_aeDIToNIIMapping[DIK_NUMPAD6] = KEY_NUMPAD6;
    m_aeDIToNIIMapping[DIK_ADD] = KEY_ADD;
    m_aeDIToNIIMapping[DIK_NUMPAD1] = KEY_NUMPAD1;
    m_aeDIToNIIMapping[DIK_NUMPAD2] = KEY_NUMPAD2;
    m_aeDIToNIIMapping[DIK_NUMPAD3] = KEY_NUMPAD3;
    m_aeDIToNIIMapping[DIK_NUMPAD0] = KEY_NUMPAD0;
    m_aeDIToNIIMapping[DIK_DECIMAL] = KEY_DECIMAL;
    m_aeDIToNIIMapping[DIK_OEM_102] = KEY_OEM_102;
    m_aeDIToNIIMapping[DIK_F11] = KEY_F11;
    m_aeDIToNIIMapping[DIK_F12] = KEY_F12;
    m_aeDIToNIIMapping[DIK_F13] = KEY_F13;
    m_aeDIToNIIMapping[DIK_F14] = KEY_F14;
    m_aeDIToNIIMapping[DIK_F15] = KEY_F15;
    m_aeDIToNIIMapping[DIK_KANA] = KEY_KANA;
    m_aeDIToNIIMapping[DIK_ABNT_C1] = KEY_ABNT_C1;
    m_aeDIToNIIMapping[DIK_CONVERT] = KEY_CONVERT;
    m_aeDIToNIIMapping[DIK_NOCONVERT] = KEY_NOCONVERT;
    m_aeDIToNIIMapping[DIK_YEN] = KEY_YEN;
    m_aeDIToNIIMapping[DIK_ABNT_C2] = KEY_ABNT_C2;
    m_aeDIToNIIMapping[DIK_NUMPADEQUALS] = KEY_NUMPADEQUALS;
    m_aeDIToNIIMapping[DIK_PREVTRACK] = KEY_PREVTRACK;
    m_aeDIToNIIMapping[DIK_AT] = KEY_AT;
    m_aeDIToNIIMapping[DIK_COLON] = KEY_COLON;
    m_aeDIToNIIMapping[DIK_UNDERLINE] = KEY_UNDERLINE;
    m_aeDIToNIIMapping[DIK_KANJI] = KEY_KANJI;
    m_aeDIToNIIMapping[DIK_STOP] = KEY_STOP;
    m_aeDIToNIIMapping[DIK_AX] = KEY_AX;
    m_aeDIToNIIMapping[DIK_UNLABELED] = KEY_UNLABELED;
    m_aeDIToNIIMapping[DIK_NEXTTRACK] = KEY_NEXTTRACK;
    m_aeDIToNIIMapping[DIK_NUMPADENTER] = KEY_NUMPADENTER;
    m_aeDIToNIIMapping[DIK_RCONTROL] = KEY_RCONTROL;
    m_aeDIToNIIMapping[DIK_MUTE] = KEY_MUTE;
    m_aeDIToNIIMapping[DIK_CALCULATOR] = KEY_CALCULATOR;
    m_aeDIToNIIMapping[DIK_PLAYPAUSE] = KEY_PLAYPAUSE;
    m_aeDIToNIIMapping[DIK_MEDIASTOP] = KEY_MEDIASTOP;
    m_aeDIToNIIMapping[DIK_VOLUMEDOWN] = KEY_VOLUMEDOWN;
    m_aeDIToNIIMapping[DIK_VOLUMEUP] = KEY_VOLUMEUP;
    m_aeDIToNIIMapping[DIK_WEBHOME] = KEY_WEBHOME;
    m_aeDIToNIIMapping[DIK_NUMPADCOMMA] = KEY_NUMPADCOMMA;
    m_aeDIToNIIMapping[DIK_DIVIDE] = KEY_DIVIDE;
    m_aeDIToNIIMapping[DIK_SYSRQ] = KEY_SYSRQ;
    m_aeDIToNIIMapping[DIK_RMENU] = KEY_RMENU;
    m_aeDIToNIIMapping[DIK_PAUSE] = KEY_PAUSE;
    m_aeDIToNIIMapping[DIK_HOME] = KEY_HOME;
    m_aeDIToNIIMapping[DIK_UP] = KEY_UP;
    m_aeDIToNIIMapping[DIK_PRIOR] = KEY_PRIOR;
    m_aeDIToNIIMapping[DIK_LEFT] = KEY_LEFT;
    m_aeDIToNIIMapping[DIK_RIGHT] = KEY_RIGHT;
    m_aeDIToNIIMapping[DIK_END] = KEY_END;
    m_aeDIToNIIMapping[DIK_DOWN] = KEY_DOWN;
    m_aeDIToNIIMapping[DIK_NEXT] = KEY_NEXT;
    m_aeDIToNIIMapping[DIK_INSERT] = KEY_INSERT;
    m_aeDIToNIIMapping[DIK_DELETE] = KEY_DELETE;
    m_aeDIToNIIMapping[DIK_LWIN] = KEY_LWIN;
    m_aeDIToNIIMapping[DIK_RWIN] = KEY_RWIN;
    m_aeDIToNIIMapping[DIK_APPS] = KEY_APPS;
    m_aeDIToNIIMapping[DIK_POWER] = KEY_POWER;
    m_aeDIToNIIMapping[DIK_SLEEP] = KEY_SLEEP;
    m_aeDIToNIIMapping[DIK_WAKE] = KEY_WAKE;
    m_aeDIToNIIMapping[DIK_WEBSEARCH] = KEY_WEBSEARCH;
    m_aeDIToNIIMapping[DIK_WEBFAVORITES] = KEY_WEBFAVORITES;
    m_aeDIToNIIMapping[DIK_WEBREFRESH] = KEY_WEBREFRESH;
    m_aeDIToNIIMapping[DIK_WEBSTOP] = KEY_WEBSTOP;
    m_aeDIToNIIMapping[DIK_WEBFORWARD] = KEY_WEBFORWARD;
    m_aeDIToNIIMapping[DIK_WEBBACK] = KEY_WEBBACK;
    m_aeDIToNIIMapping[DIK_MYCOMPUTER] = KEY_MYCOMPUTER;
    m_aeDIToNIIMapping[DIK_MAIL] = KEY_MAIL;
    m_aeDIToNIIMapping[DIK_MEDIASELECT] = KEY_MEDIASELECT;
}
//---------------------------------------------------------------------------
#define DIKB_KEYCODE_STRING(x)  case x: return #x;
//---------------------------------------------------------------------------
const char* NiInputDI8Keyboard::KeyCodeToString(unsigned int uiDIKeyCode)
{
    switch (uiDIKeyCode)
    {
    DIKB_KEYCODE_STRING(DIK_ESCAPE);
    DIKB_KEYCODE_STRING(DIK_1);
    DIKB_KEYCODE_STRING(DIK_2);
    DIKB_KEYCODE_STRING(DIK_3);
    DIKB_KEYCODE_STRING(DIK_4);
    DIKB_KEYCODE_STRING(DIK_5);
    DIKB_KEYCODE_STRING(DIK_6);
    DIKB_KEYCODE_STRING(DIK_7);
    DIKB_KEYCODE_STRING(DIK_8);
    DIKB_KEYCODE_STRING(DIK_9);
    DIKB_KEYCODE_STRING(DIK_0);
    DIKB_KEYCODE_STRING(DIK_MINUS);
    DIKB_KEYCODE_STRING(DIK_EQUALS);
    DIKB_KEYCODE_STRING(DIK_BACK);
    DIKB_KEYCODE_STRING(DIK_TAB);
    DIKB_KEYCODE_STRING(DIK_Q);
    DIKB_KEYCODE_STRING(DIK_W);
    DIKB_KEYCODE_STRING(DIK_E);
    DIKB_KEYCODE_STRING(DIK_R);
    DIKB_KEYCODE_STRING(DIK_T);
    DIKB_KEYCODE_STRING(DIK_Y);
    DIKB_KEYCODE_STRING(DIK_U);
    DIKB_KEYCODE_STRING(DIK_I);
    DIKB_KEYCODE_STRING(DIK_O);
    DIKB_KEYCODE_STRING(DIK_P);
    DIKB_KEYCODE_STRING(DIK_LBRACKET);
    DIKB_KEYCODE_STRING(DIK_RBRACKET);
    DIKB_KEYCODE_STRING(DIK_RETURN);
    DIKB_KEYCODE_STRING(DIK_LCONTROL);
    DIKB_KEYCODE_STRING(DIK_A);
    DIKB_KEYCODE_STRING(DIK_S);
    DIKB_KEYCODE_STRING(DIK_D);
    DIKB_KEYCODE_STRING(DIK_F);
    DIKB_KEYCODE_STRING(DIK_G);
    DIKB_KEYCODE_STRING(DIK_H);
    DIKB_KEYCODE_STRING(DIK_J);
    DIKB_KEYCODE_STRING(DIK_K);
    DIKB_KEYCODE_STRING(DIK_L);
    DIKB_KEYCODE_STRING(DIK_SEMICOLON);
    DIKB_KEYCODE_STRING(DIK_APOSTROPHE);
    DIKB_KEYCODE_STRING(DIK_GRAVE);
    DIKB_KEYCODE_STRING(DIK_LSHIFT);
    DIKB_KEYCODE_STRING(DIK_BACKSLASH);
    DIKB_KEYCODE_STRING(DIK_Z);
    DIKB_KEYCODE_STRING(DIK_X);
    DIKB_KEYCODE_STRING(DIK_C);
    DIKB_KEYCODE_STRING(DIK_V);
    DIKB_KEYCODE_STRING(DIK_B);
    DIKB_KEYCODE_STRING(DIK_N);
    DIKB_KEYCODE_STRING(DIK_M);
    DIKB_KEYCODE_STRING(DIK_COMMA);
    DIKB_KEYCODE_STRING(DIK_PERIOD);
    DIKB_KEYCODE_STRING(DIK_SLASH);
    DIKB_KEYCODE_STRING(DIK_RSHIFT);
    DIKB_KEYCODE_STRING(DIK_MULTIPLY);
    DIKB_KEYCODE_STRING(DIK_LMENU);
    DIKB_KEYCODE_STRING(DIK_SPACE);
    DIKB_KEYCODE_STRING(DIK_CAPITAL);
    DIKB_KEYCODE_STRING(DIK_F1);
    DIKB_KEYCODE_STRING(DIK_F2);
    DIKB_KEYCODE_STRING(DIK_F3);
    DIKB_KEYCODE_STRING(DIK_F4);
    DIKB_KEYCODE_STRING(DIK_F5);
    DIKB_KEYCODE_STRING(DIK_F6);
    DIKB_KEYCODE_STRING(DIK_F7);
    DIKB_KEYCODE_STRING(DIK_F8);
    DIKB_KEYCODE_STRING(DIK_F9);
    DIKB_KEYCODE_STRING(DIK_F10);
    DIKB_KEYCODE_STRING(DIK_NUMLOCK);
    DIKB_KEYCODE_STRING(DIK_SCROLL);
    DIKB_KEYCODE_STRING(DIK_NUMPAD7);
    DIKB_KEYCODE_STRING(DIK_NUMPAD8);
    DIKB_KEYCODE_STRING(DIK_NUMPAD9);
    DIKB_KEYCODE_STRING(DIK_SUBTRACT);
    DIKB_KEYCODE_STRING(DIK_NUMPAD4);
    DIKB_KEYCODE_STRING(DIK_NUMPAD5);
    DIKB_KEYCODE_STRING(DIK_NUMPAD6);
    DIKB_KEYCODE_STRING(DIK_ADD);
    DIKB_KEYCODE_STRING(DIK_NUMPAD1);
    DIKB_KEYCODE_STRING(DIK_NUMPAD2);
    DIKB_KEYCODE_STRING(DIK_NUMPAD3);
    DIKB_KEYCODE_STRING(DIK_NUMPAD0);
    DIKB_KEYCODE_STRING(DIK_DECIMAL);
    DIKB_KEYCODE_STRING(DIK_OEM_102);
    DIKB_KEYCODE_STRING(DIK_F11);
    DIKB_KEYCODE_STRING(DIK_F12);
    DIKB_KEYCODE_STRING(DIK_F13);
    DIKB_KEYCODE_STRING(DIK_F14);
    DIKB_KEYCODE_STRING(DIK_F15);
    DIKB_KEYCODE_STRING(DIK_KANA);
    DIKB_KEYCODE_STRING(DIK_ABNT_C1);
    DIKB_KEYCODE_STRING(DIK_CONVERT);
    DIKB_KEYCODE_STRING(DIK_NOCONVERT);
    DIKB_KEYCODE_STRING(DIK_YEN);
    DIKB_KEYCODE_STRING(DIK_ABNT_C2);
    DIKB_KEYCODE_STRING(DIK_NUMPADEQUALS);
    DIKB_KEYCODE_STRING(DIK_PREVTRACK);
    DIKB_KEYCODE_STRING(DIK_AT);
    DIKB_KEYCODE_STRING(DIK_COLON);
    DIKB_KEYCODE_STRING(DIK_UNDERLINE);
    DIKB_KEYCODE_STRING(DIK_KANJI);
    DIKB_KEYCODE_STRING(DIK_STOP);
    DIKB_KEYCODE_STRING(DIK_AX);
    DIKB_KEYCODE_STRING(DIK_UNLABELED);
    DIKB_KEYCODE_STRING(DIK_NEXTTRACK);
    DIKB_KEYCODE_STRING(DIK_NUMPADENTER);
    DIKB_KEYCODE_STRING(DIK_RCONTROL);
    DIKB_KEYCODE_STRING(DIK_MUTE);
    DIKB_KEYCODE_STRING(DIK_CALCULATOR);
    DIKB_KEYCODE_STRING(DIK_PLAYPAUSE);
    DIKB_KEYCODE_STRING(DIK_MEDIASTOP);
    DIKB_KEYCODE_STRING(DIK_VOLUMEDOWN);
    DIKB_KEYCODE_STRING(DIK_VOLUMEUP);
    DIKB_KEYCODE_STRING(DIK_WEBHOME);
    DIKB_KEYCODE_STRING(DIK_NUMPADCOMMA);
    DIKB_KEYCODE_STRING(DIK_DIVIDE);
    DIKB_KEYCODE_STRING(DIK_SYSRQ);
    DIKB_KEYCODE_STRING(DIK_RMENU);
    DIKB_KEYCODE_STRING(DIK_PAUSE);
    DIKB_KEYCODE_STRING(DIK_HOME);
    DIKB_KEYCODE_STRING(DIK_UP);
    DIKB_KEYCODE_STRING(DIK_PRIOR);
    DIKB_KEYCODE_STRING(DIK_LEFT);
    DIKB_KEYCODE_STRING(DIK_RIGHT);
    DIKB_KEYCODE_STRING(DIK_END);
    DIKB_KEYCODE_STRING(DIK_DOWN);
    DIKB_KEYCODE_STRING(DIK_NEXT);
    DIKB_KEYCODE_STRING(DIK_INSERT);
    DIKB_KEYCODE_STRING(DIK_DELETE);
    DIKB_KEYCODE_STRING(DIK_LWIN);
    DIKB_KEYCODE_STRING(DIK_RWIN);
    DIKB_KEYCODE_STRING(DIK_APPS);
    DIKB_KEYCODE_STRING(DIK_POWER);
    DIKB_KEYCODE_STRING(DIK_SLEEP);
    DIKB_KEYCODE_STRING(DIK_WAKE);
    DIKB_KEYCODE_STRING(DIK_WEBSEARCH);
    DIKB_KEYCODE_STRING(DIK_WEBFAVORITES);
    DIKB_KEYCODE_STRING(DIK_WEBREFRESH);
    DIKB_KEYCODE_STRING(DIK_WEBSTOP);
    DIKB_KEYCODE_STRING(DIK_WEBFORWARD);
    DIKB_KEYCODE_STRING(DIK_WEBBACK);
    DIKB_KEYCODE_STRING(DIK_MYCOMPUTER);
    DIKB_KEYCODE_STRING(DIK_MAIL);
    DIKB_KEYCODE_STRING(DIK_MEDIASELECT);
    }
    return "*** UNKNOWN ***";
}
//---------------------------------------------------------------------------
