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
#include "NIFViewerPlugin.h"
#include "NIFViewerDialog.h"

NiImplementRTTI(NIFViewerPlugin, NiViewerPlugin);

//---------------------------------------------------------------------------
NIFViewerPlugin::NIFViewerPlugin() :
    NiViewerPlugin("NIF Viewer", "1.2",
    "Views NIF files using the Gamebryo Scene Viewer application.",
    "Views NIF files using the Gamebryo Scene Viewer application.")
{
    CaptureNIFViewerPath();
}
//---------------------------------------------------------------------------
NiPluginInfo* NIFViewerPlugin::GetDefaultPluginInfo()
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
bool NIFViewerPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NIFViewerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    NIFViewerDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

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
bool NIFViewerPlugin::CanView(NiViewerPluginInfo* pkInfo)
{
    if ((pkInfo->GetClassName() != "NiSceneImmerseViewerPlugin" &&
        pkInfo->GetClassName() != PARAM_PLUGINCLASSNAME) ||
        !VerifyVersionString(pkInfo->GetVersion()))
    {
        return false;
    }

    if (m_strNIFViewerPath.IsEmpty())
    {
        CaptureNIFViewerPath();
        if (m_strNIFViewerPath.IsEmpty())
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NIFViewerPlugin::View(const NiViewerPluginInfo* pkInfo)
{
    // Get the NiSceneGraphSharedData, creating it if it does not exist.
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiSceneGraphSharedData* pkSGSharedData = NULL;
    pkSGSharedData =(NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    
    NiStream stream;
    stream.InsertObject(pkSGSharedData->GetFullSceneGraph());

    NiString strFilePath = m_strAbsolutePluginPath + "..\\..\\Data\\"
        "NIFVIEWERTEMP.NIF";
    if (strFilePath.Length() == 0)
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);

    stream.Save((const char*) strFilePath);

    // Determine renderer type.
    NiSystemDesc::RendererID eRendererType;
    if (pkInfo->GetName() == "NiD3D10SceneImmerseViewer" )
    {
        // Handle D3D10.
        eRendererType = NiSystemDesc::RENDERER_D3D10;
    }
    else if (pkInfo->GetClassName() == "NiSceneImmerseViewerPlugin" ||
        CompareVersions(pkInfo->GetVersion(), "1.2"))
    {
        // Handle old plug-in versions.
        eRendererType = NiSystemDesc::RENDERER_DX9;
    }
    else
    {
        eRendererType = pkInfo->GetRenderer();
    }

    if (!RunNIFViewer(strFilePath, eRendererType))
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool NIFViewerPlugin::SupportsRenderer(NiSystemDesc::RendererID eRenderer)
{
    return (eRenderer == NiSystemDesc::RENDERER_DX9 || 
        eRenderer == NiSystemDesc::RENDERER_D3D10);
}
//---------------------------------------------------------------------------
bool NIFViewerPlugin::RunNIFViewer(NiString strFile,
    NiSystemDesc::RendererID eRendererType)
{
    if(m_strNIFViewerPath.IsEmpty())
    {
        NiMessageBox("Unable to find the Gamebryo Asset Viewer "
            "executable.", m_strName + " Error");
        return false;
    }

    //Create as local copy of the viewer path.
    const char* pcNIFViewerPath = m_strNIFViewerPath;
    NiString strArgs(pcNIFViewerPath);      
    strArgs.Replace("%1", strFile);           
    //remove the D3D10 flag it it got added.
    strArgs.Replace("-D3D10"," ");

    switch (eRendererType)
    {
        case NiSystemDesc::RENDERER_GENERIC:
        case NiSystemDesc::RENDERER_DX9:
            // No further action required.
            break;
        case NiSystemDesc::RENDERER_D3D10:
            // Add D3D10 command-line argument.
            strArgs += " -D3D10";
            break;
        default:
            // Unknown renderer type.
            NiMessageBox("Unknown renderer type specified in script.",
                m_strName + " Error");
            return false;
    }

    UINT uiReturn = WinExec(strArgs, SW_SHOWNORMAL);
    
    if (uiReturn == ERROR_BAD_FORMAT)
    {
        NiMessageBox("Invalid Gamebryo Asset Viewer executable.",
            m_strName + " Error");
        return false;
    }
    else if (uiReturn == 0)
    {
        NiMessageBox("Not enough resources to run the Gamebryo Asset "
            "Viewer executable.", m_strName + " Error");
        return false;
    }
    else if (uiReturn == ERROR_FILE_NOT_FOUND || 
        uiReturn == ERROR_PATH_NOT_FOUND)
    {
        NiMessageBox("Unable to find the Gamebryo Asset Viewer "
            "executable.", m_strName + " Error");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NIFViewerPlugin::CaptureNIFViewerPath()
{
    NiRegistry* pkRegEntry = NiRegistry::Create("HKEY_CLASSES_ROOT\\NIF."
        "Document\\shell\\open\\command");

    if (pkRegEntry)
    {
         pkRegEntry->QueryString("", m_strNIFViewerPath);
         NiDelete pkRegEntry;
    }
}
//---------------------------------------------------------------------------
