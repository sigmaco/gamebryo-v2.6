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

#include "NiNavOrbitController.h"
#include "NiUIManager.h"
#include <NiNode.h>
//---------------------------------------------------------------------------
const char* NiNavOrbitController::ms_pcElementName[ORBIT_NUM_CONTROLS] = {
    "Orbit Radius",
    "Orbit Horizontally",
    "Orbit Vertically",
    "Pan Horizontally",
    "Pan Vertically",
    "Fly Forward",
};
const float NiNavOrbitController::ms_fRadiusChangeRate = 0.6f;
const float NiNavOrbitController::ms_fAngleChangeRate = 2.0f;
const float NiNavOrbitController::ms_fPanChangeRate = 1.5f;
const float NiNavOrbitController::ms_fMinRadius = (float)0.00001f;
//---------------------------------------------------------------------------
NiNavOrbitController::NiNavOrbitController(NiAVObject* pkObject, 
    const char* pcName, float fRadius, NiPoint3 kUpVector) :
    NiNavBaseController(pcName),
    m_spControlledObject(pkObject)
{
    m_kBindings.SetSize(ORBIT_NUM_CONTROLS * 2);
    m_kIsActive.SetSize(ORBIT_NUM_CONTROLS);
    m_kSlots.SetSize(ORBIT_NUM_CONTROLS * 2);

    NiUIBaseSlot1<unsigned char>* pkSlot;

    // Set these to "industry standards" as default
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(RADIUS_DOLLY, true);
        kKey.ucListenTypes = (InputKeyboard | InputMouseAxis | 
            InputGamePadButton);
        kKey.eMouseAxis = NiInputMouse::NIM_AXIS_Z;
        kKey.eKeyboardKey = NiInputKeyboard::KEY_EQUALS;// +, really
        kKey.eGamePadButton = NiInputGamePad::NIGP_R1;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::RadiusPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(RADIUS_DOLLY, false);
        kKey.ucListenTypes = (InputKeyboard | InputMouseAxis | 
            InputGamePadButton);
        kKey.eMouseAxis = NiInputMouse::NIM_AXIS_Z;
        kKey.eKeyboardKey = NiInputKeyboard::KEY_MINUS;
        kKey.eGamePadButton = NiInputGamePad::NIGP_L1;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::RadiusNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(LATITUDE_ORBIT, true);
        kKey.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKey.eMouseAxis = NiInputMouse::NIM_AXIS_X;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::LatitudePosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(LATITUDE_ORBIT, false);
        kKey.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKey.eMouseAxis = NiInputMouse::NIM_AXIS_X;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::LatitudeNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(LONGITUDE_ORBIT, true);
        kKey.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKey.eMouseAxis = NiInputMouse::NIM_AXIS_Y;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::LongitudePosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(LONGITUDE_ORBIT, false);
        kKey.ucListenTypes = (InputMouseAxis | InputGamePadAxis);
        kKey.eMouseAxis = NiInputMouse::NIM_AXIS_Y;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::LongitudeNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(PAN_RIGHT_LEFT, true);
        kKey.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKey.eKeyboardKey = NiInputKeyboard::KEY_D;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::PanRLPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(PAN_RIGHT_LEFT, false);
        kKey.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKey.eKeyboardKey = NiInputKeyboard::KEY_A;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::PanRLNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(PAN_UP_DOWN, true);
#ifdef WIN32 // NiInput does not currently handle L2 R2 on win32
        kKey.ucListenTypes = (InputKeyboard);
#else
        kKey.ucListenTypes = (InputKeyboard | InputGamePadButton);
        kKey.eGamePadButton = NiInputGamePad::NIGP_L2;
#endif
#if defined(_WII)
        // On the Wii, allow L2/R2 in combination with RRIGHT (the
        // B trigger button), so that when in navigation mode with the
        // Wii Remote, Up/Down navigation also works.
        kKey.eGamePadOptionalModifier = 
            NiInputGamePad::NIGP_MASK_RRIGHT;
#endif
        kKey.eKeyboardKey = NiInputKeyboard::KEY_R;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::PanUDPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(PAN_UP_DOWN, false);
#ifdef WIN32 // NiInput does not currently handle L2 R2 on win32
        kKey.ucListenTypes = (InputKeyboard);
#else
        kKey.ucListenTypes = (InputKeyboard | InputGamePadButton);
        kKey.eGamePadButton = NiInputGamePad::NIGP_R2;
#endif
#if defined(_WII)
        // On the Wii, allow L2/R2 in combination with RRIGHT (the
        // B trigger button), so that when in navigation mode with the
        // Wii Remote, Up/Down navigation also works.
        kKey.eGamePadOptionalModifier = 
            NiInputGamePad::NIGP_MASK_RRIGHT;
#endif
        kKey.eKeyboardKey = NiInputKeyboard::KEY_F;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::PanUDNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(FLY, true);
        kKey.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKey.eKeyboardKey = NiInputKeyboard::KEY_W;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_VERT;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::FlyPosChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }
    {
        InputBinding kKey;
        unsigned int uiIdx = GetIdx(FLY, false);
        kKey.ucListenTypes = (InputKeyboard | InputGamePadAxis);
        kKey.eKeyboardKey = NiInputKeyboard::KEY_S;
        kKey.eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_VERT;
        m_kBindings.SetAt(uiIdx, kKey);
        pkSlot = NiNew NiUIMemberSlot1<NiNavOrbitController, unsigned char>(
            this, &NiNavOrbitController::FlyNegChange);
        m_kSlots.SetAt(uiIdx, pkSlot);
    }

    for (unsigned int uiIdx = 0; uiIdx < ORBIT_NUM_CONTROLS; ++uiIdx)
    {
        m_afDeltaValues[uiIdx] = 0.0f;
        m_kIsActive.SetAt(uiIdx, true);
    }
    m_afDeltaValues[RADIUS_DOLLY] = 1.0f;
    // This will be set properly later
    m_kCenterPt = NiPoint3::ZERO;

    m_kUpVector = kUpVector;
    if (m_kUpVector.Length() != 0)
        m_kUpVector.Unitize();

    if (fRadius <= 0)
    {
        NiOutputDebugString("Radius input to NiNavOrbitController is less "
            "than or equal to zero.  Value greater than zero applied.\n");
        m_fRadius = 0.25f;
    }
    else
    {
        m_fRadius = fRadius;
    }

    if (m_spControlledObject == NULL)
    {
        NiOutputDebugString("Input Controlled Object to Orbit Nav Controller "
            "was NULL.  This leads to faulty results.\n");
    }
}
//---------------------------------------------------------------------------
NiNavOrbitController::~NiNavOrbitController()
{
    m_spControlledObject = NULL;
}
//---------------------------------------------------------------------------
void NiNavOrbitController::ResetKeyboardHotkey(Controls eControlId, 
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
void NiNavOrbitController::ResetMouseHotkey(Controls eControlId, 
    bool bPositiveDir, NiInputMouse::Button eButton, 
    NiInputMouse::Modifiers eModifier)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kKey;
    unsigned int uiIdx = GetIdx(eControlId, bPositiveDir);
    kKey = m_kBindings.GetAt(uiIdx);
    // The first line is necessary because you can't set an action to listen
    // on an axis and a hotkey at once.
    kKey.ucListenTypes &= ~((unsigned char)InputMouseAxis);
    kKey.ucListenTypes |= InputMouseButton;
    kKey.eMouseButton = eButton;
    kKey.eMouseModifier = eModifier;
    m_kBindings.SetAt(uiIdx, kKey);
}
//---------------------------------------------------------------------------
void NiNavOrbitController::ResetMouseAxis(Controls eControlId, 
    NiInputMouse::Axes eMouseAxis)
{
    // You can't switch hotkeys while the NavController is active
    if (m_bActive)
        return;

    InputBinding kPosKey, kNegKey;
    unsigned int uiPosIdx = GetIdx(eControlId, true);
    unsigned int uiNegIdx = GetIdx(eControlId, false);
    kPosKey = m_kBindings.GetAt(uiPosIdx);
    kNegKey = m_kBindings.GetAt(uiNegIdx);
    // Now to make sure that they don't listen to mouse buttons:
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
void NiNavOrbitController::ResetGamePadHotkey(Controls eControlId, 
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
void NiNavOrbitController::ResetGamePadAxis(Controls eControlId, 
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
inline const char* NiNavOrbitController::GetControlName(
    unsigned int uiControlId) const
{
    NIASSERT(uiControlId < ORBIT_NUM_CONTROLS);
    return ms_pcElementName[uiControlId];
}
//---------------------------------------------------------------------------
void NiNavOrbitController::MouseInput(unsigned int uiControlId, float fDelta)
{
    NIASSERT(uiControlId < ORBIT_NUM_CONTROLS);
    if (!m_kIsActive.GetAt(uiControlId) || !IsActive())
        return;

    switch (uiControlId)
    {
    case (RADIUS_DOLLY):
        if (fDelta > 0.0f)
        {
            m_afDeltaValues[RADIUS_DOLLY] = NiPow(
                ms_fRadiusChangeRate, fDelta * 
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        else if (fDelta < 0.0f)
        {
            m_afDeltaValues[RADIUS_DOLLY] = 1.0f / NiPow(
                ms_fRadiusChangeRate, -fDelta * 
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        break;
    case (LATITUDE_ORBIT):
        m_afDeltaValues[LATITUDE_ORBIT] -= fDelta * ms_fAngleChangeRate * 
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (LONGITUDE_ORBIT):
        m_afDeltaValues[LONGITUDE_ORBIT] += fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_RIGHT_LEFT):
        m_afDeltaValues[PAN_RIGHT_LEFT] += fDelta * ms_fPanChangeRate * 
            m_fRadius * 
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_UP_DOWN):
        m_afDeltaValues[PAN_UP_DOWN] += fDelta * ms_fPanChangeRate * 
            m_fRadius *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (FLY):
        m_afDeltaValues[FLY] += fDelta * ms_fPanChangeRate * 
            m_fRadius *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    }
}
//---------------------------------------------------------------------------
void NiNavOrbitController::GamePadInput(unsigned int uiControlId, float fDelta)
{
    NIASSERT(uiControlId < ORBIT_NUM_CONTROLS);
    if (!m_kIsActive.GetAt(uiControlId) || !IsActive())
        return;

    switch (uiControlId)
    {
    case (RADIUS_DOLLY):
        if (fDelta > 0.0f)
        {
            m_afDeltaValues[RADIUS_DOLLY] = 1.0f / NiPow(
                ms_fRadiusChangeRate, fDelta * 
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        else if (fDelta < 0.0f)
        {
            m_afDeltaValues[RADIUS_DOLLY] = NiPow(
                ms_fRadiusChangeRate, -fDelta * 
                NiNavManager::GetNavManager()->GetFrameTime());
        }
        break;
    case (LATITUDE_ORBIT):
        m_afDeltaValues[LATITUDE_ORBIT] += fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (LONGITUDE_ORBIT):
        m_afDeltaValues[LONGITUDE_ORBIT] -= fDelta * ms_fAngleChangeRate *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_RIGHT_LEFT):
        m_afDeltaValues[PAN_RIGHT_LEFT] += fDelta * ms_fPanChangeRate * 
            m_fRadius * 
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (PAN_UP_DOWN):
        m_afDeltaValues[PAN_UP_DOWN] -= fDelta * ms_fPanChangeRate * 
            m_fRadius *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    case (FLY):
        m_afDeltaValues[FLY] -= fDelta * ms_fPanChangeRate * 
            m_fRadius *
            NiNavManager::GetNavManager()->GetFrameTime();
        break;
    }
}
//---------------------------------------------------------------------------
void NiNavOrbitController::UpdatePose()
{
    NiPoint3 kWorldUp, kWorldRight, kWorldForward, kCurPos;
    GetFrameFromObject(m_spControlledObject, kWorldForward, kWorldRight, 
        kWorldUp);

    NiTransform kContTransform = m_spControlledObject->GetWorldTransform();
    kCurPos = kContTransform.m_Translate;

    // First, do any "dolly-ing" necessary
    m_fRadius = NiMax(m_fRadius * m_afDeltaValues[RADIUS_DOLLY], 
        ms_fMinRadius);
    NiPoint3 kDir = kCurPos - m_kCenterPt;
    kDir.Unitize();
    TranslateAlongAxis(kDir, m_fRadius, m_kCenterPt, kCurPos);

    // Then, do any panning
    // Both need to be moved because we are not only panning the object
    // but also the center
    TranslateAlongAxis(kWorldRight, m_afDeltaValues[PAN_RIGHT_LEFT], 
        m_kCenterPt, m_kCenterPt);
    TranslateAlongAxis(kWorldUp, m_afDeltaValues[PAN_UP_DOWN],
        m_kCenterPt, m_kCenterPt);
    TranslateAlongAxis(kWorldForward, m_afDeltaValues[FLY],
        m_kCenterPt, m_kCenterPt);
    TranslateAlongAxis(kWorldRight, m_afDeltaValues[PAN_RIGHT_LEFT],
        kCurPos, kCurPos);
    TranslateAlongAxis(kWorldUp, m_afDeltaValues[PAN_UP_DOWN],
        kCurPos, kCurPos);
    TranslateAlongAxis(kWorldForward, m_afDeltaValues[FLY],
        kCurPos, kCurPos);

    // Then orbit around the center
    NiMatrix3 kCurrentRotation, kYawRotation, kPitchRotation;
    kCurrentRotation = kContTransform.m_Rotate;
    kPitchRotation.MakeRotation(
        m_afDeltaValues[LONGITUDE_ORBIT], 
        kWorldRight);

    if(m_kUpVector != NiPoint3::ZERO)
    {
        kYawRotation.MakeRotation(
            -m_afDeltaValues[LATITUDE_ORBIT], 
            m_kUpVector);
    }
    else
    {
        kYawRotation.MakeRotation(
            -m_afDeltaValues[LATITUDE_ORBIT], 
            kWorldUp);
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

    // Finally set the new translation value
    NiPoint3 kNewPos = 
        kNewRotation * (NiPoint3::UNIT_X * -m_fRadius) + m_kCenterPt;

    // Adjust controlled object's world transform
    kContTransform.m_Rotate = kNewRotation;
    kContTransform.m_Translate = kNewPos;
    m_spControlledObject->SetLocalFromWorldTransform(kContTransform);

    // Then reset values for the next pass
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < ORBIT_NUM_CONTROLS; ++uiIdx)
    {
        m_afDeltaValues[uiIdx] = 0.0f;
    }
    m_afDeltaValues[RADIUS_DOLLY] = 1.0f;
}
//---------------------------------------------------------------------------
bool NiNavOrbitController::SetActive(bool bActive, NavState& kCurState)
{
    bool bRetVal = NiNavBaseController::SetActive(bActive, kCurState);
    if (m_spControlledObject == NULL)
        return false;

    if (bActive)
    {
        if (!kCurState.bIgnoreThis)
            m_fRadius = kCurState.fScale;

        NiPoint3 kLookDir;
        m_spControlledObject->GetWorldRotate().GetCol(0, kLookDir);
        m_kCenterPt = m_spControlledObject->GetWorldTranslate() + 
            (m_fRadius * kLookDir);
    }
    else
    {
        kCurState.fScale = m_fRadius;
        kCurState.pkReferenceObject = m_spControlledObject;
    }

    return bRetVal;
}
//---------------------------------------------------------------------------

