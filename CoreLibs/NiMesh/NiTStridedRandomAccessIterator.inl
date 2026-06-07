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
template<typename T>
inline NiTStridedRandomAccessIterator<T>::NiTStridedRandomAccessIterator() 
    : m_pcObject(0)
    , m_dtStride(0)
{
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T>::NiTStridedRandomAccessIterator(
    T* pkObject, ptrdiff_t dtStride) 
    : m_pcObject((char*)pkObject)
    , m_dtStride(dtStride)
{
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::Exists() const
{
    return m_pcObject != 0;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T>& NiTStridedRandomAccessIterator<T>
    ::operator++()
{
    m_pcObject += m_dtStride;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T>& NiTStridedRandomAccessIterator<T>
    ::operator--()
{
    m_pcObject -= m_dtStride;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiTStridedRandomAccessIterator<T>
    ::operator++(int)
{
    NiTStridedRandomAccessIterator<T> kResult(reinterpret_cast<T*>(m_pcObject),
        m_dtStride);
    m_pcObject += m_dtStride;
    return kResult;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiTStridedRandomAccessIterator<T>
    ::operator--(int)
{
    NiTStridedRandomAccessIterator<T> kResult(reinterpret_cast<T*>(m_pcObject),
        m_dtStride);
    m_pcObject -= m_dtStride;
    return kResult;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T>& NiTStridedRandomAccessIterator<T>
    ::operator+=(ptrdiff_t dtElements)
{
    m_pcObject += m_dtStride * dtElements;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T>& NiTStridedRandomAccessIterator<T>
    ::operator-=(ptrdiff_t dtElements)
{
    m_pcObject -= m_dtStride * dtElements;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiTStridedRandomAccessIterator<T>
    ::operator+(ptrdiff_t dtElements) const
{
    return NiTStridedRandomAccessIterator<T>(
        reinterpret_cast<T*>(m_pcObject + m_dtStride * dtElements), 
        m_dtStride);
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiTStridedRandomAccessIterator<T>
    ::operator-(ptrdiff_t dtElements) const
{
    return NiTStridedRandomAccessIterator<T>(
        reinterpret_cast<T*>(m_pcObject - m_dtStride * dtElements), 
        m_dtStride);
}
//---------------------------------------------------------------------------
template<typename T>
inline ptrdiff_t NiTStridedRandomAccessIterator<T>::operator-(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    return (m_pcObject - kRhs.m_pcObject) / m_dtStride;
}
//---------------------------------------------------------------------------
template<typename T>
inline T& NiTStridedRandomAccessIterator<T>::operator*() const
{
    return *reinterpret_cast<T*>(m_pcObject);
}
//---------------------------------------------------------------------------
template<typename T>
inline T* NiTStridedRandomAccessIterator<T>::operator->() const
{
    return reinterpret_cast<T*>(m_pcObject);
}
//---------------------------------------------------------------------------
template<typename T>
inline T& NiTStridedRandomAccessIterator<T>::operator[](size_t stIndex) const
{
    return *reinterpret_cast<T*>(m_pcObject + m_dtStride * stIndex);
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::operator==(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    bool bTest1 = m_pcObject == kRhs.m_pcObject;
    bool bTest2 = m_dtStride == kRhs.m_dtStride;
    return bTest1 && bTest2;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::operator!=(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    bool bTest1 = m_pcObject != kRhs.m_pcObject;
    bool bTest2 = m_dtStride != kRhs.m_dtStride;
    return bTest1 || bTest2;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::operator<(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    return m_pcObject < kRhs.m_pcObject;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::operator>(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    return m_pcObject > kRhs.m_pcObject;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::operator<=(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    return m_pcObject <= kRhs.m_pcObject;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTStridedRandomAccessIterator<T>::operator>=(
    const NiTStridedRandomAccessIterator<T>& kRhs) const
{
    return m_pcObject >= kRhs.m_pcObject;
}
//---------------------------------------------------------------------------
