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
inline const NiTerrainDataLeaf* NiTerrainDataLeaf::GetChildAt(NiUInt32 uiIndex)
    const 
{
    return m_apkChildren[uiIndex];
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainDataLeaf::GetChildAt(NiUInt32 uiIndex) 
{
    return m_apkChildren[uiIndex];
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetChildAt(NiUInt32 uiIndex, 
    NiTerrainDataLeaf* pkChild)
{
    m_apkChildren[uiIndex] = pkChild;
    if (pkChild)
    {
        pkChild->m_pkParent = this;
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetParent(NiTerrainDataLeaf* pkParent)
{
    m_pkParent = pkParent;
}
//---------------------------------------------------------------------------
inline const NiTerrainDataLeaf* NiTerrainDataLeaf::GetParent() const
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainDataLeaf::GetParent() 
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
inline const NiTerrainSector* NiTerrainDataLeaf::GetContainingSector() const
{
    return m_pkContainingSector;
}
//---------------------------------------------------------------------------
inline NiTerrainSector* NiTerrainDataLeaf::GetContainingSector()
{
    return m_pkContainingSector;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainDataLeaf::GetBlockSize() const
{
    return m_pkSectorData->GetBlockSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainDataLeaf::GetWidthInVerts() const
{
    return m_pkSectorData->GetBlockWidthInVerts();
}
//---------------------------------------------------------------------------
inline const NiBound& NiTerrainDataLeaf::GetLocalBound() const 
{
    return m_kBound;
}
//---------------------------------------------------------------------------
inline const NiBoxBV& NiTerrainDataLeaf::GetLocalBoxBound() const 
{
    return m_kBoxBound;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainDataLeaf::GetLevel() const
{    
    return m_uiLevel;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetLevel(NiUInt32 uiLevel)
{
    m_uiLevel = uiLevel;
    m_uiVertexStride = 1 << GetNumSubDivisions();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainDataLeaf::GetNumSubDivisions() const
{    
    return m_pkSectorData->GetNumLOD() - GetLevel();
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetBottomLeftIndex(NiIndex& kIndex) const
{
    kIndex = m_kDataBottomLeftIndex;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetBottomLeftIndex(const NiIndex& kIndex) 
{
    m_kDataBottomLeftIndex = kIndex;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainDataLeaf::GetRegionID() const
{
    return m_uiRegionID;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetRegionID(NiUInt32 uiRegionID)
{
    m_uiRegionID = uiRegionID;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainDataLeaf::GetBlockID() const
{
    return m_uiBlockID;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetBlockID(NiUInt32 uiBlockID)
{
    m_uiBlockID = uiBlockID;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetVertexAt(
    NiPoint3& kVertex, const NiIndex& kLocal) const 
{
    NIASSERT(kLocal.x <= GetWidthInVerts());
    NIASSERT(kLocal.y <= GetWidthInVerts());

    m_kPositionIterator.GetHighDetail(kLocal.x + kLocal.y * GetWidthInVerts(),
        kVertex);    
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetVertexAtSector(
    NiTerrainVertex& kVertex, const NiIndex& kSector) const 
{
    NiIndex kLocal;

#ifdef NIDEBUG
    IndexToLocal(kSector, kLocal);
#else
    IndexToLocalFast(kSector, kLocal);
#endif

    GetVertexAt(kVertex, kLocal);
}
//---------------------------------------------------------------------------
inline float NiTerrainDataLeaf::GetHeightAt(const NiIndex& kLocal) const
{
    NIASSERT(kLocal.x < GetWidthInVerts());
    NIASSERT(kLocal.y < GetWidthInVerts());
    
    return m_kPositionIterator.GetComponent(
        kLocal.x + kLocal.y * GetWidthInVerts(), 
        NiTerrainPositionRandomAccessIterator::Z);
}
//---------------------------------------------------------------------------
inline float NiTerrainDataLeaf::GetHeightAtSector(const NiIndex& kSector)
    const
{
    NiIndex kLocal;

#ifdef NIDEBUG
    IndexToLocal(kSector, kLocal);
#else
    IndexToLocalFast(kSector, kLocal);
#endif

    return GetHeightAt(kLocal);
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetVertexAtSector(
    NiPoint3& kVertex, const NiIndex& kSector) const 
{
    NiIndex kLocal;
#ifdef NIDEBUG
    IndexToLocal(kSector, kLocal);
#else
    IndexToLocalFast(kSector, kLocal);
#endif

    GetVertexAt(kVertex, kLocal);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiTerrainDataLeaf::GetHasExtendedData() const
{
    return m_ucExtendedDataMask;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetHasExtendedData(NiUInt8 ucMask)
{
    // 14 = TOP | RIGHT | CORNER
    m_ucExtendedDataMask = ucMask & 14;
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainDataLeaf::GetAdjacent(NiUInt32 uiBorder)
{
    return DoGetAdjacent(uiBorder);
}
//---------------------------------------------------------------------------
inline const NiTerrainDataLeaf* NiTerrainDataLeaf::GetAdjacent(
    NiUInt32 uiBorder) const
{
    return DoGetAdjacent(uiBorder);
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainDataLeaf::GetAdjacentFast(NiUInt32 uiBorder)
{
    return DoGetAdjacentFast(uiBorder);
}
//---------------------------------------------------------------------------
inline const NiTerrainDataLeaf* NiTerrainDataLeaf::GetAdjacentFast(
    NiUInt32 uiBorder) const
{
    return DoGetAdjacentFast(uiBorder);
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiTerrainDataLeaf::GetSumOfVertexPositions() const
{
    return m_kSumOfVertexLocations;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetSumOfVertexPositions(const NiPoint3& kSum) 
{
    m_kSumOfVertexLocations = kSum;
    RequestUpdate();
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetBoundVolumeBox(const NiBoxBV& kBox)
{
    m_kBoxBound = kBox;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetUseCustomSurfacePriority(
    bool bUseCustomSurfacePriority)
{
    m_bUseCustomSurfacePriority = bUseCustomSurfacePriority;
}
//---------------------------------------------------------------------------
inline bool NiTerrainDataLeaf::GetUseCustomSurfacePriority() const
{
    return m_bUseCustomSurfacePriority;
}
//---------------------------------------------------------------------------
inline const NiIndex NiTerrainDataLeaf::SectorIndexFromLocal(
    const NiIndex& kLocal) const
{
    return NiIndex(
        m_kDataBottomLeftIndex.x + (kLocal.x << GetNumSubDivisions()),
        m_kDataBottomLeftIndex.y + (kLocal.y << GetNumSubDivisions()));
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::IndexToLocalFast(
    const NiIndex& kSector, NiIndex& kLocal) const
{
    kLocal = kSector - m_kDataBottomLeftIndex;
    kLocal.x >>= GetNumSubDivisions();
    kLocal.y >>= GetNumSubDivisions();
}
//---------------------------------------------------------------------------
inline bool NiTerrainDataLeaf::TreeContains(const NiTerrainVertex& kVertex) 
    const
{    
    // Are we even inside the correct range?
    NiIndex kIndexMin = m_kDataBottomLeftIndex;

    if (kVertex.GetIndex().x < kIndexMin.x || 
        kVertex.GetIndex().y < kIndexMin.y
        )
    {
        return false;
    }

    NiUInt32 uiInc = GetBlockSize() << GetNumSubDivisions();
    NiIndex kIndexMax(
        kIndexMin.x + uiInc,
        kIndexMin.y + uiInc);

    if (kVertex.GetIndex().x > kIndexMax.x ||
        kVertex.GetIndex().y > kIndexMax.y)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetPositionIterator(
    NiTerrainPositionRandomAccessIterator& kIterator) const
{
    kIterator = m_kPositionIterator;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetNormalIterator(
    NiTerrainNormalRandomAccessIterator& kIterator) const
{
    kIterator = m_kNormalIterator;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetTangentIterator(
    NiTerrainTangentRandomAccessIterator& kIterator) const
{
    kIterator = m_kTangentIterator;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetIndexIterator(
    NiTStridedRandomAccessIterator<NiUInt16>& kIterator) const
{
   kIterator = m_kIndexIteratorNiUInt16;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetIndexIterator(
    NiTStridedRandomAccessIterator<NiUInt32>& kIterator) const
{
   kIterator = m_kIndexIteratorNiUInt32;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::GetUVIterator(
    NiTStridedRandomAccessIterator<NiPoint2>& kIterator) const
{
    kIterator = m_kUVIterator;
}
//---------------------------------------------------------------------------
inline bool NiTerrainDataLeaf::RequiresUpdate() const
{
    return m_bRequiresUpdate;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::SetQuadMesh(NiTerrainBlock* pkQuadMesh)
{
    m_pkQuadMesh = pkQuadMesh;
}
//---------------------------------------------------------------------------
inline NiTerrainBlock* NiTerrainDataLeaf::GetQuadMesh() const
{
    return m_pkQuadMesh;
}
//---------------------------------------------------------------------------
inline void NiTerrainDataLeaf::RecenterCountainingBound(
    const NiIndex& kIndex, float fOldHeight, float fNewHeight)
{
    // Even though the X and Y from the index aren't the same as the local
    // coordinate, since the difference is subtracted then added, it makes no
    // difference.
    NiPoint3 kToSubstract = NiPoint3(
        (float)kIndex.x, (float)kIndex.y, fOldHeight);
    NiPoint3 kToAdd = NiPoint3(
        (float)kIndex.x, (float)kIndex.y, fNewHeight);

    // Update our vertex location sum
    m_kSumOfVertexLocations -= kToSubstract;
    m_kSumOfVertexLocations += kToAdd;

    float fZDist = NiAbs((m_kSumOfVertexLocations / 
        (float)GetWidthInVerts()).z - fNewHeight);
    
    // Part 1 of 2 of box bounds update
    if (fZDist > m_kBoxBound.GetExtent(2)) 
        m_kBoxBound.SetExtent(2, fZDist);
   
    // Request an update
    RequestUpdate();
}
//---------------------------------------------------------------------------
#if defined (WIN32) || defined (_XENON)
#pragma inline_recursion(on)
#pragma inline_depth(6)
#endif
inline void NiTerrainDataLeaf::RequestUpdate()
{
    m_bRequiresUpdate = true;

    // Get a point that is first seen in our parent
    NiTerrainDataLeaf* pkParent = GetParent();
    if (!pkParent || pkParent->RequiresUpdate()) 
        return;
    else
        pkParent->RequestUpdate();
}
#if defined (WIN32) || defined (_XENON)
#pragma inline_recursion(off)
#endif
