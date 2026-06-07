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
#include "MFramework.h"
#include "MCamera.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MCamera::MCamera()
{
    m_pkCameraNode = NiNew NiNode;
    m_pkCameraNode->SetSelectiveUpdate(true);
    m_pkCameraNode->SetSelectiveUpdateTransforms(true);
    m_pkCameraNode->SetSelectiveUpdatePropertyControllers(true);
    m_pkCameraNode->SetSelectiveUpdateRigid(false);
    m_pkCamZRot = NiNew NiNode;
    m_pkCamZRot->SetSelectiveUpdate(true);
    m_pkCamZRot->SetSelectiveUpdateTransforms(true);
    m_pkCamZRot->SetSelectiveUpdatePropertyControllers(true);
    m_pkCamZRot->SetSelectiveUpdateRigid(false);
    m_pkCamXRot = NiNew NiNode;
    m_pkCamXRot->SetSelectiveUpdate(true);
    m_pkCamXRot->SetSelectiveUpdateTransforms(true);
    m_pkCamXRot->SetSelectiveUpdatePropertyControllers(true);
    m_pkCamXRot->SetSelectiveUpdateRigid(false);
    m_spCamRootNode = NiNew NiNode;
    m_spCamRootNode->SetSelectiveUpdate(true);
    m_spCamRootNode->SetSelectiveUpdateTransforms(true);
    m_spCamRootNode->SetSelectiveUpdatePropertyControllers(true);
    m_spCamRootNode->SetSelectiveUpdateRigid(false);

    m_spCamRootNode->AttachChild(m_pkCamZRot);
    m_pkCamZRot->AttachChild(m_pkCamXRot);
    m_pkCamXRot->AttachChild(m_pkCameraNode);

    m_pkCamera = NiNew NiCamera;
    
    m_pkCameraNode->AttachChild(m_pkCamera);
    m_spCamRootNode->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spCamRootNode);
    m_spCamRootNode->IncRefCount();

    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    pkSharedData->SetScene(MSharedData::MAIN_CAMERA_INDEX, m_spCamRootNode);
    pkSharedData->Unlock();

    m_iWidth = 0;
    m_iHeight = 0;
    m_eUpAxis = Z_AXIS;
    m_bInvertPan = false;
}
//---------------------------------------------------------------------------
MCamera::~MCamera()
{
    
}
//---------------------------------------------------------------------------
void MCamera::DeleteContents()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    
    if (m_spCamRootNode)
        m_spCamRootNode->DecRefCount();
    m_spCamRootNode = NULL;
    m_pkCamXRot = NULL;
    m_pkCamZRot = NULL;
    m_pkCameraNode = NULL;
    m_pkCamera = NULL;

    pkSharedData->RemoveScene(MSharedData::MAIN_CAMERA_INDEX);

    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
NiCamera* MCamera::GetCamera()
{
    return m_pkCamera;
}
//---------------------------------------------------------------------------
void MCamera::CalcCameraFrustum(NiAVObject* pkScene)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiBound kSceneBound;
    MUtility::GetWorldBounds(pkScene, kSceneBound, false);
    float fSceneRadius = kSceneBound.GetRadius();
    float fRight, fTop;

    fRight = fTop = 1.0f / NiSqrt(3.0f);

    if (m_iWidth > m_iHeight)
        fRight *= (float) m_iWidth / (float) m_iHeight;
    else
        fTop *= (float) m_iHeight / (float) m_iWidth;

    if (fSceneRadius == 0.0f)
    {
        fSceneRadius = 100.0f;
    }

    NiFrustum fr;
    fr.m_fLeft = -fRight;
    fr.m_fRight = fRight;
    fr.m_fTop = fTop;
    fr.m_fBottom = -fTop;
    fr.m_fNear = 0.05f * fSceneRadius;
    fr.m_fFar = 40.0f * fSceneRadius; 
    m_pkCamera->SetViewFrustum(fr);

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MCamera::CalcCameraTransform(NiAVObject* pkScene)
{
    if(!m_pkCamera || !pkScene)
        return;

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiBound kSceneBound;
    NiPoint3 kSceneCenter, kInitialCameraPos;
    float fSceneRadius;

    MUtility::GetWorldBounds(pkScene, kSceneBound, false);
    kSceneCenter = kSceneBound.GetCenter();
    fSceneRadius = kSceneBound.GetRadius();

    if (fSceneRadius == 0.0f)
    {
        fSceneRadius = 100.0f;
    }

    // Rotate and position the camera so that we are looking down the world
    // y-axis and z is up.

    NiMatrix3 kRot;
    
    if (m_eUpAxis == MCamera::Z_AXIS)
    {
        kInitialCameraPos = NiPoint3(
            0.0f, 
            - 2.0f * fSceneRadius, 
            0.0f
        );

        kRot = NiMatrix3(
            NiPoint3(0.0f, 1.0f, 0.0f),
            NiPoint3(0.0f, 0.0f, 1.0f),
            NiPoint3(1.0f, 0.0f, 0.0f));
    }
    else if (m_eUpAxis == MCamera::Y_AXIS)
    {
        kInitialCameraPos = NiPoint3(
            0.0f, 
            0.0f, 
            2.0f * fSceneRadius
        );

        kRot = NiMatrix3(
            NiPoint3(0.0f, 0.0f, -1.0f),
            NiPoint3(0.0f, 1.0f, 0.0f),
            NiPoint3(1.0f, 0.0f, 0.0f));
    }

    m_pkCameraNode->SetRotate(kRot);
    m_pkCameraNode->SetTranslate(kInitialCameraPos);

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MCamera::Zoom(float fMultiplier, NiPoint3 kZoomToPoint, 
                   NiBound kZoomToBound)
{
    NiPoint3 kCamTrans = m_pkCameraNode->GetWorldTranslate();
    NiPoint3 kDiff = (kZoomToPoint - kCamTrans);
    NiPoint3 kTrans = kCamTrans + kDiff * fMultiplier;

    // The following equation will determine if two moving spheres are 
    // in collision at any point during the time span [0,1]. If so,
    // zooming in will get us too close.
    // This equation was taken from Mathematics for 3d Game Programming
    // and Computer Graphics by Eric Lengyel
    // Section 8.2.1 Collision of Two Spheres
    // pages 221-223

    NiPoint3 kP1 = kCamTrans; // The initial location of the camera
    NiPoint3 kP2 = kTrans;    // The calculated final location of the camera

    NiPoint3 kQ1 = kZoomToPoint; // The location of the object
    NiPoint3 kQ2 = kQ1; 
        // Since the object isn't moving, the values are the same

    NiPoint3 kVp = kP2 - kP1; // The velocity of the camera
    NiPoint3 kVq = NiPoint3::ZERO; 
        // The velocity of the object (it isn't moving)

    NiPoint3 kDeltaPos = kP1 - kQ1; // Difference of intial locations 
    NiPoint3 kDeltaVel = kVp - kVq; // Difference of velocities

    float fDeltaPosDotDeltaVel = kDeltaPos.Dot(kDeltaVel);
    float fDeltaPosSquared = kDeltaPos*kDeltaPos;
    float fDeltaVelSquared = kDeltaVel*kDeltaVel;
    float fRadP = 0.001f;  // Radius of the camera
    float fRadQ = kZoomToBound.GetRadius(); // Radius of the object
    float fDeltaPosDotDeltaVelSquared = 
        fDeltaPosDotDeltaVel * fDeltaPosDotDeltaVel;
    float fRadSumSquared= (fRadP + fRadQ)*(fRadP + fRadQ);

    // Calculate the potential time that these two spheres
    // could intersect. Since this is an instantaneous velocity test,
    // we are only concerned with time being within the range of 0 to 1.
    float fNumerator = -1.0f * fDeltaPosDotDeltaVel - 
        sqrt( fDeltaPosDotDeltaVelSquared - 
        fDeltaVelSquared * (fDeltaPosSquared - fRadSumSquared));
    float fTime =  fNumerator / fDeltaVelSquared;

    // If a colliion did not occur within our time range,
    // go ahead and perform the zoom
    if (fTime > 1.0f || fTime < 0.0f)
    {
        const NiTransform kTransform = m_pkCameraNode->GetParent()->
            GetWorldTransform();
        NiTransform kInvTransform;
        kTransform.Invert(kInvTransform);
        m_pkCameraNode->SetTranslate(kInvTransform * kTrans);
    }
}
//---------------------------------------------------------------------------
void MCamera::ZoomExtents()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    
    NiAVObject* pkBackground = pkSharedData->GetScene(
        pkSharedData->BACKGROUND_INDEX);

    NiAVObject* pkCharacter = pkSharedData->GetScene(
            pkSharedData->CHARACTER_INDEX);

    NiActorManager* pkActorManager = pkSharedData->GetActorManager();
    NiAVObject* pkModelRoot = NULL;

    if (pkActorManager)
    {
        pkModelRoot = pkActorManager->GetNIFRoot();
    }
        

    if (pkModelRoot)
    {
        CalcCameraTransform(pkModelRoot);
    }
    else if (pkBackground)
    {
        CalcCameraTransform(pkBackground);
    }
    else if (pkCharacter)
    {
        CalcCameraTransform(pkCharacter);
    }

    pkSharedData->Unlock();

}
//---------------------------------------------------------------------------
void MCamera::CenterToScene(NiAVObject* pkScene)
{
    if(!m_pkCamera || !pkScene)
        return;

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiBound kSceneBound;
    NiPoint3 kSceneCenter, kInitialCameraPos;
    float fSceneRadius;

    MUtility::GetWorldBounds(pkScene, kSceneBound, false);
    kSceneCenter = kSceneBound.GetCenter();
    fSceneRadius = kSceneBound.GetRadius();

    // Rotate and position the camera so that we are looking down the world
    // y-axis and z is up.

    NiMatrix3 kRot;
    float fModifier = 0.03f;
    if (m_eUpAxis == MCamera::Z_AXIS)
    {
        kInitialCameraPos = kSceneCenter + fModifier*
            fSceneRadius*NiPoint3::UNIT_Z;

        kRot = NiMatrix3(
            NiPoint3(0.0f, 1.0f, 0.0f),
            NiPoint3(0.0f, 0.0f, 1.0f),
            NiPoint3(1.0f, 0.0f, 0.0f));
    }
    else if (m_eUpAxis == MCamera::Y_AXIS)
    {
        kInitialCameraPos = kSceneCenter + fModifier*
            fSceneRadius*NiPoint3::UNIT_Y;

        kRot = NiMatrix3(
            NiPoint3(0.0f, 0.0f, -1.0f),
            NiPoint3(0.0f, 1.0f, 0.0f),
            NiPoint3(1.0f, 0.0f, 0.0f));
    }

    m_pkCameraNode->SetRotate(kRot);
    m_pkCameraNode->SetTranslate(kInitialCameraPos);

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MCamera::RotateX(float fXRot)
{
    NiMatrix3 kXRot;
    kXRot.MakeXRotation(fXRot);

    NiMatrix3 kCamXRot = m_pkCamXRot->GetRotate();
    kCamXRot = kXRot * kCamXRot;
    m_pkCamXRot->SetRotate(kCamXRot);
}
//---------------------------------------------------------------------------
void MCamera::RotateUp(float fUpRot)
{
    NiMatrix3 kUpRot;
    if (UpAxis == MCamera::Y_AXIS)
        kUpRot.MakeYRotation(fUpRot);
    else if (UpAxis == MCamera::Z_AXIS)
        kUpRot.MakeZRotation(fUpRot);

    NiMatrix3 kCamZRot = m_pkCamZRot->GetRotate();
    kCamZRot = kUpRot * kCamZRot;
    m_pkCamZRot->SetRotate(kCamZRot);
}
//---------------------------------------------------------------------------
void MCamera::SetTranslate(NiPoint3 kTranslate)
{
    m_spCamRootNode->SetTranslate(kTranslate);
}
//---------------------------------------------------------------------------
void MCamera::Update(float fTime)
{
    m_spCamRootNode->Update(fTime);
    NiMesh::CompleteSceneModifiers(m_spCamRootNode);
}
//---------------------------------------------------------------------------
bool MCamera::WindowPointToRay(int iX, int iY, NiPoint3& origin, NiPoint3& dir)
{
    NiRenderTargetGroup* pkRenderTarget =
        MFramework::Instance->Renderer->GetMainRenderTarget();
    return m_pkCamera->WindowPointToRay(iX, iY, origin, dir, pkRenderTarget);
}
//---------------------------------------------------------------------------
void MCamera::Reset()
{
    m_spCamRootNode->SetTranslate(NiPoint3::ZERO);
    m_spCamRootNode->SetRotate(NiMatrix3::IDENTITY);
    m_spCamRootNode->SetScale(1.0f);
    
    m_pkCamXRot->SetTranslate(NiPoint3::ZERO);
    m_pkCamXRot->SetRotate(NiMatrix3::IDENTITY);
    m_pkCamXRot->SetScale(1.0f);

    m_pkCamZRot->SetTranslate(NiPoint3::ZERO);
    m_pkCamZRot->SetRotate(NiMatrix3::IDENTITY);
    m_pkCamZRot->SetScale(1.0f);

    m_pkCameraNode->SetTranslate(NiPoint3::ZERO);
    m_pkCameraNode->SetRotate(NiMatrix3::IDENTITY);
    m_pkCameraNode->SetScale(1.0f);

    m_pkCamera->SetTranslate(NiPoint3::ZERO);
    m_pkCamera->SetRotate(NiMatrix3::IDENTITY);
    m_pkCamera->SetScale(1.0f);

    m_spCamRootNode->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spCamRootNode);
}
//---------------------------------------------------------------------------
void MCamera::GetWorldTranslate(NiPoint3& kTranslate)
{
    kTranslate = m_pkCamera->GetWorldTranslate();
}
//---------------------------------------------------------------------------
void MCamera::GetWorldRotate(NiMatrix3& kRotate)
{
    kRotate = m_pkCamera->GetWorldRotate();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
bool MCamera::get_Active()
{
    return m_pkCamera != NULL;
}
//---------------------------------------------------------------------------
int MCamera::get_ViewWidth()
{
    return m_iWidth;
}
//---------------------------------------------------------------------------
void MCamera::set_ViewWidth(int iWidth)
{
    m_iWidth = iWidth;
}
//---------------------------------------------------------------------------
int MCamera::get_ViewHeight()
{
    return m_iHeight;
}
//---------------------------------------------------------------------------
void MCamera::set_ViewHeight(int iHeight)
{
    m_iHeight = iHeight;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
MCamera::UpAxisType MCamera::get_UpAxis()
{
    return m_eUpAxis;
}
//---------------------------------------------------------------------------
void MCamera::set_UpAxis(MCamera::UpAxisType eType)
{
    m_eUpAxis = eType;
}
//---------------------------------------------------------------------------
bool MCamera::get_InvertPan()
{
    return m_bInvertPan;
}
//---------------------------------------------------------------------------
void MCamera::set_InvertPan(bool bInvert)
{
    m_bInvertPan = bInvert;
}
//---------------------------------------------------------------------------

