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

#ifndef XSI_GAMEBRYO_EXPORT_PLUGIN_H
#define XSI_GAMEBRYO_EXPORT_PLUGIN_H

#include <xsi_status.h>

//---------------------------------------------------------------------------
// Extrenal references.
//---------------------------------------------------------------------------

namespace XSI
{
    class SICPPSDKDECL PluginRegistrar;
    class SICPPSDKDECL CRef;
}

//---------------------------------------------------------------------------
// XSI Plugin initialization. Registers the menu, properties and commands.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK XSI::CStatus XSILoadPlugin(XSI::PluginRegistrar& in_reg);
XSIPLUGINCALLBACK XSI::CStatus XSIUnloadPlugin(const XSI::PluginRegistrar& in_reg);

//---------------------------------------------------------------------------
// XSI Gamebryo-export custom properties. Defines the properties themselves.
// These are used by the export command below.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK XSI::CStatus ExportGamebryoOptions_Define(const XSI::CRef& in_Ctx);
XSIPLUGINCALLBACK XSI::CStatus ExportGamebryoOptions_DefineLayout(const XSI::CRef& in_Ctx);

//---------------------------------------------------------------------------
// Gamebryo z-buffer properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK XSI::CStatus GamebryoZBuffer_Define(const XSI::CRef& in_Ctx);
XSIPLUGINCALLBACK XSI::CStatus GamebryoZBuffer_DefineLayout(const XSI::CRef& in_Ctx);

//---------------------------------------------------------------------------
// Gamebryo level-of-details properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK XSI::CStatus GamebryoLOD_Define(const XSI::CRef& in_Ctx);
XSIPLUGINCALLBACK XSI::CStatus GamebryoLOD_DefineLayout(const XSI::CRef& in_Ctx);

//---------------------------------------------------------------------------
// Gamebryo lighting properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK XSI::CStatus GamebryoLighting_Define(const XSI::CRef& in_Ctx);
XSIPLUGINCALLBACK XSI::CStatus GamebryoLighting_DefineLayout(const XSI::CRef& in_Ctx);

//---------------------------------------------------------------------------
// Gamebryo shadow properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK XSI::CStatus GamebryoShadow_Define(const XSI::CRef& in_Ctx);
XSIPLUGINCALLBACK XSI::CStatus GamebryoShadow_DefineLayout(const XSI::CRef& in_Ctx);

//---------------------------------------------------------------------------
// Gamebryo export menu. Adds a menu and handles the callback.
// The callback simply calls the export command below.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK XSI::CStatus GamebryoSeparatorMenu_Init(XSI::CRef& in_context);
XSIPLUGINCALLBACK XSI::CStatus ExportGamebryoMenu_Init(const XSI::CRef& in_context);
XSIPLUGINCALLBACK XSI::CStatus OnExportGamebryoMenu(XSI::CRef& in_ref);

//---------------------------------------------------------------------------
// Gamebryo export command. Uses the Gamebryo-export custom properties.
// This is where the real export action is.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK XSI::CStatus ExportGamebryo_Init(const XSI::CRef& in_context);
XSIPLUGINCALLBACK XSI::CStatus ExportGamebryo_Execute(XSI::CRef& in_context);

//---------------------------------------------------------------------------
// Gamebryo-export custom properties creation command.
// This is provided solely for the benefits of script authors, so that
// the properties can be created without showing a dialog.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK XSI::CStatus CreateExportGamebryoOptions_Init(const XSI::CRef& in_context);
XSIPLUGINCALLBACK XSI::CStatus CreateExportGamebryoOptions_Execute(XSI::CRef& in_context);

//---------------------------------------------------------------------------
// Gamebryo version command. reports version info to user.
// This is provided solely for the benefits of script authors, so that
// the version of the plugin, Corsswalk and Gamebryo can be queried.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK XSI::CStatus GamebryoVersion_Init(const XSI::CRef& in_context);
XSIPLUGINCALLBACK XSI::CStatus GamebryoVersion_Execute(XSI::CRef& in_context);

//---------------------------------------------------------------------------

#endif /* XSI_GAMEBRYO_EXPORT_PLUGIN_H */
