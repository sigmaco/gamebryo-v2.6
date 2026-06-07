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

#ifndef NIINPUTXINPUTGAMEPAD_H
#define NIINPUTXINPUTGAMEPAD_H

#include "NiInputGamePad.h"

class NiDI8InputSystem;

class NIINPUT_ENTRY NiInputXInputGamePad : public NiInputGamePad
{
    NiDeclareRTTI;

public:
    // *** begin Emergent internal use only ***
    NiInputXInputGamePad(NiDI8InputSystem* pkInputSystem, 
        NiInputDevice::Description* pkDescription, 
        int iStickRangeLow, int iStickRangeHigh);
    // *** end Emergent internal use only ***

public:
    virtual ~NiInputXInputGamePad();

    // 
    virtual NiInputErr UpdateDevice();

    // Rumble
    virtual unsigned int GetMotorCount() const;
    virtual unsigned int GetRumbleRange(unsigned int uiMotor) const;
    virtual void SetRumbleValue(unsigned int uiMotor, unsigned int uiValue,
        bool bCommit);
    virtual void GetRumbleValue(unsigned int uiMotor, unsigned int& uiValue)
        const;
    virtual void StartRumble();
    virtual void StopRumble(bool bQuickStop = false);

    // *** begin Emergent internal use only ***
    bool UpdateDeviceState(XINPUT_STATE& kNewState);
    // *** end Emergent internal use only ***

protected:
    bool UpdateDigitalButtons(WORD wButtons);
    bool UpdateSticks(XINPUT_STATE& kNewState);

    NiDI8InputSystem* m_pkInputSystem;
    DWORD m_dwLastStatePacketNumber;

    XINPUT_VIBRATION m_kVibration;

    enum
    {
        MAX_ACTUATORS = 2
    };
    unsigned short m_ausActuatorMax[MAX_ACTUATORS];
    unsigned short m_ausActuatorData[MAX_ACTUATORS];
};

NiSmartPointer(NiInputXInputGamePad);

#endif  //#ifndef NIINPUTXINPUTGAMEPAD_H
