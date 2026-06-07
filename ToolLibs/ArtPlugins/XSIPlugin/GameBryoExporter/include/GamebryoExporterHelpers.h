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

#ifndef XSI_GAMEBRYO_EXPORT_PLUGIN_HELPERS_H
#define XSI_GAMEBRYO_EXPORT_PLUGIN_HELPERS_H

#include <xsi_status.h>
#include <xsi_ref.h>
#include <xsi_Value.h>
#include <xsi_string.h>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

namespace XSI
{
    class SICPPSDKDECL Property;
    class SICPPSDKDECL Light;
    class SICPPSDKDECL SIObject;
    class SICPPSDKDECL CustomProperty;
    class SICPPSDKDECL Application;
}

//---------------------------------------------------------------------------
// Helper functions.
//---------------------------------------------------------------------------

/// Get the current scene filename.
XSI::CString GetCurrentSceneFileName(XSI::Application& app);

/// Complete the given script file name with the proper path to be able to find it.
bool GetScriptFullPathname(
    XSI::Application& app,
    const XSI::CString& scriptFileName,
    XSI::CString& out_ScriptFullPathName);

/// Save default Gamebryo export settings.
void SaveDefaultExportSettings(const XSI::Property& in_Settings);

/// Load default Gamebryo export settings.
void LoadDefaultExportSettings(XSI::Property& io_Settings);

/// Retrieve or create the gamebryo export options.
bool GetExportOptionProperty(
    XSI::Property& io_Property,
    XSI::CString & in_ObjectName,
    XSI::CString & io_ArgOptionName);

/// Ensure that the export property contains all its required parameters.
void EnsureExportGamebryoOptionsParametersExistence(XSI::CustomProperty& io_Prop);

/// Find a sub-object of the proper type.
XSI::CRef FindNestedObject(
    XSI::SIObject& in_Object,
    const XSI::CString& in_Name,
    XSI::siClassID in_Type);

/** Create the necessary custom property on the light ans its associated
    model group.
    @param io_Light the light object that will have its associated models list converted.
    @param out_NumberOfObjects receives the number of associated objects.
    @return true if successful.
*/
bool CreateLightningCustomProperty(XSI::Light& io_Light, int& out_NumberOfObjects);

/// Ensure that a parameter exists, creates it if necessary.
bool EnsureParameterExists(
    XSI::CustomProperty& in_CustomProps,
    const XSI::CString& in_ParamName,
    XSI::CValue::DataType in_Type,
    const XSI::CString& in_ParamDescription,
    XSI::CValue& io_Value,
    bool in_FetchValue = false);

//---------------------------------------------------------------------------
#endif /* XSI_GAMEBRYO_EXPORT_PLUGIN_HELPERS_H */
