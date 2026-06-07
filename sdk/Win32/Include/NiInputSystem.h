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

#ifndef NIINPUTSYSTEM_H
#define NIINPUTSYSTEM_H

#include <NiRefObject.h>
#include <NiRTTI.h>
#include <NiSmartPointer.h>
#include <NiTPointerList.h>
#include <NiTPool.h>

#include "NiInputLibType.h"
#include "NiInputErr.h"
#include "NiInputDevice.h"
#include "NiActionMap.h"
#include "NiActionData.h"

#include "NiInputGamePad.h"
#include "NiInputKeyboard.h"
#include "NiInputMouse.h"

class NIINPUT_ENTRY NiInputSystem : public NiRefObject
{
    NiDeclareRootRTTI(NiInputSystem);

public:
    enum 
    {
#if defined(_XENON)
        MAX_GAMEPADS    = 4
#elif defined (_PS3)
        MAX_GAMEPADS    = 4
#elif defined(_WII) // 4 GC controllers + 4 Wii controllers
        MAX_GAMEPADS    = 8
#else 
        MAX_DI_GAMEPADS = 4,
        MAX_XINPUT_GAMEPADS = 4,
        MAX_GAMEPADS    = MAX_DI_GAMEPADS + MAX_XINPUT_GAMEPADS
#endif 
    };

    enum Usage
    {
        FOREGROUND      = 0x10000000,
        BACKGROUND      = 0x20000000,
        EXCLUSIVE       = 0x40000000,
        NONEXCLUSIVE    = 0x80000000,
        USAGE_MASK      = 0xf0000000
    };

    enum Mode
    {
        IMMEDIATE       = 0x00000000,
        MAPPED          = 0x00000001
    };

    enum DeviceMasks
    {
        KEYBOARD        = 0x80000000,
        MOUSE           = 0x40000000,
        GAMEPAD_MASK    = 0x0000ffff
    };

protected:
    NiInputSystem();

public:
    virtual ~NiInputSystem();

    // The CreateParams class is used to provide a single create
    // function but allow for platform-specific creation params.
    class NIINPUT_ENTRY CreateParams : public NiMemObject
    {
        NiDeclareRootRTTI(CreateParams);
    public:
        CreateParams(NiActionMap* pkActionMap = 0,
            unsigned int uiKeyboard = 0, 
            unsigned int uiMouse = 0, 
            unsigned int uiGamePads = MAX_GAMEPADS,
            int iAxisRangeLow = -100, int iAxisRangeHigh = 100);
        virtual ~CreateParams();

        // *** begin Emergent internal use only ***
        inline NiActionMap* GetActionMap();
        // *** end Emergent internal use only ***
        inline const NiActionMap* GetActionMap() const;
        inline const bool UseKeyboard() const;
        inline const unsigned int GetKeyboardUsage() const;
        inline const bool UseMouse() const;
        inline const unsigned int GetMouseUsage() const;
        inline const unsigned int GamePadCount() const;
        inline const void GetAxisRange(int& iLow, int& iHigh) const;

        inline void SetActionMap(NiActionMap* pkActionMap);
        inline void SetKeyboardUsage(unsigned int uiUsage);
        inline void SetMouseUsage(unsigned int uiUsage);
        inline void SetGamePadCount(unsigned int uiCount);
        inline void SetAxisRange(int iLow, int iHigh);

    protected:
        NiActionMap* m_pkActionMap;
        unsigned int m_uiKeyboard;
        unsigned int m_uiMouse;
        unsigned int m_uiGamePads;
        int m_iAxisRangeLow;
        int m_iAxisRangeHigh;
    };
    
    // All input systems must implement this function.
    static NiInputSystem* Create(CreateParams* pkParams);

    //
    inline const NiInputErr GetLastError() const;
    inline const Mode GetMode() const;
    inline const int GetAxisRangeLow() const;
    inline const int GetAxisRangeHigh() const;

    // Generic-device access
    virtual NiInputDevice* OpenDevice(NiInputDevice::Type eType, 
        unsigned int uiPort, unsigned int uiSlot);
    virtual NiInputErr CloseDevice(NiInputDevice* pkDevice);

    // Gamepad - will attempt to open the gamepad at the given
    // port and slot
    virtual NiInputGamePad* OpenGamePad(unsigned int uiPort, 
        unsigned int uiSlot);
    virtual NiInputGamePad* OpenGamePad(
        NiInputDevice::Description* pkNiDesc);
    inline NiInputGamePad* GetGamePad(unsigned int uiPort, 
        unsigned int uiSlot) const;
    inline NiInputGamePad* GetGamePad(unsigned int uiPadIndex) const;

    // Mouse - will return the first mouse found, searching each port
    // in numerical order.
    virtual NiInputMouse* OpenMouse();
    inline NiInputMouse* GetMouse();

    // Keyboard - will return the first keyboard found, searching each
    // port in numerical order.
    virtual NiInputKeyboard* OpenKeyboard();
    inline NiInputKeyboard* GetKeyboard();

    // 
    virtual NiInputErr UpdateAllDevices();
    virtual NiInputErr UpdateActionMap();

    // Hot-swap action maps
    virtual NiActionMap* GetActionMap();
    virtual NiInputErr SetActionMap(NiActionMap* pkActionMap);

    // Retrieve action data
    virtual unsigned int GetActionDataCount();
    virtual NiActionData* PopActionData();
    virtual NiActionData* GetFirstActionData();
    virtual NiActionData* GetNextActionData();
    virtual NiInputErr FlushActionData();

    virtual NiInputErr HandleDeviceChanges();
    virtual NiInputErr HandleRemovals();
    virtual NiInputErr HandleInsertions();

    virtual NiInputErr CheckEnumerationStatus() = 0;

    virtual NiInputErr ConfigureDevices();

    virtual void SuspendInput();
    virtual void ResumeInput();

    // *** begin Emergent internal use only ***
    virtual NiInputErr Initialize(CreateParams* pkParams);
    virtual NiInputErr Shutdown();

    virtual unsigned int AddAvailableDevice(
        NiInputDevice::Description* pkDescription);

    // These are used by the mapped input system to create
    // the list of currently queued actions
    inline NiActionData* GetActionData();
    inline void ReleaseActionData(NiActionData* pkActionData);

    // Used by devices to report actions to the input system
    NiInputErr ReportAction(NiInputDevice* pkDevice, unsigned int uiAppData,
        unsigned int uiSequence, unsigned int uiDeviceData, void* pvContext);
    static unsigned int GetNextAvailableSequence();

    // Map the NiActions to the current devices
    NiInputErr MapNiActionsToDevices();
    NiInputErr MapNiActionsToKeyboard();
    NiInputErr MapNiActionsToMouse();
    NiInputErr MapNiActionsToGamePads();
    NiInputErr MapNiActionsToGamePad(NiInputGamePad* pkGamePad);

    virtual void RemoveDeviceDesc(unsigned int uiPort, unsigned int uiSlot);
    // *** end Emergent internal use only ***

    virtual unsigned int GetAvailableDeviceCount();
    virtual NiInputDevice::Description* GetFirstDeviceDesc();
    virtual NiInputDevice::Description* GetNextDeviceDesc();
    virtual NiInputDevice::Description* GetDeviceDesc(unsigned int uiPort,
        unsigned int uiSlot);

    // Helper functions
    // DeviceID = 0xSSSSPPPP --> Slot|Port
    static unsigned int MakeDeviceID(unsigned int uiPort, 
        unsigned int uiSlot);
    // Retrieve the port for a given DeviceID
    static unsigned int DeviceIDPort(unsigned int uiDeviceID);
    // Retrieve the slot for a given DeviceID
    static unsigned int DeviceIDSlot(unsigned int uiDeviceID);

protected:
    void UpdateMouseModifiers();

    NiInputErr m_eLastError;

    Mode m_eMode;

    unsigned int m_uiKeyboard;
    unsigned int m_uiMouse;
    unsigned int m_uiGamePads;
    int m_iAxisRangeLow;
    int m_iAxisRangeHigh;

    static unsigned int ms_uiSequence;

    // Enumeration flags
    // This boolean will be true when the system is in the process
    // of enumerating the devices.
    bool m_bEnumerating;

    // Available devices
    NiInputDevice::DeviceDescList m_kAvailableDevices;
    NiTListIterator m_kAvailableDeviceIter;

    // We only allow one keyboard and one mouse
    NiInputKeyboardPtr m_spKeyboard;
    NiInputMousePtr m_spMouse;
    // Array of game pads
    NiInputGamePadPtr m_aspGamePads[MAX_GAMEPADS];

    // Action map
    NiActionMapPtr m_spActionMap;
    // Queued Actions
    NiTListIterator m_kActionDataIter;
    NiTPointerList<NiActionData*> m_kActionDataList;
    // ActionData pool
    NiTPool<NiActionData, NiTNewInterface<NiActionData> >* 
        m_pkActionDataPool;
};

NiSmartPointer(NiInputSystem);

#include "NiInputSystem.inl"

#endif  //#ifndef NIINPUTSYSTEM_H
