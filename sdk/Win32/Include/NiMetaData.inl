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
inline NiMetaData::~NiMetaData()
{
    RemoveAllKeys();
    NiDelete m_pkValueMap;
    m_pkValueMap = 0;
}
//---------------------------------------------------------------------------
inline bool NiMetaData::RemoveKey(const NiFixedString &kKey)
{
    if (!m_pkValueMap)
        return false;

    // Get the entry and delete the object pointed to:
    NiMetaData::MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
        return false;
    
    NiDelete pkCurKeyData;
    return m_pkValueMap->RemoveAt(kKey);
}
//---------------------------------------------------------------------------
inline void NiMetaData::RemoveAllKeys()
{
    if (!m_pkValueMap)
        return;

    // Loop through each entry and delete the object stored in it:
    const char* pcCurKey;
    NiMetaData::MetaDataValue* pkCurKeyData;

    // Iterate over the map and add all the key values:
    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    while (kIter != NULL)
    {
        m_pkValueMap->GetNext(kIter, pcCurKey, pkCurKeyData);

        NiDelete pkCurKeyData;
    }

    m_pkValueMap->RemoveAll();
}
//---------------------------------------------------------------------------
inline bool NiMetaData::HasKey(const NiFixedString& kMetaDataKey)
{
    NIASSERT(m_pkValueMap);
    NiMetaData::MetaDataValue* pkCurKeyData;
    
    if (!m_pkValueMap->GetAt(kMetaDataKey, pkCurKeyData))
        return false;

    return true;
}
