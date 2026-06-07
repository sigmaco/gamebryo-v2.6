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
#include "NiOrbitCamera.h"

NiImplementRTTI(NiOrbitCamera, NiViewerCamera);
//---------------------------------------------------------------------------
NiOrbitCamera::NiOrbitCamera(NiCamera* pkAffectedCam) :
    NiViewerCamera(pkAffectedCam)
{
    m_kOrbitPt.x = 0.0f;
    m_kOrbitPt.y = 0.0f;
    m_kOrbitPt.z = 0.0f;
    NiOutputDebugString("Creating new NiOrbitCamera\n");
}
//---------------------------------------------------------------------------
NiOrbitCamera::~NiOrbitCamera()
{
    /* */
}
//---------------------------------------------------------------------------
void NiOrbitCamera::Reset()
{
    NiViewerCamera::Reset();
    if (m_pkCamera)
    {
        m_pkCamera->GetParent()->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_pkCamera->GetParent());
    }
    SetOrbitPoint(m_kOrbitPt);
}
//---------------------------------------------------------------------------
void NiOrbitCamera::UpdateCamera()
{
    if (!IsAnimatedCamera())
    {
        // Strategy:
        // Translate the camera using the distance variable to the world 
        // center of the object
        // Rotate the camera appropriately in place
        // Translate the camera back the distance vector along its 
        // adjusted axes

        // Make sure that the world variables are up to date
        m_pkCamera->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_pkCamera);
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

        NiPoint3 kPoint = m_pkCamera->GetWorldTranslate() - m_kOrbitPt;
        
        m_pkCamera->SetTranslate(
            m_kViewMatrix*(kPoint + m_kOffset) +  m_kOrbitPt);

        m_kOffset = NiPoint3(0.0f, 0.0f, 0.0f);
        m_kOriginalRotation = kMatrix;
        SetPitch(0.0f);
        SetRoll(0.0f);
        SetYaw(0.0f);
        m_kFinalViewMatrix = kMatrix;
        m_pkCamera->SetRotate(kMatrix);
        m_pkCamera->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_pkCamera);
    }
}
//---------------------------------------------------------------------------
void NiOrbitCamera::SetOrbitPoint(NiPoint3 kOrbitPt)
{
    m_kOrbitPt = kOrbitPt;
    m_kOffset = NiPoint3(0.0f,0.0f,0.0f); 
    m_kOrbitPt.x = kOrbitPt.x;
    m_kOrbitPt.y = kOrbitPt.y;
    m_kOrbitPt.z = kOrbitPt.z;
}
//---------------------------------------------------------------------------
NiPoint3 NiOrbitCamera::GetOrbitPoint()
{
    return m_kOrbitPt;
}
//---------------------------------------------------------------------------
NiSceneCommand::NiSceneCommandApplyType NiOrbitCamera::GetApplyType()
{
    return NiSceneCommand::APPLY_ORBIT_CAM;
}
//---------------------------------------------------------------------------
void NiOrbitCamera::SetTranslate(NiPoint3 kTranslateVector)
{
    kTranslateVector = m_kOriginalRotation*kTranslateVector;
    m_kOffset.x = kTranslateVector.x;
    m_kOffset.y = kTranslateVector.y;
    m_kOffset.z = kTranslateVector.z;
}
//---------------------------------------------------------------------------
void NiOrbitCamera::IncrementTranslate(NiPoint3 kDeltaTranslateVector)
{
    kDeltaTranslateVector = m_kOriginalRotation*kDeltaTranslateVector;
    m_kOffset.x += kDeltaTranslateVector.x;
    m_kOffset.y += kDeltaTranslateVector.y;
    m_kOffset.z += kDeltaTranslateVector.z;
}
//---------------------------------------------------------------------------
NiPoint3 NiOrbitCamera::GetTranslate()
{
    return m_kOffset;
}
//---------------------------------------------------------------------------
NiMatrix3 NiOrbitCamera::GetPitchMatrix()
{
   NiMatrix3 matrix;
   matrix.MakeRotation(m_fPitchAngle, m_kOriginalRotation*NiPoint3::UNIT_X);
   return matrix;
}
//---------------------------------------------------------------------------
NiMatrix3 NiOrbitCamera::GetYawMatrix()
{
   NiMatrix3 matrix;
   matrix.MakeRotation(m_fYawAngle, m_kOriginalRotation*NiPoint3::UNIT_Y);
   return matrix;
}
//---------------------------------------------------------------------------
NiMatrix3 NiOrbitCamera::GetRollMatrix()
{
   NiMatrix3 matrix;
   matrix.MakeRotation(m_fRollAngle, m_kOriginalRotation*NiPoint3::UNIT_Z);
   return matrix;
}
//---------------------------------------------------------------------------
NiPoint3 NiOrbitCamera::GetCompositedTranslate(NiMatrix3& kRotMatrix)
{
    return kRotMatrix*m_kTranslation;
}
//---------------------------------------------------------------------------
void NiOrbitCamera::IncrementPitch(float fAngle)
{
/*    NiPoint3 kPoint(fAngle, 0.0f, 0.0f);
    kPoint = m_kFinalViewMatrix * kPoint;
    if (kPoint.x != 0.0f)
        SetPitch(GetPitch() + kPoint.x);
    if (kPoint.y != 0.0f)
        SetYaw(GetYaw() + kPoint.y);
    if (kPoint.z != 0.0f)
        SetRoll(GetRoll() + kPoint.z);*/
    SetPitch(GetPitch() + fAngle);
}
//---------------------------------------------------------------------------
void NiOrbitCamera::IncrementYaw(float fAngle)
{
/*    NiPoint3 kPoint(0.0f, fAngle, 0.0f);
    kPoint = m_kFinalViewMatrix * kPoint;
    if (kPoint.x != 0.0f)
        SetPitch(GetPitch() + kPoint.x);
    if (kPoint.y != 0.0f)
        SetYaw(GetYaw() + kPoint.y);
    if (kPoint.z != 0.0f)
        SetRoll(GetRoll() + kPoint.z);*/

    SetYaw(GetYaw() + fAngle);
}
//---------------------------------------------------------------------------
void NiOrbitCamera::IncrementRoll(float fAngle)
{
/*    NiPoint3 kPoint(0.0f, 0.0f, fAngle);
    kPoint = m_kFinalViewMatrix * kPoint;
    if (kPoint.x != 0.0f)
        SetPitch(GetPitch() + kPoint.x);
    if (kPoint.y != 0.0f)
        SetYaw(GetYaw() + kPoint.y);
    if (kPoint.z != 0.0f)
        SetRoll(GetRoll() + kPoint.z);*/

    SetRoll(GetRoll()+ fAngle);
}
//---------------------------------------------------------------------------
 
