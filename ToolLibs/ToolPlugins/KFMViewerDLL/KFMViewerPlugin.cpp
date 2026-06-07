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
#include "KFMViewerPlugin.h"
#include "KFMViewerDialog.h"
#include "NiKFMTool.h"

NiImplementRTTI(KFMViewerPlugin, NiViewerPlugin);

//---------------------------------------------------------------------------
KFMViewerPlugin::KFMViewerPlugin() :
    NiViewerPlugin("KFM Viewer", "1.1",
        "View KFM files using default viewer.",
        "View KFM files using default viewer.")
{
    CaptureKFMViewerPath();
}
//---------------------------------------------------------------------------
NiPluginInfo* KFMViewerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiViewerPluginInfo(
        NiSystemDesc::RENDERER_DX9);
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName(PARAM_PLUGINCLASSNAME);
    pkPluginInfo->SetType("Viewer");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool KFMViewerPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool KFMViewerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    KFMViewerDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch (kDialog.DoModal())
    {
        case IDOK:
            // Copy over the results.
            pkInitialInfo->RemoveAllParameters();
            kDialog.GetResults()->Clone(pkInitialInfo);

            // Convert script to latest version.
            pkInitialInfo->SetName(m_strName);
            pkInitialInfo->SetVersion(m_strVersion);
            pkInitialInfo->SetClassName(PARAM_PLUGINCLASSNAME);

            break;
        case IDCANCEL:
            return false;
        default:
            NIASSERT(false);
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool KFMViewerPlugin::CanView(NiViewerPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() != PARAM_PLUGINCLASSNAME ||
        !VerifyVersionString(pkInfo->GetVersion()))
    {
        return false;
    }

    if (m_strKFMViewerPath.IsEmpty())
    {
        CaptureKFMViewerPath();
        if (m_strKFMViewerPath.IsEmpty())
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr KFMViewerPlugin::View(const NiViewerPluginInfo* pkInfo)
{
    // Create the KFM export script.
    NiString strDirectory = m_strAbsolutePluginPath + "..\\..\\Data\\";
    NiString strFile = "KFMVIEWERTEMP";
    NiString strExtension = ".NIF";
    remove(strDirectory + strFile + ".KFM");
    NiScriptInfoPtr spKFMScriptInfo = NiNew NiScriptInfo;
    NiExportPluginInfo* pkPluginInfo = NiNew NiExportPluginInfo(strDirectory,
        strFile, strExtension);
    pkPluginInfo->SetName("Controller Extractor");
    pkPluginInfo->SetVersion("2.3");
    pkPluginInfo->SetClassName("NiControllerExtractorPlugin");
    pkPluginInfo->SetType("EXPORT");

    pkPluginInfo->SetBool("SingleKF", true);
    pkPluginInfo->SetBool("ExportedFileName", true);
    pkPluginInfo->SetBool("CharacterName", true);
    pkPluginInfo->SetBool("UpperCase", true);
    pkPluginInfo->AddParameter("Separator", "_");
    pkPluginInfo->SetBool("CreateKFM", true);
    pkPluginInfo->SetFloat("TransDuration", 0.1f);
    pkPluginInfo->SetBool("CharacterNameInKFMFile", true);
    pkPluginInfo->SetBool("StoreControllers", false);
    pkPluginInfo->SetInt("SyncTransType", NiKFMTool::TYPE_MORPH);
    pkPluginInfo->SetInt("NonSyncTransType", NiKFMTool::TYPE_CROSSFADE);
    pkPluginInfo->SetFloat("SyncTransDuration", 0.1f);
    pkPluginInfo->SetFloat("NonSyncTransDuration", 0.1f);
    pkPluginInfo->SetBool("CompressionAlways", false);
    pkPluginInfo->AddParameter(
        "ExcludedControllerTypes", "NiPSResetOnLoopCtlr;");
    
    spKFMScriptInfo->AddPluginInfo(pkPluginInfo);
    pkPluginInfo = NiNew NiExportPluginInfo(strDirectory, strFile,
        strExtension);
    pkPluginInfo->SetName("NIF Exporter");
    pkPluginInfo->SetVersion("1.0");
    pkPluginInfo->SetClassName("NifExportPlugin");
    pkPluginInfo->SetType("EXPORT");
    spKFMScriptInfo->AddPluginInfo(pkPluginInfo);
    
    // Run the KFM export script.
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    NIASSERT(pkManager);
    NiBatchExecutionResultPtr spBatchExecutionResult = pkManager
        ->ExecuteScript(spKFMScriptInfo, false);
    if (!spBatchExecutionResult->WasSuccessful())
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Determine renderer type.
    NiSystemDesc::RendererID eRendererType;
    if (CompareVersions(pkInfo->GetVersion(), "1.1"))
    {
        // Handle old plug-in versions.
        eRendererType = NiSystemDesc::RENDERER_DX9;
    }
    else
    {
        eRendererType = pkInfo->GetRenderer();
    }

    // Run the registered KFM application.
    NiString strKFMPath = strDirectory + strFile + ".KFM";
    if (!RunKFMViewer(strKFMPath, eRendererType))
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool KFMViewerPlugin::SupportsRenderer(NiSystemDesc::RendererID eRenderer)
{
    if (eRenderer == NiSystemDesc::RENDERER_DX9 || 
        eRenderer == NiSystemDesc::RENDERER_D3D10)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void KFMViewerPlugin::CaptureKFMViewerPath()
{
    NiRegistry* pkRegEntry = NiRegistry::Create("HKEY_CLASSES_ROOT\\KFM."
        "Document\\shell\\open\\command");

    if (pkRegEntry)
    {
         pkRegEntry->QueryString("", m_strKFMViewerPath);
         NiDelete pkRegEntry;
    }
}
//---------------------------------------------------------------------------
bool KFMViewerPlugin::RunKFMViewer(NiString strKFMPath,
    NiSystemDesc::RendererID eRendererType)
{
    if (m_strKFMViewerPath.IsEmpty() || strKFMPath.IsEmpty())
    {
        return false;
    }

    // Remove other temporary files.
    NiString strTempFilePath = strKFMPath;
    unsigned int uiIdx = strTempFilePath.FindReverse('.');
    strTempFilePath.SetAt(uiIdx, '\0');
    remove(strTempFilePath + ".set");
    remove(strTempFilePath + ".map");
    remove(strTempFilePath + "_Anim.h");

    // Execute the KFM viewer application.
    NiString strArgs = m_strKFMViewerPath;
    strArgs.Replace("%1", strKFMPath);
    switch (eRendererType)
    {
        case NiSystemDesc::RENDERER_GENERIC:
        case NiSystemDesc::RENDERER_DX9:
            // No further action required.
            break;
        case NiSystemDesc::RENDERER_D3D10:
            // Add D3D10 command-line argument.
            strArgs += " -d3d10";
            break;
        default:
            // Unknown renderer type.
            NiMessageBox("Unknown renderer type specified in script.",
                m_strName + " Error");
            return false;
    }
    UINT uiRC = WinExec(strArgs, SW_SHOWNORMAL);

    // Handle execution errors.
    if (uiRC == ERROR_BAD_FORMAT)
    {
        NiMessageBox("Invalid KFM viewer executable.", m_strName + " Error");
        return false;
    }
    else if (uiRC == 0)
    {
        NiMessageBox("Not enough system resources available\nto run KFM "
            "viewer executable.", m_strName + " Error");
        return false;
    }
    else if (uiRC == ERROR_FILE_NOT_FOUND || uiRC == ERROR_PATH_NOT_FOUND)
    {
        NiMessageBox("Unable to find KFM viewer executable.", m_strName +
            " Error");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
