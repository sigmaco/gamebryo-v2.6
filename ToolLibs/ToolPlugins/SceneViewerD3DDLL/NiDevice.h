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

#ifndef NIDEVICE_H
#define NIDEVICE_H

#include <NiMain.h>

class NiDevice : public NiRefObject
{
public:
    enum DeviceState
    {
        DEVICE_ERROR= -1,
        NOT_PRESSED =  0,
        PRESSED     =  1,
        CONTINUOUS_PRESSED,
        RELEASED,
        DOUBLE_CLICKED,
        NUM_DEVICE_STATES
    };

    enum DeviceIOEvent
    {
        PRESSED_EVENT ,
        RELEASED_EVENT,
    };

    NiDevice(unsigned int uiSize = 0, bool bAllowDblClick = false, 
        float fDoubleClickThreshold = 0.5f);
    ~NiDevice();
    DeviceState* GetDeviceStateArray();
    unsigned int NiDevice::GetDeviceStateArraySize();
    void DeviceStateChange(DeviceIOEvent eEvent, unsigned int uiItem);
    DeviceState GetDeviceState(unsigned int uiItem);
    bool AllowsDoubleClicking();
    void AllowDoubleClick(bool bAllow);

    // Call this after all events have been generated for the frame
    void UpdateDevice();
    bool StatesMatch(unsigned int uiIndex, DeviceState eState);

    // begin Emergent internal use only
    void SetDeviceState(DeviceState eState, unsigned int uiItem);
    // end Emergent internal use only use

protected:
    DeviceState* m_peDeviceState;
    float* m_pfLastPressedTime;
    float m_fDoubleClickThreshold;
    unsigned int m_uiDeviceStateSize;
    bool m_bAllowDoubleClick;

};


#endif