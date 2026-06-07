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

#include "NiPluginManager.h"
#include "NiPluginDLLInterface.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "NiScriptInfo.h"
#include "NiFileFinder.h"
#include "NiFramework.h"
#include "NiSharedDataList.h"
#include "NiScriptTemplateManager.h"

#include <NiMeshLibType.h>
#include <NiMeshUtilities.h>

NiImplementRootRTTI(NiPluginManager);

NiPluginManager* NiPluginManager::ms_pkThis = NULL;
NiScriptInfoPtr NiPluginManager::ms_spCurrentScriptInfo = NULL;
//---------------------------------------------------------------------------
void NiPluginManager::CreateInstance()
{
    NIASSERT(ms_pkThis == NULL);
    ms_pkThis = NiNew NiPluginManager;
}
//---------------------------------------------------------------------------
void NiPluginManager::DestroyInstance()
{
    NIASSERT(ms_pkThis != NULL);
    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}
//---------------------------------------------------------------------------
NiPluginManager* NiPluginManager::GetInstance()
{
    return ms_pkThis;
}
//---------------------------------------------------------------------------
NiPluginManager::NiPluginManager()
{
    ms_spCurrentScriptInfo = 0;
}
//---------------------------------------------------------------------------
NiPluginManager::~NiPluginManager()
{
    ms_spCurrentScriptInfo = 0;

    m_kImportPlugins.RemoveAll();
    m_kExportPlugins.RemoveAll();
    m_kViewerPlugins.RemoveAll();
    m_kProcessPlugins.RemoveAll();
    m_kAllPlugins.RemoveAll();

    for (unsigned int ui = 0; ui < m_kDLLHandles.GetSize(); ui++)
    {
        FreeLibrary(m_kDLLHandles.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiPluginManager::AddLibrary(NiString strPath)
{
    NiModuleRef hPlugin = LoadLibrary(strPath);
    if (!hPlugin)
    {
        return false;
    }

    NiGetCompilerVersionFunction pfnGetCompilerVersionFunc =
        (NiGetCompilerVersionFunction)GetProcAddress(hPlugin, 
        "GetCompilerVersion");
    if (pfnGetCompilerVersionFunc)
    {
        unsigned int uiPluginVersion = pfnGetCompilerVersionFunc();
        if (uiPluginVersion != (_MSC_VER))
        {
            FreeLibrary(hPlugin);
            return false;
        }
    }
    else
    {
        // Older version - run it anyway
    }

    m_kDLLHandles.Add(hPlugin);

    NiGetPluginsFunction pfnGetPluginsFunc = (NiGetPluginsFunction)
        GetProcAddress(hPlugin, "GetPlugins");
    if (!pfnGetPluginsFunc)
    {
        return false;
    }

    NiTPtrSet<NiPluginPtr, NiTNewInterface<NiPluginPtr> > kPluginArray;
    pfnGetPluginsFunc(kPluginArray);
    if (kPluginArray.GetSize() == 0)
    {
        return false;
    }

    NiString strPluginPath = strPath;
    int iLoc = strPath.FindReverse('\\');
    strPluginPath = strPath.Left(iLoc+1);
        
    for (unsigned int ui = 0; ui < kPluginArray.GetSize(); ui++)
    {
        NiPlugin* pkPlugin = kPluginArray.GetAt(ui);
        AddPlugin(pkPlugin);
        pkPlugin->SetModule(hPlugin);
        pkPlugin->SetAbsolutePluginPath(strPluginPath);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPluginManager::AddPlugin(NiPlugin* pkPlugin)
{
    if (NiIsKindOf(NiImportPlugin, pkPlugin))
        AddImporterPlugin((NiImportPlugin*)pkPlugin);
    else if (NiIsKindOf(NiExportPlugin, pkPlugin))
        AddExporterPlugin((NiExportPlugin*)pkPlugin);
    else if (NiIsKindOf(NiViewerPlugin, pkPlugin))
        AddViewerPlugin((NiViewerPlugin*) pkPlugin);
    else
        m_kProcessPlugins.Add(pkPlugin);
    
    m_kAllPlugins.Add(pkPlugin);
}

//---------------------------------------------------------------------------
NiBatchExecutionResultPtr NiPluginManager::ExecuteScript(
    NiScriptInfo* pkScript,  bool bRememberAsLastExecutedScript)
{
    NiSharedDataList::LockSharedData();

    NiBatchExecutionResultPtr spBatchResult = NiNew NiBatchExecutionResult(
        NiExecutionResult::EXECUTE_SUCCESS);

    

    for (unsigned int ui = 0; ui < pkScript->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPluginInfo = pkScript->GetPluginInfoAt(ui);
        bool bRanPluginInfo = false;
        for (unsigned int uj = 0; uj < m_kAllPlugins.GetSize() && 
            !bRanPluginInfo; uj++)
        {
            NiPlugin* pkPlugin = m_kAllPlugins.GetAt(uj);
            if (pkPlugin->CanExecute(pkPluginInfo))
            {
                NiString strScriptProgress = pkPluginInfo->GetClassName();
                NiOutputDebugString(strScriptProgress + " running...\n");
                bRanPluginInfo = true;

                // Execute the plugin
                NiExecutionResultPtr spResult =
                    pkPlugin->Execute(pkPluginInfo);
                spBatchResult->m_pkPluginResults.Add(spResult);

                if (spResult->m_eReturnCode !=
                    NiExecutionResult::EXECUTE_SUCCESS)
                {
                    spBatchResult->m_eReturnCode =
                        NiExecutionResult::EXECUTE_FAILURE;
                }
            }
        }
    }

    if (bRememberAsLastExecutedScript)
    {
        ms_spCurrentScriptInfo = pkScript;
        NiScriptTemplateManager::GetInstance()->ReplaceScript(pkScript);
    }

    NiSharedDataList::UnlockSharedData();

    return spBatchResult;
}
//---------------------------------------------------------------------------
void NiPluginManager::GetImportStrings(
    NiStringArray& kFileDescriptorStrings,
    NiStringArray& kFileTypeStrings)
{
    unsigned int uiNumImportPlugins = m_kImportPlugins.GetSize();

    kFileDescriptorStrings.SetSize(uiNumImportPlugins);
    kFileTypeStrings.SetSize(uiNumImportPlugins);

    for (unsigned int ui = 0; ui < uiNumImportPlugins; ui++)
    {
        NiImportPlugin* pkImportPlugin = m_kImportPlugins.GetAt(ui);
        kFileDescriptorStrings.Add(pkImportPlugin->GetFileDescriptorString());
        kFileTypeStrings.Add(pkImportPlugin->GetFileTypeString());
    }
}
//---------------------------------------------------------------------------
void NiPluginManager::GetExportStrings(
    NiStringArray& kFileDescriptorStrings,
    NiStringArray& kFileTypeStrings)
{
    unsigned int uiNumExportPlugins = m_kExportPlugins.GetSize();

    kFileDescriptorStrings.SetSize(uiNumExportPlugins);
    kFileTypeStrings.SetSize(uiNumExportPlugins);

    for (unsigned int ui = 0; ui < uiNumExportPlugins; ui++)
    {
        NiExportPlugin* pkExportPlugin = m_kExportPlugins.GetAt(ui);
        kFileDescriptorStrings.Add(pkExportPlugin->GetFileDescriptorString());
        kFileTypeStrings.Add(pkExportPlugin->GetFileTypeString());
    }
}
//---------------------------------------------------------------------------
void NiPluginManager::AddPluginDirectory(const char* pcPath, bool bRecurse,
    bool bSilentRunning, const char* pcIgnore)
{
#ifdef NIDEBUG
    NiString strExt = ".dld";
#else
#ifdef NIRELEASE
    NiString strExt = ".dln";
#else
    NiString strExt = ".dll";
#endif
#endif

    NiString strErrors;

    NiFileFinder kFinder(pcPath, bRecurse, strExt);

    if (!kFinder.HasMoreFiles())
    {
        strErrors += "Unable to find any plug-ins at:\n";
        strErrors += pcPath;
    }

    bool bInvalidLoadingAdded = false;
    while(kFinder.HasMoreFiles())
    {
        NiFoundFile* pkFile = kFinder.GetNextFile();
        if (pkFile && (!pcIgnore || !pkFile->m_strPath.Contains(pcIgnore)))
        {
            bool bAdded = AddLibrary(pkFile->m_strPath);
            if (!bAdded)
            {
                if (!bInvalidLoadingAdded)
                {
                    strErrors += "Unable to load the following plug-ins:\n";
                    bInvalidLoadingAdded = true;
                }
                strErrors += pkFile->m_strPath + "\n";
            }
        }
    }

    if (!bSilentRunning && (strErrors.Length() != 0))
    {
        NiMessageBox(strErrors, "Error Loading Gamebryo Plug-Ins");
    }
}
//---------------------------------------------------------------------------
NiPluginPtrSet& NiPluginManager::GetAllPlugins()
{
    return m_kAllPlugins;
}
//---------------------------------------------------------------------------
NiPlugin* NiPluginManager::GetPluginByClass(NiString strClass)
{
    for (unsigned int ui = 0; ui < m_kAllPlugins.GetSize(); ui++)
    {
        NiPlugin* pkPlugin = m_kAllPlugins.GetAt(ui);
        if (pkPlugin && strClass.Equals(pkPlugin->GetRTTI()->GetName()))
            return pkPlugin;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiPlugin* NiPluginManager::GetPluginByName(NiString strPlugin)
{
    for (unsigned int ui = 0; ui < m_kAllPlugins.GetSize(); ui++)
    {
        NiPlugin* pkPlugin = m_kAllPlugins.GetAt(ui);
        if (pkPlugin && strPlugin.Equals(pkPlugin->GetName()))
            return pkPlugin;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiPlugin* NiPluginManager::GetPluginAt(unsigned int uiIndex)
{
    if (uiIndex < m_kAllPlugins.GetSize())
        return m_kAllPlugins.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiPluginManager::GetPluginIndex(NiString strClass)
{
    for (unsigned int ui = 0; ui < m_kAllPlugins.GetSize(); ui++)
    {
        NiPlugin* pkPlugin = m_kAllPlugins.GetAt(ui);
        if (pkPlugin && strClass.Equals(pkPlugin->GetRTTI()->GetName()))
            return ui;
    }
    return NIPT_INVALID_INDEX;
}
//---------------------------------------------------------------------------
unsigned int NiPluginManager::GetTotalPluginCount()
{
    return m_kAllPlugins.GetSize();
}
//---------------------------------------------------------------------------
bool NiPluginManager::AddImporterPlugin(NiImportPlugin* pkImporter)
{
    m_kImportPlugins.Add(pkImporter);
    return true;
}
//---------------------------------------------------------------------------
bool NiPluginManager::AddExporterPlugin(NiExportPlugin* pkExporter)
{
    m_kExportPlugins.Add(pkExporter);
    return true;
}

//---------------------------------------------------------------------------
bool NiPluginManager::AddViewerPlugin(NiViewerPlugin* pkViewer)
{
    m_kViewerPlugins.Add(pkViewer);
    return true;
}
//---------------------------------------------------------------------------
bool NiPluginManager::DoManagementDialog(NiPluginInfo* pkInitialInfo, 
                                         NiWindowRef hWndParent)
{ 
    for (unsigned int ui = 0; ui < GetTotalPluginCount(); 
        ui++)
    {
        NiPlugin* pkPlugin = GetPluginAt(ui);
        if (pkPlugin && pkPlugin->CanExecute(pkInitialInfo))
        {
            if (pkPlugin->HasManagementDialog())
            {
                return pkPlugin->DoManagementDialog(pkInitialInfo,
                    hWndParent);
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiPluginManager::HasManagementDialog(NiPluginInfo* pkPluginInfo)
{
    for (unsigned int ui = 0; ui < GetTotalPluginCount(); 
        ui++)
    {
        NiPlugin* pkPlugin = GetPluginAt(ui);
        if (pkPlugin && pkPlugin->CanExecute(pkPluginInfo))
            return pkPlugin->HasManagementDialog();
    }
    
    return false;
}
//---------------------------------------------------------------------------
NiScriptInfo* NiPluginManager::GetLastExecutedScript()
{
    
    if (!ms_spCurrentScriptInfo)
    {
        NiScriptTemplateManager* pkManager = 
            NiScriptTemplateManager::GetInstance();
        ms_spCurrentScriptInfo = pkManager->GetScript("Last Executed Script");

        if (!ms_spCurrentScriptInfo)
        {
            ms_spCurrentScriptInfo = NiNew NiScriptInfo();
            ms_spCurrentScriptInfo->SetName("Last Executed Script");
            ms_spCurrentScriptInfo->SetType("Process");
            pkManager->ReplaceScript(ms_spCurrentScriptInfo);
        }
    }
        
    return ms_spCurrentScriptInfo;
}

//---------------------------------------------------------------------------
/// Verify that this ScriptInfo object is capable of being executed
bool NiPluginManager::VerifyScriptInfo(NiScriptInfo* pkInfo)
{
    if (!pkInfo)
        return false;

    for (unsigned int ui = 0; ui < pkInfo->GetPluginInfoCount(); ui++)
    {
        if (GetMatchingPlugin(pkInfo->GetPluginInfoAt(ui)) == NULL)
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------
/// Get the NiPlugin matching this NiPluginInfo object
NiPlugin* NiPluginManager::GetMatchingPlugin(NiPluginInfo* pkInfo, 
                                             unsigned int uiStartIdx)
{
    for (unsigned int ui = uiStartIdx; ui < m_kAllPlugins.GetSize(); ui++)
    {
        NiPlugin* pkPlugin = m_kAllPlugins.GetAt(ui);
        if (pkPlugin && pkPlugin->CanExecute(pkInfo))
            return pkPlugin;
    }

    return NULL;
}