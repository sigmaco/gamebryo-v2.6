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

#include "NiInputMouse.h"
#include "NiInputSystem.h"
#include "NiAction.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiInputMouse, NiInputDevice);
//---------------------------------------------------------------------------
NiInputMouse::NiInputMouse(NiInputDevice::Description* pkDescription) :
    NiInputDevice(pkDescription)
{
    for (unsigned int ui = 0; ui < NIM_AXIS_COUNT; ui++)
        m_akAxes[ui].SetValue(0);
}
//---------------------------------------------------------------------------
NiInputMouse::~NiInputMouse()
{
}
//---------------------------------------------------------------------------
NiInputErr NiInputMouse::UpdateDevice()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputMouse::UpdateActionMappedDevice(
    NiInputSystem* pkInputSystem)
{
    // Update the immediate state of the mouse
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
NiInputErr NiInputMouse::HandleRemoval()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputMouse::HandleInsertion()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
void NiInputMouse::TouchAllInputValues()
{
    unsigned int ui;

    // Axes
    for (ui = 0; ui < NIM_AXIS_COUNT; ui++)
        m_akAxes[ui].TouchValue();
    // Buttons
    for (ui = 0; ui < NIM_NUM_BUTTONS; ui++)
        m_akButtons[ui].TouchValue();
}
//---------------------------------------------------------------------------
bool NiInputMouse::IsControlMapped(unsigned int uiControl, 
    unsigned int uiSemantic, bool bUseModifiers, unsigned int uiModifiers, 
    bool, int, int)
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
                bool bCheckUsingMods = 
                    ((pkAction->GetFlags() & NiAction::USE_MODIFIERS) != 0);

                if (bUseModifiers && bCheckUsingMods)
                {
                    // Both use mods, and the mods are the same...
                    // They match
                    if (pkAction->GetMouseModifiers() == uiModifiers)
                        return true;
                }
                else
                if (!bUseModifiers && !bCheckUsingMods)
                {
                    // Action we are checking isn't using modifiers, 
                    // neither is request - so they match
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiInputMouse::UpdateMappedButtons(NiInputSystem* pkInputSystem)
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
            if ((uiSemantic == NiAction::MOUSE_AXIS_X) || 
                (uiSemantic == NiAction::MOUSE_AXIS_Y) || 
                (uiSemantic == NiAction::MOUSE_AXIS_Z) || 
                (uiSemantic == NiAction::MOUSE_AXIS_ANY_1) || 
                (uiSemantic == NiAction::MOUSE_AXIS_ANY_2) || 
                (uiSemantic == NiAction::MOUSE_AXIS_ANY_3))
            {
                continue;
            }

            // Check for device-specific and modifiers
            bSkip = !(CheckActionDeviceIDAndModifiers(pkAction,
                pkAction->GetMouseModifiers()));
            if (bSkip)
                continue;

            // Grab the current value
            unsigned int uiValue = (unsigned int)(
                m_akButtons[uiButton].GetValue());

            // Grab the current and last pressed/released state
            // for digital
            unsigned int uiModifiers;
            bool bIsPressed = 
                m_akButtons[uiButton].IsPressed(uiModifiers);
            bool bWasPressed = 
                m_akButtons[uiButton].WasPressed(uiModifiers);
            bool bWasReleased = 
                m_akButtons[uiButton].WasReleased(uiModifiers);

            // Check for 'single-shot' values
            if ((uiFlags & NiAction::CONTINUOUS) == 0)
            {
                if (bIsPressed && !bWasPressed)
                    continue;
                else
                if (!bIsPressed && !bWasReleased)
                    continue;
            }

            // For now, we are not supporting analog buttons on mice.
            // (I'm not sure if there are even any available at the
            // current time.) Therefore, we don't need any range
            // checking.

            // Check for boolean return cases
            if (uiFlags & NiAction::RETURN_BOOLEAN)
            {
                if (bIsPressed)
                    uiValue = 1;
                else
                    uiValue = 0;
            }
            pkInputSystem->ReportAction(this, 
                pkAction->GetAppData(), 0, uiValue, 
                pkAction->GetContext());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiInputMouse::UpdateMappedSticks(NiInputSystem* pkInputSystem)
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
            unsigned int uiSemantic = pkAction->GetSemantic();
            // Make sure we are checking a button!
            if ((uiSemantic == NiAction::MOUSE_BUTTON_LEFT) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_RIGHT) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_MIDDLE) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_X1) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_X2) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_X3) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_X4) || 
                (uiSemantic == NiAction::MOUSE_BUTTON_X5))
            {
                continue;
            }

            unsigned int uiFlags = pkAction->GetFlags();
            unsigned int uiAxis = pkAction->GetControl();
            int iValue = 0;
            int iLastValue = 0;

            // Check for device-specific and modifiers
            if (!(CheckActionDeviceIDAndModifiers(pkAction,
                pkAction->GetMouseModifiers())))
            {
                continue;
            }

            switch (uiAxis)
            {
            case NiInputMouse::NIM_AXIS_X:
                iValue = m_akAxes[0].GetValue();
                iLastValue = m_akAxes[0].GetLastValue();
                break;
            case NiInputMouse::NIM_AXIS_Y:
                iValue = m_akAxes[1].GetValue();
                iLastValue = m_akAxes[1].GetLastValue();
                break;
            case NiInputMouse::NIM_AXIS_Z:
                iValue = m_akAxes[2].GetValue();
                iLastValue = m_akAxes[2].GetLastValue();
                break;
            default:
                continue;
                break;
            }

            // Continuous check
            if ((uiFlags & NiAction::CONTINUOUS) == 0)
            {
                if (iValue == iLastValue)
                    continue;
            }

            unsigned int uiValue = (unsigned int)iValue;

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
                        continue;
                }

                if (uiFlags & NiAction::RETURN_BOOLEAN)
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

            pkInputSystem->ReportAction(this, 
                pkAction->GetAppData(), 0, uiValue, 
                pkAction->GetContext());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
