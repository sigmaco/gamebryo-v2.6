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

#include "NiImportPluginInfo.h"

NiImplementRTTI(NiImportPluginInfo, NiPluginInfo);

//---------------------------------------------------------------------------
NiImportPluginInfo::NiImportPluginInfo()
{
    SetType("IMPORT");
    m_eImportType = ADD;
}
//---------------------------------------------------------------------------
NiImportPluginInfo::NiImportPluginInfo(const char* pcDirectory,
    const char* pcFile, const char* pcExtension) : 
    m_strDirectory(pcDirectory), m_strFile(pcFile),
    m_strExtension(pcExtension)
{
    SetType("IMPORT");
    m_eImportType = ADD;
}
//---------------------------------------------------------------------------
NiPluginInfo* NiImportPluginInfo::Clone(NiPluginInfo* pkClone)
{
    if (!pkClone)
    {
        pkClone = NiNew NiImportPluginInfo;
    }

    NIASSERT(NiIsKindOf(NiImportPluginInfo, pkClone));

    NiPluginInfo::Clone(pkClone);

    NiImportPluginInfo* pkImportClone = (NiImportPluginInfo*) pkClone;
    pkImportClone->SetDirectory(m_strDirectory);
    pkImportClone->SetFile(m_strFile);
    pkImportClone->SetExtension(m_strExtension);

    return pkClone;
}
//---------------------------------------------------------------------------
bool NiImportPluginInfo::Equals(NiPluginInfo* pkInfo)
{
    if (!NiIsKindOf(NiImportPluginInfo, pkInfo))
    {
        return false;
    }

    if (!NiPluginInfo::Equals(pkInfo))
    {
        return false;
    }

    NiImportPluginInfo* pkImportInfo = (NiImportPluginInfo*) pkInfo;

    if (pkImportInfo->GetDirectory() != m_strDirectory)
    {
        return false;
    }

    if (pkImportInfo->GetFile() != m_strFile)
    {
        return false;
    }

    if (pkImportInfo->GetExtension() != m_strExtension)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
const NiString& NiImportPluginInfo::GetDirectory() const
{ 
    return m_strDirectory;
}
//---------------------------------------------------------------------------
const NiString& NiImportPluginInfo::GetFile() const
{ 
    return m_strFile;
}
//---------------------------------------------------------------------------
const NiString& NiImportPluginInfo::GetExtension() const
{
    return m_strExtension;
}
//---------------------------------------------------------------------------
void NiImportPluginInfo::SetDirectory(const char* pcDirectory)
{ 
    m_strDirectory = pcDirectory;
}
//---------------------------------------------------------------------------
void NiImportPluginInfo::SetFile(const char* pcFile)
{ 
    m_strFile = pcFile;
}
//---------------------------------------------------------------------------
void NiImportPluginInfo::SetExtension(const char* pcExtension)
{
    m_strExtension = pcExtension;
}
//---------------------------------------------------------------------------
NiString NiImportPluginInfo::GetFullFilePath() const
{
    return m_strDirectory + m_strFile + m_strExtension;
}
//---------------------------------------------------------------------------

/// Set the export type for this plugin
NiImportPluginInfo::ImportType NiImportPluginInfo::GetImportType() const
{
    return m_eImportType;
}

//---------------------------------------------------------------------------
/// Set the import type for this plugin
void NiImportPluginInfo::SetImportType(NiImportPluginInfo::ImportType eType)
{
    m_eImportType = eType;
}
//---------------------------------------------------------------------------
