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

// NifRenderViewCameraTumbleState.h

#include "stdafx.h"
#include "NifRenderViewCameraTumbleState.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
CNifRenderViewCameraTumbleState::CNifRenderViewCameraTumbleState(
    CNifRenderView* pkView) : CNifRenderViewBufferedUIState(pkView)
{
    m_pkCameraInfo = NULL;
    m_bTrackingMouse = false;
    m_fSceneScale = 0.0f;
    m_kOrbitPt = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraTumbleState::~CNifRenderViewCameraTumbleState()
{
    m_pkCameraInfo = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::Update()
{
    CNifRenderViewBufferedUIState::Update();
    if(m_pkCameraInfo && m_bActive)
    {
        if(!m_pkCameraInfo->m_bIsAnimated)
        {
            // Strategy:
            // Translate the camera using the distance variable to the world 
            // center of the object
            // Rotate the camera appropriately in place
            // Translate the camera back the distance vector along its 
            // adjusted axes

            // Make sure that the world variables are up to date
            m_pkCameraInfo->m_spCam->Update(0.0f);
            NiMesh::CompleteSceneModifiers(m_pkCameraInfo->m_spCam);
            NiMatrix3 kMatrix;

            // Calculate the rotation matrix of the tumble
            kMatrix = GetPitchMatrix()*GetRollMatrix()*GetYawMatrix();
            m_kViewMatrix = kMatrix;
            // Use the tumble matrix to rotate our camera offset vector
            // This essentially performs the tumble for us
            // Each frame, the tumble is recomputed from the original offset
            // vector. This helps us avoid errors in compounding float drift.

        
            // We keep the camera's original rotation
            kMatrix = kMatrix*m_kOriginalRotation;
            kMatrix.Reorthogonalize();

            NiPoint3 kPoint = m_pkCameraInfo->m_spCam->GetWorldTranslate() - 
                m_kOrbitPt;

            kPoint = m_kViewMatrix*(kPoint + m_kOffset) +  m_kOrbitPt;
            m_pkCameraInfo->m_spCam->SetTranslate(kPoint);

            m_kOffset = NiPoint3(0.0f, 0.0f, 0.0f);
            m_kOriginalRotation = kMatrix;
            SetPitch(0.0f);
            SetRoll(0.0f);
            SetYaw(0.0f);
            m_kFinalViewMatrix = kMatrix;
            m_pkCameraInfo->m_spCam->SetRotate(kMatrix);
            m_pkCameraInfo->m_spCam->Update(0.0f);
            NiMesh::CompleteSceneModifiers(m_pkCameraInfo->m_spCam);

        }
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::Initialize()
{ 
    CNifDoc* pkDoc = m_pkView->GetDocument();
    NIASSERT(pkDoc);
    pkDoc->Lock();
    CNifRenderViewBufferedUIState::Initialize();

    NiCameraInfo* kInfo = 
        pkDoc->GetCameraInfo(m_pkView->GetCurrentCameraIndices());

    m_fPitchAngle = 0.0f;
    m_fYawAngle = 0.0f;
    m_fRollAngle = 0.0f;

    m_kRollAxis = NiPoint3::ZERO;
    m_kYawAxis = NiPoint3::ZERO;
    m_kPitchAxis = NiPoint3::ZERO;
    SetOrbitPoint(m_kOrbitPt);
    NiNode* pkScene = pkDoc->GetSceneGraph();

    if(kInfo && !kInfo->m_bIsAnimated && kInfo->m_spCam != NULL && pkScene)
    {
        SetOrbitPoint(pkScene->GetWorldBound().GetCenter());
        m_pkCameraInfo = kInfo;
        m_kOriginalRotation = kInfo->m_spCam->GetRotate();
        m_kOriginalRotation.GetCol(0, m_kRollAxis);
        m_kOriginalRotation.GetCol(1, m_kYawAxis);
        m_kOriginalRotation.GetCol(2, m_kPitchAxis);
        m_bActive = m_bActive && true;
    }
    else
        m_bActive = false;


    if(pkScene)
    {
        m_fSceneScale = pkScene->GetWorldBound().GetRadius();
    }
    else
        m_fSceneScale = 0.0f;
    pkDoc->UnLock();

}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnUpdate(UINT Msg)
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
bool CNifRenderViewCameraTumbleState::CanExecuteCommand(char* pcCommandID)
{ 
    return true;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::UpdateDevices()
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
        //kDelta.y = -kDelta.y;
        NiPoint2 kBounds = m_pkView->GetScreenBounds();

        // We're using MAX's coordinate system here, not NI
        //       Y                     Y
        // NI:   |__ Z         MAX:    |__ X
        //      /                     /
        //    -X                     Z
        //
        
        if(m_eCurrentAxisConstraint == RIGHT_AXIS)   
        {
            IncrementPitch(((float)kDelta.y)/kBounds.y * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == FORWARD_AXIS) 
        {
            IncrementRoll(((float)kDelta.x)/kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == UP_AXIS) 
        {
            IncrementYaw(((float)kDelta.x)/kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == RIGHT_UP_AXIS)
        {
            IncrementPitch(((float)kDelta.y)/kBounds.y * 2.0f * NI_PI);
            IncrementYaw(((float)kDelta.x)/kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == FORWARD_UP_AXIS)
        {
            IncrementRoll(((float)kDelta.y)/kBounds.y * 2.0f * NI_PI);
            IncrementYaw(((float)kDelta.x)/kBounds.x * 2.0f * NI_PI);
        }
        else if(m_eCurrentAxisConstraint == FORWARD_RIGHT_AXIS)
        {
            IncrementRoll(((float)kDelta.y)/kBounds.y * 2.0f * NI_PI);
            IncrementPitch(((float)kDelta.x)/kBounds.x * 2.0f * NI_PI);
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
void CNifRenderViewCameraTumbleState::OnForwardKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_X * m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnBackwardKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_X * -m_kTranslateSpeed.x * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnLeftKey(NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Z * -m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnRightKey(NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Z * m_kTranslateSpeed.z * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnUpKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Y * m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnDownKey(
    NiDevice::DeviceState eState)
{ 
    IncrementTranslate(NiPoint3::UNIT_Y * -m_kTranslateSpeed.y * 
        m_fSceneScale * m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnPitchUpKey(
    NiDevice::DeviceState eState)
{
    IncrementPitch(-m_fPitchSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnPitchDownKey(
    NiDevice::DeviceState eState)
{
    IncrementPitch(m_fPitchSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnYawLeftKey(
    NiDevice::DeviceState eState)
{
    IncrementYaw(-m_fYawSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnYawRightKey(
    NiDevice::DeviceState eState)
{
    IncrementYaw(m_fYawSpeed* m_fDeltaTime);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnMouseMove(UINT nFlags, CPoint point)
{
    if(m_bTrackingMouse)
    {
        m_kLastPoint = point;
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnLButtonUp(UINT nFlags, CPoint point)
{
    ::ReleaseCapture();
    this->m_bTrackingMouse = false;
    m_kLastPoint = point;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_pkView->SetCapture();
    m_bTrackingMouse = true;
    m_kLastPoint = point;
    m_kBeginPoint = point;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::SetOrbitPoint(NiPoint3 kOrbitPt)
{
    m_kOrbitPt = kOrbitPt;
    m_kOffset = NiPoint3(0.0f,0.0f,0.0f); 
    m_kOrbitPt.x = kOrbitPt.x;
    m_kOrbitPt.y = kOrbitPt.y;
    m_kOrbitPt.z = kOrbitPt.z;
}
//---------------------------------------------------------------------------
NiPoint3 CNifRenderViewCameraTumbleState::GetOrbitPoint()
{
    return m_kOrbitPt;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::IncrementTranslate(
    NiPoint3 kDeltaTranslateVector)
{
    kDeltaTranslateVector = m_kOriginalRotation*kDeltaTranslateVector;
    m_kOffset.x += kDeltaTranslateVector.x;
    m_kOffset.y += kDeltaTranslateVector.y;
    m_kOffset.z += kDeltaTranslateVector.z;
}
//---------------------------------------------------------------------------
NiMatrix3 CNifRenderViewCameraTumbleState::GetPitchMatrix()
{
   NiMatrix3 matrix;
   matrix.MakeRotation(m_fPitchAngle, m_kOriginalRotation*NiPoint3::UNIT_Z);
   return matrix;
}
//---------------------------------------------------------------------------
NiMatrix3 CNifRenderViewCameraTumbleState::GetYawMatrix()
{
   NiMatrix3 matrix;
   matrix.MakeRotation(m_fYawAngle, m_kOriginalRotation*NiPoint3::UNIT_Y);
   return matrix;
}
//---------------------------------------------------------------------------
NiMatrix3 CNifRenderViewCameraTumbleState::GetRollMatrix()
{
   NiMatrix3 matrix;
   matrix.MakeRotation(m_fRollAngle, m_kOriginalRotation*NiPoint3::UNIT_X);
   return matrix;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::IncrementPitch(float fAngle)
{
    SetPitch(GetPitch() + fAngle);   
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::IncrementYaw(float fAngle)
{
    SetYaw(GetYaw() + fAngle);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::IncrementRoll(float fAngle)
{
    SetRoll(GetRoll()+ fAngle);
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::SetPitch(float fAngle)
{
    m_fPitchAngle = fAngle;
}
//---------------------------------------------------------------------------
float CNifRenderViewCameraTumbleState::GetPitch()
{
    return m_fPitchAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::SetYaw(float fAngle)
{
    m_fYawAngle = fAngle;
}
//---------------------------------------------------------------------------
float CNifRenderViewCameraTumbleState::GetYaw()
{
    return m_fYawAngle;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraTumbleState::SetRoll(float fAngle)
{
    m_fRollAngle = fAngle;
}
//---------------------------------------------------------------------------
float CNifRenderViewCameraTumbleState::GetRoll()
{
    return m_fRollAngle;
}
//---------------------------------------------------------------------------
bool CNifRenderViewCameraTumbleState::IsAxisConstraintAllowed(UIAxisConstraint 
                                                        eMode)
{
    return true;
}