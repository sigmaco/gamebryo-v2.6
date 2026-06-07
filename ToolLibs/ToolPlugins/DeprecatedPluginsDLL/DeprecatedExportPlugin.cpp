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
#include "DeprecatedExportPlugin.h"

// RTTI implementation macro.
NiImplementRTTI(DeprecatedExportPlugin, NiExportPlugin);

//---------------------------------------------------------------------------
DeprecatedExportPlugin::DeprecatedExportPlugin(const char* pcName) : 
    NiExportPlugin(pcName, "0.0",  NDL_DEPRECATED_PLUGIN_STRING,
        NDL_DEPRECATED_PLUGIN_STRING)
{
}
//---------------------------------------------------------------------------
NiPluginInfo* DeprecatedExportPlugin::GetDefaultPluginInfo()
{
    return NULL;
}
//---------------------------------------------------------------------------
bool DeprecatedExportPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedExportPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    NiMessageBox(GetLongDescription(), GetName());
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedExportPlugin::CanExport(NiExportPluginInfo* pkInfo)
{
    if (m_strName.EqualsNoCase(pkInfo->GetName()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr DeprecatedExportPlugin::Export(
    const NiExportPluginInfo*)
{
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool DeprecatedExportPlugin::CanCreateDefaultInstance()
{
    return false;
}
//---------------------------------------------------------------------------
const char* DeprecatedExportPlugin::GetFileDescriptorString()
{
    // Return the name of this file type. This is used by file selection
    // dialogs.
    return "My Files (*.myfile)";
}
//---------------------------------------------------------------------------
const char* DeprecatedExportPlugin::GetFileTypeString()
{
    return "*.deprecated";
}
//---------------------------------------------------------------------------
