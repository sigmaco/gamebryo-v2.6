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

#include "NiInputKeyboard.h"
#include "NiAction.h"
#include "NiInputSystem.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiInputKeyboard, NiInputDevice);
//---------------------------------------------------------------------------
NiInputKeyboard::NiInputKeyboard(NiInputDevice::Description* pkDescription) :
    NiInputDevice(pkDescription)
{
    m_bCapsLock = false;
    memset((void*)m_aucKeystate, 0, sizeof(unsigned char) * 32);
    memset((void*)m_aucKeystateLastUpdate, 0, sizeof(unsigned char) * 32);

    m_pkKeys = 0;
}
//---------------------------------------------------------------------------
NiInputKeyboard::~NiInputKeyboard()
{
    //** **//
}
//---------------------------------------------------------------------------
NiInputErr NiInputKeyboard::UpdateDevice()
{
    TouchAllInputValues();
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputKeyboard::UpdateActionMappedDevice(
    NiInputSystem* pkInputSystem)
{
    // Update the immediate state of the game pad
    NiInputErr eErr = UpdateDevice();
    if (eErr != NIIERR_OK)
        return eErr;

    if (!m_pkMappedActions || m_pkMappedActions->IsEmpty())
        return NIIERR_OK;

    // Check each action...
    NiAction* pkAction;
    NiTListIterator kPos = m_pkMappedActions->GetHeadPos();
    while (kPos)
    {
        pkAction = m_pkMappedActions->GetNext(kPos);
        if (!pkAction)
            continue;

        bool bSkip = false;
        unsigned int uiFlags = pkAction->GetFlags();

        if (!(CheckActionDeviceIDAndModifiers(pkAction, 
            pkAction->GetKeyModifiers())))
        {
            continue;
        }
        
        bool bExitLoop = false;
        for (unsigned int ui = 0; ui < KEY_TOTAL_COUNT && !bExitLoop; ui++)
        {
            if (pkAction->GetControl() == ui)
            {
                NiInputKeyboard::KeyCode eKeyCode = 
                    (NiInputKeyboard::KeyCode)ui;
                
                unsigned int uiValue = 0;
                if (KeyIsDown(eKeyCode))
                    uiValue = 0x000000ff;

                if (uiFlags & NiAction::RETURN_BOOLEAN)
                {
                    if (uiValue)
                        uiValue = 1;
                }

                if ((uiFlags & NiAction::CONTINUOUS) == 0)
                {
                    if ((KeyIsDown(eKeyCode) && !KeyWasPressed(eKeyCode)) ||
                        (!KeyIsDown(eKeyCode) && !KeyWasReleased(eKeyCode)))
                    {
                        bSkip = true;
                    }
                }

                if (!bSkip)
                {
                    pkInputSystem->ReportAction(this, 
                        pkAction->GetAppData(), 0, uiValue, 
                        pkAction->GetContext());
                }

                bExitLoop = true;
            }
        }
    }

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputKeyboard::HandleRemoval()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputKeyboard::HandleInsertion()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
void NiInputKeyboard::TouchAllInputValues()
{
    unsigned int uiSize = sizeof(unsigned char) * 32;
    NiMemcpy((void*)m_aucKeystateLastUpdate, (const void*)m_aucKeystate,
        uiSize);
}
//---------------------------------------------------------------------------
bool NiInputKeyboard::IsControlMapped(unsigned int uiControl, 
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
                    if (pkAction->GetKeyModifiers() == uiModifiers)
                        return true;
                }
                else if (!bUseModifiers && !bCheckUsingMods)
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
