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
//  NiTStringTemplateMap inline functions

//---------------------------------------------------------------------------
template <class TVAL, class THASH, class TEQUALS>  inline
NiTStringMap<TVAL, THASH, TEQUALS>::NiTStringMap(
    unsigned int uiHashSize, bool bCopy) :
    NiTStringTemplateMap<NiTMap<const char*, TVAL, THASH, TEQUALS>,
    TVAL, THASH, TEQUALS>(uiHashSize, bCopy)
{
} 
//---------------------------------------------------------------------------
template <class TVAL, class THASH, class TEQUALS>  inline
NiTStringPointerMap<TVAL, THASH, TEQUALS>::
    NiTStringPointerMap(unsigned int uiHashSize, bool bCopy) :
    NiTStringTemplateMap<NiTPointerMap<const char*, TVAL, THASH, TEQUALS>,
    TVAL>(uiHashSize, bCopy)
{
} 
//---------------------------------------------------------------------------
template <class TPARENT, class TVAL, class THASH, class TEQUALS>  inline
NiTStringTemplateMap<TPARENT,TVAL, THASH, TEQUALS>::NiTStringTemplateMap(
    unsigned int uiHashSize, bool bCopy) :
    TPARENT(uiHashSize)
{
    m_bCopy = bCopy;
} 
//---------------------------------------------------------------------------
template <class TPARENT, class TVAL, class THASH, class TEQUALS>  inline
NiTStringTemplateMap<TPARENT,TVAL, THASH, TEQUALS>::~NiTStringTemplateMap()
{
    if (m_bCopy)
    {
        for (unsigned int i = 0; i < TPARENT::m_uiHashSize; i++) 
        {
            NiTMapItem<const char*, TVAL>* pkItem = TPARENT::m_ppkHashTable[i];
            while (pkItem) 
            {
                NiTMapItem<const char*, TVAL>* pkSave = pkItem;
                pkItem = pkItem->m_pkNext;
                NiFree((char*) pkSave->m_key);
            }
        }
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiStringHashFunctor::KeyToHashIndex(
    const char* pKey, unsigned int uiTableSize)
{
    unsigned int uiHash = 0;

    while (*pKey)
        uiHash = (uiHash << 5) + uiHash + *pKey++;

    return uiHash % uiTableSize;
}
//---------------------------------------------------------------------------
inline bool NiStringEqualsFunctor::IsKeysEqual(const char* pcKey1,
    const char* pcKey2)
{
    return strcmp(pcKey1, pcKey2) == 0;
}
//---------------------------------------------------------------------------
template <class TPARENT, class TVAL, class THASH, class TEQUALS>  inline
void NiTStringTemplateMap<TPARENT,TVAL,THASH,TEQUALS>::SetValue(
    NiTMapItem<const char*, TVAL>* pkItem, const char* pcKey, TVAL val)
{
    if (m_bCopy)
    {
        size_t stLen = strlen(pcKey) + 1;
        pkItem->m_key = NiAlloc(char, stLen);
        NIASSERT(pkItem->m_key);
        NiStrcpy((char*) pkItem->m_key, stLen, pcKey);
    }
    else
    {
        pkItem->m_key = pcKey;
    }
    pkItem->m_val = val;
}
//---------------------------------------------------------------------------
template <class TPARENT, class TVAL, class THASH, class TEQUALS>  inline
void NiTStringTemplateMap<TPARENT,TVAL,THASH,TEQUALS>::ClearValue(
    NiTMapItem<const char*, TVAL>* pkItem)
{
    if (m_bCopy)
    {
        NiFree((char*) pkItem->m_key);
    }
}
//---------------------------------------------------------------------------
