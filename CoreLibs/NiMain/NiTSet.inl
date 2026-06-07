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
//  NiTSet inline functions

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTSet<T,TAlloc>::NiTSet(unsigned int uiInitialSize)
{
    if (uiInitialSize > 0)
    {
        m_pBase = TAlloc::Allocate(uiInitialSize);
        NIASSERT(m_pBase != NULL);
    }
    else
    {
        m_pBase = NULL;
    }
    m_uiAlloced = uiInitialSize;
    m_uiUsed = 0;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTSet<T,TAlloc>::~NiTSet()
{
    TAlloc::Deallocate(m_pBase);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
unsigned int NiTSet<T,TAlloc>::GetSize() const
{
    return m_uiUsed;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
unsigned int NiTSet<T,TAlloc>::GetAllocationSize() const
{
    return m_uiAlloced;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T* NiTSet<T,TAlloc>::GetBase() const
{
    return m_pBase;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
const T& NiTSet<T,TAlloc>::GetAt(unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_uiUsed);
    return m_pBase[uiIndex];
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T& NiTSet<T,TAlloc>::GetAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiUsed);
    return m_pBase[uiIndex];
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
unsigned int NiTSet<T,TAlloc>::Add(const T& element)
{
    NIASSERT(m_uiUsed <= m_uiAlloced);

    if (m_uiUsed == m_uiAlloced)
    {
        Realloc(m_uiAlloced > 0 ? (2 * m_uiAlloced) : 1);
    }

    NIASSERT(m_uiUsed < m_uiAlloced);
    m_pBase[m_uiUsed++] = element;

    return m_uiUsed - 1;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
unsigned int NiTSet<T,TAlloc>::AddUnique(const T& element)
{
    NIASSERT(m_uiUsed <= m_uiAlloced);

    int iFind = Find(element);
    if (iFind == -1)
    {
        return Add(element);
    }
    else
    {
        return (unsigned int) iFind;
    }
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
int NiTSet<T,TAlloc>::Find(const T& element) const
{
    // If the element is in the list, the index is returned, else -1.

    NIASSERT(m_uiUsed <= m_uiAlloced);

    unsigned int i;

    for (i = 0; i < m_uiUsed; i++)
    {
        if (m_pBase[i] == element)
            return i;
    }
    return -1;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::RemoveAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiUsed);

    m_pBase[uiIndex] = m_pBase[--m_uiUsed];
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::OrderedRemoveAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiUsed);

    for (unsigned int ui = uiIndex; ui < m_uiUsed - 1; ui++)
    {
        m_pBase[ui] = m_pBase[ui + 1];
    }
    m_uiUsed--;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::ReplaceAt(unsigned int uiIndex, const T& element)
{
    if (uiIndex >= m_uiUsed)
    {
        return;
    }

    m_pBase[uiIndex] = element;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::RemoveAll()
{
    m_uiUsed = 0;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::Realloc()
{
    Realloc(m_uiUsed);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::ReallocNoShrink(unsigned int uiNewSize)
{
    if (uiNewSize <= m_uiAlloced)
        return;
    Realloc(uiNewSize);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTSet<T,TAlloc>::Realloc(unsigned int uiNewSize)
{
    if (uiNewSize < m_uiUsed)
        uiNewSize = m_uiUsed;

    if (uiNewSize != m_uiAlloced)
    {
        T *pNewBase;
        unsigned int i;

        if (uiNewSize > 0)
        {
            pNewBase = TAlloc::Allocate(uiNewSize);
            NIASSERT(pNewBase != NULL);

            for (i = 0; i < m_uiUsed; i++)
            {
                pNewBase[i] = m_pBase[i];
            }
        }
        else
        {
            pNewBase = NULL;
        }
        
        TAlloc::Deallocate(m_pBase);
        m_pBase = pNewBase;
        m_uiAlloced = uiNewSize;
    }
}
//---------------------------------------------------------------------------
template <class T> inline
NiTObjectSet<T>::NiTObjectSet(unsigned int uiInitialSize) : 
    NiTSet<T, NiTNewInterface<T> >(uiInitialSize)
{
}
//---------------------------------------------------------------------------
template <class T> inline
NiTPrimitiveSet<T>::NiTPrimitiveSet(unsigned int uiInitialSize): 
    NiTSet<T, NiTMallocInterface<T> >(uiInitialSize)
{
}
//---------------------------------------------------------------------------
