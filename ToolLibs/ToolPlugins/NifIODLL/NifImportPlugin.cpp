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
#include "NifImportPlugin.h"

NiImplementRTTI(NifImportPlugin, NiImportPlugin);

//---------------------------------------------------------------------------
NifImportPlugin::NifImportPlugin() :
    NiImportPlugin("NIF Importer", "1.2", "Imports NIF files.", "Imports "
    "NIF files and stores the result in an NiSceneGraphSharedData object.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NifImportPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiImportPluginInfo;
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NifImportPlugin");
    pkPluginInfo->SetType("IMPORT");
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NifImportPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NifImportPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool NifImportPlugin::CanImport(NiImportPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NifImportPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NifImportPlugin::Import(const NiImportPluginInfo* pkInfo)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    // Get the exporter options shared data
    NiExporterOptionsSharedData* pkEOSharedData = NULL;
    pkEOSharedData = (NiExporterOptionsSharedData*)pkDataList->Get(
        NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;

    if (pkEOSharedData)
    {
        bWriteResultsToLog = pkEOSharedData->GetWriteResultsToLog();
        pkLogger = pkEOSharedData->GetXMLLogger();
    }

    // Get file path.
    NiString strFilePath = pkInfo->GetFullFilePath();
    if (strFilePath.IsEmpty())
    {
        NiString strMessage = "Nif full file path is empty!";
        
        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("NifImportPluginError", strMessage);
        }
        else
        {
            MessageBox(NULL, strMessage, "Import Failure", MB_ICONERROR |
                MB_OK);
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the NiSceneGraphSharedData, creating it if it does not exist.
    NiSceneGraphSharedData* pkSGSharedData = NULL;
    pkSGSharedData =(NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        pkSGSharedData = NiNew NiSceneGraphSharedData;
        pkDataList->Insert(pkSGSharedData);
    }
 
    // Load the NIF file.
    NiStream kStream;
    if (!kStream.Load(strFilePath))
    {
        NiString strMessage = kStream.GetLastErrorMessage();
        
        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("NifImportPluginError", strMessage);
        }
        else
        {
            MessageBox(NULL, strMessage, "Import Failure", MB_ICONERROR |
                MB_OK);
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Save the endianness
    pkSGSharedData->SetSourceLittleEndian(kStream.GetSourceIsLittleEndian());

    NiNode* pkNode = NiDynamicCast(NiNode, kStream.GetObjectAt(0));
    if (!pkNode)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    if (pkInfo->GetImportType() == NiImportPluginInfo::ADD)
    {
        pkSGSharedData->AddRoot(pkNode, strFilePath, true);
    }
    else if (pkInfo->GetImportType() == NiImportPluginInfo::REPLACE)
    {
        pkSGSharedData->RemoveAllRoots();
        pkSGSharedData->AddRoot(pkNode, strFilePath, true);
    }
    
    if (kStream.GetObjectCount() > 1)
    {
        // Add any other objects in NiExtraObjectsSharedData
        NiExtraObjectsSharedData* pkExObjSharedData = NULL;
        pkExObjSharedData =(NiExtraObjectsSharedData*) 
            pkDataList->Get(NiGetSharedDataType(NiExtraObjectsSharedData));

        if (!pkExObjSharedData)
        {
            pkExObjSharedData = NiNew NiExtraObjectsSharedData;
            pkDataList->Insert(pkExObjSharedData);
        }

        if (pkInfo->GetImportType() == NiImportPluginInfo::REPLACE)
        {
            pkExObjSharedData->RemoveAllObjects();
        }
        
        for (NiUInt32 ui = 1; ui < kStream.GetObjectCount(); ui++)
        {
            char pcTail[10];
            NiSprintf(pcTail, 10, ":%d", ui);
            NiString strName = strFilePath + pcTail;
            pkExObjSharedData->
                AddObject(kStream.GetObjectAt(ui), strName, true);
        }
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
const char* NifImportPlugin::GetFileDescriptorString()
{
    return "Gamebryo NIF Files (*.nif)";
}
//---------------------------------------------------------------------------
const char* NifImportPlugin::GetFileTypeString()
{
    return "*.nif";
}
//---------------------------------------------------------------------------
