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

#include "NiExportPlugin.h"
#include "NiExportPluginInfo.h"

NiImplementRTTI(NiExportPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiExportPlugin::NiExportPlugin(const char* pcName, const char* pcVersion,
    const char* pcShortDescription, const char* pcLongDescription) :
    NiPlugin(pcName, pcVersion, pcShortDescription, pcLongDescription)
{
}
//---------------------------------------------------------------------------
bool NiExportPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (NiIsKindOf(NiExportPluginInfo, pkInfo))
        return CanExport((NiExportPluginInfo*) pkInfo);
    else
        return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiExportPlugin::Execute(const NiPluginInfo* pkInfo)
{
    if (NiIsKindOf(NiExportPluginInfo, pkInfo))
        return Export((NiExportPluginInfo*) pkInfo);
    else
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
}
//---------------------------------------------------------------------------
bool NiExportPlugin::IsPrimaryExportPlugin()
{
    return true;
}
//---------------------------------------------------------------------------
