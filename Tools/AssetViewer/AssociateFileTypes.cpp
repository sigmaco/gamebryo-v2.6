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

#include "stdafx.h"
#include "AssociateFileTypes.h"

//---------------------------------------------------------------------------
bool AssociateFileTypes::Associate(const char* pcExtension, 
    const char* pcApplication, const char* pcOptions, 
    const char* pcIdentifier, const char* pcDescription, const char* pcIcon)
{
    RegistryKey kExtKey;
    kExtKey.GetKey(HKEY_CLASSES_ROOT, pcExtension);
    if (!kExtKey.SetValue("", pcIdentifier))
        return false;

    RegistryKey kIdKey;
    kIdKey.GetKey(HKEY_CLASSES_ROOT, pcIdentifier);
    if (!kIdKey.SetValue("", pcDescription))
        return false;

    RegistryKey kIconKey;
    kIconKey.GetKey(kIdKey, "DefaultIcon");
    if (!kIconKey.SetValue("", pcIcon))
        return false;

    RegistryKey kShellKey;
    if (!kShellKey.GetKey(kIdKey, "shell"))
        return false;

    RegistryKey kOpenKey;
    if (!kOpenKey.GetKey(kShellKey,  "open"))
        return false;

    RegistryKey kOpenCommandKey;
    if (!kOpenCommandKey.GetKey(kOpenKey,  "command"))
        return false;

    char acCommandString[NI_MAX_PATH*2];
    NiSprintf(acCommandString, sizeof(acCommandString), "\"%s\" %s \"%%1\"",
        pcApplication, (pcOptions ? pcOptions : "")); 

    return kOpenCommandKey.SetValue("", acCommandString);
}
//---------------------------------------------------------------------------
AssociateFileTypes::RegistryKey::RegistryKey() : m_kKey(NULL)
{}
//---------------------------------------------------------------------------
AssociateFileTypes::RegistryKey::~RegistryKey()
{
    RegCloseKey(m_kKey);
}
//---------------------------------------------------------------------------
AssociateFileTypes::RegistryKey::operator HKEY&()
{
    return m_kKey;
}
//---------------------------------------------------------------------------
bool AssociateFileTypes::RegistryKey::IsValid()
{
    return m_kKey != NULL;
}
//---------------------------------------------------------------------------
bool AssociateFileTypes::RegistryKey::GetKey(HKEY kParent, 
    const char* pcSubKey)
{
    if (IsValid())
    {
        RegCloseKey(m_kKey);
        m_kKey = NULL;
    };

    if (RegCreateKeyEx(kParent, pcSubKey, 0L, NULL, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, NULL, &m_kKey, NULL) == ERROR_SUCCESS)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssociateFileTypes::RegistryKey::SetValue(const char* pcName,
    const char* pcValue)
{
    if (!IsValid())
        return false;

    if (RegSetValueEx(m_kKey, pcName, 0L, REG_SZ, 
        reinterpret_cast<BYTE*>(const_cast<char*>(pcValue)), 
        (DWORD)strlen(pcValue))
        == ERROR_SUCCESS)
    {
        return true;
    }
  
    return false;
}
//---------------------------------------------------------------------------
