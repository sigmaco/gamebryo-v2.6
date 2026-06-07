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

//
// MyiInitPlugin.cpp
// =================
//
//      Handles the (un)initialization of the Plugin.  
//

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "GetShadowTechniquesCommand.h"
#include "GetDefaultShadowBias.h"
#include "GetPackingProfiles.h"
#include "ReloadPackingProfiles.h"
#include "GetProfileDescription.h"
#include "ExportScriptCommands.h"
#include "NiMultiShader.h"

#include <NiInstancingUtilities.h>
#include <NiFramework.h>

#include "MyiInitPlugin.h"

//CODEBLOCK(1) - DO NOT DELETE THIS LINE

//  ========================================================================
//  The following string is used to define the version number to the Maya
//  plug-ins Manager.  Change this whenever the translator is modified.
//  The algorithm for the number is "Gamebryo + 5". Last changed 02/28/06.
//  Also change line 60 in NifExportPlugin::NifExportPlugin() in MyiPlugin.cpp.
char    *pcMyImmerseVersion = "7.3";
int     iInfinity;
int     iState;

//---------------------------------------------------------------------------
static MyiInitPlugin* s_pkMyiInitPlugin = NULL;
//---------------------------------------------------------------------------
MyiInitPlugin::MyiInitPlugin()
{
    bool bNeedsShutdown = false;
    if (!NiShadowManager::GetShadowManager())
    {
        NiShadowManager::Initialize();
        bNeedsShutdown = true;
    }

    for (NiUInt32 ui = 0; ui < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES;
        ui++)
    {
        NiShadowTechnique* pkShadowTechnique =
            NiShadowManager::GetKnownShadowTechnique(
            (unsigned short)ui);
        if (pkShadowTechnique)
        {
            m_apShadowTechniqueNames[ui] = 
                NiStrdup(pkShadowTechnique->GetName());

            for (NiUInt32 uiLight=0; uiLight < NiStandardMaterial::LIGHT_MAX;
                uiLight++)
            {
                m_aafShadowTechniqueBias[ui][uiLight] = 
                    pkShadowTechnique->GetDefaultDepthBias(
                        (NiStandardMaterial::LightType)uiLight, true);
            }
        }
        else
        {
            m_apShadowTechniqueNames[ui] = NULL;

            for (NiUInt32 uiLight=0; uiLight < NiStandardMaterial::LIGHT_MAX;
                uiLight++)
            {
                m_aafShadowTechniqueBias[ui][uiLight] = 0.0f;
            }
        }
    }

    if (bNeedsShutdown)
    {
        NiShadowManager::Shutdown();
    }
}
//---------------------------------------------------------------------------
MyiInitPlugin::~MyiInitPlugin()
{
    for (NiUInt32 ui = 0; ui < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES;
        ui++)
    {
        NiFree(m_apShadowTechniqueNames[ui]);
        m_apShadowTechniqueNames[ui] = NULL;
    }
}
//---------------------------------------------------------------------------
void MyiInitPlugin::Init()
{
    NIASSERT(!s_pkMyiInitPlugin);
    s_pkMyiInitPlugin = NiNew MyiInitPlugin;
}
//---------------------------------------------------------------------------
void MyiInitPlugin::Shutdown()
{
    NiDelete s_pkMyiInitPlugin;
    s_pkMyiInitPlugin = NULL;
}
//---------------------------------------------------------------------------
const char* MyiInitPlugin::GetShadowTechniqueName(
    const int iTechniqueIndex)
{
    NIASSERT(s_pkMyiInitPlugin);
    return s_pkMyiInitPlugin->m_apShadowTechniqueNames[iTechniqueIndex];
}
//---------------------------------------------------------------------------
float MyiInitPlugin::GetShadowTechniqueBias(const int iTechniqueIndex, 
    const int iLightType)
{
    NIASSERT(s_pkMyiInitPlugin);
    return s_pkMyiInitPlugin->m_aafShadowTechniqueBias[iTechniqueIndex]
        [iLightType];
}
//---------------------------------------------------------------------------
void fnCallBeforeExport( void* clientData )
{
    NI_UNUSED_ARG(clientData);

    MString MayaScriptString;
    MCommandResult MayaCommandResult;
    MStatus MayaStatus;

    // Save user preferences
    MayaScriptString = "undoInfo -q -infinity";
    MayaStatus = MGlobal::executeCommand(MayaScriptString, MayaCommandResult, 
        false, false);
    MayaCommandResult.getResult(iInfinity);
    
    MayaScriptString = "undoInfo -q -state";
    MayaStatus = MGlobal::executeCommand(MayaScriptString, MayaCommandResult, 
        false, false);
    MayaCommandResult.getResult(iState);

    // Turn the undo queue ON and to INFINITE
    // For some reason - turning undo on doesn't actually work
    // even though turning infinity on does and it returns a OK message...?
    MString MayaScriptString2;
    MayaScriptString2 = "undoInfo -state 1 -infinity 1";
    MayaStatus = MGlobal::executeCommand(MayaScriptString2,false,false);

//MayaScriptString = "undoInfo -infinity on";
//MayaStatus = MGlobal::executeCommand(MayaScriptString, MayaCommandResult,
    //  false, false);

    if (MayaStatus == MS::kSuccess)
    {
        MStatus pstat;
        MString Mstring;

        if (MayaCommandResult.resultType(&pstat) == MCommandResult::kInt)
        {
        }
    }
}
//---------------------------------------------------------------------------
MStatus initializePlugin (MObject obj)
{
//CODEBLOCK(2) - DO NOT DELETE THIS LINE

    MStatus status;
    char acVersion[512];

    NiSprintf( acVersion, 512, 
        "Gamebryo Version:%s\n Exporter Plugin Version:%s\n", 
        GAMEBRYO_SDK_VERSION_STRING, pcMyImmerseVersion);

    MFnPlugin plugin (obj, "NIF Translator for Maya", acVersion, "Any");

    //register commands that the exporter plug-in is dependent on.
    status = plugin.registerCommand("OpenSelectScriptWindow", 
        OpenSelectScriptWindow::creator);
    if (!status)
    {
        status.perror ("registerCommand OpenSelectScriptWindow");
        return status;
    }

    status = plugin.registerCommand("GetGamebryoRunSilently", 
        GetGamebryoRunSilently::creator);
    if (!status)
    {
        status.perror ("registerCommand GetGamebryoRunSilently");
        return status;
    }

    status = plugin.registerCommand("SetGamebryoRunSilently", 
        SetGamebryoRunSilently::creator);
    if (!status)
    {
        status.perror ("registerCommand SetGamebryoRunSilently");
        return status;
    }

    status = plugin.registerCommand("GetGamebryoUseLastSavedScript", 
        GetGamebryoUseLastSavedScript::creator);
    if (!status)
    {
        status.perror ("registerCommand GetGamebryoUseLastSavedScript");
        return status;
    }

    status = plugin.registerCommand("SetGamebryoUseLastSavedScript", 
        SetGamebryoUseLastSavedScript::creator);
    if (!status)
    {
        status.perror ("registerCommand SetGamebryoUseLastSavedScript");
        return status;
    }

    status = plugin.registerCommand("GetGamebryoForceDefaultScript", 
        GetGamebryoForceDefaultScript::creator);
    if (!status)
    {
        status.perror ("registerCommand GetGamebryoForceDefaultScript");
        return status;
    }

    status = plugin.registerCommand("SetGamebryoForceDefaultScript", 
        SetGamebryoForceDefaultScript::creator);
    if (!status)
    {
        status.perror ("registerCommand SetGamebryoForceDefaultScript");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerCommand("GetPackingProfiles", 
        GetPackingProfiles::creator);

    if (!status)
    {
        status.perror ("registerCommand GetPackingProfiles");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerCommand("ReloadPackingProfiles", 
        ReloadPackingProfiles::creator);

    if (!status)
    {
        status.perror ("registerCommand ReloadPackingProfiles");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerCommand("GetProfileDescription", 
        GetProfileDescription::creator);

    if (!status)
    {
        status.perror ("registerCommand GetProfileDescription");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerFileTranslator ("Nif","MyImmerseIcon1.bmp",
        NifExportPlugin::creator,"MyImmerseGUI",
        NULL,false /*true*/);

    if (!status)
    {
        status.perror ("registerFileTranslator");
        return status;
    }
    else
    {
        //initialize the export log
        MString exportPath = plugin.loadPath();

        exportPath += "\\exportLog.xml";

        NiXMLLogger* pkLogger = MyImmerse::InitXMLLogger();
        pkLogger->CreateLog(exportPath.asChar());
    }

    //Register the translator with the system
    status = plugin.registerCommand("GetShadowTechniques", 
        GetShadowTechniques::creator);

    if (!status)
    {
        status.perror ("registerCommand GetShadowTechniques");
        return status;
    }

    //Register the translator with the system
    status = plugin.registerCommand("GetDefaultShadowBias", 
        GetDefaultShadowBias::creator);

    if (!status)
    {
        status.perror ("registerCommand GetDefaultShadowBias");
        return status;
    }

    // Set the clone to be exact
    NiObjectNET::SetDefaultCopyType(NiObjectNET::COPY_EXACT);

        const MString UserClassify( "shader/surface" );
    MString command( "if( `window -exists createRenderNodeWindow` )  "
        "{refreshCreateRenderNodeWindow(\"" );
    
    char szVersion[256];

    NiSprintf(szVersion, 256, "NiMultiShader Version:7.0\n");

    plugin.registerNode( "NiMultiShader", NiMultiShader::id, 
        NiMultiShader::creator, NiMultiShader::initialize,
        MPxNode::kDependNode, &UserClassify );
    command += UserClassify;
    command += "\");}\n";
    
    MGlobal::executeCommand(command);

    return status;
}
//---------------------------------------------------------------------------
MStatus uninitializePlugin (MObject obj)
{
    MStatus         status;
    MFnPlugin       plugin (obj);

    status = plugin.deregisterFileTranslator ("Nif");
    if (!status)
        status.perror ("deregisterFileTranslator");

    status = plugin.deregisterCommand("GetShadowTechniques");
    if (!status)
        status.perror ("deregisterGetShadowTechniques");

    status = plugin.deregisterCommand("GetDefaultShadowBias");
    if (!status)
        status.perror ("deregisterGetDefaultShadowBias");

    status = plugin.deregisterCommand("GetPackingProfiles");
    if (!status)
        status.perror ("deregisterGetPackingProfiles");

    status = plugin.deregisterCommand("ReloadPackingProfiles");
    if (!status)
        status.perror ("deregisterReloadPackingProfiles");

    status = plugin.deregisterCommand("GetProfileDescription");
    if (!status)
        status.perror ("deregisterGetProfileDescription");

    status = plugin.deregisterCommand("OpenSelectScriptWindow");
    if (!status)
        status.perror ("deregisterOpenSelectScriptWindow");

    status = plugin.deregisterCommand("GetGamebryoRunSilently");
    if (!status)
        status.perror ("deregisterGetGamebryoRunSilently");

    status = plugin.deregisterCommand("SetGamebryoRunSilently");
    if (!status)
        status.perror ("deregisterSetGamebryoRunSilently");

    status = plugin.deregisterCommand("GetGamebryoUseLastSavedScript");
    if (!status)
        status.perror ("GetGamebryoUseLastSavedScript");

    status = plugin.deregisterCommand("SetGamebryoUseLastSavedScript");
    if (!status)
        status.perror ("SetGamebryoUseLastSavedScript");

    status = plugin.deregisterCommand("GetGamebryoForceDefaultScript");
    if (!status)
        status.perror ("GetGamebryoForceDefaultScript");

    status = plugin.deregisterCommand("SetGamebryoForceDefaultScript");
    if (!status)
        status.perror ("SetGamebryoForceDefaultScript");

    const MString UserClassify( "shader/surface" );
    MString command( "if( `window -exists createRenderNodeWindow` )  "
    "{refreshCreateRenderNodeWindow(\"" );

    plugin.deregisterNode( NiMultiShader::id );

    command += UserClassify;
    command += "\");}\n";

    MGlobal::executeCommand(command);

    NiXMLLogger* pkLogger = MyImmerse::GetXMLLogger();
    pkLogger->CloseLog();
    MyImmerse::DestroyXMLLogger();

    NiStaticDataManager::Shutdown();

    return status;
}
//---------------------------------------------------------------------------
