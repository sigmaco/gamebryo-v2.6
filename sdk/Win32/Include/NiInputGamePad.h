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

#ifndef NIINPUTGAMEPAD_H
#define NIINPUTGAMEPAD_H

#include "NiInputDevice.h"

class NIINPUT_ENTRY NiInputGamePad : public NiInputDevice
{
    NiDeclareRTTI;

public:
    // An analog stick is composed of 2 axes - horizontal and vertical.
    // Some gamepads have single axis controls available, such as the
    // WingMan RumblePad slider. This will be viewed as a single stick
    // with only one axis ever changing in the NiInput library.

    // Buttons
    enum Button
    {
        NIGP_NONE    = -1, 
        NIGP_LUP     =  0, 
        NIGP_LDOWN   =  1, 
        NIGP_LLEFT   =  2, 
        NIGP_LRIGHT  =  3, 
        NIGP_L1      =  4, 
        NIGP_L2      =  5, 
        NIGP_R1      =  6, 
        NIGP_R2      =  7, 
        NIGP_RUP     =  8, 
        NIGP_RDOWN   =  9, 
        NIGP_RLEFT   = 10, 
        NIGP_RRIGHT  = 11, 
        NIGP_A       = 12, 
        NIGP_B       = 13, 
        NIGP_START   = 14, 
        NIGP_SELECT  = 15, 
        NIGP_NUMBUTTONS,
        // If the device has more buttons, the user can access them via
        // the following value. For example, to access the 3 'user' button,
        // use NIGP_USER_0 + 2.
        NIGP_USER_0  = NIGP_NUMBUTTONS
    };

    // ButtonMasks
    enum ButtonMask
    {
        NIGP_MASK_NONE    = 0, 
        NIGP_MASK_LUP     = 1 << NIGP_LUP,
        NIGP_MASK_LDOWN   = 1 << NIGP_LDOWN,
        NIGP_MASK_LLEFT   = 1 << NIGP_LLEFT,
        NIGP_MASK_LRIGHT  = 1 << NIGP_LRIGHT,
        NIGP_MASK_L1      = 1 << NIGP_L1,
        NIGP_MASK_L2      = 1 << NIGP_L2,
        NIGP_MASK_R1      = 1 << NIGP_R1,
        NIGP_MASK_R2      = 1 << NIGP_R2,
        NIGP_MASK_RUP     = 1 << NIGP_RUP,
        NIGP_MASK_RDOWN   = 1 << NIGP_RDOWN,
        NIGP_MASK_RLEFT   = 1 << NIGP_RLEFT,
        NIGP_MASK_RRIGHT  = 1 << NIGP_RRIGHT,
        NIGP_MASK_A       = 1 << NIGP_A,
        NIGP_MASK_B       = 1 << NIGP_B,
        NIGP_MASK_START   = 1 << NIGP_START,
        NIGP_MASK_SELECT  = 1 << NIGP_SELECT
    };

    // Default Gamebryo axes.
    enum DefaultAxis
    {
        NIGP_DEFAULT_LEFT_HORZ  = 0, 
        NIGP_DEFAULT_LEFT_VERT  = 1, 
        NIGP_DEFAULT_RIGHT_HORZ = 2, 
        NIGP_DEFAULT_RIGHT_VERT = 3, 
        NIGP_DEFAULT_X1_HORZ    = 4, 
        NIGP_DEFAULT_X1_VERT    = 5, 
        NIGP_DEFAULT_X2_HORZ    = 6, 
        NIGP_DEFAULT_X2_VERT    = 7,

        NIGP_NUM_DEFAULT_AXIS
    };

    // Stick enumerations. Includes which stick as well as which
    // axis
    enum Sticks
    {
        NIGP_STICK_LEFT         = 0x00010000,
        NIGP_STICK_RIGHT        = 0x00020000,
        NIGP_STICK_X1           = 0x00040000,
        NIGP_STICK_X2           = 0x00080000,
        NIGP_STICK_MASK         = 0x000f0000,
        NIGP_STICK_AXIS_H       = 0x00000001,
        NIGP_STICK_AXIS_V       = 0x00000002,
        NIGP_STICK_AXIS_3       = 0x00000004,
        NIGP_STICK_AXIS_MASK    = 0x0000000f
    };

    enum StickInvert
    {
        // 1 << NIGP_DEFAULT_LEFT_HORZ
        NIGP_INVERT_LEFT_H      = 0x00000001,
        // 1 << NIGP_DEFAULT_LEFT_VERT
        NIGP_INVERT_LEFT_V      = 0x00000002,
        // 1 << NIGP_DEFAULT_RIGHT_HORZ
        NIGP_INVERT_RIGHT_H     = 0x00000004,
        // 1 << NIGP_DEFAULT_RIGHT_VERT
        NIGP_INVERT_RIGHT_V     = 0x00000008,
        NIGP_INVERT_X1_H        = 0x00000010,
        NIGP_INVERT_X1_V        = 0x00000020,
        NIGP_INVERT_X2_H        = 0x00000040,
        NIGP_INVERT_X2_V        = 0x00000080
    };

    // Miscellaneous
    enum
    {
        NIGP_MAX_BUTTONS        = NIGP_NUMBUTTONS * 2,
        NIGP_MAX_AXES           = 32
    };

public:
    // *** begin Emergent internal use only ***
    NiInputGamePad(NiInputDevice::Description* pkDescription,
        int iStickRangeLow, int iStickRangeHigh);
    // *** end Emergent internal use only ***

public:
    virtual ~NiInputGamePad();

    // Setup... Must be called prior to initializing the input system
    static unsigned int GetMaxButtonCount();
    static void SetMaxButtonCount(unsigned int uiCount);
    static unsigned int GetMaxAxes();
    static void SetMaxAxes(unsigned int uiCount);
    static int GetDefaultAnalogThreshold();
    static void SetDefaultAnalogThreshold(int iThreshold);
    static float GetDefaultAnalogDeadZone();
    static void SetDefaultAnalogDeadZone(float fThreshold);

    //
    virtual NiInputErr UpdateDevice();
    virtual NiInputErr UpdateActionMappedDevice(
        NiInputSystem* pkInputSystem);
    virtual NiInputErr HandleRemoval();
    virtual NiInputErr HandleInsertion();

    // Pad index - used by the input system to identify the device
    inline const unsigned int GetPadIndex() const;

    // Axes
    // 
    inline const unsigned int GetAxisCount() const;
    const int GetAxisValue(unsigned int uiAxis) const;
    const int GetStickAxisValue(unsigned int uiStickFlags) const;
    const void GetStickValue(unsigned int uiStickFlags, int& iHorz, 
        int& iVert) const;

    inline const unsigned int GetStickInvertFlags() const;
    inline void SetStickInvertFlags(unsigned int uiFlags);
    inline void AddStickInvertFlag(StickInvert eStickInvert);
    inline void RemoveStickInvertFlag(StickInvert eStickInvert);

    // Buttons
    // 
    inline const unsigned int GetButtonCount() const;
    // For digital...
    // Quick-reference button masks
    inline const unsigned int GetCurrentButtons() const;
    inline const unsigned int GetNewButtons() const;
    // If the button is analog, the value of the button must be greater than 
    // or equal to the threshold value to register as 'down'.
    const bool ButtonIsDown(Button eButton, 
        unsigned int& uiModifiers) const;
    const bool ButtonIsDown(Button eButton) const;

    const bool ButtonWasPressed(Button eButton, 
        unsigned int& uiModifiers) const;
    const bool ButtonWasPressed(Button eButton) const;

    const bool ButtonWasReleased(Button eButton, 
        unsigned int& uiModifiers) const;
    const bool ButtonWasReleased(Button eButton) const;

    // For analog...
    // If the button is digital, a 'down' button will report 255.
    const unsigned char ButtonState(Button eButton, 
        unsigned int& uiModifiers) const;
    const unsigned char ButtonState(Button eButton) const;

    // Rumble
    virtual unsigned int GetMotorCount() const = 0;

    // The maximum allowable value for the given motor.
    virtual unsigned int GetRumbleRange(unsigned int uiMotor) const = 0;
    
    // uiValue in [0..value returned by GetRumbleRange]
    // If bCommit is true, the value and all uncommited values for other 
    // motors will be set on the device.
    virtual void SetRumbleValue(unsigned int uiMotor, unsigned int uiValue,
        bool bCommit) = 0;

    // Access the current value for a motor, even if that value is uncommited.
    virtual void GetRumbleValue(unsigned int uiMotor, unsigned int& uiValue)
        const = 0;

    // fValue in [0.0..1.0]. Floating point values are converted to the
    // fixed-point range of the motor, so values returned by GetRumbleValue
    // will not in general be exactly equal to values passed to SetRumbleValue.
    void SetRumbleValue(unsigned int uiMotor, float fValue, bool bCommit);
    void GetRumbleValue(unsigned int uiMotor, float& fValue) const;

    // Starts rumble - commits current values
    virtual void StartRumble() = 0;
    
    // Halts rumble
    virtual void StopRumble(bool bQuickStop = false) = 0;

    // *** begin Emergent internal use only ***
    inline void SetPadIndex(unsigned int uiIndex);

    inline void SetAxisRange(int iLow, int iHigh);
    inline void GetAxisRange(int &iLow, int &iHigh) const;

    inline void SetButtonCount(unsigned int uiCount);
    inline void SetAxisCount(unsigned int uiCount);

    void SetRawAxisValue(unsigned int uiAxis, int iValue);

    void RecordButtonPress(Button eButton, unsigned char ucValue = 0xff);
    void RecordButtonRelease(Button eButton);

    /// Record a button press or release based on ucValue and the analog 
    /// threshold.
    void UpdateAnalogButton(Button eButton, unsigned char ucValue = 0xff);

    virtual void TouchAllInputValues();

    virtual bool IsControlMapped(unsigned int uiControl, 
        unsigned int uiSemantic, bool bUseModifiers, 
        unsigned int uiModifiers, bool bUseRange = false,
        int iRangeLow = 0, int iRangeHigh = 0);
    // *** end Emergent internal use only ***

protected:
    // These UpdateMapped* functions are just to make the code easier
    // to follow.
    virtual bool UpdateMappedButtons(NiInputSystem* pkInputSystem);
//    virtual bool UpdateMappedAnalogButtons(NiInputSystem* pkInputSystem);
//    virtual bool UpdateMappedDigitalButtons(NiInputSystem* pkInputSystem);
    virtual bool UpdateMappedSticks(NiInputSystem* pkInputSystem);

    static unsigned int ms_uiMaxButtonCount;
    static unsigned int ms_uiMaxAxes;
    static int ms_iDefaultAnalogThreshold;
    static float ms_fDefaultAnalogDeadZone;

    // Pad 'number' - assigned by the InputSystem.
    // Typically, this will be the order the devices are created.
    unsigned int m_uiPadIndex;

    // Quick-reference button masks
    unsigned int m_uiCurrentButtons;
    unsigned int m_uiNewButtons;

    // Buttons
    unsigned int m_uiButtonCount;   // The ACTUAL button count for the device
    NiInputDevice::Button* m_pkButtons;
    // Analog axes
    unsigned int m_uiAxisCount;     // The ACTUAL axis count for the device.
    NiInputDevice::Axis* m_pkAxes;
    int* m_piAxisThresholds;
    unsigned int m_uiStickInvertFlags;
};

#define NIGP_STICK_LEFT_H       \
    (NiInputGamePad::NIGP_STICK_LEFT | NiInputGamePad::NIGP_STICK_AXIS_H)
#define NIGP_STICK_LEFT_V       \
    (NiInputGamePad::NIGP_STICK_LEFT | NiInputGamePad::NIGP_STICK_AXIS_V)
#define NIGP_STICK_RIGHT_H      \
    (NiInputGamePad::NIGP_STICK_RIGHT | NiInputGamePad::NIGP_STICK_AXIS_H)
#define NIGP_STICK_RIGHT_V      \
    (NiInputGamePad::NIGP_STICK_RIGHT | NiInputGamePad::NIGP_STICK_AXIS_V)
#define NIGP_STICK_X1_H         \
    (NiInputGamePad::NIGP_STICK_X1 | NiInputGamePad::NIGP_STICK_AXIS_H)
#define NIGP_STICK_X1_V         \
    (NiInputGamePad::NIGP_STICK_X1 | NiInputGamePad::NIGP_STICK_AXIS_V)
#define NIGP_STICK_X2_H         \
    (NiInputGamePad::NIGP_STICK_X2 | NiInputGamePad::NIGP_STICK_AXIS_H)
#define NIGP_STICK_X2_V         \
    (NiInputGamePad::NIGP_STICK_X2 | NiInputGamePad::NIGP_STICK_AXIS_V)

NiSmartPointer(NiInputGamePad);

#include "NiInputGamePad.inl"

#endif  //#ifndef NIINPUTGAMEPAD_H
