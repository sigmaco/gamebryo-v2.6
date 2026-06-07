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

#include "NiScriptInfoSet.h"
#include "NiPluginManager.h"
#include <NiSystem.h>
//---------------------------------------------------------------------------
/// Add a script to the list of known scripts, 
ReturnCode NiScriptInfoSet::AddScript(NiScriptInfo* pkInfo, 
    bool bTemplate, bool bSilentRunning)
{
    if (pkInfo)
    {
        if (GetScript(pkInfo->GetName()) != NULL)
        {
            if (!bSilentRunning)
            {
                NiString strMessage;
                strMessage.Format(
                    "A script with the name \"%s\" already exists! "
                    "The script will not be added to the list of known "
                    "scripts.", pkInfo->GetName());
                NiMessageBox(strMessage, "Script name collision");
            }
            return SCRIPT_OF_SAME_NAME_ALREADY_EXISTS;
        }
        if (IgnoreScript(pkInfo))
            return SCRIPT_IGNORED;

        m_kAllScripts.Add(pkInfo);
        m_kAllScriptsTemplateFlags.Add(bTemplate);
        return SUCCESS;
    }

    return NULL_INPUT_SCRIPT;
}

//---------------------------------------------------------------------------
/// Remove a script by name
NiScriptInfoPtr NiScriptInfoSet::RemoveScript(const char* pcScript)
{
    unsigned int uiIdx = GetScriptIndex(pcScript);
    NiScriptInfoPtr spScript = GetScriptAt(uiIdx);
    m_kAllScripts.OrderedRemoveAt(uiIdx);
    m_kAllScriptsTemplateFlags.OrderedRemoveAt(uiIdx);
    return spScript;
}
//---------------------------------------------------------------------------
/// Remove All Scripts from this set
void NiScriptInfoSet::RemoveAllScripts()
{
    m_kAllScripts.RemoveAll();
    m_kAllScriptsTemplateFlags.RemoveAll();
    m_kIgnoreClasses.RemoveAll();
}
//---------------------------------------------------------------------------
/// Get a script info by name
NiScriptInfo* NiScriptInfoSet::GetScript(const char* pcName)
{
    for (unsigned int ui=0; ui < m_kAllScripts.GetSize(); ui++)
    {
        NiScriptInfo* pkInfo = m_kAllScripts.GetAt(ui);
        if (pkInfo && pkInfo->GetName().EqualsNoCase(pcName))
            return pkInfo;
    }
    return NULL;
}

//---------------------------------------------------------------------------
/// Get the number of known scripts
unsigned int NiScriptInfoSet::GetScriptCount()
{
    return m_kAllScripts.GetSize();
}

//---------------------------------------------------------------------------
/// Get the script at the position ui
NiScriptInfo* NiScriptInfoSet::GetScriptAt(unsigned int ui)
{
    if (ui < GetScriptCount())
        return m_kAllScripts.GetAt(ui);
    else
        return NULL;
}

//---------------------------------------------------------------------------
/// Get the index of the script by the given name
unsigned int NiScriptInfoSet::GetScriptIndex(const char* pcName)
{
    for (unsigned int ui=0; ui < m_kAllScripts.GetSize(); ui++)
    {
        NiScriptInfo* pkInfo = m_kAllScripts.GetAt(ui);
        if (pkInfo && pkInfo->GetName().EqualsNoCase(pcName))
            return ui;
    }
   
    return NIPT_INVALID_INDEX;
}
//---------------------------------------------------------------------------
void NiScriptInfoSet::Copy(NiScriptInfoSet* pkSet)
{
    for (unsigned int uj = 0; uj < pkSet->m_kIgnoreClasses.GetSize(); uj++)
    {
        IgnorePluginClass(pkSet->m_kIgnoreClasses.GetAt(uj));
    }

    for (unsigned int ui = 0; ui < pkSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = pkSet->GetScriptAt(ui)->Clone();
        bool bTemplate = pkSet->m_kAllScriptsTemplateFlags.GetAt(ui);
        ReturnCode eResult;
        eResult = AddScript(pkInfo, bTemplate, true);
        if (eResult == SCRIPT_IGNORED)
            NiDelete pkInfo;
    }
}
//---------------------------------------------------------------------------
NiScriptInfoSet* NiScriptInfoSet::Clone()
{
    NiScriptInfoSet* pkSet = NiNew NiScriptInfoSet;
    for (unsigned int uj = 0; uj < m_kIgnoreClasses.GetSize(); uj++)
    {
        pkSet->IgnorePluginClass(m_kIgnoreClasses.GetAt(uj));
    }

    for (unsigned int ui = 0; ui < GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = GetScriptAt(ui);
        bool bTemplate = m_kAllScriptsTemplateFlags.GetAt(ui);

        // Store the clone in a smart pointer because AddScript may choose to
        // ignore the script.
        NiScriptInfoPtr spClone = pkInfo->Clone();
        pkSet->AddScript(spClone, bTemplate, true);
    }
    return pkSet;
}
//---------------------------------------------------------------------------
void NiScriptInfoSet::IgnorePluginClass(const NiRTTI* pkRTTI)
{
    m_kIgnoreClasses.Add(pkRTTI->GetName());
}

//---------------------------------------------------------------------------
void NiScriptInfoSet::IgnorePluginClass(const char* pcClassName)
{
    m_kIgnoreClasses.Add(pcClassName);
}
//---------------------------------------------------------------------------
bool NiScriptInfoSet::IgnoreScript(NiScriptInfo* pkInfo)
{
    if (!pkInfo)
        return true;

    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    for (unsigned int ui = 0; ui < pkInfo->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPluginInfo = pkInfo->GetPluginInfoAt(ui);
        NiPlugin* pkPlugin = pkManager->GetMatchingPlugin(pkPluginInfo);
        if (!pkPlugin && IsIgnoringMissingPlugins())
            return true;

        for (unsigned int uj = 0; uj < m_kIgnoreClasses.GetSize(); uj++)
        {
            const char* pcClassName = m_kIgnoreClasses.GetAt(uj);
            if (pcClassName && pkPlugin && 
                NiIsKindOfStringMatch(pcClassName, pkPlugin->GetRTTI()))
                return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
NiScriptInfoSet::NiScriptInfoSet(bool bIgnoreMissingPlugins)
{
    m_bIgnoreMissingPlugins = bIgnoreMissingPlugins;
}
//---------------------------------------------------------------------------
void NiScriptInfoSet::IgnoreMissingPlugins(bool bIgnore)
{
    m_bIgnoreMissingPlugins = bIgnore;
}

//---------------------------------------------------------------------------
bool NiScriptInfoSet::IsIgnoringMissingPlugins()
{
    return m_bIgnoreMissingPlugins;
}
//---------------------------------------------------------------------------
void NiScriptInfoSet::SetTemplate(NiScriptInfo* pkInfo, bool bTemplate)
{
    if (!pkInfo)
        return;

    unsigned int ui = GetScriptIndex(pkInfo->GetName());
    m_kAllScriptsTemplateFlags.ReplaceAt(ui, bTemplate);
}
//---------------------------------------------------------------------------
bool NiScriptInfoSet::IsTemplate(NiScriptInfo* pkInfo)
{
    if (!pkInfo)
        return false;

    unsigned int ui = GetScriptIndex(pkInfo->GetName());
    if (ui == NIPT_INVALID_INDEX)
        return false;
    else
        return m_kAllScriptsTemplateFlags.GetAt(ui);
}
//---------------------------------------------------------------------------
NiScriptInfoPtr NiScriptInfoSet::ReplaceScript(NiScriptInfo* pkNewScript)
{
    if (pkNewScript == NULL)
        return NULL;

    bool bTemplate = false;
    if (pkNewScript->GetFile() != NULL)
        bTemplate = true;

    NiScriptInfoPtr spOriginalScript = GetScript(pkNewScript->GetName());
    unsigned int uiIdx = GetScriptIndex(pkNewScript->GetName());
    if (uiIdx == NIPT_INVALID_INDEX)
    {
        
        AddScript(pkNewScript, bTemplate, true);
        return NULL;
    }
    else if(IgnoreScript(pkNewScript))
    {
        return NULL;
    }

    m_kAllScripts.ReplaceAt(uiIdx, pkNewScript);
    this->m_kAllScriptsTemplateFlags.ReplaceAt(uiIdx, bTemplate);
    return spOriginalScript;
}
