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
#include "MaxImmerse.h"
#include "NiMAXOptions.h"
#include "MaxImmerse422Conversion.h"
#include <NiDefaultScriptWriter.h>
#include <NiDefaultScriptReader.h>
#include <NiPluginToolkitDefinitions.h>

static bool ms_bUseOldBehaviorUserHasBeenWarned = false;
bool NiMAXOptions::ms_bFirst = true;
NiPluginInfo* NiMAXOptions::ms_pkDatabase = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastExportScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastProcessScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastDX9ViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastD3D10ViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastSceneImmerseViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastD3D10SceneImmerseViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastCustomViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastViewportScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastAnimationToolViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastXenonViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastPS3ViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastWiiViewerScript = NULL;
NiScriptInfoPtr NiMAXOptions::ms_spLastPhysXViewerScript = NULL;
NiCriticalSection NiMAXOptions::g_kOptionsCS;
 
#define MAX_VALUE_LENGTH 25000
#define SPACE_REPLACEMENT "_-_"
#define TAB_REPLACEMENT "_\\t_"
#define NEWLINE_REPLACEMENT "_\\n_"
//---------------------------------------------------------------------------
NiString ConvertFromSpacesAndTabs(NiString strIn)
{
    CHECK_MEMORY();
    strIn.Replace(" ", SPACE_REPLACEMENT);
    strIn.Replace("\t", TAB_REPLACEMENT);
    strIn.Replace("\n", NEWLINE_REPLACEMENT);
    CHECK_MEMORY();
    return strIn;
}

//---------------------------------------------------------------------------
NiString ConvertToSpacesAndTabs(NiString strIn)
{
    CHECK_MEMORY();
    strIn.Replace(SPACE_REPLACEMENT, " ");
    strIn.Replace(TAB_REPLACEMENT, "\t");
    strIn.Replace(NEWLINE_REPLACEMENT, "\n");
    CHECK_MEMORY();
    return strIn;
}

//---------------------------------------------------------------------------
//#define LOG_IT_ALL
void EchoNodeOptions(INode* pkNode)
{
    CHECK_MEMORY();
    if (!pkNode)
    {
        NILOGERROR("No INODE found in Echo Node Options");
        return;
    }
    TSTR tBuffer;
    pkNode->GetUserPropBuffer(tBuffer);
#ifdef LOG_IT_ALL
    NiOutputDebugString("***********************************\n");
    NiOutputDebugString("*    ECHOING NODE OPTIONS         *\n");
    NiOutputDebugString("***********************************\n");
    NiOutputDebugString(tBuffer);
    NiOutputDebugString("\n***********************************\n");
//#else
    NiMessageBox(tBuffer, "Echo Node Options");
#endif
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::CreateOptions()
{
    CHECK_MEMORY();
    if (ms_pkDatabase)
    {
        return;
    }
    else
    {
        ms_pkDatabase = NiNew NiPluginInfo();
        ms_spLastExportScript = NULL;
        ms_spLastProcessScript = NULL;
        ms_spLastDX9ViewerScript = NULL;
        ms_spLastD3D10ViewerScript = NULL;
        ms_spLastSceneImmerseViewerScript = NULL;
        ms_spLastD3D10SceneImmerseViewerScript = NULL;
        ms_spLastCustomViewerScript = NULL;
        ms_spLastViewportScript = NULL;
        ms_spLastAnimationToolViewerScript = NULL;
        ms_spLastXenonViewerScript = NULL;
        ms_spLastPS3ViewerScript = NULL;
        ms_spLastWiiViewerScript = NULL;
        ms_spLastPhysXViewerScript = NULL;
        SetDefaultOptions();
        ReadOptions(GetCOREInterface());
        CHECK_MEMORY();
    }
}
//---------------------------------------------------------------------------
void NiMAXOptions::CreateViewerOptions()
{
    SetDefaultViewerOptions();
    ReadOptions(GetCOREInterface());
}
//---------------------------------------------------------------------------
void NiMAXOptions::DestroyOptions()
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);
    WriteOptions(GetCOREInterface());
    ms_spLastExportScript = NULL;
    ms_spLastProcessScript = NULL;
    ms_spLastDX9ViewerScript = NULL;
    ms_spLastD3D10ViewerScript = NULL;
    ms_spLastSceneImmerseViewerScript = NULL;
    ms_spLastD3D10SceneImmerseViewerScript = NULL;
    ms_spLastCustomViewerScript = NULL;
    ms_spLastViewportScript = NULL;
    ms_spLastAnimationToolViewerScript = NULL;
    ms_spLastXenonViewerScript = NULL;
    ms_spLastPS3ViewerScript = NULL;
    ms_spLastWiiViewerScript = NULL;
    ms_spLastPhysXViewerScript = NULL;
    NiDelete ms_pkDatabase;
    ms_pkDatabase = NULL;
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
bool NiMAXOptions::MatchesCurrentVersion(Interface* pkInterface)
{
    CHECK_MEMORY();
    INode* pkNode = pkInterface->GetRootNode();
    if (pkNode)
    {
        int i = 0;
        if(!pkNode->GetUserPropInt(MAXIMMERSE_VERSION_KEY, i))
            return false;

        if (i < LAST_VERSION_AFFECTING_OPTIONS)
            return false;
        else
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetDefaultOptions()
{
    g_kOptionsCS.Lock();
#ifdef LOG_IT_ALL
    NiOutputDebugString("***********************************\n");
    NiOutputDebugString("*    SETTING DEFAULT OPTIONS      *\n");
    NiOutputDebugString("***********************************\n");
#endif
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);
    SetInt(MAXIMMERSE_VERSION_KEY, MAXIMMERSEVERSIONINT );
    SetValue(NI_TEXTURES, NI_KEEP_IN_MAIN_NIF);
    SetBool(NI_USEOLDBEHAVIOR, false); 
    SetBool(NI_USEAREASUPERSPRAY, false); 
    SetBool(NI_USEOLDSPINTIME , false);
    SetBool(NI_USEOLDNORMALS, false);
    SetValue(NI_IMAGE_FILE_DIR , "");
    SetInt(NI_EXPORT_PLATFORM , (int) NiSystemDesc::RENDERER_GENERIC);
    SetBool(NI_CONVERT_CAMERAS, true); 
    SetBool(NI_CONVERT_LIGHTS , true);
    SetBool(NI_USE_APPTIME , true);
    SetBool(NI_CONVERT_CSASKEYFRAMES, false); 
    SetBool(NI_PER_TEXTURE_OVERRIDE , false);
    SetBool(NI_ALLOW_NPOT_TEXTURES, false);
    SetInt(NI_PIXEL_LAYOUT , (int)NiTexture::FormatPrefs::PIX_DEFAULT);
    SetBool(NI_EXTRACT_KEYFRAMES, false); 
    SetBool(NI_EXTRACT_SEPERATE_SEQUENCES, false);
    SetInt(NI_VIEWER_TYPE, (int)MiViewerPlugin::MAX_DX9_VIEWER);
    SetInt(NI_RESX , 640);
    SetInt(NI_RESY, 480);
    SetInt(NI_SKINNING_THRESHOLD, 5);
    SetBool(NI_WINDOWED, true); 
    SetBool(NI_VIEWER_STRIPIFY, true);
    SetBool(NI_USE_TEMPLATE_FOR_SCRIPT, true);
    SetValue(NI_SCENEMESHPROFILENAME, NI_DEFAULTSCENEMESHPROFILENAME);
    SetValue(NI_DX9_VIEWER_SCRIPT, "");
    SetValue(NI_D3D10_VIEWER_SCRIPT, "");
    SetValue(NI_CUSTOM_VIEWER_SCRIPT, "");
    SetValue(NI_VIEWPORT_SCRIPT, "");
    SetValue(NI_ANIMATION_TOOL_SCRIPT, "");
    SetValue(NI_XBOX_360_SCRIPT, "");
    SetValue(NI_PS3_SCRIPT, "");
    SetValue(NI_WII_SCRIPT, "");
    SetValue(NI_PHYSX_SCRIPT, "");
    SetValue(NI_SCENEIMMERSE_SCRIPT, "");
    SetValue(NI_D3D10_SCENEIMMERSE_SCRIPT, "");
    SetValue(NI_EXPORT_SCRIPT, "");
    SetValue(NI_PROCESS_SCRIPT, "");
    SetValue(NI_NETWORK_SCRIPT_PATH, "");
    SetBool(NI_WRITE_ERRORS_TO_FILE, false);
    SetBool(NI_LOG_ERRORS_PROMPT, true);
    SetBool(NI_LAST_ERROR_PROMPT_RETURN, true);
    SetBool(NI_USE_OLD_PARTICLE_EXPORTER, false);

    SetValue(NI_FILENAME, "temp.nif");

#ifdef LOG_IT_ALL
    NiOutputDebugString("***********************************\n");
#endif
    CHECK_MEMORY();
    g_kOptionsCS.Unlock();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetDefaultViewerOptions()
{
    g_kOptionsCS.Lock();
    HandleValue(NI_DX9_VIEWER_SCRIPT_NAME, "Default DX9 Viewer Script");
    HandleValue(NI_D3D10_VIEWER_SCRIPT_NAME, "Default D3D10 Viewer Script");
    HandleValue(NI_EXPORT_SCRIPT_NAME, "Default Export Script");
    HandleValue(NI_PROCESS_SCRIPT_NAME, "Default Process Script");
    HandleValue(NI_CUSTOM_VIEWER_SCRIPT_NAME, "Default Custom Viewer Script");
    HandleValue(NI_SCENEIMMERSE_SCRIPT_NAME,
        "Default SceneImmerse Viewer Script");
    HandleValue(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME,
        "D3D10 SceneImmerse Viewer Script");
    HandleValue(NI_VIEWPORT_SCRIPT_NAME, "Default Viewport Script");
    HandleValue(NI_ANIMATION_TOOL_SCRIPT_NAME, "Default KFMViewer Script");
    HandleValue(NI_XBOX_360_SCRIPT_NAME, "Default Xbox 360 Viewer Script");
    HandleValue(NI_PS3_SCRIPT_NAME, "Default PS3 Viewer Script");
    HandleValue(NI_WII_SCRIPT_NAME, "Default Wii Viewer Script");
    HandleValue(NI_PHYSX_SCRIPT_NAME, "Default PhysX Viewer Script");
    g_kOptionsCS.Unlock();
}
//---------------------------------------------------------------------------
void NiMAXOptions::ReadOptions(Interface* pkInterface)
{
    CHECK_MEMORY();
    g_kOptionsCS.Lock();

#ifndef NIDEBUG 
    try
#endif
    {
#ifdef LOG_IT_ALL
        NiOutputDebugString("-------------------------------------\n");
        NiOutputDebugString("|***********************************|\n");
        NiOutputDebugString("|*    READING OPTIONS              *|\n");
        NiOutputDebugString("|***********************************|\n");
        NiOutputDebugString("-------------------------------------\n");
#endif
        TCHAR acInput[MAX_VALUE_LENGTH];
        NIASSERT(ms_pkDatabase);

        NiScriptTemplateManager::GetInstance();

        NiString strSection = NI_OPTIONS_SECTION;

        TCHAR acFilename[MAX_PATH];
        GetCfgFilename(pkInterface, acFilename, MAX_PATH);

        INode* pkNode = pkInterface->GetRootNode();

        GetPrivateProfileString(strSection, MAXIMMERSE_VERSION_KEY, "", 
            acInput, 1024, acFilename); 

        if (acInput[0] == NULL)
        {
#ifdef LOG_IT_ALL
            NiOutputDebugString("***********************************\n");
            NiOutputDebugString(" INI IS OUT OF DATE... CONVERTING OLD INI"
                ",,,\n");
            NiOutputDebugString("***********************************\n");
#endif

            SetDefaultOptions();
            HandleDeprecatedINIOptions(pkInterface);
            HandleDeprecatedNodeOptions(pkInterface);
            ms_bFirst = false;
            CHECK_MEMORY();

            // To enter this code block, the file mimmerse.ini did not
            // exist.  Create the file on disk.
            WriteOptions(pkInterface);
        }

#ifdef LOG_IT_ALL
        NiOutputDebugString("***********************************\n");
        NiOutputDebugString("Reading the mimmerse.ini file settings....\n");
        NiOutputDebugString("***********************************\n");
        // Read values from the INI file
#endif
        NiString strValue;

        unsigned int ui = 0;
        for (; ui < ms_pkDatabase->GetParameterCount(); ui++)
        {
            NiParameterInfo* pkParam = ms_pkDatabase->GetParameterAt(ui);
            if (pkParam)
            {
                NiString strKey = pkParam->GetKey();

                if (GetPrivateProfileString(strSection, strKey, "", acInput, 
                    MAX_VALUE_LENGTH, acFilename) && acInput[0] != NULL)
                {
                    strValue = ConvertToSpacesAndTabs(acInput);
                    SetValue(strKey, strValue);
                    HandleValue(strKey, strValue);
                }
            }
        }

        NiOutputDebugString("Read INI Options\n");
#ifdef LOG_IT_ALL
        NiOutputDebugString("***********************************\n");
        NiOutputDebugString("Reading the MAX root node's settings....\n");
        NiOutputDebugString("***********************************\n");
#endif

        TSTR strBuf;
        TSTR strNul = "\0";
        pkNode->GetUserPropBuffer(strBuf);

        // Make sure we have user properties to parse
        if (strBuf.length() <= 0)
        {
            g_kOptionsCS.Unlock();
            return;
        }

        strBuf[strBuf.length()-1] = '\0';
        NiOutputDebugString("Got user prop buffer from node.\n");

        strSection = "GamebryoTemp";
        NiString strData = (const char*)strBuf;
        NIASSERT(strBuf.length() == (int)strData.Length());
        NiString strFile = pkInterface->GetDir(APP_PLUGCFG_DIR);
        int iLen = strFile.Length();
        if (iLen)
        {
            if (strFile.GetAt(iLen - 1) != '\\')
                strFile += "\\";
        }
        strFile += "GamebryoTemp.ini";

        {
            NiFile tempFile((const char*)strFile, NiFile::WRITE_ONLY);
            NiString strTempBuf = "[GamebryoTemp]\n" + strData + "\n";
            tempFile.PutS(strTempBuf);
        }

        NiOutputDebugString("Wrote PrivateProfileSection.\n");
        NILOG(NIMESSAGE_GENERAL_0, "strData Length: %d\n", strData.Length());
        CHECK_MEMORY();

        NiOutputDebugString("GamebryoTemp.ini is created.\n");
        // Read values from the node itself
        for (ui = 0; ui < ms_pkDatabase->GetParameterCount(); ui++)
        {
            NiParameterInfo* pkParam = ms_pkDatabase->GetParameterAt(ui);
            if (pkParam)
            {
                NiString strKey = pkParam->GetKey();
                //NiString strOutput = "Key:" + strKey + "\t";

                if (pkNode && SaveInNode(strKey))
                {
                    if (GetPrivateProfileString(strSection, strKey, "",
                        acInput, MAX_VALUE_LENGTH, strFile) && acInput[0] !=
                        NULL)
                    {
                        NiString strValue = ConvertToSpacesAndTabs(acInput);
                        SetValue(strKey, strValue);
                        HandleValue(strKey, strValue);
                    }
                }
            }
        }

        NiOutputDebugString("GamebryoTemp.ini is written.\n");

        // Read deprecated node values
        if (!MatchesCurrentVersion(pkInterface))
        {
#ifdef LOG_IT_ALL
            NiOutputDebugString("***********************************\n");
            NiOutputDebugString("The root node has deprecated tags.. "
                "converting...\n");
            NiOutputDebugString("***********************************\n");
#endif

            // If HandleDeprecatedNodeOptions return true, then
            // we can stop reading options, b/c we have fully loaded
            // them. However, if it returns false, we need to 
            // continue our logic.
            bool bReturn = HandleDeprecatedNodeOptions(pkInterface);
            ms_bFirst = false;
            CHECK_MEMORY();
            NiOutputDebugString("Handled Deprecated Node Options\n");
            if (bReturn)
            {
                g_kOptionsCS.Unlock();
                return;
            }
        }

#ifdef LOG_IT_ALL
        NiOutputDebugString("***********************************\n");
        NiOutputDebugString("-------------------------------------\n");
#endif
        ms_bFirst = false;
        CHECK_MEMORY();
        NiOutputDebugString("End Read Options");
    }
#ifndef NIDEBUG 
    catch (...)
    {
        g_kOptionsCS.Unlock();
        NiOutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        NiOutputDebugString("\nAn exception has occured during reading"
            " options.\n\n");
        NiLogger::FlushAllLogs();
        NiMessageBox("An exception has occured. Press OK to continue", 
            "Options Exception");
        return;
    }
#endif
    g_kOptionsCS.Unlock();
}
//---------------------------------------------------------------------------
void WriteScript(NiScriptInfo* pkInfo)
{
    CHECK_MEMORY();
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    if (pkInfo && pkInfo->IsDirty() && 
        NiMAXOptions::GetValue(NI_USE_TEMPLATE_FOR_SCRIPT) &&
        pkInfo->GetFile().Length() > 0)
    {
        pkManager->WriteScript(pkInfo, pkInfo->GetFile(),
            NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void WriteScripts()
{
    CHECK_MEMORY();
    NiMAXOptions::g_kOptionsCS.Lock();
    NiScriptInfo* pkInfo = NiMAXOptions::GetDX9ViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetD3D10ViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetSceneImmerseViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetCustomViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetProcessScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetExportScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetViewportScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetAnimationViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetXenonViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetPS3ViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetWiiViewerScript();
    WriteScript(pkInfo);
    pkInfo = NiMAXOptions::GetPhysXViewerScript();
    WriteScript(pkInfo);
    NiMAXOptions::g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
// Make sure that ReadOptions is called prior to this method to prime the 
// database.
void NiMAXOptions::WriteOptions(Interface* pkInterface)
{
    CHECK_MEMORY();
    g_kOptionsCS.Lock();
#ifndef NIDEBUG 
    try 
#endif
    {

    #ifdef LOG_IT_ALL
        NiOutputDebugString("***********************************\n");
        NiOutputDebugString("*    WRITING OPTIONS              *\n");
        NiOutputDebugString("***********************************\n");
    #endif
        if (!MatchesCurrentVersion(pkInterface))
        {
            RemoveDeprecatedOptions(pkInterface);
            CHECK_MEMORY();
        }
        // Make sure to set that we are on the latest version of MaxImmerse.
        // Any deprecated options should have been fixed by the prior call to
        // ReadOptions.
        SetInt(MAXIMMERSE_VERSION_KEY, MAXIMMERSEVERSIONINT );

        NIASSERT(ms_pkDatabase);
        NiString strSection = NI_OPTIONS_SECTION;

        TCHAR acFilename[MAX_PATH];
        GetCfgFilename(pkInterface, acFilename, MAX_PATH);

        INode* pkNode = pkInterface->GetRootNode();

        NiOutputDebugString("Begin Writing Scripts...\n");
        WriteScripts();
        NiOutputDebugString("End Writing Scripts...\n");

        for (unsigned int ui = 0; ui < ms_pkDatabase->GetParameterCount();
            ui++)
        {
            NiParameterInfo* pkParam = ms_pkDatabase->GetParameterAt(ui);
            if (pkParam)
            {
                NiString strValue = ConvertFromSpacesAndTabs((const char*)
                    pkParam->GetValue());
                NiString strKey = pkParam->GetKey();
                WritePrivateProfileString(strSection, strKey, 
                    strValue, acFilename);
                TSTR kKey( (const char*)strKey);
                TSTR kValue((const char*) strValue);
                if (pkNode && SaveInNode((const char*) strKey))
                    pkNode->SetUserPropString(kKey, kValue);
            }
        }

    #ifdef LOG_IT_ALL
        NiOutputDebugString("***********************************\n");
    #endif
        CHECK_MEMORY();
    }
#ifndef NIDEBUG 
    catch (...)
    {
        g_kOptionsCS.Unlock();
        NiOutputDebugString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        NiOutputDebugString("\nAn exception has occured during writing "
            "options.\n\n");
        NiLogger::FlushAllLogs();
        NiMessageBox("An exception has occured. Press OK to continue", 
            "Options Exception");
        return;
    }
#endif

    g_kOptionsCS.Unlock();

}
//---------------------------------------------------------------------------
void NiMAXOptions::RemoveOptionsExtraData(NiNode* pkNode)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);
    if(pkNode != NULL)
    {
        pkNode = (NiNode*)pkNode->GetObjectByName("Scene Root");

        if(pkNode == NULL)
            return;

        NiTPrimitiveSet<const char*> kNames;
        unsigned int ui;
        for (ui = 0; ui < pkNode->GetExtraDataSize(); ui++)
        {
            NiExtraData* pkExtraData = pkNode->GetExtraDataAt((unsigned short)ui);
            if (pkExtraData && NiIsKindOf(NiStringExtraData, pkExtraData))
            {
                NIASSERT(pkExtraData->GetName());
                kNames.Add(pkExtraData->GetName());
            }
        }
        for (ui = 0; ui < kNames.GetSize(); ui++)
        {
            pkNode->RemoveExtraData(kNames.GetAt(ui));
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::RemoveDeprecatedOptions(Interface* pkIntf)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);
    INode* pkRoot = pkIntf->GetRootNode();
    if (pkRoot)
    {
        TSTR strBuf = "\0";
        pkRoot->SetUserPropBuffer(strBuf);
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
NiString NiMAXOptions::GetValue(const char* strKey)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);

    g_kOptionsCS.Lock();
    NiString strValue = ms_pkDatabase->GetValue(strKey);
#ifdef LOG_IT_ALL
    NiOutputDebugString("Getting Value: ");
    NiOutputDebugString(strKey);
    NiOutputDebugString(" = ");
    NiOutputDebugString(strValue);
    NiOutputDebugString("\n");
#endif
    CHECK_MEMORY();
    g_kOptionsCS.Unlock();
    return strValue;
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetValue(const char* strKey, const char* strValue)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);
    
    g_kOptionsCS.Lock();
#ifdef LOG_IT_ALL
    NiOutputDebugString("Setting Value: ");
    NiOutputDebugString(strKey);
    NiOutputDebugString(" = ");
    NiOutputDebugString(strValue);
    NiOutputDebugString("\n");
#endif
    ms_pkDatabase->SetValue(strKey, strValue);
    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
bool NiMAXOptions::GetBool(const char* strKey)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);

    g_kOptionsCS.Lock();
    bool b = ms_pkDatabase->GetBool(strKey);
#ifdef LOG_IT_ALL
    NiOutputDebugString("Getting Bool: ");
    NiOutputDebugString(strKey);
    NiOutputDebugString(" = ");
    NiOutputDebugString(NiString::FromBool(b));
    NiOutputDebugString("\n");
#endif
    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
    return b;
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetBool(const char* strKey, bool b)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);

    g_kOptionsCS.Lock();
#ifdef LOG_IT_ALL
    NiOutputDebugString("Setting Bool: ");
    NiOutputDebugString(strKey);
    NiOutputDebugString(" = ");
    NiOutputDebugString(b ? "TRUE" : "FALSE");
    NiOutputDebugString("\n");
#endif
    CHECK_MEMORY();

    g_kOptionsCS.Unlock();
    ms_pkDatabase->SetBool(strKey, b);
}
//---------------------------------------------------------------------------
void NiMAXOptions::ToggleBool(const char* strKey)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);
    bool b = GetBool(strKey);
    SetBool(strKey, !b);
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
int NiMAXOptions::GetInt(const char* strKey)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);

    g_kOptionsCS.Lock();
    int i = ms_pkDatabase->GetInt(strKey); 
#ifdef LOG_IT_ALL
    NiOutputDebugString("Getting Int: ");
    NiOutputDebugString(strKey);
    NiOutputDebugString(" = ");
    NiOutputDebugString(NiString::FromInt(i));
    NiOutputDebugString("\n");
#endif

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
    return i;
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetInt(const char* strKey, int iValue)
{
    CHECK_MEMORY();
    NIASSERT(ms_pkDatabase);

    g_kOptionsCS.Lock();
#ifdef LOG_IT_ALL
    NiOutputDebugString("Setting Int: ");
    NiOutputDebugString(strKey);
    NiOutputDebugString(" = ");
    NiOutputDebugString(NiString::FromInt(iValue));
    NiOutputDebugString("\n");
#endif
    ms_pkDatabase->SetInt(strKey, iValue);

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::HandleDeprecatedINIOptions(Interface* pkInterface)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    if (MaxImmerse422Conversion::NeedsINIConversion(pkInterface))
        MaxImmerse422Conversion::ConvertINIOptions(pkInterface);
    SetSaveRequiredFlag(TRUE);

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
// This method will attempt to convert old node settings in the user-prop
// buffer of the root node in Max. It should be called prior to reading these
// values. It will return whether or not it completely converted it or
// whether you still need to load the settings from the root node. This
// allows us to spot-fix options rather than bear the burden of converting
// all options.
bool NiMAXOptions::HandleDeprecatedNodeOptions(Interface* pkInterface)
{
    g_kOptionsCS.Lock();
    CHECK_MEMORY();

    INode* pkNode = pkInterface->GetRootNode();
    NiString strBuffer;
    if (pkNode)
    {
        TSTR strBuf;
        pkNode->GetUserPropBuffer(strBuf);
        strBuffer = (const char*) strBuf;
    }

    bool bAllOptionsHandled = true;
    
    // If the root node has ANY Gamebryo options in it, process
    // them.
    if (strBuffer.Find(MAXIMMERSE_VERSION_KEY) != NIPT_INVALID_INDEX)
    {
        // We are going to selectively set options, so we need
        // to make sure that external logic continues processing
        // once we exit this method.
        bAllOptionsHandled = false;
        int iMaxImmerseVersion = 0;
        pkNode->GetUserPropInt(MAXIMMERSE_VERSION_KEY, iMaxImmerseVersion);

        // If the max file version is old and the value
        // hasn't already been set (The latter test is an artifact
        // of not upping the version in test assets).
        if (iMaxImmerseVersion < 5200005 && 
            strBuffer.Find(NI_ALLOW_NPOT_TEXTURES) == NIPT_INVALID_INDEX)
        {
            SetBool(NI_ALLOW_NPOT_TEXTURES, false);
        }

        // Set the default mesh profile in the case where a profile is not
        // found
        if ((iMaxImmerseVersion < 6050000) && 
            (strBuffer.Find(NI_SCENEMESHPROFILENAME) == NIPT_INVALID_INDEX))
        {
            SetValue(NI_SCENEMESHPROFILENAME, NI_DEFAULTSCENEMESHPROFILENAME);
        }

        // Handle conversions of values convert the renderer id.
        if (iMaxImmerseVersion < 6060000)
        {
            NiSystemDesc::RendererID eRenderer = 
                ConvertNiPluginToolkitToNiSystemDescRenderer(
                    GetInt(NI_EXPORT_PLATFORM));
            SetInt(NI_EXPORT_PLATFORM, (NiInt32)eRenderer);
        }
    }
    // MaxImmerse 4.2.2 conversion looks for the older MaxImmerse 
    // version key data structure.
    else if (MaxImmerse422Conversion::NeedsNodeConversion(pkInterface))
    {
        MaxImmerse422Conversion::ConvertNodeOptions(pkInterface);
    }
    SetSaveRequiredFlag(TRUE);

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
    return bAllOptionsHandled;
}
//---------------------------------------------------------------------------
bool NiMAXOptions::SaveInNode(NiString strKey)
{
    CHECK_MEMORY();
    if (strKey == NI_USEOLDBEHAVIOR || strKey == NI_USEOLDSPINTIME ||
        strKey == NI_USEOLDNORMALS || 
        strKey == NI_RESX || strKey == NI_RESY ||
        strKey == NI_USEAREASUPERSPRAY ||
        strKey == NI_VIEWER_TYPE ||
        strKey == NI_WINDOWED ||
        strKey == NI_VIEWER_STRIPIFY ||
        strKey == NI_DX9_VIEWER_SCRIPT_NAME || 
        strKey == NI_D3D10_VIEWER_SCRIPT_NAME || 
        strKey == NI_CUSTOM_VIEWER_SCRIPT_NAME ||
        strKey == NI_VIEWPORT_SCRIPT_NAME ||
        strKey == NI_SCENEIMMERSE_SCRIPT_NAME ||
        strKey == NI_D3D10_SCENEIMMERSE_SCRIPT_NAME ||
        strKey == NI_ANIMATION_TOOL_SCRIPT_NAME ||
        strKey == NI_XBOX_360_SCRIPT_NAME ||
        strKey == NI_PS3_SCRIPT_NAME ||
        strKey == NI_WII_SCRIPT_NAME ||
        strKey == NI_PHYSX_SCRIPT_NAME ||
        strKey == NI_DX9_VIEWER_SCRIPT || 
        strKey == NI_D3D10_VIEWER_SCRIPT || 
        strKey == NI_CUSTOM_VIEWER_SCRIPT ||
        strKey == NI_VIEWPORT_SCRIPT ||
        strKey == NI_SCENEIMMERSE_SCRIPT ||
        strKey == NI_D3D10_SCENEIMMERSE_SCRIPT ||
        strKey == NI_ANIMATION_TOOL_SCRIPT ||
        strKey == NI_XBOX_360_SCRIPT ||
        strKey == NI_PS3_SCRIPT ||
        strKey == NI_WII_SCRIPT ||
        strKey == NI_PHYSX_SCRIPT ||
        strKey == NI_NETWORK_SCRIPT_PATH ||
        strKey == NI_WRITE_ERRORS_TO_FILE ||
        strKey == NI_LOG_ERRORS_PROMPT || 
        strKey == NI_LAST_ERROR_PROMPT_RETURN ||
        strKey == NI_USE_OLD_PARTICLE_EXPORTER
        )
    {
        CHECK_MEMORY();
        return false;
    }
    else
    {
        CHECK_MEMORY();
        return true;
    }
}
//---------------------------------------------------------------------------
void NiMAXOptions::HandleValue(NiString strKey, NiString strValue)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    NiScriptInfoPtr spInfo = NULL;
    NiDefaultScriptReader kReader;
    NiString strErrors;

    if (strKey == NI_DX9_VIEWER_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetDX9ViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_D3D10_VIEWER_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetD3D10ViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_CUSTOM_VIEWER_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetCustomViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_ANIMATION_TOOL_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetAnimationViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_XBOX_360_SCRIPT_NAME &&
        pkManager->GetScript(strValue) != NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = pkManager->GetScript(strValue);
        if (spInfo)
            SetXenonViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_PS3_SCRIPT_NAME &&
        pkManager->GetScript(strValue) != NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = pkManager->GetScript(strValue);
        if (spInfo)
            SetPS3ViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_WII_SCRIPT_NAME &&
        pkManager->GetScript(strValue) != NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = pkManager->GetScript(strValue);
        if (spInfo)
            SetWiiViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_PHYSX_SCRIPT_NAME &&
        pkManager->GetScript(strValue) != NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = pkManager->GetScript(strValue);
        if (spInfo)
            SetPhysXViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_VIEWPORT_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetViewportScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_SCENEIMMERSE_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetSceneImmerseViewerScript(spInfo);
         else
             SetValue(strKey, strValue);

    }
    else if (strKey == NI_D3D10_SCENEIMMERSE_SCRIPT_NAME && 
        pkManager->GetScript(strValue) != NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);
         if (spInfo)
             SetD3D10SceneImmerseViewerScript(spInfo);
         else
             SetValue(strKey, strValue);

    }
    else if (strKey == NI_EXPORT_SCRIPT_NAME)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);

         if (GetBool(NI_USE_TEMPLATE_FOR_SCRIPT) && spInfo)
         {
             if (spInfo->GetFile() != "")
                 SetExportScript(spInfo);
             else
                 SetValue(strKey, strValue);
         }
         else
         {
             SetValue(strKey, strValue);
         }
    }
    else if (strKey == NI_PROCESS_SCRIPT_NAME)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = pkManager->GetScript(strValue);

         if (GetBool(NI_USE_TEMPLATE_FOR_SCRIPT) && spInfo)
         {
             if (spInfo->GetFile() != "")
                 SetProcessScript(spInfo);
             else
                 SetValue(strKey, strValue);
         }
         else
         {
             SetValue(strKey, strValue);
         }

    }
    else if (strKey == NI_DX9_VIEWER_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));

         if (spInfo)
             SetDX9ViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }    
    else if (strKey == NI_D3D10_VIEWER_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));

         if (spInfo)
             SetD3D10ViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }    
    else if (strKey == NI_CUSTOM_VIEWER_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
         if (spInfo)
             SetCustomViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_ANIMATION_TOOL_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors,
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
         if (spInfo)
             SetAnimationViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_XBOX_360_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
         if (spInfo)
             SetXenonViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_PS3_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = kReader.Parse(strValue, strErrors,
            NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
        if (spInfo)
            SetPS3ViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_WII_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = kReader.Parse(strValue, strErrors,
            NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
        if (spInfo)
            SetWiiViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_PHYSX_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = kReader.Parse(strValue, strErrors, 
            NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
        if (spInfo)
            SetPhysXViewerScript(spInfo);
        else
            SetValue(strKey, strValue);
    }
    else if (strKey == NI_VIEWPORT_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
         if (spInfo)
             SetViewportScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_SCENEIMMERSE_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
         if (spInfo)
             SetSceneImmerseViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_D3D10_SCENEIMMERSE_SCRIPT && 
        pkManager->GetScript(strValue) == NULL)
    {
         NiString strOutput = "Handling: " + strKey;
         NiOutputDebugString(strOutput);
         spInfo = kReader.Parse(strValue, strErrors, 
             NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
         if (spInfo)
             SetD3D10SceneImmerseViewerScript(spInfo);
         else
             SetValue(strKey, strValue);
    }
    else if (strKey == NI_EXPORT_SCRIPT)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = kReader.Parse(strValue, strErrors,
            NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
        bool bUseTemplate = false;
        if (spInfo)
        {
            // check if there is a cached version of the same name
            NiScriptInfo* pkCachedInfo = pkManager->GetScript(
                spInfo->GetName());

            if (pkCachedInfo)
            {
                // check if the desired script is a template
                if (pkCachedInfo->GetFile() != "")
                {
                    if (GetBool(NI_USE_TEMPLATE_FOR_SCRIPT))
                    {
                        // if the user wants to use templates, and the
                        // script name is a template name, use it
                        SetExportScript(pkCachedInfo);
                        bUseTemplate = true;
                    }
                    else
                    {
                        // the user wants to use the root node's script, but
                        // that script is named the same as a template script
                        // we must give it a unique name
                        NiString NiNewName;
                        int i = 0;
                        while (pkCachedInfo != NULL)
                        {
                            NiNewName = "Modified " + spInfo->GetName() + " " +
                                NiString::FromInt(i);
                            pkCachedInfo = pkManager->GetScript(NiNewName);
                            i++;
                        }
                        spInfo->SetName(NiNewName);
                        SetValue(NI_EXPORT_SCRIPT_NAME, spInfo->GetName());
                    }
                }
            }
        }
        if (!bUseTemplate)
        {
            // load the script from the root node
            if (spInfo)
            {
                SetExportScript(spInfo);
            }
            else
            {
                SetValue(strKey, strValue);
            }
        }
    }
    else if (strKey == NI_PROCESS_SCRIPT)
    {
        NiString strOutput = "Handling: " + strKey;
        NiOutputDebugString(strOutput);
        spInfo = kReader.Parse(strValue, strErrors,
            NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
        bool bUseTemplate = false;

        if (spInfo)
        {
            // check if there is a cached version of the same name
            NiScriptInfo* pkCachedInfo = pkManager->GetScript(
                spInfo->GetName());

            if (pkCachedInfo)
            {
                // check if the desired script is a template
                if (pkCachedInfo->GetFile() != "")
                {
                    if (GetBool(NI_USE_TEMPLATE_FOR_SCRIPT))
                    {
                        // if the user wants to use templates, and the
                        // script name is a template name, use it
                        SetProcessScript(pkCachedInfo);
                        bUseTemplate = true;
                    }
                    else
                    {
                        // the user wants to use the root node's script, but
                        // that script is named the same as a template script
                        // we must give it a unique name
                        NiString NiNewName;
                        int i = 0;
                        while (pkCachedInfo != NULL)
                        {
                            NiNewName = "Modified " + spInfo->GetName() + " " +
                                NiString::FromInt(i);
                            pkCachedInfo = pkManager->GetScript(NiNewName);
                            i++;
                        }
                        spInfo->SetName(NiNewName);
                        SetValue(NI_PROCESS_SCRIPT_NAME, spInfo->GetName());
                    }
                }
            }
        }
        if (!bUseTemplate)
        {
            // load the script from the root node
            if (spInfo)
            {
                SetProcessScript(spInfo);
            }
            else
            {
                SetValue(strKey, strValue);
            }
        }
    }
    CHECK_MEMORY();

    g_kOptionsCS.Unlock();
}

//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetExportScript()
{
    CHECK_MEMORY();
    if (ms_spLastExportScript && ms_spLastExportScript->GetName() != 
        GetValue(NI_EXPORT_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_EXPORT_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetExportScript(spInfo);
            }
        }
    }
    return ms_spLastExportScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetProcessScript()
{
    CHECK_MEMORY();
    if (ms_spLastProcessScript && ms_spLastProcessScript->GetName() != 
        GetValue(NI_PROCESS_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_PROCESS_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;

            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetProcessScript(spInfo);
            }
        }
    }
    return ms_spLastProcessScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetDX9ViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastDX9ViewerScript && ms_spLastDX9ViewerScript->GetName() != 
        GetValue(NI_DX9_VIEWER_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_DX9_VIEWER_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
    
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetDX9ViewerScript(spInfo);
            }
        }
    }
    return ms_spLastDX9ViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetD3D10ViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastD3D10ViewerScript && ms_spLastD3D10ViewerScript->GetName() != 
        GetValue(NI_D3D10_VIEWER_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_D3D10_VIEWER_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
    
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetD3D10ViewerScript(spInfo);
            }
        }
    }
    return ms_spLastD3D10ViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetAnimationViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastAnimationToolViewerScript && 
        ms_spLastAnimationToolViewerScript->GetName() != 
        GetValue(NI_ANIMATION_TOOL_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_ANIMATION_TOOL_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
    
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetAnimationViewerScript(spInfo);
            }
        }
    }
    return ms_spLastAnimationToolViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetXenonViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastXenonViewerScript && 
        ms_spLastXenonViewerScript->GetName() != 
        GetValue(NI_XBOX_360_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_XBOX_360_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;

            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetXenonViewerScript(spInfo);
            }
        }
    }
    return ms_spLastXenonViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetPS3ViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastPS3ViewerScript && 
        ms_spLastPS3ViewerScript->GetName() != 
        GetValue(NI_PS3_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_PS3_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;

            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetPS3ViewerScript(spInfo);
            }
        }
    }
    return ms_spLastPS3ViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetWiiViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastWiiViewerScript && 
        ms_spLastWiiViewerScript->GetName() != 
        GetValue(NI_WII_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_WII_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;

            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
                SetWiiViewerScript(spInfo);
        }
    }
    return ms_spLastWiiViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetPhysXViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastPhysXViewerScript && 
        ms_spLastPhysXViewerScript->GetName() != 
        GetValue(NI_PHYSX_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_PHYSX_SCRIPT_NAME);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;

            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetPhysXViewerScript(spInfo);
            }
        }
    }
    return ms_spLastPhysXViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetSceneImmerseViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastSceneImmerseViewerScript && 
        ms_spLastSceneImmerseViewerScript->GetName() != 
        GetValue(NI_SCENEIMMERSE_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_SCENEIMMERSE_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
    
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetSceneImmerseViewerScript(spInfo);
            }
        }
    }
    return ms_spLastSceneImmerseViewerScript;
}        
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetD3D10SceneImmerseViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastD3D10SceneImmerseViewerScript && 
        ms_spLastD3D10SceneImmerseViewerScript->GetName() != 
        GetValue(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_D3D10_SCENEIMMERSE_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
    
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetD3D10SceneImmerseViewerScript(spInfo);
            }
        }
    }
    return ms_spLastD3D10SceneImmerseViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetCustomViewerScript()
{
    CHECK_MEMORY();
    if (ms_spLastCustomViewerScript && 
        ms_spLastCustomViewerScript->GetName() != 
        GetValue(NI_CUSTOM_VIEWER_SCRIPT_NAME))
    {
        NiString strScript = GetValue(NI_CUSTOM_VIEWER_SCRIPT);
        if (!strScript.IsEmpty())
        {
            NiString strErrors;
            NiDefaultScriptReader kReader;
    
            NiScriptInfoPtr spInfo = kReader.Parse(strScript, strErrors,
                NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE));
            if (spInfo)
            {
                SetCustomViewerScript(spInfo);
            }
        }
    }
    return ms_spLastCustomViewerScript;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiMAXOptions::GetViewportScript()
{
    CHECK_MEMORY();
    return ms_spLastViewportScript;
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetExportScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastExportScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_EXPORT_SCRIPT, strEncoding);
    SetValue(NI_EXPORT_SCRIPT_NAME, pkScript->GetName());   
    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetProcessScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastProcessScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_PROCESS_SCRIPT, strEncoding);
    SetValue(NI_PROCESS_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetDX9ViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastDX9ViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_DX9_VIEWER_SCRIPT, strEncoding);
    SetValue(NI_DX9_VIEWER_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetD3D10ViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastD3D10ViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_D3D10_VIEWER_SCRIPT, strEncoding);
    SetValue(NI_D3D10_VIEWER_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetAnimationViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastAnimationToolViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_ANIMATION_TOOL_SCRIPT, strEncoding);
    SetValue(NI_ANIMATION_TOOL_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetXenonViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastXenonViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_XBOX_360_SCRIPT, strEncoding);
    SetValue(NI_XBOX_360_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetPS3ViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastPS3ViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_PS3_SCRIPT, strEncoding);
    SetValue(NI_PS3_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetWiiViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastWiiViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_WII_SCRIPT, strEncoding);
    SetValue(NI_WII_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetPhysXViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastPhysXViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_PHYSX_SCRIPT, strEncoding);
    SetValue(NI_PHYSX_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetSceneImmerseViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastSceneImmerseViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_SCENEIMMERSE_SCRIPT, strEncoding);
    SetValue(NI_SCENEIMMERSE_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}                                                                      
//---------------------------------------------------------------------------
void NiMAXOptions::SetD3D10SceneImmerseViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastD3D10SceneImmerseViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_D3D10_SCENEIMMERSE_SCRIPT, strEncoding);
    SetValue(NI_D3D10_SCENEIMMERSE_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetCustomViewerScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    ms_spLastCustomViewerScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_CUSTOM_VIEWER_SCRIPT, strEncoding);
    SetValue(NI_CUSTOM_VIEWER_SCRIPT_NAME, pkScript->GetName());

    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXOptions::SetViewportScript(NiScriptInfo* pkScript)
{
    CHECK_MEMORY();
    ms_spLastViewportScript = pkScript;
    NiDefaultScriptWriter kWriter;
    NiString strEncoding = kWriter.Encode(pkScript);
    SetValue(NI_VIEWPORT_SCRIPT, strEncoding);
    SetValue(NI_VIEWPORT_SCRIPT_NAME, pkScript->GetName());
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
NiSystemDesc::RendererID NiMAXOptions::
    ConvertDevImageToNiSystemDescRenderer(
    NiDevImageConverter::Platform ePlatform)
{
    CHECK_MEMORY();
    switch(ePlatform)
    {
    case NiDevImageConverter::ANY:
        return  NiSystemDesc::RENDERER_GENERIC;
    case NiDevImageConverter::DX9:
        return NiSystemDesc::RENDERER_DX9;
    case NiDevImageConverter::D3D10:
        return NiSystemDesc::RENDERER_D3D10;
    case NiDevImageConverter::XENON:
        return NiSystemDesc::RENDERER_XENON;
    case NiDevImageConverter::PLAYSTATION3:
        return NiSystemDesc::RENDERER_PS3;
    case NiDevImageConverter::WII:
        return NiSystemDesc::RENDERER_WII;
    default:
        return NiSystemDesc::RENDERER_GENERIC;
    };
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
NiDevImageConverter::Platform NiMAXOptions::
    ConvertNiSystemDescRendererToDevImage(
    NiSystemDesc::RendererID ePlatform)
{
    CHECK_MEMORY();
    switch(ePlatform)
    {
    case NiSystemDesc::RENDERER_XENON:
        return NiDevImageConverter::XENON;
    case NiSystemDesc::RENDERER_PS3:
        return NiDevImageConverter::PLAYSTATION3;
    case NiSystemDesc::RENDERER_WII:
        return NiDevImageConverter::WII;
    case NiSystemDesc::RENDERER_DX9:
        return NiDevImageConverter::DX9;
    case NiSystemDesc::RENDERER_D3D10:
        return NiDevImageConverter::D3D10;
    default:
        return NiDevImageConverter::ANY;
    };
}

//---------------------------------------------------------------------------
bool NiMAXOptions::IsAnimationViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetAnimationViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsXenonViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetXenonViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsPS3ViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetPS3ViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsWiiViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetWiiViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsPhysXViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetPhysXViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsDX9ViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetDX9ViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsD3D10ViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetD3D10ViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsSceneImmerseViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetSceneImmerseViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMAXOptions::IsCustomViewerAvailable()
{
    CHECK_MEMORY();
    NiScriptInfo* pkInfo = GetCustomViewerScript();
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && pkManager->VerifyScriptInfo(pkInfo))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiScriptInfoSet* NiMAXOptions::GenerateAvailableExportScripts()
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    NiScriptTemplateManager* pkScriptManager = 
        NiScriptTemplateManager::GetInstance();
    NiScriptInfoSet* pkSet = NiNew NiScriptInfoSet();
    pkSet->IgnorePluginClass("NiMFCPlugin");
    pkSet->Copy(&pkScriptManager->GetScriptInfoSet());

    NiTObjectArray<NiScriptInfoPtr> kInvalidScripts;

    NiScriptInfo* pkCurrentScript = NiMAXOptions::GetExportScript();
    if (pkCurrentScript && pkSet->GetScriptIndex(pkCurrentScript->GetName())
        == NIPT_INVALID_INDEX)
    {
        pkSet->AddScript(pkCurrentScript);
    }
    else if (pkCurrentScript)
    {
        NiScriptInfo* pkInfo = pkSet->GetScript(pkCurrentScript->GetName());
        if (pkInfo && pkInfo->Equals(pkCurrentScript))
        {
            NiOutputDebugString("Matching export script found as template.");
        }
        else if (pkInfo && NiMAXOptions::GetBool("USE_SCRIPT_TEMPLATES"))
        {
            NiOutputDebugString("Non-matching export script found as "
                "template.");
            NiMAXOptions::SetExportScript(pkInfo);
        }
        else
        {
            NiOutputDebugString("The export script doesn't match and we're "
                "not supposed to use script templates instead...");
            pkSet->ReplaceScript(pkCurrentScript);
        }
    }   
    
    unsigned int ui = 0;
    for (; ui < pkSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = pkSet->GetScriptAt(ui);
        if (pkInfo)
        {
            NiString strType = pkInfo->GetType();
            if (!strType.EqualsNoCase("Export"))
                kInvalidScripts.Add(pkInfo);
        }
    }

    for (ui = 0; ui < kInvalidScripts.GetSize(); ui++)
    {
        NiScriptInfoPtr spInfo = kInvalidScripts.GetAt(ui);
        if (spInfo)
        {
            pkSet->RemoveScript(spInfo->GetName());
        }
    }


    CHECK_MEMORY();
    
    g_kOptionsCS.Unlock();
    return pkSet;
}
//---------------------------------------------------------------------------
NiScriptInfoSet* NiMAXOptions::GenerateAvailableProcessScripts()
{
    CHECK_MEMORY();

    g_kOptionsCS.Lock();
    NiScriptTemplateManager* pkScriptManager = 
        NiScriptTemplateManager::GetInstance();
    NiScriptInfoSet* pkSet = NiNew NiScriptInfoSet();
    pkSet->IgnorePluginClass("NiMFCPlugin");
    pkSet->Copy(&pkScriptManager->GetScriptInfoSet());

    NiTObjectArray<NiScriptInfoPtr> kInvalidScripts;

    NiScriptInfo* pkCurrentScript = NiMAXOptions::GetProcessScript();
    if (pkCurrentScript && pkSet->GetScriptIndex(pkCurrentScript->GetName())
        == NIPT_INVALID_INDEX)
    {
        pkSet->AddScript(pkCurrentScript);
    }
    else if (pkCurrentScript)
    {
        NiScriptInfo* pkInfo = pkSet->GetScript(pkCurrentScript->GetName());
        if (pkInfo && pkInfo->Equals(pkCurrentScript))
        {
            NiOutputDebugString("Matching process script found as template.");
        }
        else if (pkInfo && NiMAXOptions::GetBool("USE_SCRIPT_TEMPLATES"))
        {
            NiOutputDebugString("Non-matching process script found as"
                " template.");
            NiMAXOptions::SetProcessScript(pkInfo);
        }
        else
        {
            NiOutputDebugString("The process script doesn't match and we're "
                "not supposed to use script templates instead...");
            pkSet->ReplaceScript(pkCurrentScript);
        }
    }   
    
    unsigned int ui = 0;
    for (; ui < pkSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = pkSet->GetScriptAt(ui);
        if (pkInfo)
        {
            NiString strType = pkInfo->GetType();
            if (!strType.EqualsNoCase("Process"))
                kInvalidScripts.Add(pkInfo);
        }
    }

    for (ui = 0; ui < kInvalidScripts.GetSize(); ui++)
    {
        NiScriptInfoPtr spInfo = kInvalidScripts.GetAt(ui);
        if (spInfo)
        {
            pkSet->RemoveScript(spInfo->GetName());
        }
    }

    
    g_kOptionsCS.Unlock();
    CHECK_MEMORY();
    return pkSet;
}
//---------------------------------------------------------------------------
unsigned int NiMAXOptions::GetOptionsCount()
{
    CHECK_MEMORY();
    return ms_pkDatabase->GetParameterCount();
}
//---------------------------------------------------------------------------
NiString NiMAXOptions::GetNameAt(unsigned int ui)
{
    CHECK_MEMORY();
    NiParameterInfo* pkInfo = ms_pkDatabase->GetParameterAt(ui);
    if (pkInfo)
        return pkInfo->GetKey();
    else
        return "";
}
//---------------------------------------------------------------------------
NiString NiMAXOptions::GetValueAt(unsigned int ui)
{
    CHECK_MEMORY();

    NiParameterInfo* pkInfo = ms_pkDatabase->GetParameterAt(ui);
    if (pkInfo)
        return pkInfo->GetValue();
    else
        return "";
}
//---------------------------------------------------------------------------
NiString NiMAXOptions::GetNetworkScriptDirectory()
{
    NiString strValue = "";
    if (ms_pkDatabase)
        strValue = GetValue(NI_NETWORK_SCRIPT_PATH);
    
    if (strValue.IsEmpty())
    {
        TCHAR acFilename[MAX_PATH];
        GetCfgFilename(GetCOREInterface(), acFilename, MAX_PATH);
        NiString strSection = NI_OPTIONS_SECTION;
        NiString strKey = NI_NETWORK_SCRIPT_PATH;
        char acInput[MAX_VALUE_LENGTH + 1 ];
        GetPrivateProfileString(strSection, strKey, "", acInput, 
            MAX_VALUE_LENGTH, acFilename);
        strValue = acInput;
    }
    return strValue;
}
