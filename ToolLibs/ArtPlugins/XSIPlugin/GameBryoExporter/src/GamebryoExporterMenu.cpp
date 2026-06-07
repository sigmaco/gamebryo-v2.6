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
#include "GamebryoExporterStrings.h"
#include "GamebryoExporterHelpers.h"

#include "CrosswalkGamebryoHelpers.h"

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_menu.h>
#include <xsi_property.h>
#include <xsi_command.h>
#include <xsi_model.h>
#include <xsi_utils.h>

using namespace XSI; 

//---------------------------------------------------------------------------
// Helper function.
//---------------------------------------------------------------------------

namespace
{
    //---------------------------------------------------------------------------
    // Open the inspection window in XSI for the specified object.
    //---------------------------------------------------------------------------
    CValue InspectObj(
        const CString& in_inputobjs,
        const CString& in_keywords,
        const CString& in_title,
        const CValue& in_mode,
        bool in_throw)
    {
        Application app;

	    CValueArray args(5);
	    CValue retval;
	    LONG i(0);

	    args[i++]= in_inputobjs;
	    args[i++]= in_keywords;
	    args[i++]= in_title;
	    args[i++]= in_mode;
	    args[i++]= in_throw;

	    CStatus st = app.ExecuteCommand( L"InspectObj", args, retval );

	    return retval;
    }
    //---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
// Gamebryo export menu. Adds a menu and handles the callback.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK CStatus GamebryoSeparatorMenu_Init(CRef& in_context)
{
    CStatus status = CStatus::OK;
    Context ctx(in_context);

    // Here we add our menu item in the Crosswalk export menu.
    Menu menu(ctx.GetSource());
    MenuItem item;
    menu.AddItem(L" ", siMenuItemSeparator , item);

    return status;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus ExportGamebryoMenu_Init(const CRef& in_context)
{
    CStatus status = CStatus::OK;
    Context ctx(in_context);

    // Here we add our menu item in the Crosswalk export menu.
    Menu menu(ctx.GetSource());
    MenuItem item;
    menu.AddCallbackItem(L"Export to Gamebryo...", L"OnExportGamebryoMenu", item);

    return status;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus OnExportGamebryoMenu(CRef& in_ref)
{
    // Note: for the ExportGamebryo_Init(), see GamebryoExporterExport.cpp.

    CStatus status = CStatus::OK;
    Application app;

    // Gamebryo export custom property.
    Property prop;
    GetExportOptionProperty(prop, CString(), CString());

    // Build our label for the property page.
    // Note: CUtils::Translate() with %s seems buggy... do concatenation ourselves.
    CString label = CUtils::Translate(
        L"Gamebryo Export v",
        XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME);
    label += CUtils::Translate(
        XSI_GAMEBRYO_EXPORT_PLUGIN_VERSION_STRING,
        XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME);
    label += L" -- ";
    label += CUtils::Translate(
        XSI_GAMEBRYO_EXPORT_GAMEBRYO_VERSION_STRING,
        XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME);
    label += L" -- ";
    label += CUtils::Translate(
        XSI_GAMEBRYO_EXPORT_CROSSWALK_VERSION_STRING,
        XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME);

    // Open export property page.
    bool result = InspectObj(
        XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W,
        CValue(),
        label,
        (LONG)siModal,
        false);

    // Do export if confirmed.
    // Takes two arguments: filename and property name.
    // We leave the filename empty so that the filename in the property page is used.
    if (!result)
    {
		CValue      retValue;
		CValueArray	args(2);
		args[0] = L"";
		args[1] = prop.GetFullName();
		app.ExecuteCommand(L"ExportGamebryo", args, retValue);
    }

    return status;
}
//---------------------------------------------------------------------------
