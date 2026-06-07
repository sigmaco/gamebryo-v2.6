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

#include <NiDOMTool.h>

#include "NiMetaData.h"
#include "NiMetaDataStore.h"

//---------------------------------------------------------------------------
NiMetaData::NiMetaData(NiMetaDataStore* pkMetaDataStore) :
    m_pkMetaDataStore(pkMetaDataStore),
    m_pkValueMap(0)
{
    if (!pkMetaDataStore)
        m_pkMetaDataStore = NiMetaDataStore::GetInstance();
    
    NIASSERT(m_pkMetaDataStore && 
        "Must initialize NiMetaData with an appropriate NiMetaDataStore");
}
//---------------------------------------------------------------------------
NiMetaData::NiMetaData(const NiMetaData& kMetaData) : 
    m_pkMetaDataStore(NiMetaDataStore::GetInstance()),
    m_pkValueMap(0)
{
    *this = kMetaData;    
}
//---------------------------------------------------------------------------
NiMetaData::NiMetaData() :
    m_pkMetaDataStore(NiMetaDataStore::GetInstance()),
    m_pkValueMap(0)
{
}
//---------------------------------------------------------------------------
bool NiMetaData::Set(const NiFixedString& kKey, NiInt32 iValue, 
    KeyType eType, float fWeight)
{
    if (!m_pkValueMap)
        m_pkValueMap = NiNew NiTStringPointerMap<NiMetaData::MetaDataValue*>;

    NiMetaData::MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
    {
        pkCurKeyData = NiNew MetaDataValue();
        m_pkValueMap->SetAt(kKey, pkCurKeyData);
    }

    NIASSERT(eType == INTEGER || eType == INTEGER_BLENDED);

    pkCurKeyData->eType = eType;
    pkCurKeyData->iValue = iValue;
    pkCurKeyData->kStringValue = NULL;
    pkCurKeyData->fWeight = fWeight;

    m_pkMetaDataStore->SetKeyType(kKey, pkCurKeyData->eType);
    return true;
}
//---------------------------------------------------------------------------
bool NiMetaData::Set(const NiFixedString& kKey, float fValue, KeyType eType, 
    float fWeight)
{
    if (!m_pkValueMap)
        m_pkValueMap = NiNew NiTStringPointerMap<NiMetaData::MetaDataValue*>;

    NiMetaData::MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
    {
        pkCurKeyData = NiNew MetaDataValue();
        m_pkValueMap->SetAt(kKey, pkCurKeyData);
    }
    
    NIASSERT(eType == FLOAT || eType == FLOAT_BLENDED);

    pkCurKeyData->eType = eType;
    pkCurKeyData->fValue = fValue;
    pkCurKeyData->kStringValue = NULL;
    pkCurKeyData->fWeight = fWeight;

    m_pkMetaDataStore->SetKeyType(kKey, pkCurKeyData->eType);
    return true;
}
//---------------------------------------------------------------------------
bool NiMetaData::Set(const NiFixedString& kKey, const NiFixedString& kValue,
    KeyType eType, float fWeight)
{
    if (!m_pkValueMap)
        m_pkValueMap = NiNew NiTStringPointerMap<NiMetaData::MetaDataValue*>;

    NiMetaData::MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
    {
        pkCurKeyData = NiNew MetaDataValue();
        m_pkValueMap->SetAt(kKey, pkCurKeyData);
    }

    NIASSERT(eType == STRING);

    pkCurKeyData->eType = eType;
    pkCurKeyData->iValue = 0;
    pkCurKeyData->kStringValue = kValue;
    pkCurKeyData->fWeight = fWeight;

    m_pkMetaDataStore->SetKeyType(kKey, pkCurKeyData->eType);
    return true;
}
//---------------------------------------------------------------------------
void NiMetaData::UpdateWeights(float fWeight) const
{
    if (!m_pkValueMap)
        return;

    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    while (kIter)
    {
        const char* pkStr;
        MetaDataValue* pkValue;

        m_pkValueMap->GetNext(kIter, pkStr, pkValue);
        NIASSERT(pkValue);
        pkValue->fWeight = fWeight;
    }
}
//---------------------------------------------------------------------------
float NiMetaData::GetWeight() const
{
    if (!m_pkValueMap)
        return 0.0f;

    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    const char* pkStr;
    MetaDataValue* pkValue;
    
    if (kIter)
    {
        m_pkValueMap->GetNext(kIter, pkStr, pkValue);
        NIASSERT(pkValue);
        return pkValue->fWeight;
    }
    else
    {
        return 1.0f;
    }
}
//---------------------------------------------------------------------------
void NiMetaData::Blend(const NiMetaData* pMetaData, float fWeight)
{
    if (fWeight <= 0.0f || !pMetaData->m_pkValueMap)
        return;

    if (!m_pkValueMap)
        m_pkValueMap = NiNew NiTStringPointerMap<NiMetaData::MetaDataValue*>;

    const char* pcCurKey;
    NiMetaData::MetaDataValue* pkCurKeyData;

    // Iterate over the map and add all the key values:
    NiTMapIterator kIter = pMetaData->m_pkValueMap->GetFirstPos();
    while (kIter != NULL)
    {
        pMetaData->m_pkValueMap->GetNext(kIter, pcCurKey, pkCurKeyData);

        // For each item in the metadata:
        NiMetaData::MetaDataValue* pkLocalKeyData = 0;
        if (!m_pkValueMap->GetAt(pcCurKey, pkLocalKeyData))
        {
            // No value exists for this yet
            pkLocalKeyData = NiNew MetaDataValue();
            pkLocalKeyData->eType = pkCurKeyData->eType;
            m_pkValueMap->SetAt(pcCurKey, pkLocalKeyData);
            
            // Initialise the value:
            pkLocalKeyData->fWeight = 0.0f;
            switch(pkLocalKeyData->eType)
            {
                case FLOAT:
                case FLOAT_BLENDED:
                    pkLocalKeyData->fValue = 0.0f;
                    break;
                case INTEGER:
                case INTEGER_BLENDED:
                    pkLocalKeyData->iValue = 0;
                    break;
                case STRING:
                    pkLocalKeyData->kStringValue = "";
                    break;
                default:
                    *pkLocalKeyData = 0;
                    break;
            }
        }
        else
        {
            // Check that the types are correct
            NIASSERT(pkLocalKeyData->eType == pkCurKeyData->eType &&
                "Metadata types are not consistent for the same key");
        }
        
        switch(pkLocalKeyData->eType)
        {
            case FLOAT_BLENDED:
                pkLocalKeyData->fValue += 
                    pkCurKeyData->fValue * fWeight;
                break;
            case INTEGER_BLENDED:
                pkLocalKeyData->iValue += 
                    (NiUInt32)(pkCurKeyData->iValue * fWeight);
                break;
            default:
                if (pkLocalKeyData->fWeight < fWeight)
                {
                    *pkLocalKeyData = *pkCurKeyData;
                    pkLocalKeyData->fWeight = fWeight;
                }
                break;
        }
    }
}
//---------------------------------------------------------------------------
bool NiMetaData::Get(const NiFixedString& kKey, NiInt32& iValue, 
    float& fWeight) const
{
    if (!m_pkValueMap)
        return false;

    NiMetaData::MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
        return false;
    
    if (pkCurKeyData->eType == INTEGER || 
        pkCurKeyData->eType == INTEGER_BLENDED)
    {
        iValue = pkCurKeyData->iValue;
        fWeight = pkCurKeyData->fWeight;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMetaData::Get(const NiFixedString& kKey, float& fValue, 
    float& fWeight) const
{
    if (!m_pkValueMap)
        return false;

    NiMetaData::MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
        return false;
    
    if (pkCurKeyData->eType == FLOAT ||
        pkCurKeyData->eType == FLOAT_BLENDED)
    {
        fValue = pkCurKeyData->fValue;
        fWeight = pkCurKeyData->fWeight;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMetaData::Get(const NiFixedString& kKey, NiFixedString& kValue, 
    float& fWeight) const
{
    if (!m_pkValueMap)
        return false;

    MetaDataValue* pkCurKeyData;
    if (!m_pkValueMap->GetAt(kKey, pkCurKeyData))
        return false;
    
    if (pkCurKeyData->eType == STRING)
    {
        kValue = pkCurKeyData->kStringValue;
        fWeight = pkCurKeyData->fWeight;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMetaData::GetKeyType(const NiFixedString& kKey, 
    KeyType& kKeyType) const
{
    return m_pkMetaDataStore->GetKeyType(kKey, kKeyType);
}
//---------------------------------------------------------------------------
void NiMetaData::GetAvailableKeys(NiTObjectSet<NiFixedString>& kKeyNames) const
{
    m_pkMetaDataStore->GetAvailableKeys(kKeyNames);
}
//---------------------------------------------------------------------------
void NiMetaData::GetKeys(NiTObjectSet<NiFixedString>& kKeyNames) const
{
    if (!m_pkValueMap)
        return;

    const char* pcCurKey;
    NiMetaData::MetaDataValue* pkCurKeyData;

    // Iterate over the map and add all the key values:
    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    while (kIter != NULL)
    {
        m_pkValueMap->GetNext(kIter, pcCurKey, pkCurKeyData);

        kKeyNames.Add(pcCurKey);
    }
}
//---------------------------------------------------------------------------
bool NiMetaData::ValidateEntries(NiTObjectSet<NiFixedString>& kKeyNames)
{
    if (!m_pkValueMap)
        return false;

    const char* pcCurKey;
    NiMetaData::MetaDataValue* pkCurKeyData;
    KeyType eCurKeyType;

    // Iterate over the entries and figure out the ones that are not valid:
    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    while (kIter != NULL)
    {
        m_pkValueMap->GetNext(kIter, pcCurKey, pkCurKeyData);
        
        GetKeyType(pcCurKey, eCurKeyType);
        if (eCurKeyType != pkCurKeyData->eType)
        {            
            kKeyNames.Add(pcCurKey);
        }
    }    

    // Remove the non-compliant entries:
    NiFixedString* pkInvalidKeys = kKeyNames.GetBase();
    NiUInt32 uiNumInvalidKeys = kKeyNames.GetSize();

    for (NiUInt32 ui = 0; ui < uiNumInvalidKeys; ++ui)
    {
        RemoveKey(pkInvalidKeys[ui]);
    }

    return kKeyNames.GetSize() == 0;
}
//---------------------------------------------------------------------------
bool NiMetaData::ValidateEntries(NiTObjectSet<NiFixedString>& kKeyNames) const
{
    if (!m_pkValueMap)
        return false;

    const char* pcCurKey;
    NiMetaData::MetaDataValue* pkCurKeyData;
    KeyType eCurKeyType;

    // Iterate over the entries and figure out the ones that arn't valid:
    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    while (kIter != NULL)
    {
        m_pkValueMap->GetNext(kIter, pcCurKey, pkCurKeyData);
        
        this->GetKeyType(pcCurKey, eCurKeyType);
        if (eCurKeyType != pkCurKeyData->eType)
        {            
            kKeyNames.Add(pcCurKey);
        }
    }      

    return kKeyNames.GetSize() == 0;
}
//---------------------------------------------------------------------------
void NiMetaData::Save(NiDOMTool& kDom) const
{
    if (!m_pkValueMap)
        return;

    const char* pcCurKey;
    NiMetaData::MetaDataValue* pkCurKeyData;

    // Loop through all the values an save them.
    NiTMapIterator kIter = m_pkValueMap->GetFirstPos();
    while (kIter != NULL)
    {
        m_pkValueMap->GetNext(kIter, pcCurKey, pkCurKeyData);
        
        //Save to the DOM
        kDom.BeginSection("MetaEntry");
        {
            KeyType eKeyType = pkCurKeyData->eType;

            kDom.BeginSection("Key");
            kDom.WritePrimitive(NiFixedString(pcCurKey));
            kDom.EndSection();

            kDom.BeginSection("DataType");
            kDom.WritePrimitive(eKeyType);
            kDom.EndSection();

            kDom.BeginSection("Value");
            switch (eKeyType)
            {
                case INTEGER:
                    kDom.WritePrimitive(pkCurKeyData->iValue);
                    break;

                case INTEGER_BLENDED:
                    kDom.WritePrimitive(pkCurKeyData->iValue);
                    break;

                case FLOAT:
                    kDom.WritePrimitive(pkCurKeyData->fValue);
                    break;

                case FLOAT_BLENDED:
                    kDom.WritePrimitive(pkCurKeyData->fValue);
                    break;

                case STRING:
                    kDom.WritePrimitive(pkCurKeyData->kStringValue);
                    break;

                default:
                    break;
            }
            kDom.EndSection();
        }
        kDom.EndSection();
    }    
}
//---------------------------------------------------------------------------
void NiMetaData::Load(NiDOMTool& kDom)
{
    NiFixedString pcCurKey;

    // Make sure there is a value map to add to
    if (!m_pkValueMap)
        m_pkValueMap = NiNew NiTStringPointerMap<NiMetaData::MetaDataValue*>;

    // Loop over the DOM data and create the keys:
    kDom.SetSectionToFirstChild();
    while (kDom.GetCurrentSection())
    {
        // This should be a metadata entry.
        kDom.SetSectionTo("Key");
        kDom.ReadPrimitive(pcCurKey);
        kDom.EndSection();

        int iDataType;
        kDom.SetSectionTo("DataType");
        kDom.ReadPrimitive((int&)iDataType);
        kDom.EndSection();

        kDom.SetSectionTo("Value");
        switch ((KeyType)iDataType)
        {
            case INTEGER:
            {
                NiInt32 iValue;
                kDom.ReadPrimitive(iValue);
                Set(pcCurKey, iValue);
            }
            break;

            case INTEGER_BLENDED:
            {
                NiInt32 iValue;
                kDom.ReadPrimitive(iValue);
                Set(pcCurKey, iValue, INTEGER_BLENDED);
            }
            break;

            case FLOAT:
            {
                float fValue;
                kDom.ReadPrimitive(fValue);
                Set(pcCurKey, fValue);
            }            
            break;

            case FLOAT_BLENDED:
            {
                float fValue;
                kDom.ReadPrimitive(fValue);
                Set(pcCurKey, fValue, FLOAT_BLENDED);
            }            
            break;

            case STRING:
            {
                NiFixedString kValue;
                kDom.ReadPrimitive(kValue);
                Set(pcCurKey, kValue);                
            }            
            break;

            default:
                break;
        }

        kDom.EndSection();
        kDom.SetSectionToNextSibling();
    }
    kDom.EndSection();
}
//---------------------------------------------------------------------------
NiMetaData& NiMetaData::operator= (const NiMetaData& kMetaData)
{
    if (this == &kMetaData)
        return *this;

    m_pkMetaDataStore = kMetaData.m_pkMetaDataStore;
    if (kMetaData.m_pkValueMap)
    {
        if (!m_pkValueMap) 
        {
            m_pkValueMap = 
                NiNew NiTStringPointerMap<NiMetaData::MetaDataValue*>;
        }
        else 
        {
            RemoveAllKeys();
        }

        NiTMapIterator kIterator = kMetaData.m_pkValueMap->GetFirstPos();
        MetaDataValue* pkCurKeyData;
        const char* pcCurKey;
        KeyType eCurKeyType;

        while (kIterator)
        {
            kMetaData.m_pkValueMap->GetNext(kIterator, pcCurKey, pkCurKeyData);
            
            // Key
            if (GetKeyType(pcCurKey, eCurKeyType))
            {
                if (eCurKeyType != pkCurKeyData->eType)
                {
                    // Force an error
                    NIASSERT(eCurKeyType == pkCurKeyData->eType);
                    continue;
                }
            }
            else
            {
                // Never seen this key before
                m_pkMetaDataStore->SetKeyType(pcCurKey, pkCurKeyData->eType);
            }

            MetaDataValue* pkNewKeyData = NiNew MetaDataValue();
            *pkNewKeyData = *pkCurKeyData;

            // Value
            m_pkValueMap->SetAt(pcCurKey, pkNewKeyData);
        }
    }

    return *this;
}
