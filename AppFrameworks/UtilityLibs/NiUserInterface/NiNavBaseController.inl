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

//---------------------------------------------------------------------------
inline NiNavBaseController::InputBinding::InputBinding()
{
    ucListenTypes = 0x00;
    eKeyboardKey = NiInputKeyboard::KEY_NOKEY;
    eKeyboardModifier = NiInputKeyboard::KMOD_NONE;
    eMouseButton = NiInputMouse::NIM_NONE;
    eMouseModifier = NiInputMouse::MMOD_NONE;
    eMouseAxis = NiInputMouse::NIM_AXIS_X;
    eGamePadButton = NiInputGamePad::NIGP_NONE;
    eGamePadModifier = NiInputGamePad::NIGP_MASK_NONE;
    eGamePadOptionalModifier = NiInputGamePad::NIGP_MASK_NONE;
    eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
}
//---------------------------------------------------------------------------
inline NiNavBaseController::InputBinding::InputBinding(
    NiNavBaseController::InputBinding* pkKey)
{
    if (pkKey == NULL)
    {
        ucListenTypes = 0x00;
        eKeyboardKey = NiInputKeyboard::KEY_NOKEY;
        eKeyboardModifier = NiInputKeyboard::KMOD_NONE;
        eMouseButton = NiInputMouse::NIM_NONE;
        eMouseModifier = NiInputMouse::MMOD_NONE;
        eMouseAxis = NiInputMouse::NIM_AXIS_X;
        eGamePadButton = NiInputGamePad::NIGP_NONE;
        eGamePadModifier = NiInputGamePad::NIGP_MASK_NONE;
        eGamePadOptionalModifier = NiInputGamePad::NIGP_MASK_NONE;
        eGamePadAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
    }
    else
    {
        ucListenTypes = pkKey->ucListenTypes;
        eKeyboardKey = pkKey->eKeyboardKey;
        eKeyboardModifier = pkKey->eKeyboardModifier;
        eMouseButton = pkKey->eMouseButton;
        eMouseModifier = pkKey->eMouseModifier;
        eMouseAxis = pkKey->eMouseAxis;
        eGamePadButton = pkKey->eGamePadButton;
        eGamePadModifier = pkKey->eGamePadModifier;
        eGamePadOptionalModifier = pkKey->eGamePadOptionalModifier;
        eGamePadAxis = pkKey->eGamePadAxis;
    }
}
//---------------------------------------------------------------------------
inline NiNavBaseController::InputBinding 
    NiNavBaseController::InputBinding::operator = (const InputBinding& kKey)
{
    ucListenTypes = kKey.ucListenTypes;
    eKeyboardKey = kKey.eKeyboardKey;
    eKeyboardModifier = kKey.eKeyboardModifier;
    eMouseButton = kKey.eMouseButton;
    eMouseModifier = kKey.eMouseModifier;
    eMouseAxis = kKey.eMouseAxis;
    eGamePadButton = kKey.eGamePadButton;
    eGamePadModifier = kKey.eGamePadModifier;
    eGamePadOptionalModifier = kKey.eGamePadOptionalModifier;
    eGamePadAxis = kKey.eGamePadAxis;

    return (*this);
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::operator != (
    const InputBinding& kKey) const
{
    return !((*this) == kKey);
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::operator == (
    const InputBinding& kKey) const
{
    return ((ucListenTypes == kKey.ucListenTypes) && 
        (eKeyboardKey == kKey.eKeyboardKey) && 
        (eKeyboardModifier == kKey.eKeyboardModifier) && 
        (eMouseButton == kKey.eMouseButton) && 
        (eMouseModifier == kKey.eMouseModifier) && 
        (eMouseAxis == kKey.eMouseAxis) && 
        (eGamePadButton == kKey.eGamePadButton) && 
        (eGamePadModifier == kKey.eGamePadModifier) && 
        (eGamePadOptionalModifier == kKey.eGamePadOptionalModifier) &&
        (eGamePadAxis == kKey.eGamePadAxis));
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::SupportsKeyboard() const
{
    return ucListenTypes & NiNavBaseController::InputKeyboard ? true : false;
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::SupportsMouseAxis() const
{
    return ucListenTypes & NiNavBaseController::InputMouseAxis ? true : false;
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::SupportsMouseButton() const
{
    return ucListenTypes & NiNavBaseController::InputMouseButton ? 
        true : false;
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::SupportsGamePadAxis() const
{
    return ucListenTypes & NiNavBaseController::InputGamePadAxis ? 
        true : false;
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::InputBinding::SupportsGamePadButton() const
{
    return ucListenTypes & NiNavBaseController::InputGamePadButton ? 
        true : false;
}
//---------------------------------------------------------------------------
inline unsigned int NiNavBaseController::GetIdx(unsigned int uiControlId, 
    bool bPositiveDir)
{
    NIASSERT(uiControlId < (GetControlCount()));
    unsigned int uiVal = (bPositiveDir) ? 1 : 0;
    return (uiControlId * 2) + uiVal;
}
//---------------------------------------------------------------------------
inline void NiNavBaseController::SetControlActive(unsigned int uiControlId, 
    bool bActive)
{
    NIASSERT(uiControlId < GetControlCount());
    m_kIsActive.SetAt(uiControlId, bActive);
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::IsActive() const
{
    return m_bActive;
}
//---------------------------------------------------------------------------
inline bool NiNavBaseController::IsControlActive(
    unsigned int uiControlId) const
{
    NIASSERT(uiControlId < GetControlCount());
    return m_kIsActive.GetAt(uiControlId);
}
//---------------------------------------------------------------------------
inline const NiString& NiNavBaseController::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline unsigned int NiNavBaseController::GetControlCount() const
{
    return m_kBindings.GetAllocatedSize() / 2;
}
//---------------------------------------------------------------------------
inline const NiNavBaseController::InputBinding& 
    NiNavBaseController::GetBinding(unsigned int uiControlId, 
    ControlType eType) const
{
    NIASSERT(uiControlId < GetControlCount());
    unsigned int uiIdx = (eType == POSITIVE_AXIS) ? 1 : 0;
    uiIdx = (uiControlId * 2) + uiIdx;
    return m_kBindings.GetAt(uiIdx);
}
//---------------------------------------------------------------------------
inline void NiNavBaseController::TranslateAlongAxis(const NiPoint3& kDir, 
    float fAmount, const NiPoint3& kStartPos, NiPoint3& kEndPos) const
{
    kEndPos = kStartPos + (kDir * fAmount);
}
//---------------------------------------------------------------------------
inline void NiNavBaseController::GetFrameFromObject(
    const NiAVObject* kFrameOwner, 
    NiPoint3& kForward, NiPoint3& kRight, NiPoint3& kUp) const
{
    NiMatrix3 kRefMat = kFrameOwner->GetWorldRotate();
    kRefMat.GetCol(0, kForward);
    kRefMat.GetCol(1, kUp);
    kRefMat.GetCol(2, kRight);
}
//---------------------------------------------------------------------------
inline void NiNavBaseController::CreateNewFrame(const NiPoint3& kOldForward, 
    const NiPoint3& kOldUp, float fRotUpAmt, const NiPoint3& kNewForward, 
    NiPoint3& kNewRight, NiPoint3& kNewUp) const
{
    // This makes sure that they aren't nearly coincident
    if (fabs(kNewForward.Dot(kOldUp)) < .95)
        kNewRight = kNewForward.UnitCross(kOldUp);
    else if (fRotUpAmt > 0)
        kNewRight = kOldForward.UnitCross(kNewForward);
    else
        kNewRight = kNewForward.UnitCross(kOldForward);
    // The elses above work because when the forward direction is 
    // nearly coincident with the old up direction, we've gone 90 degrees 
    // and that makes the new forward nearly perpendicular with the 
    // old forward!

    kNewUp = kNewRight.UnitCross(kNewForward);
}
//---------------------------------------------------------------------------
