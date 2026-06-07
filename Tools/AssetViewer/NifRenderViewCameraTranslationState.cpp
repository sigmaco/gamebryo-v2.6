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

// NifRenderViewCameraTranslationState.h

#include "stdafx.h"
#include "NifRenderViewCameraTranslationState.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
CNifRenderViewCameraTranslationState::CNifRenderViewCameraTranslationState(
    CNifRenderView* pkView) : CNifRenderViewBufferedUIState(pkView)
{
    m_pkCameraInfo = NULL;
    m_bTrackingMouse = false;
    m_fSceneScale = 0.0f;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraTranslationState::~CNifRenderViewCameraTranslationState()
{
    m_pkCameraInfo = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::Update()
{
    m_kDeltaTranslate = NiPoint3::ZERO;
    CNifRenderViewBufferedUIState::Update();
    if(m_pkCameraInfo && m_bActive)
    {
        NiMatrix3 kRot = m_pkCameraInfo->m_spCam->GetRotate();
        NiPoint3 kTranslate =  m_pkCameraInfo->m_spCam->GetTranslate();
        kTranslate += kRot*m_kDeltaTranslate;
        m_pkCameraInfo->m_spCam->SetTranslate(kTranslate);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::Initialize()
{ 
    CNifDoc* pkDoc = m_pkView->GetDocument();
    NIASSERT(pkDoc);
    pkDoc->Lock();
    CNifRenderViewBufferedUIState::Initialize();

    NiCameraInfo* kInfo = 
        pkDoc->GetCameraInfo(m_pkView->GetCurrentCameraIndices());

    if(kInfo && !kInfo->m_bIsAnimated && kInfo->m_spCam != NULL)
    {
        m_pkCameraInfo = kInfo;
        m_bActive = m_bActive && true;
    }
    else
        m_bActive = false;

    NiNode* pkScene = pkDoc->GetSceneGraph();

    if(pkScene)
    {
       m_fSceneScale = pkScene->GetWorldBound().GetRadius();
    }
    else
       m_bActive = false;

    pkDoc->UnLock();

}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnUpdate(UINT Msg)
{ 
    switch (Msg)
    {
        case NIF_DESTROYSCENE:
            m_bActive = false;
        case NIF_CURRENTCAMERACHANGED:
        case NIF_USERPREFERENCESCHANGED:
        case NIF_CREATESCENE:
        case NIF_ADDNIF:
        case NIF_REMOVENIF:
        case NIF_SCENECHANGED:
            m_bReInitialize = true;
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
bool CNifRenderViewCameraTranslationState::CanExecuteCommand(
    char* pcCommandID)
{ 
    return true;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::UpdateDevices()
{ 
    CNifRenderViewBufferedUIState::UpdateDevices();

    bool bRightAxisKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_X);
    bool bUpAxisKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_SHIFT);
    bool bForwardAxisKey = m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_Z);

    if(bRightAxisKey && bUpAxisKey)
    {
        m_eCurrentAxisConstraint = RIGHT_UP_AXIS;
    }
    else if(bUpAxisKey && bForwardAxisKey)
    {
        m_eCurrentAxisConstraint = FORWARD_UP_AXIS;
    }
    else if(bRightAxisKey && bForwardAxisKey)
    {
        m_eCurrentAxisConstraint = FORWARD_RIGHT_AXIS;
    }
    else if(bRightAxisKey)
    {
        m_eCurrentAxisConstraint = RIGHT_AXIS;
    }
    else if(bUpAxisKey)
    {
        m_eCurrentAxisConstraint = UP_AXIS;
    }
    else if (bForwardAxisKey)
    {
        m_eCurrentAxisConstraint = FORWARD_AXIS;
    }
    else
    {
        m_eCurrentAxisConstraint = m_eLastSetAxisConstraint;
    }
        
    if(m_bTrackingMouse)
    {
        CPoint kDelta = m_kLastPoint - m_kBeginPoint;
        kDelta = -kDelta;
        NiPoint2 kBounds = m_pkView->GetScreenBounds();



        // We're using MAX's coordinate system here, not NI
        //       Y                     Y
        // NI:   |__ Z         MAX:    |__ X
        //      /                     /
        //    -X                     Z
        //
        if(m_eCurrentAxisConstraint == FORWARD_AXIS)
        {
            m_kDeltaTranslate += NiPoint3::UNIT_X * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == UP_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += NiPoint3::UNIT_Y * ((float)kDelta.y) /  
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == RIGHT_AXIS)
        {
            m_kDeltaTranslate += NiPoint3::UNIT_Z * ((float)kDelta.x) / 
                kBounds.x * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == RIGHT_UP_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += NiPoint3::UNIT_Z * ((float)kDelta.x) / 
                kBounds.x  * m_fSceneScale * 2.0f;
            m_kDeltaTranslate += NiPoint3::UNIT_Y * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == FORWARD_UP_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += NiPoint3::UNIT_X * ((float)kDelta.x) / 
                kBounds.x  * m_fSceneScale * 2.0f;
            m_kDeltaTranslate += NiPoint3::UNIT_Y * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        else if(m_eCurrentAxisConstraint == FORWARD_RIGHT_AXIS)
        {
            kDelta.y = -kDelta.y;
            m_kDeltaTranslate += NiPoint3::UNIT_Z * ((float)kDelta.x) / 
                kBounds.x  * m_fSceneScale * 2.0f;
            m_kDeltaTranslate += NiPoint3::UNIT_X * ((float)kDelta.y) / 
                kBounds.y * m_fSceneScale * 2.0f;
        }
        m_kBeginPoint = m_kLastPoint;
    }

    if(m_fWheelDelta != 0.0f)
    {
        m_kDeltaTranslate += NiPoint3::UNIT_X * m_fWheelDelta * 0.1f *
            m_fSceneScale;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnForwardKey(
    NiDevice::DeviceState eState)
{ 
    m_kDeltaTranslate += NiPoint3::UNIT_X * m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnBackwardKey(
    NiDevice::DeviceState eState)
{ 
    m_kDeltaTranslate -= NiPoint3::UNIT_X * m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnLeftKey(
    NiDevice::DeviceState eState)
{ 
    m_kDeltaTranslate -= NiPoint3::UNIT_Z * m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnRightKey(
    NiDevice::DeviceState eState)
{ 
    m_kDeltaTranslate += NiPoint3::UNIT_Z * m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnUpKey(
    NiDevice::DeviceState eState)
{ 
    m_kDeltaTranslate += NiPoint3::UNIT_Y * m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnDownKey(
    NiDevice::DeviceState eState)
{ 
    m_kDeltaTranslate -= NiPoint3::UNIT_Y * m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnMouseMove(UINT nFlags,
    CPoint point)
{
    if(m_bTrackingMouse)
    {        
        m_kLastPoint = point;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnLButtonUp(UINT nFlags,
    CPoint point)
{
    ::ReleaseCapture();
    this->m_bTrackingMouse = false;
    m_kLastPoint = point;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTranslationState::OnLButtonDown(UINT nFlags,
    CPoint point)
{
    m_pkView->SetCapture();
    m_bTrackingMouse = true;
    m_kLastPoint = point;
    m_kBeginPoint = point;
}
//---------------------------------------------------------------------------
bool 
CNifRenderViewCameraTranslationState::IsAxisConstraintAllowed(UIAxisConstraint 
                                                             eMode)
{
    return true;
}