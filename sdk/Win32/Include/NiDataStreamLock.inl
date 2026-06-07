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
inline NiDataStreamLock::NiDataStreamLock() : m_pkDataStream(0), 
    m_stElementOffset(0), m_pcBuffer(0), 
    m_uiLockType(0)
{
}
//---------------------------------------------------------------------------
inline NiDataStreamLock::NiDataStreamLock(
    NiDataStream* pkDataStream, size_t stElementOffset, NiUInt8 uiLockMask) : 
    m_pkDataStream(pkDataStream), m_stElementOffset(stElementOffset),
    m_pcBuffer(0), m_uiLockType(uiLockMask)
{
    if (m_pkDataStream)
    {
        m_stElementOffset = stElementOffset;
        m_pcBuffer = (char*)m_pkDataStream->Lock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
inline NiDataStreamLock::NiDataStreamLock(const NiDataStreamLock& kOther) :
    m_pkDataStream(kOther.m_pkDataStream), 
    m_stElementOffset(kOther.m_stElementOffset), 
    m_pcBuffer(0), m_uiLockType(kOther.m_uiLockType)
{
    if (m_pkDataStream)
    {
        m_pcBuffer = (char*)m_pkDataStream->Lock(m_uiLockType);
    }
}
//---------------------------------------------------------------------------
inline NiDataStreamLock& NiDataStreamLock::operator=(
    const NiDataStreamLock& kRHS)
{
    if (this != &kRHS)
    {
        // If already created and locked, we must unlock the previously 
        // assigned data stream.
        Unlock();

        m_pkDataStream = kRHS.m_pkDataStream;
        m_stElementOffset = kRHS.m_stElementOffset;
        m_pcBuffer = 0;
        m_uiLockType = kRHS.m_uiLockType;
        if (m_pkDataStream)
        {
            m_pcBuffer = (char*)m_pkDataStream->Lock(m_uiLockType);
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
inline NiDataStreamLock::~NiDataStreamLock()
{
    Unlock();
}

//---------------------------------------------------------------------------
inline bool NiDataStreamLock::DataStreamExists() const
{
    return m_pkDataStream != 0;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamLock::IsLocked() const
{
    return m_pcBuffer != 0;
}
//---------------------------------------------------------------------------
inline NiDataStream* NiDataStreamLock::GetDataStream()
{
    return m_pkDataStream; 
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiDataStreamLock::begin() 
const
{
    NIASSERT(DataStreamExists() && IsLocked());
    return NiTStridedRandomAccessIterator<T>(
        reinterpret_cast<T*>(m_pcBuffer + m_stElementOffset), 
        m_pkDataStream->GetStride());
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiDataStreamLock::end()
const
{
    NIASSERT(DataStreamExists() && IsLocked());
    return NiTStridedRandomAccessIterator<T>(
        reinterpret_cast<T*>(
        m_pcBuffer + m_stElementOffset + m_pkDataStream->GetTotalCount() * 
        m_pkDataStream->GetStride()), 
        m_pkDataStream->GetStride());
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamLock::count() const
{
    NIASSERT(DataStreamExists() && IsLocked());
    return m_pkDataStream->GetTotalCount();
}
//---------------------------------------------------------------------------
template<typename T> 
inline NiTStridedRandomAccessIterator<T> NiDataStreamLock::begin_region(
    NiUInt32 uiRegionIdx) const
{
    NIASSERT(DataStreamExists() && IsLocked());
    const NiDataStream::Region& kRegion = 
        m_pkDataStream->GetRegion(uiRegionIdx);

    return NiTStridedRandomAccessIterator<T>(
        reinterpret_cast<T*>(kRegion.ComputeStart(m_pcBuffer, 
        m_pkDataStream->GetStride(), (NiUInt32)m_stElementOffset)), 
        m_pkDataStream->GetStride());
}
//---------------------------------------------------------------------------
template<typename T> 
inline NiTStridedRandomAccessIterator<T> NiDataStreamLock::end_region(
    NiUInt32 uiRegionIdx) const
{
    NIASSERT(DataStreamExists() && IsLocked());
    const NiDataStream::Region& kRegion = 
        m_pkDataStream->GetRegion(uiRegionIdx);

    return NiTStridedRandomAccessIterator<T>(
        reinterpret_cast<T*>(kRegion.ComputeEnd(m_pcBuffer, 
        m_pkDataStream->GetStride(), (NiUInt32)m_stElementOffset)),
        m_pkDataStream->GetStride());
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamLock::count_region(NiUInt32 uiRegionIdx) const
{
    if (m_pkDataStream)
    {
        const NiDataStream::Region& kRegion = 
            m_pkDataStream->GetRegion(uiRegionIdx);

        return kRegion.GetRange();
    }
    return 0;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamLock::GetNumRegions() const
{
    if (m_pkDataStream)
        return m_pkDataStream->GetRegionCount();
    return 0;
}
//---------------------------------------------------------------------------
inline void NiDataStreamLock::Unlock()
{
    if (m_pkDataStream && m_pcBuffer)
    {
        m_pkDataStream->Unlock(m_uiLockType);
        m_pkDataStream = NULL;
        m_pcBuffer = NULL;
    }
}
//---------------------------------------------------------------------------
