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
template <class T, class TAlloc> inline
NiTArray2D<T, TAlloc>::NiTArray2D(NiUInt32 uiMaxSizeX, NiUInt32 uiMaxSizeY, 
    NiUInt32 uiGrowBy) :
    m_uiGrowBy(1),
    m_uiMaxSizeX(0),
    m_uiMaxSizeY(0),
    m_pBase(0)
{
    // Make sure the given grow by is valid
    SetGrowBy(uiGrowBy);
    
    // Allocate the array
    SetSize(uiMaxSizeX, uiMaxSizeY);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTArray2D<T, TAlloc>::~NiTArray2D() 
{
    if (m_pBase)
        TAlloc::Deallocate(m_pBase);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTArray2D<T, TAlloc>::SetSize(NiUInt32 uiMaxSizeX, 
    NiUInt32 uiMaxSizeY)
{
    // If the new dimensions are smaller, we need to set some values to zero, 
    // to keep reference counts up to date.
    if (uiMaxSizeX < m_uiMaxSizeX) 
    {
        for (NiUInt32 x = m_uiMaxSizeX - 1; x > uiMaxSizeX; --x) 
        {
            for (NiUInt32 y = 0; y < m_uiMaxSizeY; ++y) 
            {
                SetAt(x, y, T(0) );
            }
        }
    }

    if (uiMaxSizeY < m_uiMaxSizeY) 
    {
        // We don't need to set the same values to zero twice if both 
        // dimensions are being reduced
        NiUInt32 x = (uiMaxSizeX < m_uiMaxSizeX) ? uiMaxSizeX : m_uiMaxSizeX;

        for (NiUInt32 y = m_uiMaxSizeY - 1; y > uiMaxSizeY; --y) 
        {
            for (x = x - 1; x >= 0; --x) 
            {
                SetAt(x, y, T(0));
            }
        }
    }

    T* pOldBase = m_pBase;

    // Allocate new memory?
    if (uiMaxSizeX * uiMaxSizeY > 0) 
    {
        m_pBase = TAlloc::Allocate(uiMaxSizeX * uiMaxSizeY);

        // copy old values into the new array
        NiUInt32 uiItMaxX = m_uiMaxSizeX;
        NiUInt32 uiItMaxY = m_uiMaxSizeY;
        if (uiMaxSizeX < m_uiMaxSizeX)
            uiItMaxX = uiMaxSizeX;
        if (uiMaxSizeY < m_uiMaxSizeY)
            uiItMaxY = uiMaxSizeY;

        for (NiUInt32 x = 0; x < uiItMaxX; ++x) 
        {
            for (NiUInt32 y = 0; y < uiItMaxY; ++y) 
            {
                m_pBase[y * uiItMaxX + x] = pOldBase[y * m_uiMaxSizeX + x];
            }
        }

        // Initialize any new memory
        // Right block
        for (NiUInt32 x = m_uiMaxSizeX; x < uiMaxSizeX; x++)
        {
            for (NiUInt32 y = 0; y < m_uiMaxSizeY; y++) 
            {
                m_pBase[y * uiItMaxX + x] = T(0);  
            }
        }
        // Bottom Block
        for (NiUInt32 x = 0; x < uiMaxSizeX; x++) 
        {
            for (NiUInt32 y = m_uiMaxSizeY; y < uiMaxSizeY; y++) 
            {
                m_pBase[y * uiItMaxX + x] = T(0);
            }
        }
    } 
    else 
    {
        m_pBase = 0;
    }

    m_uiMaxSizeX = uiMaxSizeX;
    m_uiMaxSizeY = uiMaxSizeY;

    TAlloc::Deallocate(pOldBase);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
NiUInt32 NiTArray2D<T, TAlloc>::GetMaxX() const 
{
    return m_uiMaxSizeX;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
NiUInt32 NiTArray2D<T, TAlloc>::GetMaxY() const 
{
    return m_uiMaxSizeY;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
const T& NiTArray2D<T, TAlloc>::GetAt(NiUInt32 uiLocX, NiUInt32 uiLocY) const 
{
    NIASSERT(uiLocX < m_uiMaxSizeX && uiLocY < m_uiMaxSizeY);    
    return m_pBase[uiLocY * m_uiMaxSizeX + uiLocX];
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
void NiTArray2D<T, TAlloc>::SetGrowBy(NiUInt32 uiGrowBy) 
{
    NIASSERT(uiGrowBy > 0);
    m_uiGrowBy = uiGrowBy;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTArray2D<T, TAlloc>::SetAt(NiUInt32 uiLocX, 
    NiUInt32 uiLocY,
    const T& element)
{
    NIASSERT(uiLocX < m_uiMaxSizeX && uiLocY < m_uiMaxSizeY);
    m_pBase[uiLocY * m_uiMaxSizeX + uiLocX] = element;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiUInt32 NiTArray2D<T, TAlloc>::SetAtGrow(NiUInt32 uiLocX, 
    NiUInt32 uiLocY, 
    const T& element) 
{
    // Do we need to enlarge the array in either direction?
    NiUInt32 uiIncX = 0, uiIncY = 0;

    if (uiLocX >= m_uiMaxSizeX) {
        uiIncX = max(m_uiMaxSizeX - uiLocX + 1, getGrowBy());
    }
    if (uiLocY >= m_uiMaxSizeY) {
        uiIncY = max(m_uiMaxSizeX - uiLocY + 1, getGrowBy());
    }

    if (uiIncX > 0 || uiLocY > 0) {
        SetSize(m_uiMaxSizeX + uiIncX, m_uiMaxSizeY + uiIncY);
    }

    // Assign the new element to the array
    m_pBase[uiLocY * m_uiMaxSizeX + uiLocX] = element;

    return GetSize();
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiUInt32 NiTArray2D<T, TAlloc>::GetGrowBy() const 
{
    return m_uiGrowBy;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T NiTArray2D<T, TAlloc>::RemoveAt(NiUInt32 uiLocX, NiUInt32 uiLocY)
{
    if (uiLocX >= m_uiMaxSizeX || uiLocY >= m_uiMaxSizeY)
    {
        return T(0);
    }

    T element = m_pBase[uiLocY * m_uiMaxSizeX + uiLocX];
    m_pBase[uiLocY * m_uiMaxSizeX + uiLocX] = T(0);

    return element;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTArray2D<T, TAlloc>::RemoveAll() 
{
    for (NiUInt32 x = 0; x < m_uiMaxSizeX; ++x) {
        for (NiUInt32 y = 0; y < m_uiMaxSizeY; ++y) {
            m_pBase[y * m_uiMaxSizeX + x] = T(0);
        }
    }
}
//---------------------------------------------------------------------------
template <class T> inline
NiTObjectArray2D<T>::NiTObjectArray2D(
    NiUInt32 uiMaxSizeX, NiUInt32 uiMaxSizeY, NiUInt32 uiGrowBy) : 
    NiTArray2D<T, NiTNewInterface<T>>(uiMaxSizeX, uiMaxSizeY, uiGrowBy)
{  
}
//---------------------------------------------------------------------------
template <class T> inline
NiTPrimitiveArray2D<T>::NiTPrimitiveArray2D(
    NiUInt32 uiMaxSizeX, NiUInt32 uiMaxSizeY, NiUInt32 uiGrowBy) : 
    NiTArray2D<T, NiTMallocInterface<T>>(uiMaxSizeX, uiMaxSizeY, uiGrowBy)
{  
}
