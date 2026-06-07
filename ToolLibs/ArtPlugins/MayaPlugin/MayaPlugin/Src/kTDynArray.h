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

// kTDynArray.h
// Class template for a dynamic array.
// Implemented as an array of pointers to element objects.
// The array is dynamically expaned as needed.

#ifndef KTDYNARRAY_H
#define KTDYNARRAY_H

#define MINIMAL_ALLOCATION 16

template <class Type> 
class kTDynArray
{
    private:
        void DoubleSpace(void);
        int  AcquireUnusedIndex(void);

        int  m_iAllocated;
        int  m_iUsed;
        Type **m_ppArray;
    public:
        kTDynArray(void);
        ~kTDynArray(void);

        int  Add(Type *T);
        bool Delete(int iIndex);
        void Reset(void);
        Type* Get(int iIndex);
        Type  GetCopy(int iIndex);
        int   GetUsed(void);
};
//---------------------------------------------------------------------------
template <class Type>
Type* kTDynArray<Type>::
Get(int iIndex)
{
    if (m_ppArray)
        if (iIndex < m_iAllocated)
            return m_ppArray[iIndex];

    return NULL;
}
//---------------------------------------------------------------------------
template <class Type>
int kTDynArray<Type>::
GetUsed(void)
{
    return m_iUsed;
}
//---------------------------------------------------------------------------
template <class Type>
Type kTDynArray<Type>::
GetCopy(int iIndex)
{
    return *m_ppArray[iIndex];
}
//---------------------------------------------------------------------------
template <class Type>
kTDynArray<Type>::
kTDynArray(void)
{
    m_iAllocated = MINIMAL_ALLOCATION;
    m_ppArray = NiExternalNew Type*[m_iAllocated];
    for (int i=0; i<m_iAllocated; i++)
        m_ppArray[i] = NULL;

    m_iUsed = 0;
}
//---------------------------------------------------------------------------
template <class Type>
kTDynArray<Type>::
~kTDynArray(void)
{
    for (int i=0;i<m_iAllocated;i++)
    {
        if (m_ppArray[i])
            NiExternalDelete m_ppArray[i];

        m_ppArray[i] = NULL;
    }

    NiExternalDelete[] m_ppArray;

    m_ppArray = NULL;
}
//---------------------------------------------------------------------------
template <class Type>
int kTDynArray<Type>::AcquireUnusedIndex(void)
{
    for (int i=0;i<m_iAllocated;i++)
    {
        if (m_ppArray[i] == NULL)
            return i;
    }

    // No free slots... Double the space and return
    // the last entry before allocation.
    int iEntry = m_iAllocated;
    DoubleSpace();
    return iEntry;
}
//---------------------------------------------------------------------------
template <class Type>
void kTDynArray<Type>::
DoubleSpace(void)
{
    int iOldAllocated = m_iAllocated;
    
    if (m_iAllocated < 1)
        m_iAllocated = 2;
    else
        m_iAllocated *=2;

    Type **ppMore = NiExternalNew Type*[m_iAllocated];

    int i;
    for(i=0;i<iOldAllocated;i++)
        ppMore[i] = m_ppArray[i];

    for (i=iOldAllocated; i<m_iAllocated; i++)
        ppMore[i] = NULL;

    NiExternalDelete[] m_ppArray;

    m_ppArray = ppMore;
}
//---------------------------------------------------------------------------
template <class Type>
int kTDynArray<Type>::
Add(Type *T)
{
    // First need to find a slot.
    int iIndex = AcquireUnusedIndex();
    m_ppArray[iIndex] = T;
    m_iUsed++;
    return iIndex;
}
//---------------------------------------------------------------------------
template <class Type>
bool kTDynArray<Type>::
Delete(int iIndex)
{
    NIASSERT(m_ppArray[iIndex]);

    NiExternalDelete m_ppArray[iIndex];
    m_ppArray[iIndex] = NULL;
    m_iUsed--;
    return true;
}
//---------------------------------------------------------------------------
template <class Type>
void kTDynArray<Type>::
Reset(void)
{
    // Don't deallocated the array of points,
    // but do deallocate the objects themselves.
    m_iUsed = 0;
    for(int i=0;i<m_iAllocated;i++)
    {
        if (m_ppArray[i])
            NiExternalDelete m_ppArray[i];
        m_ppArray[i] = NULL;
    }
}
//---------------------------------------------------------------------------

#endif