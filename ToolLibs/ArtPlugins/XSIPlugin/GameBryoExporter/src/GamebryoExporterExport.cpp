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
#include "CrosswalkGamebryoConverter.h"

#include "XSIImportPluginLogger.h"

#include "NiDevImageConverter.h"
#include "NiHDRReader.h"
#include "NiOpenEXRReader.h"
#include "NiPNGReader.h"

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_menu.h>
#include <xsi_property.h>
#include <xsi_model.h>
#include <xsi_light.h>
#include <xsi_command.h>
#include <xsi_argument.h>
#include <xsi_utils.h>
#include <xsi_group.h>
#include <xsi_griddata.h>

#include <Scene.h>

using namespace XSI; 

//---------------------------------------------------------------------------
// Helpers
//---------------------------------------------------------------------------
namespace
{
    //---------------------------------------------------------------------------
    // Logging with translation.
    //---------------------------------------------------------------------------

    void LogMsg(
        Application & in_App,
        siSeverityType in_Severity,
        const wchar_t * in_Text,
        const CString & in_Arg1 = CString(),
        const CString & in_Arg2 = CString(),
        const CString & in_Arg3 = CString())
    {
        in_App.LogMessage(
            CUtils::Translate(
                in_Text,
                XSI_GAMEBRYO_EXPORT_PLUGIN_DICTIONARY_NAME,
                in_Arg1,
                in_Arg2,
                in_Arg3),
            in_Severity);
    }

    class XSILogger : public epg::Logger
    {
    public:
        XSILogger(Application& in_App)
            : m_App(in_App)
        {
        }

        virtual void StartConversion()
        {
        }

        virtual void EndConversion()
        {
        }

        virtual bool Log(epg::LogLevel in_Level, const char * in_Msg)
        {
            siSeverityType severity;

            switch (in_Level)
            {
                case epg::LOG_DEBUG:     severity = siCommentMsg; break;
                case epg::LOG_INFO:      severity = siInfoMsg;    break;
                case epg::LOG_WARNING:   severity = siWarningMsg; break;
                default:
                case epg::LOG_ERROR:     severity = siErrorMsg;   break;
            }
            
            CString msg;
            msg.PutAsciiString(in_Msg);
            LogMsg(m_App, severity, msg.GetWideString());

            return in_Level < epg::LOG_ERROR;
        }

    private:
        Application& m_App;
    };

    //---------------------------------------------------------------------------
    // Automatically deletes the temporary dotXSI file.
    //---------------------------------------------------------------------------
    class AutoFileDeleter
    {
    public:
        AutoFileDeleter(const CString & in_FileName, bool in_IsTemp)
            : m_FileName(in_FileName)
            , m_IsTemp(in_IsTemp)
        {
        }
        ~AutoFileDeleter()
        {
            if (m_IsTemp)
                ::DeleteFileW(m_FileName.GetWideString());
        }
    protected:
        CString m_FileName;
        bool m_IsTemp;
    };

    //---------------------------------------------------------------------------
    // Find the last slash in a filename.
    // @return the position of the last slash, -1UL if not found.
    //---------------------------------------------------------------------------
    ULONG FindLastSlash(const CString & in_FileName)
    {
        // Note: this function is very ugly because CString API
        //       is very poor.
        for (ULONG i = in_FileName.Length(); i != 0; --i)
            if (in_FileName[i-1] == L'\\')
                return i-1;

        return -1;
    }

    //---------------------------------------------------------------------------
    // Ensure that the given file name has a valid folder to avoid putting
    // output files in the current directory.
    //---------------------------------------------------------------------------
    void EnsureValidFolderName(CString& io_FileName, Application& app)
    {
        if (-1 != FindLastSlash(io_FileName))
            return;

        // Try using the same folder as the scene.
        CString properFolder = GetCurrentSceneFileName(app);
        ULONG lastSlash = -1;
        if (!properFolder.IsEmpty())
            lastSlash = FindLastSlash(properFolder);

        // Note: even with a scene name, it may not have a folder name.
        if (-1 == lastSlash)
        {
            // Note: the API guarantees that the returned path ends with a slash.
            properFolder = app.GetInstallationPath(siUserPath);
            lastSlash = properFolder.Length() - 1;
        }

        // Check again, in case even the user path is empty...!
        if (properFolder.Length() > 0 && -1 != lastSlash)
        {
            NIASSERT(properFolder[lastSlash] == L'\\');

            CString fullName;
            for (ULONG i = 0; i <= lastSlash; ++i)
                fullName += properFolder[i];
            for (ULONG i = 0; i < io_FileName.Length(); ++i)
                fullName += io_FileName[i];
            io_FileName = fullName;
        }
    }

    //---------------------------------------------------------------------------
    // Create the file name for the temporary dotXSI file.
    //---------------------------------------------------------------------------
    CString BuildDotXSIFileName(const CString & in_NifFileName)
    {
        // Note: this function is very ugly because CString API
        //       is very poor.
        ULONG lastSlash = FindLastSlash(in_NifFileName);

        CString dotXSIFileName;
        if (lastSlash != -1)
        {
            for (ULONG i = 0; i <= lastSlash; ++i)
            {
                dotXSIFileName += in_NifFileName[i];
            }
            lastSlash += 1;
        }

        for (ULONG i = lastSlash; i < in_NifFileName.Length(); ++i)
        {
            if (in_NifFileName[i] == L'.')
                dotXSIFileName += L'_';
            else
                dotXSIFileName += in_NifFileName[i];

        }
        dotXSIFileName += L".xsi";
        return dotXSIFileName;
    }

    //---------------------------------------------------------------------------
    // Make sure where at the first frame so that objects are at the correct
    // position. Crosswalk export export current positions, which is wrong when
    // things are animated.
    //---------------------------------------------------------------------------
    bool GotoFirstFrame(Application& app)
    {
        CValueArray	args(0);
	    CValue retValue;
	    CStatus status = app.ExecuteCommand(L"FirstFrame", args, retValue);
        if (CStatus::OK != status)
        {
            LogMsg(app, siErrorMsg, L"Going to the first frame of the scene failed.");
            return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Retrieve or create Crosswalk export properties.
    //---------------------------------------------------------------------------
    bool CreateCrosswalkOptions(
        const CString & dotXSIFileName,
        bool exportSelectionOnly,
        Application & app)
    {
        Model sceneRoot = app.GetActiveSceneRoot();
        Property crosswalkProperty = 
            sceneRoot.GetProperties().GetItem(XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME_W);
        if (!crosswalkProperty.IsValid())
        {
            // Takes two arguments: object name and property name.
            // We leave the object name empty so that the scene root is used.
	        CValueArray	args(2);
            args[0] = L"";
	        args[1] = XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME_W;

	        CValue retValue;
	        CStatus status = app.ExecuteCommand(L"CreateExportCrosswalkOptions", args, retValue);
            if (CStatus::OK != status)
            {
                LogMsg(app, siErrorMsg, L"Creation of Crosswalk export options failed.");
                return false;
            }

            crosswalkProperty = retValue;
        }

        if (!crosswalkProperty.IsValid())
        {
            LogMsg(app, siErrorMsg, L"Cannot retrieve nor create the Crosswalk export property.");
            return false;
        }

        crosswalkProperty.PutParameterValue(
            XSI_CROSSWALK_EXPORT_FILENAME_SETTING_NAME, dotXSIFileName);
        // This is crosswalk 6.0 format.
        crosswalkProperty.PutParameterValue(XSI_CROSSWALK_EXPORT_FORMAT_SETTING_NAME, (LONG) 3);
        // Make sure all polygons are triangulated.
        crosswalkProperty.PutParameterValue(XSI_CROSSWALK_EXPORT_TRIANGULATE_SETTING_NAME, true);
        // We must plot animations because that's how constraints get evaluated.
        // Otherwise some animation would be lost when driven by other animations.
        crosswalkProperty.PutParameterValue(
            XSI_CROSSWALK_EXPORT_PLOT_ANIMATION_SETTING_NAME, true);
        // We want normals if there are some.
        crosswalkProperty.PutParameterValue(
            XSI_CROSSWALK_EXPORT_EXPORT_NORMALS_SETTING_NAME, true);
        // This is linear interpolation. Crosswalk sometimes corrupt the scene without it!
        crosswalkProperty.PutParameterValue(
            XSI_CROSSWALK_EXPORT_INTERPOLATION_SETTING_NAME, (LONG) 2);
        // Support exporting only the selection if requested.
        crosswalkProperty.PutParameterValue(
            XSI_CROSSWALK_EXPORT_SELECTION_SETTING_NAME, exportSelectionOnly);
        // Make sure all paths used in Crosswalk are absolute to simplify finding images.
        crosswalkProperty.PutParameterValue(
            XSI_CROSSWALK_EXPORT_RELATIVE_PATH_SETTING_NAME, false);

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the associated models list of selective lights.
    // Necessary because dotXSI doesn't support selective light...
    //---------------------------------------------------------------------------
    bool CreateLightsCustomProperties(Application & app)
    {
        Model model = app.GetActiveSceneRoot();
        CRefArray lightArray = model.FindChildren(L"", siLightPrimType, CStringArray());
        const LONG count = lightArray.GetCount();
        for (LONG i = 0; i < count; ++i)
        {
            Light light(lightArray.GetItem(i));

            int objectCount = 0;
            if (!CreateLightningCustomProperty(light, objectCount))
            {
                LogMsg(app, 
                    siErrorMsg, 
                    L"Cannot convert light \"%s\" lit objects to a custom property.", 
                    light.GetName());
                return false;
            }

            CValue isInclusive = light.GetParameterValue(L"SelectiveInclusive");
            if (isInclusive.IsEmpty())
                continue;

            if (objectCount > 0 && ! (bool) isInclusive)
            {
                LogMsg(app, 
                    siErrorMsg, 
                    L"Light \"%s\" is using unsupported exclusive mode for selective light.", 
                    light.GetName());
                return false;
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Call Crosswalk export to generate dotXSI file.
    //---------------------------------------------------------------------------
    bool ExportToDotXSI(Application & app)
    {
        // Takes one argument: the property name.
	    CValueArray	args(1);
	    args[0] = XSI_GAMEBRYO_CROSSWALK_EXPORT_OPTIONS_NAME_W;

	    CValue retValue;
	    CStatus status = app.ExecuteCommand(L"ExportCrosswalk", args, retValue);
        if (CStatus::OK != status)
        {
            LogMsg(app, siErrorMsg, L"Intermediary Crosswalk export failed.");
            return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Try to fixup the export script.
    //---------------------------------------------------------------------------
    CString FindGamebryoScript(Application & app, const CString& in_ScriptFileName)
    {
        CString correctScriptFileName;
        if (GetScriptFullPathname(app, in_ScriptFileName, correctScriptFileName))
            return correctScriptFileName;
        else
            return in_ScriptFileName;
    }

    //---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
// Gamebryo export command. Uses the Gamebryo-export custom properties.
// This is where the real export action is.
//---------------------------------------------------------------------------

XSIPLUGINCALLBACK CStatus ExportGamebryo_Init( const CRef& in_context )
{
    CStatus status = CStatus::OK;
    Context ctx(in_context);

    // Gamebryo export argument: output file name.
    Command cmd(ctx.GetSource());
    cmd.EnableReturnValue(true);
    ArgumentArray args = cmd.GetArguments();
    args.Add(L"Filename", L"\0");
    args.Add(L"ExportOptionsPropertyName", XSI_GAMEBRYO_EXPORT_OPTIONS_NAME_W L"\0");

    return status;
}

//---------------------------------------------------------------------------
XSIPLUGINCALLBACK CStatus ExportGamebryo_Execute( CRef& in_context )
{
    Application app;
    Context ctx(in_context);

    epg::InitializeGamebryo(XSI_GAMEBRYO_EXPORT_PLUGIN_ASCII_NAME);

    // Setup the image converter.  
    // It must exist throughout the lifetime of the export.  bsowers 9/9/08
    NiDevImageConverter* pkConverter = NiNew NiDevImageConverter;
    NiImageConverterPtr pkPrevImageConverter = NiImageConverter::GetImageConverter();
    pkConverter->AddReader(NiNew NiOpenEXRReader);
    pkConverter->AddReader(NiNew NiHDRReader);
    pkConverter->AddReader(NiNew NiPNGReader);

    // Note: do this last so that exceptions leave the
    //       original default converter registered.
    NiImageConverter::SetImageConverter(pkConverter);

    // Access the arguments to the command
    CValueArray args = ctx.GetAttribute( L"Arguments" );
    CString		argFileName(args[0]);
    CString		argPropName(args[1]);

    LogMsg(app, siVerboseMsg, L"Gamebryo Export - Using \"%s\".", argPropName);
    Property exportProperty;
    GetExportOptionProperty(exportProperty, CString(), argPropName);

    // Get final export file name.
    if (argFileName.IsEmpty())
    {
        argFileName = exportProperty.GetParameterValue(XSI_GAMEBRYO_OUT_FILENAME_SETTING_NAME_W);
        if (argFileName.IsEmpty())
        {
            LogMsg(app, siErrorMsg, L"No output file name selected for Gamebryo export.");
            return CStatus::Fail;
        }
    }

    // Make sure the file will be put in a valid folder, not in the current directory.
    EnsureValidFolderName(argFileName, app);

    // Build intermediary dotXSI file name. Make sure it will get deleted.
    CString dotXSIFileName = BuildDotXSIFileName(argFileName);
#ifndef _DEBUG
    AutoFileDeleter fileDeleter(dotXSIFileName, true);
#endif

    // Make sure we're at the first frame in XSI to export animations
    // and base poses properly.
    if (!GotoFirstFrame(app))
        return CStatus::Fail;

    // Set the proper Crosswalk export options we need.
    const bool exportSelectionOnly = 
        exportProperty.GetParameterValue(XSI_GAMEBRYO_EXPORT_SELECTION_SETTING_NAME_W);
    if (!CreateCrosswalkOptions(dotXSIFileName, exportSelectionOnly, app))
        return CStatus::Fail;

    // Create custom properties needed to support lighting groups.
    if (!CreateLightsCustomProperties(app))
        return CStatus::Fail;

    // Call the Crosswalk export to have the dotXSI file (with .xsi extension, litterally).
    if (!ExportToDotXSI(app))
        return CStatus::Fail;

    // Setup the pre-conversion and post-conversion settings:
    // asset viewer launching, various Gamebryo pipeline scripts,
    // logging, platform, etc.
    epg::ConverterSettings settings;
    settings.SetLaunchGamebryoAssetViewer(
        exportProperty.GetParameterValue(XSI_GAMEBRYO_LAUNCH_ASSET_VIEWER_SETTING_NAME_W));
    settings.SetGamebryoProcessingScript(
        FindGamebryoScript(
            app, exportProperty.GetParameterValue(
            XSI_GAMEBRYO_PROCESSING_SCRIPT_SETTING_NAME_W).GetAsText()).GetAsciiString());
    settings.SetGamebryoExportingScript(
        FindGamebryoScript(
            app, exportProperty.GetParameterValue(
            XSI_GAMEBRYO_EXPORTING_SCRIPT_SETTING_NAME_W).GetAsText()).GetAsciiString());
    settings.SetGamebryoViewingScript(
        FindGamebryoScript(
            app, exportProperty.GetParameterValue(
            XSI_GAMEBRYO_VIEWING_SCRIPT_SETTING_NAME_W).GetAsText()).GetAsciiString());

    settings.SetGamebryoProcessingScriptEnc(
            ((CString)(exportProperty.GetParameterValue(
            XSI_GAMEBRYO_PROCESSING_SCRIPT_ENC_SETTING_NAME_W))).GetAsciiString());
    settings.SetGamebryoExportingScriptEnc(
            ((CString)(exportProperty.GetParameterValue(
            XSI_GAMEBRYO_EXPORTING_SCRIPT_ENC_SETTING_NAME_W))).GetAsciiString());
    settings.SetGamebryoViewingScriptEnc(
            ((CString)(exportProperty.GetParameterValue(
            XSI_GAMEBRYO_VIEWING_SCRIPT_ENC_SETTING_NAME_W))).GetAsciiString());

    settings.SetLogFileName(
        exportProperty.GetParameterValue(
        XSI_GAMEBRYO_LOG_FILENAME_SETTING_NAME_W).GetAsText().GetAsciiString());
    settings.SetTargetPlatform((epg::TargetPlatform)(LONG)
        exportProperty.GetParameterValue(
        XSI_GAMEBRYO_TARGET_PLATFORM_SETTING_NAME_W));
    settings.SetMeshProfile(exportProperty.GetParameterValue(
        XSI_GAMEBRYO_MESH_PROFILE_SETTING_NAME_W).GetAsText().GetAsciiString());


    // Do the export.
    XSILogger logger(app);
    const bool exportResult = epg::GamebryoScriptedExport(
        dotXSIFileName.GetAsciiString(),
        argFileName.GetAsciiString(),
        settings,
        logger);

    if (!exportResult)
    {
        DeleteFileW(argFileName.GetWideString());
        return CStatus::Fail;
    }

    // Save default settings when successful.
    SaveDefaultExportSettings(exportProperty);

    NiImageConverter::SetImageConverter(pkPrevImageConverter);

    return CStatus::OK;
}
//---------------------------------------------------------------------------
