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

#include "NiInputDI8GamePad.h"
#include "NiDI8InputSystem.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiInputDI8GamePad, NiInputGamePad);
//---------------------------------------------------------------------------
NiInputDI8GamePad::NiInputDI8GamePad(NiInputDevice::Description* pkDescription,
    LPDIRECTINPUTDEVICE8 pkDIDevice, int iStickRangeLow, int iStickRangeHigh) :
    NiInputGamePad(pkDescription, iStickRangeLow, iStickRangeHigh), 
    m_pkDIDevice(pkDIDevice)
{
    if (m_pkDIDevice)
    {
        HRESULT hr;

        // Setup the deadzone 
        DIPROPDWORD kDIPDW;
        kDIPDW.diph.dwSize = sizeof(DIPROPDWORD);
        kDIPDW.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        kDIPDW.diph.dwObj = 0;
        kDIPDW.diph.dwHow = DIPH_DEVICE;
        kDIPDW.dwData = 2500;   // 25% for now...
        hr = m_pkDIDevice->SetProperty(DIPROP_DEADZONE, &kDIPDW.diph);
        if (FAILED(hr))
        {
            NILOG(NIMESSAGE_GENERAL_1, "DI8GamePad> Warning - Set DeadZone "
                "failed!\n");
        }

        //
        if (SUCCEEDED(hr))
        {
            hr = m_pkDIDevice->Acquire();
            if (FAILED(hr))
            {
                m_eLastError = NIIERR_DEVICELOST;
            }
        }

        m_pkAxes[NIGP_DEFAULT_LEFT_HORZ].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_LEFT_VERT].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_RIGHT_HORZ].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_RIGHT_VERT].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_X1_HORZ].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_X1_VERT].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_X2_HORZ].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
        m_pkAxes[NIGP_DEFAULT_X2_VERT].SetRawRange(
            iStickRangeLow, iStickRangeHigh);
    }
}
//---------------------------------------------------------------------------
NiInputDI8GamePad::~NiInputDI8GamePad()
{
    if (m_pkDIDevice)
    {
        m_pkDIDevice->Unacquire();
        m_pkDIDevice->Release();
    }
    m_pkDIDevice = 0;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8GamePad::UpdateDevice()
{
    m_eLastError = NIIERR_OK;
    NiInputGamePad::UpdateDevice();

    if (!m_pkDIDevice)
    {
        NiOutputDebugString("GamePad::UpdateDevice> Invalid device!\n");
        m_eLastError = NIIERR_INVALIDDEVICE;
        return m_eLastError;
    }

    DWORD dwInOut = 17;
    DIDEVICEOBJECTDATA akDIDOD[17];

    HRESULT hr = m_pkDIDevice->Acquire();
    if (FAILED(hr))
    {
        // Acquite will return S_FALSE if the device was already acquired.
        if (hr != S_FALSE)
        {
            if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED) ||
                (hr == DIERR_UNPLUGGED))
            {
                SetStatus(NiInputDevice::REMOVED);
                m_eLastError = NIIERR_DEVICECHANGE;
            }
            else
            {
                m_eLastError = NIIERR_INVALIDDEVICE;
            }
            return m_eLastError;
        }
    }
    
    SetStatus(NiInputDevice::READY);

    hr = m_pkDIDevice->Poll();
    if (FAILED(hr))
    {
        if (hr != DI_NOEFFECT)
        {
            NiOutputDebugString("GamePad::UpdateDevice> Poll error!\n");
            m_eLastError = NIIERR_INVALIDDEVICE;
            return m_eLastError;
        }
    }
    hr = m_pkDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), akDIDOD,
        &dwInOut, 0);
    if (SUCCEEDED(hr))
    {
        if (dwInOut)
        {
            // Clear the touch mask
            m_uiButtonTouchMask = 0;
            m_uiAxisTouchMask = 0;

            unsigned int ui;

            // Update the Ni version of the gamepad
            for (ui = 0; ui < dwInOut; ui++)
            {
                UpdateImmediateData((unsigned int)akDIDOD[ui].uAppData, 
                    akDIDOD[ui].dwData);
            }

            // Touch the controls that weren't...
            for (ui = 0; ui < NIGP_NUMBUTTONS; ui++)
            {
                // 
                if ((m_uiButtonTouchMask & (1 << ui)) == 0)
                    m_pkButtons[ui].TouchValue();
            }

            // 4 axes.
            for (ui = 0; ui < 4; ui++)
            {
                if ((m_uiAxisTouchMask & (1 << ui)) == 0)
                    m_pkAxes[ui].TouchValue();
            }
        }
        else
        {
            // No data --> Update all values
            TouchAllInputValues();
        }
    }
    else
    {
        NiOutputDebugString("GamePad::UpdateDevice> GetDeviceData error!\n");
//        TouchAllInputValues();
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8GamePad::HandleRemoval()
{
    m_pkDIDevice->Unacquire();

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDI8GamePad::HandleInsertion()
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
void NiInputDI8GamePad::FlushBuffer()
{
}
//---------------------------------------------------------------------------
void NiInputDI8GamePad::UpdateImmediateData(unsigned int uiAppData, 
    DWORD dwData)
{
    int iValue = (int)dwData;

    switch (uiAppData)
    {
    case NiDI8InputSystem::AXIS_0_H:
        SetRawAxisValue(NIGP_DEFAULT_LEFT_HORZ, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_LEFT_HORZ);
        break;
    case NiDI8InputSystem::AXIS_0_V:
        SetRawAxisValue(NIGP_DEFAULT_LEFT_VERT, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_LEFT_VERT);
        break;
    case NiDI8InputSystem::AXIS_1_H:
        SetRawAxisValue(NIGP_DEFAULT_RIGHT_HORZ, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_RIGHT_HORZ);
        break;
    case NiDI8InputSystem::AXIS_1_V:
        SetRawAxisValue(NIGP_DEFAULT_RIGHT_VERT, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_RIGHT_VERT);
        break;
    case NiDI8InputSystem::AXIS_2_H:
        SetRawAxisValue(NIGP_DEFAULT_X1_HORZ, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_X1_HORZ);
        break;
    case NiDI8InputSystem::AXIS_2_V:
        SetRawAxisValue(NIGP_DEFAULT_X1_VERT, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_X1_VERT);
        break;
    case NiDI8InputSystem::AXIS_3_H:
        SetRawAxisValue(NIGP_DEFAULT_X2_HORZ, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_X2_HORZ);
        break;
    case NiDI8InputSystem::AXIS_3_V:
        SetRawAxisValue(NIGP_DEFAULT_X2_VERT, iValue);
        m_uiAxisTouchMask |= (1 << NIGP_DEFAULT_X2_VERT);
        break;
    case NiDI8InputSystem::POV_DPAD:
        {
            // We need to simulate the L* buttons...
            switch (iValue)
            {
            case -1:
                RecordButtonRelease(NIGP_LDOWN);
                RecordButtonRelease(NIGP_LLEFT);
                RecordButtonRelease(NIGP_LRIGHT);
                RecordButtonRelease(NIGP_LUP);
                break;
            case 0:
                RecordButtonRelease(NIGP_LDOWN);
                RecordButtonRelease(NIGP_LLEFT);
                RecordButtonRelease(NIGP_LRIGHT);
                RecordButtonPress(NIGP_LUP);
                break;
            case 4500:
                RecordButtonRelease(NIGP_LDOWN);
                RecordButtonRelease(NIGP_LLEFT);
                RecordButtonPress(NIGP_LUP);
                RecordButtonPress(NIGP_LRIGHT);
                break;
            case 9000:
                RecordButtonRelease(NIGP_LUP);
                RecordButtonRelease(NIGP_LDOWN);
                RecordButtonRelease(NIGP_LLEFT);
                RecordButtonPress(NIGP_LRIGHT);
                break;
            case 13500:
                RecordButtonRelease(NIGP_LUP);
                RecordButtonRelease(NIGP_LLEFT);
                RecordButtonPress(NIGP_LDOWN);
                RecordButtonPress(NIGP_LRIGHT);
                break;
            case 18000:
                RecordButtonRelease(NIGP_LUP);
                RecordButtonRelease(NIGP_LLEFT);
                RecordButtonRelease(NIGP_LRIGHT);
                RecordButtonPress(NIGP_LDOWN);
                break;
            case 22500:
                RecordButtonRelease(NIGP_LUP);
                RecordButtonRelease(NIGP_LRIGHT);
                RecordButtonPress(NIGP_LDOWN);
                RecordButtonPress(NIGP_LLEFT);
                break;
            case 27000:
                RecordButtonRelease(NIGP_LUP);
                RecordButtonRelease(NIGP_LDOWN);
                RecordButtonRelease(NIGP_LRIGHT);
                RecordButtonPress(NIGP_LLEFT);
                break;
            case 31500:
                RecordButtonRelease(NIGP_LDOWN);
                RecordButtonRelease(NIGP_LRIGHT);
                RecordButtonPress(NIGP_LUP);
                RecordButtonPress(NIGP_LLEFT);
                break;
            }
            m_uiButtonTouchMask |= (1 << NIGP_LDOWN) | (1 << NIGP_LRIGHT) | 
                (1 << NIGP_LUP) | (1 << NIGP_LLEFT);
        }
        break;
    case NiDI8InputSystem::BUTTON_RUP:
        if (dwData)
            RecordButtonPress(NIGP_RUP);
        else
            RecordButtonRelease(NIGP_RUP);
        m_uiButtonTouchMask |= (1 << NIGP_RUP);
        break;
    case NiDI8InputSystem::BUTTON_RDOWN:
        if (dwData)
            RecordButtonPress(NIGP_RDOWN);
        else
            RecordButtonRelease(NIGP_RDOWN);
        m_uiButtonTouchMask |= (1 << NIGP_RDOWN);
        break;
    case NiDI8InputSystem::BUTTON_RLEFT:
        if (dwData)
            RecordButtonPress(NIGP_RLEFT);
        else
            RecordButtonRelease(NIGP_RLEFT);
        m_uiButtonTouchMask |= (1 << NIGP_RLEFT);
        break;
    case NiDI8InputSystem::BUTTON_RRIGHT:
        if (dwData)
            RecordButtonPress(NIGP_RRIGHT);
        else
            RecordButtonRelease(NIGP_RRIGHT);
        m_uiButtonTouchMask |= (1 << NIGP_RRIGHT);
        break;
    case NiDI8InputSystem::BUTTON_L1:
        if (dwData)
            RecordButtonPress(NIGP_L1);
        else
            RecordButtonRelease(NIGP_L1);
        m_uiButtonTouchMask |= (1 << NIGP_L1);
        break;
    case NiDI8InputSystem::BUTTON_R1:
        if (dwData)
            RecordButtonPress(NIGP_R1);
        else
            RecordButtonRelease(NIGP_R1);
        m_uiButtonTouchMask |= (1 << NIGP_R1);
        break;
    case NiDI8InputSystem::BUTTON_START:
        if (dwData)
            RecordButtonPress(NIGP_START);
        else
            RecordButtonRelease(NIGP_START);
        m_uiButtonTouchMask |= (1 << NIGP_START);
        break;
    case NiDI8InputSystem::BUTTON_SELECT:
        if (dwData)
            RecordButtonPress(NIGP_SELECT);
        else
            RecordButtonRelease(NIGP_SELECT);
        m_uiButtonTouchMask |= (1 << NIGP_SELECT);
        break;
    case NiDI8InputSystem::BUTTON_L2:
        if (dwData)
            RecordButtonPress(NIGP_L2);
        else
            RecordButtonRelease(NIGP_L2);
        m_uiButtonTouchMask |= (1 << NIGP_L2);
        break;
    case NiDI8InputSystem::BUTTON_R2:
        if (dwData)
            RecordButtonPress(NIGP_R2);
        else
            RecordButtonRelease(NIGP_R2);
        m_uiButtonTouchMask |= (1 << NIGP_R2);
        break;
    case NiDI8InputSystem::BUTTON_A:
        if (dwData)
            RecordButtonPress(NIGP_A);
        else
            RecordButtonRelease(NIGP_A);
        m_uiButtonTouchMask |= (1 << NIGP_A);
        break;
    case NiDI8InputSystem::BUTTON_B:
        if (dwData)
            RecordButtonPress(NIGP_B);
        else
            RecordButtonRelease(NIGP_B);
        m_uiButtonTouchMask |= (1 << NIGP_B);
        break;
    }
}
//---------------------------------------------------------------------------
void NiInputDI8GamePad::UpdateActionData(NiInputSystem* pkInputSystem, 
    NiAction* pkAction, unsigned int uiControlID, DWORD dwData)
{
    if (pkAction->GetControl() != uiControlID)
        return;

    unsigned int uiValue = dwData;
    unsigned int uiFlags = pkAction->GetFlags();

    int iValue = (int)dwData;

    // We need to map the DPAD value properly here...
    switch (uiControlID)
    {
    case NIGP_LUP:
        switch (iValue)
        {
        case -1:        uiValue = 0x00;     break;
        case 0:         uiValue = 0x80;     break;
        case 4500:      uiValue = 0x80;     break;
        case 9000:      uiValue = 0x00;     break;
        case 13500:     uiValue = 0x00;     break;
        case 18000:     uiValue = 0x00;     break;
        case 22500:     uiValue = 0x00;     break;
        case 27000:     uiValue = 0x00;     break;
        case 31500:     uiValue = 0x80;     break;
        default:        uiValue = 0x00;     break;
        }
        break;
    case NIGP_LDOWN:
        switch (iValue)
        {
        case -1:        uiValue = 0x00;     break;
        case 0:         uiValue = 0x00;     break;
        case 4500:      uiValue = 0x00;     break;
        case 9000:      uiValue = 0x00;     break;
        case 13500:     uiValue = 0x80;     break;
        case 18000:     uiValue = 0x80;     break;
        case 22500:     uiValue = 0x80;     break;
        case 27000:     uiValue = 0x00;     break;
        case 31500:     uiValue = 0x00;     break;
        }
        break;
    case NIGP_LLEFT:
        switch (iValue)
        {
        case -1:        uiValue = 0x00;     break;
        case 0:         uiValue = 0x00;     break;
        case 4500:      uiValue = 0x00;     break;
        case 9000:      uiValue = 0x00;     break;
        case 13500:     uiValue = 0x00;     break;
        case 18000:     uiValue = 0x00;     break;
        case 22500:     uiValue = 0x80;     break;
        case 27000:     uiValue = 0x80;     break;
        case 31500:     uiValue = 0x80;     break;
        }
        break;
    case NIGP_LRIGHT:
        switch (iValue)
        {
        case -1:        uiValue = 0x00;     break;
        case 0:         uiValue = 0x00;     break;
        case 4500:      uiValue = 0x80;     break;
        case 9000:      uiValue = 0x80;     break;
        case 13500:     uiValue = 0x80;     break;
        case 18000:     uiValue = 0x00;     break;
        case 22500:     uiValue = 0x00;     break;
        case 27000:     uiValue = 0x00;     break;
        case 31500:     uiValue = 0x00;     break;
        }
        break;
    }

    // Handle inversion
    switch (uiControlID)
    {
    case NIGP_STICK_LEFT_H:
        if (m_uiStickInvertFlags & NIGP_INVERT_LEFT_H)
            iValue = -iValue;
        break;
    case NIGP_STICK_LEFT_V:
        if (m_uiStickInvertFlags & NIGP_INVERT_LEFT_V)
            iValue = -iValue;
        break;
    case NIGP_STICK_RIGHT_H:
        if (m_uiStickInvertFlags & NIGP_INVERT_RIGHT_H)
            iValue = -iValue;
        break;
    case NIGP_STICK_RIGHT_V:
        if (m_uiStickInvertFlags & NIGP_INVERT_RIGHT_V)
            iValue = -iValue;
        break;
    case NIGP_STICK_X1_H:
        if (m_uiStickInvertFlags & NIGP_INVERT_X1_H)
            iValue = -iValue;
        break;
    case NIGP_STICK_X1_V:
        if (m_uiStickInvertFlags & NIGP_INVERT_X1_V)
            iValue = -iValue;
        break;
    case NIGP_STICK_X2_H:
        if (m_uiStickInvertFlags & NIGP_INVERT_X2_H)
            iValue = -iValue;
        break;
    case NIGP_STICK_X2_V:
        if (m_uiStickInvertFlags & NIGP_INVERT_X2_V)
            iValue = -iValue;
        break;
    }

    // Check if there are looking for boolean data
    if (uiFlags & NiAction::RETURN_BOOLEAN)
    {
        switch (uiControlID)
        {
        case NIGP_STICK_LEFT_H:
        case NIGP_STICK_LEFT_V:
        case NIGP_STICK_RIGHT_H:
        case NIGP_STICK_RIGHT_V:
        case NIGP_STICK_X1_H:
        case NIGP_STICK_X1_V:
        case NIGP_STICK_X2_H:
        case NIGP_STICK_X2_V:
            uiValue = 1;
            break;
        default:
            if (uiValue != 0)
                uiValue = 1;
            break;
        }
    }
    else
    {
        uiValue = (unsigned int)iValue;
    }

    // Check if it is ranged
    if (uiFlags & NiAction::GAMEBRYORANGE)
    {
        int iLow, iHigh;
        pkAction->GetRange(iLow, iHigh);
        if ((iValue >= iLow) && (iValue <= iHigh))
        {
            pkInputSystem->ReportAction(this, pkAction->GetAppData(), 0, 
                uiValue, pkAction->GetContext());
        }
    }
    else
    {
        pkInputSystem->ReportAction(this, pkAction->GetAppData(), 0, 
            uiValue, pkAction->GetContext());
    }
}
//---------------------------------------------------------------------------
void NiInputDI8GamePad::MapDIToNiControlIDs(unsigned int uiAppData, 
    DWORD dwData, unsigned int& uiControlID1, unsigned int& uiControlID2,
    unsigned int& uiControlID3, unsigned int& uiControlID4)
{
    switch (uiAppData)
    {
    case NiDI8InputSystem::AXIS_0_H:
        uiControlID1 = NIGP_STICK_LEFT_H;
        break;
    case NiDI8InputSystem::AXIS_0_V:
        uiControlID1 = NIGP_STICK_LEFT_V;
        break;
    case NiDI8InputSystem::AXIS_1_H:
        uiControlID1 = NIGP_STICK_RIGHT_H;
        break;
    case NiDI8InputSystem::AXIS_1_V:
        uiControlID1 = NIGP_STICK_RIGHT_V;
        break;
    case NiDI8InputSystem::AXIS_2_H:
        uiControlID1 = NIGP_STICK_X1_H;
        break;
    case NiDI8InputSystem::AXIS_2_V:
        uiControlID1 = NIGP_STICK_X1_V;
        break;
    case NiDI8InputSystem::AXIS_3_H:
        uiControlID1 = NIGP_STICK_X2_H;
        break;
    case NiDI8InputSystem::AXIS_3_V:
        uiControlID1 = NIGP_STICK_X2_V;
        break;
    case NiDI8InputSystem::POV_DPAD:
        {
            // We need to simulate the L* buttons...
            switch ((int)(dwData))
            {
            case -1:
                // Need to report that the DPAD was released 
                // (ie, it is now centered)
                // Check to see if any of the directions are pressed.
                {
                    if (ButtonIsDown(NIGP_LUP))
                        uiControlID1 = NIGP_LUP;
                    if (ButtonIsDown(NIGP_LDOWN))
                        uiControlID2 = NIGP_LDOWN;
                    if (ButtonIsDown(NIGP_LLEFT))
                        uiControlID3 = NIGP_LLEFT;
                    if (ButtonIsDown(NIGP_LRIGHT))
                        uiControlID4 = NIGP_LRIGHT;
                }
                break;
            case 0:         // NIGP_LUP
                uiControlID1 = NiInputGamePad::NIGP_LUP;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LDOWN))
                    uiControlID2 = NIGP_LDOWN;
                if (ButtonIsDown(NIGP_LLEFT))
                    uiControlID3 = NIGP_LLEFT;
                if (ButtonIsDown(NIGP_LRIGHT))
                    uiControlID4 = NIGP_LRIGHT;
                break;
            case 4500:      // NIGP_LUP NIGP_LRIGHT
                uiControlID1 = NiInputGamePad::NIGP_LUP;
                uiControlID2 = NiInputGamePad::NIGP_LRIGHT;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LDOWN))
                    uiControlID3 = NIGP_LDOWN;
                if (ButtonIsDown(NIGP_LLEFT))
                    uiControlID4 = NIGP_LLEFT;
                break;
            case 9000:      // NIGP_LRIGHT
                uiControlID1 = NiInputGamePad::NIGP_LRIGHT;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LUP))
                    uiControlID2 = NIGP_LUP;
                if (ButtonIsDown(NIGP_LDOWN))
                    uiControlID3 = NIGP_LDOWN;
                if (ButtonIsDown(NIGP_LLEFT))
                    uiControlID4 = NIGP_LLEFT;
                break;
            case 13500:     // NIGP_LDOWN NIGP_LRIGHT
                uiControlID1 = NiInputGamePad::NIGP_LDOWN;
                uiControlID2 = NiInputGamePad::NIGP_LRIGHT;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LUP))
                    uiControlID3 = NIGP_LUP;
                if (ButtonIsDown(NIGP_LLEFT))
                    uiControlID4 = NIGP_LLEFT;
                break;
            case 18000:     // NIGP_LDOWN
                uiControlID1 = NiInputGamePad::NIGP_LDOWN;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LUP))
                    uiControlID2 = NIGP_LUP;
                if (ButtonIsDown(NIGP_LLEFT))
                    uiControlID3 = NIGP_LLEFT;
                if (ButtonIsDown(NIGP_LRIGHT))
                    uiControlID4 = NIGP_LRIGHT;
                break;
            case 22500:     // NIGP_LDOWN NIGP_LLEFT
                uiControlID1 = NiInputGamePad::NIGP_LDOWN;
                uiControlID2 = NiInputGamePad::NIGP_LLEFT;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LUP))
                    uiControlID3 = NIGP_LUP;
                if (ButtonIsDown(NIGP_LRIGHT))
                    uiControlID4 = NIGP_LRIGHT;
                break;
            case 27000:     // NIGP_LLEFT
                uiControlID1 = NiInputGamePad::NIGP_LLEFT;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LUP))
                    uiControlID2 = NIGP_LUP;
                if (ButtonIsDown(NIGP_LDOWN))
                    uiControlID3 = NIGP_LDOWN;
                if (ButtonIsDown(NIGP_LRIGHT))
                    uiControlID4 = NIGP_LRIGHT;
                break;
            case 31500:     // NIGP_LUP NIGP_LLEFT
                uiControlID1 = NiInputGamePad::NIGP_LUP;
                uiControlID2 = NiInputGamePad::NIGP_LLEFT;
                // Need to 'turn off' the others
                if (ButtonIsDown(NIGP_LDOWN))
                    uiControlID3 = NIGP_LDOWN;
                if (ButtonIsDown(NIGP_LRIGHT))
                    uiControlID4 = NIGP_LRIGHT;
                break;
            }
        }
        break;
    case NiDI8InputSystem::BUTTON_RUP:
        uiControlID1 = NiInputGamePad::NIGP_RUP;
        break;
    case NiDI8InputSystem::BUTTON_RDOWN:
        uiControlID1 = NiInputGamePad::NIGP_RDOWN;
        break;
    case NiDI8InputSystem::BUTTON_RLEFT:
        uiControlID1 = NiInputGamePad::NIGP_RLEFT;
        break;
    case NiDI8InputSystem::BUTTON_RRIGHT:
        uiControlID1 = NiInputGamePad::NIGP_RRIGHT;
        break;
    case NiDI8InputSystem::BUTTON_L1:
        uiControlID1 = NiInputGamePad::NIGP_L1;
        break;
    case NiDI8InputSystem::BUTTON_R1:
        uiControlID1 = NiInputGamePad::NIGP_R1;
        break;
    case NiDI8InputSystem::BUTTON_START:
        uiControlID1 = NiInputGamePad::NIGP_START;
        break;
    case NiDI8InputSystem::BUTTON_SELECT:
        uiControlID1 = NiInputGamePad::NIGP_SELECT;
        break;
    case NiDI8InputSystem::BUTTON_L2:
        uiControlID1 = NiInputGamePad::NIGP_L2;
        break;
    case NiDI8InputSystem::BUTTON_R2:
        uiControlID1 = NiInputGamePad::NIGP_R2;
        break;
    case NiDI8InputSystem::BUTTON_A:
        uiControlID1 = NiInputGamePad::NIGP_A;
        break;
    case NiDI8InputSystem::BUTTON_B:
        uiControlID1 = NiInputGamePad::NIGP_B;
        break;
    }
}
//---------------------------------------------------------------------------
