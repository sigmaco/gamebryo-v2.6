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

#include "NiScriptInfo.h"

//---------------------------------------------------------------------------
NiScriptInfo::NiScriptInfo()
{
    m_bDirty = true;
}
//---------------------------------------------------------------------------
const NiString& NiScriptInfo::GetName() const
{
    return m_strName;
}
//---------------------------------------------------------------------------
void NiScriptInfo::SetName(const char* pcName)
{
    m_strName = pcName;
    MarkAsDirty(true);
}
//---------------------------------------------------------------------------
const NiString& NiScriptInfo::GetType() const
{
    return m_strType;
}
//---------------------------------------------------------------------------
void NiScriptInfo::SetType(const char* pcType)
{
    m_strType = pcType;
    MarkAsDirty(true);
}
//--------------------------------------------------------------------------- 
NiPluginInfoPtrSet& NiScriptInfo::GetPluginSet()
{
    return m_kPluginSet;
}
//---------------------------------------------------------------------------
void NiScriptInfo::AddPluginInfo(NiPluginInfo* pkInfo)
{
    m_kPluginSet.Add(pkInfo);
    MarkAsDirty(true);
}
//---------------------------------------------------------------------------
// Remove a plugin from the list
void NiScriptInfo::RemovePlugin(NiPluginInfo* pkInfo)
{
    unsigned int ui = GetPluginInfoIndex(pkInfo);
    if (ui >= GetPluginInfoCount())
        return;
    m_kPluginSet.OrderedRemoveAt(ui);
    MarkAsDirty(true);
}

//---------------------------------------------------------------------------
// Make a deep copy of the the script info
NiScriptInfo* NiScriptInfo::Clone()
{
    if (this == NULL)
        return NULL;

    NiScriptInfo* pkInfo = NiNew NiScriptInfo();

    pkInfo->CopyMembers(this);

    return pkInfo;
}
//---------------------------------------------------------------------------
void NiScriptInfo::CopyMembers(NiScriptInfo* pkScript)
{
    if (pkScript == this)
        return;

    NIASSERT(pkScript);

    // Delete all the old data.
    SetName(NULL);
    SetType(NULL);
    SetFile(NULL);
    while (GetPluginInfoCount())
    {
        NiPluginInfo* pkPlugin = GetPluginInfoAt(0);
        RemovePlugin(pkPlugin);
    }

    // Create all the new data.
    SetName(pkScript->GetName());
    SetType(pkScript->GetType());
    SetFile(pkScript->GetFile());
    for (unsigned int ui = 0; ui < pkScript->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPlugin = pkScript->GetPluginInfoAt(ui);
        if (pkPlugin)
            AddPluginInfo(pkPlugin->Clone());
    }
    MarkAsDirty(pkScript->IsDirty());

    NIASSERT(pkScript->Equals(this));
}
//---------------------------------------------------------------------------
// Does the input script equal the current script
bool NiScriptInfo::Equals(NiScriptInfo* pkScript)
{
    if (!pkScript)
        return false;

    if (pkScript == this)
        return true;

    if (!pkScript->GetName().Equals(GetName()))
        return false;

    if ( GetPluginInfoCount() != 
        pkScript->GetPluginInfoCount())
        return false;

    for (unsigned int ui = 0; ui < GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkThisPlugin = GetPluginInfoAt(ui);
        NiPluginInfo* pkOtherPlugin = pkScript->GetPluginInfoAt(ui);

        if (!pkThisPlugin || !pkOtherPlugin)
            return false;
        if (! pkThisPlugin->Equals(pkOtherPlugin))
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// Mark this script as needing to be saved or not
void NiScriptInfo::MarkAsDirty(bool bMark)
{
    m_bDirty = bMark;
    for (unsigned int ui = 0; ui < GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkInfo = GetPluginInfoAt(ui);
        if (pkInfo)
            pkInfo->MarkAsDirty(m_bDirty);
    }
}

//---------------------------------------------------------------------------
// Does this script need to be saved?
bool NiScriptInfo::IsDirty()
{
    if (m_bDirty)
        return true;

    for (unsigned int ui = 0; ui < GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkInfo = GetPluginInfoAt(ui);
        if (pkInfo && pkInfo->IsDirty())
            return true;
    }
    
    return false;
}


//---------------------------------------------------------------------------
// Set the file associated with this script
void NiScriptInfo::SetFile(const char* pcFileName)
{
    m_strFile = pcFileName;
    MarkAsDirty(true);
}

//---------------------------------------------------------------------------
// Get the file assoicated with this script
const NiString& NiScriptInfo::GetFile()
{
    return m_strFile;
}

//---------------------------------------------------------------------------
// Get plugin at index uiIdx
NiPluginInfo* NiScriptInfo::GetPluginInfoAt(unsigned int uiIdx)
{  
    NiPluginInfo* pkInfo = m_kPluginSet.GetAt(uiIdx);
    if (pkInfo)
        return pkInfo;
    else
        return NULL;
}

//---------------------------------------------------------------------------
// Get the total number of plugins
unsigned int NiScriptInfo::GetPluginInfoCount()
{
    return m_kPluginSet.GetSize();
}
//---------------------------------------------------------------------------
// Remove PluginInfo at index uiIdx
unsigned int NiScriptInfo::GetPluginInfoIndex(NiPluginInfo* pkPlugin)
{  
    if (pkPlugin == NULL)
        return NIPT_INVALID_INDEX;

    for (unsigned int ui = 0; ui < GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkThisPlugin = GetPluginInfoAt(ui);
        
        if (pkPlugin == pkThisPlugin)
            return ui;
    }

    return NIPT_INVALID_INDEX;
}

//---------------------------------------------------------------------------
// Move plugin to position
void NiScriptInfo::MovePluginInfoToPosition(NiPluginInfo* pkPlugin, 
                                            unsigned int uiIdx)
{
    NiPluginInfoPtr spPluginInfo = NULL;
    NiPluginInfoPtr spPluginInfoRemoved = NULL;
    unsigned int uiFoundIdx = GetPluginInfoIndex(pkPlugin);
    if (uiFoundIdx == NIPT_INVALID_INDEX)
        return;
    spPluginInfo = pkPlugin;
    RemovePluginInfoAt(uiFoundIdx);
    unsigned int uiSize = GetPluginInfoCount();
    
    unsigned int ui;
    for (ui = uiIdx; ui < uiSize; ui++)
    {
        spPluginInfoRemoved = m_kPluginSet.GetAt(ui);
        m_kPluginSet.ReplaceAt(ui, spPluginInfo);
        spPluginInfo = spPluginInfoRemoved;
    }

    if (ui == GetPluginInfoCount())
    {
        AddPluginInfo(spPluginInfo);
        MarkAsDirty(true);
    }

}

//---------------------------------------------------------------------------
NiPluginInfoPtr NiScriptInfo::RemovePluginInfoAt(unsigned int uiIdx)
{
    NiPluginInfoPtr spPluginInfo = GetPluginInfoAt(uiIdx);
    m_kPluginSet.OrderedRemoveAt(uiIdx);
    MarkAsDirty(true);
    return spPluginInfo;
}
//---------------------------------------------------------------------------
// Make the values of this Script info match the input script info
void NiScriptInfo::ForceMatch(NiScriptInfo* pkScript)
{
    if (!pkScript)
        return;

    SetName(pkScript->GetName());
    m_kPluginSet.RemoveAll();

    for (unsigned int ui = 0; ui < pkScript->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkOtherPlugin = pkScript->GetPluginInfoAt(ui);
        AddPluginInfo(pkOtherPlugin);
    }

    SetFile(pkScript->GetFile());
    return;
}
//---------------------------------------------------------------------------
