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
#include "NifRenderViewCamera3dsmaxState.h"
#include "NifRenderView.h"


//---------------------------------------------------------------------------
float CNifRenderViewCamera3dsmaxState::ms_fSlowPanSpeed = 4.0f;
float CNifRenderViewCamera3dsmaxState::ms_fFastPanSpeed = 20.0f;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CNifRenderViewCamera3dsmaxState::CNifRenderViewCamera3dsmaxState(
    CNifRenderView* pkView) : CNifRenderViewCameraBaseState(pkView)
{
    m_bSelectionHappened = false;
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::Initialize()
{
    m_bSelectionHappened = false;
    CNifRenderViewCameraBaseState::Initialize();
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::Update()
{
    if (m_bSelectionHappened && (m_bTrackingMouse || m_fWheelDelta != 0.0f))
    {
        UpdateFocus();
        m_bSelectionHappened = false;
    }
    else
    {
        CNifRenderViewCameraBaseState::Update();
    }
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::UpdateDevices()
{
    CNifRenderViewCameraBaseState::UpdateDevices();

    if (m_bTrackingMouse)
    {
        CPoint kDelta = m_kLastPoint - m_kBeginPoint;
        NiPoint2 kBounds = m_pkView->GetScreenBounds();

        bool bCtrlDown =
            m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_CONTROL);
        bool bAltDown =
            m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_MENU);

        if (bAltDown && bCtrlDown)
        {
            HandleZoom(kDelta, kBounds);
        }
        else if (bAltDown)
        {
            HandleRotate(kDelta, kBounds);
        }
        else
        {
            float fPanCoefficient = ms_fSlowPanSpeed;
            if (bCtrlDown)
                fPanCoefficient = ms_fFastPanSpeed;
            
            HandlePan(kDelta, kBounds, fPanCoefficient);
        }

        m_kBeginPoint = m_kLastPoint;
    }

    if (m_fWheelDelta != 0.0f)
        HandleWheelZoom(m_fWheelDelta);
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::OnUpdate(UINT Msg)
{
    if (NIF_SELECTEDOBJECTCHANGED == Msg)
        m_bSelectionHappened = true;
    else
        CNifRenderViewCameraBaseState::OnUpdate(Msg);
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::OnMButtonDown(UINT, CPoint point)
{
    StartMouseTracking(point);
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::OnMButtonUp(UINT, CPoint point)
{
    StopMouseTracking(point);
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::OnMouseMove(UINT, CPoint point)
{
    UpdateMouseTracking(point);
}

//---------------------------------------------------------------------------
void CNifRenderViewCamera3dsmaxState::OnLButtonDown(UINT, CPoint point)
{
    PerformPick(point);
}