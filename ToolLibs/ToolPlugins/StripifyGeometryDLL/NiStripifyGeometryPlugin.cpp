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
#include "NiStripifyGeometryPlugin.h"
#include "NiStripifyGeometryDialog.h"
#include "NiStripifyDLLDefines.h"
#include <NiExporterOptionsSharedData.h>
#include <NiStripifyInterface.h>
#include <NiMesh.h>
#include <NiPSParticleSystem.h>

NiImplementRTTI(NiStripifyGeometryPlugin, NiPlugin);
NiStripifyGeometryPlugin* NiStripifyGeometryPlugin::ms_pkThis = NULL;

//---------------------------------------------------------------------------
NiStripifyGeometryPlugin::NiStripifyGeometryPlugin() :
    NiPlugin("Stripify Geometry", "2.0",
    "Convert NiMesh streams to contain triangle strips",
    "Turn the Geometry into triangle strips"), m_pkProgressBar(NULL)
{
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiStripifyGeometryPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::PerObjectCallback(NiAVObject* pkObject)
{
    if (ms_pkThis != NULL && ms_pkThis->m_pkProgressBar != NULL)
    {
        ms_pkThis->m_pkProgressBar->SetLineTwo(pkObject->GetName());
        ms_pkThis->m_pkProgressBar->StepIt();
    }
    return true;
}
//---------------------------------------------------------------------------
void NiStripifyGeometryPlugin::ErrorCallback(unsigned int, 
    NiObject*, const char* pcMessage)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if ((bWriteResultsToLog) && (pkLogger))
    {
        pkLogger->LogElement("StripificationWarning", pcMessage);
    }
    else
    {
        NiMessageBox(pcMessage, "Stripification Warning");
    }
}
//---------------------------------------------------------------------------
unsigned int NiStripifyGeometryPlugin::CountMeshes(NiAVObject* pkObject)
{
    if (pkObject == NULL)
        return 0;

    if (NiIsKindOf(NiNode, pkObject))
    {
        unsigned int uiSum = 0;
        NiNode* pkNode = (NiNode*)pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            uiSum += CountMeshes(pkChild);
        }
        return uiSum;
    }
    else if (NiIsKindOf(NiMesh, pkObject) &&
        !NiIsKindOf(NiPSParticleSystem, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;
        NIASSERT(pkMesh != NULL);

        if (pkMesh->GetPrimitiveType() == NiPrimitiveType::PRIMITIVE_TRIANGLES)
            return 1;
        else
            return 0;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiStripifyGeometryPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    ms_pkThis = this;
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    bool bViewerStrip = false;
    bool bViewerRun = false;
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));

    if (pkExporterSharedData)
    {
        bViewerRun = pkExporterSharedData->GetViewerRun();
        bViewerStrip = pkExporterSharedData->GetOptimizeMeshForViewer();
    }
    
    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    m_pkProgressBar = NiNew NiProgressDialog("Stripifying Scene Graph");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetPosition(0);
    m_pkProgressBar->SetLineOne("Stripifying Geometry...");

    NiStripify::Init();
    NiStripify::SetFinishedCallback(PerObjectCallback);
    NiStripify::SetErrorCallback(ErrorCallback);
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        m_pkProgressBar->StepIt();

        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);       
        unsigned int uiSize = CountMeshes(spScene);
        m_pkProgressBar->SetRangeSpan(uiSize + 1);

        bool bStripify = false;
        NiUInt32 uiCacheSize = 0;
        NiStripifyInterface::Platform eStripifyPlatform =
            NiStripifyInterface::PLATFORM_COUNT;
        unsigned int uiScriptVersion =
            NiPluginHelpers::GetVersionFromString(pkInfo->GetVersion());
        if (CompareVersions(uiScriptVersion, m_strVersion))
        {
            // Catch the case where we're using an old script.
            switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
            {
                case NiSystemDesc::RENDERER_GENERIC:
                case NiSystemDesc::RENDERER_DX9:
                case NiSystemDesc::RENDERER_D3D10:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_WIN32;
                    break;
                case NiSystemDesc::RENDERER_PS3:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_PS3;
                    break;
                case NiSystemDesc::RENDERER_WII:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_WII;
                    break;
                case NiSystemDesc::RENDERER_XENON:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_XBOX360;
                    break;
            }
            bStripify = false;
            uiCacheSize =
                NiStripifyInterface::GetDefaultCacheSize(eStripifyPlatform);
        }
        else
        {
            switch (NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
            {
                case NiSystemDesc::RENDERER_GENERIC:
                case NiSystemDesc::RENDERER_DX9:
                case NiSystemDesc::RENDERER_D3D10:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_WIN32;
                    bStripify = pkInfo->GetBool(STRIP_PC_STRIPIFY_VALUE);
                    uiCacheSize = pkInfo->GetInt(STRIP_PC_CACHE_VALUE);
                    break;
                case NiSystemDesc::RENDERER_PS3:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_PS3;
                    bStripify = pkInfo->GetBool(STRIP_PS3_STRIPIFY_VALUE);
                    uiCacheSize = pkInfo->GetInt(STRIP_PS3_CACHE_VALUE);
                    break;
                case NiSystemDesc::RENDERER_WII:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_WII;
                    bStripify = pkInfo->GetBool(STRIP_WII_STRIPIFY_VALUE);
                    uiCacheSize = pkInfo->GetInt(STRIP_WII_CACHE_VALUE);
                    break;
                case NiSystemDesc::RENDERER_XENON:
                    eStripifyPlatform = NiStripifyInterface::PLATFORM_XBOX360;
                    bStripify = pkInfo->GetBool(STRIP_XENON_STRIPIFY_VALUE);
                    uiCacheSize = pkInfo->GetInt(STRIP_XENON_CACHE_VALUE);
                    break;
            }
        }

        if (bViewerRun && !bViewerStrip)
        {
            bStripify = false;
        }

        if (bStripify)
        {
            NiStripifyInterface kSI(eStripifyPlatform);
            kSI.SetInterfaceCacheSize(uiCacheSize);
            kSI.Stripify_Object(spScene);
        }
    }

    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;

    NiStripify::Shutdown();

    ms_pkThis = NULL;

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiStripifyGeometryPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiStripifyGeometryPlugin");
    pkPluginInfo->SetType("PROCESS");

    pkPluginInfo->SetBool(STRIP_PC_STRIPIFY_VALUE, true);
    pkPluginInfo->SetInt(STRIP_PC_CACHE_VALUE,
        NiStripifyInterface::GetDefaultCacheSize(
        NiStripifyInterface::PLATFORM_WIN32));

    pkPluginInfo->SetBool(STRIP_PS3_STRIPIFY_VALUE, true);
    pkPluginInfo->SetInt(STRIP_PS3_CACHE_VALUE,
        NiStripifyInterface::GetDefaultCacheSize(
        NiStripifyInterface::PLATFORM_PS3));

    pkPluginInfo->SetBool(STRIP_WII_STRIPIFY_VALUE, true);
    pkPluginInfo->SetInt(STRIP_WII_CACHE_VALUE,
        NiStripifyInterface::GetDefaultCacheSize(
        NiStripifyInterface::PLATFORM_WII));

    pkPluginInfo->SetBool(STRIP_XENON_STRIPIFY_VALUE, true);
    pkPluginInfo->SetInt(STRIP_XENON_CACHE_VALUE,
        NiStripifyInterface::GetDefaultCacheSize(
        NiStripifyInterface::PLATFORM_XBOX360));

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    // Convert old version script, if necessary.
    bool bConvertedOldScript = false;
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInitialInfo->GetVersion());
    if (CompareVersions(uiScriptVersion, m_strVersion))
    {
        pkInitialInfo->SetBool(STRIP_PC_STRIPIFY_VALUE, true);
        pkInitialInfo->SetInt(STRIP_PC_CACHE_VALUE,
            NiStripifyInterface::GetDefaultCacheSize(
            NiStripifyInterface::PLATFORM_WIN32));

        pkInitialInfo->SetBool(STRIP_PS3_STRIPIFY_VALUE, true);
        pkInitialInfo->SetInt(STRIP_PS3_CACHE_VALUE,
            NiStripifyInterface::GetDefaultCacheSize(
            NiStripifyInterface::PLATFORM_PS3));

        pkInitialInfo->SetBool(STRIP_WII_STRIPIFY_VALUE, true);
        pkInitialInfo->SetInt(STRIP_WII_CACHE_VALUE,
            NiStripifyInterface::GetDefaultCacheSize(
            NiStripifyInterface::PLATFORM_WII));

        pkInitialInfo->SetBool(STRIP_XENON_STRIPIFY_VALUE, true);
        pkInitialInfo->SetInt(STRIP_XENON_CACHE_VALUE,
            NiStripifyInterface::GetDefaultCacheSize(
            NiStripifyInterface::PLATFORM_XBOX360));

        pkInitialInfo->SetVersion(m_strVersion);
        bConvertedOldScript = true;
    }

    NiStripifyGeometryDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        return true;
        break;
    case IDCANCEL:
        break;
    default:
        NIASSERT(false);
    }

    return bConvertedOldScript;
}
//---------------------------------------------------------------------------
