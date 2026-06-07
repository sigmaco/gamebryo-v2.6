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

#include "stdafx.h"
#include "MUIState.h"
#include "MSharedData.h"
#include "MFramework.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
MUIState::MUIState()
{
    m_kMousePos.x = 0;
    m_kMousePos.y = 0;
    ms_fWheelDelta = 0;
    ms_bMouseDown[0] = false;
    ms_bMouseDown[1] = false;
    ms_bMouseDown[2] = false;
    m_bHovering = false;
    m_bEntered = false;
    m_hWnd = 0;
    m_bFollowCam = true;
    m_bCachedPlayState = false;
}
//---------------------------------------------------------------------------
String* MUIState::GetName()
{
    return NULL;
}
//---------------------------------------------------------------------------
void MUIState::MouseEnter()
{
    m_bEntered = true;
}
//---------------------------------------------------------------------------
void MUIState::MouseLeave()
{
    m_bEntered = false;
}
//---------------------------------------------------------------------------
void MUIState::MouseHover()
{
    m_bHovering = true;
}
//---------------------------------------------------------------------------
void MUIState::DoubleClick()
{
    m_bHovering = false;
}
//---------------------------------------------------------------------------
void MUIState::MouseMove(int, int)
{
    m_bHovering = false;
}
//---------------------------------------------------------------------------
void MUIState::MouseUp(MouseButtonType eType, int, int)
{
    ms_bMouseDown[eType] = false;
    m_bHovering = false;
}
//---------------------------------------------------------------------------
void MUIState::MouseDown(MouseButtonType eType, int, int)
{
    ms_bMouseDown[eType] = true;
    m_bHovering = false;
}
//---------------------------------------------------------------------------
void MUIState::MouseWheel(int iDelta)
{
    ms_fWheelDelta += ((float)iDelta)/120.0f;   
}
//---------------------------------------------------------------------------
void MUIState::Update()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    if (m_pkMOrbitPoint != NULL && m_pkMOrbitPoint->Active)
    {
        NiPoint3 kOrbitWorldTrans;
        m_pkMOrbitPoint->Update(0.0f);

        m_pkMOrbitPoint->GetWorldTranslate(kOrbitWorldTrans);

        if (m_pkMCamera != NULL)
        {
            m_pkMCamera->SetTranslate(kOrbitWorldTrans);
            m_pkMCamera->Update(0.0f);
        }
    }
    else if (m_pkMOrbitPoint != NULL && m_pkMCamera != NULL)
    {
        m_pkMCamera->Update(0.0f);  
    }

    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
void MUIState::RefreshData()
{
    DeleteContents();
}
//---------------------------------------------------------------------------
void MUIState::SetWindowHandle(HWND hWnd)
{
    m_hWnd = hWnd;
}
//---------------------------------------------------------------------------
POINT MUIState::GetCurrentMousePosition()
{
    POINT pt;
    ::GetCursorPos(&pt);
    GetPositionInScreenSpace(pt);
    return pt;
}
//---------------------------------------------------------------------------
void MUIState::GetPositionInScreenSpace(POINT& kPt)
{
    ::ClientToScreen(m_hWnd, &kPt);
}
//---------------------------------------------------------------------------
void MUIState::SetCamera(MCamera* pkMCamera)
{
    m_pkMCamera = pkMCamera;
}
//---------------------------------------------------------------------------
void MUIState::SetOrbitPoint(MOrbitPoint* pkMOrbitPoint)
{
    m_pkMOrbitPoint = pkMOrbitPoint;
}
//---------------------------------------------------------------------------
void MUIState::SetFollowCam(bool bFollow)
{
    m_bFollowCam = bFollow;
}
//---------------------------------------------------------------------------
void MUIState::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIState::Activate(bool)
{
}
//---------------------------------------------------------------------------
void MUIState::Deactivate()
{
}
//---------------------------------------------------------------------------
void MUIState::BeginPause()
{
    m_bCachedPlayState = MFramework::Instance->Clock->Enabled;
    MFramework::Instance->Clock->Enabled = false;
}
//---------------------------------------------------------------------------
void MUIState::EndPause()
{
    MFramework::Instance->Clock->Enabled = m_bCachedPlayState;
}
//---------------------------------------------------------------------------
void MUIState::MouseZoom(POINT& point)
{
    if (!m_pkMOrbitPoint)
        return;
    
    if (!m_pkMOrbitPoint->Active)
        return;

    MRenderer* pkRenderer = MFramework::Instance->Renderer;
    int iVertRes = pkRenderer->Height;

    NiPoint3 kOrbitTrans;
    m_pkMOrbitPoint->GetWorldTranslate(kOrbitTrans);
    
    float fVert = (float) (point.y - m_ptClickPoint.y);
    float fVertScale = (fVert / (iVertRes/4));
    float fMultiplier = 0.0f;
    float fZoomFactor = 0.50;
    if (point.y - m_ptClickPoint.y != 0)
        fMultiplier = fZoomFactor * -fVertScale;

    NiBound kOrbitBound;
    m_pkMOrbitPoint->GetOrbitBound(kOrbitBound);

    m_pkMCamera->Zoom(fMultiplier, kOrbitTrans, kOrbitBound);
    Update();
}
//---------------------------------------------------------------------------
void MUIState::MousePan(POINT& point, bool bVert, bool bHorz)
{
    if (!m_pkMOrbitPoint)
        return;

    if (!m_pkMOrbitPoint->Active)
        return;

    if (!bVert && !bHorz)
        return;

    MRenderer* pkRenderer = MFramework::Instance->Renderer;
    int iHorzRes = pkRenderer->Width;
    int iVertRes = pkRenderer->Height;

    NiPoint3 kCamWorld;
    m_pkMCamera->GetWorldTranslate(kCamWorld);

    NiPoint3 kOrbitWorld;
    m_pkMOrbitPoint->GetWorldTranslate(kOrbitWorld);

    NiPoint3 kDist = kOrbitWorld - kCamWorld;
    float fRadius = NiAbs(kDist.Length());

    // Get horz translation.
    float fHorz = (float) (point.x - m_ptClickPoint.x);
    float fHorzTrans = (fHorz / iHorzRes) * 5.0f *
        fRadius;

    // Get vert translation.
    float fVert = (float) (m_ptClickPoint.y - point.y);
    float fVertTrans = (fVert / iVertRes) * 5.0f *
        fRadius;

    NiCamera* pkCam = m_pkMCamera->GetCamera();

    if (m_pkMCamera->InvertPan)
    {
        fVertTrans *= -1.0f;
        fHorzTrans *= -1.0f;
    }
    
    if (bVert)
    {
        kOrbitWorld =  kOrbitWorld + 
            fVertTrans * (pkCam->GetWorldRotate() * NiPoint3::UNIT_Y);
    }

    if (bHorz)
    {
        kOrbitWorld =  kOrbitWorld + 
            fHorzTrans * (pkCam->GetWorldRotate() * NiPoint3::UNIT_Z);
    }


    NiTransform kParentWorldTrans;
    m_pkMOrbitPoint->GetParentWorldTransform(kParentWorldTrans);
    NiTransform kInvParentWorldTrans;
    kParentWorldTrans.Invert(kInvParentWorldTrans);
    NiPoint3 kOrbitLocal = kInvParentWorldTrans * kOrbitWorld;
    
    m_pkMOrbitPoint->SetTranslate(kOrbitLocal);
    Update();
}
//---------------------------------------------------------------------------
void MUIState::MouseOrbit(POINT& point, bool bVert, bool bHorz)
{
    if (!m_pkMOrbitPoint)
        return;

    if (!m_pkMOrbitPoint->Active)
        return;

    if (!bVert && !bHorz)
        return;

    MRenderer* pkRenderer = MFramework::Instance->Renderer;
    int iHorzRes = pkRenderer->Width;
    int iVertRes = pkRenderer->Height;

    // Make up rotation.
    float fHorz = (float) (point.x - m_ptClickPoint.x);
    float fUpRot = (fHorz / iHorzRes) * 2.0f * NI_PI;   

    // Make X rotation.
    float fVert = (float) (point.y - m_ptClickPoint.y);
    float fXRot = (fVert / iVertRes) * 2.0f * NI_PI;
    
    if (bVert)
        m_pkMCamera->RotateUp(fUpRot);
    if (bHorz)
        m_pkMCamera->RotateX(fXRot);

    Update();
    
}
//---------------------------------------------------------------------------
void MUIState::WheelZoom(int iWheelDelta)
{
    if (!m_pkMOrbitPoint)
        return;

    if (!m_pkMOrbitPoint->Active)
        return;

    MUIState::MouseWheel(iWheelDelta); 

    if(ms_fWheelDelta != 0.0f)
    {
        MSharedData* pkSharedData = MSharedData::Instance;
        pkSharedData->Lock();
    
        NiPoint3 kOrbitTrans; 
        m_pkMOrbitPoint->GetWorldTranslate(kOrbitTrans);

        float fMultiplier = 0.0f;
        float fZoomFactor = 0.25;
        if (iWheelDelta != 0)
            fMultiplier = fZoomFactor * (float)(iWheelDelta / 120);

        NiBound kOrbitBound;
        m_pkMOrbitPoint->GetOrbitBound(kOrbitBound);
        
        m_pkMCamera->Zoom(fMultiplier, kOrbitTrans, kOrbitBound);
    
        pkSharedData->Unlock();
        Update();
    }
}
//---------------------------------------------------------------------------
void MUIState::ForceMouseShow()
{
    ReleaseCapture();
    ShowCursor(TRUE);
}
//---------------------------------------------------------------------------
void MUIState::ShowCursor(bool bShow)
{
    if (bShow == true)
        while(::ShowCursor(bShow) < 0);
    else
        while(::ShowCursor(bShow) >= 0);
}
//---------------------------------------------------------------------------
void MUIState::SetCapture(HWND hwnd)
{
    ::SetCapture(hwnd);
}
//---------------------------------------------------------------------------
void MUIState::ReleaseCapture()
{
    ::ReleaseCapture();
}
//---------------------------------------------------------------------------
