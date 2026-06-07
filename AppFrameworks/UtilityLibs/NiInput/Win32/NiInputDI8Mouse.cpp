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

#include "NiInputDI8Mouse.h"
#include "NiDI8InputSystem.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiInputDI8Mouse, NiInputMouse);
//---------------------------------------------------------------------------
NiInputDI8Mouse::NiInputDI8Mouse(NiInputDevice::Description* pkDescription,
    LPDIRECTINPUTDEVICE8 pkDIDevice, unsigned int uiUsage) :
    NiInputMouse(pkDescription),
    m_uiBufferSize(64), 
    m_pkDIDevice(pkDIDevice)
{
    if (m_pkDIDevice)
    {
        // Need to set the mouse data format and the cooperative mode.
        HRESULT hr = m_pkDIDevice->SetDataFormat(&c_dfDIMouse2);
        if (FAILED(hr))
        {
            m_eLastError = NIIERR_DEVICEINITFAIL;
        }
        else
        {
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
                m_eLastError = NIIERR_DEVICEINITFAIL;
        }

        // We need to prep the mouse for buffered input.
        if (SUCCEEDED(hr))
        {
            DIPROPDWORD kDIPropDW;
            kDIPropDW.diph.dwSize = sizeof(DIPROPDWORD);
            kDIPropDW.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            kDIPropDW.diph.dwObj = 0;
            kDIPropDW.diph.dwHow = DIPH_DEVICE;
            kDIPropDW.dwData = m_uiBufferSize;

            hr = m_pkDIDevice->SetProperty(DIPROP_BUFFERSIZE, 
                &kDIPropDW.diph);
            if (FAILED(hr))
                m_eLastError = NIIERR_DEVICEINITFAIL;
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pkDIDevice->Acquire();
            if (FAILED(hr))
            {
                m_eLastError = NIIERR_DEVICELOST;
            }
        }

    }
}
//---------------------------------------------------------------------------
NiInputDI8Mouse::~NiInputDI8Mouse()
{
    if (m_pkDIDevice)
    {
        m_pkDIDevice->Unacquire();
        m_pkDIDevice->Release();
    }
    m_pkDIDevice = 0;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8Mouse::UpdateDevice()
{
    NiInputMouse::UpdateDevice();

    if (!m_pkDIDevice)
    {
        m_eLastError = NIIERR_INVALIDDEVICE;
        return m_eLastError;
    }

    // Need to update the device.
    int iX = 0;
    int iY = 0;
    int iZ = 0;

    DWORD dwElements;
    HRESULT hr;
    DIDEVICEOBJECTDATA kDIDOD;

    m_uiButtonTouchMask = 0;

    for( ; ; )
    {
        HandleInsertion();
        dwElements = 1;
        hr = m_pkDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), 
            &kDIDOD, &dwElements, 0);
        if (FAILED(hr))
        {
            if (hr == DI_BUFFEROVERFLOW)
            {
                NILOG("Mouse> BUFFER OVERFLOW!\n");
            }

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
            break;
        }

        if (dwElements == 0)
        {
            m_eLastError = NIIERR_OK;
            break;
        }

        DWORD dwOfs = kDIDOD.dwOfs;
        switch (dwOfs)
        {
        case DIMOFS_BUTTON0:
        case DIMOFS_BUTTON1:
        case DIMOFS_BUTTON2:
        case DIMOFS_BUTTON3:
        case DIMOFS_BUTTON4:
        case DIMOFS_BUTTON5:
        case DIMOFS_BUTTON6:
        case DIMOFS_BUTTON7:
            {
                unsigned int uiBtnOffset = dwOfs - DIMOFS_BUTTON0;
                UpdateImmediateData(kDIDOD.dwData, uiBtnOffset);
            }
            break;
            // For position changes, we will accumulate all of the ones
            // contained in the buffer, and set them once we are done
            // processing it.
        case DIMOFS_X:
            iX += (int)(kDIDOD.dwData);
            break;
        case DIMOFS_Y:
            iY += (int)(kDIDOD.dwData);
            break;
        case DIMOFS_Z:
            iZ += (int)(kDIDOD.dwData);
            break;
        }
    }

    UpdateImmediatePositionData(iX, iY, iZ);

    // Touch the controls that weren't...
    // The positions are touchs in the UpdateImmediatePositionData call 
    // above, so we don't have to worry about them.
    if (m_uiButtonTouchMask)
    {
        for (unsigned int ui = 0; ui < NIM_NUM_BUTTONS; ui++)
        {
            if ((m_uiButtonTouchMask & (1 << ui)) == 0)
                m_akButtons[ui].TouchValue();
        }
    }
    else
    {
        for (unsigned int ui = 0; ui < NIM_NUM_BUTTONS; ui++)
            m_akButtons[ui].TouchValue();
    }

    FlushBuffer();
    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8Mouse::HandleRemoval()
{
    m_pkDIDevice->Unacquire();
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8Mouse::HandleInsertion()
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
        m_eLastError = NIIERR_OK;
    }
    return m_eLastError;
}
//---------------------------------------------------------------------------
void NiInputDI8Mouse::FlushBuffer()
{
    DWORD dwItems = INFINITE; 
    HRESULT hrFlush = IDirectInputDevice8_GetDeviceData(m_pkDIDevice, 
        sizeof(DIDEVICEOBJECTDATA), NULL, &dwItems, 0); 
    if (SUCCEEDED(hrFlush) && dwItems)
    {
    }
}
//---------------------------------------------------------------------------
void NiInputDI8Mouse::UpdateImmediateData(DWORD dwData, 
    unsigned int uiBtnOffset)
{
    m_uiButtonTouchMask |= (1 << uiBtnOffset);

    if (dwData & 0x80)
        RecordButtonPress((Button)(NIM_LEFT + uiBtnOffset));
    else
        RecordButtonRelease((Button)(NIM_LEFT + uiBtnOffset));
}
//---------------------------------------------------------------------------
void NiInputDI8Mouse::UpdateImmediatePositionData(int iX, int iY, int iZ)
{
    RecordPositionChange(0, iX);
    RecordPositionChange(1, iY);
    RecordPositionChange(2, iZ);
}
//---------------------------------------------------------------------------
