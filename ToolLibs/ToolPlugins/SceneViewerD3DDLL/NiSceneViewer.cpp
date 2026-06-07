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
#include "NiSceneViewer.h"
#include "NiSceneCommand.h"
#include "NiOrbitCamera.h"

#include <Ni2DStringRenderClick.h>
#include <NiMaterialToolkit.h>
#include <NiD3DShaderFactory.h>
#include <NiD3D10ShaderFactory.h>
#include <NiToolPipelineCloneHelper.h>
#include <NiMeshTools.h>
#include <NiMeshCullingProcess.h>

//---------------------------------------------------------------------------
char *NiSceneViewer::ms_pcFontPath = NULL;
//---------------------------------------------------------------------------
NiSceneViewer::NiSceneViewer(NiRenderer* pkRenderer) :
    m_kVisible(1024, 1024),
    m_kScreenElements(5, 5)
{
    NIASSERT(pkRenderer != NULL);
    m_spRenderer = pkRenderer;
    m_pkCameras = NiNew NiTPrimitiveArray<Camera*>(10, 5);
    m_iCameraIndex = -1;
    m_spCamera = NULL;
    m_pkCommandQueue = NULL;
    m_fAnimationSpeed = 1.0f;
    m_bAnimationMode = true;
    m_kBackgroundColor = NiColor::BLACK;
    for (int i = 0; i < NUM_LOCK_TARGETS; i++)
        m_abLocks[i] = false;
    m_uiNumCameraClicks = 0;
    m_fLastSampleTime = NiGetCurrentTimeInSec();
    m_fAnimLastTime = m_fLastSampleTime;
    m_fAnimAccumTime = 0.0f;
    m_bCameraLabel = false;
    m_fCurrentTime = 0.0f;

    m_pkMasterScene = NiNew NiNode();
    m_fWorldRadius = 0.0f;
    m_uiInsertionIndex = 0;
    m_uiCameraClickCeiling = 60;
    m_bAnimationLooping = false;
    m_fAnimLoopStartTime = 0;
    m_fAnimLoopEndTime = 0;
    m_bAnimateExternally = false;

    m_fMinStartTime = NI_INFINITY;

    m_spMainRenderView = NULL;
    m_spFrame = NULL;
    m_kShadowRenderStepName = "NiApplication Shadow Render Step";
    m_spShadowRenderStep = NULL;
}
//---------------------------------------------------------------------------
NiSceneViewer::~NiSceneViewer()
{
    Lock(RENDERER_LOCK);
    Lock(SCENE_LOCK);
    if (m_spRenderer)
    {
        NiOverdrawIndicatorCommand::CleanUp();
        m_spRenderer->PurgeAllRendererData(m_pkMasterScene);
    }

    RecursiveReleaseDataStreams((NiNode*)m_pkMasterScene);

    m_pkMasterScene->RemoveAllChildren();

    EmptyCameraList();
    m_spCamera = NULL;
    m_spCameraLabel = NULL;
    m_spLabelFont = NULL;

    m_kScreenElements.RemoveAll();

    Unlock(RENDERER_LOCK);
    Unlock(SCENE_LOCK);
    NiDelete m_pkCameras;
    while (m_pkCommandQueue != NULL && !m_pkCommandQueue->IsEmpty())
        NiDelete m_pkCommandQueue->Remove();
    m_pkMasterScene = NULL;
    NiDelete m_pkCommandQueue;

    m_spMainRenderView = 0;
    m_spFrame = 0;
}
//---------------------------------------------------------------------------
void NiSceneViewer::RecursiveReleaseDataStreams(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;
        pkMesh->RemoveAllStreamRefs();
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        const NiUInt32 uiArrayCount = pkNode->GetArrayCount();
        for (NiUInt32 i = 0; i < uiArrayCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                RecursiveReleaseDataStreams(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool NiSceneViewer::SetScene(NiNodePtr spScene)
{
    if (spScene == NULL)
        return false;

    NiToolPipelineCloneHelper kCloneHelper(spScene);
    NiMeshTools::ConvertStreamsToPlatformSpecific(kCloneHelper);

    if (GetWorldRadius() == 0.0f)
        SetWorldRadius(spScene->GetWorldBound().GetRadius());

    m_pkScene = spScene;
    m_pkMasterScene->SetAt(m_uiInsertionIndex, NULL);
    m_fMinStartTime = NiTimeController::GetMinBeginKeyTime(m_pkScene);
    RunUpScene();

    return m_pkSceneDB->CheckInSceneGraph(m_pkScene);
}
//---------------------------------------------------------------------------
bool NiSceneViewer::SetScene(char* pcFilename)
{
    NIASSERT(pcFilename != NULL);
    if (isUnlocked(SCENE_LOCK))
    {
        NiStream kStream;
        kStream.Load(pcFilename);
        return SetScene((NiNode*)(kStream.GetObjectAt(0)));
    }
    else 
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiNode* NiSceneViewer::GetScene()
{
    if (m_pkScene != NULL)
    {
        return m_pkScene;
    }
    else if (m_pkSceneDB->CheckOutSceneGraph(m_pkScene, false))
    {
        m_pkMasterScene->SetAt(m_uiInsertionIndex, m_pkScene);
        
        return m_pkScene;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiRenderer* NiSceneViewer::GetRenderer()
{
    return m_spRenderer;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::SelectCamera(unsigned int uiWhichCamera)
{
    unsigned int uiPreviousCamera = GetCurrentCameraID();
    NIASSERT(uiWhichCamera >= 0 && uiWhichCamera < m_pkCameras->GetSize());
    m_iCameraIndex = uiWhichCamera;

    Camera* pkCamera = m_pkCameras->GetAt(uiWhichCamera);
    if (pkCamera)
    {
        NiDelete pkCamera->m_pkCameraOrientation;
        pkCamera->m_pkCameraOrientation = 
            NiNew NiViewerCamera(pkCamera->m_spCamera);
    }

    m_spCamera = GetCamera(m_iCameraIndex, true);

    if (m_spCameraLabel)
    {
        m_spCameraLabel->SetText(m_spCamera->GetName());
    }

    if (m_spCamera != NULL && m_spCamera->GetParent() != NULL)
    {
    }
    else
    {
        if (!SelectCamera(uiPreviousCamera))
            m_spCamera = NULL;
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::AddCamera(NiCamera* pkCamera)
{
    if (isUnlocked(CAMERA_LOCK))
    {
        if (pkCamera->GetName() == NULL)
        {
            NiNode* pkNode = pkCamera->GetParent();
            // Name the pkCamera after its parents if it is unnamed
            while (pkNode != NULL && pkCamera->GetName() == NULL)
            {
                if (pkNode->GetName() != NULL)
                     pkCamera->SetName(pkNode->GetName());
                 else
                     pkNode = pkNode->GetParent();
            }

            // Still no name? Give it a default name
            if (pkCamera->GetName() == NULL)
                pkCamera->SetName("Unknown Camera");
        }

        SetupCameraExtraData(true, pkCamera);

        Camera* pkCameraHolder = NiNew Camera;
        pkCameraHolder->m_spCamera = pkCamera;
        pkCameraHolder->m_kInitialRot = pkCamera->GetRotate();
        pkCameraHolder->m_kInitialPos = pkCamera->GetTranslate();
        pkCameraHolder->m_pkCameraOrientation = NiNew NiViewerCamera(pkCamera);
        m_pkCameras->Add(pkCameraHolder);
        m_iCameraIndex = 0;
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiCamera* NiSceneViewer::GetCamera(unsigned int uiWhichCamera, bool)
{
    NIASSERT(uiWhichCamera >= 0 && uiWhichCamera < m_pkCameras->GetSize());
    m_pkCameras->GetSize();
    Camera* pkCameraHolder = m_pkCameras->GetAt(uiWhichCamera);
    if (pkCameraHolder->m_spCamera->GetParent() != NULL)
        return pkCameraHolder->m_spCamera;
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiSceneViewer::PrintOutSceneGraph(NiNode* pkScene)
{
    NiNode* pkTraverserNode = pkScene;
    if (pkTraverserNode != NULL)
    {
        const char* pcName = pkTraverserNode->GetName();
        if (pcName != NULL)
            NiOutputDebugString(pcName);
        else
            NiOutputDebugString("This node has no name.");
        NiOutputDebugString("\n");
        
        for (unsigned int ui = 0; ui < pkTraverserNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkTraverserNode->GetAt(ui);
            if (pkChild != NULL)
            {
                NIASSERT(NiIsKindOf(NiAVObject, pkChild));
                if (NiIsKindOf(NiNode, pkChild))
                    PrintOutSceneGraph((NiNode*)pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiSceneViewer::Begin()
{
    m_pkScene = GetScene();

    NiMaterialToolkit::UnloadShaders();

    if (NiIsKindOf(NiDX9Renderer, GetRenderer()))
    {
        NiD3DShaderFactory* pkD3DFactory = 
            NiD3DShaderFactory::GetD3DShaderFactory();

        if (!pkD3DFactory->IsActiveFactory())
            pkD3DFactory->SetAsActiveFactory();
    }
    else
    {
        NiD3D10ShaderFactory* pkD3D10Factory = 
            NiD3D10ShaderFactory::GetD3D10ShaderFactory();

        if (!pkD3D10Factory->IsActiveFactory())
            pkD3D10Factory->SetAsActiveFactory();
    }
    NiMaterialToolkit::ReloadShaders();

    NiMaterialHelpers::RegisterMaterials(m_pkScene, GetRenderer());

    ValidateInstancingData(m_pkScene);

    PrintOutSceneGraph(m_pkMasterScene);
}
//---------------------------------------------------------------------------
void NiSceneViewer::MainLoop()
{
    // Do the initial timing calculations for the main loop
    bool bSecondPassed = false;
    float fOldTime =  m_fCurrentTime;
    float fTempTime = NiGetCurrentTimeInSec();
    float fDeltaRealTime = fTempTime - m_fCurrentTime;
    m_fCurrentTime = fTempTime;
    float fDeltaAccumRealTime = m_fCurrentTime  - m_fLastSampleTime;
        
    // Update variables that will be used in gathering statistics
    // We revise statistics on a per-second basis
    if (fDeltaAccumRealTime >= 1)
    {
        m_uiNumCameraClicks = 0;
        m_fLastSampleTime = m_fCurrentTime;
        bSecondPassed = true;
    }
    
    // Frame Rate Limiter. We use the ceiling value as the max number of 
    // pkCamera clicks that should occur per second. If the time elapsed since 
    // the last pkCamera click is too small, we wait until the next loop to 
    // render
    if (!bSecondPassed && fDeltaRealTime < 
        (1.0f / ((float)m_uiCameraClickCeiling)))
    {
        //NiNiOutputDebugString("breaking early\n");
        m_fCurrentTime = fOldTime;
        return;
    }

    // Since this framework is meant to be used in threaded sessions, we need 
    // to lock the rendering loop

    Lock(SCENE_LOCK);
    Lock(RENDERER_LOCK);
    Lock(CAMERA_LOCK);
    //NiOutputDebugString("BEGIN Rendering in MainLoopTick\n");
    // If there is no scene to render, we opt out
    if (m_pkScene == NULL || m_spCamera == NULL)
    {
        Unlock(SCENE_LOCK);
        Unlock(RENDERER_LOCK);
        Unlock(CAMERA_LOCK);
        return;
    }

    float fDeltaTime = 0.0;
    // Animation timing is divorced from the actual real time. It is possible
    // to speed up or slow down animation time, and those calculations are 
    // done here
    if (m_bAnimationMode)
    {
        fDeltaTime = m_fAnimationSpeed * (m_fCurrentTime - m_fAnimLastTime);

        m_fAnimAccumTime += fDeltaTime;

        if (GetAnimationLooping() && m_fAnimAccumTime > m_fAnimLoopEndTime)
        {
            ResetAnimations();
        }
    }

    if (m_bAnimateExternally)
    {
        // The scenegraph is updated to reflect the current time
        m_pkMasterScene->UpdateSelected(m_fAnimCurrentTime);

    }
    else if (fDeltaTime != 0.0f)
    {
        // The scenegraph is updated to reflect the current time
        m_pkMasterScene->UpdateSelected(m_fAnimAccumTime);
    }

    // All scene commands that have been gathered from the user interface 
    // are now performed
    HandleSceneCommands();

    m_spCamera->Update(m_fAnimAccumTime);
    NiMesh::CompleteSceneModifiers(m_spCamera);

    if (!m_spFrame) 
        CreateFrame();
    
    m_spMainRenderView->SetCamera(m_spCamera);
    m_spFrame->Draw();
    m_spFrame->Display();

    m_uiNumCameraClicks++;

    // Set the animation last time count for the next loop iteration
    m_fAnimLastTime = m_fCurrentTime;

    // Return the scenegraph resources for external use
    //NiOutputDebugString("END Rendering in MainLoopTick\n");
    Unlock(SCENE_LOCK);
    Unlock(RENDERER_LOCK);
    Unlock(CAMERA_LOCK);
}
//---------------------------------------------------------------------------
void NiSceneViewer::End()
{
    m_pkScene = GetScene();

    NiMaterialHelpers::UnRegisterMaterials(m_pkScene, GetRenderer());
}
//---------------------------------------------------------------------------
void NiSceneViewer::SubmitCommands(NiSceneCommandQueue* pkCommands)
{
    while (!isUnlocked(COMMAND_LOCK))
    {
        // WAIT
    }

    if (m_pkCommandQueue == NULL && pkCommands != NULL)
    {
        m_pkCommandQueue = pkCommands;  
    }
    else if (m_pkCommandQueue != NULL && pkCommands != NULL)
    {
        while (!pkCommands->IsEmpty())
            m_pkCommandQueue->Add(pkCommands->Remove());
        NiDelete pkCommands;
    }
}
//---------------------------------------------------------------------------
bool NiSceneViewer::Lock(LockTarget eTarget)
{
    /*NiOutputDebugString("LOCKING:");
    switch(kTarget)
    {
    case SCENE_LOCK:
        {
            NiOutputDebugString("SCENE...\n");
        }
        break;
    case CAMERA_LOCK:
        {
            NiOutputDebugString("pkCamera...\n");
        }
        break;
    case RENDERER_LOCK:
        {
            NiOutputDebugString("RENDERER...\n");
        }
        break;
    case COMMAND_LOCK:
        {
            NiOutputDebugString("COMMAND...\n");
        }
        break;
    }*/
    if (eTarget == SCENE_LOCK)
    {
        if (GetScene())
        {
            m_abLocks[(int)eTarget] = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    m_akCritSecs[(int)eTarget].Lock();
    m_abLocks[(int)eTarget] = true;
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::Unlock(LockTarget eTarget)
{
    /*NiOutputDebugString("UNLOCKING:");
    switch(kTarget)
    {
    case SCENE_LOCK:
        {
            NiOutputDebugString("SCENE...\n");
        }
        break;
    case CAMERA_LOCK:
        {
            NiOutputDebugString("pkCamera...\n");
        }
        break;
    case RENDERER_LOCK:
        {
            NiOutputDebugString("RENDERER...\n");
        }
        break;
    case COMMAND_LOCK:
        {
            NiOutputDebugString("COMMAND...\n");
        }
        break;
    }*/
    if (eTarget == SCENE_LOCK)
    {
        if (m_pkScene)
        {
            m_abLocks[(int)eTarget] = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    if (m_akCritSecs[(int)eTarget].GetCurrentLockCount() != 0)
    {
        m_akCritSecs[(int)eTarget].Unlock();
    }

    if (m_akCritSecs[eTarget].GetCurrentLockCount() == 0)
    {    
        m_abLocks[(int)eTarget] = false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::isUnlocked(LockTarget kTarget)
{
    return !m_abLocks[(int) kTarget];
}
//---------------------------------------------------------------------------
void NiSceneViewer::HandleSceneCommands()
{
    NiViewerCamera* pkCameraOrientation = GetCurrentCameraOrientation();
    bool bCommandExistsForCurrentCameraState = false;

    NiSceneCommand* pkCommand;
    NiSceneCommand* pkSwitchCameraStateCommand = NULL;

    while (m_pkCommandQueue != NULL && !m_pkCommandQueue->IsEmpty())
    {
        pkCommand = m_pkCommandQueue->Remove();
        if (pkCommand)
        {
            bool bDoCommand = false;
            if ( pkCommand->GetApplyType() == NiSceneCommand::APPLY_ANY)
            {
                bDoCommand = true;
            }
            else if (pkCommand->GetApplyType() == 
                pkCameraOrientation->GetApplyType())
            {
                bDoCommand = true;
            }
            
            if (pkCommand->CanChangeCameraStates())
            {
                // We know that this command is the same type as the current
                // pkCamera state, so we go on
                if (pkCommand->GetApplyType() == 
                    pkCameraOrientation->GetApplyType()) 
                {
                    bCommandExistsForCurrentCameraState = true;
                    bDoCommand = true;
                }
                // We know that this command changes us to a pkCamera state
                // other than what we are currently in. We will change over if
                // the command queue does not contain a command in the previous
                // state type. Note: This is how we revert back to the normal
                // pkCamera when we exit Orbit pkCamera mode.
                else if (pkSwitchCameraStateCommand == NULL)
                {
                    pkSwitchCameraStateCommand = pkCommand;
                    bDoCommand = false;
                }
            }

            if (bDoCommand)
            {
                pkCommand->Apply(this);
                NiDelete pkCommand;
            }
        }
    }

    if (!bCommandExistsForCurrentCameraState || 
        pkSwitchCameraStateCommand != NULL)
    {
        // Switch to something other than the default pkCamera rotate model
        if (pkSwitchCameraStateCommand != NULL)
        {
            pkSwitchCameraStateCommand->Apply(this);
            NiDelete pkSwitchCameraStateCommand;
        }
        /*// Switch to the default pkCamera rotate model
        //else if (!NiIsKindOf(NiViewerCamera, pkCameraOrientation))
        else if (pkCameraOrientation->GetApplyType() != APPLY_ROTATE_CAM)
        {
            SetCurrentCameraOrientation(NiNew NiViewerCamera(m_spCamera));
        }*/
    }

    m_pkScene->UpdateProperties();
    m_pkScene->UpdateEffects();
}
//---------------------------------------------------------------------------
unsigned int NiSceneViewer::GetCurrentCameraID()
{
    return m_iCameraIndex;
}
//---------------------------------------------------------------------------
void NiSceneViewer::CollectCameras(NiNode* pkScene)
{
    if (pkScene == NULL)
        return;
    for (unsigned int i = 0; i < pkScene->GetArrayCount(); i++)
    {
        NiAVObject* pkObject = pkScene->GetAt(i);
        if (pkObject != NULL && NiIsKindOf(NiCamera, pkObject))   
            AddCamera((NiCamera*)pkObject);
        else if (pkObject != NULL && NiIsKindOf(NiNode, pkObject))
            CollectCameras((NiNode*)pkObject);
    }
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetAnimationMode(bool bIsAnimated)
{
    m_bAnimationMode = bIsAnimated;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::GetAnimationMode()
{
    return m_bAnimationMode;
}
//---------------------------------------------------------------------------
void NiSceneViewer::ResetAnimations()
{
    if (GetAnimationLooping())
    {
        m_fAnimAccumTime = m_fAnimLoopStartTime;
    }
    else
    {
        m_fAnimAccumTime = 0.0f;
    }

    RunUpScene();
}
//---------------------------------------------------------------------------
void NiSceneViewer::RunUpScene()
{
    float fCurrentTime;
    if (m_bAnimateExternally)
    {
        fCurrentTime = m_fAnimCurrentTime;
    }
    else
    {
        fCurrentTime = m_fAnimAccumTime;
    }

    if (m_fMinStartTime < fCurrentTime)
    {
        // Update the scene at 60 FPS until the current time.
        for (float fStepTime = m_fMinStartTime; fStepTime < fCurrentTime;
            fStepTime += 0.0167f)
        {
            m_pkMasterScene->UpdateSelected(fStepTime);
        }
    }
}
//---------------------------------------------------------------------------
unsigned int NiSceneViewer::GetCameraCount()
{
    return m_pkCameras->GetSize();
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetAnimationSpeed(float fSpeed)
{
    if (fSpeed > 0)
        m_fAnimationSpeed = fSpeed;
}
//---------------------------------------------------------------------------
float NiSceneViewer::GetAnimationSpeed()
{
    return m_fAnimationSpeed;
}
//---------------------------------------------------------------------------
NiMatrix3& NiSceneViewer::GetCameraInitialRotation(unsigned int uiWhichCamera)
{
    NIASSERT(uiWhichCamera >= 0 && uiWhichCamera < m_pkCameras->GetSize());
    Camera* pkCameraHolder = m_pkCameras->GetAt(uiWhichCamera);
    return pkCameraHolder->m_kInitialRot;
}
//---------------------------------------------------------------------------
NiPoint3& NiSceneViewer::GetCameraInitialLocation(unsigned int uiWhichCamera) 
{
    NIASSERT(uiWhichCamera >= 0 && uiWhichCamera < m_pkCameras->GetSize());
    Camera* pkCameraHolder = m_pkCameras->GetAt(uiWhichCamera);
    return pkCameraHolder->m_kInitialPos;
}
//---------------------------------------------------------------------------
void NiSceneViewer::GetCameraList(NiTPrimitiveArray<NiCamera*>& kCameras)
{
    kCameras.RemoveAll();
    kCameras.SetSize(m_pkCameras->GetSize());
    for (unsigned int i = 0; i < m_pkCameras->GetSize(); i++)
    {
        kCameras.SetAt(i, m_pkCameras->GetAt(i)->m_spCamera);
    }
    kCameras.Compact();
}
//---------------------------------------------------------------------------
void NiSceneViewer::CreateCameraLabel()
{
    // create screen label
    char acFontPath[255];
    NiSprintf(acFontPath, 255, "%s%s", ms_pcFontPath, "CourierNew10.NFF");
    m_spLabelFont = NiFont::Create(GetRenderer(), acFontPath);

    NiColorA kColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_spCameraLabel = NiNew Ni2DString(
        m_spLabelFont, NiFontString::COLORED, 64, "");
    m_spCameraLabel->SetColor(kColor);
    if (m_spCamera)
    {
        m_spCameraLabel->SetText(m_spCamera->GetName());
    }
}
//---------------------------------------------------------------------------
void NiSceneViewer::EnableCameraLabel(bool bOn)
{
    m_bCameraLabel = bOn;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::IsCameraLabelEnabled()
{
    return m_bCameraLabel;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetCameraLabelLocation(int iX, int iY)
{
    NiPoint2 kPoint((float)iX, (float)iY);
    m_spCameraLabel->SetPosition((short)iX, (short)iY);
}
//---------------------------------------------------------------------------
Ni2DString* NiSceneViewer::GetCameraLabel()
{
    return m_spCameraLabel;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetCameraLabelFontPath(const char* pcPath)
{
    if (ms_pcFontPath)
    {
        NiFree(ms_pcFontPath);
    }

    ms_pcFontPath = NiStrdup(pcPath);
}
//---------------------------------------------------------------------------
bool NiSceneViewer::SetSceneManager(NiScene* pkScene)
{
    if (!pkScene)
        return false;
    
    m_pkSceneDB = pkScene;
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::SetMasterScene(NiNode* pkMasterScene)
{
   if (!pkMasterScene)
        return false;
    
    m_pkMasterScene = pkMasterScene;
    m_fMinStartTime = NiTimeController::GetMinBeginKeyTime(m_pkMasterScene);
    RunUpScene();
    m_uiInsertionIndex = 0;
    return true;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetBackgroundColor(NiColor kColor)
{
    m_kBackgroundColor = kColor;
    if (m_spRenderer != NULL)
    {
        Lock(RENDERER_LOCK);
        //NiMessageBox("Setting bg color","Color!");
        m_spRenderer->SetBackgroundColor(kColor);
        Unlock(RENDERER_LOCK);
    
    }
}
//---------------------------------------------------------------------------
NiNode* NiSceneViewer::GetMasterScene()
{
    return m_pkMasterScene;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetLoopEndTime(float fEndTime)
{
    SetAnimationLooping(true);
    m_fAnimLoopEndTime = fEndTime;
}
//---------------------------------------------------------------------------
float NiSceneViewer::GetLoopEndTime()
{
    return m_fAnimLoopEndTime;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetLoopStartTime(float fStartTime)
{
    m_fAnimLoopStartTime = fStartTime;
    ResetAnimations();
}
//---------------------------------------------------------------------------
float NiSceneViewer::GetLoopStartTime()
{
    return m_fAnimLoopStartTime;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetAnimationLooping(bool bLoop)
{
    m_bAnimationLooping = bLoop;
}
//---------------------------------------------------------------------------
bool NiSceneViewer::GetAnimationLooping()
{
    return m_bAnimationLooping;
}
//---------------------------------------------------------------------------
unsigned int NiSceneViewer::GetCameraClickCeiling()
{
    return m_uiCameraClickCeiling;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetCameraClickCeiling(unsigned int uiCeiling)
{
    m_uiCameraClickCeiling = uiCeiling;
}
//---------------------------------------------------------------------------
void NiSceneViewer::AttachSceneToMasterRoot(NiNode*)
{
    m_pkMasterScene->SetAt(m_uiInsertionIndex, NULL);
}
//---------------------------------------------------------------------------
NiViewerCamera* NiSceneViewer::GetCurrentCameraOrientation()
{
    return m_pkCameras->GetAt(GetCurrentCameraID())->m_pkCameraOrientation;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetCurrentCameraOrientation(
    NiViewerCamera* pkCameraOrientation)
{
    NIASSERT(pkCameraOrientation != NULL);
    if (pkCameraOrientation != NULL)
    {
        Camera* pkCamera = m_pkCameras->GetAt(GetCurrentCameraID());
        pkCamera->m_pkCameraOrientation = pkCameraOrientation;
    }
}
//---------------------------------------------------------------------------
float NiSceneViewer::GetWorldRadius()
{
    return m_fWorldRadius;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetWorldRadius(float f)
{
    if (f > 0)
        m_fWorldRadius = f;
}
//---------------------------------------------------------------------------
void NiSceneViewer::EmptyCameraList()
{
    Lock(RENDERER_LOCK);
        
    for (unsigned int i = 0; i < m_pkCameras->GetSize(); i++)
    {
        Camera* pkCamera = m_pkCameras->GetAt(i);
        if (pkCamera != NULL)
        {
            if (m_spRenderer)
                m_spRenderer->PurgeAllRendererData(pkCamera->m_spCamera);

            NiDelete pkCamera->m_pkCameraOrientation;
            pkCamera->m_spCamera = NULL;
            NiDelete pkCamera;
        }
    }

    m_pkCameras->RemoveAll();
    Unlock(RENDERER_LOCK);
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetExternalTimeMode(bool bIsAnimatedExternally)
{
    m_bAnimateExternally = bIsAnimatedExternally;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetCurrentTime(float fCurrentTime)
{
    m_fAnimCurrentTime = fCurrentTime;
}
//---------------------------------------------------------------------------
float NiSceneViewer::GetCurrentTime()
{
    if (m_bAnimateExternally)
        return m_fAnimCurrentTime;
    else
        return m_fAnimAccumTime;
}
//---------------------------------------------------------------------------
void NiSceneViewer::SetupCameraExtraData(bool bIsUserCam, NiCamera* pkCamera)
{
    NiBooleanExtraData* pkUserCamED = (NiBooleanExtraData*) 
        pkCamera->GetExtraData(IS_USER_CAM);

    if (!pkUserCamED)
    {
        pkUserCamED = NiNew NiBooleanExtraData(bIsUserCam);
        pkUserCamED->SetName(IS_USER_CAM);
        pkCamera->AddExtraData(pkUserCamED);

        NiFloatExtraData* pkFloatED = NULL;

        NiFrustum kFrustum = pkCamera->GetViewFrustum();
        pkFloatED = NiNew NiFloatExtraData(kFrustum.m_fLeft);
        pkFloatED->SetName(FRUSTUM_LEFT);
        pkCamera->AddExtraData(pkFloatED);

        pkFloatED = NiNew NiFloatExtraData(kFrustum.m_fRight);
        pkFloatED->SetName(FRUSTUM_RIGHT);
        pkCamera->AddExtraData(pkFloatED);

        pkFloatED = NiNew NiFloatExtraData(kFrustum.m_fTop);
        pkFloatED->SetName(FRUSTUM_TOP);
        pkCamera->AddExtraData(pkFloatED);

        pkFloatED = NiNew NiFloatExtraData(kFrustum.m_fBottom);
        pkFloatED->SetName(FRUSTUM_BOTTOM);
        pkCamera->AddExtraData(pkFloatED);

        pkFloatED = NiNew NiFloatExtraData(kFrustum.m_fNear);
        pkFloatED->SetName(FRUSTUM_NEAR);
        pkCamera->AddExtraData(pkFloatED);

        pkFloatED = NiNew NiFloatExtraData(kFrustum.m_fFar);
        pkFloatED->SetName(FRUSTUM_FAR);
        pkCamera->AddExtraData(pkFloatED);
    }
}
//---------------------------------------------------------------------------
void NiSceneViewer::AttachScreenElements(NiMeshScreenElements* pkElements)
{
    m_kScreenElements.AddFirstEmpty(pkElements);
}
//---------------------------------------------------------------------------
void NiSceneViewer::DetachScreenElements(NiMeshScreenElements* pkElements)
{
    m_kScreenElements.Remove(pkElements);
}
//---------------------------------------------------------------------------
void NiSceneViewer::CreateFrame()
{
    // Create a standard NiRenderFrame using m_spCamera, m_spScene, and 
    // the default render target.
    Precache(m_pkMasterScene);

    // Create 3D render view.
    NiCullingProcess* pkCuller = NiNew NiMeshCullingProcess(&m_kVisible, NULL);

    if (NiShadowManager::GetShadowManager())
    {
        // Initialize shadow click generator and active it.
        NiShadowManager::SetActiveShadowClickGenerator(
            "NiDefaultShadowClickGenerator");

        // Initialize shadow manager parameters.
        NiShadowManager::SetSceneCamera(m_spCamera);
        NiShadowManager::SetCullingProcess(pkCuller);

        // Create shadow render step.
        if (!m_spShadowRenderStep)
        {
            m_spShadowRenderStep = NiNew NiDefaultClickRenderStep();
            m_spShadowRenderStep->SetName(m_kShadowRenderStepName);
            m_spShadowRenderStep->SetPreProcessingCallbackFunc(
                ShadowRenderStepPre);
        }
    }

    // Create the 3D render view
    m_spMainRenderView = NiNew Ni3DRenderView(m_spCamera, pkCuller);
    m_spMainRenderView->AppendScene(m_pkMasterScene);

    // Create main render click.
    NiViewRenderClickPtr pkMainRenderClick = NiNew NiViewRenderClick;
    pkMainRenderClick->AppendRenderView(m_spMainRenderView);
    pkMainRenderClick->SetViewport(m_spCamera->GetViewPort());
    pkMainRenderClick->SetClearAllBuffers(true);

    // Create accumulator processor and add it to render click.
    pkMainRenderClick->SetProcessor(NiNew NiAlphaSortProcessor);

    // Create screen element render click.
    Ni2DStringRenderClick* pkLabelRenderClick = 
        NiNew Ni2DStringRenderClick();
    pkLabelRenderClick->Append2DString(m_spCameraLabel);

    NiMesh2DRenderView* pkScreenElementsRenderView = NiNew NiMesh2DRenderView;
    const unsigned int uiSESize = m_kScreenElements.GetSize();
    for (unsigned int i = 0; i < uiSESize; i++)
    {
        NiMeshScreenElements* pkElements = m_kScreenElements.GetAt(i);
        if (pkElements)
            pkScreenElementsRenderView->AppendScreenElement(pkElements);
    }

    NiViewRenderClick* pkScreenElementsRenderClick = NiNew NiViewRenderClick;
    pkScreenElementsRenderClick->AppendRenderView(
        pkScreenElementsRenderView);
    // Create render step that contains all the render clicks.
    NiDefaultClickRenderStep* pkRenderStep = NiNew NiDefaultClickRenderStep;
    pkRenderStep->AppendRenderClick(pkMainRenderClick);
    pkRenderStep->AppendRenderClick(pkScreenElementsRenderClick);
    pkRenderStep->AppendRenderClick(pkLabelRenderClick);

    // Create render frame that contains the single render step.
    m_spFrame = NiNew NiRenderFrame;
    if (m_spShadowRenderStep)
    {
        m_spFrame->AppendRenderStep(m_spShadowRenderStep);
    }
    m_spFrame->AppendRenderStep(pkRenderStep);
}
//---------------------------------------------------------------------------
void NiSceneViewer::Precache(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;

        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild != NULL)
            {
                Precache(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiSceneViewer::ShadowRenderStepPre(NiRenderStep* pkCurrentStep,
    void*)
{
    // Get the list of render clicks from the shadow manager.
    const NiTPointerList<NiRenderClick*>& kShadowClicks =
        NiShadowManager::GenerateRenderClicks();

    // Replace the render clicks in the shadow render step with those provided
    // by the shadow manager.
    NIASSERT(NiIsKindOf(NiDefaultClickRenderStep, pkCurrentStep));
    NiDefaultClickRenderStep* pkClickRenderStep = (NiDefaultClickRenderStep*)
        pkCurrentStep;
    pkClickRenderStep->GetRenderClickList().RemoveAll();
    NiTListIterator kIter = kShadowClicks.GetHeadPos();
    while (kIter)
    {
        pkClickRenderStep->AppendRenderClick(kShadowClicks.GetNext(kIter));
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSceneViewer::ValidateInstancingData(NiNode* pkScene)
{
    if (pkScene != NULL)
    {
        for (unsigned int ui = 0; ui < pkScene->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkScene->GetAt(ui);

            if (NiIsKindOf(NiMesh, pkChild))
            {
                NiMesh* pkMesh = (NiMesh*)pkChild;
                if (pkMesh->GetInstanced())
                {
                    NiInstancingUtilities::ValidateInstancingData(pkMesh);
                }
            }

            if (NiIsKindOf(NiNode, pkChild))
                ValidateInstancingData((NiNode*)pkChild);
        }
    }
}