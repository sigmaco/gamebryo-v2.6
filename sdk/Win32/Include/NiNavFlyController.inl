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
#include <NiMath.h>
#include "NiNavManager.h"

//---------------------------------------------------------------------------
inline void NiNavFlyController::LookPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(LOOK_DOLLY) || !IsActive())
        return;

    m_afDeltaValues[LOOK_DOLLY] += ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::LookNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(LOOK_DOLLY) || !IsActive())
        return;

    m_afDeltaValues[LOOK_DOLLY] -= ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::PanRLPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_RIGHT_LEFT) || !IsActive())
        return;

    m_afDeltaValues[PAN_RIGHT_LEFT] += ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::PanRLNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_RIGHT_LEFT) || !IsActive())
        return;

    m_afDeltaValues[PAN_RIGHT_LEFT] -= ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::PanUDPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_UP_DOWN) || !IsActive())
        return;

    m_afDeltaValues[PAN_UP_DOWN] += ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::PanUDNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_UP_DOWN) || !IsActive())
        return;

    m_afDeltaValues[PAN_UP_DOWN] -= ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::YawPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(YAW_LOOK) || !IsActive())
        return;

    m_afDeltaValues[YAW_LOOK] += ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::YawNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(YAW_LOOK) || !IsActive())
        return;

    m_afDeltaValues[YAW_LOOK] -= ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::PitchPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PITCH_LOOK) || !IsActive())
        return;

    m_afDeltaValues[PITCH_LOOK] += ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::PitchNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PITCH_LOOK) || !IsActive())
        return;

    m_afDeltaValues[PITCH_LOOK] -= ms_fPosChangeScale * m_fScale * 
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::ScalePosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(SCALE_CHANGE) || !IsActive())
        return;

    m_afDeltaValues[SCALE_CHANGE] = 1.0f / NiPow(ms_fScaleChangeRate,
        NiNavManager::GetNavManager()->GetFrameTime());
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::ScaleNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(SCALE_CHANGE) || !IsActive())
        return;

    m_afDeltaValues[SCALE_CHANGE] = NiPow(ms_fScaleChangeRate, 
        NiNavManager::GetNavManager()->GetFrameTime());
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetControlledObject(
    NiAVObject* pkControlledObject)
{
    m_spControlledObject = pkControlledObject;

    if (m_spControlledObject == NULL)
    {
        NiOutputDebugString("Input Controlled Object to Fly "
            "Nav Controller was NULL.  This leads to faulty results.\n");
        NiNavBaseController::NavState kNavState;
        kNavState.bIgnoreThis = true;
        if (m_bActive)
            SetActive(false, kNavState);
    }

    if (m_bActive)
        CreateLookAtPt();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetReferenceObject(
    NiAVObject* pkReferenceObject)
{
    m_spReferenceObject = pkReferenceObject;

    if (m_spReferenceObject == NULL)
    {
        NiOutputDebugString("Input Reference Object to Fly "
            "Nav Controller was NULL.  This leads to faulty results.\n");
        NiNavBaseController::NavState kNavState;
        kNavState.bIgnoreThis = true;
        if (m_bActive)
            SetActive(false, kNavState);
    }
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetUpDir(const NiPoint3& kNewUpDir)
{
    m_kUpVector = kNewUpDir;
    if (m_kUpVector.Length() != 0.0f)
        m_kUpVector.Unitize();
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::CreateLookAtPt()
{
    NiPoint3 kLookDir;
    m_spControlledObject->GetWorldRotate().GetCol(0, kLookDir);
    m_kLookAtPt = m_spControlledObject->GetWorldTranslate() + 
        (m_fScale * kLookDir);
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetControlActive(bool bActive, 
    Controls eControlId)
{
    NIASSERT(eControlId >= 0);
    NIASSERT(eControlId < FLY_NUM_CONTROLS);
    m_kIsActive.SetAt((unsigned int)eControlId, bActive);
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetScale(float fScale)
{
    if (fScale < ms_fMinScale)
        m_fScale = ms_fMinScale;
    else
        m_fScale = fScale;
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetFlipPitch(bool bFlipPitch)
{
    m_fFlipPitch = bFlipPitch ? -1.0f : 1.0f;
}
//---------------------------------------------------------------------------
inline bool NiNavFlyController::GetFlipPitch()
{
    return m_fFlipPitch == -1.0f;
}
//---------------------------------------------------------------------------
inline void NiNavFlyController::SetFlipYaw(bool bFlipYaw)
{
    m_fFlipYaw = bFlipYaw ? -1.0f : 1.0f;
}
//---------------------------------------------------------------------------
inline bool NiNavFlyController::GetFlipYaw()
{
    return m_fFlipYaw == -1.0f;
}
//---------------------------------------------------------------------------
