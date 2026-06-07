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
#include "stdafx.h"  
#include "NiDevice.h"
#include "NiSystem.h"
//---------------------------------------------------------------------------
NiDevice::NiDevice(unsigned int uiSize, bool bAllowDblClick, 
    float fDoubleClickThreshold)
{
    m_bAllowDoubleClick = bAllowDblClick;
    m_fDoubleClickThreshold = fDoubleClickThreshold;
    m_uiDeviceStateSize = uiSize;
    m_pfLastPressedTime = NiAlloc(float, uiSize);
    m_peDeviceState = NiAlloc(DeviceState, uiSize);
    for (unsigned int ui = 0; ui < m_uiDeviceStateSize; ui++)
    {
        m_peDeviceState[ui] = NOT_PRESSED;
        m_pfLastPressedTime[ui] = 0.0f;
    }
}
//---------------------------------------------------------------------------
NiDevice::~NiDevice()
{
    NiFree(m_pfLastPressedTime);
    NiFree(m_peDeviceState);
}
//---------------------------------------------------------------------------
void NiDevice::DeviceStateChange(DeviceIOEvent eEvent, unsigned int uiItem)
{
    DeviceState eState = GetDeviceState(uiItem);
    float fCurrentTime = NiGetCurrentTimeInSec();
    float fDeltaTime; 
    if (m_pfLastPressedTime[uiItem] == 0)
        fDeltaTime = 0;
    else
        fDeltaTime = fCurrentTime - m_pfLastPressedTime[uiItem];
    
    if (eState == DEVICE_ERROR)
        return;

    switch(eEvent)
    {
        case PRESSED_EVENT:           
            if (eState == NOT_PRESSED || eState == RELEASED)
                SetDeviceState(PRESSED, uiItem);
            else if (eState == PRESSED)
                SetDeviceState(CONTINUOUS_PRESSED, uiItem);
            break;
        case RELEASED_EVENT:
            if (AllowsDoubleClicking() && fDeltaTime != 0.0f &&
               fDeltaTime <= m_fDoubleClickThreshold)
                SetDeviceState(DOUBLE_CLICKED, uiItem);
            else 
               SetDeviceState(RELEASED, uiItem);
            m_pfLastPressedTime[uiItem] = fCurrentTime;
            break;
    }
}
//---------------------------------------------------------------------------
unsigned int NiDevice::GetDeviceStateArraySize()
{
    return m_uiDeviceStateSize;
}
//---------------------------------------------------------------------------
void NiDevice::SetDeviceState(DeviceState eState, unsigned int uiItem)
{
    if (uiItem >= GetDeviceStateArraySize())
        return;
    else
        m_peDeviceState[uiItem] = eState;
}
//---------------------------------------------------------------------------
NiDevice::DeviceState* NiDevice::GetDeviceStateArray()
{
    return m_peDeviceState;
}
//---------------------------------------------------------------------------
NiDevice::DeviceState NiDevice::GetDeviceState(unsigned int uiItem)
{
    if (uiItem >= GetDeviceStateArraySize())
        return DEVICE_ERROR;
    else
        return m_peDeviceState[uiItem];
}
//---------------------------------------------------------------------------
bool NiDevice::AllowsDoubleClicking()
{
    return m_bAllowDoubleClick;
}
//---------------------------------------------------------------------------
void NiDevice::AllowDoubleClick(bool bAllow)
{
    m_bAllowDoubleClick = bAllow;
}
//---------------------------------------------------------------------------
void NiDevice::UpdateDevice()
{
    for (unsigned int ui = 0; ui < GetDeviceStateArraySize(); ui++)
    {
        DeviceState eState = GetDeviceState(ui);
        switch(eState)
        {
            case DOUBLE_CLICKED:
                SetDeviceState(NOT_PRESSED, ui);
                break;
            case RELEASED:
                SetDeviceState(NOT_PRESSED, ui);
                break;
            case PRESSED:
                SetDeviceState(CONTINUOUS_PRESSED, ui);
                break;
        }
    }
}
//---------------------------------------------------------------------------
bool NiDevice::StatesMatch(unsigned int uiIndex, DeviceState eState)
{
    return GetDeviceState(uiIndex) == eState;
}
//---------------------------------------------------------------------------
