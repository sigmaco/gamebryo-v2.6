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

#include "NiPluginHelpers.h"
#include "NiPluginManager.h"
#include "NiString.h"
#include "NiFramework.h"
#include "NiImportPluginInfo.h"
#include "NiExportPluginInfo.h"
#include "NiStringTokenizer.h"
#include <NiStream.h>

char* NiPluginHelpers::ms_pcLastImportDir = NULL;
char* NiPluginHelpers::ms_pcLastExportDir = NULL;

//---------------------------------------------------------------------------
void NiPluginHelpers::_SDMShutdown()
{
    NiFree(ms_pcLastImportDir);
    NiFree(ms_pcLastExportDir);
}
//---------------------------------------------------------------------------
bool NiPluginHelpers::DoFileDialog(NiPlugin* pkPlugin, NiString& strDirectory,
    NiString& strFile, NiString& strExtension, NiWindowRef hOwner, 
    const char* pcStartLocation)
{
    if (NiIsKindOf(NiImportPlugin, pkPlugin))
    {
        return DoImportFileDialog(strDirectory, strFile, strExtension,
            hOwner, pcStartLocation);
    }
    else if (NiIsKindOf(NiExportPlugin, pkPlugin))
    {
        return DoExportFileDialog(strDirectory, strFile, strExtension,
            hOwner, pcStartLocation);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiPluginHelpers::DoImportFileDialog(NiString& strDirectory,
    NiString& strFile, NiString& strExtension, NiWindowRef hOwner, 
    const char* pcStartLocation)
{
    return RunFileDialog(true, strDirectory, strFile, strExtension, hOwner, 
        pcStartLocation);
}
//---------------------------------------------------------------------------
bool NiPluginHelpers::DoExportFileDialog(NiString& strDirectory,
    NiString& strFile, NiString& strExtension, NiWindowRef hOwner, 
    const char* pcStartLocation)
{
    return RunFileDialog(false, strDirectory, strFile, strExtension, hOwner, 
        pcStartLocation);
}
//---------------------------------------------------------------------------
bool NiPluginHelpers::RunFileDialog(bool bImport, NiString& strDirectory, 
    NiString& strFile, NiString& strExtension, NiWindowRef hOwner, 
    const char* pcStartLocation)
{
    // Get module handle.
    NiModuleRef hModule = gs_hNiPluginToolkitHandle;

    // Build filter string.
    NiStringArray kNames;
    NiStringArray kExtensions;
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    NIASSERT(pkManager);
    if (bImport)
    {
        pkManager->GetImportStrings(kNames, kExtensions);
    }
    else
    {
        pkManager->GetExportStrings(kNames, kExtensions);
    }
    unsigned int ui, uiBuffSize = 0;
    for (ui = 0; ui < kNames.GetSize(); ui++)
    {
        NIASSERT(kNames.GetAt(ui));
        uiBuffSize += kNames.GetAt(ui).Length() + 1;
    }
    for (ui = 0; ui < kExtensions.GetSize(); ui++)
    {
        NIASSERT(kExtensions.GetAt(ui));
        uiBuffSize += kExtensions.GetAt(ui).Length() + 1;
    }
    uiBuffSize += 2;
    char* pcFilter = NiAlloc(char, uiBuffSize);
    char* pcPtr = pcFilter;
    int iFilterIndex = 0;
    for (ui = 0; ui < kNames.GetSize(); ui++)
    {
        NIASSERT(kNames.GetAt(ui) && kExtensions.GetAt(ui));
        NiString strTemp = kNames.GetAt(ui);
        unsigned int uiSize = strTemp.Length();

        if (uiSize == 0)
            continue;

        NiMemcpy(pcPtr, (const char*) strTemp, uiSize);
        pcPtr[strTemp.Length()] = '\0';
        pcPtr += strTemp.Length() + 1;
        strTemp = kExtensions.GetAt(ui); 
        if (strTemp.EqualsNoCase("*.nif"))
            iFilterIndex = ui + 1;

        uiSize = strTemp.Length();
        NiMemcpy(pcPtr, (const char*) strTemp, uiSize);
        pcPtr[strTemp.Length()] = '\0';
        pcPtr += strTemp.Length() + 1;
    }
    pcPtr[0] = '\0';

    // Create file name buffer.
    char acFile[1024];
    acFile[0] = '\0';

    // Set the initial directory.
    char acDir[1024];
    acDir[0] = '\0';

    if (pcStartLocation != NULL)
    {
        NiString strStartLoc = pcStartLocation;
        unsigned int uiLastBackslashLoc = strStartLoc.FindReverse('\\');
        unsigned int uiLastPeriodLoc = strStartLoc.FindReverse('.');
        if (uiLastBackslashLoc != NiString::INVALID_INDEX)
        {
            NiString strStartPath = strStartLoc.Left(uiLastBackslashLoc+1);
            NiString strFilename = NULL;
            
            if (uiLastPeriodLoc != NiString::INVALID_INDEX&& 
                uiLastPeriodLoc > uiLastBackslashLoc)
            {
                strFilename = strStartLoc.GetSubstring(uiLastBackslashLoc+1, 
                    strStartLoc.Length());
                NiSprintf(acFile, 1024, "%s", (const char*) strFilename);
                NiString strMyExt = strStartLoc.GetSubstring(uiLastPeriodLoc, 
                    strStartLoc.Length());
                for (ui = 0; ui < kExtensions.GetSize(); ui++)
                {
                    NIASSERT(kExtensions.GetAt(ui));
                    NiString strExtension = kExtensions.GetAt(ui);
                    strExtension = strExtension.GetSubstring(1, 
                        strExtension.Length());
                    if (strExtension.EqualsNoCase(strMyExt))
                        iFilterIndex = ui + 1;
                }

            }   
            else
            {
                strStartPath = strStartLoc + "\\";
            }

            NiSprintf(acDir, 1024, "%s", (const char*) strStartPath);
        }
    }

    if (acDir[0] != '\0' && bImport && ms_pcLastImportDir)
    {
        NiSprintf(acDir, 1024, "%s", ms_pcLastImportDir);
    }
    else if (acDir[0] != '\0' && !bImport && ms_pcLastExportDir)
    {
        NiSprintf(acDir, 1024, "%s", ms_pcLastExportDir);
    }

    // Set the title of dialog box.
    const char* pcTitle;
    if (bImport)
    {
        pcTitle = "Import File";
    }
    else
    {
        pcTitle = "Export File";
    }

    // Set dialog flags.
    DWORD dwFlags;
    if (bImport)
    {
        dwFlags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    }
    else
    {
        dwFlags = OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | 
            OFN_OVERWRITEPROMPT;
    }

    // Create OPENFILENAME structure.
    OPENFILENAME ofn;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hOwner;
    ofn.hInstance = hModule;
    ofn.lpstrFilter = pcFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = iFilterIndex;
    ofn.lpstrFile = acFile;
    ofn.nMaxFile = 1024;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = acDir;
    ofn.lpstrTitle = pcTitle;
    ofn.Flags = dwFlags;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = 0;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;

    // Run dialog box.
    BOOL bSuccess;
    if (bImport)
    {
        bSuccess = GetOpenFileName(&ofn);
    }
    else
    {
        bSuccess = GetSaveFileName(&ofn);
    }
    NiFree(pcFilter);

    // Handle return value.
    if(!bSuccess)
    {
        return false;
    }

    // Get directory, file name, and extension.
    
    strDirectory = ofn.lpstrFile;
    
    if (ofn.nFileExtension == 0)
    {
        ofn.nFileExtension = (WORD)strlen(ofn.lpstrFile) + 1;
        NiString strExt = kExtensions.GetAt(ofn.nFilterIndex - 1);
        strExtension = strExt.GetSubstring(1, strExt.Length());
    }
    else
    {
        strExtension = strDirectory.GetSubstring(ofn.nFileExtension - 1,
           strDirectory.Length());
    }
    strFile = strDirectory.GetSubstring(ofn.nFileOffset, ofn.nFileExtension -
            1);
    strDirectory = strDirectory.Left(ofn.nFileOffset);

    // Set last import/export directory.
    if (bImport)
    {
        SetLastImportDir(strDirectory);
    }
    else
    {
        SetLastExportDir(strDirectory);
    }

    return true;
}
//---------------------------------------------------------------------------
NiString NiPluginHelpers::ImportAndExecute(
    NiImportPluginInfo::ImportType eType, const char* pcFullPath)
{
    NiString strPath; 
    NiString strFile; 
    NiString strExtension;
    if (!pcFullPath)
    {
        if (!DoImportFileDialog(strPath, strFile, strExtension))
            return NiString();
    }
    else
    {
        ParseFullPath(pcFullPath, strPath, strFile, strExtension);
    }

    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    for (unsigned int ui = 0; ui < pkManager->GetTotalPluginCount(); ui++)
    {
        NiPlugin* pkPlugin = pkManager->GetPluginAt(ui);
        if (pkPlugin && NiIsKindOf(NiImportPlugin, pkPlugin))
        {
            if (((NiImportPlugin*)pkPlugin)->IsPrimaryImportPlugin() && 
                FileTypesMatch(strExtension, pkPlugin))
            {
                NiPluginInfo* pkPluginInfo = pkPlugin->GetDefaultPluginInfo();
                if (NiIsKindOf(NiImportPluginInfo, pkPluginInfo))
                {
                    NiImportPluginInfo* pkImportInfo = (NiImportPluginInfo*)
                        pkPluginInfo;
                    pkImportInfo->SetFile(strFile);
                    pkImportInfo->SetExtension(strExtension);
                    pkImportInfo->SetDirectory(strPath);
                    pkImportInfo->SetImportType(eType);
                    NiScriptInfo* pkScript = NiNew NiScriptInfo;
                    pkScript->SetName("ImportAndExecuteScript");
                    pkScript->SetType("Import");
                    pkScript->AddPluginInfo(pkImportInfo);
                    NiFramework& kFramework = NiFramework::GetFramework();
                    NiBatchExecutionResultPtr spResult = 
                        kFramework.ExecuteScript(pkScript);
                    
                    if (spResult && spResult->WasSuccessful())
                        return strPath + strFile + strExtension;
                    else
                        return NiString();
                }
            }
        }
    }

    return NiString();
}

//---------------------------------------------------------------------------
NiString NiPluginHelpers::ExportAndExecute(const char* pcFullPath)
{
    NiString strPath; 
    NiString strFile; 
    NiString strExtension;
    if (!pcFullPath)
    {
        if (!DoExportFileDialog(strPath, strFile, strExtension))
            return NiString();
    }
    else
    {
        ParseFullPath(pcFullPath, strPath, strFile, strExtension);
    }

    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    for (unsigned int ui = 0; ui < pkManager->GetTotalPluginCount(); ui++)
    {
        NiPlugin* pkPlugin = pkManager->GetPluginAt(ui);
        if (pkPlugin && NiIsKindOf(NiExportPlugin, pkPlugin))
        {
            if (((NiExportPlugin*)pkPlugin)->IsPrimaryExportPlugin() && 
                FileTypesMatch(strExtension, pkPlugin))
            {
                NiPluginInfo* pkPluginInfo = pkPlugin->GetDefaultPluginInfo();
                if (NiIsKindOf(NiExportPluginInfo, pkPluginInfo))
                {
                    NiExportPluginInfo* pkExportInfo = (NiExportPluginInfo*)
                        pkPluginInfo;
                    pkExportInfo->SetFile(strFile);
                    pkExportInfo->SetExtension(strExtension);
                    pkExportInfo->SetDirectory(strPath);
                    NiScriptInfo* pkScript = NiNew NiScriptInfo;
                    pkScript->SetName("ExportAndExecuteScript");
                    pkScript->SetType("Export");
                    pkScript->AddPluginInfo(pkExportInfo);
                    NiFramework& kFramework = NiFramework::GetFramework();
                    NiBatchExecutionResultPtr spResult = 
                        kFramework.ExecuteScript(pkScript);
                    
                    if (spResult && spResult->WasSuccessful())
                        return strPath + strFile + strExtension;
                    else
                        return NiString();
                }
            }
        }
    }

    return NiString();
}
//---------------------------------------------------------------------------
bool 
NiPluginHelpers::FileTypesMatch(NiString strExtension, NiPlugin* pkPlugin)
{
    if (pkPlugin && NiIsKindOf(NiImportPlugin, pkPlugin))
    {
        NiImportPlugin* pkImport = (NiImportPlugin*) pkPlugin;
        if (pkImport)
        {
            NiString strFile = pkImport->GetFileTypeString();
            NiStringTokenizer strTok(strFile);
            NiString strExt = strTok.GetNextToken(" \t\n;*");
            while (!strExt.IsEmpty())
            {
                if (strExtension.EqualsNoCase(strExt))
                    return true;

                strExt = strTok.GetNextToken(" \t\n;*");
            }
        }
    }
    else if(pkPlugin && NiIsKindOf(NiExportPlugin, pkPlugin))
    {
        NiExportPlugin* pkExport = (NiExportPlugin*) pkPlugin;
        if (pkExport)
        {
            NiString strFile = pkExport->GetFileTypeString();
            NiStringTokenizer strTok(strFile);
            NiString strExt = strTok.GetNextToken(" \t\n;*");
            while (!strExt.IsEmpty())
            {
                if (strExtension.EqualsNoCase(strExt))
                    return true;

                strExt = strTok.GetNextToken(" \t\n;*");
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiPluginHelpers::ParseFullPath(NiString strFullPath,
    NiString& strDirectory, NiString& strFile, NiString& strExtension)
{
    int iExt = strFullPath.FindReverse('.');
    if (iExt > -1)
    {
        strExtension = strFullPath.GetSubstring(iExt, strFullPath.Length());
    }
    else
    {
        iExt = strFullPath.Length();
    }

    int iName = strFullPath.FindReverse('\\');
    if (iName > -1)
    {
        iName++;
        strFile = strFullPath.GetSubstring(iName, iExt);
        strDirectory = strFullPath.GetSubstring(0, iName);
    }
    else
    {
        strFile = strFullPath.GetSubstring(0, iExt);
    }
}
//---------------------------------------------------------------------------
unsigned int NiPluginHelpers::GetVersionFromString(
    const char* pcVersionString)
{
    return NiStream::GetVersionFromString(pcVersionString);
}
//---------------------------------------------------------------------------
const char* NiPluginHelpers::GetLastImportDir()
{
    return ms_pcLastImportDir;
}
//---------------------------------------------------------------------------
const char* NiPluginHelpers::GetLastExportDir()
{
    return ms_pcLastExportDir;
}
//---------------------------------------------------------------------------
void NiPluginHelpers::SetLastImportDir(const char* pcLastImportDir)
{
    NiFree(ms_pcLastImportDir);
    ms_pcLastImportDir = NULL;
    if (pcLastImportDir)
    {
        size_t stLen = strlen(pcLastImportDir) + 1;
        ms_pcLastImportDir = NiAlloc(char, stLen);
        NiStrcpy(ms_pcLastImportDir, stLen, pcLastImportDir);
    }
}
//---------------------------------------------------------------------------
void NiPluginHelpers::SetLastExportDir(const char* pcLastExportDir)
{
    NiFree(ms_pcLastExportDir);
    ms_pcLastExportDir = NULL;
    if (pcLastExportDir)
    {
        size_t stLen = strlen(pcLastExportDir) + 1;
        ms_pcLastExportDir = NiAlloc(char, stLen);
        NiStrcpy(ms_pcLastExportDir, stLen, pcLastExportDir);
    }
}
//---------------------------------------------------------------------------
