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

#ifndef NINAVBASECONTROLLER_H
#define NINAVBASECONTROLLER_H

#include "NiUserInterfaceLibType.h"
#include <NiRect.h>
#include <NiInput.h>
#include <NiTPtrSet.h>
#include <NiAVObject.h>
#include <NiRefObject.h>
#include "NiUIRenderGroup.h"
#include "NiUIHotkeyImage.h"
#include "NiUILabel.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiNavBaseController : public NiRefObject
{
public:
    struct NavState
    {
        // Defines the frame and (perhaps) origin
        NiAVObject* pkReferenceObject;
        // Scale of all navigation
        float fScale;
        // This is used for the first navigation state
        bool bIgnoreThis;
    };

    NiNavBaseController(const char* pcName);
    virtual ~NiNavBaseController();

    // When bActive is true, kCurState should contain current state 
    // information.  Otherwise, this object will set kCurState.
    virtual bool SetActive(bool bActive, NavState& kCurState);
    void SampleAndUpdate();
    inline const NiString& GetName() const;
    inline bool IsActive() const;

    enum ControlType
    {
        NEGATIVE_AXIS = 0,
        POSITIVE_AXIS = 1,
        DEFAULT = 1
    };

    class InputBinding
    {
    public:
        InputBinding();
        InputBinding(InputBinding* pkKey);
        InputBinding operator = (const InputBinding& kKey);
        inline bool operator != (const InputBinding& kKey) const;
        inline bool operator == (const InputBinding& kKey) const;

        inline bool SupportsKeyboard() const;
        inline bool SupportsMouseAxis() const;
        inline bool SupportsMouseButton() const;
        inline bool SupportsGamePadAxis() const;
        inline bool SupportsGamePadButton() const;

        // public members
        unsigned char ucListenTypes;
        NiInputKeyboard::KeyCode eKeyboardKey;
        NiInputKeyboard::Modifiers eKeyboardModifier;
        NiInputMouse::Button eMouseButton;
        NiInputMouse::Modifiers eMouseModifier;
        NiInputMouse::Axes eMouseAxis;
        NiInputGamePad::Button eGamePadButton;
        NiInputGamePad::ButtonMask eGamePadModifier;
        NiInputGamePad::ButtonMask eGamePadOptionalModifier;
        NiInputGamePad::DefaultAxis eGamePadAxis;
    };

    inline unsigned int GetControlCount() const;
    virtual const char* GetControlName(unsigned int uiControlId) const = 0;
    virtual bool IsControlActive(unsigned int uiControlId) const;
    inline void SetControlActive(unsigned int uiControlId, bool bActive = true);
    inline const InputBinding& GetBinding(unsigned int uiControlId, 
        ControlType eType) const;

protected:
    virtual void UpdatePose() = 0;

    enum InputTypes
    {
        InputKeyboard = 0x01,
        InputMouseAxis = 0x02,
        InputMouseButton = 0x04,
        InputGamePadAxis = 0x08,
        InputGamePadButton = 0x10
    };
    
    // Pose altering functions used by several inherting classes
    inline void TranslateAlongAxis(const NiPoint3& kDir, float fAmount, 
        const NiPoint3& kStartPos, NiPoint3& kEndPos) const;
    inline void GetFrameFromObject(const NiAVObject* kFrameOwner, 
        NiPoint3& kForward, NiPoint3& kRight, NiPoint3& kUp) const;
    inline void CreateNewFrame(const NiPoint3& kOldForward, 
        const NiPoint3& kOldUp, float fRotUpAmt, const NiPoint3& kNewForward,
        NiPoint3& kNewRight, NiPoint3& kNewUp) const;

    inline unsigned int GetIdx(unsigned int uiControlId, bool bPositiveDir);

    virtual void MouseInput(unsigned int uiControlId, float fDelta) = 0;
    virtual void GamePadInput(unsigned int uiControlId, float fDelta) = 0;

    void MouseLeftButton(unsigned char ucState);
    NiUIMemberSlot1<NiNavBaseController, unsigned char> m_kMouseSlot;

    NiTPrimitiveArray<NiUIBaseSlot1<unsigned char>*> m_kSlots;
    NiTPrimitiveArray<InputBinding> m_kBindings;
    NiTPrimitiveArray<bool> m_kIsActive;

    NiString m_kName;
    bool m_bActive;
    bool m_bHaveGamePadAxis[NiInputGamePad::NIGP_NUM_DEFAULT_AXIS];
    bool m_bHaveMouseMotion;
    bool m_bHaveMouseZAxis;

    float m_fMouseScalar;
    float m_fMouseAccelPow;
    float m_fMouseAccelMul;
    float m_fGamePadAccelPow;
    float m_fGamePadAccelMul;
};
NiSmartPointer(NiNavBaseController);
//---------------------------------------------------------------------------
#include "NiNavBaseController.inl"
//---------------------------------------------------------------------------

#endif
