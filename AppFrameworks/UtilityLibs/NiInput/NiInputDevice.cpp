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

#include "NiAction.h"
#include "NiInputDevice.h"
#include "NiInputSystem.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiInputDevice);
//---------------------------------------------------------------------------
unsigned int NiInputDevice::Description::AddControlDesc(
    NiInputDevice::ControlDesc* pkDesc)
{
    // Check to see if the control description already exists...
    // This should never happen, but since it depends on the underlying
    // API to ensure that, we will do the check just to be sure.
    NiTListIterator kControlsIter;

    kControlsIter = m_kControls.GetHeadPos();
    while (kControlsIter)
    {
        const ControlDesc* pkCheckDesc = m_kControls.GetNext(kControlsIter);
        if (pkCheckDesc)
        {
            const ControlDesc* pkConstDesc = pkDesc;
            // Check each parameter of the descriptions
            // This would be simpler if we used an operator= overload, but 
            // as this is likely the only place we will use it, it really
            // doesn't matter.
            if ((!strcmp(pkConstDesc->GetName(), pkCheckDesc->GetName())) &&
                (pkConstDesc->GetType() == pkCheckDesc->GetType()) &&
                (pkConstDesc->GetFlags()== pkCheckDesc->GetFlags()))
            {
                // The control description is already contained in the
                // list. Inform the caller by returning 0xffffffff
                return 0xffffffff;
            }
        }
    }

    if (pkDesc->GetType() == NiInputDevice::ControlDesc::BUTTON)
        m_uiButtonCount++;
    if (pkDesc->GetType() == NiInputDevice::ControlDesc::AXIS)
        m_uiAxisCount++;
    if (pkDesc->GetType() == NiInputDevice::ControlDesc::POV)
        m_uiPOVCount++;

    m_kControls.AddTail(pkDesc);
    return m_kControls.GetSize();
}
//---------------------------------------------------------------------------
NiInputDevice::NiInputDevice(NiInputDevice::Description* pkDescription)
{
    NIASSERT(pkDescription);
    m_eStatus = UNKNOWN;
    m_uiModifiers = 0;
    m_bActionMapped = false;
    m_pkMappedActions = 0;
    m_spDescription = pkDescription;
    pkDescription->SetDeviceOwned(true);
}
//---------------------------------------------------------------------------
NiInputDevice::~NiInputDevice()
{
    if (m_pkMappedActions)
        m_pkMappedActions->RemoveAll();
    NiDelete m_pkMappedActions;
    NIASSERT(m_spDescription);
    m_spDescription->SetDeviceOwned(false);
    m_spDescription = 0;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::UpdateDevice()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::UpdateActionMappedDevice(NiInputSystem*)
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::HandleRemoval()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::HandleInsertion()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
void NiInputDevice::TouchAllInputValues()
{
    // This function should 'back-store' all the current input values.
    // This is to allow for quick-updating the current state of the device
    // when no input changes have occured.
}
//---------------------------------------------------------------------------
bool NiInputDevice::IsControlMapped(unsigned int, unsigned int, bool,
    unsigned int, bool, int, int)
{
    // Due to modifiers needing to be checked, specific devices must fill
    // in this function themselves.
    return false;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::AddMappedAction(NiAction* pkAction)
{
    // Ensure that we have instance the local action map list
    if (!m_pkMappedActions)
    {
        m_pkMappedActions = NiNew NiTPointerList<NiAction*>;
        if (!m_pkMappedActions)
        {
            NIASSERT(m_pkMappedActions);
            m_eLastError = NIIERR_ALLOCFAILED;
            return m_eLastError;
        }
    }

    // Make sure it is not already in the list!
    // Add it to the list
    m_pkMappedActions->AddTail(pkAction);

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::RemoveMappedAction(NiAction* pkAction)
{
    if (!m_pkMappedActions || !pkAction)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return m_eLastError;
    }

    // Need to find the action in the list
    NiTListIterator kPos = m_pkMappedActions->FindPos(pkAction);
    m_pkMappedActions->Remove(pkAction);

    if (kPos)
        return NIIERR_OK;
    return NIIERR_INVALIDACTION;
}
//---------------------------------------------------------------------------
NiInputErr NiInputDevice::ClearMappedActions()
{
    if (!m_pkMappedActions)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return m_eLastError;
    }

    m_pkMappedActions->RemoveAll();

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
bool NiInputDevice::CheckActionDeviceIDAndModifiers(NiAction* pkAction,
    unsigned int uiModifiers)
{
    unsigned int uiDeviceID = NiInputSystem::MakeDeviceID(
        GetPort(), GetSlot());

    // Check for device-specific
    if (pkAction->GetFlags() & NiAction::DEVICE_SPECIFIC)
    {
        if (pkAction->GetDeviceID() != uiDeviceID)
            return false;
    }

    // Check for modifiers
    if (pkAction->GetFlags() & NiAction::USE_MODIFIERS)
    {
#if 0
        if (uiModifiers != GetModifiers())
#else
        // With the new system, we have to OR in the current button flag...
        // This is due to the immediate data being updated prior to 
        // updating the action mapping.
        unsigned int uiCurrMods = GetModifiers();
        if (((uiModifiers | (1 << pkAction->GetControl())) != uiCurrMods) &&
            (uiModifiers != GetModifiers()))
#endif
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// NiInputDevice::Axis
//---------------------------------------------------------------------------
void NiInputDevice::Axis::SetRawValue(int iValue, bool bInvert,
    float fDeadZone)
{
    // Back store
    m_iLastRawValue = m_iRawValue;
    m_iLastValue = m_iValue;

    m_iRawValue = iValue;

    if (bInvert)
    {
        iValue = m_iRawHigh + m_iRawLow - iValue;
    }

    float fRawFraction = (float) (iValue - m_iRawLow) / 
        (float) (m_iRawHigh - m_iRawLow);

    float fClampHi = 0.5f + 0.5f * fDeadZone;

    if (fRawFraction > fClampHi)
    {
        // Redistribute value in range [fClampHi .. 1.0f] into the range
        // [0.5f .. 1.0f].
        fRawFraction = 0.5f + 0.5f * (fRawFraction - fClampHi) /
            (1.0f - fClampHi);
    }
    else 
    {
        float fClampLo = 0.5f - 0.5f * fDeadZone;

        if (fRawFraction < fClampLo)
        {
            // Redistribute value in range [0.0f .. fClampLo] into the range
            // [0.0f .. 0.5f].
            fRawFraction = 0.5f * (fRawFraction / fClampLo);
        }
        else
        {
            // Snap value to dead-center.
            fRawFraction = 0.5f;
        }
    }
    
    int iTotalRange = m_iHigh - m_iLow;
    m_iValue = m_iLow + (int) (fRawFraction * iTotalRange);
}
//---------------------------------------------------------------------------

