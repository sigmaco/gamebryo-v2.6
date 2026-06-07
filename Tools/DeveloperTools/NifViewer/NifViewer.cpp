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

#include "NifViewer.h"
#include "NiMenu.h"
#include "NiStats.h"
#include "NiTextureBrowser.h"
#include "NiTGAWriter.h"

#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiParticle.h>
#include <NiPortal.h>
#include <NiTNodeTraversal.h>

#if !defined(_WII)
#include <NiCommonMaterialLib.h>
#endif

#if defined(_XENON)
    #include <NiXenonRenderer.h>
    #include "XenonShaderToolkit.h"
#elif defined(_PS3)
    #include "PS3ShaderToolkit.h"
#elif defined(_WII)
    #include <NiWiiInputSystem.h>
    #include <NiInputWiiRemote.h>
#endif

#include <NiLicense.h>

NiEmbedGamebryoLicenseCode;

//---------------------------------------------------------------------------
NiApplication* NiApplication::Create()
{
    // process renderer-specific command line options
    if (GetCommand())
    {
        NiCommand *pkCommand = GetCommand();

        if (pkCommand->Boolean("pausefordebug"))
        {
            const int iSeconds = 60;
            printf("Pausing %d seconds for debugging.\n", iSeconds);
            printf("Attach to the process from your debugger.\n");
            fflush(stdout);
            NiSleep(iSeconds * 1000);
        }
    }
    
    return NiNew NifViewer;
}

//---------------------------------------------------------------------------
NifViewer::NifViewer() : 
    // enable frame rendering via the fourth argument
    NiApplication("NifViewer", NiApplication::DEFAULT_WIDTH, 
        NiApplication::DEFAULT_HEIGHT, true)
{
    m_bMaxImmerseMode = false;
    m_bSaveNifEnable = false;
    m_fAppNear = -1.0f;
    m_fAppFar = -1.0f;
    m_pkMenu = NULL;
    m_pspNifObjects = 0;
    m_uiNifObjCnt = 0;
    m_acFileName[0] = 0;
    m_bCompress = false;
    m_pkTextureBrowser = NULL;
    m_uiFramesTilCapture = 0;

    m_pkStats = NULL;

#if defined(_PS3) || defined(_WII)
    // Use current working directory for media path on PS3 and Wii
    SetMediaPath("./");
#endif // defined(_PS3)

#if defined(_WII)
    // Create the cursor render click.
    m_kCursorRenderStepName = "Wii Remote Cursor Render Step";
    m_kCursorRenderClickName = "Wii Remote Cursor Render Click";
    m_spCursorRenderClick = NiNew NiCursorRenderClick;
    m_spCursorRenderClick->SetName(m_kCursorRenderClickName);
#endif
}

//---------------------------------------------------------------------------
bool NifViewer::Initialize()
{
    // The ShadowManager needs to be initialized before the renderer is created
    NiShadowManager::Initialize();

#if defined(_PS3)
    // Increase the main memory buffer size and the command buffer size on PS3
    m_kPS3GLInitParameters.uiHostMemorySize = 16<<20;
    m_kPS3GLInitParameters.uiCommandBufferSize = 4<<20;
#endif

    if (!CreateRenderer())
        return false;

    if (GetCommand())
    {
        //  support for setting near and far clip planes
        GetCommand()->Float("near", m_fAppNear);
        GetCommand()->Float("far", m_fAppFar);

        char acBGString[256];

        if (GetCommand()->String("bg", acBGString, 256))
        {
            NiColor kBackground;

            sscanf(acBGString, "%f,%f,%f",
                &kBackground.r,
                &kBackground.g,
                &kBackground.b);

            m_spRenderer->SetBackgroundColor(kBackground);
        }

        GetCommand()->Boolean("MaxImmerse", m_bMaxImmerseMode);
        GetCommand()->Boolean("compress", m_bCompress);
        GetCommand()->Boolean("lk", m_bSaveNifEnable);

#ifdef _PS3
        char acShaderDirectory[NI_MAX_PATH];
        if (GetCommand()->String("shaderpath", acShaderDirectory, NI_MAX_PATH))
        {
            NILOG("Custom shader path set to %s.\n", acShaderDirectory);
            PS3ShaderToolkit::SetShaderDirectory(acShaderDirectory);
        }
#endif
        GetCommand()->Filename(m_acFileName, NI_MAX_PATH);
    }

    if (m_acFileName[0] == 0)
    {
        NiOutputDebugString("ERROR: Must supply a filename.\n");
        return false;
    }
    else
    {
        NiOutputDebugString("Found a filename\n");
        NiOutputDebugString(m_acFileName);
        NiOutputDebugString("\n");
    }

    m_fRotSpeed = m_fTrnSpeed = 1.0f;
    m_iRotSpeed = m_iTrnSpeed = 0;
    m_fAnimationSpeed = 1.0f;
    m_bTumble = false;
    m_bInvertGamepad = false;
    m_uiLockUpAxis = 3;
    m_bFreeze = false;

#if defined(_XENON)
    NiD3DRenderer* pkD3DRenderer = NiSmartPointerCast(NiD3DRenderer,
        m_spRenderer);
    XenonShaderToolkit::Create(pkD3DRenderer);
#elif defined (_PS3)
    PS3ShaderToolkit::Create();
#endif  

    if (!CreateInputSystem())
        return false;

    m_spScene = NiNew NiNode;
    NIASSERT(m_spScene != NULL);
    m_spCenterNode = NiNew NiNode;
    NIASSERT(m_spCenterNode != NULL);
    m_spScene->SetSelectiveUpdate(true);
    m_spCenterNode->SetSelectiveUpdate(true);
    m_spScene->SetSelectiveUpdateRigid(false);
    m_spCenterNode->SetSelectiveUpdateRigid(false);

    m_spScene->AttachChild(m_spCenterNode);

    m_spAlphaAccumulator = NiNew NiAlphaAccumulator;

    m_spFontTexture = 
        NiSourceTexture::Create(ConvertMediaFilename("COURIER10.TGA"));
    m_uiFontWidth = 8;
    m_uiFontHeight = 16;
    m_uiFontColumns = 12;

    if (!CreateCursor())
        return false;

    if (!CreateSceneFromNif(m_acFileName))
        return false;
    
    m_spWireframe = NiNew NiWireframeProperty;
    m_spScene->AttachProperty(m_spWireframe);
    m_spDither = NiNew NiDitherProperty;
    m_spScene->AttachProperty(m_spDither);

    CreateTextureBrowser();
    CreateMenu();
    CreateStats();

    SelectCamera(0);

    m_spCamera = GetCamera(m_uiActiveCamera);
    CreateFrame();

    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    return true;
}

//---------------------------------------------------------------------------
void NifViewer::Terminate()
{
    // Clear screen before exiting.

    if (m_spRenderer != NULL)
    {
        m_spRenderer->BeginFrame();
        m_spRenderer->BeginUsingDefaultRenderTargetGroup(
            NiRenderer::CLEAR_ALL);
        m_spRenderer->EndUsingRenderTargetGroup();
        m_spRenderer->EndFrame();
        m_spRenderer->DisplayFrame();
    }
    m_spFontTexture = NULL;

    NiDelete m_pkStats;
    m_pkStats = NULL;

    m_spWireframe = NULL;
    m_spDither = NULL;

    CloseScene();

#if defined(_WII)
    m_spCursorRenderClick = NULL;
    m_spCursor = NULL;
    m_spNavPointer = NULL;
#endif

    m_spCenterNode = NULL;
    m_spAlphaAccumulator = NULL;
    NiApplication::Terminate();
    
#if defined(_XENON)
    XenonShaderToolkit::Shutdown();
#elif defined (_PS3)
    PS3ShaderToolkit::Shutdown();
#endif  
}

//---------------------------------------------------------------------------
void NifViewer::OnIdle()
{
    BeginUpdate();
    UpdateFrame();
    EndUpdate();

    BeginFrame();
    m_spCamera = GetCamera(m_uiActiveCamera);
    RenderFrame();
    EndFrame();
    DisplayFrame();
    
#if defined(_XENON)
    // ScreenCapture not yet available on Xenon
#else   //#if defined(_XENON)
    if (m_uiFramesTilCapture > 0)
    {
        if (--m_uiFramesTilCapture == 0)
        {
            ScreenCapture();
            // Re-open the menu.
            m_pkMenu->SetActive(true);
        }
    }
#endif  //#if defined(_XENON)
}
//---------------------------------------------------------------------------
void NifViewer::UpdateInput()
{
    NiApplication::UpdateInput();
#if defined(_WII)
    NiWiiInputSystem* pkWiiIS = 
        NiSmartPointerCast(NiWiiInputSystem, m_spInputSystem);

    // Find the first active Wii Remote.
    NiInputWiiRemote* pkWiiRemote = NULL;
    bool bFoundWiiRemote = false;
    for (unsigned int uiPad = 0; 
        uiPad < NiWiiInputSystem::MAX_WII_GAMEPADS; ++uiPad)
    {
        pkWiiRemote = NiDynamicCast(
            NiInputWiiRemote, 
            pkWiiIS->GetGamePad(uiPad));

        if (pkWiiRemote && pkWiiRemote->GetStatus() == NiInputDevice::READY)
        {
            bFoundWiiRemote = true;
            break;
        }
    }

    if (bFoundWiiRemote)
    {
        // Recenter the pointer if it's invalid.
        pkWiiRemote->RecenterInvalidPointer(m_fAccumTime);

         // If the B trigger button is held down, translate the pointer 
         // location into right analog stick intensity for navigation.   
        if (pkWiiRemote->ButtonIsDown(NiInputGamePad::NIGP_RRIGHT))
        {
            pkWiiRemote->UpdateFPSPointer();
        }

        // If the B trigger button was just pressed, turn the cursor on.
        if (pkWiiRemote->ButtonWasPressed(NiInputGamePad::NIGP_RRIGHT))
        {
            if (m_spCursor->GetShow() == 0)
                m_spCursor->Show(true);
        }

        // If the B trigger button was just released, turn the cursor off.
        if (pkWiiRemote->ButtonWasReleased(NiInputGamePad::NIGP_RRIGHT))
        {
            if (m_spCursor->GetShow() > 0)
                m_spCursor->Show(false);
        }

        // Update the cursor position.
        const NiRenderTargetGroup* pkRTGroup = 
            m_spRenderer->GetDefaultRenderTargetGroup();
        NIASSERT(pkRTGroup);
        const NiPoint2& kPt = pkWiiRemote->GetRecenteredPointer();
        // Translate the [-1, 1] scale to screen space.
        // We have to clamp here since NiCursor just disregards 
        // input outside of its bounds (we want input outside the bounds
        // to be clamped to the min/max).
        int iScreenSpaceX = NiClamp(
            (int)((kPt.x + 1.0f) / 2.0f * pkRTGroup->GetWidth(0)),
            0, pkRTGroup->GetWidth(0));
        int iScreenSpaceY = NiClamp(
            (int)((kPt.y + 1.0f) / 2.0f * pkRTGroup->GetHeight(0)),
            0, pkRTGroup->GetHeight(0));
        m_spCursor->SetPosition(m_fAccumTime, iScreenSpaceX, iScreenSpaceY);
    }
    else
    {
        // No Wii Remote, so turn off the cursor.
        if (m_spCursor->GetShow() > 0)
            m_spCursor->Show(false);
    }
#endif
}
//---------------------------------------------------------------------------
void NifViewer::ProcessInput()
{
    for (unsigned int uiGamePad = 0; 
        uiGamePad < NiInputSystem::MAX_GAMEPADS;
        uiGamePad++
        )
    {
        NiInputGamePad* pkGamePad = m_spInputSystem->GetGamePad(uiGamePad);
        if (!pkGamePad)
            continue;

#if defined(_WII)
        if (pkGamePad->GetStatus() == NiInputDevice::REMOVED ||
            pkGamePad->GetStatus() == NiInputDevice::LOST)
            continue;

        // Remove RRIGHT from the modifier list so that navigation
        // can occur when the B trigger is held down on a Wii Remote.
        pkGamePad->RemoveModifiers(NiInputGamePad::NIGP_MASK_RRIGHT);
#endif

        unsigned int uiButtons = pkGamePad->GetCurrentButtons();
        unsigned int uiNewButtons = pkGamePad->GetNewButtons();

        if (m_pkTextureBrowser->GetActive())
        {
            bool bExit;

            m_pkTextureBrowser->ButtonPress(uiNewButtons, uiButtons, bExit);
            if (bExit)
            {
                DeactivateTextureBrowser();
            }
        }

        if (uiNewButtons == 0)
            continue;

        if (m_pkMenu->GetActive())
        {
            m_pkMenu->ButtonPress(uiNewButtons, uiButtons);
        }

        NiInputGamePad::ButtonMask eMenuButtonMask;
        eMenuButtonMask = NiInputGamePad::NIGP_MASK_START;

        if (uiNewButtons & eMenuButtonMask)
        {
            if (uiButtons == (unsigned int)eMenuButtonMask)
            {
                m_pkMenu->SetActive(!m_pkMenu->GetActive());

                if (m_pkMenu->GetActive())
                {
                    DeactivateTextureBrowser();
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
bool NifViewer::CreateCursor()
{
#if defined(_WII)
    // Load the cursor image.
    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;

    m_spNavPointer = 
        NiSourceTexture::Create(
            ConvertMediaFilename("WiiCursor_Nav.tga"), kPrefs);
    if (!m_spNavPointer)
        return false;

    // Create the cursor with the nav image.
    const NiRenderTargetGroup* pkRTGroup = 
        m_spRenderer->GetDefaultRenderTargetGroup();
    NIASSERT(pkRTGroup);

    NiRect<int> kScreenBounds(
        0, pkRTGroup->GetWidth(0), 
        0, pkRTGroup->GetHeight(0));
    m_spCursor = NiCursor::Create(
        m_spRenderer, kScreenBounds, 
        3, 2, m_spNavPointer, pkRTGroup);

    if (m_spCursor == NULL)
        return false;

    m_spCursor->SetPosition(0.0f, 
        (pkRTGroup->GetWidth(0) / 2), (pkRTGroup->GetHeight(0) / 2));
    m_spCursorRenderClick->AppendCursor(m_spCursor);
#endif
    return true;
}
//---------------------------------------------------------------------------
class FindLightFunctor
{
public:
    FindLightFunctor() :
        m_bFound(false) { }

    inline bool GetFound()
    {
        return m_bFound;
    }

    inline void operator() (NiAVObject* pkObject)
    {
        if (NiIsKindOf(NiLight, pkObject))
            m_bFound = true;
    }

protected:
    bool m_bFound;
};
//---------------------------------------------------------------------------
bool NifViewer::CreateSceneFromNif(const char* pcFileName)
{
    NiStream kStream;
    unsigned int i;
    
    CloseScene();

    bool bLoad;

#if defined (_PS3)
    if (pcFileName[0] == '/')
    {
        bLoad = kStream.Load(pcFileName);
    }
    else if (pcFileName[1] == ':')
    {
        char strPath[NI_MAX_PATH];
        NiSprintf(strPath, NI_MAX_PATH, "%s/%s", SYS_APP_HOME, pcFileName);
        bLoad = kStream.Load(strPath);
    }
    else
    {
        bLoad = kStream.Load(ConvertMediaFilename(pcFileName));
    }
#else
        bLoad = kStream.Load(ConvertMediaFilename(pcFileName));
#endif
    if (!bLoad)
    {
        NiMessageBox(kStream.GetLastErrorMessage(),
            pcFileName);
        return false;
    }

    NiOutputDebugString(pcFileName);
    NiOutputDebugString(": Load succeeded.\n");

    if (kStream.GetObjectCount())
    {
        m_pspNifObjects = NiNew NiObjectPtr[kStream.GetObjectCount()];
        NIASSERT(m_pspNifObjects);
    }
    m_uiNifObjCnt = 0;

    unsigned int uiChild = 0;
    for (i = 0; i < kStream.GetObjectCount(); i++)
    {
        NiObject *pkObject;

        pkObject = kStream.GetObjectAt(i);
        
        m_pspNifObjects[i] = pkObject;
        m_uiNifObjCnt++;
        
        if (NiIsKindOf(NiNode, pkObject) || NiIsKindOf(NiMesh, pkObject))
        {
            NiAVObject* pkChild = (NiAVObject*) pkObject;
            m_spCenterNode->SetAt(uiChild++, pkChild);
        }
        else if (NiIsKindOf(NiCamera, pkObject))
        {
            NiCamera* pkCamera = (NiCamera*) pkObject;
            AddCamera(pkCamera);
        }
        else if (NiIsKindOf(NiLight, pkObject))
        {
            NiLight *pkLight = (NiLight*) pkObject;
            pkLight->Update(0.0f);
            m_kLights.Add(pkLight);
        }
    }

    NIASSERT(m_uiNifObjCnt == kStream.GetObjectCount());

    // Search for a light somewhere in the scene. If no light was 
    // found, add one.
    FindLightFunctor kFindLight;
    i = 0;
    while (!kFindLight.GetFound() && i < m_uiNifObjCnt)
    {
        NiAVObject* pkAV = NiDynamicCast(NiAVObject, m_pspNifObjects[i]);
        if (pkAV)
            NiTNodeTraversal::DepthFirst_AllObjects(pkAV, kFindLight);
        ++i;
    }
    if (!kFindLight.GetFound())
    {
        NiDirectionalLight* pkDir = NiNew NiDirectionalLight();
        NiMatrix3 kRot = NiMatrix3::ZERO;
        NiPoint3 kDir(1.0f, 1.0f, 1.0f);
        kDir.Unitize();
        kRot.SetRow(0, kDir);
        pkDir->SetRotate(kRot);
        pkDir->AttachAffectedNode(
            NiSmartPointerCast(NiNode, m_pspNifObjects[0]));
        NiSmartPointerCast(NiNode, m_pspNifObjects[0])->UpdateEffects();
        pkDir->Update(0.0f);
        m_kLights.Add(pkDir);
    }

    CreateCameraList(m_spScene);

    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    
    bool bNeedAlphaSort = false;
    m_bLoop = true;
    m_fLoopTime = 0.0f;
    Traverse(m_spScene, bNeedAlphaSort);
    if (bNeedAlphaSort)
    {
        m_spRenderer->SetSorter(m_spAlphaAccumulator);
    }
    
    m_spScene->SetTranslate(NiPoint3::ZERO);
    m_spScene->SetRotate(NiMatrix3::IDENTITY);
    m_spCenterNode->SetTranslate(NiPoint3::ZERO);
    m_spScene->Update(0.0f);
    m_spScene->UpdateNodeBound();

    // Center the scene so that tumble node rotates the scene
    // about its center. Undo the translate at m_spScene
    // so that the scene is still in the same position relative
    // to cameras.
    NiPoint3 kCenter = m_spScene->GetWorldBound().GetCenter();
    m_spCenterNode->SetTranslate(-kCenter);
    m_spScene->SetTranslate(kCenter);

    CreateDefaultCamera();

    for (i = 0; i < m_kCameras.GetSize(); i++)
    {
        NiCamera *pkCamera = GetCamera(i);

        if (m_fAppNear != -1.0f || m_fAppFar != -1.0f)
        {
            const NiFrustum& kFr = pkCamera->GetViewFrustum();
            NiFrustum kFr2 = kFr;
            if (m_fAppNear != -1.0f)
                kFr2.m_fNear = m_fAppNear;
            if (m_fAppFar != -1.0f)
                kFr2.m_fFar = m_fAppFar;
            pkCamera->SetViewFrustum(kFr2);
        }
        pkCamera->Update(0.0f);
    }

    ResetAnimationTime(false);
    return true;
}

//---------------------------------------------------------------------------
void NifViewer::Traverse(NiAVObject* pkObject, bool& bNeedAlphaSort)
{
    // Old versions of MAX plugin created NiDitherProperty objects. Remove
    // them so they don't interfere with our code to toggle dithering.
    pkObject->RemoveProperty(NiProperty::DITHER);

    // Iterate over an object's properties looking for controllers.
    // Some incidental logic occurs in this loop to avoid grabbing the
    // same constoller twice.
    NiTimeController* pkCon;

    NiPropertyList& kList = pkObject->GetPropertyList();

    NiTListIterator kIter = kList.GetHeadPos();
    while (kIter)
    {
        NiProperty* pkProperty = kList.GetNext(kIter);
        if (pkProperty)
        {
            NiAlphaProperty* pkAlpha = 
                NiDynamicCast(NiAlphaProperty, pkProperty);
            if (pkAlpha != NULL && pkAlpha->GetAlphaBlending())
            {
                bNeedAlphaSort = true;
            }

            for (pkCon = pkProperty->GetControllers(); pkCon; pkCon = 
                pkCon->GetNext())
            {
                if (pkCon->GetCycleType() == NiTimeController::LOOP)
                {
                    // Don't manually loop time fed to animation sysytem if
                    // scene contains any controllers that loop
                    // automatically.
                    m_bLoop = false;
                }

                float fEndTime = pkCon->GetEndKeyTime();
                m_fLoopTime = NiMax(m_fLoopTime, fEndTime);
            }
        }
    }

    
    for (pkCon = pkObject->GetControllers(); pkCon; pkCon = pkCon->GetNext())
    {
        if (pkCon->GetCycleType() == NiTimeController::LOOP)
        {
            // Don't manually loop time fed to animation sysytem if scene
            // contains any controllers that loop automatically.
            m_bLoop = false;
        }

        float fEndTime = pkCon->GetEndKeyTime();
        m_fLoopTime = NiMax(m_fLoopTime, fEndTime);
    }

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;

        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild != NULL)
            {
                Traverse(pkChild, bNeedAlphaSort);
            }
        }
    }
}

//---------------------------------------------------------------------------
void NifViewer::CloseScene()
{
    unsigned int i;

    DestroyMenu();

    DestroyTextureBrowser();

    NiDelete [] m_pspNifObjects;
    m_pspNifObjects = NULL;
    m_uiNifObjCnt = 0;
    
    if (m_spCenterNode != NULL)
    {
        for (i = 0; i < m_spCenterNode->GetArrayCount(); i++)
        {
            m_spCenterNode->DetachChildAt(i);
        }
    }

    m_kCameras.RemoveAll();
    m_kCameras.Realloc(); // destroy smart pointer references to cameras.

    for (i = 0; i < 3; i++)
    {
        m_kTurret.SetTrnObject(i, NULL);
        m_kTurret.SetRotObject(i, NULL);
    }

    m_uiActiveCamera = 0;
    m_kLights.RemoveAll();
}

//---------------------------------------------------------------------------
bool NifViewer::SceneIsOpen() const
{
    return (m_spCenterNode->GetChildCount() > 0);
}

//---------------------------------------------------------------------------
void NifViewer::CreateDefaultCamera()
{
    NiCamera *pkCamera = NiNew NiCamera;
    NIASSERT(pkCamera != NULL);
    pkCamera->SetName("NifViewer");

    CalcCameraFrustum(pkCamera);
    CalcInitialCameraTransform(pkCamera);

    AddCamera(pkCamera);
}

//---------------------------------------------------------------------------
void NifViewer::CalcCameraFrustum(NiCamera *pkCamera) const
{
    float fSceneRadius = m_spScene->GetWorldBound().GetRadius();
    float fRight, fTop;

    fRight = fTop = 1.0f / NiSqrt(3.0f);

    if (GetAppWindow()->GetWidth() > GetAppWindow()->GetHeight())
    {
        fRight *= (float) GetAppWindow()->GetWidth() / 
            (float) GetAppWindow()->GetHeight();
    }
    else
    {
        fTop *= (float) GetAppWindow()->GetHeight() / 
            (float) GetAppWindow()->GetWidth();
    }

    NiFrustum kFr(-fRight, fRight, fTop, -fTop,
        0.05f  * fSceneRadius, 40.0f * fSceneRadius);

    pkCamera->SetViewFrustum(kFr);
}

//---------------------------------------------------------------------------
void NifViewer::CalcInitialCameraTransform(NiCamera *pkCamera) const
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

    NiMatrix3 kRot(NiPoint3(0.0f, 1.0f, 0.0f),
                   NiPoint3(0.0f, 0.0f, 1.0f),
                   NiPoint3(1.0f, 0.0f, 0.0f));
    
    pkCamera->SetRotate(kRot);
    pkCamera->SetTranslate(kInitialCameraPos);
}

//---------------------------------------------------------------------------
bool NifViewer::UpdateTurret()
{
    bool bResult = m_kTurret.Read();
    if (!bResult)
        return false;

    // Lock the up axis
    if (m_uiLockUpAxis != 0 && !m_bTumble)
    {
        // Get object to adjust
        NiAVObject *pkObjectHoldingXform = GetCamera(m_uiActiveCamera);

        // Set the axis we are using as the locked up axis
        NiPoint3 kLockUp(
            (m_uiLockUpAxis == 1) ? 1.f : 0.f,
            (m_uiLockUpAxis == 2) ? 1.f : 0.f,
            (m_uiLockUpAxis == 3) ? 1.f : 0.f
            );

        // Transform up into camera's parent's space
        NiMatrix3 kParentRot;
        NiAVObject* pkParent = pkObjectHoldingXform->GetParent();
        if (pkParent)
        {
            kParentRot = pkParent->GetWorldRotate().Inverse();
        }
        else
        {
            kParentRot.MakeIdentity();
        }
        NiPoint3 kUpInParent = kParentRot * kLockUp;

        // Now make the camera object's local transform have the same up
        NiMatrix3 kLocalRot = pkObjectHoldingXform->GetRotate();

        NiPoint3 kLook;
        kLocalRot.GetCol(0, kLook);
        NiPoint3 kRight = kLook.Cross(kUpInParent);
        if (kRight.Unitize() < 1.0e-2)
        {
            // Looking along up. If Look and target Up are parallel, then
            // existing right must already be perpendicular. So construct a
            // view with the desired up pointing directly up.
            kLocalRot.GetCol(2, kRight);
            kLocalRot.SetCol(1, kUpInParent);
            kLook = kUpInParent.Cross(kRight);
            kLocalRot.SetCol(0, kLook);
        }
        else
        {
            kUpInParent = kRight.Cross(kLook);
            kUpInParent.Unitize();
            kLocalRot.SetCol(1, kUpInParent);
            kLocalRot.SetCol(2, kRight);
        }

        // Set adjusted rotation, and update the turret axis
        pkObjectHoldingXform->SetRotate(kLocalRot);
        m_kTurret.SetAxes(kLocalRot);
    }

    return true;
}
//---------------------------------------------------------------------------
void NifViewer::SelectCamera(unsigned int uiCamera)
{
    m_pkCameraEntry->GetSubEntry(m_uiActiveCamera)->SetCheck(false);
    m_uiActiveCamera = uiCamera;
    m_pkCameraEntry->GetSubEntry(m_uiActiveCamera)->SetCheck(true);
    AttachGamePad();
}

//---------------------------------------------------------------------------
void NifViewer::AddCamera(NiCamera *pkCamera)
{
    for (unsigned int i = 0; i < m_kCameras.GetSize(); i++)
    {
        if (m_kCameras.GetAt(i).m_spCamera == pkCamera)
            return;
    }

    Camera kCamera;

    kCamera.m_spCamera = pkCamera;
    kCamera.m_kInitialRot = pkCamera->GetRotate();
    kCamera.m_kInitialPos = pkCamera->GetTranslate();
    m_kCameras.Add(kCamera);
}

//---------------------------------------------------------------------------
NiCamera *NifViewer::GetCamera(unsigned int i) const
{
    NIASSERT(i < m_kCameras.GetSize());
    return m_kCameras.GetAt(i).m_spCamera;
}

//---------------------------------------------------------------------------
void NifViewer::ViewReset()
{
    NiCamera *pkCamera;

    pkCamera = GetCamera(m_uiActiveCamera);
    pkCamera->SetRotate(m_kCameras.GetAt(m_uiActiveCamera).m_kInitialRot);
    pkCamera->SetTranslate(m_kCameras.GetAt(m_uiActiveCamera).m_kInitialPos);
    pkCamera->Update(0.0f);

    m_spScene->SetRotate(NiMatrix3::IDENTITY);
    m_spScene->Update(m_fAccumTime);

    AttachGamePad();
}

//---------------------------------------------------------------------------
void NifViewer::ToggleWireframe(NiMenu::Entry* pkEntry)
{
    bool bWire = !m_spWireframe->GetWireframe();
    m_spWireframe->SetWireframe(bWire);
    pkEntry->SetCheck(bWire);
}

//---------------------------------------------------------------------------
void NifViewer::ResetAnimationTime(bool bUpdate)
{
    if (bUpdate)
        m_spScene->Update(0.0f);

    m_fAccumTime = 0.0f;
    m_fLastTime = (float) NiGetCurrentTimeInSec();
}

//---------------------------------------------------------------------------
void NifViewer::ToggleTumbleMode(NiMenu::Entry* pkEntry)
{
    m_bTumble = !m_bTumble;
    pkEntry->SetCheck(m_bTumble);

    if (SceneIsOpen())
    {
        AttachGamePad();
    }
}
//---------------------------------------------------------------------------
void NifViewer::ToggleInvertGamepad(NiMenu::Entry* pkEntry)
{
    m_bInvertGamepad = !m_bInvertGamepad;
    pkEntry->SetCheck(m_bInvertGamepad);

    for (unsigned int iGamePad = 0; 
        iGamePad < NiInputSystem::MAX_GAMEPADS;
        iGamePad++
        )
    {
        NiInputGamePad* pkGamePad = m_spInputSystem->GetGamePad(iGamePad);
        if (!pkGamePad)
            continue;
        if (m_bInvertGamepad)
        {
            pkGamePad->SetStickInvertFlags(
                NiInputGamePad::NIGP_INVERT_RIGHT_V);
        }
        else
        {
            pkGamePad->SetStickInvertFlags(0);
        }

    }

    AttachGamePad();
}

//---------------------------------------------------------------------------
void NifViewer::AdjustRotationSpeed(bool bFaster)
{
    float fFactor;

    if (bFaster)
    {
        fFactor = 2.0f;
        m_iRotSpeed++;
    }
    else
    {
        fFactor = 0.5f;
        m_iRotSpeed--;
    }

    m_fRotSpeed *= fFactor;

    for (int i = 0; i < 3; i++)
    {
        float fRotSpeed = fFactor * m_kTurret.GetRotSpeed(i);
        m_kTurret.SetRotSpeed(i, fRotSpeed);
    }

    char acLabel[100];
    NiSprintf(acLabel, 100, "Rotate (%d)", m_iRotSpeed);
    m_pkRotateSpeedEntry->SetLabel(acLabel);
}

//---------------------------------------------------------------------------
void NifViewer::AdjustTranslationSpeed(bool bFaster)
{
    float fFactor;

    if (bFaster)
    {
        fFactor = 2.0f;
        m_iTrnSpeed++;
    }
    else
    {
        fFactor = 0.5f;
        m_iTrnSpeed--;
    }

    m_fTrnSpeed *= fFactor;

    for (int i = 0; i < 3; i++)
    {
        float fTrnSpeed = fFactor * m_kTurret.GetTrnSpeed(i);
        m_kTurret.SetTrnSpeed(i, fTrnSpeed);
    }

    char acLabel[100];
    NiSprintf(acLabel, 100, "Translate (%d)", m_iTrnSpeed);
    m_pkTranslateSpeedEntry->SetLabel(acLabel);
}

//---------------------------------------------------------------------------
void NifViewer::AdjustAnimationSpeed(float fAnimationSpeed)
{
    m_fAnimationSpeed = fAnimationSpeed;
    int iPercent = (int) (100.0f * m_fAnimationSpeed);
    char acLabel[100];
    NiSprintf(acLabel, 100, "Speed (%d%%)", iPercent);
    m_pkAnimateSpeedEntry->SetLabel(acLabel);
}
//---------------------------------------------------------------------------
void NifViewer::AdjustLockUpAxis(unsigned int iValue)
{
    NIASSERT(iValue <= 3);

    m_uiLockUpAxis = iValue;

    const char* apcLabels[4] = 
    {
        "None",
        "X",
        "Y",
        "Z"
    };

    char acLabel[100];
    NiSprintf(acLabel, 100, "Lock Up Axis (%s)  ", apcLabels[m_uiLockUpAxis]);
    m_pkLockUpAxisEntry->SetLabel(acLabel);

}
//---------------------------------------------------------------------------
void NifViewer::SwitchCamera(int iIncrement)
{
    unsigned int uiCameraCount;
    int iCamera;
    
    uiCameraCount = m_kCameras.GetSize();
    iCamera = (m_uiActiveCamera + uiCameraCount + iIncrement) % uiCameraCount;
    SelectCamera(iCamera);
}

//---------------------------------------------------------------------------
void NifViewer::AttachGamePad()
{
    NiCamera *pkCamera;
    unsigned int i;

    pkCamera = GetCamera(m_uiActiveCamera);

    if (m_bTumble)
    {
        for (i = 0; i < 3; i++)
        {
            m_kTurret.SetTrnObject(i, NULL);
            m_kTurret.SetRotObject(i, m_spScene);
            m_kTurret.SetRotUseUpdateAxes(i, true, false);
            m_kTurret.SetRotSpeed(i, 0.02f * -m_fRotSpeed);
            m_kTurret.SetRotModifiers(i, NiInputGamePad::NIGP_MASK_NONE);
        }

        m_kTurret.SetRotButtonsStickDirGP(0,
            NiInputGamePad::NIGP_STICK_RIGHT, 
            NiInputGamePad::NIGP_STICK_AXIS_H);
        m_kTurret.SetRotButtonsStickDirGP(1, 
            NiInputGamePad::NIGP_STICK_LEFT, 
            NiInputGamePad::NIGP_STICK_AXIS_H);
        m_kTurret.SetRotButtonsStickDirGP(2, 
            NiInputGamePad::NIGP_STICK_LEFT, 
            NiInputGamePad::NIGP_STICK_AXIS_V);

        m_kTurret.SetAxes(pkCamera->GetWorldRotate());
    }
    else
    {
        float fTrnSpeed = 0.025f * m_fTrnSpeed * 
            m_spScene->GetWorldBound().GetRadius();
        
        for (i = 0; i < 3; i++)
        {
            m_kTurret.SetTrnObject(i, pkCamera);
            m_kTurret.SetRotObject(i, pkCamera);
            m_kTurret.SetTrnModifiers(i, NiInputGamePad::NIGP_MASK_NONE);
            m_kTurret.SetRotModifiers(i, NiInputGamePad::NIGP_MASK_NONE);
            m_kTurret.SetRotUseUpdateAxes(i, true, true);
        }

        m_kTurret.SetTrnSpeed(0, fTrnSpeed);
        m_kTurret.SetTrnSpeed(1, fTrnSpeed);
        m_kTurret.SetTrnSpeed(2, fTrnSpeed);

        m_kTurret.SetTrnButtonsStickDirGP(0, 
            NiInputGamePad::NIGP_STICK_LEFT, 
            NiInputGamePad::NIGP_STICK_AXIS_V);
#if defined(_WII)
        // The Wii Remote does not map L1/R1, but the GC controller does. 
        // Remap translation so that it uses L2/R2 on the Wii.  It will work
        // no matter which type of controller is used.
        m_kTurret.SetTrnButtonsGP(1,
            NiInputGamePad::NIGP_R2,
            NiInputGamePad::NIGP_L2);
#else
        m_kTurret.SetTrnButtonsGP(1,
            NiInputGamePad::NIGP_L2,
            NiInputGamePad::NIGP_L1);
#endif
        m_kTurret.SetTrnButtonsStickDirGP(2, 
            NiInputGamePad::NIGP_STICK_LEFT, 
            NiInputGamePad::NIGP_STICK_AXIS_H);

        m_kTurret.SetRotSpeed(0, 0.02f * -m_fRotSpeed);
        m_kTurret.SetRotSpeed(1, 0.02f * m_fRotSpeed);
        m_kTurret.SetRotSpeed(2, 0.02f * m_fRotSpeed);

#if defined(_WII)
        // The Wii Remote does not map L1/R1, but the GC controller does.  
        // Remap rotation so that it uses L1/R1 on the Wii.  It will only work
        // if a GC controller is plugged in. 
        m_kTurret.SetRotButtonsGP(0,
            NiInputGamePad::NIGP_L1,
            NiInputGamePad::NIGP_R1);
#else
        m_kTurret.SetRotButtonsGP(0,
            NiInputGamePad::NIGP_R2,
            NiInputGamePad::NIGP_R1);
#endif
        m_kTurret.SetRotButtonsStickDirGP(1, 
            NiInputGamePad::NIGP_STICK_RIGHT, 
            NiInputGamePad::NIGP_STICK_AXIS_H);
        m_kTurret.SetRotButtonsStickDirGP(2, 
            NiInputGamePad::NIGP_STICK_RIGHT, 
            NiInputGamePad::NIGP_STICK_AXIS_V);

        m_kTurret.SetAxes(pkCamera->GetRotate());
    }    
}

//---------------------------------------------------------------------------
void NifViewer::ActivateTextureBrowser()
{
    m_pkTextureBrowser->SetActive(true);
    m_pkMenu->SetActive(false);
}

//---------------------------------------------------------------------------
void NifViewer::DeactivateTextureBrowser()
{
    m_pkTextureBrowser->SetActive(false);
}

//---------------------------------------------------------------------------
void NifViewer::CreateMenu()
{
    NIASSERT(m_pkMenu == NULL);

    const unsigned int uiMaxPages = 6;
#if defined(_XENON) || defined (_PS3) || defined(_WII)
    const unsigned int uiPosX = 32;
#endif  //#if defined(_XENON) || defined (_PS3) || defined(_WII)

    const unsigned int uiPosY = 32;
 
    m_pkMenu = NiNew NiMenu(m_spFontTexture, uiMaxPages, m_uiFontWidth,
        m_uiFontHeight, m_uiFontColumns, uiPosX, uiPosY, MenuExecuteCallback,
        this);

    m_kUITextureClicks.AddTail(m_pkMenu->GetRenderClick());
    m_pkMenu->GetRenderClick()->SetPreProcessingCallbackFunc(
        UpdateGUI_Callback, this);

    CreateMenuFile();
    CreateMenuView();
    CreateMenuAnimation();
    CreateMenuGamepad();
}

//---------------------------------------------------------------------------
void NifViewer::CreateMenuFile()
{
    const unsigned uiEntries = 3;
    
    NiMenu::Page* pkPage = m_pkMenu->AddPage(" File ", uiEntries);

#if defined(_XENON)
    // ScreenCapture not yet available on Xenon.
#else   //#if defined(_XENON)
    pkPage->AddEntry("Screen Shot", SCREEN_SHOT);
#endif

    pkPage->AddEntry("Exit", EXIT);
}

//---------------------------------------------------------------------------
void NifViewer::CreateMenuView()
{
    const unsigned int uiEntries = 9;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" View ", uiEntries);

    m_pkCameraEntry = pkPage->AddEntry("Camera", (unsigned int)~0);
    unsigned int uiCameras = m_kCameras.GetSize();

    m_pkCameraEntry->AllocateSubEntries(uiCameras);
    for (unsigned int i = 0; i < uiCameras; i++)
    {
        enum
        {
            MAX_LEN = 20
        };
        char acLabel[MAX_LEN];
        NiSprintf(acLabel, MAX_LEN, "%d: ", i);
        char *p = &acLabel[strlen(acLabel)];

        NiCamera* pkCamera = m_kCameras.GetAt(i).m_spCamera;
        const char *pcName = pkCamera->GetName();
        if (pcName == NULL)
        {
            NiNode* pkParent = pkCamera->GetParent();

            if (pkParent != NULL)
            {
                pcName = pkParent->GetName();
            }

            if (pcName == NULL)
            {
                pcName = "<noname>";
            }
        }

        while (*pcName != 0 && p < &acLabel[MAX_LEN] - 1)
        {
            *p++ = *pcName++;
        }

        *p = 0;
        NiMenu::Entry* pkEntry = m_pkCameraEntry->AddSubEntry(acLabel,
            SELECT_CAMERA + i);

        if (i == m_uiActiveCamera)
        {
            pkEntry->SetCheck(true);
        }
    }

    NiMenu::Entry* pkTextureEntry = pkPage->AddEntry("Textures...", LKEDIT);

    if (!m_pkTextureBrowser->HasEntries())
    {
        pkTextureEntry->SetEnable(false);
    }

    pkPage->AddEntry("Wireframe", WIRE);
    pkPage->AddEntry("Reset", VIEW_RESET);
    const char* pcLabel = "FPS";
    pkPage->AddEntry(pcLabel, STATS);

    NiMenu::Entry* pkSorterEntry =
        pkPage->AddEntry("Sorter", (unsigned int)~0);
    pkSorterEntry->AllocateSubEntries(3);
    m_pkSorterNoneEntry = pkSorterEntry->AddSubEntry("None", SORTER_NONE);
    m_pkSorterAlphaEntry = pkSorterEntry->AddSubEntry("Alpha", SORTER_ALPHA);

    NIASSERT(m_spRenderer->GetSorter() == NULL ||
        m_spRenderer->GetSorter() == m_spAlphaAccumulator);

    ((m_spRenderer->GetSorter() == NULL) ?
        m_pkSorterNoneEntry : m_pkSorterAlphaEntry)->
        SetCheck(true);
}

//---------------------------------------------------------------------------
void NifViewer::CreateMenuAnimation()
{
    const unsigned int uiEntries = 4;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" Animation ", uiEntries);

    NiMenu::Entry* pkFreezeEntry = pkPage->AddEntry("Freeze", FREEZE);
    pkFreezeEntry->SetCheck(m_bFreeze);

    pkPage->AddEntry("Reset Time", ANIMATION_TIME_RESET);

    NiMenu::Entry* pkLoopEntry = pkPage->AddEntry("Loop", LOOP);
    pkLoopEntry->SetCheck(m_bLoop);

    m_pkAnimateSpeedEntry =
        pkPage->AddEntry("Speed (100%) ", (unsigned int)~0);
    m_pkAnimateSpeedEntry->AllocateSubEntries(3);
    m_pkAnimateSpeedEntry->AddSubEntry("Slower", ANIMATE_SLOWER);
    m_pkAnimateSpeedEntry->AddSubEntry("Faster", ANIMATE_FASTER);
    m_pkAnimateSpeedEntry->AddSubEntry("Reset", ANIMATE_SPEED_RESET);
}

//---------------------------------------------------------------------------
void NifViewer::CreateMenuGamepad()
{
    const unsigned int uiEntries = 5;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" Gamepad ", uiEntries);

    NiMenu::Entry* pkTumbleEntry = pkPage->AddEntry("Tumble", TUMBLE);
    pkTumbleEntry->SetCheck(m_bTumble);

    NiMenu::Entry* pkInvertEntry = pkPage->AddEntry("Invert Camera", INVERT);
    pkInvertEntry->SetCheck(m_bInvertGamepad);

    m_pkRotateSpeedEntry = pkPage->AddEntry("Rotate (0)  ", (unsigned int)~0);
    m_pkRotateSpeedEntry->AllocateSubEntries(2);
    m_pkRotateSpeedEntry->AddSubEntry("Slower", ROTATE_SLOWER);
    m_pkRotateSpeedEntry->AddSubEntry("Faster", ROTATE_FASTER);

    m_pkTranslateSpeedEntry =
        pkPage->AddEntry("Translate (0)  ", (unsigned int)~0);
    m_pkTranslateSpeedEntry->AllocateSubEntries(2);
    m_pkTranslateSpeedEntry->AddSubEntry("Slower", TRANSLATE_SLOWER);
    m_pkTranslateSpeedEntry->AddSubEntry("Faster", TRANSLATE_FASTER);

    m_pkLockUpAxisEntry =
        pkPage->AddEntry("Lock Up Axis (None)  ", (unsigned int)~0);
    AdjustLockUpAxis(m_uiLockUpAxis); // set menu string
    m_pkLockUpAxisEntry->AllocateSubEntries(4);
    m_pkLockUpAxisEntry->AddSubEntry("None", LOCK_UP_AXIS_NONE);
    m_pkLockUpAxisEntry->AddSubEntry("X", LOCK_UP_AXIS_X);
    m_pkLockUpAxisEntry->AddSubEntry("Y", LOCK_UP_AXIS_Y);
    m_pkLockUpAxisEntry->AddSubEntry("Z", LOCK_UP_AXIS_Z);
}

//---------------------------------------------------------------------------
void NifViewer::MenuExecuteCallback(NiMenu::Entry* pkEntry,
    void* pvCallbackData)
{
    NifViewer* pkViewer = (NifViewer*) pvCallbackData;
    pkViewer->OnMenuCommand(pkEntry);
}

//---------------------------------------------------------------------------
void NifViewer::OnMenuCommand(NiMenu::Entry* pkEntry)
{
    unsigned int uiCommand = pkEntry->GetCommand();

    switch (uiCommand)
    {
        case SCREEN_SHOT:

#if defined(_XENON)
        // ScreenCapture not yet available on Xenon or Wii.
#else   //#if defined(_XENON)
            RequestScreenCapture();
#endif  //#if defined(_XENON)

            break;

        case LKEDIT:
            ActivateTextureBrowser();
            break;

        case EXIT:
            QuitApplication();
            break;
        
        case WIRE:
            ToggleWireframe(pkEntry);
            break;

        case VIEW_RESET:
            ViewReset();
            break;

        case STATS:
            m_pkStats->SetActive(!m_pkStats->GetActive());
            pkEntry->SetCheck(m_pkStats->GetActive());
            break;

        case SORTER_NONE:
            m_spRenderer->SetSorter(NULL);
            m_pkSorterNoneEntry->SetCheck(true);
            m_pkSorterAlphaEntry->SetCheck(false);
            break;

        case SORTER_ALPHA:
            m_spRenderer->SetSorter(m_spAlphaAccumulator);
            m_pkSorterNoneEntry->SetCheck(false);
            m_pkSorterAlphaEntry->SetCheck(true);
            break;
        
        case FREEZE:
            m_bFreeze = !m_bFreeze;
            pkEntry->SetCheck(m_bFreeze);
            break;

        case ANIMATION_TIME_RESET:
            ResetAnimationTime(m_bFreeze);
            break;

        case ANIMATE_FASTER:
            AdjustAnimationSpeed(m_fAnimationSpeed * 1.2f);
            break;

        case ANIMATE_SLOWER:
            AdjustAnimationSpeed(m_fAnimationSpeed * (1.0f / 1.2f));
            break;

        case ANIMATE_SPEED_RESET:
            AdjustAnimationSpeed(1.0f);
            break;

        case LOOP:
            m_bLoop = !m_bLoop;
            pkEntry->SetCheck(m_bLoop);
            break;

        case TUMBLE:
            ToggleTumbleMode(pkEntry);
            break;

        case INVERT:
            ToggleInvertGamepad(pkEntry);
            break;

        case ROTATE_FASTER:
            AdjustRotationSpeed(true);
            break;

        case ROTATE_SLOWER:
            AdjustRotationSpeed(false);
            break;
        
        case TRANSLATE_FASTER:
            AdjustTranslationSpeed(true);
            break;
        
        case TRANSLATE_SLOWER:
            AdjustTranslationSpeed(false);
            break;

        case LOCK_UP_AXIS_NONE:
        case LOCK_UP_AXIS_X:
        case LOCK_UP_AXIS_Y:
        case LOCK_UP_AXIS_Z:
            AdjustLockUpAxis(uiCommand - LOCK_UP_AXIS_NONE);
            break;

        default:
            NIASSERT(uiCommand >= SELECT_CAMERA &&
                uiCommand < SELECT_CAMERA + m_kCameras.GetSize());
            unsigned int uiCamera = uiCommand - SELECT_CAMERA;
            SelectCamera(uiCamera);
            break;
    }
}

//---------------------------------------------------------------------------
void NifViewer::DestroyMenu()
{
    NiDelete m_pkMenu;
    m_pkMenu = NULL;
}

//---------------------------------------------------------------------------
void NifViewer::CreateTextureBrowser()
{
    NIASSERT(m_pkTextureBrowser == NULL);

    unsigned int uiPosY = 32;
    
    m_pkTextureBrowser = NiNew NiTextureBrowser(m_spFontTexture,
        m_uiFontWidth, m_uiFontHeight, m_uiFontColumns, 340, uiPosY,
        m_spScene);
    NIASSERT(m_pkTextureBrowser != NULL);
        
    // add brower ScreenTexture to the UI list
    m_kUITextureClicks.AddTail(m_pkTextureBrowser->GetRenderClick());
    m_pkTextureBrowser->GetRenderClick()->SetPreProcessingCallbackFunc(
        UpdateTextureBrowser_Callback, this);
}

//---------------------------------------------------------------------------
void NifViewer::CreateStats()
{
    m_pkStats = NiNew NiStats(m_spRenderer, m_spFontTexture, m_uiFontWidth,
        m_uiFontHeight, m_uiFontColumns);

    // add stats texture to the UI list
    m_kUITextureClicks.AddTail(m_pkStats->GetRenderClick());
    m_pkStats->GetRenderClick()->SetPreProcessingCallbackFunc(
        UpdateStats_Callback, this);
}

//---------------------------------------------------------------------------
void NifViewer::DestroyTextureBrowser()
{
    NiDelete m_pkTextureBrowser;
    m_pkTextureBrowser = NULL;
}

//---------------------------------------------------------------------------
void NifViewer::CreateCameraList(NiAVObject* pkObject)
{
    if (NiIsKindOf(NiCamera, pkObject))
    {
        NiCamera* pkCamera = (NiCamera*) pkObject;
        AddCamera(pkCamera);
    }

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild != NULL)
            {
                CreateCameraList(pkChild);
            }
        }
    }    
}
//---------------------------------------------------------------------------
#if !defined(_XENON)
void NifViewer::RequestScreenCapture()
{
    m_pkMenu->SetActive(false);

#if defined(_PS3)
    m_uiFramesTilCapture = 2;
#else
    m_uiFramesTilCapture = 1;
#endif
}
#endif  //#if !defined(_XENON)
//---------------------------------------------------------------------------
#if !defined(_XENON)
void NifViewer::ScreenCapture()
{
    static unsigned int suiFrame = 0;

    char acName[20];
    NiSprintf(acName, 20, "screen%03d.tga", suiFrame++);

    NiOutputDebugString("Begin dump ");
    NiOutputDebugString(acName);
    NiOutputDebugString(".\n");

#if !defined(_WII)
    NiFile kFile(ConvertMediaFilename(acName), NiFile::WRITE_ONLY);
    if (!kFile)
    {
        NiMessageBox("File open failed", 
            ConvertMediaFilename(acName));
        return;
    }
#endif

#if defined(_WII)
    // NiTGAWriter is not implemented on the Wii, but the SaveScreenshot
    // function is capable of saving to TGA.
    if (!m_spRenderer->SaveScreenShot(acName, NiRenderer::FORMAT_TARGA))
    {
        NiMessageBox("File dump failed", acName);
    }
    NiOutputDebugString("Dump finished.\n");
#else
    NiPixelData* pkPixelData = m_spRenderer->TakeScreenShot(NULL);
    NIASSERT(pkPixelData != NULL);
    if (!NiTGAWriter::Dump(pkPixelData, kFile))
    {
        NiMessageBox("File dump failed", acName);
    }
    NiOutputDebugString("Dump finished.\n");
    NiDelete pkPixelData;
#endif
}
//---------------------------------------------------------------------------
#endif  //#if !defined(_XENON)
//---------------------------------------------------------------------------
bool NifViewer::UpdateGUI_Callback(NiRenderClick* pkClick, void* pvThis)
{
    NifViewer* pkThis = (NifViewer*)pvThis;

    pkThis->m_pkMenu->Update();
    if (pkThis->m_pkTextureBrowser->GetActive())
        pkThis->m_pkTextureBrowser->Update();

    return true;
}
//---------------------------------------------------------------------------
bool NifViewer::UpdateStats_Callback(NiRenderClick* pkClick, void* pvThis)
{
    NifViewer* pkThis = (NifViewer*)pvThis;

    pkThis->m_pkStats->Update();

    return true;
}
//---------------------------------------------------------------------------
bool NifViewer::UpdateTextureBrowser_Callback(
    NiRenderClick* pkClick, void* pvThis)
{
    NifViewer* pkThis = (NifViewer*)pvThis;

    pkThis->m_pkTextureBrowser->Update();

    return true;
}
//---------------------------------------------------------------------------
bool NifViewer::CreateFrame()
{
    bool bSuccess = NiApplication::CreateFrame();

    // retrieve the render step
    NiDefaultClickRenderStep* pkRenderStep = NiNew NiDefaultClickRenderStep;
    pkRenderStep->SetName("NifViewer UI Texture Render Step");

    // Add the UI render clicks - these are treated separately
    // because they each require their own callback
    NiTListIterator kIter = m_kUITextureClicks.GetHeadPos();
    while (kIter)
    {
        NiViewRenderClickPtr spUIClick = 
            m_kUITextureClicks.GetNext(kIter);
        pkRenderStep->AppendRenderClick(spUIClick);
    }
    m_spFrame->AppendRenderStep(pkRenderStep);

#if defined(_WII)
    // Create the Wii Remote cursor render step.
    NiDefaultClickRenderStep* pkCursorRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkCursorRenderStep->SetName(m_kCursorRenderStepName);
    pkCursorRenderStep->AppendRenderClick(m_spCursorRenderClick);
    m_spFrame->AppendRenderStep(pkCursorRenderStep);
#endif 

    return bSuccess;
}
//---------------------------------------------------------------------------
void NifViewer::UpdateFrame()
{
    // Update the input system
    UpdateInput();
    // Handle user input
    ProcessInput();

    NiCamera *pkCamera = GetCamera(m_uiActiveCamera);

    if (SceneIsOpen())
    {
        float fCurrentTime = NiGetCurrentTimeInSec();
        if (!m_bFreeze)
        {
            float fDeltaTime = 
                m_fAnimationSpeed * (fCurrentTime - m_fLastTime);
            m_fAccumTime += fDeltaTime;

            if (m_bLoop && m_fAccumTime > m_fLoopTime)
            {
                m_fAccumTime = 0.0f;
            }
        }

        bool bTurretRead = UpdateTurret();

        if (bTurretRead) 
        {
            NiAVObject* pkObject = 
                (m_bTumble ? (NiAVObject*) m_spScene : pkCamera);
            pkObject->Update(m_fAccumTime);
        }

        if (!m_bFreeze && !(bTurretRead && m_bTumble))
        {
            m_spScene->UpdateSelected(m_fAccumTime);
        }
        m_fLastTime = fCurrentTime;
    }
}
//---------------------------------------------------------------------------
