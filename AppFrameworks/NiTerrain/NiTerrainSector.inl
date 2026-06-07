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
inline NiTerrain* NiTerrainSector::GetTerrain()
{
    return m_pkTerrain;
}
//---------------------------------------------------------------------------
inline const NiTerrain* NiTerrainSector::GetTerrain() const
{
    return m_pkTerrain;
}
//---------------------------------------------------------------------------
inline bool NiTerrainSector::HasShapeChangedLastUpdate()
{
    return m_bHasShapeChangedLastUpdate;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetShapeChangedLastUpdate(bool bChanged)
{
    m_bHasShapeChangedLastUpdate = bChanged;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::GetVertexAt(NiTerrainVertex& kVertex, 
    const NiIndex& kIndex, NiUInt32 uiLevel) const
{
	// Assign the level if not specified:
	if (uiLevel == NiTerrainUtils::ms_uiMAX_LOD)
	{
		uiLevel = m_pkSectorData->GetNumLOD();
	}

    // Only check first existant level in debug
#ifdef NIDEBUG
    // What level does this point first exist at?
    NiUInt32 uiShallowestLevel = 0;
    NiUInt32 uiDivisor = 1 << m_pkSectorData->GetNumLOD();

    for (; uiDivisor > 0; uiDivisor /= 2)
    {
        if (kIndex.x % uiDivisor || kIndex.y % uiDivisor)
            uiShallowestLevel++;
    }
    NIASSERT(uiLevel >= uiShallowestLevel);
#endif

    // Get the block that this index belongs to. A vertex on an edge of a block
    // only 'belongs' to a single block, but is regarded as extended data to 
    // any neighboring blocks
    NiTerrainDataLeaf* pkLeaf = GetLeafContaining(kIndex, uiLevel);

    pkLeaf->GetVertexAtSector(kVertex, kIndex);
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::GetVertexAt(NiPoint3& kVertex, 
    const NiIndex& kIndex, NiUInt32 uiLevel) const
{
	// Assign the level if not specified:
	if (uiLevel == NiTerrainUtils::ms_uiMAX_LOD)
	{
		uiLevel = m_pkSectorData->GetNumLOD();
	}

    // Only check first existant level in debug
#ifdef NIDEBUG
    // What level does this point first exist at?
    NiUInt32 uiShallowestLevel = 0;
    NiUInt32 uiDivisor = 1 << m_pkSectorData->GetNumLOD();

    for (; uiDivisor > 0; uiDivisor /= 2)
    {
        if (kIndex.x % uiDivisor && kIndex.y % uiDivisor)
            uiShallowestLevel++;
    }
    NIASSERT(uiLevel >= uiShallowestLevel);
#endif

    // Get the block that this index belongs to. A vertex on an edge of a block
    // only 'belongs' to a single block, but is regarded as extended data to 
    // any neighboring blocks
    NiTerrainDataLeaf* pkLeaf = GetLeafContaining(kIndex, uiLevel);

    pkLeaf->GetVertexAtSector(kVertex, kIndex);
}
//---------------------------------------------------------------------------
template <typename T>
inline void NiTerrainSector::AddTri(NiUInt32 x0, NiUInt32 y0,
    NiUInt32 x1, NiUInt32 y1, NiUInt32 x2, NiUInt32 y2, 
    NiTStridedRandomAccessIterator<T>& kIterator)
{
    kIterator[0] = (T)(x0 + y0 * m_pkSectorData->GetBlockWidthInVerts());
    kIterator[1] = (T)(x1 + y1 * m_pkSectorData->GetBlockWidthInVerts());
    kIterator[2] = (T)(x2 + y2 * m_pkSectorData->GetBlockWidthInVerts());
    kIterator += 3; 
}
//---------------------------------------------------------------------------
inline float NiTerrainSector::GetHeightAt(
    const NiIndex& kIndex, NiUInt32 uiLevel) const
{
    NiPoint3 kPosition;
    GetVertexAt(kPosition, kIndex, uiLevel);
    return kPosition.z;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::GetSectorIndex(
    NiInt16& sXIndex, NiInt16& sYIndex) const
{
    sXIndex = m_pkSectorData->GetSectorIndexX();
    sYIndex = m_pkSectorData->GetSectorIndexY();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTerrainSector::GetArchivePath() const
{
    return m_kArchivePath;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetArchivePath(NiFixedString kArchivePath)
{
    m_kArchivePath = kArchivePath;
}
//---------------------------------------------------------------------------
inline NiFixedString NiTerrainSector::GetSectorPath(
    const char* pcArchive) const
{
    NiString kTerrainPath;
    if (pcArchive)
        kTerrainPath = pcArchive;
    else
        kTerrainPath = GetArchivePath();

    char acSectorPath[512];
    NiSnprintf(acSectorPath, 512, NI_TRUNCATE, "%s\\Sector_%d_%d", 
        (const char*)kTerrainPath, m_pkSectorData->GetSectorIndexX(),
        m_pkSectorData->GetSectorIndexY());
    return NiFixedString(acSectorPath);
}
//---------------------------------------------------------------------------
inline NiFixedString NiTerrainSector::GetSurfacePath(
    const char* pcArchive) const
{
    NiFixedString kTerrainPath;
    if (pcArchive)
        kTerrainPath = pcArchive;
    else
        kTerrainPath = GetArchivePath();

    return kTerrainPath;
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainSector::GetLeafContaining(
    const NiIndex& kIndex, NiUInt32 uiDetailLevel) const
{
    // Side length in level 0 verts  
    NiUInt32 uiSideLength= m_pkSectorData->GetBlockSize() << 
        (m_pkSectorData->GetNumLOD() - uiDetailLevel);
    NiUInt32 uiWidthInBlocks = 1 << uiDetailLevel;

    // Find out which block the index is in, at given detail level
    NiIndex kBlockIndex = kIndex / uiSideLength;

    // Deal with extended data on the right hand side of the sector
    if (kIndex.x == m_pkSectorData->GetSectorSize())
        --kBlockIndex.x;
    if (kIndex.y == m_pkSectorData->GetSectorSize())
        --kBlockIndex.y;

    // Width of this detail level, in blocks
    return m_kLeafArray[m_puiLeafOffsets[uiDetailLevel] + 
        kBlockIndex.x + (kBlockIndex.y * uiWidthInBlocks)];
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainSector::GetLeafContaining(
    const NiTerrainVertex& kVertex, NiUInt32 uiDetailLevel) const
{
    return GetLeafContaining(kVertex.GetIndex(), uiDetailLevel);
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainSector::GetLeaf(NiUInt32 uiLeafID) const
{
    if (uiLeafID < m_kLeafArray.GetSize())
    return m_kLeafArray[uiLeafID];
    else
        return 0;
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainSector::GetLeafByRegion(NiUInt32 uiLeafID) 
    const
{
    return m_kLeafRegionArray[uiLeafID];
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetLeafOffset(NiUInt32 uiDetailLevel) const
{
    return m_puiLeafOffsets[uiDetailLevel];
}
//---------------------------------------------------------------------------
inline const NiTerrainSectorData* NiTerrainSector::GetSectorData() const
{
    return m_pkSectorData;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetMetaDataStore(NiMetaDataStore* pkMetaDataStore)
{
    if (!pkMetaDataStore)
        pkMetaDataStore = NiMetaDataStore::GetInstance();

    m_pkMetaDataStore = pkMetaDataStore;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetLightingOutdated(bool bOutdated)
{
    m_bLightingOutdated = bOutdated;
}
//---------------------------------------------------------------------------
inline NiSurfacePalette* NiTerrainSector::GetSurfacePalette() const
{
    return m_spSurfacePalette;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetSurfacePalette(NiSurfacePalettePtr spPalette)
{
    m_spSurfacePalette = spPalette;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetDefaultMaskSize() const
{
    return m_uiDefaultMaskSize;
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetDefaultMaskSize(NiUInt32 uiSize)
{
    m_uiDefaultMaskSize = uiSize;
}
//---------------------------------------------------------------------------
inline float NiTerrainSector::GetLODScale() const
{
    return m_pkSectorData->GetLODScale();
}
//---------------------------------------------------------------------------
inline float NiTerrainSector::GetLODShift() const
{
    return m_pkSectorData->GetLODShift();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetLODMode() const
{
    return m_pkSectorData->GetLODMode();
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::UpdateLODData()
{
	// Load any required new data
    NiInt32 iTargetLOD = GetSectorData()->GetTargetLoadedLOD();
    NiInt32 iLoadedLOD = GetSectorData()->GetHighestLoadedLOD();
    if (iTargetLOD > iLoadedLOD)
    {
	    LoadLODData(iTargetLOD);
    }
    else if (iTargetLOD < iLoadedLOD)
    {
	    UnloadLODData(iTargetLOD + 1);
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainSector::SetTargetLoadedLOD(NiInt32 sTargetLoadedLOD, 
	bool bAllowUnloading)
{
	if (NiTerrainSectorData::InToolMode())
	{
		m_pkSectorData->SetTargetLoadedLOD(m_pkSectorData->GetNumLOD(), true);
	}
	else
	{
		m_pkSectorData->SetTargetLoadedLOD(sTargetLoadedLOD, bAllowUnloading);
	}
}
//---------------------------------------------------------------------------
inline const NiTerrainConfiguration& NiTerrainSector::GetConfiguration() const
{
    return m_pkSectorData->GetConfiguration();
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Format NiTerrainSector::GetPositionStreamFormat(
    const NiTerrainConfiguration* pkConfiguration)
    const
{
    if (!pkConfiguration)
    {
        const NiTerrainConfiguration& kTemp = GetConfiguration();
        pkConfiguration = &kTemp;
    }

    NiUInt32 uiNumComponents = pkConfiguration->GetNumPositionComponents();
    switch (uiNumComponents)
    {
    case 3:
        return NiDataStreamElement::F_FLOAT32_3;
    case 4:
        return NiDataStreamElement::F_FLOAT32_4;
    default:
        NIASSERT(!"Invalid number of position components");
    }

    // We should never get here
    return NiDataStreamElement::F_FLOAT32_3;
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Format NiTerrainSector::GetNormalStreamFormat(
    const NiTerrainConfiguration* pkConfiguration) 
    const
{
    if (!pkConfiguration)
    {
        const NiTerrainConfiguration& kTemp = GetConfiguration();
        pkConfiguration = &kTemp;
    }

    NiUInt32 uiNumComponents = pkConfiguration->GetNumNormalComponents();
    switch (uiNumComponents)
    {
    case 2: 
        return NiDataStreamElement::F_FLOAT32_2;
    case 3:
        return NiDataStreamElement::F_FLOAT32_3;
    case 4:
        return NiDataStreamElement::F_FLOAT32_4;
    default:
        NIASSERT(!"Invalid number of normal components");
    }

    // We should never get here
    return NiDataStreamElement::F_FLOAT32_3;
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Format NiTerrainSector::GetTangentStreamFormat(
    const NiTerrainConfiguration* pkConfiguration)
    const
{
    if (!pkConfiguration)
    {
        const NiTerrainConfiguration& kTemp = GetConfiguration();
        pkConfiguration = &kTemp;
    }

    NiUInt32 uiNumComponents = pkConfiguration->GetNumTangentComponents();
    switch (uiNumComponents)
    {
    case 2: 
        return NiDataStreamElement::F_FLOAT32_2;
    case 3:
        return NiDataStreamElement::F_FLOAT32_3;
    case 4:
        return NiDataStreamElement::F_FLOAT32_4;
    default:
        NIASSERT(!"Invalid number of tangent components");
    }

    // We should never get here
    return NiDataStreamElement::F_FLOAT32_3;
}
//---------------------------------------------------------------------------
