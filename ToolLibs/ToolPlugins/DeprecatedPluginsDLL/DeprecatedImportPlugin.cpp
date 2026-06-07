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
#include "DeprecatedImportPlugin.h"

// RTTI implementation macro.
NiImplementRTTI(DeprecatedImportPlugin, NiImportPlugin);

//---------------------------------------------------------------------------
DeprecatedImportPlugin::DeprecatedImportPlugin(const char* pcName) : 
    NiImportPlugin(pcName, "0.0",  NDL_DEPRECATED_PLUGIN_STRING,
        NDL_DEPRECATED_PLUGIN_STRING)
{
}
//---------------------------------------------------------------------------
NiPluginInfo* DeprecatedImportPlugin::GetDefaultPluginInfo()
{
    return NULL;
}
//---------------------------------------------------------------------------
bool DeprecatedImportPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedImportPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
   NiMessageBox(GetLongDescription(), GetName());
   return true;
}
//---------------------------------------------------------------------------
bool DeprecatedImportPlugin::CanImport(NiImportPluginInfo* pkInfo)
{
    if (m_strName.EqualsNoCase(pkInfo->GetName()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr DeprecatedImportPlugin::Import(
    const NiImportPluginInfo*)
{
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
const char* DeprecatedImportPlugin::GetFileDescriptorString()
{
    // Return the name of this file type. This is used by file selection
    // dialogs.
    return "My Files (*.myfile)";
}
//---------------------------------------------------------------------------
bool DeprecatedImportPlugin::CanCreateDefaultInstance()
{
    return false;
}
//---------------------------------------------------------------------------
const char* DeprecatedImportPlugin::GetFileTypeString()
{
    // Return the extension used by this import plug-in.
    return "*.myfile";
}
//---------------------------------------------------------------------------
