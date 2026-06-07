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
inline NiTIndexGenerationIterator<T>::NiTIndexGenerationIterator(
    T kValue) 
    : m_kValue(kValue)
{
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T>::NiTIndexGenerationIterator() 
    : m_kValue(T(0))
{
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T>& NiTIndexGenerationIterator<T>
    ::operator++()
{
    m_kValue += 1;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T>& NiTIndexGenerationIterator<T>
    ::operator--()
{
    m_kValue -= 1;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T> NiTIndexGenerationIterator<T>
    ::operator++(int)
{
    NiTIndexGenerationIterator<T> kResult(m_kValue);
    m_kValue += 1;
    return kResult;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T> NiTIndexGenerationIterator<T>
    ::operator--(int)
{
    NiTIndexGenerationIterator<T> kResult(m_kValue);
    m_kValue -= 1;
    return kResult;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T>& NiTIndexGenerationIterator<T>
    ::operator+=(ptrdiff_t dtElements)
{
    m_kValue = (NiUInt32)(m_kValue + dtElements);
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T>& NiTIndexGenerationIterator<T>
    ::operator-=(ptrdiff_t dtElements)
{
    m_kValue -= (NiUInt32)dtElements;
    return *this;
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T> NiTIndexGenerationIterator<T>
    ::operator+(ptrdiff_t dtElements) const
{
    return NiTIndexGenerationIterator<T>(m_kValue + dtElements);
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTIndexGenerationIterator<T> NiTIndexGenerationIterator<T>
    ::operator-(ptrdiff_t dtElements) const
{
    return NiTIndexGenerationIterator<T>(m_kValue - dtElements);
}
//---------------------------------------------------------------------------
template<typename T>
inline ptrdiff_t NiTIndexGenerationIterator<T>::operator-(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue - kRhs.m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline T NiTIndexGenerationIterator<T>::operator*() const
{
    return m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline const T* NiTIndexGenerationIterator<T>::operator->() const
{
    return &m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline T NiTIndexGenerationIterator<T>::operator[](size_t stIndex) const
{
    return m_kValue + stIndex;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTIndexGenerationIterator<T>::operator==(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue == kRhs.m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTIndexGenerationIterator<T>::operator!=(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue != kRhs.m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTIndexGenerationIterator<T>::operator<(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue < kRhs.m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTIndexGenerationIterator<T>::operator>(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue > kRhs.m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTIndexGenerationIterator<T>::operator<=(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue <= kRhs.m_kValue;
}
//---------------------------------------------------------------------------
template<typename T>
inline bool NiTIndexGenerationIterator<T>::operator>=(
    const NiTIndexGenerationIterator<T>& kRhs) const
{
    return m_kValue >= kRhs.m_kValue;
}
//---------------------------------------------------------------------------
