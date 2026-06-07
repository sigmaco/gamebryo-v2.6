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
//  NiTQueue inline functions

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTQueue<T,TAlloc>::NiTQueue(unsigned int uiInitialSize)
{
    NIASSERT(uiInitialSize > 0);

    m_pBase = TAlloc::Allocate(uiInitialSize);
    NIASSERT(m_pBase != NULL);
    m_uiAlloced = uiInitialSize;
    m_uiHead = UINT_MAX;
    m_uiTail = 0;
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTQueue<T,TAlloc>::~NiTQueue()
{
    while (!IsEmpty())
        Remove();
    TAlloc::Deallocate(m_pBase);
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
bool NiTQueue<T,TAlloc>::IsEmpty() const
{
    return (m_uiHead == UINT_MAX);
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
bool NiTQueue<T,TAlloc>::IsFull() const
{
    return (m_uiHead == m_uiTail);
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T NiTQueue<T,TAlloc>::Head() const
{
    NIASSERT(!IsEmpty());
    return m_pBase[m_uiHead];
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T NiTQueue<T,TAlloc>::Remove()
{
    if (IsEmpty())
        return 0;

    T element;
    element = m_pBase[m_uiHead];
    m_pBase[m_uiHead] = 0;
    if (++m_uiHead == m_uiAlloced)
    {
        m_uiHead = 0;
    }
    if (m_uiHead == m_uiTail)
    {
        m_uiHead = UINT_MAX;
        m_uiTail = 0;
    }
    return element;
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTQueue<T,TAlloc>::Add(T element)
{
    if (IsEmpty())
    {
        m_pBase[m_uiHead = 0] = element;
        m_uiTail = 1;
    }
    else
    {
        if (IsFull())
        {
            Realloc(2 * m_uiAlloced);
        }

        m_pBase[m_uiTail++] = element;
    }

    if (m_uiTail == m_uiAlloced)
    {
        m_uiTail = 0;
    }
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTQueue<T,TAlloc>::AddUnique(T element)
{
    unsigned int i;

    if (!IsEmpty())
    {
        if (m_uiHead >= m_uiTail)
        {
            for (i = m_uiHead; i < m_uiAlloced; i++)
            {
                if (m_pBase[i] == element)
                    return;
            }

            for (i = 0; i < m_uiTail; i++)
            {
                if (m_pBase[i] == element)
                    return;
            }
        }
        else
        {
            for (i = m_uiHead; i < m_uiTail; i++)
            {
                if (m_pBase[i] == element)
                    return;
            }
        }
    }

    Add(element);
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
unsigned int NiTQueue<T,TAlloc>::GetSize() const
{
    if (IsEmpty())
    {
        return 0;
    }
    else if (m_uiHead >= m_uiTail)
    {
        return m_uiAlloced - m_uiHead + m_uiTail;
    }
    else
    {
        return m_uiTail - m_uiHead;
    }
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTQueue<T,TAlloc>::Realloc()
{
    Realloc(GetSize());
}

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTQueue<T,TAlloc>::Realloc(unsigned int uiNewSize)
{
    T *pNewBase;
    unsigned int i, j;

    if (uiNewSize == 0)
        return;

    NIASSERT(uiNewSize >= GetSize());

    pNewBase = TAlloc::Allocate(uiNewSize);
    NIASSERT(pNewBase != NULL);

    j = 0;
    if (m_uiHead >= m_uiTail)
    {
        for (i = m_uiHead; i < m_uiAlloced; i++)
        {
            pNewBase[j++] = m_pBase[i];
        }
        
        for (i = 0; i < m_uiTail; i++)
        {
            pNewBase[j++] = m_pBase[i];
        }
    }
    else
    {
        for (i = m_uiHead; i < m_uiTail; i++)
        {
            pNewBase[j++] = m_pBase[i];
        }
    }

    NIASSERT(j == GetSize());

    m_uiHead = 0;
    m_uiTail = j;
    m_uiAlloced = uiNewSize;
    TAlloc::Deallocate(m_pBase);
    m_pBase = pNewBase;
}
//---------------------------------------------------------------------------
template <class T> inline
NiTObjectQueue<T>::NiTObjectQueue(unsigned int uiInitialSize) : 
    NiTQueue<T, NiTNewInterface<T> >(uiInitialSize)
{  
}
//---------------------------------------------------------------------------
template <class T> inline
NiTPrimitiveQueue<T>::NiTPrimitiveQueue(unsigned int uiInitialSize) :
    NiTQueue<T, NiTMallocInterface<T> >(uiInitialSize)
{  
}
//---------------------------------------------------------------------------
