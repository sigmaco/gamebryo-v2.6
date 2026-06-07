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

#include "NiScriptInfoDialogs.h"
#include "NiScriptTemplateManager.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "resource.h"
#include <NiSystem.h>
#include "NiStringTokenizer.h"
#include "NiPluginManager.h"
#include "NiScriptManagementDialog.h"
#include "NiScriptSelectionDialog.h"
#include "NiImportPluginInfo.h"
#include "NiExportPluginInfo.h"
#include "NiPluginHelpers.h"

//---------------------------------------------------------------------------
bool NiScriptInfoDialogs::FileTypesMatch(const char* pcExtension, 
    NiPluginInfo* pkInfo)
{
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    bool bTypesMatch = false; 
    // We've found an NiExportPlugin, need to fill in its data
    if (pkInfo && NiIsKindOf(NiExportPluginInfo, pkInfo))
    {
        // Find the matching plugin
        NiPlugin* pkPlugin = pkManager->GetMatchingPlugin(pkInfo);

        // This plugin must be an NiExportPlugin
        if (pkPlugin && NiIsKindOf(NiExportPlugin, pkPlugin))
        {
            NiExportPlugin* pkExport = (NiExportPlugin*)pkPlugin;
            NiString strFileType = pkExport->GetFileTypeString();
            NiStringTokenizer kTokenizer(strFileType);

            NiString strOriginalType = kTokenizer.GetNextToken("*\t; \n");
            NiString strType = strOriginalType;
                
            // Search through all of the file types for this plugin to
            // see if we've already got the correct file, path, & extension
            while (!strType.IsEmpty() && !bTypesMatch)
            {
                bTypesMatch = strType.EqualsNoCase(pcExtension);
                if (!bTypesMatch)
                    strType = kTokenizer.GetNextToken("*\t; \n");
                else
                    return true;
            }
        }
    }
    // We've found an NiImportPlugin, need to fill in its data
    else if (pkInfo && NiIsKindOf(NiImportPluginInfo, pkInfo))
    {
        // Find the matching plugin
        NiPlugin* pkPlugin = pkManager->GetMatchingPlugin(pkInfo);

        // This plugin must be an NiExportPlugin
        if (pkPlugin && NiIsKindOf(NiImportPlugin, pkPlugin))
        {
            NiImportPlugin* pkImport = (NiImportPlugin*)pkPlugin;
            NiString strFileType = pkImport->GetFileTypeString();
            NiStringTokenizer kTokenizer(strFileType);

            NiString strOriginalType = kTokenizer.GetNextToken("*\t; \n");
            NiString strType = strOriginalType;
                
            // Search through all of the file types for this plugin to
            // see if we've already got the correct file, path, & extension
            while (!strType.IsEmpty() && !bTypesMatch)
            {
                bTypesMatch = strType.EqualsNoCase(pcExtension);
                if (!bTypesMatch)
                    strType = kTokenizer.GetNextToken("*\t; \n");
                else
                    return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
/// Useful method for generating the import and export filenames for an 
/// NiScriptInfo object. 
void NiScriptInfoDialogs::CompileImportExportInfo(const char* pcBaseFilename,  
    NiScriptInfo* pkScript, bool bPromptForFiles, bool bSilentRunning)
{
    if (!pkScript)
        return;

    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    NiString strPath, strFile, strExtension;
    NiPluginHelpers::ParseFullPath(pcBaseFilename, strPath, strFile,
        strExtension);

    bool bMatchesExtension = false;
    unsigned int ui = 0;
    for (; ui < pkScript->GetPluginInfoCount() && 
        !bMatchesExtension; ui++)
    {
        NiPluginInfo* pkInfo = pkScript->GetPluginInfoAt(ui);
        bMatchesExtension = FileTypesMatch(strExtension, pkInfo);
    }

    if (!bMatchesExtension && !bSilentRunning)
    {
        char acString[512];
        NiSprintf(acString, 512, "No plugin in your script uses the extension "
            "\"%s\".\nIf you wish to export this a file with this extension,"
            " please add the proper export plugin to the script in the "
            "script editor.",
            (const char*) strExtension);
        NiMessageBox(acString, "File extension warning");
    }

    // Go through all of the NiPluginInfo objects to find all import and
    // export info object
    for (ui = 0; ui < pkScript->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkInfo = pkScript->GetPluginInfoAt(ui);

        // We've found an NiExportPlugin, need to fill in its data
        if (pkInfo && NiIsKindOf(NiExportPluginInfo, pkInfo))
        {
            // Find the matching plugin
            NiExportPluginInfo* pkExportInfo = (NiExportPluginInfo*) pkInfo;
            NiPlugin* pkPlugin = pkManager->GetMatchingPlugin(pkInfo);

            // This plugin must be an NiExportPlugin
            if (pkPlugin && NiIsKindOf(NiExportPlugin, pkPlugin))
            {
                NiString strInputFile = strFile;
                NiString strInputPath = strPath;
                NiString strInputExtension = strExtension;

                NiExportPlugin* pkExport = (NiExportPlugin*)pkPlugin;
                NiString strFileType = pkExport->GetFileTypeString();
                NiStringTokenizer kTokenizer(strFileType);

                NiString strOriginalType = kTokenizer.GetNextToken("*\t; \n");
                NiString strType = strOriginalType;
                bool bTypesMatch = false; 
                    
                // Search through all of the file types for this plugin to
                // see if we've already got the correct file, path, & extension
                while (!strType.IsEmpty() && !bTypesMatch)
                {
                    bTypesMatch = strType.EqualsNoCase(strExtension);
                    if (!bTypesMatch)
                        strType = kTokenizer.GetNextToken("*\t; \n");   
                }

                // If we don't have the correct path, file, & extension and we
                // want to prompt the user for them, prompt the user
                if (!bTypesMatch && bPromptForFiles && !bSilentRunning)
                {
                    if (!NiPluginHelpers::DoFileDialog(pkExport, strInputPath, 
                        strInputFile, strInputExtension))
                    {
                        strInputExtension = strOriginalType;
                    }
                }
                // Otherwise, use the default extension
                else if (!bTypesMatch)
                {
                    strInputExtension = strOriginalType;
                }

                pkExportInfo->SetFile(strInputFile);
                pkExportInfo->SetDirectory(strInputPath);
                pkExportInfo->SetExtension(strInputExtension);
            }
        }
        // We've found an NiImportPlugin, need to fill in its data
        else if (pkInfo && NiIsKindOf(NiImportPluginInfo, pkInfo))
        {
            // Find the matching plugin
            NiImportPluginInfo* pkImportInfo = (NiImportPluginInfo*) pkInfo;
            NiPlugin* pkPlugin = pkManager->GetMatchingPlugin(pkInfo);

            // This plugin must be an NiImportPlugin
            if (pkPlugin && NiIsKindOf(NiImportPlugin, pkPlugin))
            {
                NiString strInputFile = strFile;
                NiString strInputPath = strPath;
                NiString strInputExtension = strExtension;

                NiImportPlugin* pkImport = (NiImportPlugin*)pkPlugin;
                NiString strFileType = pkImport->GetFileTypeString();
                NiStringTokenizer kTokenizer(strFileType);

                NiString strOriginalType = kTokenizer.GetNextToken("*\t; \n");
                NiString strType = strOriginalType;
                bool bTypesMatch = false; 
                    
                // Search through all of the file types for this plugin to
                // see if we've already got the correct file, path, & extension
                while (!strType.IsEmpty() && !bTypesMatch)
                {
                    bTypesMatch = strType.EqualsNoCase(strExtension);
                    if (!bTypesMatch)
                        strType = kTokenizer.GetNextToken("*\t; \n");   
                }

                // If we don't have the correct path, file, & extension and we
                // want to prompt the user for them, prompt the user
                if (!bTypesMatch && bPromptForFiles && !bSilentRunning)
                {
                    if (!NiPluginHelpers::DoFileDialog(pkImport, strInputPath, 
                        strInputFile, strInputExtension))
                    {
                        strInputExtension = strOriginalType;
                    }
                }
                // Otherwise, use the default extension
                else if (!bTypesMatch)
                {
                    strInputExtension = strOriginalType;
                }

                pkImportInfo->SetFile(strInputFile);
                pkImportInfo->SetDirectory(strInputPath);
                pkImportInfo->SetExtension(strInputExtension);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiScriptInfoDialogs::DoManagementDialog(
    NiScriptInfo* pkDefaultInfo, NiScriptInfoSet* pkInputSet, 
    NiWindowRef kWindow, NiString strTypes)
{
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;

    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    
    pkDefaultInfo = GenerateDefaultScript(pkDefaultInfo, pkInputSet, true, 
        false);
    NIASSERT(pkDefaultInfo);
    
    NiScriptInfoSetPtr spClonedSet = pkInputSet->Clone();

    spClonedSet->ReplaceScript(pkDefaultInfo);
    NiScriptInfoPtr spOriginalInfo = pkDefaultInfo->Clone();
    NiScriptManagementDialog kDialog(kModuleRef, pkDefaultInfo, spClonedSet, 
        kWindow, strTypes);
    int iReturn = kDialog.DoModal();

    NiScriptInfoPtr spInfo = kDialog.m_spInfo;
    if (iReturn == IDOK)
    {
        NiScriptInfoSet& kMasterSet = pkManager->GetScriptInfoSet();
        for (unsigned int ui = 0; ui < spClonedSet->GetScriptCount(); ui++)
        {
            NiScriptInfo* pkNewScript = spClonedSet->GetScriptAt(ui);
            unsigned int uiIndex = 
                kMasterSet.GetScriptIndex(pkNewScript->GetName());
            
            if (pkInputSet != (&kMasterSet))
                pkInputSet->ReplaceScript(pkNewScript);

            if (uiIndex == NIPT_INVALID_INDEX)
            {
                pkManager->AddScript(pkNewScript, 
                    spClonedSet->IsTemplate(pkNewScript));
            }
            else 
            {
                NiScriptInfo* pkMasterCopy = kMasterSet.GetScriptAt(uiIndex);
                bool bMasterIsTemplate = kMasterSet.IsTemplate(pkMasterCopy);
                bool bNewIsTemplate = spClonedSet->IsTemplate(pkNewScript);

                if (pkMasterCopy->Equals(pkNewScript))
                {
                    if (!pkMasterCopy->GetFile().
                        EqualsNoCase(pkNewScript->GetFile()))
                    {
                        pkMasterCopy->SetFile(pkNewScript->GetFile());
                    }
                    if (bMasterIsTemplate != bNewIsTemplate)
                        kMasterSet.SetTemplate(pkMasterCopy, bNewIsTemplate);
                    continue;
                }

                // We want to copy the settings to the master list in 
                // the following cases:
                // Master is template && New is template
                // Master is not a template and new is a template
                // Master is not a template and new is not a template
                if (bMasterIsTemplate && bNewIsTemplate)
                    pkManager->ReplaceScript(pkNewScript);

                if (!bMasterIsTemplate)
                    pkManager->ReplaceScript(pkNewScript);
            }

        }

        for (unsigned int uj = 0; uj < kDialog.m_spRemovedScripts->
            GetScriptCount(); uj++)
        {
            NiScriptInfo* pkInfo = kDialog.m_spRemovedScripts->GetScriptAt(uj);
            pkInputSet->RemoveScript(pkInfo->GetName());
        }
        return spInfo;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiScriptInfoDialogs::DoSelectionDialog(
    NiScriptInfo* pkDefaultInfo, NiScriptInfoSet* pkInputSet,
    NiWindowRef kWindow, NiString strTypes)
{
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;
    bool bDestroyInputSet = false;

    pkDefaultInfo = GenerateDefaultScript(pkDefaultInfo, pkInputSet, 
        true, false);
    NIASSERT(pkDefaultInfo);
    
    //NiScriptInfoPtr spOriginalInfo = pkDefaultInfo->Clone();
    NiScriptSelectionDialog kDialog(kModuleRef, pkDefaultInfo, pkInputSet,
        kWindow, strTypes);
    int iReturn = kDialog.DoModal();
    
    if (bDestroyInputSet)
        NiDelete pkInputSet;
    
    NiScriptInfoPtr spInfo = kDialog.ms_spInfo;
    if (iReturn == IDOK)
        return spInfo;
    else
        return NULL;
}

//---------------------------------------------------------------------------
/// Creates a modal dialog box that allows a user to save
/// an existing script
ReturnCode NiScriptInfoDialogs::DoScriptSaveDialog(NiScriptInfo* pkScript,
    NiWindowRef kWindow, bool bPromptForLoc)
{
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;

    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    if (!pkScript)
        return NULL_INPUT_SCRIPT;

    if (!bPromptForLoc)
    {
        
        NiString strFilePath = pkScript->GetFile();
        
        if (!strFilePath.IsEmpty())
        {
            NiScriptInfo* pkInfo = pkManager->SaveScript(strFilePath, 
                pkScript, false);
            if (!pkInfo)
            {
                return pkManager->GetLastReturnCode();
            }
            else
            {
                return SUCCESS;
            }
        }
    }

    LPOPENFILENAME pkOpenFile = NiExternalNew OPENFILENAME;
    pkOpenFile->lStructSize = sizeof(OPENFILENAME);
    pkOpenFile->hwndOwner = kWindow;
    pkOpenFile->hInstance = kModuleRef;

    NiString strNames("All Script Files (");
    NiString strExtensions;

    NiScriptWriterPtrSet& kAllWriters = pkManager->GetAllWriters();

    for (unsigned int ui = 0; ui < kAllWriters.GetSize(); ui++)
    {
        NiScriptWriter* pkWriter = kAllWriters.GetAt(ui);
        if (pkWriter)
        {
            NiString strExt = pkWriter->GetFileExtensions();
            strNames += strExt;
            strExtensions += strExt;
        }
    }

    strNames += ")";

    char acFile[1024];
    if (pkScript->GetFile().IsEmpty())
    {
        NiSprintf(acFile, 512, "%s.script", (const char*) pkScript->GetName());
    }
    else
    {
        NiSprintf(acFile, 512, "%s", (const char*) pkScript->GetFile());
    }
    char acFileName[MAX_PATH + 1];
    char* pcString = NiAlloc(char, 
        strNames.Length() + strExtensions.Length() + 3);
           
    unsigned int uiSize = strNames.Length() + 1;
    NiMemcpy(pcString, (const char*) strNames, uiSize);

    uiSize = strExtensions.Length() + 1;
    NiMemcpy(pcString + strNames.Length() + 1, (const char*) strExtensions,
        uiSize);

    pcString[strNames.Length() + strExtensions.Length() + 2] = '\0';
    pkOpenFile->lpstrFilter = pcString;
    pkOpenFile->lpstrCustomFilter = NULL; 
    pkOpenFile->nMaxCustFilter = 0; 
    pkOpenFile->nFilterIndex = 0;  
    pkOpenFile->lpstrFile = acFile;
    pkOpenFile->nMaxFile = 1024;
    pkOpenFile->lpstrFileTitle = acFileName; 
    pkOpenFile->nMaxFileTitle = MAX_PATH; 
    pkOpenFile->lpstrInitialDir = pkManager->GetLastDirectoryAdded(); 
    pkOpenFile->lpstrTitle = "Save Script"; 
    pkOpenFile->Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | 
        OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT; 
    pkOpenFile->nFileOffset = 0; 
    pkOpenFile->nFileExtension = 0; 
    pkOpenFile->lpstrDefExt = NULL; 
    pkOpenFile->lCustData = NULL; 
    pkOpenFile->lpfnHook = NULL; 
    pkOpenFile->lpTemplateName = NULL; 

    BOOL bReturn = GetSaveFileName(pkOpenFile);
    NiFree(pcString);

    if(bReturn)
    {
        NiString strFilePath = pkOpenFile->lpstrFile;

        unsigned int uiFindDot = strFilePath.FindReverse('.');
        if (uiFindDot == NIPT_INVALID_INDEX)
        {
            NiScriptWriter* pkWriter = kAllWriters.GetAt(0);
            if (pkWriter)
            {
               NiString strExt = pkWriter->GetFileExtensions();
               strFilePath += strExt.Right(strExt.Length()-1);
            }
        }

        NiString strJustFilename(acFileName);
        uiFindDot = strJustFilename.FindReverse('.');
        if (uiFindDot != NIPT_INVALID_INDEX)
            strJustFilename = strJustFilename.Left(uiFindDot);

        if (!strJustFilename.EqualsNoCase(pkScript->GetName()))
        {            
            int iReturn = MessageBox(NULL, "The filename you selected does "
                "not match the script's name.\n"
                "Would you like to rename the script to avoid potential "
                "conflicts later?",
                "Script filename doesn't match script name",
                MB_YESNO | MB_ICONQUESTION
            );  
            if (iReturn == IDYES)
                pkScript->SetName(strJustFilename);

        }
        NiScriptInfo* pkInfo = pkManager->SaveScript(strFilePath, pkScript, 
            false);

        if (!pkInfo)
        {
            NiExternalDelete pkOpenFile;
            return pkManager->GetLastReturnCode();
        }
    }
    else
    {
        DWORD dwError = CommDlgExtendedError();
        NiExternalDelete pkOpenFile;
        if (dwError == FNERR_INVALIDFILENAME)
            return IO_FAILURE;
        else
            return CANCELLED;
    }
    return SUCCESS;
}
//---------------------------------------------------------------------------
/// Creates a modal dialog box that allows a user to open
/// scripts and adds them to the known script list
NiScriptInfo* NiScriptInfoDialogs::DoScriptOpenDialog(NiWindowRef kWindow,
    bool bAddToManager)
{
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;

    LPOPENFILENAME pkOpenFile = NiExternalNew OPENFILENAME;
    pkOpenFile->lStructSize = sizeof(OPENFILENAME);
    pkOpenFile->hwndOwner = kWindow;
    pkOpenFile->hInstance = kModuleRef;

    NiString strNames("All Script Files (");
    NiString strExtensions;

    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    
    NiScriptReaderPtrSet& kAllReaders = pkManager->GetAllReaders();

    for (unsigned int ui = 0; ui < kAllReaders.GetSize(); ui++)
    {
        NiScriptReader* pkReader = kAllReaders.GetAt(ui);
        if (pkReader)
        {
            NiString strExt = pkReader->GetFileExtensions();
            strNames += strExt;
            strExtensions += strExt;
        }
    }

    strNames += ")";

    char acFile[1024];
    acFile[0] = '\0';
    unsigned int uiDestSize = strNames.Length() + strExtensions.Length() + 3;
    char* pcString = NiAlloc(char, uiDestSize);

    unsigned int uiSize = strNames.Length() + 1;
    NiMemcpy(pcString, uiDestSize, (const char*) strNames, uiSize);

    uiSize = strExtensions.Length() + 1;
    NiMemcpy(pcString + strNames.Length() + 1, 
        uiSize, // strExtensions.Length() + 2
        (const char*) strExtensions, uiSize);

    pcString[strNames.Length() + strExtensions.Length() + 2] = '\0';
    pkOpenFile->lpstrFilter = pcString;
    pkOpenFile->lpstrCustomFilter = NULL; 
    pkOpenFile->nMaxCustFilter = 0; 
    pkOpenFile->nFilterIndex = 0;  
    pkOpenFile->lpstrFile = acFile;
    pkOpenFile->nMaxFile = 1024;
    pkOpenFile->lpstrFileTitle = NULL; 
    pkOpenFile->nMaxFileTitle = 0; 
    pkOpenFile->lpstrInitialDir = pkManager->GetLastDirectoryAdded(); 
    pkOpenFile->lpstrTitle = "Open Script"; 
    pkOpenFile->Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | 
        OFN_PATHMUSTEXIST; 
    pkOpenFile->nFileOffset = 0; 
    pkOpenFile->nFileExtension = 0; 
    pkOpenFile->lpstrDefExt = NULL; 
    pkOpenFile->lCustData = NULL; 
    pkOpenFile->lpfnHook = NULL; 
    pkOpenFile->lpTemplateName = NULL; 

    BOOL bReturn = GetOpenFileName(pkOpenFile);
    NiFree(pcString);
    if(bReturn)
    {
        NiString strFilePath = pkOpenFile->lpstrFile;
        NiScriptInfo* pkInfo = pkManager->OpenScript(strFilePath);

        if (!pkInfo)
        {
            NiExternalDelete pkOpenFile;
            return NULL;
        }

        if (bAddToManager)
        {
            ReturnCode eCode = pkManager->AddScript(pkInfo);
            if (eCode != SUCCESS)
            {
                NiExternalDelete pkOpenFile;
                NiDelete pkInfo;
                return NULL;
            }
        }
        NiExternalDelete pkOpenFile;
        return pkInfo;

    }
    else
    {
        CommDlgExtendedError();
        NiExternalDelete pkOpenFile;
        return NULL;
    }
}

//---------------------------------------------------------------------------
/// Useful method for generating the default script for execution. It adds
/// the generated default script to the NiScriptInfoSet if it doesn't already
/// exist
/// Check performed:
/// 1) The input script exists. If it doesn't, use the last script executed by
/// the plugin manager.
/// 2) The input script uses valid plugins loaded by the framework. If not,  
/// strip off the offending plugin info objects and return
/// 3) If the script matches an existing template from the NiScriptInfoSet, 
/// check to see if it differs 
/// from the template. If it differs, ask the user if they want to use the
/// template or the original input script.
NiScriptInfo* NiScriptInfoDialogs::GenerateDefaultScript(NiScriptInfo* pkInfo, 
    NiScriptInfoSet*& pkInfoSet, bool bPromptUser, bool bForceTemplateUse)
{
    // Does the input info set exist? If no, use the default one
    if (pkInfoSet == NULL)
        pkInfoSet = GenerateDefaultScriptInfoSet();

    NiScriptInfo* pkMatchedInInfoSet = NULL;
    
    if (pkInfo)
    {   
        // Find out if the current script will be ignored by the set
        // Find out if set already contains a script with this name
        if (pkInfoSet->IgnoreScript(pkInfo))
        {
            pkInfo = NULL;
            NiMessageBox("The chosen script uses unknown plugins. " 
                "Using the last script executed instead.","Invalid Script");
        }
        else
        {
            pkMatchedInInfoSet = pkInfoSet->GetScript(pkInfo->GetName());
        }
    }

    // Does the input script info exist? If no, use the last executed script.
    // If yes, is it a template?
    if (pkInfo == NULL)
    {
        pkInfo = NiPluginManager::GetInstance()->GetLastExecutedScript();
        NIASSERT(pkInfo);

        // Add to the script info set if necessary
        pkInfoSet->ReplaceScript(pkInfo);
        return pkInfo;
    }
    else 
    {
        // If this matches a template and we differ from the template,
        // do we use the template or the input version?
        if (!pkMatchedInInfoSet)
        {
            // Add the custom script to the info set. If we fail at this,
            // grab the first script you can and pray that it is okay
            if (pkInfoSet->AddScript(pkInfo, false, true) != SUCCESS)
            {
                pkInfo = pkInfoSet->GetScriptAt(0);
                NIASSERT(pkInfo);
                return pkInfo;
            }
            return pkInfo;
        }
        // If we don't differ from the one already in the set, use the
        // one in the set.
        else if (pkMatchedInInfoSet->Equals(pkInfo))
        {
            return pkMatchedInInfoSet;
        }
        // If we do differ from the one already in the set and the match
        // in the set is a template, do we use the template?
        else if (pkInfoSet->IsTemplate(pkMatchedInInfoSet))
        {
            int iReturn = IDNO;
            if (!bPromptUser && bForceTemplateUse)
            {
                iReturn = IDYES;
            }
            else if (bPromptUser)
            {
                iReturn = MessageBox(NULL, "The current script name matches "
                    "a known template, but differs in content. Would you "
                    "rather use the template?", "Similar Template Found", 
                    MB_YESNO);
            }

            if (iReturn == IDYES)
            {
                return pkMatchedInInfoSet;
            }
            else
            {
                pkInfoSet->ReplaceScript(pkInfo);
                return pkInfo;
            }
        }
        // We aren't a template and we differ from the match already in the set
        // replace the match already in the set with our custom version.
        else
        {
            // Add to the script info set if necessary
            pkInfoSet->ReplaceScript(pkInfo);
            return pkInfo;
        }

    }
    
}

//---------------------------------------------------------------------------
/// Useful method for generating the default script info set for the execution
/// It adds the existing NiScriptInfo object to the set of template scripts
/// from the NiScriptTemplateManager
NiScriptInfoSet* 
NiScriptInfoDialogs::GenerateDefaultScriptInfoSet()
{
    NiScriptTemplateManager* pkScriptManager = 
        NiScriptTemplateManager::GetInstance();
    NiScriptInfoSet* pkSet = NiNew NiScriptInfoSet();
    pkSet->IgnorePluginClass("NiMFCPlugin");
    pkSet->Copy(&pkScriptManager->GetScriptInfoSet());
    return pkSet;
}

//---------------------------------------------------------------------------
unsigned int 
NiScriptInfoDialogs::CountNumberOfExportPluginInfos(NiScriptInfo* pkScript)
{
    if (pkScript == NULL)
        return 0;

    unsigned int uiCount = 0;

    for (unsigned int ui = 0; ui < pkScript->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPluginInfo = pkScript->GetPluginInfoAt(ui);
        if (pkPluginInfo && NiIsKindOf(NiExportPluginInfo, pkPluginInfo))
            uiCount++;
    }

    return uiCount;
}

//---------------------------------------------------------------------------
unsigned int 
NiScriptInfoDialogs::CountNumberOfImportPluginInfos(NiScriptInfo* pkScript)
{
    if (pkScript == NULL)
        return 0;

    unsigned int uiCount = 0;

    for (unsigned int ui = 0; ui < pkScript->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPluginInfo = pkScript->GetPluginInfoAt(ui);
        if (pkPluginInfo && NiIsKindOf(NiImportPluginInfo, pkPluginInfo))
            uiCount++;
    }

    return uiCount;
}
//---------------------------------------------------------------------------
