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
//---------------------------------------------------------------------------
#include "NiRegistry.h"
#include "NiMemManager.h"
#include <NiRTLib.h>
#include <NiSystem.h>
#include "NiString.h"

//---------------------------------------------------------------------------
NiRegistry::NiRegistry()
{
    m_hRootKey = 0;
    m_hAppKey = 0;
    m_hkey = 0;
}
//---------------------------------------------------------------------------
NiRegistry::~NiRegistry()
{
    CloseKey();
}
//---------------------------------------------------------------------------
NiRegistry* NiRegistry::Create(const char* pcAppPath)
{
    NiRegistry* pkRegistry = NiNew NiRegistry();
    if (pkRegistry)
    {
        
        if (!pkRegistry->SetAppPath(pcAppPath) ||
            !pkRegistry->OpenKey())
        {
            NiDelete pkRegistry;
            pkRegistry = 0;
        }
    }

    return pkRegistry;
}
//---------------------------------------------------------------------------
bool NiRegistry::SetAppPath(NiString strAppPath)
{
    m_hTopLevelKey = 0;
    unsigned int uiFound = (unsigned int)NiString::INVALID_INDEX;

    uiFound = strAppPath.Find("HKEY_CLASSES_ROOT");
    if (uiFound != NiString::INVALID_INDEX)
    {
        m_hTopLevelKey = HKEY_CLASSES_ROOT;
        m_strAppPath = strAppPath.Right(strAppPath.Length() - 
            strlen("HKEY_CLASSES_ROOT") -1);
        return true;
    }

    uiFound = strAppPath.Find("HKEY_CURRENT_CONFIG");
    if (uiFound != NiString::INVALID_INDEX)
    {
        m_hTopLevelKey = HKEY_CURRENT_CONFIG;
        m_strAppPath = strAppPath.Right(strAppPath.Length() - 
            strlen("HKEY_CURRENT_CONFIG") -1);
        return true;
    }

    uiFound = strAppPath.Find("HKEY_CURRENT_USER");
    if (uiFound != NiString::INVALID_INDEX)
    {
        m_hTopLevelKey = HKEY_CURRENT_USER;
        m_strAppPath = strAppPath.Right(strAppPath.Length() -
            strlen("HKEY_CURRENT_USER") -1);
        return true;
    }

    uiFound = strAppPath.Find("HKEY_LOCAL_MACHINE");
    if (uiFound != NiString::INVALID_INDEX)
    {
        m_hTopLevelKey =  HKEY_LOCAL_MACHINE;
        m_strAppPath = strAppPath.Right(strAppPath.Length() - 
            strlen("HKEY_LOCAL_MACHINE") -1);
        return true;
    }

    uiFound = strAppPath.Find("HKEY_USERS");
    if (uiFound != NiString::INVALID_INDEX)
    {
        m_hTopLevelKey = HKEY_USERS;
        m_strAppPath = strAppPath.Right(strAppPath.Length() - 
            strlen("HKEY_USERS") -1);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiRegistry::OpenKey()
{
    DWORD dwDisp; 
    
    if (RegCreateKeyEx(m_hTopLevelKey, (const char*) m_strAppPath, 0, NULL, 
        REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE | 
        KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS, NULL, 
        &m_hRootKey, &dwDisp)) 
    {
        return FALSE;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiRegistry::CloseKey()
{
    if (m_hAppKey)
        RegCloseKey(m_hAppKey);
    if (m_hRootKey)
        RegCloseKey(m_hRootKey);

    return true;
}
//---------------------------------------------------------------------------
void NiRegistry::SetDWORD(const char* pcSubKey, DWORD dwValue) 
{
    if (RegSetValueEx(m_hRootKey, pcSubKey, 0, REG_DWORD, 
        (LPBYTE)&dwValue, 4) != ERROR_SUCCESS)
    {
        // Error...
    }
}
//---------------------------------------------------------------------------
void NiRegistry::SetBool(const char* pcSubKey, BOOL dwValue) 
{
    if (RegSetValueEx(m_hRootKey, pcSubKey, 0, REG_DWORD, 
        (LPBYTE)&dwValue, 4) != ERROR_SUCCESS)
    {
        // Error...
    }
}
//---------------------------------------------------------------------------
void NiRegistry::SetString(const char* pcSubKey, const char* pcValue) 
{
    if (RegSetValueEx(m_hRootKey, pcSubKey, 0, REG_SZ, 
        (const unsigned char*) pcValue, (DWORD)strlen(pcValue) + 1) != 
        ERROR_SUCCESS)
    {
        // Error...
    }
}
//---------------------------------------------------------------------------
void NiRegistry::SetGUID(const char* pcSubKey, GUID* pkGUID) 
{
    char acBuff[40];
    NiSprintf(acBuff, 40, 
        "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
        pkGUID->Data1, pkGUID->Data2, pkGUID->Data3, pkGUID->Data4[0], 
        pkGUID->Data4[1], pkGUID->Data4[2], pkGUID->Data4[3], pkGUID->Data4[4],
        pkGUID->Data4[5], pkGUID->Data4[6], pkGUID->Data4[7]);

    if (RegSetValueEx(m_hRootKey, pcSubKey, 0, REG_SZ, 
        (const unsigned char *)acBuff, (DWORD)strlen(acBuff) + 1) != 
        ERROR_SUCCESS)
    {
        // Error...
    }
}
//---------------------------------------------------------------------------
bool NiRegistry::QueryBool(const char* pcSubKey, bool& bValue) 
{
    DWORD dwRC;
    DWORD dwLen;
    DWORD dwType;
 
    dwLen = 4;
    dwRC = RegQueryValueEx(m_hRootKey, pcSubKey, 0, &dwType, 
        (LPBYTE)&bValue, &dwLen);
    if (dwRC != ERROR_SUCCESS)
    {
        return false;
    }
    else
    {
        if (dwType != REG_DWORD)
        {
            return false;
        }
    }
 
    return true;
}
//---------------------------------------------------------------------------
bool NiRegistry::QueryDWORD(const char* pcSubKey, unsigned int& uiValue) 
{
    DWORD dwRC;
    DWORD dwLen;
    DWORD dwType;
 
    dwLen = 4;
    dwRC = RegQueryValueEx(m_hRootKey, pcSubKey, 0, &dwType, 
        (LPBYTE)&uiValue, &dwLen);
    if (dwRC != ERROR_SUCCESS)
    {
        return false;
    } 
    else
    {
        if (dwType != REG_DWORD)
            return false;
    } 
 
    return true; 
}
//---------------------------------------------------------------------------
bool NiRegistry::QueryString(const char* pcSubKey, NiString& strValue) 
{
    DWORD dwRC;
    DWORD dwLen;
    DWORD dwType;
    char  acBuff[1024];
 
    dwLen = sizeof(acBuff);
    dwRC = RegQueryValueEx(m_hRootKey, pcSubKey, 0, &dwType, (LPBYTE)acBuff, 
        &dwLen); 
    if (dwRC != ERROR_SUCCESS)
    {
        return false;
    }
    else
    { 
        if (dwType != REG_SZ)
            return false;
    } 
 
    strValue = acBuff; 
    return true;
}
//---------------------------------------------------------------------------
bool NiRegistry::QueryGUID(const char* pcSubKey, GUID* pkGUID)
{
    DWORD dwRC;
    DWORD dwLen;
    DWORD dwType;
    char  acBuff[40];
 
    dwLen = sizeof(acBuff);
    dwRC = RegQueryValueEx(m_hRootKey, pcSubKey, 0, &dwType, (LPBYTE)acBuff, 
        &dwLen); 
    if (dwRC != ERROR_SUCCESS)
    {
        return false;
    }
    else
    { 
        if(dwType != REG_SZ)
            return false;
    } 

    // Read Data4 into separate array to prevent buffer overrun
    unsigned int uiData4[8];
#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf_s(acBuff, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf(acBuff, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
        &pkGUID->Data1, &pkGUID->Data2, &pkGUID->Data3, &uiData4[0], 
        &uiData4[1], &uiData4[2], &uiData4[3], &uiData4[4], &uiData4[5], 
        &uiData4[6], &uiData4[7]);

    for (unsigned int i = 0; i < 8; i++)
        pkGUID->Data4[i] = (char)uiData4[i];

    return true;
}
//---------------------------------------------------------------------------
