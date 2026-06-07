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
inline void NiNavOrbitController::RadiusPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(RADIUS_DOLLY) || !IsActive())
        return;

    m_afDeltaValues[RADIUS_DOLLY] = 1.0f / NiPow(ms_fRadiusChangeRate, 
        NiNavManager::GetNavManager()->GetFrameTime());
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::RadiusNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(RADIUS_DOLLY) || !IsActive())
        return;

    m_afDeltaValues[RADIUS_DOLLY] = NiPow(ms_fRadiusChangeRate, 
        NiNavManager::GetNavManager()->GetFrameTime());
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::LatitudePosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(LATITUDE_ORBIT) || !IsActive())
        return;

    m_afDeltaValues[LATITUDE_ORBIT] += ms_fAngleChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::LatitudeNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(LATITUDE_ORBIT) || !IsActive())
        return;

    m_afDeltaValues[LATITUDE_ORBIT] -= ms_fAngleChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::LongitudePosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(LONGITUDE_ORBIT) || !IsActive())
        return;

    m_afDeltaValues[LONGITUDE_ORBIT] += ms_fAngleChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::LongitudeNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(LONGITUDE_ORBIT) || !IsActive())
        return;

    m_afDeltaValues[LONGITUDE_ORBIT] -= ms_fAngleChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::PanRLPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_RIGHT_LEFT) || !IsActive())
        return;

    m_afDeltaValues[PAN_RIGHT_LEFT] += ms_fPanChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::PanRLNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_RIGHT_LEFT) || !IsActive())
        return;

    m_afDeltaValues[PAN_RIGHT_LEFT] -= ms_fPanChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::PanUDPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_UP_DOWN) || !IsActive())
        return;

    m_afDeltaValues[PAN_UP_DOWN] += ms_fPanChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::PanUDNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(PAN_UP_DOWN) || !IsActive())
        return;

    m_afDeltaValues[PAN_UP_DOWN] -= ms_fPanChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::FlyPosChange(unsigned char)
{
    if (!m_kIsActive.GetAt(FLY) || !IsActive())
        return;

    m_afDeltaValues[FLY] += ms_fPanChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::FlyNegChange(unsigned char)
{
    if (!m_kIsActive.GetAt(FLY) || !IsActive())
        return;

    m_afDeltaValues[FLY] -= ms_fPanChangeRate * m_fRadius *
        NiNavManager::GetNavManager()->GetFrameTime();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::SetObject(NiAVObject* pkObject)
{
    m_spControlledObject = pkObject;

    if (m_spControlledObject == NULL)
    {
        NiOutputDebugString("Input Controlled Object to Orbit Nav Controller "
            "was NULL.  This leads to faulty results.\n");
        NiNavBaseController::NavState kNavState;
        kNavState.bIgnoreThis = true;
        if (m_bActive)
            SetActive(false, kNavState);
    }

    if (m_bActive)
    {
        NiPoint3 kLookDir;
        m_spControlledObject->GetWorldRotate().GetCol(0, kLookDir);
        m_kCenterPt = m_spControlledObject->GetWorldTranslate() + 
            (m_fRadius * kLookDir);
    }
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::SetUpDir(const NiPoint3& kNewUpDir)
{
    m_kUpVector = kNewUpDir;
    if (m_kUpVector.Length() != 0.0f)
        m_kUpVector.Unitize();
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::SetControlActive(bool bActive, 
    Controls eControlId)
{
    NIASSERT(eControlId >= 0);
    NIASSERT(eControlId < ORBIT_NUM_CONTROLS);
    m_kIsActive.SetAt(eControlId, bActive);
}
//---------------------------------------------------------------------------
inline void NiNavOrbitController::SetRadius(float fRadius)
{
    if (fRadius < ms_fMinRadius)
        m_fRadius = ms_fMinRadius;
    else
        m_fRadius = fRadius;
}
//---------------------------------------------------------------------------
