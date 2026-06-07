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

//--------------------------------------------------------------------------
inline NiDataStream::Region::Region() :
    m_uiStartIndex(0),
    m_uiRange(0)
{
}
//--------------------------------------------------------------------------
inline NiDataStream::Region::Region(NiUInt32 uiStartIndex, NiUInt32 uiRange) :
    m_uiStartIndex(uiStartIndex),
    m_uiRange(uiRange)
{
}
//--------------------------------------------------------------------------
inline void NiDataStream::Region::SetStartIndex(NiUInt32 uiStartIndex)
{
    m_uiStartIndex = uiStartIndex;
}
//--------------------------------------------------------------------------
inline void NiDataStream::Region::SetStartIndex(NiUInt32 uiStartIndex,
    NiUInt32 uiRange)
{
    m_uiStartIndex = uiStartIndex;
    m_uiRange = uiRange;
}
//--------------------------------------------------------------------------
inline NiUInt32 NiDataStream::Region::GetStartIndex() const
{
    return m_uiStartIndex;
}
//--------------------------------------------------------------------------
inline void NiDataStream::Region::SetRange(NiUInt32 uiRange)
{
    m_uiRange = uiRange;
}
//--------------------------------------------------------------------------
inline NiUInt32 NiDataStream::Region::GetRange() const
{
    return m_uiRange;
}
//--------------------------------------------------------------------------
inline void* NiDataStream::Region::ComputeStart(void* pDataStreamStart,
    NiUInt32 uiStride, NiUInt32 uiElementOffsetInBytes) const
{
    return ((char*)pDataStreamStart) + (ComputeOffsetInBytes(uiStride) + 
        uiElementOffsetInBytes);
}
//--------------------------------------------------------------------------
inline void* NiDataStream::Region::ComputeEnd(void* pDataStreamStart,
    NiUInt32 uiStride, NiUInt32 uiElementOffsetInBytes) const
{
    return ((char*)pDataStreamStart) + (ComputeOffsetInBytes(uiStride) + 
        ComputeSizeInBytes(uiStride) + uiElementOffsetInBytes);
}
//--------------------------------------------------------------------------
inline NiUInt32 NiDataStream::Region::ComputeOffsetInBytes(
    NiUInt32 uiStride) const
{
    return m_uiStartIndex * uiStride;
}
//--------------------------------------------------------------------------
inline NiUInt32 NiDataStream::Region::ComputeSizeInBytes(
    NiUInt32 uiStride) const
{
    return m_uiRange * uiStride;
}
//---------------------------------------------------------------------------
inline bool operator==(const NiDataStream::Region& r1, 
    const NiDataStream::Region& r2)
{
    return r1.IsEqual(r2);
}
//---------------------------------------------------------------------------
inline bool operator!=(const NiDataStream::Region& r1, 
    const NiDataStream::Region& r2)
{
    return !(r1.IsEqual(r2));
}
//---------------------------------------------------------------------------
inline NiDataStream::NiDataStream(
    const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    Usage eUsage) :
    m_kElements(kElements),
    m_uiSize(uiCount * kElements.m_uiStride),
    m_eUsage(eUsage),
    m_uiAccessMask(uiAccessMask),
    m_uiLockFlags(0),
    m_bLocked(false),
    m_bStreamable(true),
    m_bGPUConstantSingleEntry(false)
{
    SetDefaultCloningBehavior();
}
//---------------------------------------------------------------------------
inline NiDataStream::NiDataStream(
    NiUInt8 uiAccessMask, 
    Usage eUsage) :
    m_uiSize(0),
    m_eUsage(eUsage),
    m_uiAccessMask(uiAccessMask),
    m_uiLockFlags(0),
    m_bLocked(false),
    m_bStreamable(true),
    m_bGPUConstantSingleEntry(false)
{
    SetDefaultCloningBehavior();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetStride() const
{
    return m_kElements.m_uiStride;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetSize() const
{
    return m_uiSize;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetTotalCount() const
{
    return m_uiSize / m_kElements.m_uiStride;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetCount(NiUInt32 uiRegionIdx) const
{
    NIASSERT(uiRegionIdx < GetRegionCount());
    return GetRegion(uiRegionIdx).GetRange();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetRegionCount() const
{
    return m_kRegions.GetSize();
}
//---------------------------------------------------------------------------
inline const NiDataStream::Region& NiDataStream::GetRegion(NiUInt32 uiRegion) 
    const
{
    NIASSERT(uiRegion < m_kRegions.GetSize());
    return m_kRegions.GetAt(uiRegion);
}
//---------------------------------------------------------------------------
inline NiDataStream::Region& NiDataStream::GetRegion(NiUInt32 uiRegion)
{
    NIASSERT(uiRegion < m_kRegions.GetSize());
    return m_kRegions.GetAt(uiRegion);
}
//---------------------------------------------------------------------------
inline NiDataStream::Usage NiDataStream::GetUsage() const
{
    return m_eUsage;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStream::GetAccessMask() const
{
    return m_uiAccessMask;
}
//---------------------------------------------------------------------------
inline bool NiDataStream::GetLocked() const
{
    return m_bLocked;
}
//---------------------------------------------------------------------------
inline void NiDataStream::SetLocked(bool bLocked)
{
    m_bLocked = bLocked;
}
//---------------------------------------------------------------------------
inline const void* NiDataStream::LockRead() const
{
    // Use const_cast to implement const-safe LockRead and UnlockRead methods.
    // These methods are "read-only" by definition and implementing them
    // with const_cast avoids code duplication.
    return const_cast<NiDataStream*>(this)->Lock(NiDataStream::LOCK_READ);
}
//---------------------------------------------------------------------------
inline void NiDataStream::Unlock(NiUInt8 uiLockMask)
{
    if (GetLocked())
        UnlockImpl(uiLockMask);
}
//---------------------------------------------------------------------------
inline void* NiDataStream::LockWrite()
{
    return Lock(NiDataStream::LOCK_WRITE);
}
//---------------------------------------------------------------------------
inline void NiDataStream::UnlockWrite()
{
    Unlock(NiDataStream::LOCK_WRITE);
}
//---------------------------------------------------------------------------
inline void NiDataStream::UnlockRead() const
{
    // Use const_cast to implement const-safe LockRead and UnlockRead methods.
    // These methods are "read-only" by definition and implementing them
    // with const_cast avoids code duplication.
    const_cast<NiDataStream*>(this)->Unlock(NiDataStream::LOCK_READ);
}
//---------------------------------------------------------------------------
inline bool NiDataStream::ValidateDataAlignment(unsigned int*, 
    unsigned int) const
{
    return true;
}
//---------------------------------------------------------------------------
// Region
//---------------------------------------------------------------------------
inline bool NiDataStream::Region::IsEqual(const Region& kRhs) const
{
    return m_uiStartIndex == kRhs.m_uiStartIndex
        && m_uiRange == kRhs.m_uiRange;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::AddRegion(const Region& kRegion)
{
    return m_kRegions.Add(kRegion);
}
//---------------------------------------------------------------------------
inline void NiDataStream::SetRegion(const Region& kRegion, NiUInt32 uiRegion)
{
    NIASSERT(uiRegion < GetRegionCount());
    m_kRegions.ReplaceAt(uiRegion, kRegion);
}
//---------------------------------------------------------------------------
inline const NiDataStreamElement& NiDataStream::GetElementDescAt(
    NiUInt32 uiDescIdx) const
{
    NIASSERT(uiDescIdx < GetElementDescCount());
    return m_kElements.GetAt(uiDescIdx);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetElementDescCount() const
{
    return m_kElements.GetSize();
}
//---------------------------------------------------------------------------
inline void NiDataStream::RemoveRegion(NiUInt32 uiIndex)
{
    m_kRegions.RemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiDataStream::RemoveAllRegions()
{
    m_kRegions.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiDataStream::SetCloningBehavior(
    NiObject::CloningBehavior eCloningBehavior)
{
    m_eCloningBehavior = eCloningBehavior;
}
//---------------------------------------------------------------------------
inline NiObject::CloningBehavior NiDataStream::GetCloningBehavior() const
{
    return m_eCloningBehavior;
}
//---------------------------------------------------------------------------
inline void NiDataStream::SetFactory(NiDataStreamFactory* pkFactory)
{
    ms_pkFactory = pkFactory;
}
//---------------------------------------------------------------------------
inline NiDataStreamFactory* NiDataStream::GetFactory()
{
    return ms_pkFactory;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiSPDataStream functions
//---------------------------------------------------------------------------
inline void NiDataStream::EnableWriteFlag()
{
    m_uiLockFlags |= NiSPDataStream::LOCK_WRITE;
}
//---------------------------------------------------------------------------
inline void NiDataStream::EnableReadFlag()
{
    m_uiLockFlags |= NiSPDataStream::LOCK_READ;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStream::GetLockFlags() const
{
    return m_uiLockFlags; 
}
//---------------------------------------------------------------------------
inline void NiDataStream::ClearLockFlags()
{
    m_uiLockFlags = 0;
}
//---------------------------------------------------------------------------
inline void NiDataStream::SetGPUConstantSingleEntry(bool bSingleEntry)
{
    m_bGPUConstantSingleEntry = bSingleEntry;
}
//---------------------------------------------------------------------------
inline bool NiDataStream::GetGPUConstantSingleEntry() const
{
    return m_bGPUConstantSingleEntry;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline bool NiDataStream::GetStreamable() const
{
    return m_bStreamable;
}
//---------------------------------------------------------------------------
inline void NiDataStream::SetStreamable(bool bStreamable)
{
    m_bStreamable = bStreamable;
}
//---------------------------------------------------------------------------
inline void NiDataStream::SaveLinkID(NiStream& kStream)
{
    if (m_bStreamable)
    {
        kStream.SaveLinkID(this);
    }
}
//---------------------------------------------------------------------------
