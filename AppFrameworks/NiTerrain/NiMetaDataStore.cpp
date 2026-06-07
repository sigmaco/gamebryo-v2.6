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

#include "NiTerrainPCH.h"

#include "NiMetaDataStore.h"

NiMetaDataStore* NiMetaDataStore::ms_pkSingleton = 0;

NiMetaDataStore::NiMetaDataStore()
{
}
//---------------------------------------------------------------------------
NiMetaDataStore::~NiMetaDataStore()
{
}
//---------------------------------------------------------------------------
NiMetaDataStore* NiMetaDataStore::GetInstance()
{
    /*if (!ms_pkSingleton)
        CreateInstance();*/

    NIASSERT(ms_pkSingleton);

    return ms_pkSingleton;
}
//---------------------------------------------------------------------------
void NiMetaDataStore::CreateInstance()
{
    if (!ms_pkSingleton)
        ms_pkSingleton = NiNew NiMetaDataStore();
}
//---------------------------------------------------------------------------
void NiMetaDataStore::DestroyInstance()
{
    NiDelete(ms_pkSingleton);
}
//---------------------------------------------------------------------------
void NiMetaDataStore::SetKeyType(const NiFixedString& kKey, KeyType kType)
{
    m_kTypeMap.SetAt(kKey, kType);
}
//---------------------------------------------------------------------------
bool NiMetaDataStore::GetKeyType(const NiFixedString& kKey, KeyType& kType) 
    const
{
    int iType;
    bool bResult = m_kTypeMap.GetAt(kKey, iType);
    if (bResult)
    {
        kType = KeyType(iType);
    }
    return bResult;
}
//---------------------------------------------------------------------------
bool NiMetaDataStore::RemoveKey(const NiFixedString& kKey)
{
    //Clear this key from all the metadata:
    ClearKey(kKey);
    
    //Remove the type mapping for this key:
    return m_kTypeMap.RemoveAt(kKey);
}
//---------------------------------------------------------------------------
void NiMetaDataStore::RemoveAll()
{
    const char* kCurKey;
    int kCurKeyType;

    NiTMapIterator kIterator = m_kTypeMap.GetFirstPos();
    while (kIterator)
    {
        m_kTypeMap.GetNext(kIterator, kCurKey, kCurKeyType);
        RemoveKey(kCurKey);

        kIterator = m_kTypeMap.GetFirstPos();
    }
}
//---------------------------------------------------------------------------
void NiMetaDataStore::RegisterMetaData(NiMetaData* kMetaData)
{
    m_kMetaData.Add(kMetaData);
}
//---------------------------------------------------------------------------
void NiMetaDataStore::UnregisterMetaData(NiMetaData* kMetaData)
{
    int iIndex = m_kMetaData.Find(kMetaData);
    if (iIndex >= 0)
    {
        m_kMetaData.RemoveAt(iIndex);
    }
}

//---------------------------------------------------------------------------
void NiMetaDataStore::ClearKey(const NiFixedString& kKey)
{
    NiMetaData** ppkData = m_kMetaData.GetBase();
    NiUInt32 uiNumMetaData = m_kMetaData.GetSize();

    for (NiUInt32 ui = 0; ui < uiNumMetaData; ++ui)
    {
        ppkData[ui]->RemoveKey(kKey);
    }
}

//---------------------------------------------------------------------------
bool NiMetaDataStore::ValidateKeys()
{
    bool bResult = true;

    NiTObjectSet<NiFixedString> kInvalidKeys;
    NiMetaData** ppkData = m_kMetaData.GetBase();
    NiUInt32 uiNumMetaData = m_kMetaData.GetSize();

    for (NiUInt32 ui = 0; ui < uiNumMetaData; ++ui)
    {
        bResult |= ppkData[ui]->ValidateEntries(kInvalidKeys);
    }
    
    return bResult;
}

//---------------------------------------------------------------------------
bool NiMetaDataStore::ValidateKeys() const
{
    bool bResult = true;

    NiTObjectSet<NiFixedString> kInvalidKeys;
    const NiMetaData** ppkData = (const NiMetaData**)m_kMetaData.GetBase();
    NiUInt32 uiNumMetaData = m_kMetaData.GetSize();

    for (NiUInt32 ui = 0; ui < uiNumMetaData; ++ui)
    {
        bResult |= ppkData[ui]->ValidateEntries(kInvalidKeys);
    }
    
    return bResult;
}
//---------------------------------------------------------------------------
void NiMetaDataStore::GetAvailableKeys(NiTObjectSet<NiFixedString>& kKeySet) 
    const
{
    const char* kCurKey;
    int kCurKeyType;

    // Iterate over the map and add all the key values:
    NiTMapIterator kIter = m_kTypeMap.GetFirstPos();
    while (kIter != NULL)
    {
        m_kTypeMap.GetNext(kIter, kCurKey, kCurKeyType);

        kKeySet.Add(kCurKey);
    }
}
//---------------------------------------------------------------------------
