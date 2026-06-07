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

#include "NiInputSystem.h"
#include "NiInputGamePad.h"
#include "NiInputKeyboard.h"
#include "NiInputMouse.h"

#include <NiSystem.h>

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiInputSystem);
//---------------------------------------------------------------------------
NiImplementRootRTTI(NiInputSystem::CreateParams);
//---------------------------------------------------------------------------
unsigned int NiInputSystem::ms_uiSequence = 0;
//---------------------------------------------------------------------------
// NiInputSystem
//---------------------------------------------------------------------------
NiInputSystem::NiInputSystem() :
    m_eLastError(NIIERR_OK), 
    m_eMode(IMMEDIATE), 
    m_uiKeyboard(0), 
    m_uiMouse(0), 
    m_uiGamePads(0), 
    m_iAxisRangeLow(-100), 
    m_iAxisRangeHigh(100), 
    m_bEnumerating(false), 
    m_spKeyboard(0), 
    m_spMouse(0), 
    m_spActionMap(0),
    m_pkActionDataPool(0)
{
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
        m_aspGamePads[ui] = 0;

    m_kActionDataList.RemoveAll();
    m_kAvailableDevices.RemoveAll();
}
//---------------------------------------------------------------------------
NiInputSystem::~NiInputSystem()
{
    m_spActionMap = 0;

    NiActionData* pkAD;
    NiTListIterator kPos = m_kActionDataList.GetHeadPos();
    while (kPos)
    {
        pkAD = (NiActionData*)(m_kActionDataList.GetNext(kPos));
        if (pkAD)
            ReleaseActionData(pkAD);
    }
    NiDelete m_pkActionDataPool;
    NiActionMap::ReleaseActionPool();
}
//---------------------------------------------------------------------------
NiInputDevice* NiInputSystem::OpenDevice(NiInputDevice::Type eType, 
    unsigned int uiPort, unsigned int uiSlot)
{
    NiInputDevice* pkDevice = 0;

    switch (eType)
    {
    case NiInputDevice::NIID_KEYBOARD:
        pkDevice = OpenKeyboard();
        break;
    case NiInputDevice::NIID_MOUSE:
        pkDevice = OpenMouse();
        break;
    case NiInputDevice::NIID_GAMEPAD:
        pkDevice = OpenGamePad(uiPort, uiSlot);
        break;
    default:
        m_eLastError = NIIERR_INVALIDDEVICE;
        break;
    }

    return pkDevice;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::CloseDevice(NiInputDevice* pkDevice)
{
    if (m_spKeyboard == pkDevice)
        m_spKeyboard = 0;
    else
    if (m_spMouse == pkDevice)
        m_spMouse = 0;
    else
    {
        for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
        {
            if (m_aspGamePads[ui] == pkDevice)
                m_aspGamePads[ui] = 0;
        }
    }

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputGamePad* NiInputSystem::OpenGamePad(unsigned int, unsigned int)
{
    m_eLastError = NIIERR_UNSUPPORTED;
    return 0;
}
//---------------------------------------------------------------------------
NiInputGamePad* NiInputSystem::OpenGamePad(
    NiInputDevice::Description* pkNiDesc)
{
    // 
    if (!pkNiDesc)
        return 0;

    return OpenGamePad(pkNiDesc->GetPort(), pkNiDesc->GetSlot());
}
//---------------------------------------------------------------------------
NiInputMouse* NiInputSystem::OpenMouse()
{
    m_eLastError = NIIERR_UNSUPPORTED;
    return 0;
}
//---------------------------------------------------------------------------
NiInputKeyboard* NiInputSystem::OpenKeyboard()
{
    m_eLastError = NIIERR_UNSUPPORTED;
    return 0;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::UpdateAllDevices()
{
    m_eLastError = NIIERR_OK;

    // 
    NiInputErr eErr_Keyboard;
    NiInputErr eErr_Mouse;
    NiInputErr eErr_Gamepad[MAX_GAMEPADS];
    bool bLostDevice = false;

    // Keyboard
    if (m_spKeyboard)
    {
        eErr_Keyboard = m_spKeyboard->UpdateDevice();
        if (eErr_Keyboard == NIIERR_DEVICELOST)
            bLostDevice = true;
        else
        {
            // Set the modifiers on the mouse
            UpdateMouseModifiers();
        }
    }
    // Mouse
    if (m_spMouse)
    {
        eErr_Mouse = m_spMouse->UpdateDevice();
        if (eErr_Mouse == NIIERR_DEVICELOST)
            bLostDevice = true;
    }
    // Gamepads
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
        {
            eErr_Gamepad[ui] = m_aspGamePads[ui]->UpdateDevice();
            if (eErr_Gamepad[ui] == NIIERR_DEVICELOST)
                bLostDevice = true;
        }
    }

    // See if any devices were lost
    if (bLostDevice)
        m_eLastError = NIIERR_DEVICELOST;

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::UpdateActionMap()
{
    m_eLastError = NIIERR_OK;

    // 
    NiInputErr eErr_Keyboard;
    NiInputErr eErr_Mouse;
    NiInputErr eErr_Gamepad[MAX_GAMEPADS];
    bool bLostDevice = false;

    // Keyboard
    if (m_spKeyboard)
    {
        eErr_Keyboard = m_spKeyboard->UpdateActionMappedDevice(this);
        if (eErr_Keyboard == NIIERR_DEVICELOST)
            bLostDevice = true;
        else
            UpdateMouseModifiers();
    }
    // Mouse
    if (m_spMouse)
    {
        eErr_Mouse = m_spMouse->UpdateActionMappedDevice(this);
        if (eErr_Mouse == NIIERR_DEVICELOST)
            bLostDevice = true;
    }
    // Gamepads
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
        {
            eErr_Gamepad[ui] = 
                m_aspGamePads[ui]->UpdateActionMappedDevice(this);
            if (eErr_Gamepad[ui] == NIIERR_DEVICELOST)
                bLostDevice = true;
        }
    }

    // See if any devices were lost
    if (bLostDevice)
        m_eLastError = NIIERR_DEVICELOST;

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiActionMap* NiInputSystem::GetActionMap()
{
    return m_spActionMap;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::SetActionMap(NiActionMap* pkActionMap)
{
    if (pkActionMap == m_spActionMap)
        return NIIERR_OK;

    // Set the new action map
    m_spActionMap = pkActionMap;
    if (m_spActionMap)
    {
        // There is a new one, so create the pool if needed
        m_eMode = MAPPED;
        if (!m_pkActionDataPool)
        {
            m_pkActionDataPool = NiNew NiTPool<NiActionData, 
                NiTNewInterface<NiActionData> >;
            NIASSERT(m_pkActionDataPool);
        }

        // Map the actions to the device
        MapNiActionsToDevices();
    }
    else
    {
        if (m_eMode == MAPPED)
        {
            FlushActionData();

            // Clear all the devices
            if (m_spKeyboard)
                m_spKeyboard->ClearMappedActions();
            if (m_spMouse)
                m_spMouse->ClearMappedActions();
            for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
            {
                if (m_aspGamePads[ui])
                    m_aspGamePads[ui]->ClearMappedActions();
            }

            NiDelete m_pkActionDataPool;
            m_pkActionDataPool = 0;
            m_eMode = IMMEDIATE;
        }
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::FlushActionData()
{
    NiActionData* pkActionData;
    m_kActionDataIter = m_kActionDataList.GetHeadPos();
    while (m_kActionDataIter)
    {
        pkActionData = m_kActionDataList.GetNext(m_kActionDataIter);
        if (pkActionData)
            ReleaseActionData(pkActionData);
    }
    m_kActionDataList.RemoveAll();

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::HandleDeviceChanges()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::HandleRemovals()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::HandleInsertions()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::Initialize(CreateParams* pkParams)
{
    if (pkParams == 0)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
    }
    else
    {
        m_uiKeyboard = pkParams->GetKeyboardUsage();
        m_uiMouse = pkParams->GetMouseUsage();
        m_uiGamePads = pkParams->GamePadCount();
        pkParams->GetAxisRange(m_iAxisRangeLow, m_iAxisRangeHigh);

        // Check for an action map!
        m_spActionMap = pkParams->GetActionMap();
        if (m_spActionMap)
        {
            m_eMode = MAPPED;
            m_pkActionDataPool = NiNew NiTPool<NiActionData, 
                NiTNewInterface<NiActionData> >;
            NIASSERT(m_pkActionDataPool);
        }
    }
    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::Shutdown()
{
    m_spKeyboard = 0;
    m_spMouse = 0;
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
        m_aspGamePads[ui] = 0;

    // Clear out the available device list.
    m_kAvailableDevices.RemoveAll();
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::ConfigureDevices()
{
    // Base implementation does nothing...
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
void NiInputSystem::SuspendInput()
{
    // Base implementation does nothing...
}
//---------------------------------------------------------------------------
void NiInputSystem::ResumeInput()
{
    // Base implementation does nothing...
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::ReportAction(NiInputDevice* pkDevice, 
    unsigned int uiAppData, unsigned int uiSequence, 
    unsigned int uiDeviceData, void* pvContext)
{
    // Look up the action
    NIASSERT(m_spActionMap);

    NiActionData* pkActionData = GetActionData();
    NIASSERT(pkActionData);

    pkActionData->SetAppData(uiAppData);
    pkActionData->SetDataValue(uiDeviceData);
    if (uiSequence)
        pkActionData->SetSequence(uiSequence);
    else
        pkActionData->SetSequence(GetNextAvailableSequence());
    pkActionData->SetDeviceID(pkDevice->GetPort(), pkDevice->GetSlot());
    pkActionData->SetContext(pvContext);

    m_kActionDataList.AddTail(pkActionData);

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
unsigned int NiInputSystem::GetNextAvailableSequence()
{
    return ms_uiSequence++;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::MapNiActionsToDevices()
{
    NiInputErr eKeyboardErr = MapNiActionsToKeyboard();
    NiInputErr eMouseErr = MapNiActionsToMouse();
    NiInputErr eGamePadErr = MapNiActionsToGamePads();

    if (eKeyboardErr)
        m_eLastError = NIIERR_MAPPINGFAILED_KEYBOARD;
    else if (eMouseErr)
        m_eLastError = NIIERR_MAPPINGFAILED_MOUSE;
    else if (eGamePadErr)
        m_eLastError = NIIERR_MAPPINGFAILED_GAMEPAD;
    else
        m_eLastError = NIIERR_OK;

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::MapNiActionsToKeyboard()
{
    if (!m_spKeyboard)
        return NIIERR_OK;

    if (m_spActionMap == 0)
        return NIIERR_NOTMAPPED;

    // Clear the currently mapped actions
    m_spKeyboard->ClearMappedActions();

    // It is ok to have an empty action map...
    unsigned int uiActionCount = m_spActionMap->GetActionCount();
    if (uiActionCount == 0)
        return NIIERR_OK;

    NiInputErr eErr;
    NiAction* pkAction;
    unsigned int ui;
    unsigned int uiSemantic;

    // Map all keyboard actions to the keyboard device
    for (ui = 0; ui < uiActionCount; ui++)
    {
        pkAction = m_spActionMap->GetAction(ui);
        if (!pkAction)
            continue;

        uiSemantic = pkAction->GetSemantic();
        // Only map it if it's a key mapping...
        if (IS_KEY_MAPPING(uiSemantic))
        {
            // See if it's a direct mapping
            if (!IS_KEY_ANY(uiSemantic))
            {
                // It is. No need to 'map it'
                // Make sure it is not already mapped
                bool bUseModifiers = ((pkAction->GetFlags() & 
                    NiAction::USE_MODIFIERS) != 0);

                if (m_spKeyboard->IsControlMapped(
                    NiAction::SemanticToDeviceControl(uiSemantic), 
                    uiSemantic, bUseModifiers, pkAction->GetKeyModifiers()))
                {
                    // This is a problem!
                    m_eLastError = NIIERR_MAPPING_TAKEN;
                }
                else
                {
                    // Add it to the device's map
                    eErr = m_spKeyboard->AddMappedAction(pkAction);
                    if (eErr != NIIERR_OK)
                    {
                        m_eLastError = eErr;
                        pkAction->SetHow(NiAction::APPNOMAP);
                    }
                    else
                    {
                        pkAction->SetHow(NiAction::APPMAPPED);
                        pkAction->SetControl(
                            NiAction::SemanticToDeviceControl(uiSemantic));
                    }
                }
            }
            else
            {
                // We need to find an available key to map it to
                NIASSERT(!"AnyKey mapping not support at this time!");
                pkAction->SetHow(NiAction::NIA_UNMAPPED);
            }
        }
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::MapNiActionsToMouse()
{
    if (!m_spMouse)
        return NIIERR_OK;

    if (m_spActionMap == 0)
        return NIIERR_NOTMAPPED;

    // Clear the currently mapped actions
    m_spMouse->ClearMappedActions();

    // It is ok to have an empty action map...
    unsigned int uiActionCount = m_spActionMap->GetActionCount();
    if (uiActionCount == 0)
        return NIIERR_OK;

    NiInputErr eErr;
    NiAction* pkAction;
    unsigned int ui;
    unsigned int uiSemantic;

    // Map all mouse actions to the mouse device
    for (ui = 0; ui < uiActionCount; ui++)
    {
        pkAction = m_spActionMap->GetAction(ui);
        if (!pkAction)
            continue;

        uiSemantic = pkAction->GetSemantic();
        // Only map it if it's a key mapping...
        if (IS_MOUSE_MAPPING(uiSemantic))
        {
            // Check for direct axis maps
            switch (uiSemantic)
            {
            case NiAction::MOUSE_AXIS_X:
            case NiAction::MOUSE_AXIS_Y:
            case NiAction::MOUSE_AXIS_Z:
            case NiAction::MOUSE_BUTTON_LEFT:
            case NiAction::MOUSE_BUTTON_RIGHT:
            case NiAction::MOUSE_BUTTON_MIDDLE:
            case NiAction::MOUSE_BUTTON_X1:
            case NiAction::MOUSE_BUTTON_X2:
            case NiAction::MOUSE_BUTTON_X3:
            case NiAction::MOUSE_BUTTON_X4:
            case NiAction::MOUSE_BUTTON_X5:
                {
                    // Make sure it is not already mapped
                    bool bUseModifiers = ((pkAction->GetFlags() & 
                        NiAction::USE_MODIFIERS) != 0);

                    if (m_spMouse->IsControlMapped(
                        NiAction::SemanticToDeviceControl(uiSemantic),
                        uiSemantic, bUseModifiers, 
                        pkAction->GetMouseModifiers()))
                    {
                        // This is a problem!
                        m_eLastError = NIIERR_MAPPING_TAKEN;
                    }
                    else
                    {
                        // Add it to the device's map
                        eErr = m_spMouse->AddMappedAction(pkAction);
                        if (eErr != NIIERR_OK)
                        {
                            m_eLastError = eErr;
                            pkAction->SetHow(NiAction::APPNOMAP);
                        }
                        else
                        {
                            pkAction->SetHow(NiAction::APPMAPPED);
                            pkAction->SetControl(
                                NiAction::SemanticToDeviceControl(
                                    uiSemantic));
                        }
                    }
                }
                break;
            default:
                {
                    if (IS_MOUSE_BUTTON_ANY(uiSemantic))
                    {
                        NIASSERT(!"MouseAnyButton mapping not supported "
                            "at this time!");
                        pkAction->SetHow(NiAction::NIA_UNMAPPED);
                    }
                    else
                    if ((IS_MOUSE_AXIS_ANY_1(uiSemantic)) ||
                        (IS_MOUSE_AXIS_ANY_2(uiSemantic)) ||
                        (IS_MOUSE_AXIS_ANY_3(uiSemantic)))
                    {
                        NIASSERT(!"MouseAnyAxis mapping not supported at "
                            "this time!");
                        pkAction->SetHow(NiAction::NIA_UNMAPPED);
                    }
                }
                break;
            }
        }
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::MapNiActionsToGamePads()
{
#if defined(_MAP_ALL_GPS_AT_ONCE_)
    if (m_spActionMap == 0)
        return NIIERR_NOTMAPPED;

    // Clear the currently mapped actions
    for (uj = 0; uj < MAX_GAMEPADS; uj++)
    {
        if (m_aspGamePads[uj])
            m_aspGamePads[uj]->ClearMappedActions();
    }

    // It is ok to have an empty action map...
    unsigned int uiActionCount = m_spActionMap->GetActionCount();
    if (uiActionCount == 0)
        return NIIERR_OK;

    NiInputErr eErr;
    NiAction* pkAction;
    unsigned int ui, uj;
    unsigned int uiSemantic;

    // We will just check the first pad we find for now when seeing if a 
    // mapping is valid. We should really do this per-gamepad.
    NiInputGamePad* pkGamePad = 0;
    for (uj = 0; uj < MAX_GAMEPADS; uj++)
    {
        if (m_aspGamePads[uj])
        {
            pkGamePad = m_aspGamePads[uj];
            break;
        }
    }
    if (!pkGamePad)
    {
        // No GamePads???
        return NIIERR_OK;
    }

    // Map all gamepad actions to each gamepad device
    for (ui = 0; ui < uiActionCount; ui++)
    {
        pkAction = m_spActionMap->GetAction(ui);
        if (!pkAction)
            continue;

        uiSemantic = pkAction->GetSemantic();
        // Only map it if it's a key mapping...
        if (IS_GP_MAPPING(uiSemantic))
        {
            // Check for direct axis maps
            switch (uiSemantic)
            {
                // Axis values
            case NiAction::GP_AXIS_LEFT_H:
            case NiAction::GP_AXIS_LEFT_V:
            case NiAction::GP_AXIS_RIGHT_H:
            case NiAction::GP_AXIS_RIGHT_V:
            case NiAction::GP_AXIS_X1_H:
            case NiAction::GP_AXIS_X1_V:
            case NiAction::GP_AXIS_X2_H:
            case NiAction::GP_AXIS_X2_V:
                {
                    // Make sure it is not already mapped
                    bool bUseModifiers = ((pkAction->GetFlags() & 
                        NiAction::USE_MODIFIERS) != 0);
                    bool bUseRange = ((pkAction->GetFlags() &
                        NiAction::GAMEBRYORANGE) != 0);

                    int iRangeLow, iRangeHigh;
                    pkAction->GetRange(iRangeLow, iRangeHigh);

                    if (pkGamePad->IsControlMapped(
                        NiAction::SemanticToDeviceControl(uiSemantic),
                        uiSemantic, bUseModifiers, 
                        pkAction->GetPadModifiers(), 
                        bUseRange, 
                        iRangeLow, iRangeHigh))
                    {
                        // This is a problem!
                        m_eLastError = NIIERR_MAPPING_TAKEN;
                    }
                    else
                    {
                        // Set the control
                        pkAction->SetControl(
                            NiAction::SemanticToDeviceControl(
                                uiSemantic));
                        pkAction->SetHow(NiAction::APPMAPPED);
                        // Add it to the device's map
                        for (uj = 0; uj < MAX_GAMEPADS; uj++)
                        {
                            if (m_aspGamePads[uj])
                            {
                                eErr = m_aspGamePads[uj]->AddMappedAction(
                                    pkAction);
                                if (eErr != NIIERR_OK)
                                    m_eLastError = eErr;
                            }
                        }
                    }
                }
                break;
                // Button values
            case NiAction::GP_BUTTON_LUP:
            case NiAction::GP_BUTTON_LDOWN:
            case NiAction::GP_BUTTON_LLEFT:
            case NiAction::GP_BUTTON_LRIGHT:
            case NiAction::GP_BUTTON_L1:
            case NiAction::GP_BUTTON_L2:
            case NiAction::GP_BUTTON_R1:
            case NiAction::GP_BUTTON_R2:
            case NiAction::GP_BUTTON_RUP:
            case NiAction::GP_BUTTON_RDOWN:
            case NiAction::GP_BUTTON_RLEFT:
            case NiAction::GP_BUTTON_RRIGHT:
            case NiAction::GP_BUTTON_A:
            case NiAction::GP_BUTTON_B:
            case NiAction::GP_BUTTON_START:
            case NiAction::GP_BUTTON_SELECT:
                {
                    pkAction->SetControl(
                        NiAction::SemanticToDeviceControl(uiSemantic));
                    for (uj = 0; uj < MAX_GAMEPADS; uj++)
                    {
                        if (m_aspGamePads[uj])
                        {
                            eErr = m_aspGamePads[uj]->AddMappedAction(
                                pkAction);
                            if (eErr != NIIERR_OK)
                                m_eLastError = eErr;
                        }
                    }
                }
                break;
                // DPad
/***
            case NiAction::GP_DPAD:
                {
                    for (uj = 0; uj < MAX_GAMEPADS; uj++)
                    {
                        if (m_aspGamePads[uj])
                        {
                            eErr = m_aspGamePads[uj]->AddMappedAction(
                                pkAction);
                            if (eErr != NIIERR_OK)
                                m_eLastError = eErr;
                        }
                    }
                }
                break;
***/
            default:
                {
                    // Any Axis
                    // Any Button
                    if (IS_GP_BUTTON_ANY(uiSemantic))
                    {
                        NIASSERT(!"GPAnyButton mapping not supported "
                            "at this time!");
                        pkAction->SetHow(NiAction::NIA_UNMAPPED);
                    }
                    else
                    if ((IS_GP_AXIS_ANY_1(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_2(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_3(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_4(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_5(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_6(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_7(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_8(uiSemantic)))
                    {
                        NIASSERT(!"GPAnyAxis mapping not supported at "
                            "this time!");
                        pkAction->SetHow(NiAction::NIA_UNMAPPED);
                    }
                }
                break;
            }
        }
    }

    return NIIERR_OK;
#else   //#if defined(_MAP_ALL_GPS_AT_ONCE_)
    NiInputErr eErr;
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
        {
            eErr = MapNiActionsToGamePad(m_aspGamePads[ui]);
            if (eErr != NIIERR_OK)
                m_eLastError = eErr;
        }
    }

    return m_eLastError;
#endif  //#if defined(_MAP_ALL_GPS_AT_ONCE_)
}
//---------------------------------------------------------------------------
NiInputErr NiInputSystem::MapNiActionsToGamePad(NiInputGamePad* pkGamePad)
{
    // Find the game pad...
    unsigned int ui;

    if (pkGamePad == 0)
        return NIIERR_INVALIDPARAMS;

    if (m_spActionMap == 0)
        return NIIERR_NOTMAPPED;

    pkGamePad->ClearMappedActions();

    // It is ok to have an empty action map...
    unsigned int uiActionCount = m_spActionMap->GetActionCount();
    if (uiActionCount == 0)
        return NIIERR_OK;

    NiInputErr eErr;
    NiAction* pkAction;
    unsigned int uiSemantic;

    // Map all keyboard actions to the keyboard device
    for (ui = 0; ui < uiActionCount; ui++)
    {
        pkAction = m_spActionMap->GetAction(ui);
        if (!pkAction)
            continue;

        uiSemantic = pkAction->GetSemantic();
        // Only map it if it's a key mapping...
        if (IS_GP_MAPPING(uiSemantic))
        {
            // Check for direct axis maps
            switch (uiSemantic)
            {
                // Axis values
            case NiAction::GP_AXIS_LEFT_H:
            case NiAction::GP_AXIS_LEFT_V:
            case NiAction::GP_AXIS_RIGHT_H:
            case NiAction::GP_AXIS_RIGHT_V:
            case NiAction::GP_AXIS_X1_H:
            case NiAction::GP_AXIS_X1_V:
            case NiAction::GP_AXIS_X2_H:
            case NiAction::GP_AXIS_X2_V:
                {
                    pkAction->SetControl(
                        NiAction::SemanticToDeviceControl(uiSemantic));
                    eErr = pkGamePad->AddMappedAction(pkAction);
                    if (eErr != NIIERR_OK)
                        m_eLastError = eErr;
                }
                break;
                // Button values
            case NiAction::GP_BUTTON_LUP:
            case NiAction::GP_BUTTON_LDOWN:
            case NiAction::GP_BUTTON_LLEFT:
            case NiAction::GP_BUTTON_LRIGHT:
            case NiAction::GP_BUTTON_L1:
            case NiAction::GP_BUTTON_L2:
            case NiAction::GP_BUTTON_R1:
            case NiAction::GP_BUTTON_R2:
            case NiAction::GP_BUTTON_RUP:
            case NiAction::GP_BUTTON_RDOWN:
            case NiAction::GP_BUTTON_RLEFT:
            case NiAction::GP_BUTTON_RRIGHT:
            case NiAction::GP_BUTTON_A:
            case NiAction::GP_BUTTON_B:
            case NiAction::GP_BUTTON_START:
            case NiAction::GP_BUTTON_SELECT:
                {
                    pkAction->SetControl(
                        NiAction::SemanticToDeviceControl(uiSemantic));
                    eErr = pkGamePad->AddMappedAction(pkAction);
                    if (eErr != NIIERR_OK)
                        m_eLastError = eErr;
                }
                break;
                // DPad
/***
            case NiAction::GP_DPAD:
                {
                    for (uj = 0; uj < MAX_GAMEPADS; uj++)
                    {
                        if (m_aspGamePads[uj])
                        {
                            eErr = m_aspGamePads[uj]->AddMappedAction(
                                pkAction);
                            if (eErr != NIIERR_OK)
                                m_eLastError = eErr;
                        }
                    }
                }
                break;
***/
            default:
                {
                    // Any Axis
                    // Any Button
                    if (IS_GP_BUTTON_ANY(uiSemantic))
                    {
                        NIASSERT(!"GPAnyButton mapping not supported "
                            "at this time!");
                        pkAction->SetHow(NiAction::NIA_UNMAPPED);
                    }
                    else
                    if ((IS_GP_AXIS_ANY_1(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_2(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_3(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_4(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_5(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_6(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_7(uiSemantic)) ||
                        (IS_GP_AXIS_ANY_8(uiSemantic)))
                    {
                        NIASSERT(!"GPAnyAxis mapping not supported at "
                            "this time!");
                        pkAction->SetHow(NiAction::NIA_UNMAPPED);
                    }
                }
                break;
            }
        }
    }

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputDevice::Description* NiInputSystem::GetDeviceDesc(unsigned int uiPort,
    unsigned int uiSlot)
{
    // NOTE: This function will break the GetFirst/GetNext cycle if called
    // while 'looping' over the available device descriptions.
    NiInputDevice::Description* pkDesc = GetFirstDeviceDesc();
    while (pkDesc)
    {
        if ((pkDesc->GetPort() == uiPort) &&
            (pkDesc->GetSlot() == uiSlot))
        {
            break;
        }
        pkDesc = GetNextDeviceDesc();
    }

    return pkDesc;
}
//---------------------------------------------------------------------------
void NiInputSystem::RemoveDeviceDesc(unsigned int uiPort, 
    unsigned int uiSlot)
{
    NiTListIterator kIter = m_kAvailableDevices.GetHeadPos();
    while (kIter)
    {
        NiInputDevice::Description* pkNiDesc = m_kAvailableDevices.GetNext(
            kIter);
        if (pkNiDesc)
        {
            if ((pkNiDesc->GetPort() == uiPort) &&
                (pkNiDesc->GetSlot() == uiSlot))
            {
                m_kAvailableDevices.Remove(pkNiDesc);
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiInputSystem::UpdateMouseModifiers()
{
    if (!m_spKeyboard || !m_spMouse)
        return;

    static unsigned int s_uiLastModifiers = 0;

    unsigned int uiCheck;
    unsigned int uiModifiers = m_spKeyboard->GetModifiers();
    if (uiModifiers != s_uiLastModifiers)
    {
        // Determine which ones changed...
        uiCheck = (s_uiLastModifiers ^ uiModifiers);
        if (uiCheck & NiInputKeyboard::KMOD_LCONTROL)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_LCONTROL)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_LCONTROL);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_LCONTROL);
        }
        if (uiCheck & NiInputKeyboard::KMOD_RCONTROL)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_RCONTROL)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_RCONTROL);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_RCONTROL);
        }
        if (uiCheck & NiInputKeyboard::KMOD_LMENU)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_LMENU)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_LMENU);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_LMENU);
        }
        if (uiCheck & NiInputKeyboard::KMOD_RMENU)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_RMENU)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_RMENU);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_RMENU);
        }
        if (uiCheck & NiInputKeyboard::KMOD_LWIN)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_LWIN)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_LWIN);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_LWIN);
        }
        if (uiCheck & NiInputKeyboard::KMOD_RWIN)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_RWIN)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_RWIN);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_RWIN);
        }
        if (uiCheck & NiInputKeyboard::KMOD_LSHIFT)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_LSHIFT)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_LSHIFT);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_LSHIFT);
        }
        if (uiCheck & NiInputKeyboard::KMOD_RSHIFT)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_RSHIFT)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_RSHIFT);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_RSHIFT);
        }
        if (uiCheck & NiInputKeyboard::KMOD_CAPS_LOCK)
        {
            if (uiModifiers & NiInputKeyboard::KMOD_CAPS_LOCK)
                m_spMouse->AddModifiers(NiInputMouse::MMOD_CAPS_LOCK);
            else
                m_spMouse->RemoveModifiers(NiInputMouse::MMOD_CAPS_LOCK);
        }

        s_uiLastModifiers = uiModifiers;
    }
}
//---------------------------------------------------------------------------
