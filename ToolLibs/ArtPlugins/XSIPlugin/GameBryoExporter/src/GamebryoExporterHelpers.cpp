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

#include "GamebryoExporterStrings.h"
#include "GamebryoExporterHelpers.h"

#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoSettings.h"

#include <xsi_application.h>
#include <xsi_model.h>
#include <xsi_property.h>
#include <xsi_group.h>
#include <xsi_light.h>
#include <xsi_plugin.h>
#include <xsi_project.h>
#include <xsi_scene.h>

#include "NiStaticDataManager.h"
#include "NiMain.h"
#include "NiMesh.h"
#include "NiPluginToolkit.h"
#include "NiMeshProfileProcessor.h"
#include "NiMeshProfileXMLParser.h"
#include "NiMaterialToolkit.h"

using namespace XSI; 

namespace
{
    //---------------------------------------------------------------------------
    // Default export settings.
    //
    // Note: we tried using a XSI::Property before but it would not work.
    //       So we use straight-out XSI::CValue instead.
    //       (It didn't work probably because free-floating properties
    //       are not supported by XSI?) 
    //---------------------------------------------------------------------------
    bool     g_ValidDefault;
    epg::ConverterSettings g_DefaultSettings;

    struct SavedDefault
    {
        const wchar_t* propName;
        CValue value;
    }
    defaultSettings[] =
    {
        { XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W,    CValue() },
        { XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W,     CValue() },
        { XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME_W,       CValue() },
        { XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME_W,    CValue() },
        { XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME_W,     CValue() },
        { XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME_W,       CValue() },
        { XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME_W,     CValue() },
        { XSI_GAMEBRYO_EXPORT_CAMERAS_SETTING_NAME_W,       CValue() },
        { XSI_GAMEBRYO_EXPORT_LIGHTS_SETTING_NAME_W,        CValue() },
        { XSI_GAMEBRYO_EXPORT_TEXTURES_SETTING_NAME_W,      CValue() },
        { XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME_W,  CValue() },
        { XSI_GAMEBRYO_EMBED_TEXTURES_SETTING_NAME_W,       CValue() },
        { XSI_GAMEBRYO_TEXTURE_FORMAT_SETTING_NAME_W,       CValue() },
        { XSI_GAMEBRYO_TEXTURE_COMPRESSION_SETTING_NAME_W,  CValue() },
        { XSI_GAMEBRYO_KEYFRAME_RATE_SETTING_NAME_W,        CValue() },
        { XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME_W,         CValue() }
        // Note: make sure that the output file name is not re-used to avoid
        //       overwriting output files by mistake.
        // XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W,        CValue() },
    };

    //---------------------------------------------------------------------------
    // Retrieve the XSI scene file name without its file extension.
    // Return "Untitled" if not found.
    //---------------------------------------------------------------------------
    CString GetSceneNameWithoutExtension()
    {
        Application app;
        CString sRealName = GetCurrentSceneFileName(app);

        // Tokenize the string to find the leaf name (eg: "foo.scn")
        if (!sRealName.IsEmpty())
        {
            // Remove extension ".scn" and save back into sRealName.
            static const wchar_t extension[] = L".scn";
            const size_t extensionLength = wcslen(extension);
            if (sRealName.Length() > extensionLength)
            {
                ULONG nToCopy = sRealName.Length() - extensionLength;
                if (0 == wcscmp(sRealName.GetWideString() + nToCopy, extension))
                {
                    CString sTemp = L"";
                    for (ULONG i = 0 ; i < nToCopy; ++i)
                        sTemp += sRealName[i];
                    sRealName = sTemp;
                }
            }
        }

        if (sRealName.IsEmpty())
            sRealName = L"Untitled";
        
        return sRealName;
    }

    //---------------------------------------------------------------------------
    // Retrieve the filename of the gamebryo plugin dll.
    //---------------------------------------------------------------------------
    CString GetPluginFilename(Application& app)
    {
        CString pluginFilename(L"");

        CRefArray& plugins = app.GetPlugins();
        for (LONG i = 0; i < plugins.GetCount(); ++i)
        {
            CRef& pluginRef = plugins.GetItem(i);
            Plugin plugin(pluginRef);
            if (plugin.IsValid() && plugin.GetName() == XSI_GAMEBRYO_EXPORT_PLUGIN_NAME)
            {
                pluginFilename = plugin.GetFilename();
                break;
            }
        }

        return pluginFilename;
    }

    //---------------------------------------------------------------------------
    // Retrieve the scripts path for the gamebryo plugin.
    // Assumes the plugin is installed as an XSI addon.
    //---------------------------------------------------------------------------
    CString GetPluginScriptsPath(Application& app)
    {
        CString pluginScriptPath(L"");

        CString pluginFilename = GetPluginFilename(app);
        if (!pluginFilename.IsEmpty())
        {
            // The file name is of the form:
            //
            //    C:\XSI\Addons\Application\Plugins\gb.dll
            //
            // And we want:
            //
            //    C:\XSI\Addons\Data\Scripts
            //
            // So we need to remove the last 3 elements of the path
            // and add back "Data\Scripts".
            CStringArray pathElements = pluginFilename.Split(L"\\");
            if (pathElements.GetCount() >= 3)
            {
                pathElements.Resize(pathElements.GetCount() - 3);
                for (LONG i = 0; i < pathElements.GetCount(); ++i)
                {
                    pluginScriptPath += pathElements[i];
                    pluginScriptPath += L"\\";
                }
                pluginScriptPath += L"Data\\Scripts";
            }
        }

        return pluginScriptPath;
    }

    //---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
/// Get the current scene filename.
//---------------------------------------------------------------------------
CString GetCurrentSceneFileName(XSI::Application& app)
{
    // Get the active projects's scene.
    Project project = app.GetActiveProject();
    Scene scene = project.GetActiveScene();

    // Get actual scene name.
    CParameterRefArray params = scene.GetParameters();
    Parameter p = params.GetItem( L"Filename" );
    CString fileName = p.GetValue( double(1) );

    return fileName;
}

//---------------------------------------------------------------------------
// Complete the given script file name with the proper path to be able to find it.
//---------------------------------------------------------------------------
bool GetScriptFullPathname(
    Application& app,
    const CString& scriptFileName,
    XSI::CString& out_ScriptFullPathName)
{
    if (scriptFileName.IsEmpty())
        return false;

    // Try the file name given as-is.
    out_ScriptFullPathName = scriptFileName;
    if (::GetFileAttributesW(out_ScriptFullPathName.GetWideString()) != INVALID_FILE_ATTRIBUTES)
        return true;

    // Extract the file name itself only.
    CString scriptWithoutPath;
    CStringArray pathElements = scriptFileName.Split(L"\\");
    if (pathElements.GetCount() > 0)
        scriptWithoutPath = pathElements[pathElements.GetCount() - 1];
    else
        scriptWithoutPath = scriptFileName;

    // Try in the add-on script directory.
    CString scriptsPath = GetPluginScriptsPath(app);
    if (!scriptsPath.IsEmpty())
    {
        out_ScriptFullPathName = scriptsPath;
        out_ScriptFullPathName += L"\\";
        out_ScriptFullPathName += scriptWithoutPath;
        const DWORD attr = ::GetFileAttributesW(out_ScriptFullPathName.GetWideString());
        if (attr != INVALID_FILE_ATTRIBUTES)
            return true;
    }

    // Try in current directory.
    out_ScriptFullPathName = scriptWithoutPath;
    if (::GetFileAttributesW(out_ScriptFullPathName.GetWideString()) != INVALID_FILE_ATTRIBUTES)
        return true;

    // Not found.
    return false;
}

//---------------------------------------------------------------------------
// Save default Gamebryo export settings.
//---------------------------------------------------------------------------
void SaveDefaultExportSettings(const Property& in_Settings)
{
    for (int i = 0; i < sizeof(defaultSettings)/sizeof(defaultSettings[0]); ++i)
    {
        defaultSettings[i].value = in_Settings.GetParameterValue(defaultSettings[i].propName);
    }
    g_ValidDefault = true;
}

//---------------------------------------------------------------------------
// Load default Gamebryo export settings.
//---------------------------------------------------------------------------
void LoadDefaultExportSettings(Property& io_Settings)
{
    if (g_ValidDefault)
    {
        for (int i = 0; i < sizeof(defaultSettings)/sizeof(defaultSettings[0]); ++i)
        {
            io_Settings.PutParameterValue(defaultSettings[i].propName, defaultSettings[i].value);
        }
    }
}

//---------------------------------------------------------------------------
// Retrieve or create the gamebryo export options.
//---------------------------------------------------------------------------
bool GetExportOptionProperty(
    Property& io_Property,
    CString & in_ObjectName,
    CString & io_ArgOptionName)
{
    // Set default name for the custom property.
    if (io_ArgOptionName.IsEmpty())
        io_ArgOptionName = XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W;

    // Make sure we have an object to set the property on.
    Application app;
    Model sceneRoot = app.GetActiveSceneRoot();
    X3DObject object;
    if (!in_ObjectName.IsEmpty())
        object = sceneRoot.FindChild(in_ObjectName, CString(), CStringArray());
    else
        object = sceneRoot;

    if (!object.IsValid())
        return false;

    // If property doesn't already exists, create it with some defaults.
    io_Property = object.GetProperties().GetItem(io_ArgOptionName);
    if (!io_Property.IsValid())
    {
        io_Property = object.AddProperty(
            XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W, false, io_ArgOptionName);
        LoadDefaultExportSettings(io_Property);

        // Make sure we try to we have a processing script by default.
        CString argFileName = io_Property.GetParameterValue(
            XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W);
        if (argFileName.IsEmpty())
        {
			// Process script name match that of Max and Maya exporters.
            CString scriptFullPathName;
            if (GetScriptFullPathname(app, L"Default Process Script.script", scriptFullPathName))
            {
                io_Property.PutParameterValue(
                    XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W,
                    scriptFullPathName);
            }
        }

        // Make sure we try to we have an exporting script by default.
        argFileName = io_Property.GetParameterValue(XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W);
        if (argFileName.IsEmpty())
        {
			// Export script name match that of Max and Maya exporters.
            CString scriptFullPathName;
            if (GetScriptFullPathname(app, L"Default Export Script.script", scriptFullPathName))
            {
                io_Property.PutParameterValue(
                    XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W,
                    scriptFullPathName);
            }
        }
    }

    // Ensure that all the property parameter exists. This solves the problem that when
    // we add new parameters in new versions, the old proeprty magically gains the new
    // parameters.
    if (io_Property.IsA(siCustomPropertyID))
    {
        CustomProperty& customProp = (CustomProperty&) io_Property;
        EnsureExportGamebryoOptionsParametersExistence(customProp);
    }

    // Make sure we always have an output file name, even if the proerty already existed.
    // It may slightly annoy the user that will always gives a defsult output file name
    // but it's much more likely that the user finds it very convenient.
    CString argFileName = io_Property.GetParameterValue(XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W);
    if (argFileName.IsEmpty())
    {
        argFileName = GetSceneNameWithoutExtension();
        argFileName += L".nif";
        io_Property.PutParameterValue(XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W, argFileName);
    }

    return io_Property.IsValid();
}

//---------------------------------------------------------------------------
// Find a sub0object of the proper type.
//---------------------------------------------------------------------------
CRef FindNestedObject(SIObject& in_Object, const CString& in_Name, siClassID in_Type)
{
    CRefArray children = in_Object.GetNestedObjects();
    for (LONG i = 0; i < children.GetCount(); ++i)
    {
        CRef child = children[i];
        if (child.IsValid())
        {
            SIObject obj(child);
            if (siUnknownClassID == in_Type || child.IsA(in_Type))
            {
                if (obj.GetName() == in_Name)
                {
                    return child;
                }
            }

            child = FindNestedObject(obj, in_Name, in_Type);
            if (child.IsValid())
                return child;
        }
    }

    return CRef();
}

//---------------------------------------------------------------------------
// Create a parameter if needed.
//---------------------------------------------------------------------------
bool EnsureParameterExists(
    CustomProperty& in_CustomProps,
    const CString& in_ParamName,
    CValue::DataType in_Type,
    const CString& in_ParamDescription,
    CValue& io_Value,
    bool in_FetchValue)
{
    Parameter param = in_CustomProps.GetParameter(in_ParamName);
    if (!param.IsValid())
    {
        in_CustomProps.AddParameter(
            in_ParamName,
            in_Type,
            siPersistable,
            in_ParamName,
            in_ParamDescription,
            io_Value,
            param);
    }
    else if (param.GetValue().IsEmpty())
    {
        param.PutValue(io_Value);
    }
    else if (CValue::siString == in_Type && CString(param.GetValue()).IsEmpty())
    {
        param.PutValue(io_Value);
    }
    else if (in_FetchValue)
    {
        io_Value = param.GetValue();
    }

    return param.IsValid();
}

//---------------------------------------------------------------------------
// Create the lightning custom property if needed.
//---------------------------------------------------------------------------
bool EnsureCustomPropertyExists(
    SIObject& io_Object,
    const CString& in_PropName,
    CustomProperty& io_Prop)
{
    if (!io_Object.IsA(siSceneItemID))
        return false;

    SceneItem& item = (SceneItem&) io_Object;
    io_Prop = item.GetProperties().GetItem(in_PropName);
    if (!io_Prop.IsValid())
        io_Prop = item.AddProperty(in_PropName, false, in_PropName);

    return io_Prop.IsValid();
}

//---------------------------------------------------------------------------
// Create the necessary custom property on the light ans its associated
// model group.
//---------------------------------------------------------------------------
bool CreateLightningCustomProperty(Light& io_Light, int& out_NumberOfObjects)
{
    out_NumberOfObjects = 0;

    // Check if the light has associated models. If not, we're done.
    Group associatedModels(FindNestedObject(io_Light, L"Associated Models", siGroupID));
    if (!associatedModels.IsValid())
        return true;

    CRefArray members = associatedModels.GetMembers();
    if (!members.GetCount())
        return true;

    // Find or create the Gamebryo lightning custom property.
    CustomProperty lightning;
    if (!EnsureCustomPropertyExists(io_Light, XSI_GAMEBRYO_LIGHTNING_PROPERTY_NAME_W, lightning))
        return false;

    // Set the group name if not already set.
    CValue groupNames(io_Light.GetName());
    EnsureParameterExists(
        lightning,
        XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W,
        CValue::siString,
        XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_DESC,
        groupNames,
        true);

    // Help users get the parameter name right by creating it if it doesn't exists.
    CValue empty;
    EnsureParameterExists(
        lightning,
        XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_NAME_W,
        CValue::siString,
        XSI_GAMEBRYO_SHADOW_TECHNIQUE_PARAM_DESC,
        empty);

    // Add the light group names to the light group parameter
    // of the lightning custom property of the meshes.
    for (LONG i = 0; i < members.GetCount(); ++i)
    {
        SIObject obj(members[i]);
        CustomProperty meshLightning;
        if (!EnsureCustomPropertyExists(obj, XSI_GAMEBRYO_SHADOW_PROPERTY_NAME_W, meshLightning))
            return false;

        EnsureParameterExists(
            meshLightning,
            XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_NAME_W,
            CValue::siString,
            XSI_GAMEBRYO_LIGHT_GROUPS_PARAM_DESC,
            groupNames);

        // Help users get the parameter name right by creating it.
        EnsureParameterExists(
            meshLightning,
            XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_NAME_W,
            CValue::siString,
            XSI_GAMEBRYO_IGNORED_SHADOW_CASTER_PARAM_DESC,
            empty);

        EnsureParameterExists(
            meshLightning,
            XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_NAME_W,
            CValue::siString,
            XSI_GAMEBRYO_IGNORED_SHADOW_RECEIVERS_PARAM_DESC,
            empty);
    }

    out_NumberOfObjects = (int) members.GetCount();
    return true;
}
