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
inline void NiGeometryConverter::SetAutoGeometryConversion(bool bEnable)
{
    ms_bAutoGeometryConversion = bEnable;
}
//---------------------------------------------------------------------------
inline bool NiGeometryConverter::GetAutoGeometryConversion()
{
    return ms_bAutoGeometryConversion;
}
//--------------------------------------------------------------------------
inline void NiGeometryConverter::ClearGeometryCache()
{
    m_kSharedDataMap.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiGeometryConverter::SetCPUWriteAccess(NiUInt32& uiAccess,
    NiDataStream::Access uiMask)
{
    uiAccess &= ~NiDataStream::ACCESS_CPU_WRITE_ANY;
    uiAccess |= uiMask;
}
//--------------------------------------------------------------------------
inline void NiGeometryConverter::SetCPUReadAccess(NiUInt32& uiAccess)
{
    uiAccess |= NiDataStream::ACCESS_CPU_READ;
    if (uiAccess & NiDataStream::ACCESS_CPU_WRITE_VOLATILE)
        SetCPUWriteAccess(uiAccess, NiDataStream::ACCESS_CPU_WRITE_MUTABLE);
}
//--------------------------------------------------------------------------
template <class T>
inline void NiGeometryConverter::BuildPartitions(NiDataStreamRef* pkStreamRef,
    NiUInt32 uiDestCount, const T* pkSrcData, NiUInt32 uiSrcCount, 
    NiSkinPartition::Partition* pkPartitions, NiUInt32 uiPartitionCount)
{
    NIASSERT(pkStreamRef);
    NiDataStream* pkDataStream = pkStreamRef->GetDataStream();
    NIASSERT(pkDataStream);

    // Copy vertex data and create new regions
    NiUInt32 eLock = NiDataStream::LOCK_WRITE;
    T* pkDestData = static_cast<T*>(pkDataStream->Lock((NiUInt8)eLock));

    if (uiPartitionCount == 0)
    {
        // Copy vertex data 
        size_t stDestSize = uiDestCount * sizeof(T);
        size_t stSrcSize = uiSrcCount * sizeof(T);
        NiMemcpy(pkDestData, stDestSize, pkSrcData, stSrcSize);

        // Add region
        NiDataStream::Region kRegion(0, uiDestCount);
        pkStreamRef->BindRegionToSubmesh(0, kRegion);
    }
    else 
    {
        NiUInt32 uiVertexStart = 0;
        for (NiUInt32 ui = 0; ui < uiPartitionCount; ui++)
        {
            NiSkinPartition::Partition* pkPartition = &pkPartitions[ui];
            NIASSERT(pkPartition);

            // Copy vertex data 
            NiUInt16 uiVertexCount = pkPartition->m_usVertices;
            NIASSERT(uiVertexStart + uiVertexCount <= uiDestCount);
            for (NiUInt32 i = 0; i < uiVertexCount; i++)
            {
                NiUInt32 uiIndex = pkPartition->m_pusVertexMap[i];
                NIASSERT(uiIndex < uiSrcCount);
                *pkDestData++ = pkSrcData[uiIndex];
            }

            // Add regions
            NiUInt32 uiRange = uiVertexCount;
            NiDataStream::Region kRegion(uiVertexStart, uiRange);
            pkStreamRef->BindRegionToSubmesh(ui, kRegion);
            uiVertexStart += uiVertexCount;
        }
    }
    pkDataStream->Unlock((NiUInt8)eLock);
}
//--------------------------------------------------------------------------
template <class T>
inline void NiGeometryConverter::BuildPartitions(NiDataStreamRef* pkStreamRef,
     const T* pkSrcData)
{
    BuildPartitions<T>(pkStreamRef, m_uiMeshVertexCount, pkSrcData, 
        m_uiGeomVertexCount, m_pkPartitions, m_uiPartitionCount);
}
//--------------------------------------------------------------------------
