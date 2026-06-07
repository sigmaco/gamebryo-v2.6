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
#include "ApplyMeshProfileDialog.h"
#include "ApplyMeshProfilePlugin.h"
#include "ApplyMeshProfilePluginDefines.h"

#include <NiMeshProfileProcessor.h>
#include <NiTNodeTraversal.h>

// RTTI implementation macro.
NiImplementRTTI(ApplyMeshProfilePlugin, NiPlugin);

//---------------------------------------------------------------------------
ApplyMeshProfilePlugin::ApplyMeshProfilePlugin() : NiPlugin(
    // name
    "Apply Mesh Profile Plug-in",
    // version
    "1.0", 
    // short description
    "A plug-in to apply a custom mesh profile to meshes in the scene.",
    // long description
    "Replaces the extra data on all meshes in the scene that specifies "  
    "the mesh profile to apply with the user-defined profile name.  "
    "This plug-in should run prior to the MeshFinalize plug-in.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* ApplyMeshProfilePlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName(APPLYMESHPROFILEPLUGIN_CLASSNAME);
    pkPluginInfo->SetType("PROCESS");
    pkPluginInfo->SetValue(APPLY_MESH_PROFILE_NAME, "Default");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool ApplyMeshProfilePlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return true;
}
//---------------------------------------------------------------------------
bool ApplyMeshProfilePlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    ApplyMeshProfileDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

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
//---------------------------------------------------------------------------
bool ApplyMeshProfilePlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the 
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() != APPLYMESHPROFILEPLUGIN_CLASSNAME ||
        !VerifyVersionString(pkInfo->GetVersion()))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void ApplyMeshProfilePlugin::ErrorPrintf(const char* pcFormat, ...)
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

    char acBuffer[1024];
    va_list kArgs;
    va_start(kArgs, pcFormat);
    NiVsprintf(acBuffer, sizeof(acBuffer), pcFormat, kArgs);

    if ((bWriteResultsToLog) && (pkLogger))
    {
        // Log all messages...
        pkLogger->LogElement("ApplyMeshProfile Issues", acBuffer);
    }
    else
    {
        NiInfoDialog kDlg("ApplyMeshProfile Issues");
        kDlg.SetText(acBuffer);
        kDlg.DoModal();
    }
}
//---------------------------------------------------------------------------
struct ReplaceProfileNameFunctor
{
    inline void operator () (NiAVObject* pkAVObject)
    {
        if (!NiIsKindOf(NiMesh, pkAVObject))
            return;

        NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, pkAVObject);
        NiStringExtraData* pkData = NiDynamicCast(NiStringExtraData, 
            pkMesh->GetExtraData("MeshProfileName"));
        if (pkData)
        {
            pkData->SetValue(m_kProfileName);
        }
        else
        {
            pkData = NiNew NiStringExtraData(m_kProfileName);
            pkMesh->AddExtraData("MeshProfileName", pkData);
        }
    }

    NiFixedString m_kProfileName;
};
//---------------------------------------------------------------------------
NiExecutionResultPtr ApplyMeshProfilePlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    if (pkSGSharedData->GetRootCount() == 0)
    {
        // If no roots, there's nothing to do...
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
    }

    ReplaceProfileNameFunctor kFunc;
    kFunc.m_kProfileName = pkInfo->GetValue(APPLY_MESH_PROFILE_NAME);

    // We will get an error in MeshFinalize as well, but we should throw
    // it here so it's obvious where the incorrect profile is coming from.
    if (!NiMeshProfileProcessor::GetProfile(kFunc.m_kProfileName, 
        NiSystemDesc::GetSystemDesc().GetToolModeRendererID()))
    {
        ErrorPrintf("Invalid profile name (%s) for renderer (%s).\n"
            "=====================================================\n",
            (const char*)kFunc.m_kProfileName,
            NiSystemDesc::GetRendererString(
                NiSystemDesc::GetSystemDesc().GetToolModeRendererID()));

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        NiTNodeTraversal::DepthFirst_AllObjects(spScene, kFunc);
    }

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//-----------------------------------------------------------------------
