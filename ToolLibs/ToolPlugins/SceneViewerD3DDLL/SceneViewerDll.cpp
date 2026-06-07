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


#include "SceneViewerDll.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include "NifColorDialog.h"
#include "NiConfigurableUIMapLoader.h"
#include "NiPluginDLLInterface.h"
#include "NiConfigurableUIMapLoader.h"
#include "NiConfigurableUIMapWriter.h"
#include <NiVersion.h>

//---------------------------------------------------------------------------
LPCRITICAL_SECTION  pkCriticalSection = NULL;



//---------------------------------------------------------------------------
unsigned long _stdcall RunThread(void* pvInfo)
{
    EnterCriticalSection(pkCriticalSection);
    SceneViewerInfo* pkWindow = (SceneViewerInfo*)pvInfo;

    NiUIMap* pkUIMap = NULL;
    long lResult = 1;

    NiConfigurableUIMapLoader::Init();
    NiConfigurableUIMapWriter::Init();

    // create the application
    NiWin32SceneViewerWindowPtr spTheApp = 
        NiNew NiWin32SceneViewerWindow(pkWindow->m_pcWindowTitle,
        (int)pkWindow->m_kDimensions.x, (int)pkWindow->m_kDimensions.y);

    if (!spTheApp)
    {
        NiMessageBox("Unable to create application","Test");
        pkWindow->m_bAppIsDead = true;
        pkWindow->m_bWindowActive = false;
        LeaveCriticalSection(pkCriticalSection);
        return 1;
    }
 
    // Set reference
    NiWin32SceneViewerWindow::SetInstanceReference(pkWindow->m_hI);

    // Now, we are ready to initialize the window and the main loop.
    // register the window class
    if (pkWindow->m_hWnd == NULL)
    {
        spTheApp->SetWindowClassName("SceneViewer");
        WNDCLASS kWC;
        kWC.style         = CS_HREDRAW | CS_VREDRAW;
        kWC.lpfnWndProc   = NiWin32SceneViewerWindow::WinProc;
        kWC.cbClsExtra    = 0;
        kWC.cbWndExtra    = 0;
        kWC.hInstance     = pkWindow->m_hI;
        kWC.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        kWC.hCursor       = LoadCursor(NULL, IDC_ARROW);
        kWC.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        kWC.lpszClassName = NiWin32SceneViewerWindow::GetWindowClassName();

        // register menu (if requested)
        if (spTheApp->GetMenuID())
            kWC.lpszMenuName = MAKEINTRESOURCE(spTheApp->GetMenuID());
        else
            kWC.lpszMenuName = 0;

        RegisterClass(&kWC);
    }

    // create the application window
    spTheApp->CreateMainWindow(pkWindow->m_iWinMode, pkWindow->m_hWnd);

    // If a UIMap isn't specified directly, use the one defined by the filename
    if (pkWindow->m_pkUIMap == NULL && pkWindow->m_pcUIMapFilename != NULL)
    {
        NiConfigurableUIMapLoader::ShowDebugOutputStrings(false);
        pkWindow->m_pkUIMap = 
            NiConfigurableUIMapLoader::LoadConfigurableUIMapFromFile(
            pkWindow->m_pcUIMapFilename, spTheApp->GetKeyboard(), 
            spTheApp->GetMouse());
    }

    if (pkWindow->m_pkUIMap == NULL)
    {
        NiMessageBox("User Interface was not found! \nPlease check that"
            " there are .uim files present in the installation directory.",
            "Gamebryo Error");
        pkWindow->m_bAppIsDead = true;
        pkWindow->m_bWindowActive = false;
        LeaveCriticalSection(pkCriticalSection);
        return 1;
    }
    else
    {
        pkUIMap = pkWindow->m_pkUIMap;
    }
    
    NiSceneViewer::SetCameraLabelFontPath(pkWindow->m_pcDefaultFilePath);
    pkUIMap->Initialize();
    spTheApp->SetUI(pkUIMap);
    spTheApp->SetHardware(!pkWindow->m_bSoftware);
    spTheApp->SetFullscreen(pkWindow->m_bFullScreen);
    spTheApp->SetD3D10(pkWindow->m_bD3D10);
    if (pkWindow->m_spCameraNode != NULL)
        spTheApp->SetDefaultCamera(pkWindow->m_spCameraNode);

    if (spTheApp->Initialize(pkWindow->m_spScene, NULL))
    {
        pkWindow->m_bWindowActive = true;
        pkWindow->m_bAppIsDead = false;
        pkWindow->m_pkSceneViewerWindow = spTheApp;
        spTheApp->SetBackgroundColor(pkWindow->m_kBackgroundColor);
        spTheApp->SetFrameRateLimit(pkWindow->m_uiFrameRate);
        spTheApp->SetAnimationLooping(pkWindow->m_bLoopAnimations);
        spTheApp->SetAnimationLoopStartTime(
            pkWindow->m_fAnimationLoopStartTime);
        spTheApp->SetAnimationLoopEndTime(pkWindow->m_fAnimationLoopEndTime);
        LeaveCriticalSection(pkCriticalSection);
        spTheApp->PreMainLoop();
        while (pkWindow->m_bWindowActive && lResult)
            lResult = spTheApp->MainLoopTick();
        spTheApp->PostMainLoop();
    }
    else
    {
        LeaveCriticalSection(pkCriticalSection);
    }

    pkWindow->m_spScene->DeleteScene();
    pkWindow->m_spScene = NULL;

    pkWindow->m_spCameraNode = NULL;
    pkWindow->m_pkSceneViewerWindow = NULL;
    spTheApp = NULL;    
    pkWindow->m_pkUIMap = NULL;
    NiDelete pkUIMap;

    pkWindow->m_bWindowActive = false;
    pkWindow->m_bAppIsDead = true;

    NiConfigurableUIMapLoader::Cleanup();
    NiConfigurableUIMapWriter::Cleanup();

    NiSceneViewer::SetCameraLabelFontPath(NULL);

    return lResult;
};
//---------------------------------------------------------------------------
CSceneViewerDll::CSceneViewerDll()
{ 
    m_spWindow = NULL;
    //m_spWindow->bWindowActive = false;
    if (pkCriticalSection == NULL)
    {
        pkCriticalSection = NiExternalNew CRITICAL_SECTION;
        InitializeCriticalSection(pkCriticalSection);
    }
    m_pkTreeControl = NULL;
    hThread = NULL;

    m_hNiCgShaderLib = NULL;

    // Only attempt to load the Cg Shader Library if Cg is installed
    HMODULE hCgCheck = LoadLibrary("CgD3D9.dll");
    if (hCgCheck)
    {
        FreeLibrary(hCgCheck);

        // Load Cg Shader Library, if it exists
        const char* const pcCgShaderLibName = "NiCgShaderLib"
            NI_DLL_SUFFIX
            ".dll";

        m_hNiCgShaderLib = LoadLibrary(pcCgShaderLibName);
        if (m_hNiCgShaderLib)
        {
            NiGetCompilerVersionFunction pfnGetCompilerVersionFunc =
                (NiGetCompilerVersionFunction)GetProcAddress(m_hNiCgShaderLib, 
                "GetCompilerVersion");
            if (pfnGetCompilerVersionFunc)
            {
                unsigned int uiPluginVersion = pfnGetCompilerVersionFunc();
                if (uiPluginVersion != (_MSC_VER))
                {
                    FreeLibrary(m_hNiCgShaderLib);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
CSceneViewerDll::~CSceneViewerDll()
{
    if (m_hNiCgShaderLib)
        FreeLibrary(m_hNiCgShaderLib);

    Shutdown();
    if (pkCriticalSection != NULL)
    {
        DeleteCriticalSection(pkCriticalSection);
        NiExternalDelete pkCriticalSection;
        pkCriticalSection = NULL;
    }
    if (hThread != NULL)
    {
        CloseHandle(hThread);
        hThread = 0;
    }
    NiDelete m_pkTreeControl;
    m_pkTreeControl = 0;
    m_spWindow = NULL;
}
//---------------------------------------------------------------------------
void CSceneViewerDll::RunThreaded(SceneViewerInfo* pkWindow)
{
    if (pkWindow == NULL)
        return;

    if (m_spWindow != NULL && !m_spWindow->m_bAppIsDead)
    {
        m_spWindow->m_bWindowActive = false;
        while (!m_spWindow->m_bAppIsDead)
            Sleep(30);//Wait 
    }

    if (hThread != NULL)
        CloseHandle( hThread );
    
    DWORD dwThreadId; 
//    RunThread((void*)pkWindow);
    m_spWindow = pkWindow;

    hThread = CreateThread( 
        NULL,                        // no security attributes 
        0,                           // use default stack size  
        &RunThread,                  // thread function 
        (void*)pkWindow,             // argument to thread function 
        0,                           // use default creation flags 
        &dwThreadId);                // returns the thread identifier 

    LPSYSTEM_INFO pkSystemInfo = NiExternalNew SYSTEM_INFO;
    GetSystemInfo(pkSystemInfo);
    DWORD dwNumProcessors = pkSystemInfo->dwNumberOfProcessors;
    NiExternalDelete pkSystemInfo;
    DWORD dwThreadAffinityMask;

    if (dwNumProcessors == 1)
        dwThreadAffinityMask = 1;
    else
        dwThreadAffinityMask = 2;

    // Check the return value for success. 
    if (hThread == NULL) 
        NiOutputDebugString( "CreateThread failed." ); 
    else
        SetThreadAffinityMask(hThread, dwThreadAffinityMask);

    // Wait until we are done viewing the object.
    WaitForSingleObject(hThread, INFINITE);
}
//---------------------------------------------------------------------------
void CSceneViewerDll::Run(SceneViewerInfo* pkWindow)
{
    if (pkWindow == NULL)
        return;

    if (m_spWindow!= NULL && m_spWindow->m_bWindowActive)
    {
        m_spWindow->m_bWindowActive = false;
        while (!m_spWindow->m_bAppIsDead)
            Sleep(30);//Wait 
    }

    m_spWindow = pkWindow;
    RunThread(pkWindow);
}
//---------------------------------------------------------------------------
bool CSceneViewerDll::IsRunning()
{
    if (m_spWindow != NULL)
        return m_spWindow->m_bWindowActive;
    else
        return false;
}
//---------------------------------------------------------------------------
void CSceneViewerDll::Shutdown()
{
    if (m_spWindow != NULL && !m_spWindow->m_bAppIsDead)
    {
        m_spWindow->m_bWindowActive = false;
        while (!m_spWindow->m_bAppIsDead)
            Sleep(30);//Wait 
    
        CloseHandle( hThread );
        NiExternalDelete hThread;
    }
}
//---------------------------------------------------------------------------
char* CSceneViewerDll::RunConfigurableUIMapEditor(char* pcExecPath,
    char* pcParentPath, char* pcStartFile)
{
    if (pcExecPath == NULL)
    {
        MessageBox(NULL, "Unable to find the Configurable UI Map Editor "
            "executable", "Error!", MB_OK | MB_ICONEXCLAMATION);
    }
    char* pcArgs = NiAlloc(char, 1028);
    char* pcTempCallbackFile = NiAlloc(char, 1024);
    NiSprintf(pcTempCallbackFile, 1024, "%s%s", pcParentPath,
        "TempUIMapCallback.txt");
    NiSprintf(pcArgs, 1028, "%s", pcExecPath);
    NiSprintf(pcArgs, 1028, "%s %s", pcArgs, pcTempCallbackFile);
    if (pcStartFile != NULL)
        NiSprintf(pcArgs, 1028, "%s %s", pcArgs, pcStartFile);

    float fTimeBefore = NiGetCurrentTimeInSec();
    _spawnl(_P_WAIT, pcExecPath, pcArgs, NULL );
    float fTimeAfter = NiGetCurrentTimeInSec();

    if (fTimeAfter - fTimeBefore < 1)
    {
        MessageBox(NULL, "Unable to find the Configurable UI Map Editor "
            "executable",
                   "Error!" ,MB_OK | MB_ICONEXCLAMATION);
        NiFree(pcArgs);
        NiFree(pcTempCallbackFile);
        return NULL;
    }

    char* pcFilename = NiAlloc(char, 1024);
    NiFile* kStream = NiFile::GetFile(pcTempCallbackFile, NiFile::READ_ONLY, 
        1024);
    kStream->GetLine(pcFilename, 1024);
    NiDelete kStream;

    _unlink(pcTempCallbackFile);

    NiFree(pcArgs);
    NiFree(pcTempCallbackFile);

    if (pcFilename == NULL)
        return NULL;
    if (strcmp(pcFilename, "CANCELLED") == 0)
        return NULL;
    return pcFilename;
}
//---------------------------------------------------------------------------
void CSceneViewerDll::RunSceneGraphTreeViewer(NiInstanceRef kInstanceRef,
    NiWindowRef kWindowRef, NiNode* pkScene, char* pcTitle, int iXPos, 
    int iYPos, int iWidth, int iHeight)
{
    NiDelete m_pkTreeControl;

    m_pkTreeControl = NiNew NiTreeCtrl(kInstanceRef, kWindowRef, pkScene, 
        pcTitle, iXPos, iYPos, iWidth, iHeight);
}
//---------------------------------------------------------------------------
bool CSceneViewerDll::RunColorSelector(NiWindowRef kWindowRef, 
    NiColor& kColor)
{
    return NifColorDialog::GetColor(kWindowRef, kColor);
}
//---------------------------------------------------------------------------
SceneViewerInfo::SceneViewerInfo()
{
    m_hI = 0;
    m_hWnd = NULL;
    m_iWinMode = SW_SHOWNORMAL;
    m_bWindowActive = false;
    m_spScene = NULL;
    m_pkUIMap = NULL;
    m_kDimensions = NiPoint2(640, 480);
    m_kLocation = NiPoint2(0,0);
    m_bStencilBuffer = false;
    m_bFullScreen = false;
    m_bSoftware = false;
    m_bD3D10 = false;
    m_kBackgroundColor = NiColor::BLACK;
    m_bAppIsDead = false;
    m_pkSceneViewerWindow = NULL;
    m_pcDefaultFilePath = NULL;
    m_pcUIMapFilename = NULL;
    m_pkSceneStream = NULL;
    m_pcWindowTitle = "SceneViewer";
    m_uiFrameRate = 60;
    m_bLoopAnimations = false;
    m_fAnimationLoopEndTime = 0;
    m_fAnimationLoopStartTime = 0; 
    m_spCameraNode = NULL;
}
//---------------------------------------------------------------------------
SceneViewerInfo::~SceneViewerInfo()
{
    m_spScene = NULL;
    NiDelete m_pkUIMap;
    NiFree(m_pcUIMapFilename);
    NiFree(m_pcDefaultFilePath);
    NiDelete m_pkSceneStream;
    NiDelete m_pkSceneViewerWindow;
    NiFree(m_pcWindowTitle);
}
//---------------------------------------------------------------------------

