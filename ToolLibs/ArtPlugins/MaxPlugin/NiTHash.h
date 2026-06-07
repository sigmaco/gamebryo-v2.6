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

#ifndef NITHASH_H
#define NITHASH_H

#include <NiMemObject.h>

template <class T> class NiTHashEntry : public NiMemObject
{
public:
    void *p;
    void *p2;
    char *pName;
    T spData;
    NiTHashEntry<T> *pNext;
};


template<class T> class NiTHash : public NiMemObject
{
public:
    typedef bool (*NiPFHashEq)(const void *pSearch, const void *p, 
        const void *p2Search, const void *p2);
    typedef void (*NiPFHashFree)(const void *p2, T &spData);
    
    ~NiTHash();
    NiTHash(unsigned int uiNumEntries, 
           NiPFHashEq pHashEqFunc, 
           NiPFHashFree pHashFreeFunc);
    int Insert(void *p, void *p2, T spData);
    int NameInsert(const char *pName, void *p2, T spData);
    T Retrieve(void *p, void *p2);
    T NameRetrieve(const char *pName, void *p2);
    void Remove(void *p, void *p2);
    void Reset();
    inline int GetNumHashed() { return m_uiNumHashed; }

    void IterInit();
    T IterGetNext();

private:
    NiTHashEntry<T> **m_ppTable;
    unsigned int m_uiNumEntries;
    unsigned int m_uiNumHashed;
    NiPFHashEq m_pHashEqFunc;
    NiPFHashFree m_pHashFreeFunc;

    unsigned int m_uiIterSlot;
    NiTHashEntry<T> *m_pLastReturned;
};

//---------------------------------------------------------------------------
template <class T> NiTHash<T>::NiTHash(unsigned int uiNumEntries, 
    NiPFHashEq pHashEqFunc, NiPFHashFree pHashFreeFunc)
{
    unsigned int i;

    m_ppTable = NiAlloc(NiTHashEntry<T>*, uiNumEntries);
    m_uiNumEntries = uiNumEntries;
    m_uiNumHashed = 0;
    m_pHashEqFunc = pHashEqFunc;
    m_pHashFreeFunc = pHashFreeFunc;
    m_uiIterSlot = 0;
    m_pLastReturned = NULL;

    for (i = 0; i < uiNumEntries; i++)
        m_ppTable[i] = NULL;
}
//---------------------------------------------------------------------------
template <class T> NiTHash<T>::~NiTHash()
{
    NiTHashEntry<T> *pTemp, *pNext;
    unsigned int i;

    for (i = 0; i < m_uiNumEntries; i++)
    {
        for (pTemp = m_ppTable[i]; pTemp != NULL; pTemp = pNext)
        {
            pNext = pTemp->pNext;
            (*m_pHashFreeFunc)(pTemp->p2, pTemp->spData);
            if (pTemp->pName)
                NiFree(pTemp->pName);
            NiDelete pTemp;
        }
    }

    NiFree(m_ppTable);
}
//---------------------------------------------------------------------------
template <class T> int NiTHash<T>::Insert(void *p, void *p2, T spData)
{
    int iHashVal = ((((int ) PtrToInt(p)) >> 2) ^ 0x19fa83) % m_uiNumEntries;
    NiTHashEntry<T> *pTemp;

    pTemp = NiNew NiTHashEntry<T>;

    pTemp->p = p;
    pTemp->pName = NULL;
    pTemp->p2 = p2;
    pTemp->spData = spData;
    pTemp->pNext = m_ppTable[iHashVal];
    m_ppTable[iHashVal] = pTemp;
    m_uiNumHashed++;
    return(0);
}
//---------------------------------------------------------------------------
template <class T> int NiTHash<T>::NameInsert(const char *pName, void *p2, 
    T spData)
{
    int iHashVal, i, iTot;
    NiTHashEntry<T> *pTemp;

    for (i = 0, iTot = 0; pName[i] != '\0'; i++)
        iTot += pName[i];

    iHashVal = ((iTot >> 2) ^ 0x19fa83) % m_uiNumEntries;

    pTemp = NiNew NiTHashEntry<T>;

    pTemp->p = NULL;
    pTemp->pName = NiStrdup(pName);
    pTemp->p2 = p2;
    pTemp->spData = spData;
    pTemp->pNext = m_ppTable[iHashVal];
    m_ppTable[iHashVal] = pTemp;
    m_uiNumHashed++;
    return(0);
}
//---------------------------------------------------------------------------
template <class T> T 
NiTHash<T>::NameRetrieve(const char *pName, void *p2)
{
    int iHashVal, i, iTot;
    NiTHashEntry<T> *pTemp;

    for (i = 0, iTot = 0; pName[i] != '\0'; i++)
        iTot += pName[i];

    iHashVal = ((iTot >> 2) ^ 0x19fa83) % m_uiNumEntries;

    for (pTemp = m_ppTable[iHashVal]; pTemp; pTemp = pTemp->pNext)
        if ((*m_pHashEqFunc)(pName, pTemp->pName, p2, pTemp->p2))
            return(pTemp->spData);

    return(NULL);
}
//---------------------------------------------------------------------------
template <class T> T 
NiTHash<T>::Retrieve(void *p, void *p2)
{
    int iHashVal = ((((int ) PtrToInt(p)) >> 2) ^ 0x19fa83) % m_uiNumEntries;
    NiTHashEntry<T> *pTemp;

    NIASSERT(iHashVal >= 0 && iHashVal < int(m_uiNumEntries));

    for (pTemp = m_ppTable[iHashVal]; pTemp; pTemp = pTemp->pNext)
        if ((*m_pHashEqFunc)(p, pTemp->p, p2, pTemp->p2))
            return(pTemp->spData);

    return(NULL);
}
//---------------------------------------------------------------------------
template <class T> void 
NiTHash<T>::Remove(void *p, void *p2)
{
    int iHashVal = ((((int ) PtrToInt(p)) >> 2) ^ 0x19fa83) % m_uiNumEntries;
    NiTHashEntry<T> *pTemp;
    NiTHashEntry<T> *pPrev = NULL;

    for (pTemp = m_ppTable[iHashVal]; pTemp; pTemp = pTemp->pNext)
    {
        if ((*m_pHashEqFunc)(p, pTemp->p, p2, pTemp->p2))
        {
            //
            // If entry is not head of list (i.e. pPrev exists) 
            // point the previous entry's Next to the current
            // entry's Next value. Otherwise set the bucket's head 
            // to the current entry's Next.
            //
            if (pPrev) 
                pPrev->pNext = pTemp->pNext;
            else
                m_ppTable[iHashVal] = pTemp->pNext;

            if (pTemp->pName)
                NiFree(pTemp->pName);
            NiDelete pTemp;
            m_uiNumHashed--;

            return;
        }
        
        pPrev = pTemp;
    }
}
//---------------------------------------------------------------------------
template <class T> void 
NiTHash<T>::Reset()
{
    NiTHashEntry<T> *pTemp, *pNext;
    unsigned int i;

    for (i = 0; i < m_uiNumEntries; i++)
    {
        for (pTemp = m_ppTable[i]; pTemp != NULL; pTemp = pNext)
        {
            pNext = pTemp->pNext;
            (*m_pHashFreeFunc)(pTemp->p2, pTemp->spData);
            if (pTemp->pName)
                NiFree(pTemp->pName);
            NiDelete pTemp;
        }

        m_ppTable[i] = NULL;
    }
    m_uiNumHashed = 0;
    m_uiIterSlot = 0;
    m_pLastReturned = NULL;
}
//---------------------------------------------------------------------------

template <class T> void 
NiTHash<T>::IterInit()
{
    m_uiIterSlot = 0;
    m_pLastReturned = NULL;
}

//---------------------------------------------------------------------------
template <class T> T
NiTHash<T>::IterGetNext()
{
    for (; m_uiIterSlot < m_uiNumEntries; m_uiIterSlot++)
    {
        if (m_pLastReturned)
            m_pLastReturned = m_pLastReturned->pNext;
        else
            m_pLastReturned = m_ppTable[m_uiIterSlot];

        if (m_pLastReturned)
            return(m_pLastReturned->spData);
    }

    return(NULL);
}

//---------------------------------------------------------------------------

#endif

