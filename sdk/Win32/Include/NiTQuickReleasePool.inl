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
NiTQuickReleasePool< T,  TAlloc>::NiTQuickReleasePool(NiUInt32 uiSize, 
    float fGrowByPercent) :

    m_uiMaxSize(0),
    m_uiCurrentSize(0),
    m_fGrowByPercent(0.1f),
    m_pPool(0),
    m_uiRefCount(0)
{
    if (uiSize < 1)
        uiSize = 1;

    SetGrowByPercent(fGrowByPercent);
    SetSize(uiSize);
    Reset();
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>
inline NiTQuickReleasePool< T,  TAlloc>::~NiTQuickReleasePool()
{
    if (m_pPool)
    {
        NiFree(m_pPool);
        m_pPool = 0;
    }
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
void NiTQuickReleasePool< T,  TAlloc>::SetSize(NiUInt32 uiSize)
{
    m_pPool = (T*)NiRealloc(m_pPool, uiSize * sizeof(T));

    m_uiMaxSize = uiSize;
    if (m_uiCurrentSize > m_uiMaxSize)
    {
        m_uiCurrentSize = m_uiMaxSize;
    }
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::GetSize(NiUInt32& uiSize) const
{
    uiSize = m_uiCurrentSize;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::GetMaxSize(NiUInt32& uiSize) const
{
    uiSize = m_uiMaxSize;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
void NiTQuickReleasePool< T,  TAlloc>::SetGrowByPercent(
    float fGrowByPercent)
{
    if (fGrowByPercent <= 0.0f)
    {
        fGrowByPercent = 0.01f;
    }
    m_fGrowByPercent = fGrowByPercent;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
void NiTQuickReleasePool< T,  TAlloc>::GetGrowByPercent(
    float& fGrowByPercent) const
{
    fGrowByPercent = m_fGrowByPercent;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
void NiTQuickReleasePool< T,  TAlloc>::Reset()
{
    m_uiCurrentSize = 0;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline 
NiUInt32 NiTQuickReleasePool< T,  TAlloc>::GetNew()
{
    if (m_uiCurrentSize == m_uiMaxSize)
        SetSize(
        (NiUInt32)(((float)m_uiMaxSize + 1.0f) * (m_fGrowByPercent + 1.0f)));

    NiUInt32 uiOldSize = m_uiCurrentSize++;
    return uiOldSize;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::SetValue(
    NiUInt32 uiIndex, T& value)
{
    NIASSERT(uiIndex < m_uiCurrentSize);
    m_pPool[uiIndex] = value;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::GetValue(
    NiUInt32 uiIndex, T& value) const
{
    NIASSERT(uiIndex < m_uiCurrentSize);
    value = m_pPool[uiIndex];
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::GetPointerTo(
    NiUInt32 uiIndex, T*& pValue) const
{
    NIASSERT(uiIndex < m_uiCurrentSize);
    pValue = &m_pPool[uiIndex];
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::IncRefCount()
{
    m_uiRefCount = NiAtomicIncrement(m_uiRefCount);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::DecRefCount()
{
    NIASSERT(m_uiRefCount > 0);
    m_uiRefCount = NiAtomicDecrement(m_uiRefCount);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
NiUInt32 NiTQuickReleasePool< T,  TAlloc>::GetRefCount() const
{
    return m_uiRefCount;
}

//---------------------------------------------------------------------------
template <class T, class TAlloc>inline 
void NiTQuickReleasePool< T,  TAlloc>::Copy(
    const NiTQuickReleasePool< T,  TAlloc>* pkOther)
{
    m_uiMaxSize = pkOther->m_uiCurrentSize;
    m_uiCurrentSize = pkOther->m_uiCurrentSize;
    m_fGrowByPercent = pkOther->m_fGrowByPercent;
    m_pPool = (T*)NiRealloc(m_pPool, m_uiMaxSize * sizeof(T));
    NiMemcpy(m_pPool, pkOther->m_pPool, m_uiMaxSize * sizeof(T));
}
//---------------------------------------------------------------------------
template <class T> inline 
NiTQuickReleaseObjectPool<T>::NiTQuickReleaseObjectPool(NiUInt32 uiMaxSize, 
    float fGrowByPercent): NiTQuickReleasePool<T, NiTNewInterface<T> >
    (uiMaxSize, fGrowByPercent)
{
}
//---------------------------------------------------------------------------
template <class T> inline 
void NiTQuickReleaseObjectPool<T>::Copy(
    const NiTQuickReleaseObjectPool<T>* pkOther)
{
    NiTQuickReleasePool< T, NiTNewInterface<T> >::
        Copy((NiTQuickReleasePool<T, NiTNewInterface<T> >*)pkOther);
}
//---------------------------------------------------------------------------
template <class T> inline 
NiTQuickReleasePrimitivePool<T>::NiTQuickReleasePrimitivePool(
    NiUInt32 uiMaxSize, float fGrowByPercent): 
    NiTQuickReleasePool<T, NiTMallocInterface<T> >
    (uiMaxSize, fGrowByPercent)
{
}
//---------------------------------------------------------------------------
template <class T> inline 
void NiTQuickReleasePrimitivePool<T>::Copy(
    const NiTQuickReleasePrimitivePool<T>* pkOther)
{
    NiTQuickReleasePool< T, NiTMallocInterface<T> >::
        Copy((NiTQuickReleasePool<T, NiTMallocInterface<T> >*)pkOther);
}
