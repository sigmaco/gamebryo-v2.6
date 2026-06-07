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

#include "StdAfx.h"
#include "NiPortalConverterPlugin.h"
#include "PortalConverter.h"
#include <NiXMLLogger.h>

NiImplementRTTI(NiPortalConverterPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiPortalConverterPlugin::NiPortalConverterPlugin() :
    NiPlugin("Portal Converter", "3.0", "Converts tagged geometry to "
    "portals.", "Converts a scene graph with the proper tagged geometry to "
    "one with NiPortal and NiWall objects.")
{
}
//---------------------------------------------------------------------------
bool NiPortalConverterPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiPortalConverterPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiPortalConverterPlugin::Execute(
    const NiPluginInfo*)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    
    NiNodePtr spScene = pkSGSharedData->GetFullSceneGraph();

    PortalConverter kConverter(spScene);

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
        pkLogger->LogElement("PortalMessages", kConverter.Messages());

        NiString strMsg;
        if (!kConverter.Successful())
        {
            strMsg.Format("The Portal Converter did not convert the scene "
                "graph rooted at %s.\nSee the Portal Messages dialog for "
                "details.\n", spScene->GetName());
            pkLogger->LogElement("PortalError", strMsg);
        }
    }
    else
    {
        NiInfoDialog kDlg("Portal Messages");
        kDlg.SetText(kConverter.Messages());
        kDlg.DoModal();

        NiString strMsg;
        if (!kConverter.Successful())
        {
            strMsg.Format("The Portal Converter did not convert the scene "
                "graph rooted at %s.\nSee the Portal Messages dialog for "
                "details.\n", spScene->GetName());
            ::MessageBox(0, strMsg, "Portal Converter Error",
                MB_OK | MB_ICONERROR);
        }
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiPortalConverterPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiPortalConverterPlugin");
    pkPluginInfo->SetType("PROCESS");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiPortalConverterPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiPortalConverterPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
