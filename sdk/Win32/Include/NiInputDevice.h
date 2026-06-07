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

#ifndef NIINPUTDEVICE_H
#define NIINPUTDEVICE_H

#if defined(_XENON)
    #include "NiXenonInputHeaders.h"
#endif  //#if defined(_XENON)
#include "NiInputLibType.h"
#include "NiInputErr.h"

#include <NiRefObject.h>
#include <NiRTTI.h>
#include <NiSmartPointer.h>
#include <NiTPointerList.h>
#include <NiSystem.h>

class NiInputSystem;
class NiAction;

class NIINPUT_ENTRY NiInputDevice : public NiRefObject
{
    NiDeclareRootRTTI(NiInputDevice);

public:
    // Type - the type of input device. Categorizes the devices.
    enum Type
    {
        NIID_NULL = 0,
        NIID_KEYBOARD,
        NIID_MOUSE,
        NIID_GAMEPAD
    };

    // Status - the current status of the device.
    enum Status
    {
        UNKNOWN = -1,
        READY,
        BUSY,
        REMOVED,
        LOST
    };

    // ControlDesc - describes a single 'control' on a device, such as a 
    // button or an axis on a gamepad stick.
    class NIINPUT_ENTRY ControlDesc : public NiMemObject
    {
    public:
        enum Type
        {
            UNKNOWN = -1,  // Unknown control type
            BUTTON = 0,    // Button - analog or digital
            AXIS,          // Axis - single axis of a stick or a slider
            POV,           // PointOfView hat or similar control
            MOTOR          // Rumble motor (output only)
        };

        enum Flags
        {
            DATA_DIGITAL    = 0x00000001,
            DATA_ANALOG     = 0x00000002,
            DATA_MASK       = 0x0000000f,
            TYPE_INPUT      = 0x00000010,
            TYPE_OUTPUT     = 0x00000020,
            TYPE_HYBRID     = 0x00000030,
            TYPE_MASK       = 0x000000f0,
            OUT_RUMBLE      = 0x00000100,
            OUT_FORCEFB     = 0x00000200,
            OUT_MASK        = 0x00000f00
        };

        ControlDesc();
        ControlDesc(unsigned int uiIndex, const char* pcName, Type eType, 
            unsigned int uiFlags);
        ~ControlDesc();

        inline const unsigned int GetIndex() const;
        inline const char* GetName() const;
        inline const Type GetType() const;
        inline const unsigned int GetFlags() const;

        inline const bool IsDigital() const;
        inline const bool IsAnalog() const;
        inline const bool IsInput() const;
        inline const bool IsOutput() const;
        inline const bool IsHybrid() const;
        inline const bool IsRumble() const;
        inline const bool IsForceFeedback() const;

        // *** begin Emergent internal use only ***
        inline void SetIndex(unsigned int uiIndex);
        inline void SetName(const char* pcName);
        inline void SetType(Type eType);
        inline void SetFlags(unsigned int uiFlags);

        inline void SetDigital(bool bEnabled);
        inline void SetAnalog(bool bEnabled);
        inline void SetInput(bool bEnabled);
        inline void SetOutput(bool bEnabled);
        inline void SetHybrid(bool bEnabled);
        inline void SetRumble(bool bEnabled);
        inline void SetForceFeedback(bool bEnabled);

        inline const unsigned int GetPlatformSpecificData() const;
        inline void SetPlatformSpecificData(unsigned int uiPlatformSpecificData);
        // *** end Emergent internal use only ***

    protected:
        unsigned int m_uiIndex;
        char* m_pcName;
        Type m_eType;
        unsigned int m_uiFlags;
        unsigned int m_uiPlatformSpecificData;
    };

    typedef NiTPointerList<ControlDesc*> NiIDControlList;

    // Description - describes the input device for enumeration methods.
    // The application can check the description of the device to determine
    // what it's capabilities are.
    class NIINPUT_ENTRY Description : public NiRefObject
    {
    public:
        Description();
        Description(Type eType, unsigned int uiPort, unsigned int uiSlot,
            const char* pcName);
        ~Description();

        inline const Type GetType() const;
        inline const unsigned int GetPort() const;
        inline const unsigned int GetSlot() const;
        inline const char* GetName() const;
        inline const char* GetDescriptionString() const;
        const void* GetDeviceSpecificData() const;

        // *** begin Emergent internal use only ***
        inline void SetType(Type eType);
        inline void SetPort(unsigned int uiPort);
        inline void SetSlot(unsigned int uiSlot);
        inline void SetName(const char* pcName);
        inline void SetDescriptionString(const char* pcDesc);
        void SetDeviceSpecificData(void* pvData);
        inline const bool GetDeviceOwned() const;
        inline void SetDeviceOwned(bool bOwned);

        unsigned int AddControlDesc(ControlDesc* pkDesc);
        // Used by the enumeration function for keyboards.
        inline void IncrementButtonCount();
        // *** end Emergent internal use only ***

        inline unsigned int GetControlCount();
        inline ControlDesc* GetFirstControlDesc();
        inline ControlDesc* GetNextControlDesc();
        inline ControlDesc* GetControlDescAtIndex(unsigned int uiIndex);
        inline ControlDesc* GetControlDescByName(const char* pcName);

        inline const unsigned int GetButtonCount() const;
        inline const unsigned int GetAxisCount() const;
        inline const unsigned int GetPOVCount() const;

    protected:
        Type m_eType;
        unsigned int m_uiPort;
        unsigned int m_uiSlot;
        char* m_pcName;
        char* m_pcDesc;
        bool m_bDeviceOwned;

        NiIDControlList m_kControls;
        NiTListIterator m_kControlsIter;

        unsigned int m_uiButtonCount;
        unsigned int m_uiAxisCount;
        unsigned int m_uiPOVCount;
    };

    typedef NiPointer<Description> DescriptionPtr;
    typedef NiTPointerList<DescriptionPtr> DeviceDescList;

protected:
    NiInputDevice(Description* pkDescription);

public:
    virtual ~NiInputDevice();

    // member access
    inline const Type GetType() const;
    inline const Status GetStatus() const;
    inline const unsigned int GetPort() const;
    inline const unsigned int GetSlot() const;

    inline const unsigned int GetModifiers() const;

    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr UpdateActionMappedDevice(
        NiInputSystem* pkInputSystem);
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    //
    inline NiInputErr GetLastError();

    // *** begin Emergent internal use only ***
    inline void SetStatus(Status eStatus);
    inline void SetPort(unsigned int uiPort);
    inline void SetSlot(unsigned int uiSlot);

    inline const bool GetActionMapped() const;
    inline void SetActionMapped(bool bMapped);

    virtual void TouchAllInputValues();

    inline void AddModifiers(unsigned int uiMod);
    inline void RemoveModifiers(unsigned int uiMod);
    inline void ClearModifiers();
    inline bool ModifersAreDown(unsigned int uiModifiers) const;
    inline bool ModifiersAreUp(unsigned int uiModifiers) const;

    virtual bool IsControlMapped(unsigned int uiControl, 
        unsigned int uiSemantic, bool bUseModifiers, 
        unsigned int uiModifiers, bool bUseRange = false,
        int iRangeLow = 0, int iRangeHigh = 0);
    virtual NiInputErr AddMappedAction(NiAction* pkAction);
    NiInputErr RemoveMappedAction(NiAction* pkAction);
    NiInputErr ClearMappedActions();

    inline void SetLastError(NiInputErr eErr);
    // *** end Emergent internal use only ***

    // Button and Axes are defined here, as they are used in both 
    // NiInputGamePad and NiInputMouse.
    //
    // We are going to assume EVERY button as analog.
    // If a button is digital, then the platform-specific code dealing
    // with it should set it's value to 255 when it is pressed.
    // Since we need to have the state as well as the modifiers that were 
    // active when it was pressed, we will use a small class per button.
    class NIINPUT_ENTRY Button : public NiMemObject
    {
    public:
        Button();

        //
        inline const unsigned int GetModifiers() const;
        inline bool RemoveModifiers(unsigned int uiModifiers);
        inline const bool IsPressed(unsigned int& uiModifiers) const;
        inline const bool WasPressed(unsigned int& uiModifiers) const;
        inline const bool WasReleased(unsigned int& uiModifiers) const;

        inline const unsigned char GetValue() const;
        inline const unsigned char GetValue(unsigned int& uiModifiers) const;
        inline const unsigned char GetLastValue() const;
        inline const unsigned char GetLastValue(unsigned int& uiModifiers) const;
        inline const bool ValueChanged() const;
        inline const bool ValueChanged(bool& bModifiersChanged) const;

        inline void SetButtonPress(unsigned int uiModifiers, bool bPressed = true,
            unsigned char ucValue = 0xff);
        inline void SetButtonRelease();
    
        inline void UpdatePressedLastUpdate();

        inline const bool GetLastPressed() const;
        inline const unsigned int GetLastModifiers() const;

        // *** begin Emergent internal use only ***
        inline void TouchValue();
        // *** end Emergent internal use only ***

    protected:
        unsigned char m_ucValue;        // Analog
        bool m_bPressed;                // Digital (>= 128 on analog ones)

        unsigned char m_ucLastValue;    // Last Analog
        bool m_bLastPressed;            // Last Digital (>= 128 on analog)

        bool m_bPressedLastUpdate;

        unsigned int m_uiModifiers;     // Mod present when pressed
        unsigned int m_uiLastModifiers; // Mod present when last pressed
    };

    // Axis
    class NIINPUT_ENTRY Axis : public NiMemObject
    {
    public:
        Axis();
        Axis(int iLow, int iHigh);
        ~Axis();
        
        //
        inline void GetRange(int& iLow, int& iHigh) const;
        inline const int GetRawValue() const;
        inline const int GetValue() const;

        inline const int GetLastRawValue() const;
        inline const int GetLastValue() const;

        // *** begin Emergent internal use only ***
        inline void SetRawRange(int iLow, int iHigh);
        inline void SetRange(int iLow, int iHigh);

        void SetRawValue(int iValue, bool bInvert, float fDeadZone);
        inline void SetValue(int iValue);

        inline int MapRawValue(int iValue);

        inline void TouchValue();
        // *** end Emergent internal use only ***

    protected:
        int m_iRawValue;
        int m_iValue;

        int m_iLastRawValue;
        int m_iLastValue;

        int m_iRawLow;      // Used to specify the range
        int m_iRawHigh;
        int m_iLow;         // Used to specify the range
        int m_iHigh;
    };

protected:
    bool CheckActionDeviceIDAndModifiers(NiAction* pkAction,
        unsigned int uiModifiers);

    DescriptionPtr m_spDescription;

    Status m_eStatus;

    // Modifiers - all devices use them to some degree...
    unsigned int m_uiModifiers;

    bool m_bActionMapped;
    NiTPointerList<NiAction*>* m_pkMappedActions;

    NiInputErr m_eLastError;
};

NiSmartPointer(NiInputDevice);

#include "NiInputDevice.inl"

#endif  //#ifndef NIINPUTDEVICE_H
