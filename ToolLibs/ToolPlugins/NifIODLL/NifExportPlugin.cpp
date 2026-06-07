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
#include "NifExportPlugin.h"
#include "NiExporterOptionsSharedData.h"
#include <errno.h>

NiImplementRTTI(NifExportPlugin, NiExportPlugin);

//---------------------------------------------------------------------------
NifExportPlugin::NifExportPlugin() :
    NiExportPlugin("NIF Exporter", "1.2", "Exports NIF files.", "Exports "
    "NIF files from an NiSceneGraphSharedData object.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NifExportPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiExportPluginInfo;
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NifExportPlugin");
    pkPluginInfo->SetType("EXPORT");

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NifExportPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool NifExportPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool NifExportPlugin::CanExport(NiExportPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NifExportPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NifExportPlugin::Export(const NiExportPluginInfo* pkInfo)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkEOSharedData = NULL;
    pkEOSharedData = (NiExporterOptionsSharedData*)pkDataList->Get(
        NiGetSharedDataType(NiExporterOptionsSharedData));

    // Get file path.
    NiString strFilePath = pkInfo->GetFullFilePath();
    if (strFilePath.IsEmpty())
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Check for existing read-only file.
    if (!NiFile::Access(strFilePath, NiFile::WRITE_ONLY))
    {
        NiString strMessage;
        strMessage.Format("%s is read-only. The file will not be "
            "overwritten.", strFilePath);
        bool bWriteResultsToLog = false;
        NiXMLLogger* pkLogger = NULL;

        if (pkEOSharedData)
        {
            bWriteResultsToLog = pkEOSharedData->GetWriteResultsToLog();
            pkLogger = pkEOSharedData->GetXMLLogger();
        }

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("NifExportPluginError", strMessage);
        }
        else
        {
            MessageBox(NULL, strMessage, "Read-Only File", MB_ICONERROR |
                MB_OK);
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the NiSceneGraphSharedData, throwing an error if it does not exist.
    NiSceneGraphSharedData* pkSGSharedData = NULL;
    pkSGSharedData = (NiSceneGraphSharedData*)pkDataList->Get(
        NiGetSharedDataType(NiSceneGraphSharedData));
    
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Save the NIF file.
    NiStream kStream;

    // Set the endianness of the export stream.
    // Priority 1 - Exporter Options (art exporter), via
    //     NiSystemDesc::GetToolModeRendererIsLittleEndian
    // Priority 2 - Scene Graph Options (importer)
    // Fallback - use default (exporter platform) endianness
    bool bTargetLittle;
    if (pkEOSharedData)
    {
        bTargetLittle =
            NiSystemDesc::GetSystemDesc().GetToolModeRendererIsLittleEndian();
    }
    else if (pkSGSharedData->GetPreserveEndianness())
    {
        bTargetLittle = pkSGSharedData->GetSourceLittleEndian();
    }
    else
    {
        bTargetLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    }
    kStream.SetSaveAsLittleEndian(bTargetLittle);
        
    // Get the scene graph.
    NiUInt32 ui = 0;
    for (; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNode* pkRoot = pkSGSharedData->GetRootAt(ui);

        if (pkSGSharedData->IsRootStreamable(pkRoot))
        {
            kStream.InsertObject(pkRoot);
        }
    }

    // Get the cameras.
    for (ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNode* pkRoot = pkSGSharedData->GetRootAt(ui);
        if (pkSGSharedData->IsRootStreamable(pkRoot))
        {
            InsertCameras(pkRoot, &kStream);
        }
    }
    
    // Look for extra streamable data
    NiExtraObjectsSharedData* pkExtraSharedData = NULL;
    pkExtraSharedData = (NiExtraObjectsSharedData*)pkDataList->Get(
        NiGetSharedDataType(NiExtraObjectsSharedData));
    
    if (pkExtraSharedData)
    {
        for (ui = 0; ui < pkExtraSharedData->GetObjectCount(); ui++)
        {
            NiObject* pkObj = pkExtraSharedData->GetObjectAt(ui);
            if (pkExtraSharedData->IsObjectStreamable(pkObj))
            {
                // Check not already at the top level in the NIF. This
                // situation can arise because, for instance, the cameras
                // are already inserted.
                bool bAlreadyStreamed = false;
                for (NiUInt32 uj = 0; uj < kStream.GetObjectCount(); uj++)
                {
                    if (kStream.GetObjectAt(uj) == pkObj)
                    {
                        bAlreadyStreamed = true;
                        break;
                    }
                }
                if (!bAlreadyStreamed)
                    kStream.InsertObject(pkObj);
            }
        }
    }

    if (!kStream.Save(strFilePath))
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
const char* NifExportPlugin::GetFileDescriptorString()
{
    return "Gamebryo NIF Files (*.nif)";
}
//---------------------------------------------------------------------------
const char* NifExportPlugin::GetFileTypeString()
{
    return "*.nif";
}
//---------------------------------------------------------------------------
void NifExportPlugin::InsertCameras(NiAVObject* pkObj, NiStream* pkStream)
{
    if (pkObj == NULL)
        return;

    if (pkStream == NULL)
        return;

    if (NiIsKindOf(NiCamera, pkObj))
    {
        pkStream->InsertObject(pkObj);
    }
    else if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
            InsertCameras(pkNode->GetAt(ui), pkStream);
    }
}
//---------------------------------------------------------------------------
