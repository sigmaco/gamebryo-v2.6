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

#ifndef NIACTION_H
#define NIACTION_H

#include "NiInputLibType.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>

class NIINPUT_ENTRY NiAction : public NiRefObject
{
public:
    // Flags - used to request specific attributes or processing
    enum Flags
    {
        // Action cannot be remapped
        APPFIXED        = 0x00000001, 
        // The uiControl member is valid, should not be overridden
        APPMAPPED       = 0x00000002, 
        // Action not to be mapped
        APPNOMAP        = 0x00000004, 
        // Action must be mapped to an actuator or a trigger
        FORCEFEEDBACK   = 0x00000010, 
        // Default range should not be set for this action
        NORANGE         = 0x00000020, 
        // Use the Gamebryo-supplied range for the action
        GAMEBRYORANGE   = 0x00000040,
        // Analog button requested (Consoles only???)
        ANALOG          = 0x00000080,
        // Boolean - return 0 or 1 for an action, indicating that the
        // action has occured/been pressed or release.
        RETURN_BOOLEAN  = 0x00000100,
        // Indicates that the modifiers should be checked to see if
        // an action has occured.
        USE_MODIFIERS   = 0x00000200,
        // Indicates this action should only 'count' when it occurs
        // on a specific device, indicated by the m_uiDeviceID member.
        DEVICE_SPECIFIC = 0x00000400,
        // Indicates this action should be reported EVERY update.
        // If not present, the action will only be reported on the
        // edge cases.
        CONTINUOUS      = 0x00001000
    };

    enum How
    {
        NIA_APPREQUESTED    = 0x00000001,
        NIA_DEFAULT         = 0x00000002,
        NIA_ERROR           = 0x00000003,
        NIA_HWAPP           = 0x00000004,
        NIA_HWDEFAULT       = 0x00000005,
        NIA_UNMAPPED        = 0x00000006,
        NIA_USERCONFIG      = 0x00000007
    };

    enum Semantic
    {
        SEMANTIC_TYPE_MASK      = 0xfffff000,
        //*** GAME PAD MAPPINGS ***//
        GAMEPAD_MASK            = 0x10000000,
        //*** Defined Axis Mapping
        // Left analog stick
        GP_AXIS_LEFT_H          = 0x10000001,
        GP_AXIS_LEFT_V          = 0x10000002,
        // Right analog stick
        GP_AXIS_RIGHT_H         = 0x10000003,
        GP_AXIS_RIGHT_V         = 0x10000004,
        // Additional analog stick(s) for future support
        GP_AXIS_X1_H            = 0x10000005,
        GP_AXIS_X1_V            = 0x10000006,
        GP_AXIS_X2_H            = 0x10000007,
        GP_AXIS_X2_V            = 0x10000008,
        //*** Defined Button Mapping
        GP_BUTTON_LUP           = 0x10000100, 
        GP_BUTTON_LDOWN         = 0x10000101, 
        GP_BUTTON_LLEFT         = 0x10000102, 
        GP_BUTTON_LRIGHT        = 0x10000103, 
        GP_BUTTON_L1            = 0x10000104, 
        GP_BUTTON_L2            = 0x10000105, 
        GP_BUTTON_R1            = 0x10000106, 
        GP_BUTTON_R2            = 0x10000107, 
        GP_BUTTON_RUP           = 0x10000108, 
        GP_BUTTON_RDOWN         = 0x10000109, 
        GP_BUTTON_RLEFT         = 0x1000010a, 
        GP_BUTTON_RRIGHT        = 0x1000010b, 
        GP_BUTTON_A             = 0x1000010c, 
        GP_BUTTON_B             = 0x1000010d, 
        GP_BUTTON_START         = 0x1000010e, 
        GP_BUTTON_SELECT        = 0x1000010f, 
        // Direction Pad (DPad)
        //GP_DPAD                 = 0x10000200,
        //*** Helper mapping - *_ANY_*
        // Any Axis
        GP_AXIS_ANY_1           = 0x10010000,
        GP_AXIS_ANY_2           = 0x10010001,
        GP_AXIS_ANY_3           = 0x10010002,
        GP_AXIS_ANY_4           = 0x10010003,
        GP_AXIS_ANY_5           = 0x10001004,
        GP_AXIS_ANY_6           = 0x10001005,
        GP_AXIS_ANY_7           = 0x10001006,
        GP_AXIS_ANY_8           = 0x10001007,
        // Any Button
        GP_BUTTON_ANY_BASE      = 0x10010100,

        //*** KEYBOARD MAPPINGS ***//
        KEY_MASK                = 0x20000000,
        KEY_ANY_BASE            = 0x20010000,

        //*** MOUSE MAPPINGS ***//
        MOUSE_MASK              = 0x40000000,
        //*** Axis mappings
        MOUSE_AXIS_X            = 0x40000001,
        MOUSE_AXIS_Y            = 0x40000002,
        MOUSE_AXIS_Z            = 0x40000003,
        //*** Button mappings
        MOUSE_BUTTON_LEFT       = 0x40001000,
        MOUSE_BUTTON_RIGHT      = 0x40001001,
        MOUSE_BUTTON_MIDDLE     = 0x40001002,
        MOUSE_BUTTON_X1         = 0x40001003,
        MOUSE_BUTTON_X2         = 0x40001004,
        MOUSE_BUTTON_X3         = 0x40001005,
        MOUSE_BUTTON_X4         = 0x40001006,
        MOUSE_BUTTON_X5         = 0x40001007,
        //*** Any axis mapping
        MOUSE_AXIS_ANY_1        = 0x40010001,
        MOUSE_AXIS_ANY_2        = 0x40010002,
        MOUSE_AXIS_ANY_3        = 0x40010003,
        //*** Any button mapping
        MOUSE_BUTTON_ANY_BASE   = 0x40010100
    };

    NiAction();
    NiAction(const char* pcName, unsigned int uiAppData, 
        unsigned int uiSemantic, unsigned int uiFlags, 
        unsigned int uiDeviceID = 0, int iRangeLow = 0, int iRangeHigh = 0,
        void* pvContext = 0);
    virtual ~NiAction();

    //
    inline const char* GetName() const;
    inline const unsigned int GetAppData() const;
    inline const unsigned int GetSemantic() const;
    inline const unsigned int GetFlags() const;
    inline const unsigned int GetDeviceID() const;
    inline const void GetRange(int& iLow, int& iHigh) const;
    inline void* GetContext();
    inline const unsigned int GetKeyModifiers() const;
    inline const unsigned int GetMouseModifiers() const;
    inline const unsigned int GetPadModifiers() const;

    inline const unsigned int GetControl() const;
    inline const unsigned int GetHow() const;

    void SetName(const char* pcName);
    inline void SetAppData(unsigned int uiAppData);
    inline void SetSemantic(unsigned int uiSemantic);
    inline void SetFlags(unsigned int uiFlags);
    inline void SetDeviceID(unsigned int uiDeviceID);
    inline void SetRange(int iLow, int iHigh);
    inline void SetContext(void* pvContext);
    inline void SetKeyModifiers(unsigned int uiModifiers);
    inline void SetMouseModifiers(unsigned int uiModifiers);
    inline void SetPadModifiers(unsigned int uiModifiers);

    // *** begin Emergent internal use only ***
    inline void SetControl(unsigned int uiControl);
    inline void SetHow(unsigned int uiHow);

    static unsigned int SemanticToDeviceControl(unsigned int uiSemantic);
    static unsigned int SemanticToDeviceControl_GamePad(
        unsigned int uiSemantic);
    static unsigned int SemanticToDeviceControl_Mouse(
        unsigned int uiSemantic);
    static unsigned int SemanticToDeviceControl_Keyboard(
        unsigned int uiSemantic);
    // *** end Emergent internal use only ***

protected:
    char* m_pcName;             // Name of the action (assigned by the user)
    unsigned int m_uiAppData;   // App assigned data, can be a pointer
    unsigned int m_uiSemantic;  // Predefined mapping value
    unsigned int m_uiFlags;     // Flags used for assignement
    unsigned int m_uiDeviceID;  // DeviceID (0xSSSSPPPP --> Slot|Port)
    int m_iRangeLow;            // Range of input (for axis & analog values)
    int m_iRangeHigh;           // When in this range, action triggered

    void* m_pvContext;          // Context data passed back

    // DeviceID

    // Modifiers to require for the action
    unsigned int m_uiKeyModifiers;
    unsigned int m_uiMouseModifiers;
    unsigned int m_uiPadModifiers;

    // Return values
    unsigned int m_uiControl;   // Control action was mapped to
    unsigned int m_uiHow;       // Result - how the control was mapped
};

//*** Helper macros for action mapping
#define SEMANTIC_TYPE(x)        (x & NiAction::SEMANTIC_TYPE_MASK)
// Button macro
#define IS_GP_MAPPING(x)                                    \
    (((x & NiAction::GAMEPAD_MASK) ==                       \
    NiAction::GAMEPAD_MASK) ? true : false)

#define IS_GP_AXIS_ANY_1(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_1) ==                      \
    NiAction::GP_AXIS_ANY_1) ? true : false)
#define IS_GP_AXIS_ANY_2(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_2) ==                      \
    NiAction::GP_AXIS_ANY_2) ? true : false)
#define IS_GP_AXIS_ANY_3(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_3) ==                      \
    NiAction::GP_AXIS_ANY_3) ? true : false)
#define IS_GP_AXIS_ANY_4(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_4) ==                      \
    NiAction::GP_AXIS_ANY_4) ? true : false)
#define IS_GP_AXIS_ANY_5(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_5) ==                      \
    NiAction::GP_AXIS_ANY_5) ? true : false)
#define IS_GP_AXIS_ANY_6(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_6) ==                      \
    NiAction::GP_AXIS_ANY_6) ? true : false)
#define IS_GP_AXIS_ANY_7(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_7) ==                      \
    NiAction::GP_AXIS_ANY_7) ? true : false)
#define IS_GP_AXIS_ANY_8(x)                                 \
    (((x & NiAction::GP_AXIS_ANY_8) ==                      \
    NiAction::GP_AXIS_ANY_8) ? true : false)
#define IS_GP_BUTTON_ANY(x)                                 \
    (((x & NiAction::GP_BUTTON_ANY_BASE) ==                 \
    NiAction::GP_BUTTON_ANY_BASE) ? true : false)

#define GP_BUTTON_ANY(x)    (x | NiAction::BUTTON_ANY_BASE)

// Key macros
#define IS_KEY_MAPPING(x)                                   \
    (((x & NiAction::KEY_MASK) ==                           \
    NiAction::KEY_MASK) ? true : false)
#define IS_KEY_ANY(x)                                       \
    (((x & NiAction::KEY_ANY_BASE) ==                       \
    NiAction::KEY_ANY_BASE) ? true : false)

#define KEY_MAP(x)          (x | NiAction::KEY_MASK)
#define KEY_ANY(x)          (x | NiAction::KEY_ANY_BASE)

// Mouse macros
// IS_MOUSE_MAPPING(x) - Returns true if x is a mouse-mapped action
#define IS_MOUSE_MAPPING(x)                                 \
    (((x & NiAction::MOUSE_MASK) ==                \
    NiAction::MOUSE_MASK) ? true : false)
// IS_MOUSE_BUTTON_ANY(x) - Returns true if x is mapped for any mouse button
#define IS_MOUSE_BUTTON_ANY(x)                              \
    (((x & NiAction::MOUSE_BUTTON_ANY_BASE) ==     \
    NiAction::MOUSE_BUTTON_ANY_BASE) ? true : false)
// IS_MOUSE_AXIS_ANY_#(x) - Returns true if x is mapped for any mouse axis #
#define IS_MOUSE_AXIS_ANY_1(x)                              \
    (((x & NiAction::MOUSE_AXIS_ANY_1) ==          \
    NiAction::MOUSE_AXIS_ANY_1) ? true : false)
#define IS_MOUSE_AXIS_ANY_2(x)                              \
    (((x & NiAction::MOUSE_AXIS_ANY_2) ==          \
    NiAction::MOUSE_AXIS_ANY_2) ? true : false)
#define IS_MOUSE_AXIS_ANY_3(x)                              \
    (((x & NiAction::MOUSE_AXIS_ANY_3) ==          \
    NiAction::MOUSE_AXIS_ANY_3) ? true : false)

// MOUSE_BUTTON_ANY(x) - used to set the semantic
#define MOUSE_BUTTON_ANY(x) (x | NiAction::MOUSE_BUTTON_ANY_BASE)
//***

NiSmartPointer(NiAction);

#include "NiAction.inl"

#endif  //#ifndef NIACTION_H
