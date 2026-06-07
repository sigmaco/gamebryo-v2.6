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

#include "NiInputGamePad.h"
#include "NiAction.h"
#include "NiInputSystem.h"

#include <NiMath.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiInputGamePad, NiInputDevice);
//---------------------------------------------------------------------------
unsigned int NiInputGamePad::ms_uiMaxButtonCount = 
    NiInputGamePad::NIGP_MAX_BUTTONS;
unsigned int NiInputGamePad::ms_uiMaxAxes = 8;

int NiInputGamePad::ms_iDefaultAnalogThreshold = 32;
float NiInputGamePad::ms_fDefaultAnalogDeadZone = 0.20f;

//---------------------------------------------------------------------------
NiInputGamePad::NiInputGamePad(NiInputDevice::Description* pkDescription,
    int iStickRangeLow, int iStickRangeHigh) :
    NiInputDevice(pkDescription),
    m_uiCurrentButtons(0), 
    m_uiNewButtons(0),
    m_pkButtons(0), 
    m_pkAxes(0), 
    m_piAxisThresholds(0), 
    m_uiStickInvertFlags(0)
{
    m_uiButtonCount = NIGP_NUMBUTTONS;
    m_pkButtons = NiNew NiInputDevice::Button[m_uiButtonCount];
    NIASSERT(m_pkButtons);

    m_uiAxisCount = ms_uiMaxAxes;
    m_pkAxes = NiNew NiInputDevice::Axis[m_uiAxisCount];
    NIASSERT(m_pkAxes);    

    for (unsigned int i = 0; i < m_uiAxisCount; i++)
        m_pkAxes[i].SetRange(iStickRangeLow, iStickRangeHigh);
}
//---------------------------------------------------------------------------
NiInputGamePad::~NiInputGamePad()
{
    NiFree(m_piAxisThresholds);
    NiDelete [] m_pkAxes;
    NiDelete [] m_pkButtons;
}
//---------------------------------------------------------------------------
NiInputErr NiInputGamePad::UpdateDevice()
{
    // Clear the 'new' buttons field
    m_uiNewButtons = 0;
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputGamePad::UpdateActionMappedDevice(
    NiInputSystem* pkInputSystem)
{
    // Update the immediate state of the game pad
    NiInputErr eErr = UpdateDevice();
    if (!m_pkMappedActions || m_pkMappedActions->IsEmpty())
        return eErr;

    if (eErr == NIIERR_OK)
    {
        if (!UpdateMappedButtons(pkInputSystem))
            eErr = NIIERR_GENERIC;
        if (!UpdateMappedSticks(pkInputSystem))
            eErr = NIIERR_GENERIC;
    }
    return eErr;
}
//---------------------------------------------------------------------------
NiInputErr NiInputGamePad::HandleRemoval()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputGamePad::HandleInsertion()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
const int NiInputGamePad::GetAxisValue(unsigned int uiAxis) const
{
    if (!m_pkAxes)
        return 0;

    if (uiAxis >= m_uiAxisCount)
        return 0;

    return m_pkAxes[uiAxis].GetValue();
}
//---------------------------------------------------------------------------
const int NiInputGamePad::GetStickAxisValue(unsigned int uiStickFlags) const
{
    unsigned int uiAxis = 0xffffffff;

    switch (uiStickFlags)
    {
    case NIGP_STICK_LEFT_H:     uiAxis = 0;     break;
    case NIGP_STICK_LEFT_V:     uiAxis = 1;     break;
    case NIGP_STICK_RIGHT_H:    uiAxis = 2;     break;
    case NIGP_STICK_RIGHT_V:    uiAxis = 3;     break;
    case NIGP_STICK_X1_H:       uiAxis = 4;     break;
    case NIGP_STICK_X1_V:       uiAxis = 5;     break;
    case NIGP_STICK_X2_H:       uiAxis = 6;     break;
    case NIGP_STICK_X2_V:       uiAxis = 7;     break;
    }

    if (uiAxis != 0xffffffff)
        return GetAxisValue(uiAxis);

    return 0;
}
//---------------------------------------------------------------------------
const void NiInputGamePad::GetStickValue(unsigned int uiStickFlags, 
    int& iHorz, int& iVert) const
{
    unsigned int uiAxisH = 0xffffffff;
    unsigned int uiAxisV = 0xffffffff;

    unsigned int uiStick = uiStickFlags & NIGP_STICK_MASK;
    switch (uiStick)
    {
    case NIGP_STICK_LEFT:   uiAxisH = 0;    uiAxisV = 1;    break;
    case NIGP_STICK_RIGHT:  uiAxisH = 2;    uiAxisV = 3;    break;
    case NIGP_STICK_X1:     uiAxisH = 4;    uiAxisV = 5;    break;
    case NIGP_STICK_X2:     uiAxisH = 6;    uiAxisV = 7;    break;
    }

    if ((uiAxisH != 0xffffffff) && (uiAxisV != 0xffffffff))
    {
        iHorz = GetAxisValue(uiAxisH);
        iVert = GetAxisValue(uiAxisV);
    }
    else
    {
        iHorz = 0;
        iVert = 0;
    }
}
//---------------------------------------------------------------------------
const bool NiInputGamePad::ButtonIsDown(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (!m_pkButtons)
        return false;

    if ((unsigned int)eButton >= m_uiButtonCount)
        return false;

    return m_pkButtons[eButton].IsPressed(uiModifiers);
}
//---------------------------------------------------------------------------
const bool NiInputGamePad::ButtonIsDown(Button eButton) const
{
    unsigned int uiModifiers;

    return ButtonIsDown(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
const bool NiInputGamePad::ButtonWasPressed(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (!m_pkButtons)
        return false;

    if ((unsigned int)eButton >= m_uiButtonCount)
        return false;

    return m_pkButtons[eButton].WasPressed(uiModifiers);
}
//---------------------------------------------------------------------------
const bool NiInputGamePad::ButtonWasPressed(Button eButton) const
{
    unsigned int uiModifiers;

    return ButtonWasPressed(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
const bool NiInputGamePad::ButtonWasReleased(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (!m_pkButtons)
        return false;

    if ((unsigned int)eButton >= m_uiButtonCount)
        return false;

    return m_pkButtons[eButton].WasReleased(uiModifiers);
}
//---------------------------------------------------------------------------
const bool NiInputGamePad::ButtonWasReleased(Button eButton) const
{
    unsigned int uiModifiers;

    return ButtonWasReleased(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
const unsigned char NiInputGamePad::ButtonState(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (!m_pkButtons)
        return 0x00;

    if ((unsigned int)eButton >= m_uiButtonCount)
        return 0x00;

    return m_pkButtons[eButton].GetValue(uiModifiers);
}
//---------------------------------------------------------------------------
const unsigned char NiInputGamePad::ButtonState(Button eButton) const
{
    unsigned int uiModifiers;

    return ButtonState(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
// Rumble
//---------------------------------------------------------------------------
void NiInputGamePad::SetRumbleValue(unsigned int uiMotor, float fValue,
    bool bCommit)
{
    unsigned int uiRange = GetRumbleRange(uiMotor);
    unsigned int uiValue;

    if (uiRange == 1)
    {
        // Special case 1-bit motors: Any positive value turns motor on.

        uiValue = (fValue > 0.0f);
    }
    else
    {
        // Add 1 to uiRange so that floating values are distributes evenly over
        // [0..uiRange]. That creates need to clamp, since 1.0 will map to
        // uiRange + 1.

        // Cast from float to int instead of unsigned int avoids software
        // conversion.

        uiValue = (int) (fValue * (uiRange + 1));
        uiValue = NiMin((int) uiValue, (int) uiRange);
    }

    SetRumbleValue(uiMotor, uiValue, bCommit);
}
//---------------------------------------------------------------------------
void NiInputGamePad::GetRumbleValue(unsigned int uiMotor, float& fValue) const
{
    unsigned int uiRange = GetRumbleRange(uiMotor);
    unsigned int uiValue;

    GetRumbleValue(uiMotor, uiValue);

    fValue = (float) uiValue / (float) uiRange;
}
//---------------------------------------------------------------------------
void NiInputGamePad::SetRawAxisValue(unsigned int uiAxis, int iValue)
{
    NIASSERT(m_pkAxes);
    NIASSERT(uiAxis < m_uiAxisCount);

    bool bInvert = ((m_uiStickInvertFlags & (1 << uiAxis)) != 0);

    m_pkAxes[uiAxis].SetRawValue(iValue, bInvert, ms_fDefaultAnalogDeadZone);
}
//---------------------------------------------------------------------------
void NiInputGamePad::RecordButtonPress(Button eButton, unsigned char ucValue)
{
    NIASSERT(m_pkButtons);

    if ((unsigned int)eButton < m_uiButtonCount)
    {
        unsigned int uiButtonMask = 1 << eButton;
        if (ucValue >= ms_iDefaultAnalogThreshold)
        {
            m_pkButtons[eButton].SetButtonPress(m_uiModifiers, true, 
                ucValue);
            AddModifiers(uiButtonMask);

            // Update the quick-reference button masks
            if ((m_uiCurrentButtons & uiButtonMask) == 0)
                m_uiNewButtons |= uiButtonMask;
            m_uiCurrentButtons |= uiButtonMask;
        }
        else
        {
            m_pkButtons[eButton].SetButtonPress(m_uiModifiers, false, 
                ucValue);
            RemoveModifiers(uiButtonMask);

            // Update the quick-reference button masks
            if (m_uiCurrentButtons & uiButtonMask)
                m_uiCurrentButtons &= ~uiButtonMask;
        }
    }
}
//---------------------------------------------------------------------------
void NiInputGamePad::UpdateAnalogButton(Button eButton, 
    unsigned char ucValue)
{
    NIASSERT(m_pkButtons);
    if ((unsigned int)eButton < m_uiButtonCount)
    {
        unsigned int uiModifiers = 0;
        bool bButtonIsDown = ButtonIsDown(eButton, uiModifiers);
        unsigned int uiButtonMask = 1 << eButton;
        if (ucValue >= ms_iDefaultAnalogThreshold)
        {
            if (bButtonIsDown)
            {
                // If the button was already pressed then then keep same 
                // modifier and only update the analog value.
                m_pkButtons[eButton].SetButtonPress(uiModifiers, true, 
                    ucValue);
            }
            else
            {
                // Record a new button press
                m_pkButtons[eButton].SetButtonPress(m_uiModifiers, true, 
                    ucValue);

                // Add the button to the current set of modifiers
                AddModifiers(uiButtonMask);

                // Update the quick-reference button masks
                if ((m_uiCurrentButtons & uiButtonMask) == 0)
                    m_uiNewButtons |= uiButtonMask;
                m_uiCurrentButtons |= uiButtonMask;

            }
        }
        else 
        {
            // Button is not pressed. Clear the pressed state
            // and the button's modifiers, but still set the analog value.
            m_pkButtons[eButton].SetButtonPress(0, false, ucValue);
            
            if (bButtonIsDown)
            {
                // Button was released. Remove the modifiers.
                RemoveModifiers(uiButtonMask);

                for (unsigned int ui = 0; ui < m_uiButtonCount; ui++)
                {
                    if ((Button)ui != eButton)
                        m_pkButtons[ui].RemoveModifiers(uiButtonMask);
                }

                // Update the quick-reference button masks
                if (m_uiCurrentButtons & uiButtonMask)
                    m_uiCurrentButtons &= ~uiButtonMask;
            }

        }
    }
}
//---------------------------------------------------------------------------
void NiInputGamePad::RecordButtonRelease(Button eButton)
{
    NIASSERT(m_pkButtons);

    if ((unsigned int)eButton < m_uiButtonCount)
    {
        unsigned int uiButtonMask = (1 << eButton);
        m_pkButtons[eButton].SetButtonRelease();
        RemoveModifiers(uiButtonMask);
        
        // Update the quick-reference button masks
        if (m_uiCurrentButtons & uiButtonMask)
            m_uiCurrentButtons &= ~uiButtonMask;

        for (unsigned int ui = 0; ui < m_uiButtonCount; ui++)
        {
            if ((Button)ui != eButton)
                m_pkButtons[ui].RemoveModifiers(uiButtonMask);
        }
    }
}
//---------------------------------------------------------------------------
void NiInputGamePad::TouchAllInputValues()
{
    unsigned int ui;

    for (ui = 0; ui < m_uiButtonCount; ui++)
        m_pkButtons[ui].TouchValue();

    for (ui = 0; ui < m_uiAxisCount; ui++)
        m_pkAxes[ui].TouchValue();
}
//---------------------------------------------------------------------------
bool NiInputGamePad::IsControlMapped(unsigned int uiControl, 
    unsigned int uiSemantic, bool bUseModifiers, unsigned int uiModifiers, 
    bool bUseRange, int iRangeLow, int iRangeHigh)
{
    // Run through the mapped actions, checking the control field versus the
    // given value. If found, return true, otherwise return false.
    if (m_pkMappedActions)
    {
        NiAction* pkAction;
        NiTListIterator kPos = m_pkMappedActions->GetHeadPos();
        while (kPos)
        {
            pkAction = (NiAction*)(m_pkMappedActions->GetNext(kPos));
            if (pkAction && 
                (SEMANTIC_TYPE(pkAction->GetSemantic()) == 
                    SEMANTIC_TYPE(uiSemantic)) &&
                (pkAction->GetControl() == uiControl))
            {
                if (bUseModifiers)
                {
                    if (pkAction->GetPadModifiers() != uiModifiers)
                        return false;
                }
                if (bUseRange)
                {
                    int iCheckLow;
                    int iCheckHigh;

                    pkAction->GetRange(iCheckLow, iCheckHigh);

                    if ((iCheckLow >= iRangeLow) &&
                        (iCheckHigh <= iRangeLow))
                    {
                        // The low range is in the range of the action
                        return false;
                    }
                    if ((iCheckLow >= iRangeHigh) &&
                        (iCheckHigh <= iRangeHigh))
                    {
                        // The high range is in the range of the action
                        return false;
                    }
                }
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiInputGamePad::UpdateMappedButtons(NiInputSystem* pkInputSystem)
{
    NiAction* pkAction;
    NiTListIterator kPos;
    // Check each action
    kPos = m_pkMappedActions->GetHeadPos();
    while (kPos)
    {
        pkAction = m_pkMappedActions->GetNext(kPos);
        if (pkAction)
        {
            bool bSkip = false;
            unsigned int uiFlags = pkAction->GetFlags();
            unsigned int uiButton = pkAction->GetControl();
            unsigned int uiSemantic = pkAction->GetSemantic();

            // Make sure we are checking a button!
            if ((uiSemantic == NiAction::GP_AXIS_LEFT_H) || 
                (uiSemantic == NiAction::GP_AXIS_LEFT_V) || 
                (uiSemantic == NiAction::GP_AXIS_RIGHT_H) || 
                (uiSemantic == NiAction::GP_AXIS_RIGHT_V) || 
                (uiSemantic == NiAction::GP_AXIS_X1_H) || 
                (uiSemantic == NiAction::GP_AXIS_X1_V) || 
                (uiSemantic == NiAction::GP_AXIS_X2_H) || 
                (uiSemantic == NiAction::GP_AXIS_X2_V))
            {
                continue;
            }

            // Check for device-specific and modifiers
            bSkip = !(CheckActionDeviceIDAndModifiers(pkAction,
                pkAction->GetPadModifiers()));

            // Grab the current value, and the previous value
            unsigned int uiValue = (unsigned int)(
                m_pkButtons[uiButton].GetValue());
            unsigned int uiLastValue = (unsigned int)(
                m_pkButtons[uiButton].GetLastValue());

            // Grab the current and last pressed/released state
            // for digital
            unsigned int uiModifiers;
            bool bIsPressed = 
                m_pkButtons[uiButton].IsPressed(uiModifiers);
            bool bWasPressed = 
                m_pkButtons[uiButton].WasPressed(uiModifiers);
            bool bWasReleased = 
                m_pkButtons[uiButton].WasReleased(uiModifiers);

            if (!bSkip)
            {
                // Check for 'single-shot' values
                if ((uiFlags & NiAction::CONTINUOUS) == 0)
                {
                    if (uiFlags & NiAction::ANALOG)
                    {
                        if (uiValue == uiLastValue)
                            bSkip = true;
                    }
                    else
                    {
                        if (bIsPressed && !bWasPressed)
                            bSkip = true;
                        else
                        if (!bIsPressed && !bWasReleased)
                            bSkip = true;
                    }
                }
            }

            if (!bSkip)
            {
                // Check for range values
                if (uiFlags & NiAction::GAMEBRYORANGE)
                {
                    // Since analog buttons return 0-255, we can
                    // use unsigned int for value.
                    int iLow, iHigh;
                    bool bInRange = false;
                    bool bLastInRange = false;

                    pkAction->GetRange(iLow, iHigh);

                    int iValue = (int)uiValue;
                    if ((iValue < iLow) || (iValue > iHigh))
                        uiValue = 0;
                    else
                        bInRange = true;

                    int iLastValue = (int)uiLastValue;
                    if ((iLastValue < iLow) || (iLastValue > iHigh))
                        uiLastValue = 0;
                    else
                        bLastInRange = true;

                    if ((uiFlags & NiAction::CONTINUOUS) == 0)
                    {
                        if (bInRange == bLastInRange)
                            bSkip = true;
                    }
                }
            }

            if (!bSkip)
            {
                // Check for boolean return cases
                if (uiFlags & NiAction::RETURN_BOOLEAN)
                {
                    if (bIsPressed)
                        uiValue = 1;
                    else
                        uiValue = 0;
                }
            }

            if (!bSkip)
            {
                pkInputSystem->ReportAction(this, 
                    pkAction->GetAppData(), 0, uiValue, 
                    pkAction->GetContext());
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiInputGamePad::UpdateMappedSticks(NiInputSystem* pkInputSystem)
{
    NiAction* pkAction;
    NiTListIterator kPos;
    // Check each action
    kPos = m_pkMappedActions->GetHeadPos();
    while (kPos)
    {
        pkAction = m_pkMappedActions->GetNext(kPos);
        if (pkAction)
        {
            bool bSkip = true;
            unsigned int uiFlags = pkAction->GetFlags();
            unsigned int uiStick = pkAction->GetControl();
            unsigned int uiSemantic = pkAction->GetSemantic();
            int iValue = 0;
            int iLastValue = 0;

            // Make sure we are checking a button!
            if ((uiSemantic == NiAction::GP_AXIS_LEFT_H) || 
                (uiSemantic == NiAction::GP_AXIS_LEFT_V) || 
                (uiSemantic == NiAction::GP_AXIS_RIGHT_H) || 
                (uiSemantic == NiAction::GP_AXIS_RIGHT_V) || 
                (uiSemantic == NiAction::GP_AXIS_X1_H) || 
                (uiSemantic == NiAction::GP_AXIS_X1_V) || 
                (uiSemantic == NiAction::GP_AXIS_X2_H) || 
                (uiSemantic == NiAction::GP_AXIS_X2_V))
            {
                bSkip = false;
            }

            if (!bSkip)
            {
                // Check for device-specific and modifiers
                bSkip = !(CheckActionDeviceIDAndModifiers(pkAction,
                    pkAction->GetPadModifiers()));
            }

            if (!bSkip)
            {
                switch (uiStick)
                {
                case NIGP_STICK_LEFT_H:
                    iValue = m_pkAxes[NIGP_DEFAULT_LEFT_HORZ].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_LEFT_HORZ].GetLastValue();
                    break;
                case NIGP_STICK_LEFT_V:
                    iValue = m_pkAxes[NIGP_DEFAULT_LEFT_VERT].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_LEFT_VERT].GetLastValue();
                    break;
                case NIGP_STICK_RIGHT_H:
                    iValue = m_pkAxes[NIGP_DEFAULT_RIGHT_HORZ].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_RIGHT_HORZ].GetLastValue();
                    break;
                case NIGP_STICK_RIGHT_V:
                    iValue = m_pkAxes[NIGP_DEFAULT_RIGHT_VERT].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_RIGHT_VERT].GetLastValue();
                    break;
                case NIGP_STICK_X1_H:
                    iValue = m_pkAxes[NIGP_DEFAULT_X1_HORZ].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_X1_HORZ].GetLastValue();
                    break;
                case NIGP_STICK_X1_V:
                    iValue = m_pkAxes[NIGP_DEFAULT_X1_VERT].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_X1_VERT].GetLastValue();
                    break;
                case NIGP_STICK_X2_H:
                    iValue = m_pkAxes[NIGP_DEFAULT_X2_HORZ].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_X2_HORZ].GetLastValue();
                    break;
                case NIGP_STICK_X2_V:
                    iValue = m_pkAxes[NIGP_DEFAULT_X2_VERT].GetValue();
                    iLastValue = 
                        m_pkAxes[NIGP_DEFAULT_X2_VERT].GetLastValue();
                    break;
                default:
                    bSkip = true;
                    break;
                }
            }

            if (!bSkip)
            {
                // Continuous check
                if ((uiFlags & NiAction::CONTINUOUS) == 0)
                {
                    if (iValue == iLastValue)
                        bSkip = true;
                }
            }

            unsigned int uiValue = (unsigned int)iValue;

            if (!bSkip)
            {
                if (uiFlags & NiAction::GAMEBRYORANGE)
                {
                    int iLow, iHigh;
                    bool bInRange = false;
                    bool bLastInRange = false;

                    pkAction->GetRange(iLow, iHigh);

                    if ((iValue >= iLow) && (iValue <= iHigh))
                        bInRange = true;
                    if ((iLastValue >= iLow) && (iLastValue <= iHigh))
                        bLastInRange = true;

                    if ((uiFlags & NiAction::CONTINUOUS) == 0)
                    {
                        if (bInRange == bLastInRange)
                            bSkip = true;
                    }

                    if (!bSkip && (uiFlags & NiAction::RETURN_BOOLEAN))
                    {
                        if (bInRange)
                            uiValue = 1;
                        else
                            uiValue = 0;
                    }
                }
                else
                if (uiFlags & NiAction::RETURN_BOOLEAN)
                {
                    // Pin the value
                    if (uiValue)
                        uiValue = 1;
                }
            }

            if (!bSkip)
            {
                pkInputSystem->ReportAction(this, 
                    pkAction->GetAppData(), 0, uiValue, 
                    pkAction->GetContext());
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
