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

#include "MaxImmerse.h"
#include "MiDlgProcRender.h"
#include <NiViewerPluginInfo.h>
#include <NiScriptInfoDialogs.h>
//---------------------------------------------------------------------------
void SpinnerOff(HWND hWnd, int iSpinNum)
{
    ISpinnerControl* spin2 = GetISpinner(GetDlgItem(hWnd, iSpinNum));
    spin2->Disable();
    ReleaseISpinner(spin2);
}
//---------------------------------------------------------------------------
void SpinnerOn(HWND hWnd, int iSpinNum)
{ 
    ISpinnerControl* spin2 = GetISpinner(GetDlgItem(hWnd, iSpinNum));
    spin2->Enable();
    ReleaseISpinner(spin2);
}

//---------------------------------------------------------------------------
void MiDlgProcRenderer::SetStripifyState(HWND hWnd)
{
    bool b; 
        NiMAXOptions::GetInt(NI_VIEWER_TYPE);
    b = NiMAXOptions::GetBool(NI_VIEWER_STRIPIFY);

    EnableWindow(GetDlgItem(hWnd, IDC_STRIPIFYCHECK), TRUE);
    SendMessage(GetDlgItem(hWnd, IDC_STRIPIFYCHECK), BM_SETCHECK, 
        b, 0);
}
//---------------------------------------------------------------------------
bool MiDlgProcRenderer::IsWindowsNT()
{
    OSVERSIONINFO info;

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&info);

    // NT version 5 is Windows 2000
    return(info.dwPlatformId == VER_PLATFORM_WIN32_NT &&
           info.dwMajorVersion < 5);
};
//---------------------------------------------------------------------------
BOOL MiDlgProcRenderer::DlgProc(TimeValue, IParamMap*,
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM) 
{
    CHECK_MEMORY();
    MiViewerPlugin::ViewerType eType = (MiViewerPlugin::ViewerType) 
        NiMAXOptions::GetInt(NI_VIEWER_TYPE);

    switch (msg) 
    {
    case WM_INITDIALOG:
        NiMAXOptions::ReadOptions(GetCOREInterface());
        
        if (!NiMAXOptions::IsAnimationViewerAvailable() &&
            eType == MiViewerPlugin::MAX_ANIMATION_VIEWER)
        {
            eType = MiViewerPlugin::MAX_DX9_VIEWER;            
        }

        {
            bool bDX9Avail = NiMAXOptions::IsDX9ViewerAvailable();
            bool bD3D10Avail = NiMAXOptions::IsD3D10ViewerAvailable();
            bool bSIAvail = NiMAXOptions::IsSceneImmerseViewerAvailable();
            bool bCustomAvail = NiMAXOptions::IsCustomViewerAvailable();
            bool bAnimationAvail = NiMAXOptions::IsAnimationViewerAvailable();
            bool bXenonAvail = NiMAXOptions::IsXenonViewerAvailable();
            bool bPS3Avail = NiMAXOptions::IsPS3ViewerAvailable();
            bool bWiiAvail = NiMAXOptions::IsWiiViewerAvailable();
            bool bPhysXAvail = NiMAXOptions::IsPhysXViewerAvailable();

            EnableWindow(GetDlgItem(hWnd, IDC_DX9_VIEWER), bDX9Avail);
            EnableWindow(GetDlgItem(hWnd, IDC_D3D10_VIEWER), bD3D10Avail);
            EnableWindow(GetDlgItem(hWnd, IDC_SCENEIMMERSE_VIEWER), 
                bSIAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_CUSTOM_VIEWER), bCustomAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_CUSTOM_RENDERER_BUTTON), TRUE);
            EnableWindow(GetDlgItem(hWnd, IDC_ANIMATION_VIEWER), 
                bAnimationAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_XENON_VIEWER), bXenonAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_PS3_VIEWER), bPS3Avail);
            EnableWindow(GetDlgItem(hWnd, IDC_WII_VIEWER), bWiiAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_PHYSX_VIEWER), bPhysXAvail);


            if (bCustomAvail)
            {
                NiString strName = 
                    NiMAXOptions::GetValue(NI_CUSTOM_VIEWER_SCRIPT_NAME);
                if (strName.Length() > 7)
                    strName = strName.Left(7) + "...";
                strName = "Custom View: " + strName;
                SendDlgItemMessage(hWnd, IDC_CUSTOM_RENDERER_BUTTON,
                    WM_SETTEXT, 0, (LPARAM) (const char*) strName);
            }
            else
            {
                SendDlgItemMessage(hWnd, IDC_CUSTOM_RENDERER_BUTTON, 
                    WM_SETTEXT, 0, 
                    (LPARAM) (const char*) "Custom View: None");
            }

            EnableWindow(GetDlgItem(hWnd, IDC_DX9_VIEWER_OPTIONS),
                bDX9Avail);
            EnableWindow(GetDlgItem(hWnd, IDC_D3D10_VIEWER_OPTIONS),
                bD3D10Avail);
            EnableWindow(GetDlgItem(hWnd, IDC_SCENEIMMERSE_VIEWER_OPTIONS), 
                bSIAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_CUSTOM_VIEWER_OPTIONS), 
                bCustomAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_ANIMATION_VIEWER_OPTIONS), 
                bAnimationAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_XENON_VIEWER_OPTIONS), 
                bXenonAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_PS3_VIEWER_OPTIONS), bPS3Avail);
            EnableWindow(GetDlgItem(hWnd, IDC_WII_VIEWER_OPTIONS), bWiiAvail);
            EnableWindow(GetDlgItem(hWnd, IDC_PHYSX_VIEWER_OPTIONS), 
                bPhysXAvail);

            if (!bDX9Avail &&!bSIAvail && !bCustomAvail && !bAnimationAvail &&
                !bXenonAvail && !bPS3Avail && !bWiiAvail && !bD3D10Avail && !bPhysXAvail)
            {
                SendMessage(hWnd, WM_HIDEVIEW, 0 ,0);
            }
        }

        SetStripifyState(hWnd);

        if (eType != MiViewerPlugin::MAX_DX9_VIEWER && eType != MiViewerPlugin::MAX_D3D10_VIEWER)
        {
            // disable irrelevant options 
            SpinnerOff(hWnd, IDC_SP_WINDOW_X_SPIN);
            SpinnerOff(hWnd, IDC_SP_WINDOW_Y_SPIN);
            EnableWindow(GetDlgItem(hWnd, IDC_WINDOWED), FALSE);
        }

        m_pUtil->m_pRendererPMap->Invalidate();
        break;
    case WM_HIDEVIEW:
        {
            if(m_pUtil->ms_bViewButtonVisible)
                EnableWindow(GetDlgItem(hWnd, ID_VIEW), TRUE);
            else
                EnableWindow(GetDlgItem(hWnd, ID_VIEW), FALSE);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
        {
        case ID_VIEW:
            if(m_pUtil->ms_bViewButtonVisible && BN_CLICKED == HIWORD(wParam))
            {
                m_pUtil->View();
            }
            else
            {
                NiMessageBox("Only one Gamebryo renderer can be active"
                    " at once.\nPlease close the viewport renderer.",
                    "Viewport Is Already Open");
            }
            break;
        case IDC_DX9_VIEWER:
        case IDC_D3D10_VIEWER:
            SpinnerOn(hWnd, IDC_SP_WINDOW_X_SPIN);
            SpinnerOn(hWnd, IDC_SP_WINDOW_Y_SPIN);
            EnableWindow(GetDlgItem(hWnd, IDC_WINDOWED), TRUE);
            SetStripifyState(hWnd);
            m_pUtil->m_pRendererPMap->Invalidate();
            break;
        case IDC_SCENEIMMERSE_VIEWER:
        case IDC_CUSTOM_VIEWER:
        case IDC_ANIMATION_VIEWER:
        case IDC_XENON_VIEWER:
        case IDC_PS3_VIEWER:
        case IDC_WII_VIEWER:
        case IDC_PHYSX_VIEWER:
            SpinnerOff(hWnd, IDC_SP_WINDOW_X_SPIN);
            SpinnerOff(hWnd, IDC_SP_WINDOW_Y_SPIN);
            EnableWindow(GetDlgItem(hWnd, IDC_WINDOWED), FALSE);
            SetStripifyState(hWnd);
            m_pUtil->m_pRendererPMap->Invalidate();
            break;
        case IDC_ANIMATION_VIEWER_OPTIONS:
        case IDC_DX9_VIEWER_OPTIONS:
        case IDC_D3D10_VIEWER_OPTIONS:
        case IDC_SCENEIMMERSE_VIEWER_OPTIONS:
        case IDC_CUSTOM_VIEWER_OPTIONS:
        case IDC_XENON_VIEWER_OPTIONS:
        case IDC_PS3_VIEWER_OPTIONS:
        case IDC_WII_VIEWER_OPTIONS:
        case IDC_PHYSX_VIEWER_OPTIONS:
            DoOptionsDialog(LOWORD(wParam), hWnd);
            break;
        case IDC_SP_WINDOW_X:
        case IDC_SP_WINDOW_Y:
        case IDC_WINDOWED:
            m_pUtil->m_pRendererPMap->Invalidate();
            break;
        case IDC_STRIPIFYCHECK:
            SetStripifyState(hWnd);
            m_pUtil->m_pRendererPMap->Invalidate();
            break;
        case IDC_CUSTOM_RENDERER_BUTTON:
            {
                if (BN_CLICKED == HIWORD(wParam))
                {
                    CHECK_MEMORY();
                    NiMAXOptions::g_kOptionsCS.Lock();

                    NiMAXOptions::ReadOptions(GetCOREInterface());

                    NiScriptTemplateManager::GetInstance();
                    NiScriptInfoPtr spScript = 
                        NiScriptInfoDialogs::DoManagementDialog(
                        NiMAXOptions::GetCustomViewerScript(), 
                        NULL, hWnd, "VIEWER");

                    if (spScript)
                    {
                        NiMAXOptions::SetCustomViewerScript(spScript);
                        NiString strName = spScript->GetName();
                        if (strName.Length() > 7)
                            strName = strName.Left(7) + "...";

                        strName = "Custom View: " + strName;
                        SendDlgItemMessage(hWnd, 
                            IDC_CUSTOM_RENDERER_BUTTON, WM_SETTEXT,
                            0, (LPARAM) (const char*) strName);
                        EnableWindow(GetDlgItem(hWnd, IDC_CUSTOM_VIEWER), 
                            TRUE);
                        EnableWindow(GetDlgItem(hWnd, 
                            IDC_CUSTOM_RENDERER_BUTTON), TRUE);
                    }
                    else
                    {
                        SendDlgItemMessage(hWnd, IDC_CUSTOM_RENDERER_BUTTON,
                            WM_SETTEXT, 0, 
                            (LPARAM) (const char*) "Custom View: None");
                        EnableWindow(GetDlgItem(hWnd, 
                            IDC_CUSTOM_VIEWER_OPTIONS), FALSE);
                        EnableWindow(GetDlgItem(hWnd, 
                            IDC_CUSTOM_VIEWER), FALSE);
                    }
                    NiMAXOptions::WriteOptions(GetCOREInterface());
                    NiMAXOptions::g_kOptionsCS.Unlock();

                }
            }
            break;
        }   
        break;
    default:
        return FALSE;
    }
    CHECK_MEMORY();
    return FALSE; 
}

//---------------------------------------------------------------------------
void MiDlgProcRenderer::DoOptionsDialog(UINT uiWhichOptions, HWND hwnd)
{
    CHECK_MEMORY();
    NiScriptInfoPtr spScript = NULL;
    NiViewerPluginInfoPtr spPluginInfo = NULL;
    NiPluginInfo* spTemp = NULL;
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    
    switch (uiWhichOptions)
    {
        case IDC_DX9_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetDX9ViewerScript();
            break;
        case IDC_D3D10_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetD3D10ViewerScript();
            break;
        case IDC_SCENEIMMERSE_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetSceneImmerseViewerScript();
            break;
        case IDC_ANIMATION_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetAnimationViewerScript();
            break;
        case IDC_XENON_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetXenonViewerScript();
            break;
        case IDC_PS3_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetPS3ViewerScript();
            break;
        case IDC_WII_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetWiiViewerScript();
            break;
        case IDC_PHYSX_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetPhysXViewerScript();
            break;
        case IDC_CUSTOM_VIEWER_OPTIONS:
            spScript = NiMAXOptions::GetCustomViewerScript();
            break;
    }

    if (spScript)
    {
        for (unsigned int ui = 0; spPluginInfo == NULL && 
            ui < spScript->GetPluginInfoCount(); ui++)
        {
            spTemp = spScript->GetPluginInfoAt(ui);
            if (spTemp && NiIsKindOf(NiViewerPluginInfo, spTemp) &&
                pkManager->HasManagementDialog(spTemp))
                spPluginInfo = (NiViewerPluginInfo*) spTemp;
        }
    }

    if (spPluginInfo && pkManager->HasManagementDialog(spPluginInfo))
    {
        if (pkManager->DoManagementDialog(spPluginInfo, hwnd))
        {
            NiScriptTemplateManager* pkScriptManager = 
                NiScriptTemplateManager::GetInstance();

            pkScriptManager->ReplaceScript(spScript);
            if (spScript->GetFile() != NULL)
            {
                pkScriptManager->SaveScript((const char*)spScript->GetFile(),
                    spScript);
            }

            switch (uiWhichOptions)
            {
                case IDC_DX9_VIEWER_OPTIONS:
                    NiMAXOptions::SetDX9ViewerScript(spScript);
                    break;
                case IDC_D3D10_VIEWER_OPTIONS:
                    NiMAXOptions::SetD3D10ViewerScript(spScript);
                    break;
                case IDC_SCENEIMMERSE_VIEWER_OPTIONS:
                    NiMAXOptions::SetSceneImmerseViewerScript(spScript);
                    break;
                case IDC_ANIMATION_VIEWER_OPTIONS:
                    NiMAXOptions::SetAnimationViewerScript(spScript);
                    break;
                case IDC_CUSTOM_VIEWER_OPTIONS:
                    NiMAXOptions::SetCustomViewerScript(spScript);
                    break;
                case IDC_XENON_VIEWER_OPTIONS:
                    NiMAXOptions::SetXenonViewerScript(spScript);
                    break;
                case IDC_PS3_VIEWER_OPTIONS:
                    NiMAXOptions::SetPS3ViewerScript(spScript);
                    break;
                case IDC_WII_VIEWER_OPTIONS:
                    NiMAXOptions::SetWiiViewerScript(spScript);
                    break;
                case IDC_PHYSX_VIEWER_OPTIONS:
                    NiMAXOptions::SetPhysXViewerScript(spScript);
                    break;
            }
            NiMAXOptions::WriteOptions(GetCOREInterface());
        }
    }
    else
    {
        NiMessageBox("This viewer does not have any options.",
            "Viewer Options");
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
