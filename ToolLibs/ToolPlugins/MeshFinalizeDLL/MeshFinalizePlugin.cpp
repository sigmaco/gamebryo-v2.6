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
#include "MeshFinalizePlugin.h"
#include "MeshFinalizeDialog.h"
#include <NiMeshProfileProcessor.h>
#include <NiExporterOptionsSharedData.h>

NiImplementRTTI(MeshFinalizePlugin, NiPlugin);
MeshFinalizePlugin* MeshFinalizePlugin::ms_pkThis = NULL;

MeshFinalizePlugin::RendererFinalizeDLLInfo MeshFinalizePlugin::ms_kRendererFinalizeDLLInfo[] =
{
    {NiSystemDesc::RENDERER_WII, "NiWiiMeshFinalize" NI_DLL_SUFFIX ".dll"},
    {NiSystemDesc::RENDERER_NUM, 0}
};

//-----------------------------------------------------------------------------------------------
MeshFinalizePlugin::MeshFinalizePlugin() :
    NiPlugin("Mesh Finalize", "1.0", 
        "Finalizes NiMesh data streams.",
        "Finalizes NiMesh data streams by setting final format and "
        "interleaving elements."), 
        m_pkProgressBar(NULL)
{
    // Load any renderer specific finalize libraries if they exists
    LoadRendererFinalizeLibraries();
}
//-----------------------------------------------------------------------------------------------
MeshFinalizePlugin::~MeshFinalizePlugin()
{
    // Unload any renderer specific finalize libraries
    UnloadRendererFinalizeLibraries();
}
//-----------------------------------------------------------------------------------------------
bool MeshFinalizePlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "MeshFinalizePlugin" 
    && VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::ProgressInitCB(unsigned int uiSize)
{
    // Initialize the range for the progress bar...
    ms_pkThis->m_pkProgressBar->SetRangeSpan(uiSize);
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::ProgressCB(NiMeshProfileErrorHandler&
    kMessageHandler)
{
    ms_pkThis->m_pkProgressBar->StepIt();

    NiUInt32 uiMsgCnt = kMessageHandler.GetMessageCount();
    bool bFoundIssues = false;
    for(NiUInt32 ui=0; ui < uiMsgCnt; ui++)
    {
        if (kMessageHandler.GetMessageType(ui) !=
            NiMeshProfileErrorHandler::MT_ANNOTATION)
        {
            bFoundIssues = true;
            break;
        }
    }

    // Early out if no issues
    if (bFoundIssues == false)
    {
        kMessageHandler.ClearMessages();
        return;
    }

    ms_pkThis->m_bErrorsOrWarningsFound = true;

    for(NiUInt32 ui=0; ui < uiMsgCnt; ui++)
    {
        // If annotation, only add if next is not an annotation.
        if (kMessageHandler.GetMessageType(ui) == 
            NiMeshProfileErrorHandler::MT_ANNOTATION)
        {
            if (ui < (uiMsgCnt - 1))
            {
                if (kMessageHandler.GetMessageType(ui+1) != 
                    NiMeshProfileErrorHandler::MT_ANNOTATION)
                {
                    ms_pkThis->m_strMessage += kMessageHandler.
                        GetMessage(ui);
                }
            }
        }
        else
        {
            // This is an error or warning, so always add
            ms_pkThis->m_strMessage += kMessageHandler.GetMessage(ui);
        }
    }

    kMessageHandler.ClearMessages();
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::RendererFinalizeProgressCB(NiUInt32 uiTotalMeshCount, 
    NiUInt32 uiCurrentMesh)
{
    ms_pkThis->m_pkProgressBar->SetRangeSpan(uiTotalMeshCount);
    ms_pkThis->m_pkProgressBar->SetPosition(uiCurrentMesh);
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::ReportErrors()
{
    if (ms_pkThis->m_bErrorsOrWarningsFound == false)
        return;

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

    ms_pkThis->m_strMessage +=
        "=====================================================\n";

    if ((bWriteResultsToLog) && (pkLogger))
    {
        // Log all messages...
        pkLogger->LogElement("MeshFinalizePlugin Issues", 
            ms_pkThis->m_strMessage);
    }
    else
    {
        NiInfoDialog kDlg("MeshFinalizedPlugin Issues");
        kDlg.SetText(ms_pkThis->m_strMessage);
        kDlg.DoModal();
    }
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::ErrorPrintf(const char* pcFormat, ...)
{
    ms_pkThis->m_bErrorsOrWarningsFound = true;

    char acBuffer[1024];
    va_list kArgs;
    va_start(kArgs, pcFormat);
    NiVsprintf(acBuffer, sizeof(acBuffer), pcFormat, kArgs);

    ms_pkThis->m_strMessage += acBuffer;
}
//-----------------------------------------------------------------------------------------------
unsigned int MeshFinalizePlugin::CountMeshes(NiAVObject* pkObject)
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
    else if (NiIsKindOf(NiMesh, pkObject))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
//-----------------------------------------------------------------------------------------------
NiExecutionResultPtr MeshFinalizePlugin::Execute(const NiPluginInfo* pkInfo)
{
    ms_pkThis = this;
    ms_pkThis->m_bErrorsOrWarningsFound = false;

    // Create single string of all errors and warnings encountered.
    // But filter out annotations that succeeded
    ms_pkThis->m_strMessage =
        "MeshFinalizePlugin Errors and Warnings encountered\n";
    ms_pkThis->m_strMessage +=
        "=====================================================\n";

    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    m_pkProgressBar = NiNew NiProgressDialog(
        "Finalize NiMesh");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetPosition(0);
    m_pkProgressBar->SetLineOne("Finalize...");
    
    // Iterate over all roots and finalize all meshes in the NIF
    // [1] Build the list of top level objects
    NiTLargeObjectArray<NiObjectPtr> kTopObjects;
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
        kTopObjects.Add((NiObject*)pkSGSharedData->GetRootAt(ui));

    // Call renderer specific pre-finalize DLL func if available
    const char* pcRendererFinalizeDLLName = GetRendererFinalizeLibraryName();
    if (pcRendererFinalizeDLLName)
    {
        NiMeshRendererFinalizeFunc pfnPreFinalizeFunc;
        if (m_kPreFinalizeFuncMap.GetAt(
            NiSystemDesc::GetSystemDesc().GetToolModeRendererID(), pfnPreFinalizeFunc))
        {
            // Handle Wii renderer specific mesh finalize processing
            if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() ==
                NiSystemDesc::RENDERER_WII)
            {
                NiTFixedStringMap<const char*> kOptions;
                NiString kIndex = pkInfo->GetValue(WII_INDEX_STREAM_REMOVAL_KEY);
                NiString kSkinning = pkInfo->GetValue(WII_CONVERT_SKINNING_STREAMS_KEY);
                NiString kDisplayList = pkInfo->GetValue(WII_CREATE_DISPLAY_LIST_STREAM_KEY);

                if (kIndex.IsEmpty())
                    kIndex = "1";
                if (kSkinning.IsEmpty())
                    kSkinning = "1";
                if (kDisplayList.IsEmpty())
                    kDisplayList = "1";

                kOptions.SetAt(WII_INDEX_STREAM_REMOVAL_KEY, (const char*)kIndex);
                kOptions.SetAt(WII_CONVERT_SKINNING_STREAMS_KEY, (const char*)kSkinning);
                kOptions.SetAt(WII_CREATE_DISPLAY_LIST_STREAM_KEY, (const char*)kDisplayList);

                m_pkProgressBar->SetLineOne("Renderer Pre-Finalize...");
                pfnPreFinalizeFunc(kTopObjects, kOptions, 
                    RendererFinalizeProgressCB, ErrorPrintf);
            }
        }
        else
        {
            ErrorPrintf("WARNING: Expected to find %s for\n"
                "additional renderer specific mesh finalize "
                "processing. Meshes\nmay not be completely "
                "finalized for the selected platform.\n", pcRendererFinalizeDLLName);
        }
    }

    // Reset progress
    m_pkProgressBar->SetPosition(0);
    m_pkProgressBar->SetLineOne("Finalize...");

    // Call the profile processor
    NiMeshProfileProcessor::SetProgressInitCallback(ProgressInitCB);
    NiMeshProfileProcessor::SetProgressCallback(ProgressCB);
    NiMeshProfileProcessor::Finalize(kTopObjects, 
        NiSystemDesc::GetSystemDesc().GetToolModeRendererID());

    // Call renderer specific post-finalize DLL if available
    if (pcRendererFinalizeDLLName)
    {
        NiMeshRendererFinalizeFunc pfnPostFinalizeFunc;
        if (m_kPostFinalizeFuncMap.GetAt(
            NiSystemDesc::GetSystemDesc().GetToolModeRendererID(), pfnPostFinalizeFunc))
        {
            // Handle Wii renderer specific mesh finalize processing
            if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() ==
                NiSystemDesc::RENDERER_WII)
            {
                NiTFixedStringMap<const char*> kOptions;
                NiString kIndex = pkInfo->GetValue(WII_INDEX_STREAM_REMOVAL_KEY);
                NiString kSkinning = pkInfo->GetValue(WII_CONVERT_SKINNING_STREAMS_KEY);
                NiString kDisplayList = pkInfo->GetValue(WII_CREATE_DISPLAY_LIST_STREAM_KEY);

                if (kIndex.IsEmpty())
                    kIndex = "1";
                if (kSkinning.IsEmpty())
                    kSkinning = "1";
                if (kDisplayList.IsEmpty())
                    kDisplayList = "1";

                kOptions.SetAt(WII_INDEX_STREAM_REMOVAL_KEY, (const char*)kIndex);
                kOptions.SetAt(WII_CONVERT_SKINNING_STREAMS_KEY, (const char*)kSkinning);
                kOptions.SetAt(WII_CREATE_DISPLAY_LIST_STREAM_KEY, (const char*)kDisplayList);

                m_pkProgressBar->SetLineOne("Renderer Post-Finalize...");
                pfnPostFinalizeFunc(kTopObjects, kOptions, 
                    RendererFinalizeProgressCB, ErrorPrintf);
            }
        }
    }

    ReportErrors();

    // Cleanup
    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;
    ms_pkThis = NULL;

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//-----------------------------------------------------------------------------------------------
NiPluginInfo* MeshFinalizePlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("MeshFinalizePlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//-----------------------------------------------------------------------------------------------
bool MeshFinalizePlugin::HasManagementDialog()
{
    return true;
}
//-----------------------------------------------------------------------------------------------
bool MeshFinalizePlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    // The management dialog depends on if there are any loaded renderer finalize DLLs or not
    WORD wResourceId = (m_kFinalizeDLLMap.GetCount() == 0) ? 
        IDD_OPT_MESHFINALIZE : IDD_OPT_RENDERER_MESHFINALIZE;

    MeshFinalizeDialog kDialog(wResourceId, m_hModule, hWndParent, pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
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
//-----------------------------------------------------------------------------------------------
const char* MeshFinalizePlugin::GetRendererFinalizeLibraryName()
{
    // See if a DLL is named for the current renderer
    for (NiUInt32 ui = 0; ms_kRendererFinalizeDLLInfo[ui].m_pcDLLName; ++ui)
    {
        NiSystemDesc::RendererID eRendererID =
            ms_kRendererFinalizeDLLInfo[ui].m_eRendererID;
        const char* pcDLLName =
            ms_kRendererFinalizeDLLInfo[ui].m_pcDLLName;

        if (eRendererID == NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
        {
            return pcDLLName;
        }
    }

    return NULL;
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::LoadRendererFinalizeLibraries()
{
    // Attempt to load any renderer specific finalize DLLs
    for (NiUInt32 ui = 0; ms_kRendererFinalizeDLLInfo[ui].m_pcDLLName; ++ui)
    {
        NiSystemDesc::RendererID eRendererID =
            ms_kRendererFinalizeDLLInfo[ui].m_eRendererID;
        const char* pcDLLName =
            ms_kRendererFinalizeDLLInfo[ui].m_pcDLLName;
      
        HMODULE hRendererFinalizeDLL = LoadLibrary(pcDLLName);

        if (hRendererFinalizeDLL)
        {
            NiMeshRendererFinalizeFunc pfnRendererPreFinalizeFunc = 
                (NiMeshRendererFinalizeFunc)GetProcAddress(
                hRendererFinalizeDLL, "PreFinalize");
            NiMeshRendererFinalizeFunc pfnRendererPostFinalizeFunc = 
                (NiMeshRendererFinalizeFunc)GetProcAddress(
                hRendererFinalizeDLL, "PostFinalize");

            if (pfnRendererPreFinalizeFunc && pfnRendererPostFinalizeFunc)
            {
                m_kFinalizeDLLMap.SetAt(eRendererID, 
                    hRendererFinalizeDLL);
                m_kPreFinalizeFuncMap.SetAt(eRendererID,
                    pfnRendererPreFinalizeFunc);
                m_kPostFinalizeFuncMap.SetAt(eRendererID,
                    pfnRendererPostFinalizeFunc);
            }
            else
            {
                NIASSERT(false && "Platform Finalize funcs not found!");
            }
        }
    }
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizePlugin::UnloadRendererFinalizeLibraries()
{
    // Unload any renderer specific finalize DLLs
    NiTMapIterator kIter = m_kFinalizeDLLMap.GetFirstPos();
    while (kIter)
    {
        NiSystemDesc::RendererID eRendererID;
        HMODULE hRendererFinalizeDLL;

        m_kFinalizeDLLMap.GetNext(kIter, eRendererID, hRendererFinalizeDLL);
        FreeLibrary(hRendererFinalizeDLL);
    }
}
//-----------------------------------------------------------------------------------------------
