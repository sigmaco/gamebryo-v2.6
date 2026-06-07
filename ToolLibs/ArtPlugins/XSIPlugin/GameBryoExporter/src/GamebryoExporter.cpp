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

#include "GamebryoExporter.h"
#include "GamebryoExporterHelpers.h"
#include "GamebryoExporterStrings.h"

#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoInit.h"

#include <xsi_application.h>
#include <xsi_pluginregistrar.h>

using namespace XSI; 

//---------------------------------------------------------------------------
// XSI Plugin initialization. Registers the menu, properties and commands.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK CStatus XSILoadPlugin(PluginRegistrar& in_reg)
{
    in_reg.PutAuthor(L"Emergent Game Technologies");
    in_reg.PutName(XSI_GAMEBRYO_EXPORT_PLUGIN_NAME);
    in_reg.PutEmail(L"info@emergent.net");
    in_reg.PutURL(L"http://www.Gamebryo.com");
    in_reg.PutVersion(
        XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_MAJOR,
        XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_MINOR);
    //RegistrationInsertionPoint - do not remove this line

    // Register export menu.
    in_reg.RegisterMenu(siMenuMainFileCrosswalkID, L"GamebryoSeparatorMenu", false, false);
    in_reg.RegisterMenu(siMenuMainFileCrosswalkID, L"ExportGamebryoMenu", false, false);

    // Register export command.
    in_reg.RegisterCommand(L"ExportGamebryo", L"ExportGamebryo");

    // Register the properties for export.
    // Register command for option custom property set creation.
    in_reg.RegisterProperty(XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W);
    in_reg.RegisterCommand(L"CreateExportGamebryoOptions", L"CreateExportGamebryoOptions");

    // Register the properties for z-bufering.
    in_reg.RegisterProperty(XSI_GAMEBRYO_ZBUFFER_PROPERTY_NAME_W);
    in_reg.RegisterCommand(L"CreateGamebryoZBuffer", L"CreateGamebryoZBuffer");

    // Register the properties for level-of-details.
    in_reg.RegisterProperty(XSI_GAMEBRYO_LOD_PROPERTY_NAME_W);
    in_reg.RegisterCommand(L"CreateGamebryoLOD", L"CreateGamebryoLOD");
    
    // Register the properties for lighting and shadow.
    in_reg.RegisterProperty(XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME_W);
    in_reg.RegisterCommand(L"CreateGamebryoLighting", L"CreateGamebryoLighting");

    in_reg.RegisterProperty(XSI_GAMEBRYO_SHADOW_PROPERTY_NAME_W);
    in_reg.RegisterCommand(L"CreateGamebryoShadow", L"CreateGamebryoShadow");

    // Register command to return version number
    in_reg.RegisterCommand(L"GamebryoVersion", L"GamebryoVersion");

    return CStatus::OK;
}
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus XSIUnloadPlugin(const PluginRegistrar& in_reg)
{
    epg::ShutdownGamebryo(XSI_GAMEBRYO_EXPORT_PLUGIN_ASCII_NAME, true);
    return CStatus::OK;
}

//---------------------------------------------------------------------------
