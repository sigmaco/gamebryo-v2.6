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

#ifndef NIINPUTDI8GAMEPAD
#define NIINPUTDI8GAMEPAD

#include "NiInputGamePad.h"
#include "NiDI8Headers.h"

class NIINPUT_ENTRY NiInputDI8GamePad : public NiInputGamePad
{
    NiDeclareRTTI;

public:
    NiInputDI8GamePad(NiInputDevice::Description* pkDescription,
        LPDIRECTINPUTDEVICE8 pkDIDevice, int iStickRangeLow, 
        int iStickRangeHigh);
    virtual ~NiInputDI8GamePad();

    //
    virtual NiInputErr UpdateDevice();
//    virtual NiInputErr UpdateActionMappedDevice(
//        NiInputSystem* pkInputSystem);
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    virtual unsigned int GetMotorCount() const;
    virtual unsigned int GetRumbleRange(unsigned int uiMotor) const;
    virtual void SetRumbleValue(unsigned int uiMotor, unsigned int uiValue,
        bool bCommit);
    virtual void GetRumbleValue(unsigned int uiMotor, unsigned int& uiValue)
        const;
    virtual void StartRumble();
    virtual void StopRumble(bool bQuickStop = false);

    // *** begin Emergent internal use only ***
    inline const LPDIRECTINPUTDEVICE8 GetDIDevice();
    // *** end Emergent internal use only ***

protected:
    void FlushBuffer();
    void UpdateImmediateData(unsigned int uiAppData, DWORD dwData);
    void UpdateActionData(NiInputSystem* pkInputSystem, NiAction* pkAction, 
        unsigned int uiControlID, DWORD dwData);
    void MapDIToNiControlIDs(unsigned int uiAppData, DWORD dwData, 
        unsigned int& uiControlID1, unsigned int& uiControlID2,
        unsigned int& uiControlID3, unsigned int& uiControlID4);

    LPDIRECTINPUTDEVICE8 m_pkDIDevice;
    DIJOYSTATE2 m_kLastState;
    unsigned int m_uiButtonTouchMask;
    unsigned int m_uiAxisTouchMask;
};

NiSmartPointer(NiInputDI8GamePad);

#include "NiInputDI8GamePad.inl"

#endif  //#ifndef NIINPUTDI8GAMEPAD
