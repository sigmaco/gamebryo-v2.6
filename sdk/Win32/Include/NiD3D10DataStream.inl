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
inline void NiD3D10DataStream::SetAccessMask(NiUInt8 uiAccessMask)
{
    if (IsAccessRequestValid(uiAccessMask, m_eUsage))
        m_uiAccessMask = uiAccessMask;
}
//---------------------------------------------------------------------------
template <class LockPolicy>
inline NiD3D10LockableDataStream<LockPolicy>::NiD3D10LockableDataStream(
    const NiDataStreamElementSet& kElements, NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, D3D10_USAGE eD3D10Usage, 
    NiUInt32 uiBindFlags, NiUInt32 uiCPUAccessFlags) :
    NiD3D10DataStream(kElements, uiCount, uiAccessMask, eUsage, eD3D10Usage, 
    uiBindFlags, uiCPUAccessFlags)
{
    /* */
}
//---------------------------------------------------------------------------
template <class LockPolicy>
inline NiD3D10LockableDataStream<LockPolicy>::NiD3D10LockableDataStream(
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, D3D10_USAGE eD3D10Usage, 
    NiUInt32 uiBindFlags, NiUInt32 uiCPUAccessFlags) :
    NiD3D10DataStream(uiAccessMask, eUsage, eD3D10Usage, uiBindFlags, 
    uiCPUAccessFlags)
{
    /* */
}
//---------------------------------------------------------------------------
template <class LockPolicy>
inline NiD3D10LockableDataStream<LockPolicy>::~NiD3D10LockableDataStream()
{
    if (GetLocked())
    {
        NILOG("Warning: Destroying locked NiD3D10DataStream.\n");
    }

    m_kLockPolicy.Delete(this);
}
//---------------------------------------------------------------------------
template <class LockPolicy>
inline void* NiD3D10LockableDataStream<LockPolicy>::LockImpl(
    NiUInt8 uiLockMask)
{
    bool bForceRead = (uiLockMask & LOCK_TOOL_READ) != 0;
    bool bForceWrite = (uiLockMask & LOCK_TOOL_WRITE) != 0;
    bool bRead = (uiLockMask & LOCK_READ) != 0;
    bool bWrite = (uiLockMask & LOCK_WRITE) != 0;

    if (bRead || bForceRead)
    {
        if (bWrite || bForceWrite)
            return m_kLockPolicy.LockReadWrite(this, bForceRead, bForceWrite);
        else
            return m_kLockPolicy.LockRead(this, bForceRead);
    }
    else// if (bWrite || bForceWrite)
    {
        NIASSERT(bWrite || bForceWrite);
        return m_kLockPolicy.LockWrite(this, bForceWrite);
    }
}
//---------------------------------------------------------------------------
template <class LockPolicy>
inline void NiD3D10LockableDataStream<LockPolicy>::UnlockImpl(
    NiUInt8 uiLockMask)
{
    bool bForceRead = (uiLockMask & LOCK_TOOL_READ) != 0;
    bool bForceWrite = (uiLockMask & LOCK_TOOL_WRITE) != 0;
    bool bRead = (uiLockMask & LOCK_READ) != 0;
    bool bWrite = (uiLockMask & LOCK_WRITE) != 0;

    if (bRead || bForceRead)
    {
        if (bWrite || bForceWrite)
            return m_kLockPolicy.UnlockReadWrite(this, bForceRead, bForceWrite);
        else
            return m_kLockPolicy.UnlockRead(this, bForceRead);
    }
    else// if (bWrite || bForceWrite)
    {
        NIASSERT(bWrite || bForceWrite);
        return m_kLockPolicy.UnlockWrite(this, bForceWrite);
    }
}
//---------------------------------------------------------------------------
inline NiD3D10NonLockableDataStream::NiD3D10NonLockableDataStream(
    const NiDataStreamElementSet& kElements, NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, D3D10_USAGE eD3D10Usage, 
    NiUInt32 uiBindFlags, NiUInt32 uiCPUAccessFlags) :
    NiD3D10DataStream(kElements, uiCount, uiAccessMask, eUsage, eD3D10Usage, 
    uiBindFlags, uiCPUAccessFlags)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiD3D10NonLockableDataStream::NiD3D10NonLockableDataStream(
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, D3D10_USAGE eD3D10Usage, 
    NiUInt32 uiBindFlags, NiUInt32 uiCPUAccessFlags) :
    NiD3D10DataStream(uiAccessMask, eUsage, eD3D10Usage, uiBindFlags, 
    uiCPUAccessFlags)
{
    /* */
}
//---------------------------------------------------------------------------
inline NiD3D10NonLockableDataStream::~NiD3D10NonLockableDataStream()
{
    /* */
}
//---------------------------------------------------------------------------
inline void* NiD3D10NonLockableDataStream::LockImpl(
    NiUInt8 uiLockMask)
{
    bool bForceRead = (uiLockMask & LOCK_TOOL_READ) != 0;
    bool bForceWrite = (uiLockMask & LOCK_TOOL_WRITE) != 0;

    if (bForceRead || bForceWrite)
    {
        // This is so not thread safe, but it's a very rare situation
        SetLocked(true);
        return MapBuffer(uiLockMask, 1, 1);
    }
    else
    {
        // Failure by design.
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline void NiD3D10NonLockableDataStream::UnlockImpl(
    NiUInt8 uiLockMask)
{
    bool bForceRead = (uiLockMask & LOCK_TOOL_READ) != 0;
    bool bForceWrite = (uiLockMask & LOCK_TOOL_WRITE) != 0;

    if (bForceRead || bForceWrite)
    {
        // This is so not thread safe, but it's a very rare situation
        SetLocked(false);
        return UnmapBuffer(uiLockMask, 0, 0);
    }
}       
//---------------------------------------------------------------------------
inline bool NiD3D10DataStream::NeedsLocalBuffer(NiUInt8 uiAccessMask)
{
    return ((uiAccessMask & ACCESS_CPU_WRITE_VOLATILE) != 0) ||
        ((uiAccessMask & ACCESS_CPU_WRITE_MUTABLE) != 0);
}
//---------------------------------------------------------------------------
inline D3D10_USAGE NiD3D10DataStream::GetD3D10Usage() const
{
    return m_eD3D10Usage;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10DataStream::GetD3D10BindFlags() const
{
    return m_uiD3D10BindFlags;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiD3D10DataStream::GetD3D10CPUAccessFlags() const
{
    return m_uiD3D10CPUAccessFlags;
}
//---------------------------------------------------------------------------
inline ID3D10Buffer* NiD3D10DataStream::GetBuffer() const
{
    return m_pkD3D10Buffer;
}
//---------------------------------------------------------------------------