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
#include "PhysXViewerPlugin.h"
#include "PhysXViewerDefines.h"
#include "PhysXViewerDialog.h"

NiImplementRTTI(PhysXViewerPlugin, NiViewerPlugin);

//---------------------------------------------------------------------------
PhysXViewerPlugin::PhysXViewerPlugin() :
    NiViewerPlugin("PhysX Viewer", "1.1",
        "View PhysX files using default viewer.",
        "View PhysX files using default viewer.")
{
    CapturePhysXViewerPath();
}
//---------------------------------------------------------------------------
NiPluginInfo* PhysXViewerPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiViewerPluginInfo(
        NiSystemDesc::RENDERER_DX9);
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName(PARAM_PLUGINCLASSNAME);
    pkPluginInfo->SetType("Viewer");

    pkPluginInfo->SetBool(OPT_DEBUGOUTPUT, false);
    pkPluginInfo->SetBool(OPT_GROUNDPLANE, false);
    pkPluginInfo->SetFloat(OPT_GROUNDHEIGHT, 0.0f);
    pkPluginInfo->SetFloat(OPT_GRAVITY_X, 0.0f);
    pkPluginInfo->SetFloat(OPT_GRAVITY_Y, 0.0f);
    pkPluginInfo->SetFloat(OPT_GRAVITY_Z, -9.8f);
    pkPluginInfo->SetBool(OPT_FULLSCREEN, false);
    pkPluginInfo->SetInt(OPT_RESOLUTIONWIDTH, 640);
    pkPluginInfo->SetInt(OPT_RESOLUTIONHEIGHT, 480);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool PhysXViewerPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool PhysXViewerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    PhysXViewerDialog kDialog(m_hModule, hWndParent, pkInitialInfo);
    if (kDialog.DoModal() == IDOK)
    {
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);

        // Convert script to latest version.
        pkInitialInfo->SetName(m_strName);
        pkInitialInfo->SetVersion(m_strVersion);
        pkInitialInfo->SetClassName(PARAM_PLUGINCLASSNAME);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool PhysXViewerPlugin::CanView(NiViewerPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() != PARAM_PLUGINCLASSNAME ||
        !VerifyVersionString(pkInfo->GetVersion()))
    {
        return false;
    }

    if (m_strPhysXViewerPath.IsEmpty())
    {
        CapturePhysXViewerPath();
        if (m_strPhysXViewerPath.IsEmpty())
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr PhysXViewerPlugin::View(const NiViewerPluginInfo* pkInfo)
{
    // Create the PhysX export script.
    NiString strDirectory = m_strAbsolutePluginPath + "..\\..\\Data\\";
    NiString strFile = "PHYSXVIEWERTEMP";
    NiString strExtension = ".NIF";
    remove(strDirectory + strFile + strExtension);
    NiScriptInfo* pkPhysXScriptInfo = NiNew NiScriptInfo;
    NiExportPluginInfo* pkPluginInfo =
        NiNew NiExportPluginInfo(strDirectory, strFile, strExtension);
    pkPluginInfo->SetName("NIF Exporter");
    pkPluginInfo->SetVersion("1.2");
    pkPluginInfo->SetClassName("NifExportPlugin");
    pkPluginInfo->SetType("EXPORT");
    pkPhysXScriptInfo->AddPluginInfo(pkPluginInfo);
    
    // Run the PhysX export script.
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    NIASSERT(pkManager);
    NiBatchExecutionResultPtr spBatchExecutionResult = pkManager
        ->ExecuteScript(pkPhysXScriptInfo, false);
    NiDelete pkPhysXScriptInfo;

    if (!spBatchExecutionResult->WasSuccessful())
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Check that we have some PhysX content
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    // Get the PhysX shared data.
    NiPhysXSharedData* pkPhysXSharedData = (NiPhysXSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiPhysXSharedData));

    if (!pkPhysXSharedData)
    {
        NiMessageBox(
            "The PhysX Viewer plug-in could not find any PhysX content.\n"
            "Did you forget to run the PhysX Extractor process plug-in?\n",
            m_strName + " Error: No PhysX content");
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    if (!pkPhysXSharedData->GetPropCount())
    {
        NiMessageBox(
            "The PhysX Viewer plug-in could not find any PhysX content.\n"
            "Did you forget to run the PhysX Extractor process plug-in?\n",
            m_strName + " Error: No PhysX content");
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

    // Run the PhysXNifViewer application.
    NiString strPhysXPath = strDirectory + strFile + strExtension;
    if (!RunPhysXViewer(strPhysXPath, pkInfo, eRendererType))
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool PhysXViewerPlugin::SupportsRenderer(NiSystemDesc::RendererID eRenderer)
{
    if (eRenderer == NiSystemDesc::RENDERER_DX9 || 
        eRenderer == NiSystemDesc::RENDERER_D3D10)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void PhysXViewerPlugin::CapturePhysXViewerPath()
{
    char pcNDLPath[1024];
    DWORD iCharsRead = GetEnvironmentVariable("EGB_PATH", pcNDLPath, 1024);

    if (iCharsRead >= 1024)
    {
        m_strPhysXViewerPath.Empty();
        return;
    }
    
    m_strPhysXViewerPath = "\"";
    m_strPhysXViewerPath += pcNDLPath;

    m_strPhysXViewerPath +=
#if (_MSC_VER == 1500)  // VC 9.0 (Visual C++ 2008 compiler)
#ifdef NIDEBUG
       "\\Tools\\DeveloperTools\\PhysXNifViewer\\Win32\\VC90\\"
       "PhysXNifViewerD.exe\" \"%1\"";
#else
       "\\Tools\\DeveloperTools\\PhysXNifViewer\\Win32\\VC90\\"
       "PhysXNifViewer.exe\" \"%1\"";
#endif
#else   // VC 8.0 (Visual C++ 2005 compiler)
#ifdef NIDEBUG
       "\\Tools\\DeveloperTools\\PhysXNifViewer\\Win32\\VC80\\"
       "PhysXNifViewerD.exe\" \"%1\"";
#else
       "\\Tools\\DeveloperTools\\PhysXNifViewer\\Win32\\VC80\\"
       "PhysXNifViewer.exe\" \"%1\"";
#endif
#endif

}
//---------------------------------------------------------------------------
bool PhysXViewerPlugin::RunPhysXViewer(NiString strPhysXPath,
    const NiViewerPluginInfo* pkPluginInfo,
    NiSystemDesc::RendererID eRendererType)
{
    if (m_strPhysXViewerPath.IsEmpty() || strPhysXPath.IsEmpty())
    {
        return false;
    }

    // Build base command line.
    NiString strArgs = m_strPhysXViewerPath;
    strArgs.Replace("%1", strPhysXPath);

    // Add command-line options.
    if (pkPluginInfo->GetBool(OPT_DEBUGOUTPUT))
    {
        strArgs += " -debugGeom";
    }
    if (pkPluginInfo->GetBool(OPT_GROUNDPLANE))
    {
        strArgs += " -ground";

        NiString strGroundHeight;
        strGroundHeight.Format(" -height %f", pkPluginInfo->GetFloat(
            OPT_GROUNDHEIGHT));
        strArgs += strGroundHeight;
    }
    if (pkPluginInfo->GetBool(OPT_FULLSCREEN))
    {
        strArgs += " -full";
    }
    NiString strResolution;
    strResolution.Format(" -resolution %dx%d", pkPluginInfo->GetInt(
        OPT_RESOLUTIONWIDTH), pkPluginInfo->GetInt(OPT_RESOLUTIONHEIGHT));
    strArgs += strResolution;

    if (pkPluginInfo->GetParameterIndex(OPT_GRAVITY_X) != NIPT_INVALID_INDEX &&
        pkPluginInfo->GetParameterIndex(OPT_GRAVITY_Y) != NIPT_INVALID_INDEX &&
        pkPluginInfo->GetParameterIndex(OPT_GRAVITY_Z) != NIPT_INVALID_INDEX)
    {
        NiString strGravity;
        strGravity.Format(" -gravity %g,%g,%g",
            pkPluginInfo->GetFloat(OPT_GRAVITY_X),
            pkPluginInfo->GetFloat(OPT_GRAVITY_Y),
            pkPluginInfo->GetFloat(OPT_GRAVITY_Z));
        strArgs += strGravity;
    }

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

    // Execute the PhysX viewer application.
    UINT uiRC = WinExec(strArgs, SW_SHOWNORMAL);

    // Handle execution errors.
    if (uiRC == ERROR_BAD_FORMAT)
    {
        NiMessageBox("Invalid PhysX viewer executable.",
            m_strName + " Error");
        return false;
    }
    else if (uiRC == 0)
    {
        NiMessageBox("Not enough system resources available\nto run "
            "PhysX viewer executable.", m_strName + " Error");
        return false;
    }
    else if (uiRC == ERROR_FILE_NOT_FOUND || uiRC == ERROR_PATH_NOT_FOUND)
    {
        NiMessageBox("Unable to find PhysX viewer executable.", m_strName +
            " Error");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
