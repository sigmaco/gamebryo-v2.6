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

#include "NiSceneViewerD3DPlugin.h"
#include <NiPluginDLLInterface.h>
#include <NiSceneGraphSharedData.h>
#include <NiRendererSharedData.h>
#include <NiTimerSharedData.h>
#include <NiViewerPluginInfo.h>
#include <NiSharedDataList.h>
#include "SceneViewerDll.h"
#include "NiSceneViewerConfigDialog.h"
#include "ScriptParameterDefinitions.h"

NiImplementRTTI(NiSceneViewerD3DPlugin, NiViewerPlugin);

void PrintOutSceneGraph(NiNode* pkScene)
{
    NiNode* pkTraverserNode = pkScene;
    if (pkTraverserNode != NULL)
    {
        const char* pcName = pkTraverserNode->GetName();
        if (pcName != NULL)
            NiOutputDebugString(pcName);
        else
            NiOutputDebugString("This node has no name.");
        
        if (pkTraverserNode->GetSelectiveUpdate())
            NiOutputDebugString(" SelectedUpdate ");

        if (pkTraverserNode->GetSelectiveUpdateTransforms())
            NiOutputDebugString(" SelectedUpdateTransforms ");

        if (pkTraverserNode->GetSelectiveUpdatePropertyControllers())
            NiOutputDebugString(" SelectedUpdatePC ");

        if (pkTraverserNode->GetSelectiveUpdateRigid())
            NiOutputDebugString(" SelectedUpdateRigid ");

        NiOutputDebugString("\n");

        for (unsigned int ui = 0; ui < pkTraverserNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkTraverserNode->GetAt(ui);
            if (NiIsKindOf(NiNode, pkChild))
                PrintOutSceneGraph((NiNode*)pkChild);
        }
    }
}

//---------------------------------------------------------------------------
NIPLUGINDLL_ENTRY void GetPlugins(NiPluginPtrSet& kPluginArray)
{
    kPluginArray.Add(NiNew NiSceneViewerD3DPlugin);
}
//---------------------------------------------------------------------------
NIPLUGINDLL_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
//---------------------------------------------------------------------------
NiSceneViewerD3DPlugin::NiSceneViewerD3DPlugin() :
    NiViewerPlugin("Scene Viewer D3D", "2.0",
    "Views the scene using the Gamebryo D3D viewer plugin.",
    "Views the scene using the Gamebryo D3D viewer plugin.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NiSceneViewerD3DPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiViewerPluginInfo(
        NiSystemDesc::RENDERER_DX9);
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName(PARAM_PLUGINNAME);
    pkPluginInfo->SetType("Viewer");

    pkPluginInfo->SetValue(PARAM_SHOWTREEVIEW, NiString::FromBool(false));
    pkPluginInfo->SetValue(PARAM_UIMFILE, ".\\InputMap.uim");

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiSceneViewerD3DPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewerD3DPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    // Build UIM path.
    char acUIMPath[NI_MAX_PATH];
    NiSprintf(acUIMPath, NI_MAX_PATH, "%s%s", (const char*)
        m_strAbsolutePluginPath, "..\\..\\Data\\");
    NiPath::RemoveDotDots(acUIMPath);

    // Create dialog class.
    NiSceneViewerConfigDialog kDialog(m_hModule, hWndParent, pkInitialInfo,
        NiString(acUIMPath), &m_kSceneViewerDll);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);

        // Convert script to latest version.
        pkInitialInfo->SetName(m_strName);
        pkInitialInfo->SetVersion(m_strVersion);
        pkInitialInfo->SetClassName(PARAM_PLUGINNAME);

        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSceneViewerD3DPlugin::CanView(NiViewerPluginInfo* pkInfo)
{
    // Check current version.
    if (pkInfo->GetClassName() == PARAM_PLUGINNAME &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    // Have this plug-in handle scripts that specify NiSceneViewerDX9Plugin,
    // NiSceneViewerD3D10Plugin or NiSceneViewerPlugin.
    if (pkInfo->GetClassName() == "NiSceneViewerDX9Plugin" ||
        pkInfo->GetClassName() == "NiSceneViewerD3D10Plugin" ||
        pkInfo->GetClassName() == "NiSceneViewerPlugin")
    {
        if (CompareVersions("1.0", pkInfo->GetVersion()))
        {
            // The latest version of these types of scripts supported is
            // 1.0. All later versions are not supported.
            return false;
        }

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiSceneViewerD3DPlugin::View(
    const NiViewerPluginInfo* pkInfo)
{
    NiFramework& kFramework = NiFramework::GetFramework();
    // Get the NiSceneGraphSharedData, creating it if it does not exist.
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiSceneGraphSharedData* pkSGSharedData = NULL;
    pkSGSharedData =(NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    // Build plug-in data path.
    char acPluginDataPath[NI_MAX_PATH];
    NiSprintf(acPluginDataPath, NI_MAX_PATH, "%s%s", (const char*)
        m_strAbsolutePluginPath, "..\\..\\Data\\");
    NiPath::RemoveDotDots(acPluginDataPath);

    // Retrieve UIM relative path from script.
    NiString strUIMFile = pkInfo->GetValue(PARAM_UIMFILE);

    if (NiPath::IsRelative(strUIMFile))
    {
        // Convert UIM path from relative to absolute.
        char acAbsoluteUIMPath[NI_MAX_PATH];
        size_t stBytes = NiPath::ConvertToAbsolute(acAbsoluteUIMPath,
            NI_MAX_PATH, strUIMFile, acPluginDataPath);
        if (stBytes == 0)
        {
            NiMessageBox("Unable to build absolute user-interface map file "
                "path.", m_strName + " Error");
            return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
        }
        strUIMFile = acAbsoluteUIMPath;
    }

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiRendererSharedData* pkRendererSharedData = NULL;
    pkRendererSharedData =(NiRendererSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiRendererSharedData));

    if (!pkRendererSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    NiTimerSharedData* pkTimerSharedData = NULL;
    pkTimerSharedData =(NiTimerSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiTimerSharedData));

    SceneViewerInfo* pkViewerInfo = NiNew SceneViewerInfo();

    NiSystemDesc::RendererID eRendererType = NiSystemDesc::RENDERER_GENERIC;
    if (pkInfo->GetClassName() == "NiSceneViewerDX9Plugin" ||
        pkInfo->GetClassName() == "NiSceneViewerPlugin" ||
        pkInfo->GetClassName() == "NiSceneViewerD3DPlugin")
    {
        // Handle old plug-in versions.
        eRendererType = NiSystemDesc::RENDERER_DX9;
    }
    else if (pkInfo->GetClassName() == "NiSceneViewerD3D10Plugin")
    {
        eRendererType = NiSystemDesc::RENDERER_D3D10;
    }
    switch (eRendererType)
    {
        case NiSystemDesc::RENDERER_GENERIC:
        case NiSystemDesc::RENDERER_DX9:
            pkViewerInfo->m_bD3D10 = false;
            break;
        case NiSystemDesc::RENDERER_D3D10:
            pkViewerInfo->m_bD3D10 = true;
            break;
        default:
            // Unknown renderer type.
            NiMessageBox("Unknown renderer type specified in script.",
                m_strName + " Error");
            return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
            break;
    }

    //Instanced data requires this to be set.  
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(eRendererType);

    pkViewerInfo->m_bFullScreen = pkRendererSharedData->GetFullscreen();
    pkViewerInfo->m_bStencilBuffer = true;
    
    pkViewerInfo->m_kDimensions = pkRendererSharedData->GetWindowSize();
    pkViewerInfo->m_kLocation.x = 0.0f;
    pkViewerInfo->m_kLocation.y = 0.0f;


    pkViewerInfo->m_kBackgroundColor = 
        pkRendererSharedData->GetBackgroundColor();
    pkViewerInfo->m_fAnimationLoopStartTime =
        pkTimerSharedData->GetStartTime();
    pkViewerInfo->m_fAnimationLoopEndTime = pkTimerSharedData->GetEndTime();

    NiNode* pkScene = pkSGSharedData->GetFullSceneGraph();
    NiNode* pkTreeRoot = pkScene;

    PrintOutSceneGraph(pkScene);
    if (pkViewerInfo->m_spScene != NULL)
        pkViewerInfo->m_spScene->CheckInSceneGraph(pkScene);
    else
    {
        pkViewerInfo->m_spScene = NiNew NiScene();
        pkViewerInfo->m_spScene->CheckInSceneGraph(pkScene);
    }

    pkViewerInfo->m_hI = m_hModule;
    pkViewerInfo->m_iWinMode = SW_SHOWNORMAL;

    pkViewerInfo->m_pcDefaultFilePath = NiAlloc(char, NI_MAX_PATH);
    NiStrcpy(pkViewerInfo->m_pcDefaultFilePath, NI_MAX_PATH, acPluginDataPath);

    pkViewerInfo->m_pcUIMapFilename = NiAlloc(char, NI_MAX_PATH);
    NiStrcpy(pkViewerInfo->m_pcUIMapFilename, NI_MAX_PATH, strUIMFile);

    char* pcTitleString = NiAlloc(char, 256);
    NiString strTitle = kFramework.GetAppName() + " Viewer";
    NiStrcpy(pcTitleString, 256, (const char*) strTitle);
    pkViewerInfo->m_pcWindowTitle = pcTitleString;

    pkViewerInfo->m_bLoopAnimations = true;

    pkViewerInfo->m_uiFrameRate = 60;

    if (!pkViewerInfo->m_bFullScreen)
        m_kSceneViewerDll.RunThreaded(pkViewerInfo);
    else
        m_kSceneViewerDll.Run(pkViewerInfo);

    if (pkInfo->GetBool(PARAM_SHOWTREEVIEW))
    {
        m_kSceneViewerDll.RunSceneGraphTreeViewer(m_hModule, 0,
            pkTreeRoot,"Tree View", 0,0,640,480);
    }
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool NiSceneViewerD3DPlugin::SupportsRenderer(
    NiSystemDesc::RendererID eRenderer)
{
    return (eRenderer == NiSystemDesc::RENDERER_DX9 || 
        eRenderer == NiSystemDesc::RENDERER_D3D10);
}
//---------------------------------------------------------------------------
