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


#include "NiString.h"
#include "NiStringTokenizer.h"
#include "NiScriptInfo.h"
#include "NiScriptTemplateManager.h"
#include "NiScriptConvertDialog.h"
#include "NiPluginInfo.h"
#include "NiParameterInfo.h"
#include "NiDefaultScriptReader.h"
#include "NiExportPluginInfo.h"
#include "NiImportPluginInfo.h"
#include "NiViewerPluginInfo.h"
/// Format:
///    Script("SCRIPT_NAME","TYPE")
///    {
///        Plugin("PLUGIN_CLASSNAME","PLUGIN_NAME","PLUGIN_VERSION",
///            "PLUGIN_TYPE")
///        {
///            Parameter("PARAM_KEY", "PARAM_VALUE");
///            ...
///        }
///        ...
///  
//---------------------------------------------------------------------------
/// Takes an input character script and creates the ScriptInfo object
/// from it.

#define SEARCH_FOR_HEADER -1
#define SEARCH_FOR_SCRIPT 0
#define GET_SCRIPT_NAME 1
#define SEARCH_FOR_OPEN_SCRIPT 2
#define SEARCH_FOR_CLOSE_SCRIPT 3
#define SEARCH_FOR_PLUGIN 4
#define GET_PLUGIN_CLASS 5
#define GET_PLUGIN_NAME 6
#define GET_PLUGIN_VERSION 7
#define GET_PLUGIN_TYPE 8
#define SEARCH_FOR_OPEN_PLUGIN 9
#define SEARCH_FOR_CLOSE_PLUGIN 10
#define SEARCH_FOR_PARAM 11
#define GET_PARAM_KEY 12
#define GET_PARAM_VALUE 13
#define PARSE_ERROR 14
#define SCRIPT_COMPLETED 15
#define GET_SCRIPT_TYPE 16

NiScriptInfo* NiDefaultScriptReader::Parse(const char* pcScript, 
    NiString& strErrors, bool bSilentRunning)
{
    int state = SEARCH_FOR_HEADER;
    NiStringTokenizer kTokenizer(pcScript);

    bool bConvertScript = false;
    NiScriptInfo* pkScript = NULL;
    NiPluginInfoPtr spPlugin = NULL;
    NiParameterInfoPtr spParam = NULL;
    NiString strClassName;
    NiString strName;
    NiString strVersion;

    while(!kTokenizer.IsEmpty() && state != SCRIPT_COMPLETED)
    {
        NiString strTemp;
        switch(state)
        {
            case SEARCH_FOR_HEADER:
                {
                    strTemp = kTokenizer.GetNextToken("\t \n\x00D");
                    if (strTemp.EqualsNoCase(SCRIPT_HEADER))
                    {
                        strTemp = kTokenizer.GetNextToken("\t \n\x00D");
                        if (!strTemp.EqualsNoCase(SCRIPT_VERSION))
                        {
                            bConvertScript = true;
                        }
                    }
                    else
                    {
                        bConvertScript = true;
                        kTokenizer.Restart();
                    }
                    state = SEARCH_FOR_SCRIPT;
                }
            break;
            case SEARCH_FOR_SCRIPT:
                {
                    strTemp = kTokenizer.GetNextToken("\t \n\x00D");
                    if (strTemp.EqualsNoCase("SCRIPT"))
                    {
                        if (pkScript != NULL)
                            NiDelete pkScript;

                        pkScript = NiNew NiScriptInfo();
                        state = GET_SCRIPT_NAME;
                    }
                    else
                    {
                        strErrors.Concatenate(
                            "Could not find 'Script' tag!\n");
                        state = PARSE_ERROR;
                    }
                }
                break;
            case GET_SCRIPT_NAME:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \x00D");
                   
                    // Eat the beginning parenthesis
                    if(kTokenizer.Consume("(") != 1 && !kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate("Could not find '(' tag!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    // Eat all whitespaces
                    kTokenizer.Consume("\t \x00D");
                    // Get the name
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);
                    }
                    pkScript->SetName(strTemp);
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \x00D");

                    // Eat the ending parenthesis
                    if (kTokenizer.Consume(",") != 1 && !kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find ',' for the type tag!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    kTokenizer.Consume("\t \n\x00D");
                    state = GET_SCRIPT_TYPE;
                }
                break;
            case GET_SCRIPT_TYPE:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \x00D");

                    // Get the type
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);
                    }
                    pkScript->SetType(strTemp);
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \x00D");

                    // Eat the ending parenthesis
                    if (kTokenizer.Consume(")") != 1 && !kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate("Could not find ')' tag!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    kTokenizer.Consume("\t \n\x00D");
                    state = SEARCH_FOR_OPEN_SCRIPT;
                }
                break;
            case SEARCH_FOR_OPEN_SCRIPT:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    // Eat the beginning bracket
                    if (kTokenizer.Consume("{") == 1)
                    {
                        state = SEARCH_FOR_PLUGIN;
                        // Eat all whitespaces
                        kTokenizer.Consume("\t \n\x00D");
                    }
                    else
                    {
                        char szBuffer[1024];
                        NiStrncpy(szBuffer, 1204, kTokenizer.Peek(), 10);

                        strErrors.Concatenate("OPEN SCRIPT::Could not find "
                            "'{' tag!\n");
                        strErrors.Concatenate("STRING**");
                        strErrors.Concatenate(szBuffer);
                        strErrors.Concatenate("**STRING\n");

                        char cFirst = szBuffer[0];
                        NiSprintf(szBuffer, 1024, "Char %d Tab %d NewLine "
                            "%d\n", cFirst, '\t', '\n');
                        strErrors.Concatenate(szBuffer);

                        strErrors.Concatenate("FOOFOO\n");
                        state = PARSE_ERROR;
                        break;
                    }
                }
                break;
            case SEARCH_FOR_CLOSE_SCRIPT:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    // Eat the beginning bracket
                    if (kTokenizer.Consume("}") == 1)
                    {
                        state = SCRIPT_COMPLETED;
                    }
                    else
                    {
                        strErrors.Concatenate("Could not find '}' tag!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                }
                break;
            case SEARCH_FOR_PLUGIN:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    const char* pcPlugin = kTokenizer.Peek();

                    // Peek and see if we have a Plugin
                    if (_strnicmp(pcPlugin, "Plugin", 6) == 0)
                    {
                        kTokenizer.Consume(6);
                        state = GET_PLUGIN_CLASS;
                    }
                    else
                    {
                        state = SEARCH_FOR_CLOSE_SCRIPT;
                    }                   
                }
                break;
            case GET_PLUGIN_CLASS:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume("(") != 1 || kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find Plugin class name!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n\x00D");
                   
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1); 
                    }

                    if (kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find Plugin class name!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    strClassName = strTemp;
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume(",") != 1)
                    {
                        strErrors.Concatenate(
                            "Could not find ',' after Plugin class name!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n\x00D");
                    state = GET_PLUGIN_NAME;
                }
                break;
            case GET_PLUGIN_NAME:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);   
                    }

                    if (kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find Plugin user name!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    strName = strTemp;
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume(",") != 1)
                    {
                        strErrors.Concatenate(
                            "Could not find ',' after Plugin user name!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n");
                    state = GET_PLUGIN_VERSION;
                }
                break;
            case GET_PLUGIN_VERSION:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);     
                    }

                    if (kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find Plugin version!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    strVersion = strTemp;
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume(",") != 1)
                    {
                        strErrors.Concatenate(
                            "Could not find ',' after Plugin version!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n\x00D");
                    state = GET_PLUGIN_TYPE;
                }
                break;
            case GET_PLUGIN_TYPE:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);                   
                    }
                   
                    if (kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate("Could not find Plugin type!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    spPlugin = CreateByType(strClassName, strName, strVersion, 
                        strTemp);
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume(")") != 1)
                    {
                        strErrors.Concatenate(
                            "Could not find ')' after Plugin type!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n\x00D");
                    state = SEARCH_FOR_OPEN_PLUGIN;
                }
                break;
            case SEARCH_FOR_OPEN_PLUGIN:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    // Eat the beginning bracket
                    if (kTokenizer.Consume("{") == 1)
                    {
                        state = SEARCH_FOR_PARAM;
                        // Eat all whitespaces
                        kTokenizer.Consume("\t \n\x00D");
                    }
                    else
                    {
                        strErrors.Concatenate("Could not find '{' tag!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                }
                break;
           case SEARCH_FOR_CLOSE_PLUGIN:
               {
                   // Eat all whitespaces
                   kTokenizer.Consume("\t \n\x00D");
                   // Eat the beginning bracket
                   if (kTokenizer.Consume("}") == 1)
                   {
                       pkScript->AddPluginInfo(spPlugin);
                       state = SEARCH_FOR_PLUGIN;
                       // Eat all whitespaces
                       kTokenizer.Consume("\t \n\x00D");
                   }
                   else
                   {
                      strErrors.Concatenate("Could not find '}' tag!\n");
                      state = PARSE_ERROR;
                      break;
                   }
               }
               break;
            case SEARCH_FOR_PARAM :
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    const char* pcPlugin = kTokenizer.Peek();
                    const char* pcParam = "Parameter";
                    // Peek and see if we have a Plugin
                    if (_strnicmp(pcPlugin, pcParam, strlen(pcParam)) == 0)
                    {
                        kTokenizer.Consume((unsigned int)strlen(pcParam));
                        spParam = NiNew NiParameterInfo();
                        state = GET_PARAM_KEY;
                    }
                    else
                    {
                        state = SEARCH_FOR_CLOSE_PLUGIN;
                    }        
               }
               break;
            case GET_PARAM_KEY:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume("(") != 1 || kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find Plugin parameter name!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n\x00D");
                   
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);   
                    }

                    if (kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate(
                            "Could not find Plugin parameter name!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    spParam->SetKey(strTemp);
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume(",") != 1)
                    {
                        strErrors.Concatenate(
                        "Could not find ',' after Plugin parameter name!\n");
                        state = PARSE_ERROR;
                        break;
                    }
                    kTokenizer.Consume("\t \n\x00D");
                    state = GET_PARAM_VALUE;
                }
                break;
            case GET_PARAM_VALUE:
                {
                    // Eat all whitespaces
                    kTokenizer.Consume("\t \n\x00D");
                    const char* pcBuffer = kTokenizer.Peek();
                    if (pcBuffer[0] == '\"' && pcBuffer[1] == '\"')
                    {
                        strTemp = "\0";
                        kTokenizer.Consume(2);
                    }
                    else
                    {
                        strTemp = kTokenizer.GetNextToken("\"");
                        kTokenizer.Consume(1);                   
                    }
                   
                    if (kTokenizer.IsEmpty())
                    {
                        strErrors.Concatenate("Could not find param value!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    spParam->SetValue(strTemp);
                    kTokenizer.Consume("\t \n\x00D");
                    if (kTokenizer.Consume(")") != 1)
                    {
                        strErrors.Concatenate(
                            "Could not find ')' after param value!\n");
                        state = PARSE_ERROR;
                        break;
                    }

                    if (NiIsKindOf(NiViewerPluginInfo, spPlugin) && 
                        spParam->GetKey() == VIEWER_PLUGIN_TYPE)
                    {
                       spPlugin->
                           SetValue(spParam->GetKey(), spParam->GetValue());
                    }
                    else
                    {
                        spPlugin->AddParameter(spParam);
                    }

                    kTokenizer.Consume("\t \n;\x00D");
                    state = SEARCH_FOR_PARAM;
                }
                break;
            case PARSE_ERROR:
                {
                    bConvertScript = false;
                    NiDelete pkScript;
                    return NULL;
                }
                break;
        }
    }

    if (kTokenizer.IsEmpty() && state != SCRIPT_COMPLETED)
    {
        strErrors.Concatenate("String ran out before script was completed!\n");
        bConvertScript = false;
        NiDelete pkScript;
        return NULL;
    }

    // Make sure we set this flag so we do not need to resave the file.
    pkScript->MarkAsDirty(false);

    if (bConvertScript)
    {
        ConvertScript(pkScript, bSilentRunning);
    }

    return pkScript;
}

//---------------------------------------------------------------------------
NiString NiDefaultScriptReader::GetFileExtensions()
{
    return "*.script";
}

//---------------------------------------------------------------------------
/// Creates an NiPluginInfo of the appropriate class based on the input
/// parameters
NiPluginInfo* NiDefaultScriptReader::CreateByType(NiString strClassName,
    NiString strName, NiString strVersion, NiString strType)
{
    NiPluginInfo* pkPluginInfo = NULL;
    if (strType.EqualsNoCase("PROCESS"))
        pkPluginInfo = NiNew NiPluginInfo();
    else if (strType.EqualsNoCase("EXPORT"))
        pkPluginInfo = NiNew NiExportPluginInfo();
    else if (strType.EqualsNoCase("IMPORT"))
        pkPluginInfo = NiNew NiImportPluginInfo();
    else if (strType.EqualsNoCase("VIEWER"))
        pkPluginInfo = NiNew NiViewerPluginInfo();

    if (!pkPluginInfo)
    {
        NIASSERT(!pkPluginInfo);
        strType = "PROCESS";
        pkPluginInfo = NiNew NiPluginInfo();
    }

    pkPluginInfo->SetClassName(strClassName);
    pkPluginInfo->SetName(strName);
    pkPluginInfo->SetVersion(strVersion);
    pkPluginInfo->SetType(strType);
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
void NiDefaultScriptReader::ConvertScript(NiScriptInfo* pkScriptInfo,
    bool bSilentRunning)
{ 
    if (pkScriptInfo->GetType().CompareNoCase("PROCESS") == 0)
    {
        bool bConvertScript = true;
        if (!NiScriptConvertDialog::GetYesToAll() && !bSilentRunning)
        {
            NiString strTitle = "Update script: ";
            strTitle += pkScriptInfo->GetName();
            NiScriptConvertDialog kDlg(strTitle);
            if (kDlg.DoModal() != IDOK)
            {
                bConvertScript = false;
            }
        }

        if (NiScriptConvertDialog::GetYesToAll() || bConvertScript)
        {
            if (NiScriptConvertDialog::GetModifyScript())
            {
                bool bFoundFinalize = false;
                NiUInt32 uiFirstExportPluginIndex = UINT_MAX;

                NiUInt32 uiCount = pkScriptInfo->GetPluginInfoCount();
                for (NiUInt32 ui=0; ui < uiCount; ui++)
                {
                    NiPluginInfo* pkPlugin = pkScriptInfo->GetPluginInfoAt(ui);
                    if(pkPlugin->GetName().CompareNoCase("Mesh Finalize") == 0)
                    {
                        bFoundFinalize = true;
                    }

                    if (uiFirstExportPluginIndex == UINT_MAX &&
                        pkPlugin->GetType().CompareNoCase("EXPORT") == 0)
                    {
                        uiFirstExportPluginIndex = ui;
                    }
                }

                if (uiFirstExportPluginIndex == UINT_MAX)
                    uiFirstExportPluginIndex = uiCount;

                if (!bFoundFinalize)
                {
                    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo;
                    pkPluginInfo->SetClassName("MeshFinalizePlugin");
                    pkPluginInfo->SetName("Mesh Finalize");
                    pkPluginInfo->SetVersion("1.0");
                    pkPluginInfo->SetType("PROCESS");
                    pkScriptInfo->AddPluginInfo(pkPluginInfo);
                    pkScriptInfo->MovePluginInfoToPosition(pkPluginInfo,
                        uiFirstExportPluginIndex);
                }
            }

            // Mark the script as dirty
            pkScriptInfo->MarkAsDirty(true);

            // Save the file if we have one
            if (pkScriptInfo->GetFile().Length() > 0)
            {
                NiScriptTemplateManager::SaveScript(pkScriptInfo->GetFile(),
                    pkScriptInfo, bSilentRunning);
            }
        }
    } // if (pkScriptInfo->GetType().CompareNoCase("PROCESS") == 0)
    else if (pkScriptInfo->GetType().CompareNoCase("VIEWER") == 0)
    {
        NiUInt32 uiCount = pkScriptInfo->GetPluginInfoCount();
        for (NiUInt32 ui=0; ui < uiCount; ui++)
        {
            NiViewerPluginInfo* pkPlugin = 
                (NiViewerPluginInfo*)pkScriptInfo->GetPluginInfoAt(ui);
            pkPlugin->SetRenderer(
                ConvertNiPluginToolkitToNiSystemDescRenderer(
                    (NiInt32)pkPlugin->GetRenderer()));
        }

        // Mark the script as dirty
        pkScriptInfo->MarkAsDirty(true);

        // Save the file if we have one
        if (pkScriptInfo->GetFile().Length() > 0)
        {
            NiScriptTemplateManager::SaveScript(pkScriptInfo->GetFile(),
                pkScriptInfo, bSilentRunning);
        }

    } // if (pkScriptInfo->GetType().CompareNoCase("VIEWER") == 0)
}
