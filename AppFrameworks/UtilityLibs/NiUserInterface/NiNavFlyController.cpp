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

// Precompiled Header
#include "NiUserInterfacePCH.h"

#include "NiNavFlyController.h"
#include <NiNode.h>
//---------------------------------------------------------------------------
const char* NiNavFlyController::ms_pcElementName[FLY_NUM_CONTROLS] = {
    "Change Motion Scale",
    "Rotate Horizontally",
    "Rotate Vertically",
    "Fly Horizontally",
    "Fly Vertically",
    "Fly Forward",
};
const float NiNavFlyController::ms_fPosChangeScale = 1.5f;
const float NiNavFlyController::ms_fScaleChangeRate = 0.7f;
const float NiNavFlyController::ms_fMinScale = (float)0.00001f;
const float NiNavFlyController::ms_fAngleChangeRate = 2.0f;
//---------------------------------------------------------------------------
NiNavFlyController::NiNavFlyController(
    NiAVObject* pkReferenceObject, 
    NiAVObject* pkControlledObject, 
    const char* pcName, 
    float fScale, 
    NiPoint3 kUpVector, 
    bool bFlipPitch, 
    bool bFlipYaw) :
    NiNavBaseController(pcName),
    m_kUpVector(kUpVector),
    m_kLookAtPt(NiPoint3::ZERO),
    m_spReferenceObject(pkReferenceObject), 
    m_spControlledObject(pkControlledObject), 
    m_fScale(fScale)
{
    m_kSlots.SetSize(FLY_NUM_CONTROLS * 2);
    m_kBindings.SetSize(FLY_NUM_CONTROLS * 2);
    m_kIsActive.SetSize(FLY_NUM_CONTROLS);
    unsigned int uiIdx;
    NiUIBaseSlot1<unsigned char>* pkSlot;

    {
        InputBinding kKeys;
        uiIdx = GetIdx(LOOK_DOLLY, true);
        kKeys.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_W;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_VERT;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::LookPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(LOOK_DOLLY, false);
        kKeys.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_S;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_VERT;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::LookNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(PAN_RIGHT_LEFT, true);
        kKeys.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_D;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::PanRLPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(PAN_RIGHT_LEFT, false);
        kKeys.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_A;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::PanRLNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(PAN_UP_DOWN, true);
#ifdef WIN32 // NiInput does not currently handle L2 R2 on win32
        kKeys.ucListenTypes = (InputKeyboard);
#else
        kKeys.ucListenTypes = (InputKeyboard | InputGamePadButton);
        kKeys.eGamePadButton = NiInputGamePad::NIGP_L2;
#endif
#if defined(_WII)
        // On the Wii, allow L2/R2 in combination with RRIGHT (the
        // B trigger button), so that when in navigation mode with the
        // Wii Remote, Up/Down navigation also works.
        kKeys.eGamePadOptionalModifier = 
            NiInputGamePad::NIGP_MASK_RRIGHT;
#endif
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_R;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::PanUDPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(PAN_UP_DOWN, false);
#ifdef WIN32 // NiInput does not currently handle L2 R2 on win32
        kKeys.ucListenTypes = (InputKeyboard);
#else
        kKeys.ucListenTypes = (InputKeyboard | InputGamePadButton);
        kKeys.eGamePadButton = NiInputGamePad::NIGP_R2;
#endif
#if defined(_WII)
        // On the Wii, allow L2/R2 in combination with RRIGHT (the
        // B trigger button), so that when in navigation mode with the
        // Wii Remote, Up/Down navigation also works.
        kKeys.eGamePadOptionalModifier = 
            NiInputGamePad::NIGP_MASK_RRIGHT;
#endif
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_F;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::PanUDNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(YAW_LOOK, true);
        kKeys.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKeys.eMouseAxis = NiInputMouse::NIM_AXIS_X;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::YawPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(YAW_LOOK, false);
        kKeys.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKeys.eMouseAxis = NiInputMouse::NIM_AXIS_X;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::YawNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(PITCH_LOOK, true);
        kKeys.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKeys.eMouseAxis = NiInputMouse::NIM_AXIS_Y;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::PitchPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(PITCH_LOOK, false);
        kKeys.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKeys.eMouseAxis = NiInputMouse::NIM_AXIS_Y;
        kKeys.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::PitchNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(SCALE_CHANGE, true);
        kKeys.ucListenTypes = (InputKeyboard | InputMouseAxis | 
            InputGamePadButton);
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_EQUALS;
        kKeys.eMouseAxis = NiInputMouse::NIM_AXIS_Z;
        kKeys.eGamePadButton = NiInputGamePad::NIGP_R1;
        m_kBindings.SetAt(uiIdx, kKeys);
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::ScalePosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKeys;
        uiIdx = GetIdx(SCALE_CHANGE, false);
        kKeys.ucListenTypes = (InputKeyboard | InputMouseAxis | 
            InputGamePadButton);
        kKeys.eKeyboardKey = NiInputKeyboard::KEY_MINUS;
        kKeys.eMouseAxis = NiInputMouse::NIM_AXIS_Z;
        kKeys.eGamePadButton = NiInputGamePad::NIGP_L1;
        pkSlot = NiNew NiUIMemberSlot1<NiNavFlyController, unsigned char>(
            this, &NiNavFlyController::ScaleNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
        m_kBindings.SetAt(uiIdx, kKeys);
    }

    for (uiIdx = 0; uiIdx < FLY_NUM_CONTROLS; ++uiIdx)
    {
        m_afDeltaValues[uiIdx] = 0.0f;
        m_kIsActive.SetAt(uiIdx, true);
    }
    m_afDeltaValues[SCALE_CHANGE] = 1.0f;

    if (m_fScale <= 0.0f)
    {
        NiOutputDebugString("Scale input to NiNavFlyController is less "
            "than or equal to zero.  Value greater than zero applied.\n");
        m_fScale = 0.25f;
    }
    if ((m_spControlledObject == NULL) || (m_spReferenceObject == NULL))
    {
        NiOutputDebugString("Input Controlled and/or Reference Object to Fly "
            "Nav Controller was NULL.  This leads to faulty results.\n");
    }

    m_fFlipPitch = bFlipPitch ? -1.0f : 1.0f;
    m_fFlipYaw = bFlipYaw ? -1.0f : 1.0f;
}
//---------------------------------------------------------------------------
NiNavFlyController::~NiNavFlyController()
{
    m_spReferenceObject = NULL;
    m_spControlledObject = NULL;
}
//---------------------------------------------------------------------------
void NiNavFlyController::ResetKeyboardHotkey(Controls eControlId, 
    bool bPositiveDir, NiInputKeyboard::KeyCode eKey, 
    NiInputKeyboard::Modifiers eModifier)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kKey;
    unsigned int uiIdx = GetIdx(eControlId, bPositiveDir);
    kKey = m_kBindings.GetAt(uiIdx);
    kKey.ucListenTypes |= InputKeyboard;
    kKey.eKeyboardKey = eKey;
    kKey.eKeyboardModifier = eModifier;
    m_kBindings.SetAt(uiIdx, kKey);
}
//---------------------------------------------------------------------------
void NiNavFlyController::ResetMouseHotkey(Controls eControlId, 
    bool bPositiveDir, NiInputMouse::Button eButton, 
    NiInputMouse::Modifiers eModifier)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kKey;
    unsigned int uiIdx = GetIdx(eControlId, bPositiveDir);
    // The first line is necessary because you can't set an action to listen
    // on an axis and a hotkey at once.
    kKey = m_kBindings.GetAt(uiIdx);
    kKey.ucListenTypes &= ~((unsigned char)InputMouseAxis);
    kKey.ucListenTypes |= InputMouseButton;
    kKey.eMouseButton = eButton;
    kKey.eMouseModifier = eModifier;
    m_kBindings.SetAt(uiIdx, kKey);
}
//---------------------------------------------------------------------------
void NiNavFlyController::ResetMouseAxis(Controls eControlId, 
    NiInputMouse::Axes eMouseAxis)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kPosKey, kNegKey;
    unsigned int uiPosIdx = GetIdx(eControlId, true);
    unsigned int uiNegIdx = GetIdx(eControlId, false);
    // Now to make sure that they don't listen to mouse buttons:
    kPosKey = m_kBindings.GetAt(uiPosIdx);
    kNegKey = m_kBindings.GetAt(uiNegIdx);
    kPosKey.ucListenTypes &= ~((unsigned char)InputMouseButton);
    kNegKey.ucListenTypes &= ~((unsigned char)InputMouseButton);
    // Now to make sure that they listen to mouse axis:
    kPosKey.ucListenTypes |= InputMouseAxis;
    kNegKey.ucListenTypes |= InputMouseAxis;
    kPosKey.eMouseAxis = eMouseAxis;
    kNegKey.eMouseAxis = eMouseAxis;
    m_kBindings.SetAt(uiPosIdx, kPosKey);
    m_kBindings.SetAt(uiNegIdx, kNegKey);
}
//---------------------------------------------------------------------------
void NiNavFlyController::ResetGamePadHotkey(Controls eControlId, 
    bool bPositiveDir, NiInputGamePad::Button eButton, 
    NiInputGamePad::ButtonMask eModifier)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kKey;
    unsigned int uiIdx = GetIdx(eControlId, bPositiveDir);
    kKey = m_kBindings.GetAt(uiIdx);
    // The first line makes sure that it doesn't listen to GamePad Axes
    kKey.ucListenTypes &= ~((unsigned char)InputGamePadAxis);
    kKey.ucListenTypes |= InputGamePadButton;
    kKey.eGamePadButton = eButton;
    kKey.eGamePadModifier = eModifier;
    m_kBindings.SetAt(uiIdx, kKey);
}
//---------------------------------------------------------------------------
void NiNavFlyController::ResetGamePadAxis(Controls eControlId, 
    NiInputGamePad::DefaultAxis eAxis)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kPosKey, kNegKey;
    unsigned int uiPosIdx = GetIdx(eControlId, true);
    unsigned int uiNegIdx = GetIdx(eControlId, false);
    kPosKey = m_kBindings.GetAt(uiPosIdx);
    kNegKey = m_kBindings.GetAt(uiNegIdx);
    // Now make sure that they don't listen to GamePad buttons
    kPosKey.ucListenTypes &= ~((unsigned char)InputGamePadButton);
    kNegKey.ucListenTypes &= ~((unsigned char)InputGamePadButton);
    // Now to make sure that they listen to GamePad axis
    kPosKey.ucListenTypes |= InputGamePadAxis;
    kNegKey.ucListenTypes |= InputGamePadAxis;
    kPosKey.eGamePadAxis = eAxis;
    kNegKey.eGamePadAxis = eAxis;
    m_kBindings.SetAt(uiPosIdx, kPosKey);
    m_kBindings.SetAt(uiNegIdx, kNegKey);
}
//---------------------------------------------------------------------------
inline const char* NiNavFlyController::GetControlName(
    unsigned int uiControlId) const
{
    NIASSERT(uiControlId < FLY_NUM_CONTROLS);
    return ms_pcElementName[uiControlId];
}
//---------------------------------------------------------------------------
void NiNavFlyController::MouseInput(unsigned int uiControlId, float fDelta)
{
    NIASSERT(uiControlId < FLY_NUM_CONTROLS);
    if (!m_kIsActive.GetAt(uiControlId) || !IsActive())
        return;

    switch (uiControlId)
    {
    case (LOOK_DOLLY):
        m_afDeltaValues[LOOK_DOLLY] += ms_fPosChangeScale * m_fScale * fDelta *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_RIGHT_LEFT):
        m_afDeltaValues[PAN_RIGHT_LEFT] += ms_fPosChangeScale * m_fScale * 
            fDelta *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (YAW_LOOK):
        m_afDeltaValues[YAW_LOOK] += fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PITCH_LOOK):
        m_afDeltaValues[PITCH_LOOK] += fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (SCALE_CHANGE):
        if (fDelta > 0.0f)
        {
            m_afDeltaValues[SCALE_CHANGE] = NiPow(
                ms_fScaleChangeRate, fDelta *
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        else if (fDelta < 0.0f)
        {
            m_afDeltaValues[SCALE_CHANGE] = 1.0f / NiPow(
                ms_fScaleChangeRate, -fDelta *
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        break;
    };
}
//---------------------------------------------------------------------------
void NiNavFlyController::GamePadInput(unsigned int uiControlId, float fDelta)
{
    NIASSERT(uiControlId < FLY_NUM_CONTROLS);
    if (!m_kIsActive.GetAt(uiControlId) || !IsActive())
        return;

    switch (uiControlId)
    {
    case (LOOK_DOLLY):
        m_afDeltaValues[LOOK_DOLLY] -= ms_fPosChangeScale * m_fScale * fDelta *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_RIGHT_LEFT):
        m_afDeltaValues[PAN_RIGHT_LEFT] += ms_fPosChangeScale * m_fScale * 
            fDelta * NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_UP_DOWN):
        m_afDeltaValues[PAN_UP_DOWN] += ms_fPosChangeScale * m_fScale * 
            fDelta * NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (YAW_LOOK):
        m_afDeltaValues[YAW_LOOK] += fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PITCH_LOOK):
        m_afDeltaValues[PITCH_LOOK] += fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (SCALE_CHANGE):
        if (fDelta > 0.0f)
        {
            m_afDeltaValues[SCALE_CHANGE] = NiPow(
                ms_fScaleChangeRate, fDelta *
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        else if (fDelta < 0.0f)
        {
            m_afDeltaValues[SCALE_CHANGE] = 1.0f / NiPow(
                ms_fScaleChangeRate, -fDelta *
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        break;
    };
}
//---------------------------------------------------------------------------
void NiNavFlyController::UpdatePose()
{
    NiPoint3 kRefWorldUp, kRefWorldRight, kRefWorldForward;
    NiPoint3 kContWorldUp, kContWorldRight, kContWorldForward, kCurPos;
    GetFrameFromObject(m_spReferenceObject, kRefWorldForward,
        kRefWorldRight, kRefWorldUp);
    GetFrameFromObject(m_spControlledObject, kContWorldForward,
        kContWorldRight, kContWorldUp);

    NiTransform kContTransform = m_spControlledObject->GetWorldTransform();
    kCurPos = kContTransform.m_Translate;

    // First do any "dolly-ing" necessary
    m_fScale = NiMax(m_fScale * m_afDeltaValues[SCALE_CHANGE], ms_fMinScale);
    NiPoint3 kDir = m_kLookAtPt - kCurPos;
    kDir.Unitize();
    TranslateAlongAxis(kDir, m_fScale, kCurPos, m_kLookAtPt);   

    // Then, handle all translations:
    TranslateAlongAxis(kRefWorldForward, m_afDeltaValues[LOOK_DOLLY], 
        kCurPos, kCurPos);
    TranslateAlongAxis(kRefWorldForward, m_afDeltaValues[LOOK_DOLLY], 
        m_kLookAtPt, m_kLookAtPt);
    TranslateAlongAxis(kRefWorldRight, m_afDeltaValues[PAN_RIGHT_LEFT], 
        kCurPos, kCurPos);
    TranslateAlongAxis(kRefWorldRight, m_afDeltaValues[PAN_RIGHT_LEFT], 
        m_kLookAtPt, m_kLookAtPt);
    TranslateAlongAxis(kRefWorldUp, m_afDeltaValues[PAN_UP_DOWN], 
        kCurPos, kCurPos);
    TranslateAlongAxis(kRefWorldUp, m_afDeltaValues[PAN_UP_DOWN], 
        m_kLookAtPt, m_kLookAtPt);

    // Then handle the look around movement
    NiMatrix3 kCurrentRotation, kYawRotation, kPitchRotation;
    kCurrentRotation = kContTransform.m_Rotate;
    kPitchRotation.MakeRotation(m_afDeltaValues[PITCH_LOOK] * m_fFlipPitch, 
        kRefWorldRight);    

    if(m_kUpVector != NiPoint3::ZERO)
    {
        kYawRotation.MakeRotation(m_afDeltaValues[YAW_LOOK] * m_fFlipYaw, 
            m_kUpVector);
    }
    else
    {
        kYawRotation.MakeRotation(m_afDeltaValues[YAW_LOOK] * m_fFlipYaw, 
            kRefWorldUp);
    }
    
    NiMatrix3 kNewRotation = kYawRotation * kPitchRotation * kCurrentRotation;
    kNewRotation.Reorthogonalize();

    if(m_kUpVector != NiPoint3::ZERO)
    {
        NiPoint3 kUpDir;
        kNewRotation.GetCol(1, kUpDir);
        
        // Check to see if new rotation's pitch causes the object to be 
        // upside-down, if so then remove pitch from rotation calculations.
        if(m_kUpVector.Dot(kUpDir) <= 0) 
        {
            kNewRotation = kYawRotation * kCurrentRotation;
            kNewRotation.Reorthogonalize();
        }

        // Adjust rotation matrix to obey up vector constraint
        NiPoint3 kNewForward, kNewRight, kNewUp;
        kNewRotation.GetCol(0, kNewForward);

        kNewRight = kNewForward.UnitCross(m_kUpVector);
        kNewUp = kNewRight.UnitCross(kNewForward);

        kNewRotation = NiMatrix3(kNewForward, kNewUp, kNewRight);
    }

    // Adjust controlled object's world transform
    kContTransform.m_Rotate = kNewRotation;
    kContTransform.m_Translate = kCurPos;
    m_spControlledObject->SetLocalFromWorldTransform(kContTransform);

    // Now reset the values for next time
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < FLY_NUM_CONTROLS; ++uiIdx)
    {
        m_afDeltaValues[uiIdx] = 0.0f;
    }
    m_afDeltaValues[SCALE_CHANGE] = 1.0f;
}
//---------------------------------------------------------------------------
bool NiNavFlyController::SetActive(bool bActive, NavState& kCurState)
{
    bool bRetVal = NiNavBaseController::SetActive(bActive, kCurState);

    if (bActive)
    {
        if (!kCurState.bIgnoreThis)
            m_fScale = kCurState.fScale;

        if (m_spControlledObject)
            CreateLookAtPt();
    }
    else
    {
        kCurState.fScale = m_fScale;
        kCurState.pkReferenceObject = m_spReferenceObject;
    }

    return bRetVal;
}
//---------------------------------------------------------------------------
