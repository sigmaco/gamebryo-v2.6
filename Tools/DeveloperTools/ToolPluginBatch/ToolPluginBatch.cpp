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

#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiCommand.h>
#include <NiMain.h>
#include <NiPluginToolkit.h>
#include <NiPortal.h>
#include <NiParticle.h>
#include <NiStandardSharedData.h>
#include <NiSystem.h>
#include <NiVersion.h>
#include <NiMeshProfileProcessor.h>
#include <NiMeshProfileXMLParser.h>
#include <NiGeometryConverter.h>
#include <NiDevImageQuantizer.h>

#include <iostream>
#include <fstream>
using namespace std;

//---------------------------------------------------------------------------
static bool RunScripts(NiCommand& kCmdLine, NiXMLLogger kLog)
{
    // Retrieve the process script (if any - the process script is optional)
    char acProcessScriptName[256];
    NiString strProcessScript;
    if (kCmdLine.String("p", acProcessScriptName, 256))
        strProcessScript = acProcessScriptName;


    // Retrieve the export script name (if any - the export script is
    // optional)
    char acExportScriptName[256];
    NiString strExportScript;
    if (kCmdLine.String("e", acExportScriptName, 256))
        strExportScript = acExportScriptName;

    // Retrieve the import script name (optional)
    NiString strImportScript;
    {
        char acImportScriptName[256];

        if (kCmdLine.String("i", acImportScriptName, 256))
            strImportScript = acImportScriptName;
    }


    // Retrieve a particular renderer choice (optional)
    NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_GENERIC;
    {
        char acRendererName[_MAX_PATH];
        if (kCmdLine.String("r", acRendererName, _MAX_PATH))
        {
            if (NiStricmp(acRendererName, "generic") == 0)
            {
                eRenderer = NiSystemDesc::RENDERER_GENERIC;
            }
            else if (NiStricmp(acRendererName, "dx9") == 0)
            {
                eRenderer = NiSystemDesc::RENDERER_DX9;
            }
            else if (NiStricmp(acRendererName, "d3d10") == 0)
            {
                eRenderer = NiSystemDesc::RENDERER_D3D10;
            }
            else if (NiStricmp(acRendererName, "xenon") == 0)
            {
                eRenderer = NiSystemDesc::RENDERER_XENON;
            }
            else if (NiStricmp(acRendererName, "ps3") == 0)
            {
                eRenderer = NiSystemDesc::RENDERER_PS3;
            }
            else if (NiStricmp(acRendererName, "wii") == 0)
            {
                eRenderer = NiSystemDesc::RENDERER_WII;
            }
            else
            {
                cout << "ERROR: '" << acRendererName << "' is not a valid " \
                    "renderer to specify with the -r switch." << endl <<
                    "Valid options "\
                    "are 'generic', 'dx9', 'd3d10', 'xenon', 'wii', and 'ps3'." << 
                    endl;

                return false;
            }
        }
    }

    // Set the tool mode render id
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(eRenderer);

    // Retrieve the input file name (required)
    NiString strImportFile;
    {
        char acImportFileName[_MAX_PATH];
        if (kCmdLine.Filename(acImportFileName, _MAX_PATH))
        {
            strImportFile = acImportFileName;
        }
        else
        {
            cout << "ERROR: No input file specified" << endl;

            return false;
        }
    }

    // Retrieve the output file name (required)
    NiString strExportFile;
    {
        char acExportFileName[_MAX_PATH];
        if (kCmdLine.Filename(acExportFileName, _MAX_PATH))
        {
            strExportFile = acExportFileName;
        }
        else
        {
            cout << "ERROR: No output file specified" << endl;

            return false;
        }
    }

    NiScriptInfoPtr spProcessScript;
    if (!strProcessScript.IsEmpty())
    {
        spProcessScript = NiScriptTemplateManager::OpenScript(
            strProcessScript, true);

        if (!spProcessScript)
        {
            cout << "ERROR: The process script \"" << strProcessScript 
                << "\" could not be found." << endl;

            return false;
        }
    }

    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();

    pkDataList->Lock();
    {
        pkDataList->DeleteAll();
        // Add an NiExportOptionsSharedData to make export silent
        NiExporterOptionsSharedDataPtr spEOSharedData = 
            NiNew NiExporterOptionsSharedData();
        spEOSharedData->SetWriteResultsToLog(true);
        spEOSharedData->SetXMLLogger(&kLog);
        pkDataList->Insert(spEOSharedData);
    }
    pkDataList->Unlock();

    // Execute the specified scripts to import, process and export the data

    if (strImportScript.IsEmpty())
    {
        // Simply find a matching import plugin and generate a default 
        // Import script for the file.  All properties will be defaulted
        NiString strResult = NiPluginHelpers::ImportAndExecute(
            NiImportPluginInfo::REPLACE, strImportFile);

        if (strResult.IsEmpty())
        {
            cout << "ERROR: Could not import file: \"" << strImportFile
                << "\"" << endl;

            return false;
        }
    } 
    else
    {
        // Use the specified explicit Import script to allow the user to set
        // values for each of the Import plugin's properties
        NiScriptInfoPtr spImportScript;
        spImportScript = NiScriptTemplateManager::OpenScript(
            strImportScript, true);

        if (!spImportScript)
        {
            cout << "ERROR: The import script \"" << strImportScript 
                << "\" could not be found." << endl;

            return false;
        }

        NiImportPluginInfo* pkImport = NULL;

        // Find the Import plugin - the script _must_ contain _exactly_ one
        // Import plugin, or else the app will explicitly fail
        unsigned int i;
        for (i = 0; i < spImportScript->GetPluginSet().GetSize(); i++)
        {
            NiPluginInfo* pkPlugin = spImportScript->GetPluginSet().GetAt(i);

            if (pkPlugin && pkPlugin->GetType().EqualsNoCase("IMPORT"))
            {
                if (pkImport)
                {
                    cout << "ERROR: The import script contained more than"
                        "one import script." << endl;

                    return false;
                }

                pkImport = NiDynamicCast(NiImportPluginInfo, pkPlugin);
            }
        }

        if (!pkImport)
        {
            cout << "ERROR: The import script contained no import plugins"
                << endl;

            return false;
        }

        NiString strPath; 
        NiString strFile; 
        NiString strExtension;
        NiPluginHelpers::ParseFullPath(strImportFile, strPath, strFile,
            strExtension);

        pkImport->SetFile(strFile);
        pkImport->SetExtension(strExtension);
        pkImport->SetDirectory(strPath);
        pkImport->SetImportType(NiImportPluginInfo::REPLACE);

        NiBatchExecutionResultPtr spResult 
            = NiFramework::GetFramework().ExecuteScript(spImportScript);

        if (!spResult->WasSuccessful())
        {
            cout << "ERROR: The import script failed to execute successfully."
                << endl;

            return false;
        }
    }

    // Check if the file was imported correctly
    pkDataList = NiSharedDataList::GetInstance();
    pkDataList->Lock();
    NiSharedData* pkScene = pkDataList->Get(
        NiGetSharedDataType(NiSceneGraphSharedData));
    pkDataList->Lock();

    if (!pkScene)
    {
        cout << "ERROR: Imported file was empty: \"" << strImportFile << "\""
            << endl;

        return false;
    }

    // Create the mesh profile data
    NiMeshProfileProcessor::CreateMeshProfileProcessor();
    NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor();

    // Process script (if selected).
    if (spProcessScript)
    {
        NiBatchExecutionResultPtr spResult 
            = NiFramework::GetFramework().ExecuteScript(spProcessScript);

        if (!spResult->WasSuccessful())
        {
            cout << "ERROR: The process script failed to execute"
                << " successfully." << endl;

            return false;
        }
    }

    // Destroy the mesh profile data
    NiMeshProfileProcessor::DestroyMeshProfileProcessor();

    // Export script
    NiScriptInfoPtr spExportScript;
    if (strExportScript.IsEmpty())
    {
        // Simply find a matching export plugin and generate a default 
        // Export script for the file.  All properties will be defaulted
        NiString strResult = NiPluginHelpers::ExportAndExecute(strExportFile);

        if (strResult.IsEmpty())
        {
            cout << "ERROR: Could not export file: \"" << strExportFile
                << "\"" << endl;

            return false;
        }
    }
    else
    {
        spExportScript 
            = NiScriptTemplateManager::OpenScript(strExportScript, true);

        if (!spExportScript)
        {
            cout << "ERROR: The export script \"" << strExportScript 
                << "\" could not be found." << endl;

            return false;
        }

        // Export script.
        NiScriptInfoDialogs::CompileImportExportInfo(
            strExportFile, spExportScript, false,
            false);
        NiBatchExecutionResultPtr spResult 
            = NiFramework::GetFramework().ExecuteScript(spExportScript);

        if (!spResult->WasSuccessful())
        {
            cout << "ERROR: The export script failed to execute"
                << " successfully." << endl;
            
            return false;
        }
    }

    cout << "SUCCESS: \"" << strImportFile << "\" -> \"" 
        << strExportFile << "\"" << endl;

    return true;
}
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Get plug-in directory.
#if defined(_MSC_VER) && _MSC_VER >= 1400
    char acPluginPath[NI_MAX_PATH];
    unsigned int uiPathLen = 0;
    const char* pcPluginPath = NULL;
    if (getenv_s(&uiPathLen, acPluginPath, NI_MAX_PATH, 
        "EGB_TOOL_PLUGIN_PATH") != 0 || uiPathLen > NI_MAX_PATH)
    {
        acPluginPath[0] = '\0';
    }
    else
    {
        pcPluginPath = acPluginPath;
    }
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    const char* pcPluginPath = getenv("EGB_TOOL_PLUGIN_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (!pcPluginPath)
    {
        cout << "ERROR: The environment variable EGB_TOOL_PLUGIN_PATH "
            "must be defined." << endl;
        return -1;
    }

    // Get the current application directory.
    char acAppPath[_MAX_PATH];
    GetModuleFileName(NULL, acAppPath, _MAX_PATH);
    char* pcPtr = strrchr(acAppPath, '\\');
    NIASSERT(pcPtr);
    pcPtr += 1;
    *pcPtr = '\0';

    // Initialize the tool plugin framework (including Gamebryo)
    NiFramework::InitFramework(acAppPath, "NiPluginToolkit Batch Tool",
        GAMEBRYO_SDK_VERSION_STRING);

    // Make sure we are using tool mode for the system
    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        NiSystemDesc::RENDERER_GENERIC);

    // Initialize the shadow manager.
    NiShadowManager::Initialize();

    // Use the tool time image converter
    NiImageConverter::SetImageConverter(NiDevImageQuantizer::Create());

    // Because this is a tool that may not have loaded all available shaders,
    // don't remove any shaders that can't be loaded when loading assets.
    // This will allow for processing of assets even if their shaders
    // are not currently available.
    NiMaterialLibrary::SetCreateReturnsNullIfMissingFlag(false);

    bool bReturn = true;
    // Limit the scope of all Gamebryo variables within the framework
    {
        // Parse the plugin directory path(s)
        NiStringTokenizer strTok(pcPluginPath);
        while (!strTok.IsEmpty())
        {
            NiString strPath = strTok.GetNextToken(";");
            int iPtr = strPath.FindReverse('\\');
            NIASSERT(iPtr >= 0);
            if ((unsigned int) iPtr < strPath.Length() - 1)
            {
                strPath += '\\';
            }

    #if _MSC_VER == 1310 //VC7.1
            strPath += "Lib\\VC71\\";
    #elif _MSC_VER == 1400 //VC8.0
            strPath += "Lib\\VC80\\";
    #elif _MSC_VER == 1500 //VC9.0
            strPath += "Lib\\VC90\\";
    #else
        #error Unsupported version of Visual Studio
    #endif

            NiPluginManager::GetInstance()->AddPluginDirectory(strPath, true, 
                true);
        }

        // Parse the command line
        NiCommand kCmdLine(argc, argv);

        // Create an output log
        NiXMLLogger kLogger;
        kLogger.CreateLog("GamebryoOutputLog.xml");

        // Ensure geometry conversion if needed.
        NiGeometryConverter::SetAutoGeometryConversion(true);

        // Run the scripts on the desired file
        bReturn = RunScripts(kCmdLine, kLogger);

        // Close the output log
        kLogger.CloseLog();
    }

    // Shutdown the shadow manager.
    NiShadowManager::Shutdown();

    // Shutdown the entire framework (including Gamebryo)
    NiFramework::ShutdownFramework();

    return bReturn ? 0 : -1;
}
//---------------------------------------------------------------------------
