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

#include "PhysXNifViewer.h"
#include "NiMenu.h"
#include "NiStats.h"

#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiParticle.h>
#include <NiPortal.h>

#if !defined(_WII)
#include <NiCommonMaterialLib.h>
#endif

#if defined(WIN32)
    #include "Win32ShaderToolkit.h"
#elif defined(_XENON)
    #include <NiXenonRenderer.h>
    #include "XenonShaderToolkit.h"
#elif defined (_PS3)
    #include "PS3ShaderToolkit.h"
#elif defined(_WII)
    #include <NiInputWiiRemote.h>
#else
    #error Unknown platform!
#endif

const float PhysXNifViewer::MAX_FRAME_STEP = 1.0f / 20.0f;
const float PhysXNifViewer::INIT_ROT_SPEED = 1.0f;
const float PhysXNifViewer::INIT_TRN_SPEED = 1.0f;
const float PhysXNifViewer::FORCE_SPRING_CONST = 0.1f;

#include <NiLicense.h>

NiEmbedGamebryoLicenseCode;


//---------------------------------------------------------------------------
NiApplication* NiApplication::Create()
{
    // process renderer-specific command line options
    if (GetCommand())
    {
        if (GetCommand()->Boolean("pausefordebug"))
        {
            const int iSeconds = 60;
            printf("Pausing %d seconds for debugging.\n", iSeconds);
            printf("Attach to the process from your debugger.\n");
            fflush(stdout);
            NiSleep(iSeconds * 1000);
        }
    }
    return NiNew PhysXNifViewer;
}

//---------------------------------------------------------------------------
PhysXNifViewer::PhysXNifViewer() :
    NiApplication("PhysX Nif Viewer", NiApplication::DEFAULT_WIDTH, 
        NiApplication::DEFAULT_HEIGHT, true)
{
    char acBGString[256];
    if (GetCommand()->String("resolution", acBGString, 256))
    {
        unsigned int uiWidth;
        unsigned int uiHeight;
        
#if defined(WIN32) && (_MSC_VER >= 1400)
        sscanf_s(acBGString, "%dx%d", &uiWidth, &uiHeight);
#else
        sscanf(acBGString, "%dx%d", &uiWidth, &uiHeight);
#endif
        m_pkAppWindow->SetWidth(uiWidth);
        m_pkAppWindow->SetHeight(uiHeight);
        m_bRendererDialog = false;
    }

#ifdef WIN32    
    // Disable system cursor since we're using NiCursor.
    m_bExclusiveMouse = true;
#endif

    m_fAppNear = -1.0f;
    m_fAppFar = -1.0f;
    m_pkMenu = NULL;
    m_pspNifObjects = 0;
    m_uiNifObjCnt = 0;
    m_acFileName[0] = 0;
    
    m_bPreferKeyboard = false;
    m_pkCursor = 0;
    
    m_pkPhysManager = 0;
    m_spSDKDefaults = 0;
    m_fDebugScale = 1.0f;
    m_bDoingGround = false;
    m_fGroundHeight = 0.0f;
    m_bDebugGeom = false;
    
    m_bDisableKeyframes = true;
    
    m_pkStats = NULL;

    m_kGravity = NiPoint3::UNIT_Z * -9.81f;    
    m_bDebugOutput = false;
    m_bGravityProvided = false;

#if defined (WIN32)
    char acPath[NI_MAX_PATH];
    NIVERIFY(NiPath::GetExecutableDirectory(acPath, NI_MAX_PATH));
    SetMediaPath(acPath);
#elif defined (_PS3) || defined(_WII)
    SetMediaPath("./");
#elif defined (_XENON)
    SetMediaPath("D:/DATA/");
#endif

    m_bUsingVRD = false;
}

//---------------------------------------------------------------------------
bool PhysXNifViewer::Initialize()
{
    NiColor kBackground(0.5f, 0.5f, 0.5f);
    
    char acVRDHost[1024];
    NiUInt32 uiVRDPort = 5425;
    
    if (GetCommand())
    {
        //  support for setting near and far clip planes
        GetCommand()->Float("near", m_fAppNear);
        GetCommand()->Float("far", m_fAppFar);

        char acBGString[256];

        if (GetCommand()->String("bg", acBGString, 256))
        {
#if defined(WIN32) && (_MSC_VER >= 1400)
            sscanf_s(acBGString, "%f,%f,%f",
                &kBackground.r, &kBackground.g, &kBackground.b);

#else
            sscanf(acBGString, "%f,%f,%f",
                &kBackground.r, &kBackground.g, &kBackground.b);

#endif
        }

        if (GetCommand()->String("gravity", acBGString, 256))
        {
#if defined(WIN32) && (_MSC_VER >= 1400)
            sscanf_s(acBGString, "%f,%f,%f",
                &m_kGravity.x, &m_kGravity.y, &m_kGravity.z);

#else
            sscanf(acBGString, "%f,%f,%f",
                &m_kGravity.x, &m_kGravity.y, &m_kGravity.z);

#endif
            m_bGravityProvided = true;
        }

        if (GetCommand()->String("vrdHost", acVRDHost, 1024))
        {
            m_bUsingVRD = true;
        }

        GetCommand()->Boolean("keyboard", m_bPreferKeyboard);
        GetCommand()->Boolean("debugText", m_bDebugOutput);
        GetCommand()->Boolean("debugGeom", m_bDebugGeom);
        GetCommand()->Boolean("ground", m_bDoingGround);
        GetCommand()->Float("height", m_fGroundHeight);
        NiInt32 iTemp = 0;
        if (GetCommand()->Integer("vrdPort", iTemp))
            uiVRDPort = (NiUInt32)iTemp;

#ifdef _PS3
        char acShaderDirectory[NI_MAX_PATH];
        if (GetCommand()->String("shaderpath", acShaderDirectory, NI_MAX_PATH))
        {
            NILOG("Custom shader path set to %s.\n", acShaderDirectory);
            PS3ShaderToolkit::SetShaderDirectory(acShaderDirectory);
        }
#endif

        // Deal with the filename, spaces and all. But only works if there
        // is at most one contiguous space anywhere in the name.
        int iPrevPosn;
        iPrevPosn = GetCommand()->Filename(m_acFileName, NI_MAX_PATH);
        if (iPrevPosn)
        {
            char acFileWord[NI_MAX_PATH];
            int iNextPosn;
            while (
                (iNextPosn = GetCommand()->Filename(acFileWord, NI_MAX_PATH))
                == iPrevPosn + 1)
            {
                iPrevPosn++;
                if (strlen(acFileWord) + strlen(m_acFileName) + 2
                    < NI_MAX_PATH)
                {
                    NiStrcat(m_acFileName, NI_MAX_PATH, " ");
                    NiStrcat(m_acFileName, NI_MAX_PATH, acFileWord);
                }
                else
                {
                    NiMessageBox(
                        "File name is too long", "Cannot open file");
                    return false;
                }
            }
        }
    }


    if (m_acFileName[0] == 0)
    {
        NiMessageBox(
            "PhysXNifViewer requires a file name argument",
            "Missing file name");
        return false;
    }
    else
    {
        NiOutputDebugString("Found a filename\n");
        NiOutputDebugString(m_acFileName);
        NiOutputDebugString("\n");
    }

    // The ShadowManager needs to be initialized before the renderer is created
    NiShadowManager::Initialize();

#if defined(_PS3)
    // Increase the main memory buffer size and the command buffer size on PS3
    m_kPS3GLInitParameters.uiHostMemorySize = 128<<20;
    m_kPS3GLInitParameters.uiCommandBufferSize = 4<<20;
#endif

    if (!CreateRenderer())
        return false;

    m_spRenderer->SetBackgroundColor(kBackground);

    if (!CreateCursor())
        return false;
    m_bSelecting = false;
    
#if defined(WIN32)
    Win32ShaderToolkit::Create();
#elif defined(_XENON)
    NiD3DRenderer* pkD3DRenderer = NiSmartPointerCast(NiD3DRenderer,
        m_spRenderer);
    XenonShaderToolkit::Create(pkD3DRenderer);
#elif defined (_PS3)
    PS3ShaderToolkit::Create();
#elif defined(_WII)
#else
    #error Unknown platform!
#endif  

    // Get the physics SDK and set some parameters
    // Have to do it before scene creation
    m_pkPhysManager = NiPhysXManager::GetPhysXManager();
    if (!m_pkPhysManager->Initialize())
        return false;
    ResetPhysXSDKParams();

    if (m_bUsingVRD)
    {
        m_pkPhysManager->m_pkPhysXSDK->getFoundationSDK().getRemoteDebugger()->connect(
            acVRDHost, uiVRDPort);
    }
    
    if (!CreateInputSystem())
        return false;
    if (!m_bPreferKeyboard)
    {
        // Iterate looking for a gamepad - take the first
        for (unsigned int i = 0; i < NiInputSystem::MAX_GAMEPADS; i++)
        {
            NiInputGamePad* pkGamePad = m_spInputSystem->GetGamePad(i);
            if (pkGamePad && pkGamePad->GetStatus() == NiInputDevice::READY)
            {
                m_kTurret.SetInputDevice(NiTurret::TUR_GAMEPAD);
                break;            
            }
        }
    }
    if (m_kTurret.GetInputDevice() == NiTurret::TUR_GAMEPAD)
    {
        m_pkCursor->Show(false);
    }

    m_spScene = NiNew NiNode;
    NIASSERT(m_spScene != NULL);
    m_spBoundNode = NiNew NiNode;
    NIASSERT(m_spBoundNode != NULL);
    m_spScene->SetSelectiveUpdate(true);
    m_spBoundNode->SetSelectiveUpdate(true);
    m_spScene->SetSelectiveUpdateRigid(false);
    m_spBoundNode->SetSelectiveUpdateRigid(false);

    m_spScene->AttachChild(m_spBoundNode);

    m_spAlphaAccumulator = NiNew NiAlphaAccumulator;

    CreateMenuFont();
    
    m_uiFontWidth = 8;
    m_uiFontHeight = 16;
    m_uiFontColumns = 12;

    if (!CreateSceneFromNif(m_acFileName))
        return false;

    m_iRotSpeed = m_iTrnSpeed = 0;
    m_fAnimationSpeed = 1.0f;
    m_bInvertGamepad = false;
    m_bLockGravity = true;
    m_fSimulationTime = 0.0f;
    m_bFreeze = true;
    m_bSingleStep = false;
    m_fStepLength = 0.5f;
    m_fStepLengthRem = 0.0f;

    m_bApplyingForce = false;
    m_iForceExponent = 0;

    m_spWireframe = NiNew NiWireframeProperty;
    m_spScene->AttachProperty(m_spWireframe);

    m_spDither = NiNew NiDitherProperty;
    m_spScene->AttachProperty(m_spDither);

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
void PhysXNifViewer::Terminate()
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

    CloseScene();

#if defined(_WII)
    m_spNavCursorRenderClick = NULL;
    m_spNavCursor = NULL;
    m_spNavPointer = NULL;
#endif

    m_spBoundNode = NULL;
    m_spWireframe = NULL;
    m_spDither = NULL;
    m_spCursorRenderClick = NULL;
    
    NiSystemCursor::Shutdown();

    NiApplication::Terminate();

    if (m_bUsingVRD)
        m_pkPhysManager->m_pkPhysXSDK->getFoundationSDK().getRemoteDebugger()->disconnect();

    if (m_pkPhysManager)
        m_pkPhysManager->Shutdown();

#if defined(WIN32)
    Win32ShaderToolkit::Shutdown();
#elif defined(_XENON)
    XenonShaderToolkit::Shutdown();
#elif defined (_PS3)
    PS3ShaderToolkit::Shutdown();
#elif defined(_WII)
#else
    #error Unknown platform!
#endif
}

//---------------------------------------------------------------------------
void PhysXNifViewer::OnIdle()
{
    BeginUpdate();
    UpdateFrame();
    EndUpdate();

    BeginFrame();
    m_spCamera = GetCamera(m_uiActiveCamera);
    RenderFrame();
    EndFrame();
    DisplayFrame();
}

//---------------------------------------------------------------------------
void PhysXNifViewer::UpdateFrame()
{
    if (!MeasureTime())
        return;

    // Always update the input system
    UpdateInput();

    // Always handle User Input
    ProcessInputWithTime(m_fFrameTime);

    NiCamera *pkCamera = GetCamera(m_uiActiveCamera);
    
    if (!m_bSelecting)
    {
        bool bTurretRead = UpdateTurret(m_fFrameTime);
        if (bTurretRead) 
        {
            pkCamera->Update(m_fAccumTime);
        }
    }
    
    if (SceneIsOpen())
    {
        float fDeltaTime = m_fAnimationSpeed * m_fFrameTime;
        
        if (m_bSingleStep && fDeltaTime > m_fStepLengthRem)
        {
            fDeltaTime = m_fStepLengthRem;
        }
        
        if (fDeltaTime > MAX_FRAME_STEP)
            fDeltaTime = MAX_FRAME_STEP;

        if (!m_bFreeze)
        {
            m_fSimulationTime += fDeltaTime;

            // Update physics
            if (m_spPhysScene && m_bDisableKeyframes)
            {
                m_spPhysScene->UpdateSources(m_fSimulationTime);
                if (m_spPhysScene->Simulate(m_fSimulationTime))
                {
                    m_spPhysScene->FetchResults(m_fSimulationTime, true);
                    m_spPhysScene->UpdateDestinations(m_fSimulationTime);

                    if (m_bDebugOutput)
                    {
                        float fFetchTime = m_spPhysScene->GetPrevFetchTime();
                        char pcMsg[256];
                        NiSprintf(pcMsg, 256, "Time %g, Fetch time %g\n",
                            m_fSimulationTime, fFetchTime);
                        NiOutputDebugString(pcMsg);
                    }
                }
            }
         
            m_spScene->Update(m_fSimulationTime);
        
            if (m_bSingleStep)
            {
                m_fStepLengthRem -= fDeltaTime;
                if (m_fStepLengthRem < 1.0e-6)
                {
                    m_bFreeze = true;
                    m_bSingleStep = false;
                }
            }
        }
    }

    // see if the cursor is visible
    m_spCursorRenderClick->SetActive(m_pkCursor->GetShow() > 0);
}
//---------------------------------------------------------------------------
bool PhysXNifViewer::CreateFrame()
{
    bool bSuccess = NiApplication::CreateFrame();

    // retrieve the render step
    NiDefaultClickRenderStep* pkRenderStep = NiNew NiDefaultClickRenderStep;
    pkRenderStep->SetName("Cursor Render Step");

    // Add the UI render clicks - these are treated separately
    // because they each require their own callback
    NiTListIterator kIter = m_kUITextureClicks.GetHeadPos();
    while (kIter)
    {
        NiViewRenderClickPtr spUIClick = 
            m_kUITextureClicks.GetNext(kIter);
        pkRenderStep->AppendRenderClick(spUIClick);
    }

    // Add the cursor render click
    NiMesh2DRenderViewPtr spScreenElementsRenderView =
        NiNew NiMesh2DRenderView;
    spScreenElementsRenderView->
        AppendScreenElement(m_pkCursor->GetScreenElements());
    m_spCursorRenderClick = NiNew NiViewRenderClick;
    m_spCursorRenderClick->SetName("Force Cursor Render Click");
    m_spCursorRenderClick->AppendRenderView(
        spScreenElementsRenderView);
    pkRenderStep->AppendRenderClick(m_spCursorRenderClick);

#if defined(_WII)
    // Create the nav cursor render click.
    m_spNavCursorRenderClick = NiNew NiCursorRenderClick;
    m_spNavCursorRenderClick->SetName("Wii Remote Cursor Render Click");
    m_spNavCursorRenderClick->AppendCursor(m_spNavCursor);
    pkRenderStep->AppendRenderClick(m_spNavCursorRenderClick);
#endif

    // Append the cursor step to the render frame.
    m_spFrame->AppendRenderStep(pkRenderStep);
    return bSuccess;
}

//---------------------------------------------------------------------------
void PhysXNifViewer::UpdateInput()
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
            if (m_spNavCursor->GetShow() == 0)
                m_spNavCursor->Show(true);
        }

        // If the B trigger button was just released, turn the cursor off.
        if (pkWiiRemote->ButtonWasReleased(NiInputGamePad::NIGP_RRIGHT))
        {
            if (m_spNavCursor->GetShow() > 0)
                m_spNavCursor->Show(false);
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
        m_spNavCursor->SetPosition(m_fAccumTime, iScreenSpaceX, iScreenSpaceY);
        m_pkCursor->SetPosition(m_fAccumTime, iScreenSpaceX, iScreenSpaceY);
    }
    else
    {
        // No Wii Remote, so turn off the nav cursor.
        if (m_spNavCursor->GetShow() > 0)
            m_spNavCursor->Show(false);
    }
#endif
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ProcessInputWithTime(float fDt)
{
    NiInputKeyboard* pkKeyboard = m_spInputSystem->GetKeyboard();
    if (pkKeyboard)
    {
        if (m_pkMenu->GetActive())
        {
            m_pkMenu->KeyPress(pkKeyboard);
        }

        if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_TAB))
        {
            m_pkMenu->SetActive(!m_pkMenu->GetActive());
        }
        else if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE))
        {
            QuitApplication();
        }
        else if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_SPACE))
        {
            ToggleFrozen();
            m_pkMenu->MarkAsChanged();
        }
    }
    NiInputMouse* pkMouse = m_spInputSystem->GetMouse();
    if (pkMouse && (m_kTurret.GetInputDevice() != NiTurret::TUR_GAMEPAD))
    {
        // Not using GamePad, so check for mouse input
        int iX, iY, iZ = 0;
        if (pkMouse->GetPositionDelta(iX, iY, iZ))
        {
            if ((iX != 0) || (iY != 0))
                m_pkCursor->Move(0.0f, iX, iY);
        }

        m_pkCursor->GetScreenSpacePosition(iX, iY);
        
        // Selection always on with a mouse
        unsigned int uiMods;
        if (pkMouse->ButtonWasPressed(NiInputMouse::NIM_LEFT, uiMods))
        {
            NiPoint3 kOrigin;
            NiPoint3 kDir;
            NiCamera* pkCamera =
                m_kCameras.GetAt(m_uiActiveCamera).m_spCamera;

            if (pkCamera->WindowPointToRay(iX, iY, kOrigin, kDir))
            {
                if (SelectForceActor(kOrigin, kDir))
                {
                    m_bApplyingForce = true;
                    ((NiSystemCursor*)m_pkCursor)
                        ->SetType(NiSystemCursor::CLOSEHAND);
                }
            }
        }
        else if (pkMouse->ButtonIsDown(NiInputMouse::NIM_LEFT, uiMods))
        {
            if (m_bApplyingForce)
            {
                ApplyForce(iX, iY);
            }
        }
        else if (pkMouse->ButtonWasReleased(NiInputMouse::NIM_LEFT, uiMods))
        {
            m_bApplyingForce = false;
            ((NiSystemCursor*)m_pkCursor)->SetType(NiSystemCursor::STANDARD);
        }
    }
    
#if defined(_WII)
    NiUInt32 uiForcePickMask = NiInputGamePad::NIGP_MASK_RDOWN;
    NiUInt32 uiFreezeMask = NiInputGamePad::NIGP_MASK_RUP;
#else
    NiUInt32 uiForcePickMask = NiInputGamePad::NIGP_MASK_RUP;
    NiUInt32 uiFreezeMask = NiInputGamePad::NIGP_MASK_RRIGHT;
#endif
    for (unsigned int iGamePad = 0;
        iGamePad < NiInputSystem::MAX_GAMEPADS;
        iGamePad++
        )
    {
        NiInputGamePad* pkGamePad = m_spInputSystem->GetGamePad(iGamePad);
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
        
        if (m_bSelecting)
        {
            bool bUseAnalogStickForceCursor = true;
#if defined(_WII)
            // If a Wii remote is in use, we're using its pointer value
            // to move the force cursor instead of the analog stick.
            if (NiIsKindOf(NiInputWiiRemote, pkGamePad))
                bUseAnalogStickForceCursor = false;
#endif
            // Only here if using GamePad for selection. Move the cursor.
            if (bUseAnalogStickForceCursor)
            {
                int iHorizVal;
                int iVertVal;
                pkGamePad->GetStickValue(NiInputGamePad::NIGP_STICK_LEFT,
                    iHorizVal, iVertVal);
                if ((iHorizVal != 0) || (iVertVal != 0))
                {
                    m_pkCursor->Move(0.0f,
                        (int)((float)iHorizVal * 2.0f * fDt),
                        (int)((float)iVertVal * 2.0f * fDt));
                }
            }

            int iX = 0;
            int iY = 0;
            m_pkCursor->GetScreenSpacePosition(iX, iY);
                
#if defined(_WII)
            // We need the A button for the menu and picking on the
            // Wii remote, so disable picking when the menu is active.
            if (!m_pkMenu->GetActive())
#endif
            {
                if (uiNewButtons & uiForcePickMask)
                {
                    // User has pushed the button down to indicate a selection
                    NiPoint3 kOrigin;
                    NiPoint3 kDir;
                    NiCamera* pkCamera =
                        m_kCameras.GetAt(m_uiActiveCamera).m_spCamera;

                    if (pkCamera->WindowPointToRay(iX, iY, kOrigin, kDir))
                    {
                        if (SelectForceActor(kOrigin, kDir))
                        {
                            m_bApplyingForce = true;
                            ((NiSystemCursor*)m_pkCursor)->
                                SetType(NiSystemCursor::CLOSEHAND);
                        }
                        else
                        {
                            m_bSelecting = false;
                            m_pkCursor->Show(false);
                        }
                    }
                }
                else if (m_bApplyingForce)
                {
                    if (uiButtons & uiForcePickMask)
                    {
                        ApplyForce(iX, iY);
                    }
                    else
                    {
                        m_bApplyingForce = false;
                        m_bSelecting = false;
                        m_pkCursor->Show(false);
                    }
                }
            }
        }
        else
        {
            if (uiNewButtons == 0)
                break;

            if (m_pkMenu->GetActive())
            {
                m_pkMenu->ButtonPress(uiNewButtons, uiButtons);
            }

            // Cursor control first
            if (!m_bPreferKeyboard && (uiNewButtons & uiForcePickMask))
            {
#if defined(_WII)
                // We need the A button for the menu and picking on the
                // Wii remote, so disable picking when the menu is active.
                if (!m_pkMenu->GetActive())
#endif
                {
                    m_bSelecting = true;
                    ((NiSystemCursor*)m_pkCursor)->
                        SetType(NiSystemCursor::BULLSEYE);
                    m_pkCursor->Show(true);
                }
            }
            else if (uiNewButtons & NiInputGamePad::NIGP_MASK_SELECT)
            {
                if (uiButtons == NiInputGamePad::NIGP_MASK_SELECT)
                {
                    m_pkMenu->SetActive(!m_pkMenu->GetActive());
                }
            }
            else if (uiNewButtons & NiInputGamePad::NIGP_MASK_START)
            {
                if (uiButtons == NiInputGamePad::NIGP_MASK_START)
                {
                    m_pkMenu->SetActive(!m_pkMenu->GetActive());
                }
            }
            else if (uiNewButtons & uiFreezeMask)
            {
                if (uiButtons == uiFreezeMask)
                {
                    ToggleFrozen();
                    m_pkMenu->MarkAsChanged();
                }
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Scene management functions
//---------------------------------------------------------------------------
bool PhysXNifViewer::CreateSceneFromNif(const char* pcFileName)
{
    NiStream kStream;
    unsigned int i;
    
    CloseScene();

    bool bLoad = false;

#if defined(_PS3) 
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
#elif defined(_XENON)
    bLoad = kStream.Load(NiApplication::ConvertMediaFilename(m_acFileName));
#elif defined(WIN32)
    bLoad = kStream.Load(pcFileName);
#elif defined(_WII)
    bLoad = kStream.Load(NiApplication::ConvertMediaFilename(m_acFileName));
#else
    #error Unknown platform!
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
    NiTObjectArray<NiPhysXPropPtr> kProps;
    for (i = 0; i < kStream.GetObjectCount(); i++)
    {
        NiObject *pkObject;

        pkObject = kStream.GetObjectAt(i);
        
        m_pspNifObjects[i] = pkObject;
        m_uiNifObjCnt++;
        
        if (NiIsKindOf(NiNode, pkObject) || NiIsKindOf(NiMesh, pkObject))
        {
            NiAVObject* pkChild = (NiAVObject*) pkObject;
            m_spBoundNode->SetAt(uiChild++, pkChild);
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
        else if (NiIsKindOf(NiPhysXProp, pkObject))
        {
            NiPhysXProp* pkProp = (NiPhysXProp*)pkObject;
            kProps.Add(pkProp);
            pkProp->SetAllDestInterp(true);
            pkProp->SetAllSrcInterp(true);
        }
        else if (NiIsKindOf(NiPhysXSDKDesc, pkObject))
        {
            m_spSDKDefaults = (NiPhysXSDKDesc*)pkObject;
            float fDebugScale =
                m_spSDKDefaults->GetParameter(NX_VISUALIZATION_SCALE); 
            if (fDebugScale > 0.0f)
                m_fDebugScale = fDebugScale;
        }
    }
    
    NIASSERT(m_uiNifObjCnt == kStream.GetObjectCount());
    
    CreateCameraList(m_spScene);

    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    
    bool bNeedAlphaSort = false;
    Traverse(m_spScene, bNeedAlphaSort);
    if (bNeedAlphaSort)
    {
        m_spRenderer->SetSorter(m_spAlphaAccumulator);
    }
    
    m_spScene->SetTranslate(NiPoint3::ZERO);
    m_spScene->SetRotate(NiMatrix3::IDENTITY);
    m_spBoundNode->SetTranslate(NiPoint3::ZERO);
    m_spScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateNodeBound();

    // Derive gravity from the camera, if gravity wasn't given
    if (!m_bGravityProvided && (m_kCameras.GetSize() > 0))
    {
        GravityFromCamera(m_kCameras.GetAt(0).m_spCamera);
    }
    
    // Create the PhysX content
    if (!CreatePhysX(kProps))
    {
        kProps.RemoveAll();
        return false;
    }
    kProps.RemoveAll();
   
    // Create ground planes.
    if (m_bDoingGround)
        AddGroundPlane();
        
    // Disable animations, if necessary
    DisableKeyframes(m_bDisableKeyframes);
    
    // Deal with cameras
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
        AdjustCameraAspectRatio(pkCamera);
        pkCamera->Update(0.0f);
    }

    if (m_kStateNames.GetSize() > 0)
    {
        ResetAnimationTime(m_kStateNames.GetAt(0));
    }
    else
    {
        NiFixedString kNullString;
        ResetAnimationTime(kNullString);
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool PhysXNifViewer::CreatePhysX(NiTObjectArray<NiPhysXPropPtr>& kProps)
{
    // check for hardware
    bool bHaveHardware = m_pkPhysManager->m_pkPhysXSDK->getHWVersion() > 0;
    
    // Do we need hardware? Do we have fluids?
    bool bWantHardware = false;
    bool bNeedHardware = false;
    
    if (kProps.GetSize()) // We loaded props.
    {
        TraverseProps(kProps, bNeedHardware, bWantHardware);

        NxSceneDesc kNxSceneDesc;
        kNxSceneDesc.simType = NX_SIMULATION_SW;
        NiPhysXTypes::NiPoint3ToNxVec3(m_kGravity, kNxSceneDesc.gravity);

        if (bNeedHardware && !bHaveHardware)
        {
            NiMessageBox(
                "Hardware scene required but no hardware found.\n",
                "Hardware Missing");
            return false;
        }

        if (bWantHardware)
        {
            if (bHaveHardware)
            {
                kNxSceneDesc.simType = NX_SIMULATION_HW;
            }
            else
            {
                NiMessageBox(
                    "Hardware scene detected, but no hardware found.\n"
                    "Switching to software scene.\n", "Hardware Scene");
                kNxSceneDesc.simType = NX_SIMULATION_SW;
            }
        }

        NiPhysXProp* pkProp = kProps.GetAt(0);
        
        m_spPhysScene = NiNew NiPhysXScene();
        
        m_spPhysScene->SetScaleFactor(pkProp->GetScaleFactor());
        
        m_pkPhysManager->WaitSDKLock();
        NxScene* pkNxScene =
            m_pkPhysManager->m_pkPhysXSDK->createScene(kNxSceneDesc);
        if (!pkNxScene && kNxSceneDesc.simType == NX_SIMULATION_HW)
        {
            kNxSceneDesc.simType = NX_SIMULATION_SW;
            pkNxScene =
                m_pkPhysManager->m_pkPhysXSDK->createScene(kNxSceneDesc);
            NiMessageBox(
                "Couldn't create hardware scene. Using Software.\n",
                "Scene Failure");
            bHaveHardware = false;
        }
        m_pkPhysManager->ReleaseSDKLock();
        
        if (!pkNxScene)
        {
            NiMessageBox(
                "Unable to create PhysX scene.\n", "Scene Failure");
            return false;
        }
        
        m_spPhysScene->SetPhysXScene(pkNxScene);
    }
    
    for (unsigned int ui = 0; ui < kProps.GetSize(); ui++)
    {
        NiPhysXProp* pkProp = kProps.GetAt(ui);
        if (m_spPhysScene->AddProp(pkProp) == (unsigned int)-1)
        {
            NiMessageBox(
                "Unable to instantiate PhysX prop. Something about your\n"
                "PhysX content is invalid. Use the PhysXSceneGraphPrinter\n"
                "application to look for bad PhysX data in your NIF file.\n",
                "Prop Creation Failure");
        }
        
        // Add state names now, after excess states have been deleted
        // post-physx-creation.
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NiTMapIterator kIter = pkSnapshot->GetFirstStateName();
            NIASSERT(kIter);
            while (kIter)
            {
                NiFixedString kName;
                NiUInt32 uiIndex;
                pkSnapshot->GetNextStateName(kIter, kName, uiIndex);
                AddStateName(kName);
            }
        }
    }

    if (bHaveHardware)
        PageMeshes();
            
    if (m_spPhysScene)
    {
        m_spPhysScene->SetUpdateDest(true);
        m_spPhysScene->SetUpdateSrc(true);
        m_spPhysScene->SetDebugRender(m_bDebugGeom, m_spScene);
        if (m_bDebugGeom)
        {
            m_pkPhysManager->m_pkPhysXSDK->
                setParameter(NX_VISUALIZATION_SCALE, 1.0f);
        }
        else
        {
            m_pkPhysManager->m_pkPhysXSDK->
                setParameter(NX_VISUALIZATION_SCALE, 0.0f);
        }

        m_spPhysScene->UpdateDestinations(0.0f, true);
        m_spPhysScene->UpdateSources(0.0f, true);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void PhysXNifViewer::Traverse(NiAVObject* pkObject, bool& bNeedAlphaSort)
{
    // Old versions of MAX plugin created NiDitherProperty objects. Remove
    // them so they don't interfere with our code to toggle dithering.
    pkObject->RemoveProperty(NiProperty::DITHER);

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
        }
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
void PhysXNifViewer::TraverseProps(NiTObjectArray<NiPhysXPropPtr>& kProps,
    bool& bNeedHardware, bool& bWantHardware)
{
    bNeedHardware = false;
    bWantHardware = false;
    
    for (unsigned int ui = 0; ui < kProps.GetSize(); ui++)
    {
        NiPhysXProp* pkProp = kProps.GetAt(ui);
        if (!pkProp)
            continue;
            
        NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();
        if (!pkPropDesc)
            continue;
        
        bWantHardware = bWantHardware || pkPropDesc->GetUseHardware();
        
        pkPropDesc->SetRBCompartmentID(0);
        pkPropDesc->SetClothCompartmentID(0);
        pkPropDesc->SetFluidCompartmentID(0);

        unsigned int uiNumActors = pkPropDesc->GetActorCount();
        for (unsigned int uj = 0; uj < uiNumActors; uj++)
        {
            NiPhysXActorDesc* pkActor = pkPropDesc->GetActorAt(uj);
            
            // Work through shapes looking for meshes cooked for hardware
            NiTObjectArray<NiPhysXShapeDescPtr>&
                kShapes = pkActor->GetActorShapes();
            for (unsigned int uk = 0; uk < kShapes.GetSize(); uk++)
            {
                NiPhysXShapeDesc* pkShape = kShapes.GetAt(uk);
                if (!pkShape)
                    continue;
                NiPhysXMeshDesc* pkMesh = pkShape->GetMeshDesc();
                if (!pkMesh)
                    continue;
                bWantHardware = bWantHardware || pkMesh->GetHardware();
            }
        }

        unsigned int uiNumClothes = pkPropDesc->GetClothCount();
        for (unsigned int uj = 0; uj < uiNumClothes; uj++)
        {
            NiPhysXClothDesc* pkCloth = pkPropDesc->GetClothAt(uj);
            
            // Look for the hardware flag
            if (pkCloth->GetClothFlags() & NX_CLF_HARDWARE)
                bWantHardware = true;
        }
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::PageMeshes()
{
    if (!m_spPhysScene)
        return;
        
    NxScene* pkScene = m_spPhysScene->GetPhysXScene();
    if (!pkScene)
        return;
        
    NxActor** ppkActors = pkScene->getActors();
    unsigned int uiNumActors = pkScene->getNbActors();
    for (unsigned int ui = 0; ui < uiNumActors; ui++)
    {
        NxShape*const* ppkShapes = ppkActors[ui]->getShapes();
        unsigned int uiNumShapes = ppkActors[ui]->getNbShapes();
        for (unsigned int uj = 0; uj < uiNumShapes; uj++)
        {
            NxTriangleMeshShape*
                pkTriShape = ppkShapes[uj]->isTriangleMesh();
            if (pkTriShape)
            {
                const NxTriangleMesh& kMesh = pkTriShape->getTriangleMesh();
                unsigned int uiNumPages = kMesh.getPageCount();
                for (unsigned int uk = 0; uk < uiNumPages; uk++)
                {
                    pkTriShape->mapPageInstance(uk);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::DisableKeyframes(bool bDisable)
{
    if (!m_spPhysScene)
        return;
        
    // Work through the destinations in the scene and disable animations
    // on the targets of Transform destinations.
    for (unsigned int uj = 0; uj < m_spPhysScene->GetPropCount(); uj++)
    {
        NiPhysXProp* pkProp = m_spPhysScene->GetPropAt(uj);
        for (unsigned int ui = 0; ui < pkProp->GetDestinationsCount(); ui++)
        {
            NiPhysXDest* pkDest = pkProp->GetDestinationAt(ui);
            NiPhysXTransformDest* pkTransformDest =
                NiDynamicCast(NiPhysXTransformDest, pkDest);
            if (!pkTransformDest)
                continue;
                
            NiAVObject* pkTarget = pkTransformDest->GetTarget();
            if (!pkTarget)
                continue;

            NiTransformController* pkCtrl =
                NiGetController(NiTransformController, pkTarget);
            if (!pkCtrl)
                continue;
                
            if (bDisable)
                pkCtrl->Stop();
            else
                pkCtrl->Start();
        }
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CloseScene()
{
    unsigned int i;

    DestroyMenu();

    NiDelete [] m_pspNifObjects;
    m_pspNifObjects = NULL;
    m_uiNifObjCnt = 0;

    // Delete the PhysX scene
    m_spGroundPlane = 0;
    m_spPhysScene = 0;
    m_spSDKDefaults = 0;
    
    if (m_spBoundNode != NULL)
    {
        for (i = 0; i < m_spBoundNode->GetArrayCount(); i++)
        {
            m_spBoundNode->DetachChildAt(i);
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
bool PhysXNifViewer::SceneIsOpen() const
{
    return (m_spBoundNode->GetChildCount() > 0);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Ground plane functions
//---------------------------------------------------------------------------
void PhysXNifViewer::AddGroundPlane()
{
    if (!m_spPhysScene || !m_spPhysScene->GetPhysXScene() || m_spGroundPlane)
        return;
        
    // get gravity
    NiPoint3 kNorm = NiPoint3(0.0f, 0.0f, 1.0f);
    NxSceneDesc kSceneDesc;
    m_spPhysScene->GetPhysXScene()->saveToDesc(kSceneDesc);
    NiPoint3 kGravity;
    NiPhysXTypes::NxVec3ToNiPoint3(kSceneDesc.gravity, kGravity);
    kGravity = -kGravity;
    if (kGravity.Unitize() > 0.0f)
        kNorm = kGravity;
        
    m_spGroundPlane = NiNew GroundPlane(m_spPhysScene, kNorm,
        m_fGroundHeight, m_spBoundNode->GetWorldBound().GetRadius() * 10.0f);  

    m_spScene->AttachChild(m_spGroundPlane->GetGeometry());
    m_spGroundPlane->GetGeometry()->UpdateEffects();
    m_spGroundPlane->GetGeometry()->UpdateProperties();
    m_spGroundPlane->GetGeometry()->Update(m_fSimulationTime);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::RemoveGroundPlane()
{
    if (m_spGroundPlane)
    {
        m_spScene->DetachChild(m_spGroundPlane->GetGeometry());
        m_spGroundPlane = 0;
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ResetPhysXSDKParams(const NxParameter kParam)
{
    if (!m_pkPhysManager)
        return;

    m_pkPhysManager->WaitSDKLock();

    if (!m_spSDKDefaults)
    {
        m_spSDKDefaults = NiNew NiPhysXSDKDesc;
        m_spSDKDefaults->FromSDK();
        m_spSDKDefaults->SetParameter(NX_VISUALIZE_BODY_AXES, 1.0f);
        m_spSDKDefaults->SetParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
        m_spSDKDefaults->SetParameter(NX_VISUALIZE_JOINT_LIMITS, 1.0f); 
        m_spSDKDefaults->SetParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1.0f); 
        // Fluids only supported on Win32.
#if defined(WIN32)
        m_spSDKDefaults->SetParameter(NX_VISUALIZE_FLUID_PACKETS, 1.0f);
        m_spSDKDefaults->SetParameter(NX_VISUALIZE_FLUID_EMITTERS, 1.0f);
#endif

    }
    if (kParam == NX_PARAMS_NUM_VALUES)
    {
        m_spSDKDefaults->ToSDK();
    }
    else
    {
        m_pkPhysManager->m_pkPhysXSDK->setParameter(kParam,
            m_spSDKDefaults->GetParameter(kParam));
    }
            
    m_pkPhysManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Parameter adjustment functions
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleDisableKeyframe()
{
    m_bDisableKeyframes = !m_bDisableKeyframes;
    m_pkDisableKeyEntry->SetCheck(m_bDisableKeyframes);
    DisableKeyframes(m_bDisableKeyframes);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustStepLength(float fStepLength)
{
    m_fStepLength = fStepLength;

    char acLabel[100];
    NiSprintf(acLabel, sizeof(acLabel), "Step Size (%7.3g)", m_fStepLength);
    m_pkStepLengthEntry->SetLabel(acLabel);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleWireframe(NiMenu::Entry* pkEntry)
{
    bool bWire = !m_spWireframe->GetWireframe();
    m_spWireframe->SetWireframe(bWire);
    pkEntry->SetCheck(bWire);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleFrozen()
{
    m_bFreeze = !m_bFreeze;
    m_pkFreezeEntry->SetCheck(m_bFreeze);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ResetAnimationTime(NiFixedString& kStateName)
{
    m_fSimulationTime = 0.0f;
    
    // Update physics
    if (m_spPhysScene)
    {
        // We know snapshot state 0 exists because we loaded the scene
        m_spPhysScene->RestoreSnapshotState(kStateName);

        // Update scene to get updated sources
        m_spScene->Update(m_fSimulationTime);
        
        m_spPhysScene->SetFetchTime(m_fSimulationTime);
        m_spPhysScene->UpdateSources(m_fSimulationTime, true);

        if (m_spPhysScene->Simulate(m_fSimulationTime, true))
        {
            m_spPhysScene->FetchResults(m_fSimulationTime, true);
            m_spPhysScene->UpdateDestinations(m_fSimulationTime, true);
        } 
        
        // And update to get new physics state
        m_spScene->Update(m_fSimulationTime);
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleInvertGamepad(NiMenu::Entry* pkEntry)
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
            pkGamePad->SetStickInvertFlags(
                NiInputGamePad::NIGP_INVERT_RIGHT_V);
        else
            pkGamePad->SetStickInvertFlags(0);

    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleDebugRender(NiMenu::Entry* pkEntry)
{
    if (m_bDebugGeom)
    {
        if (m_spPhysScene)
        {
            m_spPhysScene->SetDebugRender(false);
            ToggleClothDebug(false);
        }
        m_pkPhysManager->m_pkPhysXSDK->setParameter(
            NX_VISUALIZATION_SCALE, 0.0f);
        pkEntry->SetCheck(false);
        m_bDebugGeom = false;
    }
    else
    {
        if (m_spPhysScene)
        {
            m_spPhysScene->SetDebugRender(true, m_spScene);
            ToggleClothDebug(true);
        }
        m_pkPhysManager->m_pkPhysXSDK->setParameter(
            NX_VISUALIZATION_SCALE, m_fDebugScale);
        pkEntry->SetCheck(true);
        m_bDebugGeom = true;
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleClothDebug(bool bState)
{
    unsigned int uiNumProps = m_spPhysScene->GetPropCount();
    for (unsigned int ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_spPhysScene->GetPropAt(ui);
        if (!pkProp)
            continue;
            
        NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();
        if (!pkPropDesc)
            continue;

        unsigned int uiNumCloth = pkPropDesc->GetClothCount();
        for (unsigned int uj = 0; uj < uiNumCloth; uj++)
        {
            NiPhysXClothDesc* pkCloth = pkPropDesc->GetClothAt(uj);
            
            NxU32 kFlags = pkCloth->GetCloth()->getFlags();
            if (bState)
                kFlags |= NX_CLF_VISUALIZATION;
            else
                kFlags &= (~NX_CLF_VISUALIZATION);
            
            pkCloth->GetCloth()->setFlags(kFlags);
        }
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustRotationSpeed(bool bFaster)
{
    if (bFaster)
    {
        m_iRotSpeed++;
    }
    else
    {
        m_iRotSpeed--;
    }

    char acLabel[100];
    NiSprintf(acLabel, sizeof(acLabel), "Rotate (%d)", m_iRotSpeed);
    m_pkRotateSpeedEntry->SetLabel(acLabel);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustTranslationSpeed(bool bFaster)
{
    if (bFaster)
    {
        m_iTrnSpeed++;
    }
    else
    {
        m_iTrnSpeed--;
    }

    char acLabel[100];
    NiSprintf(acLabel, sizeof(acLabel), "Translate (%d)", m_iTrnSpeed);
    m_pkTranslateSpeedEntry->SetLabel(acLabel);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustAnimationSpeed(float fAnimationSpeed)
{
    m_fAnimationSpeed = fAnimationSpeed;
    int iPercent = (int) (100.0f * m_fAnimationSpeed);
    char acLabel[100];
    NiSprintf(acLabel, sizeof(acLabel), "Speed (%d%%)", iPercent);
    m_pkAnimateSpeedEntry->SetLabel(acLabel);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleGravityMode(NiMenu::Entry* pkEntry)
{
    m_bLockGravity = !m_bLockGravity;
    pkEntry->SetCheck(m_bLockGravity);
    if (m_bLockGravity)
    {
        // Get object to adjust
        NiAVObject *pkObjectHoldingXform = GetCamera(m_uiActiveCamera);
        LockGravity(pkObjectHoldingXform);
    }
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustDebugScale(float fRatio)
{
    m_fDebugScale *= fRatio;
    
    if (!m_pkPhysManager)
        return;

    m_pkPhysManager->WaitSDKLock();

    float fDebugScale =
        m_pkPhysManager->m_pkPhysXSDK->getParameter(NX_VISUALIZATION_SCALE);
    if (fDebugScale > 0.0f)
    {
        m_pkPhysManager->m_pkPhysXSDK->setParameter(NX_VISUALIZATION_SCALE,
            m_fDebugScale);
    }
    m_pkPhysManager->ReleaseSDKLock();
    
    char acLabel[100];
    NiSprintf(acLabel, 100, "Debug Scale (%7.3g)", m_fDebugScale);
    m_pkDebugScaleEntry->SetLabel(acLabel);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ToggleGroundPlane(NiMenu::Entry* pkEntry)
{
    if (m_bDoingGround)
    {
        RemoveGroundPlane();
        m_bDoingGround = false;
    }
    else
    {
        AddGroundPlane();
        m_bDoingGround = true;
    }
    pkEntry->SetCheck(m_bDoingGround);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustGroundHeight(const int iSign)
{
    if (m_fGroundHeight < 0.0001f && m_fGroundHeight > -0.0001f)
    {
        if (iSign > 0)
            m_fGroundHeight = 1.0f;
        else
            m_fGroundHeight = -1.0f;
    }
    else
    {
        if (iSign > 0)
        {
            if (m_fGroundHeight > 0.0)
                m_fGroundHeight *= 1.2f;
            else
                m_fGroundHeight *= 0.8f;
        }
        else
        {
            if (m_fGroundHeight > 0.0)
                m_fGroundHeight *= 0.8f;
            else
                m_fGroundHeight *= 1.2f;
        }
    }
    
    if (m_bDoingGround)
    {
        RemoveGroundPlane();
        AddGroundPlane();
    }
    
    char acLabel[100];
#if defined(_WII)
    NiSprintf(acLabel, 100, "Height (%7.3g)", m_fGroundHeight);
#else
    NiSprintf(acLabel, 100, "Ground Height (%7.3g)", m_fGroundHeight);
#endif
    m_pkGroundHeightEntry->SetLabel(acLabel);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AdjustForceExponent(int iNewVal)
{
    m_iForceExponent = iNewVal;
    
    char acLabel[100];
    NiSprintf(acLabel, sizeof(acLabel), "Force Exp. (%d)", iNewVal);
    m_pkForceExponentEntry->SetLabel(acLabel);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Camera set-up functions
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateDefaultCamera()
{
    NiCamera *pkCamera = NiNew NiCamera;
    NIASSERT(pkCamera != NULL);
    pkCamera->SetName("PhysXNifViewer");

    CalcCameraFrustum(pkCamera);
    CalcInitialCameraTransform(pkCamera);

    AddCamera(pkCamera);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::CalcCameraFrustum(NiCamera *pkCamera) const
{
    float fSceneRadius = m_spBoundNode->GetWorldBound().GetRadius();
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
void PhysXNifViewer::CalcInitialCameraTransform(NiCamera *pkCamera) const
{
    NiPoint3 kSceneCenter, kInitialCameraPos;
    float fSceneRadius;

    kSceneCenter = m_spScene->GetWorldBound().GetCenter();
    fSceneRadius = m_spBoundNode->GetWorldBound().GetRadius();

    // Rotate and position the camera so that gravity is down and we
    // are looking as close as possible along the y axis. If gravity is
    // the y-axis, then we are looking along z.
    NiPoint3 kUp = NiPoint3(0.0f, 0.0f, 1.0f);
    if (m_spPhysScene && m_spPhysScene->GetPhysXScene())
    {
        NxSceneDesc kSceneDesc;
        m_spPhysScene->GetPhysXScene()->saveToDesc(kSceneDesc);
        NiPoint3 kGravity;
        NiPhysXTypes::NxVec3ToNiPoint3(kSceneDesc.gravity, kGravity);
        kGravity = -kGravity;
        if (kGravity.Unitize() > 0.0f)
            kUp = kGravity;
    }
    NiPoint3 kLeft = NiPoint3::UNIT_Y.Cross(kUp);
    if (kLeft.Unitize() < 0.001f)
    {
        kLeft = NiPoint3::UNIT_X.Cross(kUp);
        kLeft.Unitize();
    }
    NiPoint3 kIn = kUp.Cross(kLeft);

    kInitialCameraPos = kSceneCenter - 2.0f * fSceneRadius * kIn;
    
    NiMatrix3 kRot(kIn, kUp, kLeft);
    
    pkCamera->SetRotate(kRot);
    pkCamera->SetTranslate(kInitialCameraPos);
}

//---------------------------------------------------------------------------
bool PhysXNifViewer::UpdateTurret(const float fDt)
{
    float fRotSpeed = INIT_ROT_SPEED * NiPow(2.0f, (float)m_iRotSpeed) * fDt;
    float fTrnSpeed = INIT_TRN_SPEED * NiPow(2.0f, (float)m_iTrnSpeed) * fDt
        * m_spBoundNode->GetWorldBound().GetRadius();
    for (unsigned int i = 0; i < 3; i++)
    {
        m_kTurret.SetRotSpeed(i, fRotSpeed);
        m_kTurret.SetTrnSpeed(i, fTrnSpeed);
    }
    
    bool bResult = m_kTurret.Read();
    if (!bResult)
        return false;

    // Lock the up axis
    if (m_bLockGravity)
    {
        // Get object to adjust
        NiAVObject *pkObjectHoldingXform = GetCamera(m_uiActiveCamera);
        LockGravity(pkObjectHoldingXform);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void PhysXNifViewer::LockGravity(NiAVObject *pkObjectHoldingXform)
{
    // Rotate and position the camera so that gravity is down.
    NiPoint3 kUp = NiPoint3(0.0f, 0.0f, 1.0f);
    if (m_spPhysScene && m_spPhysScene->GetPhysXScene())
    {
        NxSceneDesc kSceneDesc;
        m_spPhysScene->GetPhysXScene()->saveToDesc(kSceneDesc);
        NiPoint3 kGravity;
        NiPhysXTypes::NxVec3ToNiPoint3(kSceneDesc.gravity, kGravity);
        kGravity = -kGravity;
        if (kGravity.Unitize() > 0.0f)
            kUp = kGravity;
    }

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
    NiPoint3 kUpInParent = kParentRot * kUp;

    // Now make the camera object's local transform have the same up directon
    NiMatrix3 kLocalRot = pkObjectHoldingXform->GetRotate();

    NiPoint3 kLook;
    kLocalRot.GetCol(0, kLook);
    NiPoint3 kRight = kLook.Cross(kUpInParent);
    if (kRight.Unitize() < 1.0e-2)
    {
        // Looking along up. If Look and target Up are parallel, then
        // existing right must already be perpendicular. So construct a view
        // with the desired up pointing directly up.
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
//---------------------------------------------------------------------------
void PhysXNifViewer::GravityFromCamera(NiCamera *pkCamera)
{
    NiMatrix3 kCameraRotate = pkCamera->GetWorldRotate();
    NiPoint3 kCameraUp(0.0f, 1.0f, 0.0f);
    kCameraRotate.GetCol(1, kCameraUp);
    m_kGravity.x = kCameraUp.x * -9.81f;
    m_kGravity.y = kCameraUp.y * -9.81f;
    m_kGravity.z = kCameraUp.z * -9.81f;
}
//---------------------------------------------------------------------------
void PhysXNifViewer::SelectCamera(unsigned int uiCamera)
{
    m_pkCameraEntry->GetSubEntry(m_uiActiveCamera)->SetCheck(false);
    m_uiActiveCamera = uiCamera;
    m_pkCameraEntry->GetSubEntry(m_uiActiveCamera)->SetCheck(true);
    if (m_kTurret.GetInputDevice() == NiTurret::TUR_KEYBOARD)
        AttachKeyboard();
    else
        AttachGamePad();
}

//---------------------------------------------------------------------------
void PhysXNifViewer::AddCamera(NiCamera *pkCamera)
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
NiCamera *PhysXNifViewer::GetCamera(unsigned int i) const
{
    NIASSERT(i < m_kCameras.GetSize());
    return m_kCameras.GetAt(i).m_spCamera;
}

//---------------------------------------------------------------------------
void PhysXNifViewer::ViewReset()
{
    NiCamera *pkCamera;

    pkCamera = GetCamera(m_uiActiveCamera);
    pkCamera->SetRotate(m_kCameras.GetAt(m_uiActiveCamera).m_kInitialRot);
    pkCamera->SetTranslate(m_kCameras.GetAt(m_uiActiveCamera).m_kInitialPos);
    pkCamera->Update(0.0f);

    m_spScene->SetRotate(NiMatrix3::IDENTITY);
    m_spScene->Update(m_fSimulationTime);

    if (m_kTurret.GetInputDevice() == NiTurret::TUR_KEYBOARD)
        AttachKeyboard();
    else
        AttachGamePad();
}
//---------------------------------------------------------------------------
void PhysXNifViewer::SwitchCamera(int iIncrement)
{
    unsigned int uiCameraCount;
    int iCamera;
    
    uiCameraCount = m_kCameras.GetSize();
    iCamera = (m_uiActiveCamera + uiCameraCount + iIncrement) % uiCameraCount;
    SelectCamera(iCamera);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::AttachKeyboard()
{
    NiCamera *pkCamera;
    unsigned int i;

    pkCamera = GetCamera(m_uiActiveCamera);

    for (i = 0; i < 3; i++)
    {
        m_kTurret.SetTrnObject(i, pkCamera);
        m_kTurret.SetRotObject(i, pkCamera);
        m_kTurret.SetTrnModifiers(i, NiInputKeyboard::KMOD_NONE);
        m_kTurret.SetRotModifiers(i, NiInputKeyboard::KMOD_NONE);
        m_kTurret.SetRotUseUpdateAxes(i, true, true);
    }

    m_kTurret.SetTrnButtonsKB(0,
        NiInputKeyboard::KEY_Z, NiInputKeyboard::KEY_X);
    m_kTurret.SetTrnButtonsKB(1,
        NiInputKeyboard::KEY_S, NiInputKeyboard::KEY_W);
    m_kTurret.SetTrnButtonsKB(2,
        NiInputKeyboard::KEY_A, NiInputKeyboard::KEY_D);
        
    m_kTurret.SetRotButtonsKB(0,
        NiInputKeyboard::KEY_U, NiInputKeyboard::KEY_I);
    m_kTurret.SetRotButtonsKB(1,
        NiInputKeyboard::KEY_J, NiInputKeyboard::KEY_L);
    m_kTurret.SetRotButtonsKB(2,
        NiInputKeyboard::KEY_M, NiInputKeyboard::KEY_K);

    m_kTurret.SetAxes(pkCamera->GetRotate());
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AttachGamePad()
{
    NiCamera *pkCamera;
    unsigned int i;

    pkCamera = GetCamera(m_uiActiveCamera);

    for (i = 0; i < 3; i++)
    {
        m_kTurret.SetTrnObject(i, pkCamera);
        m_kTurret.SetTrnModifiers(i, 0);
        m_kTurret.SetRotObject(i, pkCamera);
        m_kTurret.SetRotModifiers(i, NiInputGamePad::NIGP_MASK_NONE);
        m_kTurret.SetRotUseUpdateAxes(i, true, true);
    }

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
            NiInputGamePad::NIGP_L1, 
            NiInputGamePad::NIGP_R1);
#endif
    m_kTurret.SetTrnButtonsStickDirGP(2, 
        NiInputGamePad::NIGP_STICK_LEFT, 
        NiInputGamePad::NIGP_STICK_AXIS_H);

    m_kTurret.SetRotModifiers(0, NiInputGamePad::NIGP_MASK_RLEFT);
    m_kTurret.SetRotButtonsStickDirGP(0, 
        NiInputGamePad::NIGP_STICK_LEFT, 
        NiInputGamePad::NIGP_STICK_AXIS_H);
    m_kTurret.SetRotButtonsStickDirGP(1, 
        NiInputGamePad::NIGP_STICK_RIGHT, 
        NiInputGamePad::NIGP_STICK_AXIS_H);
    m_kTurret.SetRotButtonsStickDirGP(2, 
        NiInputGamePad::NIGP_STICK_RIGHT, 
        NiInputGamePad::NIGP_STICK_AXIS_V);

    m_kTurret.SetAxes(pkCamera->GetRotate());
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Picking functions
//---------------------------------------------------------------------------
bool PhysXNifViewer::CreateCursor()
{
    // Find the font texture
    char acPath[NI_MAX_PATH];
#ifndef WIN32
    NiStrcpy(acPath, NI_MAX_PATH, GetMediaPath());
#else
    NIVERIFY(NiPath::GetExecutableDirectory(acPath, NI_MAX_PATH));
#endif

    // append the file name to the end.
    size_t stPathLen = strlen(acPath);
    char *pcCopyLoc = &acPath[stPathLen];

    size_t stSpace = NI_MAX_PATH - stPathLen;

#ifdef WIN32
    NiStrcpy(pcCopyLoc, stSpace, "..\\");
    pcCopyLoc += 3;
    stSpace -= 3;
#endif
   
    NiStrcpy(pcCopyLoc, stSpace, "SystemCursors.tga");

    NiRect<int> kRect;
    const NiRenderTargetGroup* pkRTGroup = 
        m_spRenderer->GetDefaultRenderTargetGroup();
    kRect.m_left = 0;
    kRect.m_top = 0;
    kRect.m_right = pkRTGroup->GetWidth(0);
    kRect.m_bottom = pkRTGroup->GetHeight(0);

    m_pkCursor = NiSystemCursor::Create(m_spRenderer, 
        kRect, 
        NiSystemCursor::STANDARD, 
        //NiSystemCursor::SystemCursor::HAND, 
        acPath, 
        pkRTGroup);

    if (m_pkCursor == 0)
        return false;

    m_pkCursor->SetPosition(0.0f,
        pkRTGroup->GetWidth(0) / 2, pkRTGroup->GetHeight(0) / 2);
    m_pkCursor->Show(true);

#if defined(_WII)
    // Load the nav pointer image.
    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;
    m_spNavPointer = NiSourceTexture::Create(
        ConvertMediaFilename("WiiCursor_Nav.tga"), kPrefs);
    if (!m_spNavPointer)
        return false;

    // Create the cursor with the nav image.
    m_spNavCursor = NiCursor::Create(
        m_spRenderer, kRect, 3, 2, m_spNavPointer, pkRTGroup);
    if (m_spNavCursor == NULL)
        return false;
    m_spNavCursor->SetPosition(0.0f,
        pkRTGroup->GetWidth(0) / 2, pkRTGroup->GetHeight(0) / 2);
#endif
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Force application functions
//---------------------------------------------------------------------------
bool PhysXNifViewer::SelectForceActor(NiPoint3& kOrigin, NiPoint3& kDir)
{
    // If there's not a PhysX scene, we can't pick any actor.
    if (!m_spPhysScene)
        return false;

    // Ray is in PhysX space, which is the same as Gamebryo World space
    // except for the scale factor.
    NxScene* pkScene = m_spPhysScene->GetPhysXScene();
    NxRay kRay;
    NiPhysXTypes::NiPoint3ToNxVec3(kOrigin, kRay.orig);
    kRay.orig /= m_spPhysScene->GetScaleFactor();
    NiPhysXTypes::NiPoint3ToNxVec3(kDir, kRay.dir);

    // Check for an actor under the mouse.
    NxRaycastHit kHitInfo;
    NxShape* pkHitShape;
    float fDistance = 1.0e20f;
    bool bHitSomething = false;
    pkHitShape =
        pkScene->raycastClosestShape(kRay, NX_DYNAMIC_SHAPES, kHitInfo);
    if (pkHitShape)
    {
        // Find the actor
        NxActor& kActor = pkHitShape->getActor();
        if (!kActor.readBodyFlag(NX_BF_KINEMATIC))
        {
            fDistance = kHitInfo.distance;
            bHitSomething = true;

            m_pkForceActor = &kActor;
            m_pkForceCloth = 0;
            
            // Convert the hit point in to local actor coordinates
            NxMat34 kActorPose = m_pkForceActor->getGlobalPose();
            kActorPose.multiplyByInverseRT(kHitInfo.worldImpact, m_kForcePt);
        }
    }
    
    // Check for cloth under the mouse
    unsigned int uiNumCloth = pkScene->getNbCloths();
    NxCloth** ppkCloth = pkScene->getCloths();
    for (unsigned int ui = 0; ui < uiNumCloth; ui++)
    {
        NxCloth* pkCloth = ppkCloth[ui];
        NxVec3 kHit;
        NxU32 uiID;
        if (pkCloth->raycast(kRay, kHit, uiID))
        {
            float fThisDist = kHit.distance(kRay.orig);
            if (!bHitSomething || fThisDist < fDistance)
            {
                m_pkForceActor = 0;
                m_pkForceCloth = pkCloth;
                m_uiForceVertex = uiID;
                fDistance = fThisDist;
                bHitSomething = true;
            }
        }
    }
    
    // See what, if anything, was the closest thing hit.
    if (bHitSomething)
    {
        if (m_pkForceActor)
        {
            m_fForceMass = m_pkForceActor->getMass();
        }
        else if (m_pkForceCloth)
        {
            NiPhysXClothMesh* pkMeshDesc = m_pkPhysManager->GetClothMesh(
                m_pkForceCloth->getClothMesh());
            m_fForceMass =
                m_pkForceCloth->getDensity() * pkMeshDesc->GetArea();
        }
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
void PhysXNifViewer::ApplyForce(int iPtrX, int iPtrY)
{
    if (m_bFreeze)
        return;

    // Get the force application point
    NxVec3 kWorldPt;
    if (m_pkForceActor)
    {
        NxMat34 kActorPose = m_pkForceActor->getGlobalPose();
        kActorPose.multiply(m_kForcePt, kWorldPt);
    }
    else if (m_pkForceCloth)
    {
        NxMeshData kMeshData = m_pkForceCloth->getMeshData();
        NIASSERT(m_uiForceVertex < kMeshData.numVerticesPtr[0]);
        kWorldPt = *(NxVec3*)((unsigned char*)kMeshData.verticesPosBegin +
            m_uiForceVertex * kMeshData.verticesPosByteStride);
    }
    else
    {
        NIASSERT(false && "How am I applying force with no actor or cloth\n");
    }
    
    kWorldPt *= m_spPhysScene->GetScaleFactor();
     
    // Convert the world point to screen coordinates.
    NiCamera* pkCamera = m_kCameras.GetAt(m_uiActiveCamera).m_spCamera;

    NiPoint3 kPt;
    NiPhysXTypes::NxVec3ToNiPoint3(kWorldPt, kPt);
    
    float fNDCX;
    float fNDCY;
    if (pkCamera->WorldPtToScreenPt(kPt, fNDCX, fNDCY))
    {
        unsigned int uiWidth = GetAppWindow()->GetWidth();
        unsigned int uiHeight = GetAppWindow()->GetHeight();
        
        int uiObjX = (int)(fNDCX * uiWidth);
        int uiObjY = (int)((1.0f - fNDCY) * uiHeight);
        
        float fDx = (float)iPtrX - (float)uiObjX;
        float fDy = (float)uiObjY - (float)iPtrY;
        float dist = NiSqrt(fDx * fDx + fDy * fDy)
            / m_spPhysScene->GetScaleFactor();
        
        float fMagnitude = dist * m_fForceMass * FORCE_SPRING_CONST *
            NiPow(2.0f, (float)m_iForceExponent);
        
        // Get the view basis
        NiPoint3 kUp = pkCamera->GetWorldUpVector();
        NiPoint3 kRight = pkCamera->GetWorldRightVector();
        NiPoint3 kDir = kRight * fDx + kUp * fDy;
        NiPoint3::UnitizeVector(kDir);
        kDir *= fMagnitude;

        NxVec3 kForce;
        NiPhysXTypes::NiPoint3ToNxVec3(kDir, kForce);
        
        if (m_pkForceActor)
            m_pkForceActor->addForceAtPos(kForce, kWorldPt);
        else
            m_pkForceCloth->addForceAtVertex(kForce, m_uiForceVertex);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Menu functions
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenuFont()
{
    // Find the font texture
    char acPath[NI_MAX_PATH];
#ifndef WIN32
    NiStrcpy(acPath, NI_MAX_PATH, GetMediaPath());
#else
    NIVERIFY(NiPath::GetExecutableDirectory(acPath, NI_MAX_PATH));
#endif

    // append the file name to the end.
    size_t stPathLen = strlen(acPath);
    char *pcCopyLoc = &acPath[stPathLen];
    
    size_t stSpace = NI_MAX_PATH - stPathLen;
   
#ifdef WIN32
    NiStrcpy(pcCopyLoc, stSpace, "..\\");
    pcCopyLoc += 3;
    stSpace -= 3;
#endif

    NiStrcpy(pcCopyLoc, stSpace, "COURIER10.TGA");

    m_spFontTexture = NiSourceTexture::Create(acPath);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenu()
{
    NIASSERT(m_pkMenu == NULL);

    const unsigned int uiMaxPages = 6;
#if defined(_XENON)
    const unsigned int uiPosX = 32;
#elif defined(_PS3)
    const unsigned int uiPosX = 32;
#elif defined(WIN32)
    const unsigned int uiPosX = 32;
#elif defined(_WII)
    const unsigned int uiPosX = 16;
#else
    #error Unknown platform!
#endif

    const unsigned int uiPosY = 32;
        
    m_pkMenu = NiNew NiMenu(m_spFontTexture, uiMaxPages, m_uiFontWidth,
        m_uiFontHeight, m_uiFontColumns, uiPosX, uiPosY, MenuExecuteCallback,
        this);
        
    // add the texture to the UI list
    m_kUITextureClicks.AddTail(m_pkMenu->GetRenderClick());
    m_pkMenu->GetRenderClick()->SetPreProcessingCallbackFunc(
        UpdateGUI_Callback, this);

    CreateMenuFile();
    CreateMenuView();
    CreateMenuGamepad();
    CreateMenuAnimation();
    CreateMenuSDK();
    CreateMenuGround();
    
    m_pkMenu->MarkAsChanged();
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateStats()
{
    m_pkStats = NiNew NiStats(m_spRenderer, m_spFontTexture, m_uiFontWidth,
        m_uiFontHeight, m_uiFontColumns);
    m_kUITextureClicks.AddTail(m_pkStats->GetRenderClick());
    m_pkStats->GetRenderClick()->SetPreProcessingCallbackFunc(
        UpdateStats_Callback, this);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenuFile()
{
    const unsigned uiEntries = 1;
    
    NiMenu::Page* pkPage = m_pkMenu->AddPage(" File ", uiEntries);

    pkPage->AddEntry("Exit", EXIT);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenuView()
{
    const NiUInt32 uiEntries = 9;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" View ", uiEntries);

    m_pkCameraEntry = pkPage->AddEntry("Camera", (NiUInt32)~0);
    NiUInt32 uiCameras = m_kCameras.GetSize();

    m_pkCameraEntry->AllocateSubEntries(uiCameras);
    for (NiUInt32 i = 0; i < uiCameras; i++)
    {
        enum
        {
            MAX_LEN = 20
        };
        char acLabel[MAX_LEN];
        NiSprintf(acLabel, sizeof(acLabel), "%d: ", i);
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

    pkPage->AddEntry("Wireframe", WIRE);
    pkPage->AddEntry("Reset", VIEW_RESET);
    const char* pcLabel = "FPS";
    pkPage->AddEntry(pcLabel, STATS);

    NiMenu::Entry* pkSorterEntry = pkPage->AddEntry("Sorter", (NiUInt32)~0);
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
void PhysXNifViewer::CreateMenuAnimation()
{
    const NiUInt32 uiEntries = 6;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" Animation ", uiEntries);

    m_pkFreezeEntry = pkPage->AddEntry("Freeze", FREEZE);
    m_pkFreezeEntry->SetCheck(m_bFreeze);

    pkPage->AddEntry("Step", STEP);

    m_pkStepLengthEntry = pkPage->AddEntry("Step Size (    0.5) ", (unsigned int)~0);
    m_pkStepLengthEntry->AllocateSubEntries(3);
    m_pkStepLengthEntry->AddSubEntry("Larger", STEP_LARGER);
    m_pkStepLengthEntry->AddSubEntry("Smaller", STEP_SMALLER);
    m_pkStepLengthEntry->AddSubEntry("Reset", STEP_RESET);

    m_pkPhysicsResetEntry = pkPage->AddEntry("Reset Time", (NiUInt32)~0);
    NiUInt32 uiNumStates = m_kStateNames.GetSize();
    m_pkPhysicsResetEntry->AllocateSubEntries(uiNumStates);
    for (NiUInt32 ui = 0; ui < uiNumStates; ui++)
    {
        m_pkPhysicsResetEntry->AddSubEntry(
            m_kStateNames.GetAt(ui), PHYSICS_RESET + ui);
    }
   
    m_pkDisableKeyEntry = pkPage->AddEntry("Disable Keys", DISABLE_KEYS);
    m_pkDisableKeyEntry->SetCheck(m_bDisableKeyframes);
    
    m_pkAnimateSpeedEntry = pkPage->AddEntry("Speed (100%) ", (NiUInt32)~0);
    m_pkAnimateSpeedEntry->AllocateSubEntries(3);
    m_pkAnimateSpeedEntry->AddSubEntry("Slower", ANIMATE_SLOWER);
    m_pkAnimateSpeedEntry->AddSubEntry("Faster", ANIMATE_FASTER);
    m_pkAnimateSpeedEntry->AddSubEntry("Reset", ANIMATE_SPEED_RESET);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenuGamepad()
{
    const NiUInt32 uiEntries = 4;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" Controls ", uiEntries);

    NiMenu::Entry* pkInvertEntry = pkPage->AddEntry("Invert Camera", INVERT);
    pkInvertEntry->SetCheck(m_bInvertGamepad);

    m_pkRotateSpeedEntry = pkPage->AddEntry("Rotate (0)  ", (NiUInt32)~0);
    m_pkRotateSpeedEntry->AllocateSubEntries(2);
    m_pkRotateSpeedEntry->AddSubEntry("Slower", ROTATE_SLOWER);
    m_pkRotateSpeedEntry->AddSubEntry("Faster", ROTATE_FASTER);

    m_pkTranslateSpeedEntry = pkPage->AddEntry("Translate (0)  ",
        (NiUInt32)~0);
    m_pkTranslateSpeedEntry->AllocateSubEntries(2);
    m_pkTranslateSpeedEntry->AddSubEntry("Slower", TRANSLATE_SLOWER);
    m_pkTranslateSpeedEntry->AddSubEntry("Faster", TRANSLATE_FASTER);

    NiMenu::Entry* pkGravityEntry = pkPage->AddEntry("Gravity Down", GRAVITY);
    pkGravityEntry->SetCheck(m_bLockGravity);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenuSDK()
{
    char acLabel[100];

    const NiUInt32 uiEntries = 4;

#if defined(_WII)
    NiMenu::Page* pkPage = m_pkMenu->AddPage(" PhysX ", uiEntries);
#else
    NiMenu::Page* pkPage = m_pkMenu->AddPage(" SDK Settings ", uiEntries);
#endif

    pkPage->AddEntry("Reset Defaults", SDK_RESET);

    NiSprintf(acLabel, sizeof(acLabel), "Force Exp. (%d)", m_iForceExponent);
    m_pkForceExponentEntry = pkPage->AddEntry(acLabel, (NiUInt32)~0);
    m_pkForceExponentEntry->AllocateSubEntries(3);
    m_pkForceExponentEntry->AddSubEntry("Higher", FORCE_EXPONENT_BIGGER);
    m_pkForceExponentEntry->AddSubEntry("Lower", FORCE_EXPONENT_SMALLER);
    m_pkForceExponentEntry->AddSubEntry("Reset", FORCE_EXPONENT_RESET);

    NiMenu::Entry* pkDebugEntry = pkPage->AddEntry("Debug Viz On", DEBUG_ON);
    pkDebugEntry->SetCheck(m_bDebugGeom);

    NiSprintf(acLabel, sizeof(acLabel), "Debug Scale (%7.3g)", m_fDebugScale);
    m_pkDebugScaleEntry = pkPage->AddEntry(acLabel, (NiUInt32)~0);
    m_pkDebugScaleEntry->AllocateSubEntries(3);
    m_pkDebugScaleEntry->AddSubEntry("Bigger", DEBUG_SCALE_BIGGER);
    m_pkDebugScaleEntry->AddSubEntry("Smaller", DEBUG_SCALE_SMALLER);
    m_pkDebugScaleEntry->AddSubEntry("Reset", DEBUG_SCALE_RESET);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateMenuGround()
{
    char acLabel[100];

    const NiUInt32 uiEntries = 2;

    NiMenu::Page* pkPage = m_pkMenu->AddPage(" Ground Plane ", uiEntries);

    NiMenu::Entry* pkGroundEntry =
#if defined(_WII)
        pkPage->AddEntry("Plane", GROUND_PLANE_ON);
#else
        pkPage->AddEntry("Ground Plane", GROUND_PLANE_ON);
#endif
    pkGroundEntry->SetCheck(m_bDoingGround);

#if defined(_WII)
    NiSprintf(acLabel, 100, "Height (%7.3g)", m_fGroundHeight);
#else
    NiSprintf(acLabel, 100, "Ground Height (%7.3g)", m_fGroundHeight);
#endif
    m_pkGroundHeightEntry = pkPage->AddEntry(acLabel, (NiUInt32)~0);
    m_pkGroundHeightEntry->AllocateSubEntries(2);
    m_pkGroundHeightEntry->AddSubEntry("Higher", GROUND_PLANE_HIGHER);
    m_pkGroundHeightEntry->AddSubEntry("Lower", GROUND_PLANE_LOWER);
}
//---------------------------------------------------------------------------
void PhysXNifViewer::MenuExecuteCallback(NiMenu::Entry* pkEntry,
    void* pvCallbackData)
{
    PhysXNifViewer* pkViewer = (PhysXNifViewer*) pvCallbackData;
    pkViewer->OnMenuCommand(pkEntry);
}

//---------------------------------------------------------------------------
void PhysXNifViewer::OnMenuCommand(NiMenu::Entry* pkEntry)
{
    NiUInt32 uiCommand = pkEntry->GetCommand();

    switch (uiCommand)
    {
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
            ToggleFrozen();
            break;

        case STEP:
            m_bSingleStep = true;
            m_fStepLengthRem = m_fStepLength;
            m_bFreeze = false;
            break;

        case STEP_LARGER:
            AdjustStepLength(m_fStepLength * 2.0f);
            break;

        case STEP_SMALLER:
            AdjustStepLength(m_fStepLength * 0.5f);
            break;

        case STEP_RESET:
            AdjustStepLength(0.5f);
            break;

        case DISABLE_KEYS:
            ToggleDisableKeyframe();
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

        case GRAVITY:
            ToggleGravityMode(pkEntry);
            break;
            
        case SDK_RESET:
            ResetPhysXSDKParams();
            break;

        case FORCE_EXPONENT_BIGGER:
            AdjustForceExponent(m_iForceExponent + 1);
            break;

        case FORCE_EXPONENT_SMALLER:
            AdjustForceExponent(m_iForceExponent - 1);
            break;

        case FORCE_EXPONENT_RESET:
            AdjustForceExponent(0);
            break;
        
        case DEBUG_ON:
            ToggleDebugRender(pkEntry);
            break;
            
        case DEBUG_SCALE_BIGGER:
            AdjustDebugScale(1.2f);
            break;
            
        case DEBUG_SCALE_SMALLER:
            AdjustDebugScale(1.0f / 1.2f);
            break;

        case DEBUG_SCALE_RESET:
            ResetPhysXSDKParams(NX_VISUALIZATION_SCALE);
            break;

        case GROUND_PLANE_ON:
            ToggleGroundPlane(pkEntry);
            break;
            
        case GROUND_PLANE_HIGHER:
            AdjustGroundHeight(1);
            break;
            
        case GROUND_PLANE_LOWER:
            AdjustGroundHeight(-1);
            break;
            
        default:
            // Camera selection or state reset
            if (uiCommand >= SELECT_CAMERA &&
                uiCommand < SELECT_CAMERA + m_kCameras.GetSize())
            {
                NiUInt32 uiCamera = uiCommand - SELECT_CAMERA;
                SelectCamera(uiCamera);
            }
            else if (uiCommand >= PHYSICS_RESET &&
                uiCommand < PHYSICS_RESET + m_kStateNames.GetSize())
            {
                NiFixedString& kName =
                    m_kStateNames.GetAt(uiCommand - PHYSICS_RESET);
                ResetAnimationTime(kName);
            }
            break;
    }
}

//---------------------------------------------------------------------------
void PhysXNifViewer::DestroyMenu()
{
    NiDelete m_pkMenu;
    m_pkMenu = NULL;
}
//---------------------------------------------------------------------------
void PhysXNifViewer::CreateCameraList(NiAVObject* pkObject)
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
bool PhysXNifViewer::UpdateGUI_Callback(NiRenderClick*, void* pvThis)
{
    PhysXNifViewer* pkThis = (PhysXNifViewer*)pvThis;

    pkThis->m_pkMenu->Update();

    return true;
}
//---------------------------------------------------------------------------
bool PhysXNifViewer::UpdateStats_Callback(NiRenderClick*, void* pvThis)
{
    PhysXNifViewer* pkThis = (PhysXNifViewer*)pvThis;

    pkThis->m_pkStats->Update();

    return true;
}
//---------------------------------------------------------------------------
void PhysXNifViewer::AddStateName(NiFixedString& kName)
{
    // Look for it already in the array
    for (NiUInt32 ui = 0; ui < m_kStateNames.GetSize(); ui++)
    {
        if (m_kStateNames.GetAt(ui) == kName)
            return;
    }

    m_kStateNames.Add(kName);
}
//---------------------------------------------------------------------------

