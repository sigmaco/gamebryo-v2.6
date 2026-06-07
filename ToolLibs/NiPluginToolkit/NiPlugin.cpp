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

#include "NiPlugin.h"
#include "NiFramework.h"
#include "NiSharedDataList.h"
#include "NiPluginHelpers.h"

NiImplementRootRTTI(NiPlugin);

//---------------------------------------------------------------------------
NiPlugin::NiPlugin(const char* pcName, const char* pcVersion,
    const char* pcShortDescription, const char* pcLongDescription) :
    m_strName(pcName), m_strVersion(pcVersion),
    m_strShortDescription(pcShortDescription),
    m_strLongDescription(pcLongDescription), m_hModule(NULL)
{
}
//---------------------------------------------------------------------------
NiPlugin::~NiPlugin()
{
}
//---------------------------------------------------------------------------
const NiString& NiPlugin::GetName() const
{
    return m_strName;
}
//---------------------------------------------------------------------------
const NiString& NiPlugin::GetVersion() const
{
    return m_strVersion;
}
//---------------------------------------------------------------------------
const NiString& NiPlugin::GetShortDescription() const 
{
    return m_strShortDescription;
}
//---------------------------------------------------------------------------
const NiString& NiPlugin::GetLongDescription() const
{
    return m_strLongDescription;
}
//---------------------------------------------------------------------------
void NiPlugin::SetModule(NiModuleRef hModule)
{
    m_hModule = hModule;
}
//---------------------------------------------------------------------------
NiModuleRef NiPlugin::GetModule() const
{
    return m_hModule;
}
//---------------------------------------------------------------------------
NiSharedDataList* NiPlugin::GetSharedDataList()
{
    return &NiFramework::GetFramework().GetSharedDataList();
}
//---------------------------------------------------------------------------
NiSharedData* NiPlugin::GetSharedData(NiSharedDataType pkType)
{
    NiSharedData* pkData = NULL;
    pkData = GetSharedDataList()->Get(pkType);
    return pkData;
}
//---------------------------------------------------------------------------
void NiPlugin::SetAbsolutePluginPath(const char* pcPath)
{
    m_strAbsolutePluginPath = pcPath;
}
//---------------------------------------------------------------------------
const NiString& NiPlugin::GetAbsolutePluginPath()
{
    return m_strAbsolutePluginPath;
}
//---------------------------------------------------------------------------
bool NiPlugin::CanCreateDefaultInstance()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiPlugin::VerifyVersionString(const char* pcVersionString)
{
    if (NiPluginHelpers::GetVersionFromString(pcVersionString) <=
        NiPluginHelpers::GetVersionFromString(m_strVersion))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiPlugin::CompareVersions(const char* pcA, const char* pcB)
{
    if (NiPluginHelpers::GetVersionFromString(pcA) <
        NiPluginHelpers::GetVersionFromString(pcB))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiPlugin::CompareVersions(unsigned int uiA, const char* pcB)
{
    if (uiA < NiPluginHelpers::GetVersionFromString(pcB))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiPlugin::CompareVersions(const char* pcA, unsigned int uiB)
{
    if (NiPluginHelpers::GetVersionFromString(pcA) < uiB)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
