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
#include "CrosswalkGamebryoInit.h"
#include "CrosswalkGamebryoSettings.h"

#include <xsi_context.h>
#include <xsi_customproperty.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_utils.h>
#include <xsi_desktop.h>
#include <xsi_application.h>

#include "NiScriptInfoDialogs.h"
#include "NiScriptInfoSet.h"
#include "NiDefaultScriptReader.h"
#include "NiScriptTemplateManager.h"
#include "NiPluginToolkitDefinitions.h"

#include <NiDefaultScriptWriter.h>
#include <NiDefaultScriptReader.h>

using namespace XSI; 

//---------------------------------------------------------------------------
// helper function.
//---------------------------------------------------------------------------

namespace
{
    //---------------------------------------------------------------------------
    // Translate a label.
    //---------------------------------------------------------------------------
    CString translate(const wchar_t * in_Text)
    {
        return CUtils::Translate(in_Text, XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME);
    }

    //---------------------------------------------------------------------------
    // XSI Gamebryo-export custom properties UI builder.
    //---------------------------------------------------------------------------
    CStatus ExportGamebryoOptions_RebuildLayout(PPGLayout &in_Layout)
    {
        CStatus status = CStatus::OK;
        PPGItem item;

        in_Layout.Clear();

	    in_Layout.AddTab(L"Export");

	    in_Layout.AddGroup(L"Export"); 
        {
	        item = in_Layout.AddItem(
                XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_OUT_FILENAME_SETTING_LABEL), 
                siControlFilePath);
	        item.PutAttribute(siUIFileFilter, XSI_GAMEBRYO_NIF_FILE_TYPES);

            item = in_Layout.AddItem(XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_LOG_FILENAME_SETTING_LABEL), 
                siControlFilePath);
	        item.PutAttribute(siUIFileFilter, XSI_GAMEBRYO_LOG_FILE_TYPES);
        }
	    in_Layout.EndGroup();

	    in_Layout.AddGroup(L"Scripts"); 
        {
            in_Layout.AddRow();
            {
	            item = in_Layout.AddItem(XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W, 
                    translate(XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_LABEL), 
                    siControlFilePath);
	            item.PutAttribute(siUIFileFilter, XSI_GAMEBRYO_SCRIPT_FILE_TYPES);
                item.PutAttribute(siUIFileMustExist, true);
                item.PutAttribute(siUIOpenFile, true);
	            item = in_Layout.AddButton(
                    XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_EDIT, 
                    XSI_GAMEBRYO_EDIT_SCRIPT_LABEL ) ;
            }
            in_Layout.EndRow();

            in_Layout.AddRow();
            {
	            item = in_Layout.AddItem(XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W, 
                    translate(XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_LABEL), 
                    siControlFilePath);
	            item.PutAttribute(siUIFileFilter, XSI_GAMEBRYO_SCRIPT_FILE_TYPES);
                item.PutAttribute(siUIFileMustExist, true);
                item.PutAttribute(siUIOpenFile, true);
	            item = in_Layout.AddButton( 
                    XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_EDIT, 
                    XSI_GAMEBRYO_EDIT_SCRIPT_LABEL ) ;
            }
            in_Layout.EndRow();

            in_Layout.AddRow();
            {
	            item = in_Layout.AddItem(XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME_W, 
                    translate(XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_LABEL), 
                    siControlFilePath);
	            item.PutAttribute(siUIFileFilter, XSI_GAMEBRYO_SCRIPT_FILE_TYPES);
                item.PutAttribute(siUIFileMustExist, true);
                item.PutAttribute(siUIOpenFile, true);
	            item = in_Layout.AddButton( XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_EDIT, 
                    XSI_GAMEBRYO_EDIT_SCRIPT_LABEL ) ;
            }
            in_Layout.EndRow();
        }
	    in_Layout.EndGroup();

	    in_Layout.AddGroup(L"Export Control");
        {
            in_Layout.AddItem(XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_LABEL));
            in_Layout.AddItem(XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_LABEL));
            in_Layout.AddItem(XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_LABEL));
            in_Layout.AddItem(XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_LABEL));
            in_Layout.AddItem(XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_LABEL));

	        CValueArray	platformChoices(12);
            {
                int i = 0;
	            platformChoices[i++] = CString(XSI_GAMEBRYO_GENERIC_PLATFORM_VALUE_LABEL);
                platformChoices[i++] = CValue((LONG)epg::GENERIC_PLATFORM);
	            platformChoices[i++] = CString(XSI_GAMEBRYO_DIRECTX_9_PLATFORM_VALUE_LABEL);
	            platformChoices[i++] = CValue((LONG)epg::DIRECTX_9_PLATFORM);
	            platformChoices[i++] = CString(XSI_GAMEBRYO_DIRECTX_10_PLATFORM_VALUE_LABEL);
	            platformChoices[i++] = CValue((LONG)epg::DIRECTX_10_PLATFORM);
	            platformChoices[i++] = CString(XSI_GAMEBRYO_XBOX_360_PLATFORM_VALUE_LABEL);
	            platformChoices[i++] = CValue((LONG)epg::XBOX_360_PLATFORM);
	            platformChoices[i++] = CString(XSI_GAMEBRYO_PS3_PLATFORM_VALUE_LABEL);
	            platformChoices[i++] = CValue((LONG)epg::PS3_PLATFORM);
                platformChoices[i++] = CString(XSI_GAMEBRYO_WII_PLATFORM_VALUE_LABEL);
	            platformChoices[i++] = CValue((LONG)epg::WII_PLATFORM);
            }
            item = in_Layout.AddEnumControl(XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME_W, 
                platformChoices, 
                XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_LABEL) ;
            // The label is rather long and by default XSI gives too much space
            // to the combo-box so adjust the proportions.
	        item.PutAttribute(siUILabelPercentage, CValue((LONG)70));
            
            item = in_Layout.AddItem(XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_MESH_PROFILE_SETTING_LABEL));
            item.PutAttribute(siUILabelPercentage, CValue((LONG)70));
        }
	    in_Layout.EndGroup();

        in_Layout.AddGroup(L"Animation");
        {
            in_Layout.AddItem(XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_LABEL), 
                siControlNumber);
        }
	    in_Layout.EndGroup();

	    in_Layout.AddGroup(L"Post-export");
        {
            in_Layout.AddItem(XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME_W, 
                translate(XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_LABEL));
        }
	    in_Layout.EndGroup();    

        return status;
    }

    //---------------------------------------------------------------------------
    // XSI Gamebryo z-buffer custom properties UI builder.
    //---------------------------------------------------------------------------
    CStatus GamebryoZBuffer_RebuildLayout(PPGLayout &in_Layout)
    {
        CStatus status = CStatus::OK;
        PPGItem item;

        in_Layout.Clear();

        in_Layout.AddItem(XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_LABEL));
        in_Layout.AddItem(XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_LABEL));

        CValueArray	zFunctionChoices(16);
        {
            int i = 0;
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_LESS_EQUAL_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_GREATER_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_GREATER_EQUAL_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_EQUAL_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_NOT_EQUAL_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_ALWAYS_VALUE_NAME_W);
            zFunctionChoices[i++] = CString(XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_LABEL);
            zFunctionChoices[i++] = CValue( XSI_GAMEBRYO_ZBUFFER_NEVER_VALUE_NAME_W);
        }
        item = in_Layout.AddEnumControl(XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_NAME_W, 
            zFunctionChoices, XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_LABEL) ;
        // The label is rather long and by default XSI gives too much space
        // to the combo-box so adjust the proportions.
        item.PutAttribute(siUILabelPercentage, CValue((LONG)70));

        return status;
    }

    //---------------------------------------------------------------------------
    // XSI Gamebryo LOD custom properties UI builder.
    //---------------------------------------------------------------------------
    CStatus GamebryoLOD_RebuildLayout(PPGLayout &in_Layout)
    {
        CStatus status = CStatus::OK;
        PPGItem item;

        in_Layout.Clear();

        in_Layout.AddItem(XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_LABEL));
        in_Layout.AddItem(XSI_GAMEBRYO_LOD_DISTANCE_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_LOD_DISTANCE_PARAM_LABEL));

        return status;
    }

    //---------------------------------------------------------------------------
    // XSI Gamebryo lighting custom properties UI builder.
    //---------------------------------------------------------------------------
    CStatus GamebryoLighting_RebuildLayout(PPGLayout &in_Layout)
    {
        CStatus status = CStatus::OK;
        PPGItem item;

        in_Layout.Clear();

        in_Layout.AddItem(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_LABEL));

        CValueArray	techniqueChoices(8);
        {
            int i = 0;
            techniqueChoices[i++] = CString(XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_LABEL);
            techniqueChoices[i++] = CValue( XSI_GAMEBRYO_NONE_SHADOW_TECHNIQUE_NAME_W);
            techniqueChoices[i++] = CString(XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_LABEL);
            techniqueChoices[i++] = CValue( XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_NAME_W);
            techniqueChoices[i++] = CString(XSI_GAMEBRYO_VSM_SHADOW_TECHNIQUE_LABEL);
            techniqueChoices[i++] = CValue( XSI_GAMEBRYO_VSM_SHADOW_TECHNIQUE_NAME_W);
            techniqueChoices[i++] = CString(XSI_GAMEBRYO_PCF_SHADOW_TECHNIQUE_LABEL);
            techniqueChoices[i++] = CValue( XSI_GAMEBRYO_PCF_SHADOW_TECHNIQUE_NAME_W);
        }
        item = in_Layout.AddEnumControl(XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME_W, 
            techniqueChoices, XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_LABEL) ;
        // The label is rather long and by default XSI gives too much space
        // to the combo-box so adjust the proportions.
        item.PutAttribute(siUILabelPercentage, CValue((LONG)70));

        return status;
    }

    //---------------------------------------------------------------------------
    // XSI Gamebryo shadow custom properties UI builder.
    //---------------------------------------------------------------------------
    CStatus GamebryoShadow_RebuildLayout(PPGLayout &in_Layout)
    {
        CStatus status = CStatus::OK;
        PPGItem item;

        in_Layout.Clear();

        in_Layout.AddItem(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_LABEL));
        in_Layout.AddItem(XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_LABEL));
        in_Layout.AddItem(XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME_W, 
            translate(XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_LABEL));

        return status;
    }

    //---------------------------------------------------------------------------
    // Open the Gamebryo pipeline plugin option dialog for the given script.
    //---------------------------------------------------------------------------
    void EditGamebryoScriptOptions(CustomProperty& in_Prop, 
        const CString& in_ScriptPropName, 
        const CString& in_ScriptPropDataName,
        const CString& in_ScriptType)
    {
        Application app;

        if (in_ScriptPropName.IsEmpty())
        {
            app.LogMessage(L"Missing script property name.", siErrorMsg);
            return;
        }

        CString scriptName = in_Prop.GetParameterValue(in_ScriptPropName);
        if (scriptName.IsEmpty())
        {
            app.LogMessage(L"No script to edit.", siErrorMsg);
            return;
        }

        HWND parentWindow = (HWND) app.GetDesktop().GetApplicationWindowHandle();
        if (parentWindow == 0)
        {
            app.LogMessage(L"Cannot find application window.", siErrorMsg);
            return;
        }

        epg::InitializeGamebryo(XSI_GAMEBRYO_EXPORT_PLUGIN_ASCII_NAME);
        if (!epg::LoadAllGamebryoPlugins())
        {
            app.LogMessage(L"Gamebryo initialization failed.", siErrorMsg);
            return;
        }

        CString scriptFileName;
        bool bGetScriptPath = GetScriptFullPathname(app, scriptName, scriptFileName);

        NiString kScriptFileName = scriptFileName.GetAsciiString();

        bool bEndsWithScript = false;
        unsigned int uiScriptIndex = kScriptFileName.Find(".script");
        bEndsWithScript = (uiScriptIndex != NiString::INVALID_INDEX &&
            uiScriptIndex == kScriptFileName.Length() - 7);

        if (!bGetScriptPath && bEndsWithScript)
        {
            app.LogMessage(L"Cannot find the script.", siErrorMsg);
            return;
        }

        // Load the script.
        NiScriptInfoPtr spInfo = NULL;
        if (bGetScriptPath)
        {
            spInfo = NiScriptTemplateManager::OpenScript(
                scriptFileName.GetAsciiString());
        }
        else if (!in_Prop.GetParameterValue(in_ScriptPropDataName).IsEmpty())
        {
            NiDefaultScriptReader kReader;
            NiString strErrors;
            CString str = in_Prop.GetParameterValue(in_ScriptPropDataName);
            spInfo = kReader.Parse(
                str.GetAsciiString(), 
                strErrors, true);
        }

        if (!spInfo)
        {
            app.LogMessage(L"Cannot read the script.", siErrorMsg);
            return;
        }

        NiScriptInfoSet scripSet;
        scripSet.AddScript(spInfo);
        NiScriptInfoPtr spScript = NiScriptInfoDialogs::DoManagementDialog(
            spInfo, &scripSet, parentWindow, in_ScriptType.GetAsciiString());

        if (spScript)
        {
            const NiString& newScriptFileName = spScript->GetFile();
            if (0 != newScriptFileName.CompareNoCase(scriptFileName.GetAsciiString()))
            {
                if (newScriptFileName.Length() != 0)
                {
                    scriptFileName.PutAsciiString(newScriptFileName);
                    in_Prop.PutParameterValue(in_ScriptPropName, scriptFileName);
                    in_Prop.PutParameterValue(in_ScriptPropDataName, "");
                }
                else
                {
                    scriptFileName.PutAsciiString(spScript->GetName());
                    in_Prop.PutParameterValue(in_ScriptPropName, scriptFileName);
                    NiDefaultScriptWriter kWriter;
                    NiString strEncoding = kWriter.Encode(spScript);
                    CString wideEncoding;
                    wideEncoding.PutAsciiString(strEncoding);
                    in_Prop.PutParameterValue(in_ScriptPropDataName, wideEncoding);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
// XSI Gamebryo-export custom properties UI event callback.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus ExportGamebryoOptions_PPGEvent(const CRef & in_Ctx)
{
    PPGEventContext eventContext(in_Ctx);

    const PPGEventContext::PPGEvent eventID = eventContext.GetEventID();
    if (eventID == PPGEventContext::siButtonClicked)
    {
        CValue buttonPressed = eventContext.GetAttribute(L"Button");
        CustomProperty prop = eventContext.GetSource();

        if (buttonPressed.GetAsText() == XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_EDIT)
        {
            EditGamebryoScriptOptions(prop, XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W, 
                XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME_W, L"PROCESS");
        }
        else if (buttonPressed.GetAsText() == XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_EDIT)
        {
            EditGamebryoScriptOptions(prop, XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W, 
                XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME_W, L"EXPORT");
        }
        else if (buttonPressed.GetAsText() == XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_EDIT)
        {	
            EditGamebryoScriptOptions(prop, XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME_W, 
                XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME_W, L"VIEWER");
        }				
    }

    return CStatus::OK;
}

//---------------------------------------------------------------------------
// XSI Gamebryo-export custom properties.
//---------------------------------------------------------------------------
void EnsureExportGamebryoOptionsParametersExistence(CustomProperty& io_Prop)
{
	CValue defaultValue;

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_OUT_FILENAME_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_LOG_FILENAME_SETTING_DESC),
        defaultValue);

    defaultValue = CValue(false);
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_DESC),
        defaultValue);

    defaultValue = CValue(true);
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EXPORT_SHAPE_ANIMS_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EXPORT_SKINNINGS_SETTING_DESC),
        defaultValue);

    defaultValue = CValue(false);
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_IGNORE_ALL_ERRORS_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_DESC),
        defaultValue);

    defaultValue = CValue(0);
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_NAME_W,
        CValue::siInt4,
        translate(XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_DESC),
        defaultValue);

    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_NAME_W,
        CValue::siInt4,
        translate(XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_DESC),
        defaultValue);

    defaultValue = CValue(30.0f);
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME_W,
        CValue::siFloat,
        translate(XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_DESC),
        defaultValue);

    defaultValue = CValue(epg::GENERIC_PLATFORM);
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME_W,
        CValue::siInt4,
        translate(XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_DESC),
        defaultValue);

    defaultValue = CString(L"Default");
    EnsureParameterExists(
        io_Prop,
        XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_MESH_PROFILE_SETTING_DESC),
        defaultValue);
}

//---------------------------------------------------------------------------
// XSI Gamebryo-export custom properties XSI definition callbacks.
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus ExportGamebryoOptions_Define(const CRef & in_Ctx)
{
    CustomProperty prop = Context(in_Ctx).GetSource();
    EnsureExportGamebryoOptionsParametersExistence(prop);
    return CStatus::OK;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus ExportGamebryoOptions_DefineLayout(const CRef& in_Ctx)
{
    // XSI will call this to define the visual appearance of the CustomProperty
    // The layout is shared between all instances of the CustomProperty
    // and is cached.  You can force the code to re-execute by using the 
    // XSIUtils.Refresh feature.

    PPGLayout oLayout = Context(in_Ctx).GetSource() ;
    CStatus status = status = ExportGamebryoOptions_RebuildLayout(oLayout);
    return status;
}

//---------------------------------------------------------------------------
// Gamebryo z-buffer properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoZBuffer_Define(const CRef & in_Ctx)
{
    CustomProperty prop = Context(in_Ctx).GetSource();

	CValue defaultValue;

    // The settings.
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_ZBUFFER_TEST_PARAM_DESC),
        defaultValue);

    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_NAME_W,
        CValue::siBool,
        translate(XSI_GAMEBRYO_ZBUFFER_WRITE_PARAM_DESC),
        defaultValue);

    defaultValue = CValue(XSI_GAMEBRYO_ZBUFFER_LESS_VALUE_NAME_W);
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_ZBUFFER_FUNCTION_PARAM_DESC),
        defaultValue);

    return CStatus::OK;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoZBuffer_DefineLayout(const CRef& in_Ctx)
{
    // XSI will call this to define the visual appearance of the CustomProperty
    // The layout is shared between all instances of the CustomProperty
    // and is cached.  You can force the code to re-execute by using the 
    // XSIUtils.Refresh feature.

    PPGLayout oLayout = Context(in_Ctx).GetSource() ;
    CStatus status = status = GamebryoZBuffer_RebuildLayout(oLayout);
    return status;
}

//---------------------------------------------------------------------------
// Gamebryo level-of-details properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoLOD_Define(const CRef& in_Ctx)
{
    CustomProperty prop = Context(in_Ctx).GetSource();

    // Default values.
	CValue defaultValue;

    // The settings.
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_LOD_GROUP_NAME_PARAM_DESC),
        defaultValue);

    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_LOD_DISTANCE_PARAM_NAME_W,
        CValue::siFloat,
        translate(XSI_GAMEBRYO_LOD_DISTANCE_PARAM_DESC),
        defaultValue);

    return CStatus::OK;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoLOD_DefineLayout(const CRef& in_Ctx)
{
    // XSI will call this to define the visual appearance of the CustomProperty
    // The layout is shared between all instances of the CustomProperty
    // and is cached.  You can force the code to re-execute by using the 
    // XSIUtils.Refresh feature.

    PPGLayout oLayout = Context(in_Ctx).GetSource() ;
    CStatus status = status = GamebryoLOD_RebuildLayout(oLayout);
    return status;
}

//---------------------------------------------------------------------------
// Gamebryo lighting properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoLighting_Define(const CRef& in_Ctx)
{
    CustomProperty prop = Context(in_Ctx).GetSource();

    // Default values.
	CValue defaultValue;

    // The settings.
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_DESC),
        defaultValue);

    defaultValue = CValue(XSI_GAMEBRYO_DEFAULT_SHADOW_TECHNIQUE_NAME_W);
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_DESC),
        defaultValue);

    return CStatus::OK;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoLighting_DefineLayout(const CRef& in_Ctx)
{
    // XSI will call this to define the visual appearance of the CustomProperty
    // The layout is shared between all instances of the CustomProperty
    // and is cached.  You can force the code to re-execute by using the 
    // XSIUtils.Refresh feature.

    PPGLayout oLayout = Context(in_Ctx).GetSource() ;
    CStatus status = status = GamebryoLighting_RebuildLayout(oLayout);
    return status;
}

//---------------------------------------------------------------------------
// Gamebryo shadow properties
//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoShadow_Define(const CRef& in_Ctx)
{
    CustomProperty prop = Context(in_Ctx).GetSource();

    // Default values.
	CValue defaultValue;

    // The settings.
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_DESC),
        defaultValue);

    defaultValue = CValue(L"");
    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_DESC),
        defaultValue);

    EnsureParameterExists(
        prop,
        XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME_W,
        CValue::siString,
        translate(XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_DESC),
        defaultValue);

    return CStatus::OK;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus GamebryoShadow_DefineLayout(const CRef& in_Ctx)
{
    // XSI will call this to define the visual appearance of the CustomProperty
    // The layout is shared between all instances of the CustomProperty
    // and is cached.  You can force the code to re-execute by using the 
    // XSIUtils.Refresh feature.

    PPGLayout oLayout = Context(in_Ctx).GetSource() ;
    CStatus status = status = GamebryoShadow_RebuildLayout(oLayout);
    return status;
}

//---------------------------------------------------------------------------
