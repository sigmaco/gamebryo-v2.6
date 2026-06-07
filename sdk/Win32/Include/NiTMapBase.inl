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

#include <NiUniversalTypes.h>
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    NiTMapBase(NiUInt32 uiHashSize)
{
    NIASSERT(uiHashSize > 0);

    m_uiHashSize = uiHashSize;
    m_kAllocator.m_uiCount = 0;

    NiUInt32 uiSize = sizeof(NiTMapItem<TKEY,TVAL>*) * m_uiHashSize;
    m_ppkHashTable = (NiTMapItem<TKEY,TVAL>**)NiMalloc(uiSize);
    NIASSERT(m_ppkHashTable);
    memset(m_ppkHashTable, 0, uiSize);
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::~NiTMapBase()
{
    RemoveAll();
    NiFree(m_ppkHashTable);
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline NiUInt32 NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    GetCount() const
{ 
    return m_kAllocator.m_uiCount;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline bool NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    IsEmpty() const
{ 
    return m_kAllocator.m_uiCount == 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline void NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    SetAt(TKEY key, TVAL val)
{
    // look up hash table location for key
    NiUInt32 uiIndex = THASH::KeyToHashIndex(key, m_uiHashSize);
    NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[uiIndex];
    
    NiUInt32 uiDepth = 0;


    // search list at hash table location for key
    while (pkItem)
    {
        uiDepth++;
        if (TEQUALS::IsKeysEqual(key, pkItem->m_key))
        {
            // item already in hash table, set its new value
            pkItem->m_val = val;
            return;
        }
        pkItem = pkItem->m_pkNext;
    }

    // add object to beginning of list for this hash table index
    pkItem = (NiTMapItem<TKEY, TVAL>*)NewItem();

    NIASSERT(pkItem);
    SetValue(pkItem, key, val);
    pkItem->m_pkNext = m_ppkHashTable[uiIndex];
    m_ppkHashTable[uiIndex] = pkItem;
    m_kAllocator.m_uiCount++;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline bool NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    RemoveAt(TKEY key)
{
    // look up hash table location for key
    NiUInt32 uiIndex = THASH::KeyToHashIndex(key, m_uiHashSize);
    NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[uiIndex];

    // search list at hash table location for key
    if (pkItem)
    {
        if (TEQUALS::IsKeysEqual(key, pkItem->m_key))
        {
            // item at front of list, remove it
            m_ppkHashTable[uiIndex] = pkItem->m_pkNext;
            ClearValue(pkItem);
            
            DeleteItem(pkItem);

            m_kAllocator.m_uiCount--;
            return true;
        }
        else
        {
            // search rest of list for item
            NiTMapItem<TKEY, TVAL>* pkPrev = pkItem;
            NiTMapItem<TKEY, TVAL>* pkCurr = pkPrev->m_pkNext;
            while (pkCurr && !TEQUALS::IsKeysEqual(key, pkCurr->m_key))
            {
                pkPrev = pkCurr;
                pkCurr = pkCurr->m_pkNext;
            }
            if (pkCurr)
            {
                // found the item, remove it
                pkPrev->m_pkNext = pkCurr->m_pkNext;
                ClearValue(pkCurr);
                
                DeleteItem(pkCurr);

                m_kAllocator.m_uiCount--;
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline void NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::RemoveAll()
{
    for (NiUInt32 ui = 0; ui < m_uiHashSize; ui++) 
    {
        while (m_ppkHashTable[ui])
        {
            NiTMapItem<TKEY, TVAL>* pkSave = m_ppkHashTable[ui];
            m_ppkHashTable[ui] = m_ppkHashTable[ui]->m_pkNext;
            ClearValue(pkSave);
            
            DeleteItem(pkSave);
        }
    }

    m_kAllocator.m_uiCount = 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline bool NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    GetAt(TKEY key, TVAL& val) const
{
    // look up hash table location for key
    NiUInt32 uiIndex = THASH::KeyToHashIndex(key, m_uiHashSize);
    NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[uiIndex];

    // search list at hash table location for key
    while (pkItem)
    {
        if (TEQUALS::IsKeysEqual(key, pkItem->m_key))
        {
            // item found
            val = pkItem->m_val;
            return true;
        }
        pkItem = pkItem->m_pkNext;
    }

    return false;
}
//---------------------------------------------------------------------------
template <class TKEY> inline
NiUInt32 NiTMapHashFunctor<TKEY>::
    KeyToHashIndex(TKEY key, unsigned int uiTableSize)
{
    // Modular arithmetic is used for building key. If a different scheme
    // is preferred, define your own templated class.
    return (NiUInt32) (((size_t) key) % uiTableSize);
}
//---------------------------------------------------------------------------
template <class TKEY> inline
bool NiTMapEqualsFunctor<TKEY>::IsKeysEqual(TKEY key1, TKEY key2)
{
    return key1 == key2;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline void NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    SetValue(NiTMapItem<TKEY, TVAL>* pkItem, TKEY key, TVAL val)
{
    pkItem->m_key = key;
    pkItem->m_val = val;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline void NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    ClearValue(NiTMapItem<TKEY, TVAL>* /* pkItem */)
{
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline NiTMapIterator NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    GetFirstPos() const
{
    for (NiUInt32 ui = 0; ui < m_uiHashSize; ui++) 
    {
        if (m_ppkHashTable[ui])
            return m_ppkHashTable[ui];
    }
    return 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline void NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    GetNext(NiTMapIterator& pos, TKEY& key, TVAL& val) const
{
    NiTMapItem<TKEY, TVAL>* pkItem = (NiTMapItem<TKEY, TVAL>*) pos;
    
    key = pkItem->m_key;
    val = pkItem->m_val;

    if (pkItem->m_pkNext)
    {
        pos = pkItem->m_pkNext;
        return;
    }

    NiUInt32 ui = THASH::KeyToHashIndex(pkItem->m_key, m_uiHashSize);
    for (++ui; ui < m_uiHashSize; ui++) 
    {
        pkItem = m_ppkHashTable[ui];
        if (pkItem) 
        {
            pos = pkItem;
            return;
        }
    }

    pos = 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class TKEY, class TVAL, class THASH,
    class TEQUALS>
inline void NiTMapBase<TheAllocator, TKEY, TVAL, THASH, TEQUALS>::
    Resize(NiUInt32 uiNewHashSize)
{
    NIASSERT(uiNewHashSize > 0);
    
    // Allocate a new hash array
    NiUInt32 uiNewSize = sizeof(NiTMapItem<TKEY,TVAL>*) * uiNewHashSize;
    NiTMapItem<TKEY,TVAL>** ppkNewHashTable =
        (NiTMapItem<TKEY,TVAL>**)NiMalloc(uiNewSize);
    NIASSERT(ppkNewHashTable);
    memset(ppkNewHashTable, 0, uiNewSize);
    
    // Go through all entries in the existing hash array and transfer the
    // entire entry to the new hash array.
    NiUInt32 uiNewCount = 0;
    for (NiUInt32 ui = 0; ui < m_uiHashSize; ui++) 
    {
        while (m_ppkHashTable[ui])
        {
            // Remove the entry from the existing array
            NiTMapItem<TKEY, TVAL>* pkItem = m_ppkHashTable[ui];
            m_ppkHashTable[ui] = pkItem->m_pkNext;
            m_kAllocator.m_uiCount--;
            
            // Clear values in the entry
            pkItem->m_pkNext = 0;
            
            // Insert the entry in the new map
            NiUInt32 uiIndex =
                THASH::KeyToHashIndex(pkItem->m_key, uiNewHashSize);
            pkItem->m_pkNext = ppkNewHashTable[uiIndex];
            ppkNewHashTable[uiIndex] = pkItem;
            uiNewCount++;
        }
    }

    // At the end, the existing hash array should be empty, and we can just
    // delete it.
    NIASSERT(m_kAllocator.m_uiCount == 0);
    NiFree(m_ppkHashTable);

    // Set the updated values
    m_ppkHashTable = ppkNewHashTable;
    m_uiHashSize = uiNewHashSize;
    m_kAllocator.m_uiCount = uiNewCount;
}
//---------------------------------------------------------------------------
