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
#include "DeprecatedViewerPlugin.h"

// RTTI implementation macro.
NiImplementRTTI(DeprecatedViewerPlugin, NiViewerPlugin);

//---------------------------------------------------------------------------
DeprecatedViewerPlugin::DeprecatedViewerPlugin(const char* pcName) : 
    NiViewerPlugin(pcName, "0.0",  NDL_DEPRECATED_PLUGIN_STRING,
        NDL_DEPRECATED_PLUGIN_STRING)
{
}
//---------------------------------------------------------------------------
NiPluginInfo* DeprecatedViewerPlugin::GetDefaultPluginInfo()
{
    return NULL;
}
//---------------------------------------------------------------------------
bool DeprecatedViewerPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedViewerPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    NiMessageBox(GetLongDescription(), GetName());
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedViewerPlugin::CanView(NiViewerPluginInfo* pkInfo)
{
    if (m_strName.EqualsNoCase(pkInfo->GetName()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr DeprecatedViewerPlugin::View(
    const NiViewerPluginInfo*)
{
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool DeprecatedViewerPlugin::CanCreateDefaultInstance()
{
    return false;
}
//---------------------------------------------------------------------------
bool DeprecatedViewerPlugin::SupportsRenderer(
    NiSystemDesc::RendererID)
{
    return false;
}
//---------------------------------------------------------------------------
