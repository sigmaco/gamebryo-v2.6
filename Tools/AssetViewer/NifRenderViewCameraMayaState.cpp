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


#include "StdAfx.h"
#include "NifRenderViewCameraMayaState.h"


#include "NifRenderViewCamera3dsmaxState.h"
#include "NifRenderView.h"
#include <NiSceneMouse.h>


//---------------------------------------------------------------------------
float CNifRenderViewCameraMayaState::ms_fPanSpeed = 4.0f;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CNifRenderViewCameraMayaState::CNifRenderViewCameraMayaState(
    CNifRenderView* pkView) : CNifRenderViewCameraBaseState(pkView)
{
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::Initialize()
{
    m_bLDown = false;
    m_bMDown = false;
    m_bRDown = false;
    m_bMouseShifted = false;

    CNifRenderViewCameraBaseState::Initialize();
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::UpdateDevices()
{
    CNifRenderViewCameraBaseState::UpdateDevices();

    if (m_bTrackingMouse)
    {
        bool bAltDown =
            m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_MENU);

        if (bAltDown)
        {
            CPoint kDelta = m_kLastPoint - m_kBeginPoint;
            NiPoint2 kBounds = m_pkView->GetScreenBounds();

            if (m_bLDown)
                HandleRotate(kDelta, kBounds);

            if (m_bMDown)
                HandlePan(kDelta, kBounds, ms_fPanSpeed);

            if (m_bRDown)
                HandleZoom(kDelta, kBounds);
        }

        if (m_kBeginPoint != m_kLastPoint)
        {
            m_kBeginPoint = m_kLastPoint;
            m_bMouseShifted = true;
        }
    }

    if (0.0f != m_fWheelDelta)
    {
        HandleWheelZoom(m_fWheelDelta);
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnUpdate(UINT Msg)
{
    if (NIF_CAMERAFOCUSREQUESTED == Msg)
    {
        UpdateFocus();
    }
    else
    {
        CNifRenderViewCameraBaseState::OnUpdate(Msg);
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnLButtonDown(UINT, CPoint point)
{
    StartMouseTracking(point);
    m_bLDown = true;
    m_bMouseShifted = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnLButtonUp(UINT, CPoint point)
{
    if (!m_bMouseShifted)
    {
        PerformPick(point);
    }

    StopMouseTracking(point);
    m_bLDown = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnMButtonDown(UINT, CPoint point)
{
    StartMouseTracking(point);
    m_bMDown = true;
    m_bMouseShifted = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnMButtonUp(UINT, CPoint point)
{
    StopMouseTracking(point);
    m_bMDown = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnRButtonDown(UINT, CPoint point)
{
    StartMouseTracking(point);
    m_bRDown = true;
    m_bMouseShifted = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnRButtonUp(UINT, CPoint point)
{
    StopMouseTracking(point);
    m_bRDown = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::OnMouseMove(UINT, CPoint point)
{
    UpdateMouseTracking(point);
}

//---------------------------------------------------------------------------
void CNifRenderViewCameraMayaState::HandleZoom(
    const CPoint& kDelta,
    const NiPoint2& kScreenBounds)
{
    float fDeltaY = ((float)kDelta.y) / kScreenBounds.y;
    float fDeltaX = ((float)kDelta.x) / kScreenBounds.x;

    float fZoomIn =
        (fDeltaY > 0.0f ? fDeltaY : 0.0f) +
        (fDeltaX > 0.0f ? fDeltaX : 0.0f);

    float fZoomOut =
        (fDeltaY < 0.0f ? fDeltaY : 0.0f) +
        (fDeltaX < 0.0f ? fDeltaX : 0.0f);

    float fZoomInc = -(fZoomIn + fZoomOut) * ms_fZoomSpeed * m_fFocusRadius;
    m_fDeltaZoom += fZoomInc;
}

//---------------------------------------------------------------------------
