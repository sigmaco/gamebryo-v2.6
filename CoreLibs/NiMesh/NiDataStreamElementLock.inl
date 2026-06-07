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
inline NiDataStreamElementLock::~NiDataStreamElementLock()
{
    Unlock();
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiDataStreamElementLock::begin(
    NiUInt32 uiSubmeshIdx) const
{
    NIASSERT(DataStreamExists() && IsLocked());
    NIASSERT(m_pkRef);
    const NiDataStream::Region& kRegion = 
        m_pkRef->GetRegionForSubmesh(uiSubmeshIdx);

    void* pvStart = kRegion.ComputeStart(m_pcBuffer, 
        m_pkRef->GetStride(), m_kElement.GetOffset());

    return NiTStridedRandomAccessIterator<T>((T*)pvStart, 
        m_pkRef->GetStride());
}
//---------------------------------------------------------------------------
template<typename T>
inline NiTStridedRandomAccessIterator<T> NiDataStreamElementLock::end(
    NiUInt32 uiSubmeshIdx) const
{
    NIASSERT(DataStreamExists() && IsLocked());
    NIASSERT(m_pkRef);
    const NiDataStream::Region& kRegion = 
        m_pkRef->GetRegionForSubmesh(uiSubmeshIdx);

    void* pvEnd = kRegion.ComputeEnd(m_pcBuffer, m_pkRef->GetStride(),
        m_kElement.GetOffset());

    return NiTStridedRandomAccessIterator<T>((T*)pvEnd, m_pkRef->GetStride());
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamElementLock::count(
    NiUInt32 uiSubmeshIdx) const
{
    NIASSERT(DataStreamExists() && IsLocked());
    NIASSERT(m_pkRef);
    return m_pkRef->GetCount(uiSubmeshIdx);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElementLock::DataStreamExists() const
{
    if (m_pkRef && m_pkRef->GetDataStream() != NULL)
        return true;
    return false;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElementLock::IsLocked() const
{
    return m_pcBuffer != NULL;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElementLock::GetLockMask() const
{
    return m_uiLockType;
}
//---------------------------------------------------------------------------
inline NiDataStreamRef* NiDataStreamElementLock::GetDataStreamRef()
{
    return m_pkRef;
}
//---------------------------------------------------------------------------
inline const NiDataStreamElement&
    NiDataStreamElementLock::GetDataStreamElement()
{
    return m_kElement;
}
//---------------------------------------------------------------------------
inline NiDataStream* NiDataStreamElementLock::GetDataStream()
{
    if (m_pkRef)
        return m_pkRef->GetDataStream();
    return NULL;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamElementLock::GetSubmeshCount() const
{
    return m_uiSubmeshCount;
}
//---------------------------------------------------------------------------
inline void NiDataStreamElementLock::Unlock()
{
    if (m_pcBuffer && m_pkRef)
    {
        NiDataStream* pkDS = m_pkRef->GetDataStream();
        if (pkDS)
            pkDS->Unlock(m_uiLockType);

        m_pcBuffer = NULL;
        m_pkRef = NULL;
    }
}
//---------------------------------------------------------------------------
