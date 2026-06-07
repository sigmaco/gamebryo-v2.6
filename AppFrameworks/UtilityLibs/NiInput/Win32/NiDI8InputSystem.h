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

#ifndef NIDI8INPUTSYSTEM_H
#define NIDI8INPUTSYSTEM_H

#include "NiInputSystem.h"

#include <NiSystem.h>
#include "NiDI8Headers.h"

#include "NiInput_D3DHeaders.h"

typedef HRESULT (WINAPI *NILPDICREATE)(HINSTANCE, DWORD, REFIID, LPVOID*, 
    LPUNKNOWN);
typedef DWORD (WINAPI *NILPXINPUTGETSTATE)(DWORD, XINPUT_STATE*);
typedef DWORD (WINAPI *NILPXINPUTSETSTATE)(DWORD, XINPUT_VIBRATION*);
typedef DWORD (WINAPI *NILPXINPUTGETCAPABILITIES)(DWORD, DWORD, 
    XINPUT_CAPABILITIES*);
typedef void (WINAPI *NILPXINPUTENABLE)(BOOL);
typedef DWORD (WINAPI *NILPXINPUTGETDSOUNDAUDIODEVICEGUIDS)(DWORD, GUID*, 
    GUID*);
typedef DWORD (WINAPI *NILPXINPUTGETBATTERYINFORMATION)(DWORD, BYTE, 
    XINPUT_BATTERY_INFORMATION*);
typedef DWORD (WINAPI *NILPXINPUTGETKEYSTROKE)(DWORD, DWORD, 
    PXINPUT_KEYSTROKE);

class NIINPUT_ENTRY NiDI8InputSystem : public NiInputSystem
{
    NiDeclareRTTI;

public:
    // Default action mapping for immediate mode
    enum DefaultActions
    {
        // By default, we will map 2 analog sticks, 12 buttons,
        // and 1 POV
        // Sticks
        AXIS_0_H = 0x00000000,
        AXIS_0_V,
        AXIS_1_H,
        AXIS_1_V,
        AXIS_2_H,
        AXIS_2_V,
        AXIS_3_H,
        AXIS_3_V,
        // POV - Represents LUP, LDOWN, LLEFT, and LRIGHT
        POV_DPAD,
        // Buttons
        BUTTON_RUP,
        BUTTON_RDOWN,
        BUTTON_RLEFT,
        BUTTON_RRIGHT,
        BUTTON_L1,
        BUTTON_R1,
        BUTTON_START,
        BUTTON_SELECT,
        BUTTON_L2,
        BUTTON_R2,
        BUTTON_A,
        BUTTON_B,
        // 
        DEFACTION_COUNT
    };

    // DirectInput Create Params
    class NIINPUT_ENTRY DI8CreateParams : public NiInputSystem::CreateParams
    {
        NiDeclareRTTI;
    public:
        DI8CreateParams();
        DI8CreateParams(HINSTANCE hInst, HWND hWnd, 
            NiActionMap* pkActionMap = 0, unsigned int uiKeyboard = 0, 
            unsigned int uiMouse = 0, 
            unsigned int uiGamePads = MAX_GAMEPADS,
            int iAxisRangeLow = -100, int iAxisRangeHigh = 100);
        ~DI8CreateParams();

        bool SetControlMapName(DefaultActions eAction, const char* pacName);
        const char* GetControlMapName(DefaultActions eAction);

        inline const HINSTANCE GetOwnerInstance() const;
        inline const HWND GetOwnerWindow() const;

        // *** begin Emergent internal use only ***
        inline void SetOwnerInstance(HINSTANCE hInst);
        inline void SetOwnerWindow(HWND hWnd);
        // *** end Emergent internal use only ***

    protected:
        HINSTANCE m_hOwnerInst;
        HWND m_hOwnerWnd;
        char* m_apcMappingNames[DEFACTION_COUNT];
    };

    class DI8Description : public NiInputDevice::Description
    {
    public:
        DI8Description();
        DI8Description(NiInputDevice::Type eType, unsigned int uiPort, 
            unsigned int uiSlot, const char* pcName, 
            const GUID* pkGUIDInstance);
        ~DI8Description();

        inline const GUID* GetGUIDInstance() const;

        // *** begin Emergent internal use only ***
        inline void SetGUIDInstance(const GUID* pkGUID);
        // *** end Emergent internal use only ***

    protected:
        GUID m_kGUIDInstance;
    };

public:
    // The constructor should NEVER be called by anyone but the
    // static NiInputSystem::Create function! It's public due to
    // the static function needing to access it.
    NiDI8InputSystem();

    virtual ~NiDI8InputSystem();

    // Gamepad - will attempt to open the gamepad at the given
    // port and slot
    virtual NiInputGamePad* OpenGamePad(unsigned int uiPort, 
        unsigned int uiSlot);

    // Mouse - will return the first mouse found, searching each port
    // in numerical order.
    virtual NiInputMouse* OpenMouse();

    // Keyboard - will return the first keyboard found, searching each
    // port in numerical order.
    virtual NiInputKeyboard* OpenKeyboard();

    // 
    virtual NiInputErr UpdateAllDevices();
    virtual NiInputErr UpdateActionMap();
    virtual NiInputErr HandleDeviceChanges();
    virtual NiInputErr HandleRemovals();
    virtual NiInputErr HandleInsertions();

    virtual NiInputErr CheckEnumerationStatus();

    virtual void SuspendInput();
    virtual void ResumeInput();

    // *** begin Emergent internal use only ***
    //
    static HINSTANCE GetOwnerInstance();
    static HWND GetOwnerWindow();
    static HINSTANCE GetXInputLibraryInstance();
    static const GUID* GetNiInputDefaultGUID();
    static unsigned int GetDefaultActionBufferSize();
    //
    inline const LPDIRECTINPUT8 GetDirectInput8() const;
    inline void SetOwnerInstance(HINSTANCE hInst);
    inline void SetOwnerWindow(HWND hWnd);
    virtual NiInputErr Initialize(CreateParams* pkParams);
    virtual NiInputErr Shutdown();

    NiInputErr SetActionMap();
    NiInputErr SetActionMap(NiInputDevice* pkDevice);
    NiInputErr SetActionMapOnDIDevice(LPDIRECTINPUTDEVICE8 pkDIDevice);

    static BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE pkDDI, 
        LPVOID pvRef);
    static BOOL CALLBACK DIEnumDevicesBySemanticsDefaultCallback(
        LPCDIDEVICEINSTANCE pkDDI, LPDIRECTINPUTDEVICE8 pkDID, DWORD dwFlags,
        DWORD dwRemaining, LPVOID pvRef);

    virtual NiInputErr EnumerateDeviceControls(
        NiDI8InputSystem::DI8Description* pkNiDesc,
        LPDIRECTINPUTDEVICE8 pkDI8Device);
    static BOOL CALLBACK DIEnumDeviceObjectsCallback(
        LPCDIDEVICEOBJECTINSTANCE pkDDOI, LPVOID pvRef);

    NiInputErr EnumerateDevices();

    bool SetControlMapName(DefaultActions eAction, const char* pacName);

    XINPUT_STATE* GetXInputState(unsigned int uiPort, unsigned int uiSlot);

    // XInput library functions
    static DWORD XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
    static DWORD XInputSetState(DWORD dwUserIndex, 
        XINPUT_VIBRATION* pVibration);
    static DWORD XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, 
        XINPUT_CAPABILITIES* pCapabilities);
    static void XInputEnable(BOOL enable);
    static DWORD XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, 
        GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
    static DWORD XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType, 
        XINPUT_BATTERY_INFORMATION* pBatteryInformation);
    static DWORD XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, 
        PXINPUT_KEYSTROKE pKeystroke);
    // *** end Emergent internal use only ***

protected:
    bool CreateDI8Interface();
    bool DIDeviceInstanceToNiInputDescription(LPCDIDEVICEINSTANCE pkDDI, 
        DI8Description* pkNiDesc);
    
    bool SetupDefaultActionMap();
    bool AddDefaultActionMappedDevice(const DIDEVICEINSTANCE* pkDDI, 
        const LPDIRECTINPUTDEVICE8 pkDevice);

    bool SetupNiActionMap();
    bool MapNiActionToDIAction(NiAction* pkNiAction, DIACTION* pkDIAction);

    NiInputDevice::Description* CreateNiInputDescriptionForXInput(
        unsigned int uiPort, unsigned int uiSlot);
    void ReadXInputGamePad(unsigned int uiPort);

    static BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput);

    virtual NiInputErr ConfigureDevices();
#if 0
    static BOOL CALLBACK DIConfigureDevicesCallback(
        IUnknown FAR* pkDDSTarget, LPVOID pvRef);
    bool RenderConfigureDevicesScreen();
    bool SetupConfigureDevices();
    bool ShutdownConfigureDevices();
#endif

    static HINSTANCE ms_hOwnerInst;
    static HWND ms_hOwnerWnd;

    static HINSTANCE ms_hXInputLib;
    
    static NILPXINPUTGETSTATE ms_pfnXInputGetState;
    static NILPXINPUTSETSTATE ms_pfnXInputSetState;
    static NILPXINPUTGETCAPABILITIES ms_pfnXInputGetCapabilities;
    static NILPXINPUTENABLE ms_pfnXInputEnable;
    static NILPXINPUTGETDSOUNDAUDIODEVICEGUIDS 
        ms_pfnXInputGetDSoundAudioDeviceGuids;
    static NILPXINPUTGETBATTERYINFORMATION ms_pfnXInputGetBatteryInformation;
    static NILPXINPUTGETKEYSTROKE ms_pfnXInputGetKeystroke;

    LPDIRECTINPUT8 m_pkDirectInput8;

    static unsigned int ms_uiDefaultActionBufferSize;
    unsigned int m_uiActions;
    DIACTION* m_pkActions;
    DIACTIONFORMAT m_kDIActionFormat;

    // Gamepad ports 0-3 are reserved for XInput devices, 
    // and 4-7 are for DirectInput devices.
    enum
    {
        XINPUT_PORT_BASE = 0,
        DI_PORT_BASE = XINPUT_PORT_BASE + NiInputSystem::MAX_XINPUT_GAMEPADS
    };

    class PortDevice
    {
    public:
        XINPUT_CAPABILITIES m_kCaps;    // Caps for game pad
        XINPUT_STATE m_kState;          // Last grabbed state
        bool m_bConnected;              // true if game pad connected
        bool m_bInserted;               // true if game pad inserted
        bool m_bRemoved;                // true if game pad removed
    };

    PortDevice m_akPorts[MAX_XINPUT_GAMEPADS];

    char* m_apcMappingNames[DEFACTION_COUNT];

    friend NiInputSystem* NiInputSystem::Create(CreateParams* pkParams);
};

NiSmartPointer(NiDI8InputSystem);

#include "NiDI8InputSystem.inl"

#endif  //#ifndef NIDI8INPUTSYSTEM_H
