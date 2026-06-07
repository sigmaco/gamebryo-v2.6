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
#include "NiSceneViewerWindow.h"

//---------------------------------------------------------------------------
NiSceneViewerWindow::NiSceneViewerWindow()
{
    m_spScene = NULL;
    m_bOwnsShadowManager = false;
}
//---------------------------------------------------------------------------
NiSceneViewerWindow::~NiSceneViewerWindow()
{
}
//---------------------------------------------------------------------------
void  NiSceneViewerWindow::PreMainLoop()
{
    m_fStartTime = NiGetCurrentTimeInSec();
    m_fLastTime = 0.0f;
}
//---------------------------------------------------------------------------
void  NiSceneViewerWindow::PostMainLoop()
{
    if (m_pkSceneViewer)
        m_pkSceneViewer->End();
    Terminate();
}
//---------------------------------------------------------------------------
long NiSceneViewerWindow::MainLoopTick()
{
    long lReturnValue = 0;
   
    if (this == NULL || m_pkSceneViewer == NULL)
        return 0;

    if (!m_bIsAlive)
        return 0;
    m_fElapsedTime = NiGetCurrentTimeInSec() - m_fStartTime;

    //  Process system and input
    if (!Process(&lReturnValue))
        return 0;

    float fDeltaTime = m_fElapsedTime - m_fLastTime;
    if (fDeltaTime >= 0.02)
    {
        m_pkSceneEventHandler->GenerateIdleCommands();
        m_fLastTime = m_fElapsedTime;

        // We've done the necessary processing, now we need to update
        // the state of the UI devices

        if (m_pkKeyboard && m_pkSceneEventHandler->IsKeyboardActive())
            m_pkKeyboard->UpdateDevice();
        if (m_pkMouse && m_pkSceneEventHandler->IsMouseActive())
            m_pkMouse->UpdateDevice();
    }

    return 1;
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::Initialize(NiScenePtr spScene, 
    NiSceneChangeInfo* pkChangeInfo)
{
    spScene->CheckOutSceneGraph(m_spScene, true);
    m_spTheSceneDatabase = spScene;

    if (!NiShadowManager::GetShadowManager())
    {
        m_bOwnsShadowManager = true;
        NiShadowManager::Initialize();
    }

    if (!CreateRenderer())
        return false;
    
    if (!CreateScene(spScene))
        return false;

    if (!CreateCamera(spScene))
        return false;
    
    m_pkSceneViewer= NiNew NiSceneViewer(m_spRenderer);
    
    m_pkSceneViewer->SetBackgroundColor(m_kBackground);
    m_pkSceneChangeInfo = pkChangeInfo;
    m_pkSceneViewer->SetSceneManager(spScene);
    m_pkSceneViewer->SetMasterScene(m_spScene);
    
    NiNode* pkChildNode = (NiNode*) m_spScene->GetAt(0);

    if (!SceneHasLights(m_spScene))
    {
        m_spDefaultLight = NiNew NiAmbientLight();
        m_spDefaultLight->SetName("<Default SceneViewer Light>");
        m_spDefaultLight->AttachAffectedNode(m_spScene);
        NiOutputDebugString("Adding default lighting to Scene\n");
        m_spDefaultLight->UpdateEffects();
        m_spScene->UpdateEffects();
    }
    
    m_pkSceneViewer->CollectCameras(m_spScene);
    for (unsigned int ui = 0; ui < m_pkSceneViewer->GetCameraCount(); ui++)
    {
        NiCamera* pkCamera = m_pkSceneViewer->GetCamera(ui);
        CalcCameraFrustum(pkCamera);
    }
    m_pkSceneViewer->SelectCamera(0);

    m_pkSceneViewer->SetScene(pkChildNode);
    spScene->CheckInSceneGraph(pkChildNode);

    m_pkSceneEventHandler->SetSceneViewer(m_pkSceneViewer);

    //Screen Console related setup
    m_pkSceneViewer->CreateCameraLabel();
    m_pkSceneViewer->SetCameraLabelLocation(10, 10);

    m_pkSceneViewer->Begin();
    m_bIsAlive = true;
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::Initialize(const char* pcFilename, 
    NiSceneChangeInfo* pkChangeInfo)
{
    NiStream kStream;

    if (!kStream.Load(pcFilename))
    {
        NiMessageBox("Unable to open file!", "Error");
        return false;
    }
    NiNode* pkNode = (NiNode*)(kStream.GetObjectAt(0));

    NiScenePtr spScene = NiNew NiScene();
    spScene->CheckInSceneGraph(pkNode);
    return Initialize(spScene, pkChangeInfo);
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::CloseScene()
{
    m_spCamera = 0;
    m_spDefaultLight = 0;
    m_spScene = 0;

    // The shadow manager needs to be shutdown after the scene has been 
    // destroyed, but before the renderer is destroyed.
    if (m_bOwnsShadowManager && NiShadowManager::GetShadowManager())
    {
        NiShadowManager::Shutdown();
    }

    m_spRenderer = 0;
    if (m_pkSceneViewer != NULL)
        m_pkSceneViewer->End();
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::CreateCamera(NiScenePtr spScene)
{
    if (m_spDefaultCamNode != NULL)
    {
        bool bFoundCamera = false;
        for (unsigned int ui = 0; ui < m_spDefaultCamNode->GetArrayCount() //&&
            //!bFoundCamera
            ; ui++)
        {
            NiAVObject* pkChild = m_spDefaultCamNode->GetAt(ui);
            if (NiIsKindOf(NiCamera, pkChild))
            {
                bFoundCamera = true;
                m_spCamera = (NiCamera*) pkChild;
                m_spScene->AttachChild(m_spDefaultCamNode);
            }
        }

        //if (bFoundCamera)
        //    m_spScene->AttachChild(m_spDefaultCamNode);   
    }

    if (m_spCamera == NULL)
    {
        m_spCamera = NiNew NiCamera;
        NIASSERT(m_spCamera != NULL);
        m_spCamera->SetName("<default camera>");

        m_spScene->AttachChild(m_spCamera);
        CalcCameraFrustum(m_spCamera);
        CalcInitialCameraTransform(m_spCamera);
        NiSceneViewer::SetupCameraExtraData(false, m_spCamera);
        m_spCamera->Update(0.0f);
        NiMesh::CompleteSceneModifiers(m_spCamera);
    }

    return true;
}

//---------------------------------------------------------------------------
void NiSceneViewerWindow::CalcCameraFrustum(NiCamera* pkCamera) const
{
    if (!pkCamera || !m_spScene)
        return;

    bool bIsDefaultCamera = false;
    float fSceneRadius = m_spScene->GetWorldBound().GetRadius();

    pkCamera->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkCamera);
    NiFrustum kFrustum = pkCamera->GetViewFrustum();
 
    float fWidth = (float) GetWidth();
    float fHeight = (float) GetHeight();
    
    float fWindowAspectRatio = (fHeight / fWidth);
    float fRight = kFrustum.m_fRight;
    float fLeft = kFrustum.m_fLeft;
    float fTop = fRight * fWindowAspectRatio;
    float fBottom = -fTop;
    float fFar = kFrustum.m_fFar;
    float fNear = kFrustum.m_fNear;
    bool bOrtho = kFrustum.m_bOrtho;

    NiBooleanExtraData* pkUserCamED = (NiBooleanExtraData*) 
        pkCamera->GetExtraData(IS_USER_CAM);

    if (pkUserCamED && pkUserCamED->GetValue() == true)
        bIsDefaultCamera = false;
    else
        bIsDefaultCamera = true;

    if (bIsDefaultCamera)
    {
        fFar = fSceneRadius * 4.0f;
        fNear = 0.01f;
        NiPoint3 kCameraLocation = pkCamera->GetTranslate();
        NiPoint3 kWorldCenter = (m_spScene->GetWorldBound()).GetCenter();
        NiPoint3 kDistanceVector = kCameraLocation - kWorldCenter;
        float fDist = kDistanceVector.Length();
        fFar += fDist;
        fFar > 0.0f ? 0 : fFar = 5.0f;

        NiFrustum fr;
        fr.m_fLeft = fLeft;
        fr.m_fRight = fRight;
        fr.m_fTop = fTop;
        fr.m_fBottom = fBottom;
        fr.m_fNear = fNear;
        fr.m_fFar = fFar;
        fr.m_bOrtho = bOrtho;

        pkCamera->SetViewFrustum(fr);
        pkCamera->Update(0.0f);
        NiMesh::CompleteSceneModifiers(pkCamera);
    } 
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::CalcInitialCameraTransform(NiCamera *pkCamera) const
{
    NiBound kSceneBound;
    NiPoint3 kSceneCenter, kInitialCameraPos;
    float fSceneRadius;

    kSceneBound = m_spScene->GetWorldBound();
    kSceneCenter = kSceneBound.GetCenter();
    fSceneRadius = kSceneBound.GetRadius();

    // Rotate and position the camera so that we are looking down the world
    // y-axis and z is up.

    kInitialCameraPos = NiPoint3(
        kSceneCenter.x, 
        kSceneCenter.y - 2.0f * fSceneRadius, 
        kSceneCenter.z
    );

    NiMatrix3 kRot(
        NiPoint3(0.0f, 1.0f, 0.0f),
        NiPoint3(0.0f, 0.0f, 1.0f),
        NiPoint3(1.0f, 0.0f, 0.0f));

    pkCamera->SetRotate(kRot);
    pkCamera->SetTranslate(kInitialCameraPos);
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::CreateScene(NiScenePtr spScene)
{
    NiNode* m_pkSceneGraph = m_spScene;
    m_pkSceneGraph->SetName("<loaded scene graph>");
    m_spScene = NiNew NiNode;
    m_spScene->SetSelectiveUpdate(true);
    m_spScene->SetSelectiveUpdateRigid(false);

    if (m_pkSceneGraph != NULL)
        m_spScene->AttachChild(m_pkSceneGraph);

    m_spScene->SetName("<root node>");
    m_spScene->SetTranslate(NiPoint3::ZERO);
    m_spScene->SetRotate(NiMatrix3::IDENTITY);
    m_spScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    m_spScene->UpdateNodeBound();
    return true;
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::SetAnimationLooping(bool bLoop)
{
    if (m_pkSceneViewer)
        m_pkSceneViewer->SetAnimationLooping(bLoop);
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::GetAnimationLooping()
{
    if (m_pkSceneViewer)
        return m_pkSceneViewer->GetAnimationLooping();
    else
        return false;
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::SetAnimationLoopEndTime(float fEndTime)
{
    if (m_pkSceneViewer)
        m_pkSceneViewer->SetLoopEndTime(fEndTime);
}
//---------------------------------------------------------------------------
float NiSceneViewerWindow::GetAnimationLoopEndTime()
{
    if (m_pkSceneViewer)
        return m_pkSceneViewer->GetLoopEndTime();
    else
        return false;
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::SetAnimationLoopStartTime(float fStartTime)
{
    if (m_pkSceneViewer)
        m_pkSceneViewer->SetLoopStartTime(fStartTime);
}
//---------------------------------------------------------------------------
float NiSceneViewerWindow::GetAnimationLoopStartTime()
{
    if (m_pkSceneViewer)
        return m_pkSceneViewer->GetLoopStartTime();
    else
        return false;
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::SetFrameRateLimit(unsigned int uiCeiling)
{
    if (m_pkSceneViewer)
        m_pkSceneViewer->SetCameraClickCeiling(uiCeiling);    
}
//---------------------------------------------------------------------------
unsigned int NiSceneViewerWindow::GetFrameRateLimit()
{
    if (m_pkSceneViewer)
        return m_pkSceneViewer->GetCameraClickCeiling();
    else
        return false;
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::SceneHasLights(NiNode* pkScene)
{
   for (unsigned int ui = 0; ui < pkScene->GetArrayCount(); ui++)
   {
       NiAVObject* pkObject = pkScene->GetAt(ui);
       if (NiIsKindOf(NiLight, pkObject))
       {
           return true;
       }
       else if (NiIsKindOf(NiNode, pkObject))
       {
           if (SceneHasLights((NiNode*)pkObject))
                return true;
       }
   }
   return false;
}
//---------------------------------------------------------------------------
void NiSceneViewerWindow::SetDefaultCamera(NiNodePtr spCamNode)
{
    if (m_pkSceneViewer)
    {
        NiNode* pkScene = m_pkSceneViewer->GetMasterScene();
        pkScene->DetachChild(m_spDefaultCamNode);
        pkScene->AttachChild(spCamNode);
    }
    m_spDefaultCamNode = spCamNode;
}
//---------------------------------------------------------------------------
bool NiSceneViewerWindow::ReinitializeCameras()
{   
    if (m_pkSceneViewer == NULL)
    {
        return false;
    }

    NiNode* pkScene = m_pkSceneViewer->GetScene();
    m_pkSceneViewer->EmptyCameraList();
    m_pkSceneViewer->CollectCameras(m_pkSceneViewer->GetMasterScene());
    m_pkSceneViewer->SelectCamera(0);
    m_pkSceneViewer->SetScene(pkScene);
    return true;
}
//---------------------------------------------------------------------------
NiSceneViewer* NiSceneViewerWindow::GetSceneViewer()
{
    return m_pkSceneViewer;
}
//---------------------------------------------------------------------------
