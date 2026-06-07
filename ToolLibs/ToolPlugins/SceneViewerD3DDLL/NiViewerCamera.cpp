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
#include "NiViewerCamera.h"
#include <NiMain.h>
#include <NiAnimation.h>

NiImplementRTTI( NiViewerCamera, NiObjectNET);

bool HasAnimationController(NiAVObject* pkObject)
{
    if (pkObject == NULL)
        return false;
    else if (NiGetController(NiTransformController, pkObject) != NULL )
        return true;
    else
        return HasAnimationController(pkObject->GetParent());
}

//---------------------------------------------------------------------------
NiViewerCamera::NiViewerCamera(NiCamera* pkAffectedCam)
{
    m_bIsAnimatedCamera = true;
    m_fPitchAngle = 0.0f;
    m_fRollAngle = 0.0f;
    m_fYawAngle = 0.0f;
    m_fIncPitchAngle = 0.0f;
    m_fIncRollAngle = 0.0f;
    m_fIncYawAngle = 0.0f;
    NiOutputDebugString("Creating new NiViewerCamera\n");
    m_kTranslation = NiPoint3::ZERO;
    m_kRollAxis = NiPoint3::ZERO;
    m_kPitchAxis = NiPoint3::ZERO;
    m_kYawAxis = NiPoint3::ZERO;
    m_pkCamera = NULL;

    if (pkAffectedCam != NULL && !HasAnimationController(pkAffectedCam))
    {
        m_kOriginalRotation = pkAffectedCam->GetRotate();
        m_kOriginalTranslate = pkAffectedCam->GetTranslate();
        m_pkCamera = pkAffectedCam;
        m_bIsAnimatedCamera = false;

        m_kOriginalRotation.GetCol(0, m_kRollAxis);
        m_kOriginalRotation.GetCol(1, m_kYawAxis);
        m_kOriginalRotation.GetCol(2, m_kPitchAxis);

    }
}
//---------------------------------------------------------------------------

NiViewerCamera::~NiViewerCamera()
{
    m_pkCamera = NULL;
}
//---------------------------------------------------------------------------
void NiViewerCamera::SetPitch(float fAngle)
{
    m_fPitchAngle = fAngle;
}
//---------------------------------------------------------------------------
void NiViewerCamera::IncrementPitch(float fAngle)
{
    m_fIncPitchAngle += fAngle;
}
//---------------------------------------------------------------------------
float NiViewerCamera::GetPitch()
{
    return m_fPitchAngle;
}
//---------------------------------------------------------------------------
void NiViewerCamera::SetYaw(float fAngle)
{
    m_fYawAngle = fAngle;
}
//---------------------------------------------------------------------------
void NiViewerCamera::IncrementYaw(float fAngle)
{
    m_fIncYawAngle += fAngle;
}
//---------------------------------------------------------------------------
float NiViewerCamera::GetYaw()
{
    return m_fYawAngle;
}
//---------------------------------------------------------------------------
void NiViewerCamera::SetRoll(float fAngle)
{
    m_fRollAngle = fAngle;
}
//---------------------------------------------------------------------------
void NiViewerCamera::IncrementRoll(float fAngle)
{
    m_fIncRollAngle += fAngle;
}
//---------------------------------------------------------------------------
float NiViewerCamera::GetRoll()
{
    return m_fRollAngle;
}
//---------------------------------------------------------------------------
void NiViewerCamera::SetTranslate(NiPoint3 kTranslateVector)
{
    m_kTranslation = kTranslateVector;
}
//---------------------------------------------------------------------------
void NiViewerCamera::IncrementTranslate(NiPoint3 kDeltaTranslateVector)
{
    m_kTranslation += kDeltaTranslateVector;                     
}
//---------------------------------------------------------------------------
NiPoint3 NiViewerCamera::GetTranslate()
{
    return m_kTranslation;
}
//---------------------------------------------------------------------------
void NiViewerCamera::Reset()
{
    if (m_pkCamera)
    {
        m_kOriginalRotation = m_pkCamera->GetRotate();
        m_kOriginalTranslate = m_pkCamera->GetTranslate();

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
}
//---------------------------------------------------------------------------
void NiViewerCamera::UpdateCamera()
{
    if (!IsAnimatedCamera())
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
       
        if (GetRoll() != 0.0f)
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
        m_pkCamera->SetRotate(kMatrix);

        DoRollAxisTranslate();
        DoYawAxisTranslate();
        DoPitchAxisTranslate();
      
        m_fIncPitchAngle = 0.0f;
        m_fIncRollAngle = 0.0f;
        m_fIncYawAngle = 0.0f;
        SetTranslate(NiPoint3(0.0f,0.0f,0.0f));
        m_pkCamera->Update(0.0f);        
        NiMesh::CompleteSceneModifiers(m_pkCamera);

    }
}
//---------------------------------------------------------------------------
void NiViewerCamera::DoRollAxisTranslate()
{
    if (m_kTranslation.x != 0.0f)
    {
        NiPoint3 trn = m_pkCamera->GetTranslate();
        NiPoint3 kIncr = m_kTranslation.x * m_kRollAxis;
        trn += kIncr;
        if (m_pkCamera)
            m_pkCamera->SetTranslate(trn);
    }
}
//---------------------------------------------------------------------------
void NiViewerCamera::DoYawAxisTranslate()
{
    if (m_kTranslation.y != 0.0f)
    {
        NiPoint3 trn = m_pkCamera->GetTranslate();
        NiPoint3 kIncr = m_kTranslation.y * m_kYawAxis;
        trn += kIncr;
        if (m_pkCamera)
            m_pkCamera->SetTranslate(trn);
    }
}
//---------------------------------------------------------------------------
void NiViewerCamera::DoPitchAxisTranslate()
{
    if (m_kTranslation.z != 0.0f)
    {
        NiPoint3 trn = m_pkCamera->GetTranslate();
        NiPoint3 kIncr = m_kTranslation.z * m_kPitchAxis;
        trn += kIncr;
        if (m_pkCamera)
            m_pkCamera->SetTranslate(trn);
    }
}
//---------------------------------------------------------------------------
bool NiViewerCamera::IsAnimatedCamera()
{
    return m_bIsAnimatedCamera;
}
//---------------------------------------------------------------------------
void NiViewerCamera::SetAnimatedCamera(bool bIsAnimated)
{
    m_bIsAnimatedCamera = bIsAnimated;
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType NiViewerCamera::GetApplyType()
{
    return NiSceneCommand::APPLY_ROTATE_CAM;
}
//---------------------------------------------------------------------------
