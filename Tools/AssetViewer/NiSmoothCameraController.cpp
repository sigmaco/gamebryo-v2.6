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
#include "NiSmoothCameraController.h"


//---------------------------------------------------------------------------
CNiSmoothCameraController::CNiSmoothCameraController(
    NiCameraPtr spCamera,
    const NiPoint3& kTargetPos,
    float fMinDistToTarget,
    float fControlTime) :
    m_spCamera(spCamera),
    m_fControlTime(fControlTime),
    m_fTime(-1.0f)
{
    m_kStartPos = spCamera->GetTranslate();
    spCamera->GetRotate(m_kStartRot);

    NiPoint3 kMoveDir = kTargetPos - m_kStartPos;
    kMoveDir.Unitize();

    // Calculating target pos taking into account min dist
    m_kTargetPos =
        kTargetPos - kMoveDir * fMinDistToTarget;

    // Calculating final rotation
    NiPoint3 kUpVec = NiPoint3::UNIT_Z;
    NiPoint3 kRightVec = kMoveDir.Cross(kUpVec);
    kRightVec.Unitize();
    kUpVec = kRightVec.Cross(kMoveDir);
    kUpVec.Unitize();

    NiMatrix3 kEndRot(kMoveDir, kUpVec, kRightVec);
    m_kTargetRot.FromRotation(kEndRot);

    // Checking interpolation to be nearest
    if (NiQuaternion::Dot(m_kStartRot, m_kTargetRot) < 0.0f)
        m_kTargetRot = -m_kTargetRot;
}

//---------------------------------------------------------------------------
void CNiSmoothCameraController::Update(float fTimeStep)
{
    if (IsDone())
        return;

    // First update
    if (m_fTime < 0.0f)
    {
        m_fTime = 0.0f;
        return;
    }

    m_fTime += fTimeStep;

    // Calculating time ratio
    float fTimeValue = m_fTime / m_fControlTime;
    if (fTimeValue >= 1.0f)
        fTimeValue = 1.0f;

    // Computing position on this time
    NiPoint3 kNewPos;

    NiPoint3 kMovePath = m_kTargetPos - m_kStartPos;
    kNewPos = m_kStartPos + kMovePath * fTimeValue;

    // Computing rotation on this time
    NiMatrix3 kNewRot;

    NiQuaternion kInterQuat = NiQuaternion::Slerp(
        fTimeValue, m_kStartRot, m_kTargetRot);
    kInterQuat.ToRotation(kNewRot);

    // Setting all into camera
    m_spCamera->SetTranslate(kNewPos);
    m_spCamera->SetRotate(kNewRot);
    NiMesh::CompleteSceneModifiers(m_spCamera);
}

//---------------------------------------------------------------------------
bool CNiSmoothCameraController::IsDone() const
{
    return (m_fTime / m_fControlTime) >= 1.0f;
}