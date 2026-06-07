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
// Static
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template <class TDataStream>
StaticLockPolicy<TDataStream>::StaticLockPolicy() : 
    m_uiWriters(0),
    m_uiReadLockCount(0)
{}
//---------------------------------------------------------------------------
template <class TDataStream>
void* StaticLockPolicy<TDataStream>::LockRead(
    TDataStream* pkDS, 
    bool bForceRead)
{
    NiUInt32 uiLockCount = NiAtomicIncrement(m_uiReadLockCount);
    NiUInt8 uiLockType = NiDataStream::LOCK_READ;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    void* pvRetVal= pkDS->MapBuffer(uiLockType, uiLockCount, 0);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void* StaticLockPolicy<TDataStream>::LockWrite(
    TDataStream* pkDS, 
    bool bForceWrite)
{
    ++m_uiWriters;
    NIASSERT(m_uiWriters > 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    void* pvRetVal = pkDS->MapBuffer(uiLockType, 0, m_uiWriters);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void* StaticLockPolicy<TDataStream>::LockReadWrite(
    TDataStream* pkDS, 
    bool bForceRead, 
    bool bForceWrite)
{
    ++m_uiWriters;
    ++m_uiReadLockCount;
    NIASSERT(m_uiWriters > 0);
    NIASSERT(m_uiReadLockCount > 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    void* pvRetVal = pkDS->MapBuffer(
        uiLockType, 
        m_uiReadLockCount, 
        m_uiWriters);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void StaticLockPolicy<TDataStream>::UnlockRead(
    TDataStream* pkDS, 
    bool bForceRead)
{
    NiUInt32 uiLockCount = NiAtomicDecrement(m_uiReadLockCount);
    pkDS->SetLocked((m_uiWriters != 0) || (uiLockCount != 0));
    NiUInt8 uiLockType = NiDataStream::LOCK_READ;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    pkDS->UnmapBuffer(uiLockType, uiLockCount, 0);
}
//---------------------------------------------------------------------------
template <class TDataStream>
void StaticLockPolicy<TDataStream>::UnlockWrite(
    TDataStream* pkDS, 
    bool bForceWrite)
{
    if (m_uiWriters > 0)
    {
        --m_uiWriters;
        NIASSERT(m_uiWriters != 0xFFFFFFFF);
        if (m_uiWriters == 0)
        {
            pkDS->SetAccessMask((NiUInt8)(pkDS->GetAccessMask() | 
                NiDataStream::ACCESS_CPU_WRITE_STATIC_INITIALIZED));
        }
    }
    pkDS->SetLocked((m_uiWriters != 0) || (m_uiReadLockCount != 0));
    NiUInt8 uiLockType = NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    pkDS->UnmapBuffer(uiLockType, 
        m_uiReadLockCount, 
        m_uiWriters);
}
//---------------------------------------------------------------------------
template <class TDataStream>
void StaticLockPolicy<TDataStream>::UnlockReadWrite(
    TDataStream* pkDS, 
    bool bForceRead, 
    bool bForceWrite)
{
    --m_uiReadLockCount;
    NIASSERT(m_uiReadLockCount != 0xFFFFFFFF);
    if (m_uiWriters > 0)
    {
        --m_uiWriters;
        NIASSERT(m_uiWriters != 0xFFFFFFFF);
        if (m_uiWriters == 0)
        {
            pkDS->SetAccessMask((NiUInt8)(pkDS->GetAccessMask() | 
                NiDataStream::ACCESS_CPU_WRITE_STATIC_INITIALIZED));
        }
    }
    pkDS->SetLocked((m_uiWriters != 0) || (m_uiReadLockCount != 0));
    NiUInt8 uiLockType = NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    pkDS->UnmapBuffer(
        uiLockType,
        m_uiReadLockCount, 
        m_uiWriters);
}
//---------------------------------------------------------------------------
template <class TDataStream>
void StaticLockPolicy<TDataStream>::Delete(TDataStream*)
{
    // Assert that the stream is not currently locked while we're deleting.
    // Deleting a locked stream would indicate a logical error since the
    // locked region would be invalidated.
    NIASSERT(m_uiWriters == 0);
    NIASSERT(m_uiReadLockCount == 0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Volatile
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template <class TDataStream>
VolatileLockPolicy<TDataStream>::VolatileLockPolicy() : 
    m_uiWriteLockCount(0)
{}
//---------------------------------------------------------------------------
template <class TDataStream>
void* VolatileLockPolicy<TDataStream>::LockRead(
    TDataStream* pkDS, 
    bool bForceRead)
{
    NIASSERT(bForceRead);
    // Assume the read lock count is one
    void* pvRetVal= pkDS->MapBuffer(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ, 
        1, 
        0);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void* VolatileLockPolicy<TDataStream>::LockWrite(
    TDataStream* pkDS, 
    bool bForceWrite)
{
    // no region locking supported for volatile

    // There is no requirement that locking a data stream enforce 
    // dependencies within a single frame between NiRenderObject objects that 
    // share that volatile data stream.

    // Therefore we don't protect against multiple writers.

    // Get new buffer

    // Lock system resource
    //pkDS->SystemLock();

    // replace the resource on the data stream

    // release the old resource 
    // (must handle deferring deletion on resources still in use)

    // return buffer
    NiUInt32 uiLockCount = NiAtomicIncrement(m_uiWriteLockCount);
    NiUInt8 uiLockType = NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    void* pvRetVal = pkDS->MapBuffer(uiLockType, 0, uiLockCount);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void* VolatileLockPolicy<TDataStream>::LockReadWrite(
    TDataStream* pkDS, 
    bool bForceRead, 
    bool bForceWrite)
{
    NIASSERT(bForceRead);
    // Assume the read lock count is one
    NiUInt32 uiLockCount = NiAtomicIncrement(m_uiWriteLockCount);
    NiUInt8 uiLockType = NiDataStream::LOCK_READ | 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    void* pvRetVal= pkDS->MapBuffer(
        uiLockType,
        1, 
        uiLockCount);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void VolatileLockPolicy<TDataStream>::UnlockRead(
    TDataStream* pkDS, 
    bool bForceRead)
{
    NIASSERT(bForceRead);
    pkDS->SetLocked(m_uiWriteLockCount != 0);
    // Assume the read lock count is now zero
    pkDS->UnmapBuffer(NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ,
        0, 
        0);
}
//---------------------------------------------------------------------------
template <class TDataStream>
void VolatileLockPolicy<TDataStream>::UnlockWrite(TDataStream* pkDS, 
    bool bForceWrite)
{
    NiUInt32 uiLockCount = NiAtomicDecrement(m_uiWriteLockCount);
    pkDS->SetLocked(uiLockCount != 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    pkDS->UnmapBuffer(
        uiLockType, 
        0, 
        uiLockCount);
}
//---------------------------------------------------------------------------
template <class TDataStream>
void VolatileLockPolicy<TDataStream>::UnlockReadWrite(
    TDataStream* pkDS, 
    bool bForceRead, 
    bool bForceWrite)
{
    NIASSERT(bForceRead);
    NiUInt32 uiLockCount = NiAtomicDecrement(m_uiWriteLockCount);
    pkDS->SetLocked(uiLockCount != 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_READ | 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    pkDS->UnmapBuffer(
        uiLockType, 
        0, 
        uiLockCount);
}
//---------------------------------------------------------------------------
template <class TDataStream>
void VolatileLockPolicy<TDataStream>::Delete(TDataStream*)
{
    // Assert that the stream is not currently locked while we're deleting.
    // Deleting a locked stream would indicate a logical error since the
    // locked region would be invalidated.
    NIASSERT(m_uiWriteLockCount == 0);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Mutable
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template <class TDataStream>
MutableLockPolicy<TDataStream>::MutableLockPolicy() : 
    m_kLock(),
    m_uiReadLockCount(0),
    m_uiWriteLockCount(0)
{}
//---------------------------------------------------------------------------
template <class TDataStream>
void* MutableLockPolicy<TDataStream>::LockRead(
    TDataStream* pkDS, 
    bool bForceRead)
{
    m_kLock.Lock();
    ++m_uiReadLockCount;
    NIASSERT(m_uiReadLockCount > 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_READ;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    void* pvRetVal = pkDS->MapBuffer(
        uiLockType, 
        m_uiReadLockCount, 
        m_uiWriteLockCount);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void* MutableLockPolicy<TDataStream>::LockWrite(
    TDataStream* pkDS, 
    bool bForceWrite)
{
    // no region locking supported for Mutable

    // There is no requirement that locking a data stream enforce 
    // dependencies within a single frame between NiRenderObject objects that 
    // share that Mutable data stream.

    // Therefore we don't protect against multiple writers.

    // Get new buffer

    // Lock system resource
    //pkDS->SystemLock();

    // replace the resource on the data stream

    // release the old resource 
    // (must handle deferring deletion on resources still in use)

    // return buffer
    m_kLock.Lock();
    ++m_uiWriteLockCount;
    NIASSERT(m_uiWriteLockCount > 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    void* pvRetVal = pkDS->MapBuffer(
        uiLockType, 
        m_uiReadLockCount, 
        m_uiWriteLockCount);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void* MutableLockPolicy<TDataStream>::LockReadWrite(
    TDataStream* pkDS, 
    bool bForceRead, 
    bool bForceWrite)
{
    m_kLock.Lock();
    ++m_uiReadLockCount;
    ++m_uiWriteLockCount;
    NIASSERT(m_uiReadLockCount > 0);
    NIASSERT(m_uiWriteLockCount > 0);
    NiUInt8 uiLockType = NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    void* pvRetVal = pkDS->MapBuffer(uiLockType,
        m_uiReadLockCount, 
        m_uiWriteLockCount);
    pkDS->SetLocked(pvRetVal != 0);
    return pvRetVal;
}
//---------------------------------------------------------------------------
template <class TDataStream>
void MutableLockPolicy<TDataStream>::UnlockRead(
    TDataStream* pkDS, 
    bool bForceRead)
{
    --m_uiReadLockCount;
    NIASSERT(m_uiReadLockCount != 0xFFFFFFFF)
    pkDS->SetLocked((m_uiWriteLockCount != 0) || (m_uiReadLockCount != 0));
    NiUInt8 uiLockType = NiDataStream::LOCK_READ;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    pkDS->UnmapBuffer(uiLockType, m_uiReadLockCount,
        m_uiWriteLockCount);
    m_kLock.Unlock();
}
//---------------------------------------------------------------------------
template <class TDataStream>
void MutableLockPolicy<TDataStream>::UnlockWrite(
    TDataStream* pkDS, 
    bool bForceWrite)
{
    --m_uiWriteLockCount;
    NIASSERT(m_uiWriteLockCount != 0xFFFFFFFF)
    pkDS->SetLocked((m_uiWriteLockCount != 0) || (m_uiReadLockCount != 0));
    NiUInt8 uiLockType = NiDataStream::LOCK_WRITE;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    pkDS->UnmapBuffer(uiLockType, 
        m_uiReadLockCount,
        m_uiWriteLockCount);
    m_kLock.Unlock();
}
//---------------------------------------------------------------------------
template <class TDataStream>
void MutableLockPolicy<TDataStream>::UnlockReadWrite(
    TDataStream* pkDS, 
    bool bForceRead, 
    bool bForceWrite)
{
    --m_uiReadLockCount;
    --m_uiWriteLockCount;
    NIASSERT(m_uiReadLockCount != 0xFFFFFFFF)
    NIASSERT(m_uiWriteLockCount != 0xFFFFFFFF)
    pkDS->SetLocked((m_uiWriteLockCount != 0) || (m_uiReadLockCount != 0));
    NiUInt8 uiLockType = NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE;
    if (bForceRead)
        uiLockType |= NiDataStream::LOCK_TOOL_READ;
    if (bForceWrite)
        uiLockType |= NiDataStream::LOCK_TOOL_WRITE;
    pkDS->UnmapBuffer(uiLockType, 
        m_uiReadLockCount, 
        m_uiWriteLockCount);
    m_kLock.Unlock();
}
//---------------------------------------------------------------------------
template <class TDataStream>
void MutableLockPolicy<TDataStream>::Delete(TDataStream*)
{
    // Assert that the stream is not currently locked while we're deleting.
    // Deleting a locked stream would indicate a logical error since the
    // locked region would be invalidated.
    NIASSERT(m_uiReadLockCount == 0);
    NIASSERT(m_uiWriteLockCount == 0);
}
//---------------------------------------------------------------------------
