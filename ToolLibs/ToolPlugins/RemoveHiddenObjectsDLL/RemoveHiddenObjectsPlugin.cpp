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
#include "RemoveHiddenObjectsPlugin.h"

// RTTI implementation macro.
NiImplementRTTI(RemoveHiddenObjectsPlugin, NiPlugin);

//---------------------------------------------------------------------------
RemoveHiddenObjectsPlugin::RemoveHiddenObjectsPlugin() : NiPlugin(
    "Remove Hidden Objects Plug-in",                // name
    "1.0",                                          // version
    "A plug-in to remove all hidden objects",       // short description
    "A plug-in to remove all hidden objects.")      // long description
{
    m_pkCurrentRoot = NULL;
}
//---------------------------------------------------------------------------
NiPluginInfo* RemoveHiddenObjectsPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("RemoveHiddenObjectsPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool RemoveHiddenObjectsPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool RemoveHiddenObjectsPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool RemoveHiddenObjectsPlugin::CanExecute(NiPluginInfo* pkInfo)
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

    if (pkInfo->GetClassName() == "RemoveHiddenObjectsPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr RemoveHiddenObjectsPlugin::Execute(
    const NiPluginInfo*)
{
    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll want to use the scene graph in this process plug-in,
    // so we'll go ahead and get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!pkSGSharedData)
    {
        // Pop up an NiMessageBox giving a more useful warning.
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
            pkLogger->LogElement("RemoveHiddenObjectsError", "The Remove "
                "Hidden Objects Process Plug-in could not find the "
                "NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Remove Hidden Objects Process Plug-in could not"
                " find the NiSceneGraphSharedData!",
                "Scene Graph Shared Data Missing");
        }

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        m_pkCurrentRoot = pkSGSharedData->GetRootAt(ui);
        NiOptimize::RemoveAppCulledObjects(m_pkCurrentRoot);
    }

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
