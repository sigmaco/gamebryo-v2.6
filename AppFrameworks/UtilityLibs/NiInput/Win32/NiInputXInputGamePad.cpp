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
#include "NiInputPCH.h"

#include "NiDI8InputSystem.h"
#include "NiInputXInputGamePad.h"
#include "NiAction.h"

NiImplementRTTI(NiInputXInputGamePad, NiInputGamePad);

//---------------------------------------------------------------------------
NiInputXInputGamePad::NiInputXInputGamePad(NiDI8InputSystem* pkInputSystem, 
    NiInputDevice::Description* pkDescription, int iStickRangeLow, 
    int iStickRangeHigh) : 
    NiInputGamePad(pkDescription, iStickRangeLow, iStickRangeHigh)
{
    m_pkInputSystem = pkInputSystem;
    m_dwLastStatePacketNumber = 0xffffffff;

    // Initialize the rumble settings.
    m_kVibration.wLeftMotorSpeed = 0;
    m_kVibration.wRightMotorSpeed = 0;

    m_ausActuatorMax[0] = 0xffff;
    m_ausActuatorMax[1] = 0xffff;
    m_ausActuatorData[0] = 0;
    m_ausActuatorData[1] = 0;

    // Set the raw ranges for the axes
    m_pkAxes[NIGP_DEFAULT_LEFT_HORZ].SetRawRange(-32*1024, 32*1024 - 1);
    m_pkAxes[NIGP_DEFAULT_LEFT_VERT].SetRawRange(-32*1024, 32*1024 - 1);
    m_pkAxes[NIGP_DEFAULT_RIGHT_HORZ].SetRawRange(-32*1024, 32*1024 - 1);
    m_pkAxes[NIGP_DEFAULT_RIGHT_VERT].SetRawRange(-32*1024, 32*1024 - 1);
}
//---------------------------------------------------------------------------
NiInputXInputGamePad::~NiInputXInputGamePad()
{
    /* */
}
//---------------------------------------------------------------------------
NiInputErr NiInputXInputGamePad::UpdateDevice()
{
    NiInputGamePad::UpdateDevice();

    NIASSERT(m_spDescription->GetPort() < 
        NiDI8InputSystem::MAX_XINPUT_GAMEPADS);
    XINPUT_STATE* pkInputState = m_pkInputSystem->GetXInputState(
        m_spDescription->GetPort(), 0);
    if (pkInputState)
    {
        if (UpdateDeviceState(*pkInputState))
        {
            if (m_eStatus == LOST)
            {
                SetStatus(REMOVED);
                return NIIERR_DEVICELOST;
            }
            return NIIERR_OK;
        }
    }

    return NIIERR_APIFAILURE;
}
//---------------------------------------------------------------------------
// Rumble
//---------------------------------------------------------------------------
unsigned int NiInputXInputGamePad::GetMotorCount() const
{
    return MAX_ACTUATORS;
}
//---------------------------------------------------------------------------
unsigned int NiInputXInputGamePad::GetRumbleRange(unsigned int uiMotor) const
{
    if (uiMotor >= MAX_ACTUATORS)
        return 0;
    return (unsigned int)m_ausActuatorMax[uiMotor];
}
//---------------------------------------------------------------------------
void NiInputXInputGamePad::SetRumbleValue(unsigned int uiMotor, 
    unsigned int uiValue, bool bCommit)
{
    if (uiMotor < MAX_ACTUATORS)
        m_ausActuatorData[uiMotor] = (unsigned short)uiValue;
    if (bCommit)
        StartRumble();
}
//---------------------------------------------------------------------------
void NiInputXInputGamePad::GetRumbleValue(unsigned int uiMotor, 
    unsigned int& uiValue) const
{
    if (uiMotor < MAX_ACTUATORS)
        uiValue = m_ausActuatorData[uiMotor];
    else
        uiValue = 0;
}
//---------------------------------------------------------------------------
void NiInputXInputGamePad::StartRumble()
{
    m_kVibration.wLeftMotorSpeed = m_ausActuatorData[0];
    m_kVibration.wRightMotorSpeed = m_ausActuatorData[1];
    NiDI8InputSystem::XInputSetState(m_spDescription->GetPort(), 
        &m_kVibration);
}
//---------------------------------------------------------------------------
void NiInputXInputGamePad::StopRumble(bool)
{
    m_ausActuatorData[0] = 0;
    m_ausActuatorData[1] = 0;

    StartRumble();
}
//---------------------------------------------------------------------------
bool NiInputXInputGamePad::UpdateDeviceState(XINPUT_STATE& kNewState)
{
    TouchAllInputValues();

    // If it's the same packet as the last update, don't bother!
    if (kNewState.dwPacketNumber == m_dwLastStatePacketNumber)
    {
        // Just quick-store all the current values.
        return true;
    }

    // Update the device according to the new state.
    // Analog buttons
    UpdateAnalogButton(NiInputGamePad::NIGP_L2, 
        kNewState.Gamepad.bLeftTrigger);
    UpdateAnalogButton(NiInputGamePad::NIGP_R2, 
        kNewState.Gamepad.bRightTrigger);
    // Digital buttons
    UpdateDigitalButtons(kNewState.Gamepad.wButtons);
    // Map the axis values...
    UpdateSticks(kNewState);

    m_dwLastStatePacketNumber = kNewState.dwPacketNumber;

    return true;
}
//---------------------------------------------------------------------------
bool NiInputXInputGamePad::UpdateDigitalButtons(WORD wButtons)
{
    bool bButtonState;

    // DPAD UP
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_LUP);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_DPAD_UP))
        RecordButtonRelease(NiInputGamePad::NIGP_LUP);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_DPAD_UP))
        RecordButtonPress(NiInputGamePad::NIGP_LUP);

    // DPAD DOWN
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_LDOWN);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
        RecordButtonRelease(NiInputGamePad::NIGP_LDOWN);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
        RecordButtonPress(NiInputGamePad::NIGP_LDOWN);

    // DPAD LEFT
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_LLEFT);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
        RecordButtonRelease(NiInputGamePad::NIGP_LLEFT);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
        RecordButtonPress(NiInputGamePad::NIGP_LLEFT);

    // DPAD RIGHT
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_LRIGHT);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
        RecordButtonRelease(NiInputGamePad::NIGP_LRIGHT);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
        RecordButtonPress(NiInputGamePad::NIGP_LRIGHT);

    // A BUTTON
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_RDOWN);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_A))
        RecordButtonRelease(NiInputGamePad::NIGP_RDOWN);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_A))
        RecordButtonPress(NiInputGamePad::NIGP_RDOWN);

    // B BUTTON
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_RRIGHT);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_B))
        RecordButtonRelease(NiInputGamePad::NIGP_RRIGHT);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_B))
        RecordButtonPress(NiInputGamePad::NIGP_RRIGHT);

    // X BUTTON
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_RLEFT);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_X))
        RecordButtonRelease(NiInputGamePad::NIGP_RLEFT);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_X))
        RecordButtonPress(NiInputGamePad::NIGP_RLEFT);

    // Y BUTTON
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_RUP);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_Y))
        RecordButtonRelease(NiInputGamePad::NIGP_RUP);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_Y))
        RecordButtonPress(NiInputGamePad::NIGP_RUP);    

    // LEFT SHOULDER BUTTON - WHITE ON ALPHA HARDWARE
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_L1);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
        RecordButtonRelease(NiInputGamePad::NIGP_L1);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
        RecordButtonPress(NiInputGamePad::NIGP_L1);    

    // RIGHT SHOULDER BUTTON - BLACK ON ALPHA HARDWARE
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_R1);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
        RecordButtonRelease(NiInputGamePad::NIGP_R1);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
        RecordButtonPress(NiInputGamePad::NIGP_R1);    

    // L2
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_A);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
        RecordButtonRelease(NiInputGamePad::NIGP_A);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
        RecordButtonPress(NiInputGamePad::NIGP_A);    

    // R2
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_B);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
        RecordButtonRelease(NiInputGamePad::NIGP_B);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
        RecordButtonPress(NiInputGamePad::NIGP_B);    

    // START
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_START);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_START))
        RecordButtonRelease(NiInputGamePad::NIGP_START);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_START))
        RecordButtonPress(NiInputGamePad::NIGP_START);    

    // SELECT
    bButtonState = ButtonIsDown(NiInputGamePad::NIGP_SELECT);
    if (bButtonState && !(wButtons & XINPUT_GAMEPAD_BACK))
        RecordButtonRelease(NiInputGamePad::NIGP_SELECT);
    else if (!bButtonState && (wButtons & XINPUT_GAMEPAD_BACK))
        RecordButtonPress(NiInputGamePad::NIGP_SELECT);    


    return true;
}
//---------------------------------------------------------------------------
bool NiInputXInputGamePad::UpdateSticks(XINPUT_STATE& kNewState)
{
    int iValue;

    // LEFT HORIZONTAL
    iValue = kNewState.Gamepad.sThumbLX;
    // Dead-zone
    if (abs(iValue) > (int)(ms_fDefaultAnalogDeadZone * 32*1024))
        SetRawAxisValue(NIGP_DEFAULT_LEFT_HORZ, iValue);
    else
        SetRawAxisValue(NIGP_DEFAULT_LEFT_HORZ, 0);

    // LEFT VERTICAL
    // Need to invert vertical mappings so that it matches the rest of 
    // the platforms. ie, Pushing UP is in the negative direction;
    // Pushing DOWN is in the positive
    iValue = -kNewState.Gamepad.sThumbLY;
    // Dead-zone
    if (abs(iValue) > (int)(ms_fDefaultAnalogDeadZone * 32*1024))
        SetRawAxisValue(NIGP_DEFAULT_LEFT_VERT, iValue);
    else
        SetRawAxisValue(NIGP_DEFAULT_LEFT_VERT, 0);

    // RIGHT HORIZONTAL
    iValue = kNewState.Gamepad.sThumbRX;
    // Dead-zone
    if (abs(iValue) > (int)(ms_fDefaultAnalogDeadZone * 32*1024))
        SetRawAxisValue(NIGP_DEFAULT_RIGHT_HORZ, iValue);
    else
        SetRawAxisValue(NIGP_DEFAULT_RIGHT_HORZ, 0);

    // RIGHT VERTICAL
    // Need to invert vertical mappings so that it matches the rest of 
    // the platforms. ie, Pushing UP is in the negative direction;
    // Pushing DOWN is in the positive
    iValue = -kNewState.Gamepad.sThumbRY;
    // Dead-zone
    if (abs(iValue) > (int)(ms_fDefaultAnalogDeadZone * 32*1024))
        SetRawAxisValue(NIGP_DEFAULT_RIGHT_VERT, iValue);
    else
        SetRawAxisValue(NIGP_DEFAULT_RIGHT_VERT, 0);

    return true;
}
//---------------------------------------------------------------------------
