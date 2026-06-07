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

// NifRenderViewCameraRotateState.h

#include "stdafx.h"
#include "NifRenderViewCameraRotateState.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
CNifRenderViewCameraRotateState::CNifRenderViewCameraRotateState(
    CNifRenderView* pkView) : CNifRenderViewBufferedUIState(pkView)
{
    m_pkCameraInfo  = NULL;
    m_bTrackingMouse = false;
    m_fSceneScale = 0.0f;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraRotateState::~CNifRenderViewCameraRotateState()
{
    m_pkCameraInfo = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::Update()
{
    CNifRenderViewBufferedUIState::Update();
    if(m_pkCameraInfo && m_bActive)
    {
        m_kOriginalRotation.GetCol(0, m_kRollAxis);
        m_kOriginalRotation.GetCol(1, m_kYawAxis);
        m_kOriginalRotation.GetCol(2, m_kPitchAxis);

        SetPitch(GetPitch() + m_fIncPitchAngle);
        SetRoll(GetRoll() + m_fIncRollAngle);
        SetYaw(GetYaw() + m_fIncYawAngle);
        
        NiMatrix3 kPitch;
        NiMatrix3 kRoll;
        NiMatrix3 kYaw;
        kPitch.MakeRotation(GetPitch(), m_kPitchAxis);
        kYaw.MakeRotation(GetYaw(), m_kYawAxis);
        kRoll.MakeRotation(GetRoll(), m_kRollAxis);
       
        if(GetRoll() != 0.0f)
        {
            m_kYawAxis = kRoll*m_kYawAxis;
            m_kPitchAxis = kRoll*m_kPitchAxis;
        }

        m_kYawAxis = kPitch*m_kYawAxis;
        m_kRollAxis = kPitch*m_kRollAxis;
    
        m_kPitchAxis = kYaw*m_kPitchAxis;
        m_kRollAxis = kYaw*m_kRollAxis;
        
        NiMatrix3 kMatrix = kYaw*kPitch*kRoll*m_kOriginalRotation;
        kMatrix.Reorthogonalize();
        m_pkCameraInfo->m_spCam->SetRotate(kMatrix);

        DoRollAxisTranslate();
        DoYawAxisTranslate();
        DoPitchAxisTranslate();
      
        m_fIncPitchAngle = 0.0f;
        m_fIncRollAngle = 0.0f;
        m_fIncYawAngle = 0.0f;
        SetTranslate(NiPoint3(0.0f,0.0f,0.0f));
        m_pkCameraInfo->m_spCam->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_pkCameraInfo->m_spCam);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::Initialize()
{ 
    CNifDoc* pkDoc = m_pkView->GetDocument();
    NIASSERT(pkDoc);
    pkDoc->Lock();
    CNifRenderViewBufferedUIState::Initialize();

    NiCameraInfo* kInfo = 
        pkDoc->GetCameraInfo(m_pkView->GetCurrentCameraIndices());
    
    m_fPitchAngle = 0.0f;
    m_fRollAngle = 0.0f;
    m_fYawAngle = 0.0f;
    m_fIncPitchAngle = 0.0f;
    m_fIncRollAngle = 0.0f;
    m_fIncYawAngle = 0.0f;
    m_kTranslation = NiPoint3::ZERO;
    m_kRollAxis = NiPoint3::ZERO;
    m_kPitchAxis = NiPoint3::ZERO;
    m_kYawAxis = NiPoint3::ZERO;

    if(kInfo && !kInfo->m_bIsAnimated && kInfo->m_spCam != NULL)
    {
        m_pkCameraInfo = kInfo;
        m_bActive = m_bActive && true;
    
        m_kOriginalRotation = m_pkCameraInfo->m_spCam->GetRotate();
        m_kOriginalTranslate = m_pkCameraInfo->m_spCam->GetTranslate();

        SetTranslate(NiPoint3(0.0f,0.0f,0.0f));
        SetPitch(0.0f);
        SetRoll(0.0f);
        SetYaw(0.0f);
        m_fIncPitchAngle = 0.0f;
        m_fIncRollAngle = 0.0f;
        m_fIncYawAngle = 0.0f;
        m_kOriginalRotation.GetCol(0, m_kRollAxis);
        m_kOriginalRotation.GetCol(1, m_kYawAxis);
        m_kOriginalRotation.GetCol(2, m_kPitchAxis);
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
void CNifRenderViewCameraRotateState::OnUpdate(UINT Msg)
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
bool CNifRenderViewCameraRotateState::CanExecuteCommand(char* pcCommandID)
{ 
    return true;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::UpdateDevices()
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
        NiPoint2 kBounds = m_pkView->GetScreenBounds();

        // We're using MAX's coordinate system here, not NI
        //       Y                     Y
        // NI:   |__ Z         MAX:    |__ X
        //      /                     /
        //    -X                     Z
        //
        if(m_eCurrentAxisConstraint == RIGHT_AXIS)   
        {
            IncrementPitch(((float)kDelta.y) / kBounds.y * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == FORWARD_AXIS) 
        {
            IncrementRoll(((float)kDelta.x) / kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == UP_AXIS) 
        {
            IncrementYaw(((float)kDelta.x) / kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == RIGHT_UP_AXIS)
        {
            IncrementPitch(((float)kDelta.y) / kBounds.y * 2.0f * NI_PI);
            IncrementYaw(((float)kDelta.x) / kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == FORWARD_UP_AXIS)
        {
            IncrementRoll(((float)kDelta.y) / kBounds.y * 2.0f * NI_PI);
            IncrementYaw(((float)kDelta.x) / kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == FORWARD_RIGHT_AXIS)
        {
            IncrementRoll(((float)kDelta.y) / kBounds.y * 2.0f * NI_PI);
            IncrementPitch(((float)kDelta.x) / kBounds.x * 2.0f * NI_PI);
        }

        m_kBeginPoint = m_kLastPoint;
    }
    
    if(m_fWheelDelta != 0.0f)
    {
        IncrementTranslate(NiPoint3::UNIT_X * m_fWheelDelta * 0.1f *
            m_fSceneScale);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnForwardKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_X * m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnBackwardKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_X * -m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnLeftKey(NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Z * -m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnRightKey(NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Z * m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnPitchUpKey(
    NiDevice::DeviceState eState)
{
    IncrementPitch(-m_fPitchSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnPitchDownKey(
    NiDevice::DeviceState eState)
{
    IncrementPitch(m_fPitchSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnYawLeftKey(
    NiDevice::DeviceState eState)
{
    IncrementYaw(-m_fYawSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnYawRightKey(
    NiDevice::DeviceState eState)
{
    IncrementYaw(m_fYawSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnUpKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Y * m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnDownKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Y * -m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnMouseMove(UINT nFlags, CPoint point)
{
    if(m_bTrackingMouse)
    {        
        m_kLastPoint = point;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnLButtonUp(UINT nFlags, CPoint point)
{
    ::ReleaseCapture();
    this->m_bTrackingMouse = false;
    m_kLastPoint = point;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_pkView->SetCapture();
    m_bTrackingMouse = true;
    m_kLastPoint = point;
    m_kBeginPoint = point;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::SetPitch(float fAngle)
{
    m_fPitchAngle = fAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::IncrementPitch(float fAngle)
{
    m_fIncPitchAngle += fAngle;
}
//---------------------------------------------------------------------------
float CNifRenderViewCameraRotateState::GetPitch()
{
    return m_fPitchAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::SetYaw(float fAngle)
{
    m_fYawAngle = fAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::IncrementYaw(float fAngle)
{
    m_fIncYawAngle += fAngle;
}
//---------------------------------------------------------------------------
float CNifRenderViewCameraRotateState::GetYaw()
{
    return m_fYawAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::SetRoll(float fAngle)
{
    m_fRollAngle = fAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::IncrementRoll(float fAngle)
{
    m_fIncRollAngle += fAngle;
}
//---------------------------------------------------------------------------
float CNifRenderViewCameraRotateState::GetRoll()
{
    return m_fRollAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::SetTranslate(NiPoint3 kTranslateVector)
{
    m_kTranslation = kTranslateVector;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::IncrementTranslate(
    NiPoint3 kDeltaTranslateVector)
{
    m_kTranslation += kDeltaTranslateVector;                     
}
//---------------------------------------------------------------------------
NiPoint3 CNifRenderViewCameraRotateState::GetTranslate()
{
    return m_kTranslation;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::DoRollAxisTranslate()
{
    if (m_kTranslation.x != 0.0f)
    {
        NiPoint3 trn = m_pkCameraInfo->m_spCam->GetTranslate();
        NiPoint3 kIncr = m_kTranslation.x * m_kRollAxis;
        trn += kIncr;
        m_pkCameraInfo->m_spCam->SetTranslate(trn);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::DoYawAxisTranslate()
{
    if (m_kTranslation.y != 0.0f)
    {
        NiPoint3 trn = m_pkCameraInfo->m_spCam->GetTranslate();
        NiPoint3 kIncr = m_kTranslation.y * m_kYawAxis;
        trn += kIncr;
        m_pkCameraInfo->m_spCam->SetTranslate(trn);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraRotateState::DoPitchAxisTranslate()
{
    if (m_kTranslation.z != 0.0f)
    {
        NiPoint3 trn = m_pkCameraInfo->m_spCam->GetTranslate();
        NiPoint3 kIncr = m_kTranslation.z * m_kPitchAxis;
        trn += kIncr;
        m_pkCameraInfo->m_spCam->SetTranslate(trn);
    }
}
//---------------------------------------------------------------------------
bool CNifRenderViewCameraRotateState::IsAxisConstraintAllowed(UIAxisConstraint
                                                              eConstraint)
{
    return true;
}