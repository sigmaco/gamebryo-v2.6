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

#include "NiExportPluginInfo.h"

NiImplementRTTI(NiExportPluginInfo, NiPluginInfo);

//---------------------------------------------------------------------------
NiExportPluginInfo::NiExportPluginInfo()
{
    SetType("EXPORT");
}
//---------------------------------------------------------------------------
NiExportPluginInfo::NiExportPluginInfo(const char* pcDirectory,
    const char* pcFile, const char* pcExtension) :
    m_strDirectory(pcDirectory), m_strFile(pcFile),
    m_strExtension(pcExtension)
{
    SetType("EXPORT");
}
//---------------------------------------------------------------------------
NiPluginInfo* NiExportPluginInfo::Clone(NiPluginInfo* pkClone)
{
    if (!pkClone)
    {
        pkClone = NiNew NiExportPluginInfo;
    }

    NIASSERT(NiIsKindOf(NiExportPluginInfo, pkClone));

    NiPluginInfo::Clone(pkClone);

    NiExportPluginInfo* pkExportClone = (NiExportPluginInfo*) pkClone;
    pkExportClone->SetDirectory(m_strDirectory);
    pkExportClone->SetFile(m_strFile);
    pkExportClone->SetExtension(m_strExtension);

    return pkClone;
}
//---------------------------------------------------------------------------
bool NiExportPluginInfo::Equals(NiPluginInfo* pkInfo)
{
    if (!NiIsKindOf(NiExportPluginInfo, pkInfo))
    {
        return false;
    }

    if (!NiPluginInfo::Equals(pkInfo))
    {
        return false;
    }

    NiExportPluginInfo* pkExportInfo = (NiExportPluginInfo*) pkInfo;

    if (pkExportInfo->GetDirectory() != m_strDirectory)
    {
        return false;
    }

    if (pkExportInfo->GetFile() != m_strFile)
    {
        return false;
    }

    if (pkExportInfo->GetExtension() != m_strExtension)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
const NiString& NiExportPluginInfo::GetDirectory() const
{ 
    return m_strDirectory;
}
//---------------------------------------------------------------------------
const NiString& NiExportPluginInfo::GetFile() const
{ 
    return m_strFile;
}
//---------------------------------------------------------------------------
const NiString& NiExportPluginInfo::GetExtension() const
{
    return m_strExtension;
}
//---------------------------------------------------------------------------
void NiExportPluginInfo::SetDirectory(const char* pcDirectory)
{ 
    m_strDirectory = pcDirectory;
}
//---------------------------------------------------------------------------
void NiExportPluginInfo::SetFile(const char* pcFile)
{ 
    m_strFile = pcFile;
}
//---------------------------------------------------------------------------
void NiExportPluginInfo::SetExtension(const char* pcExtension)
{
    m_strExtension = pcExtension;
}
//---------------------------------------------------------------------------
NiString NiExportPluginInfo::GetFullFilePath() const
{
    return m_strDirectory + m_strFile + m_strExtension;
}
//---------------------------------------------------------------------------
