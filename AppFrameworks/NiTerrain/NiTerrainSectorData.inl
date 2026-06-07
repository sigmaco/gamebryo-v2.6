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
inline NiUInt32 NiTerrainSectorData::GetBlockSize() const
{
    return m_uiBlockSize;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorData::GetBlockWidthInVerts() const
{
    return m_uiBlockWidthInVerts;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorData::GetSectorSize() const
{
    return m_uiSectorSize;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorData::GetSectorWidthInVerts() const
{
    return m_uiSectorWidthInVerts;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetBlockSize(NiUInt32 uiSize)
{
    NIASSERT(NiIsPowerOf2(uiSize));

    m_uiBlockSize = uiSize;
    m_uiBlockWidthInVerts = uiSize + 1;
    m_uiSectorSize = m_uiBlockSize << m_uiNumLOD;
    m_uiSectorWidthInVerts = m_uiSectorSize + 1;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorData::GetNumLOD() const
{
    return m_uiNumLOD;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetNumLOD(NiUInt32 uiNumLOD)
{
    m_uiNumLOD = uiNumLOD;
    m_uiSectorSize = m_uiBlockSize << m_uiNumLOD;
    m_uiSectorWidthInVerts = m_uiSectorSize + 1;

    // Number of blocks
    NiUInt32 uiNumBlocks = 0;
    for (NiUInt32 ui = 0; ui <= m_uiNumLOD; ++ui)
        uiNumBlocks += (1 << ui) * (1 << ui);
    m_kVisibleBlocks.SetSize(uiNumBlocks);

    // Resize the stream arrays:
    m_kStaticVertexStreams.SetSize(GetNumLOD() + 1);
    m_kStaticNormalTangentStreams.SetSize(GetNumLOD() + 1);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorData::GetLODMode() const
{
    return m_uiTerrainLODmode;
}
//---------------------------------------------------------------------------
inline bool NiTerrainSectorData::SetLODMode(NiUInt32 uiLODMode)
{
    if ((uiLODMode & ~LOD_MORPH_ENABLE) < NUM_LOD_MODES)
    {
        m_uiTerrainLODmode = uiLODMode;
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorData::GetNumBlocks() const
{
    NiUInt32 uiSize;
    m_kVisibleBlocks.GetMaxSize(uiSize);
    return uiSize;
}
//---------------------------------------------------------------------------
inline NiInt32 NiTerrainSectorData::GetHighestLoadedLOD() const
{
    return m_iHighestLoadedLOD;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetHighestLoadedLOD(
    NiInt32 iHighestLoadedLOD)
{
    m_iHighestLoadedLOD = iHighestLoadedLOD;
}
//---------------------------------------------------------------------------
inline NiInt32 NiTerrainSectorData::GetTargetLoadedLOD() const
{
    return m_iTargetLoadedLOD;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetTargetLoadedLOD(NiInt32 iTargetLoadedLOD, 
    bool bAllowUnloading)
{
    if (bAllowUnloading || iTargetLoadedLOD > m_iTargetLoadedLOD)
    {
        m_iTargetLoadedLOD = iTargetLoadedLOD;
    }
}
//---------------------------------------------------------------------------
inline float NiTerrainSectorData::GetLODShift() const
{
    return m_fTerrainLODshift;
}
//---------------------------------------------------------------------------
inline float NiTerrainSectorData::GetLODScale() const
{
    return m_fTerrainLODscale;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetLODScale(float fScale, float fShift)
{
    // Minimum scale represents the minimum diameter of the smallest
    // circle used to define the highest detail region.
    static const float fMinimumScale = 2.0f * NiSqrt(2.0);
    NIASSERT(fScale >= fMinimumScale);
    if (fScale < fMinimumScale)
        fScale = fMinimumScale;
    m_fTerrainLODscale = fScale;
    m_fTerrainLODshift = fShift;
}
//---------------------------------------------------------------------------
inline float NiTerrainSectorData::GetWorldScale() const
{
    return m_kWorldTransform.m_fScale;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetWorldScale(float fScale)
{
    m_kWorldTransform.m_fScale = fScale;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiTerrainSectorData::GetWorldTranslate() const
{
    return m_kWorldTransform.m_Translate;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetWorldTranslate(
    const NiPoint3& kWorldTranslate)
{
    m_kWorldTransform.m_Translate = kWorldTranslate;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiTerrainSectorData::GetWorldTransform() const
{
    return m_kWorldTransform;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetWorldTransform(
    const NiTransform& kWorldTransform)
{
    m_kWorldTransform = kWorldTransform;
}
//---------------------------------------------------------------------------
inline NiDataStream* NiTerrainSectorData::GetStaticIndexStream() const
{
    return m_spStaticIndexStream;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetStaticIndexStream(NiDataStream* pkStream)
{
    m_spStaticIndexStream = pkStream;
}
//---------------------------------------------------------------------------
inline NiDataStream::Region* NiTerrainSectorData::GetIndexRegion(NiUInt8 ucPos)
    const
{
    if (ucPos >= m_kIndexRegions.GetSize())
        return 0;

    return m_kIndexRegions[ucPos];
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetIndexRegion(
    NiUInt8 ucPos, NiDataStream::Region* pkIndexRegion)
{
    NIASSERT(ucPos < NUM_INDEX_REGIONS);
    m_kIndexRegions.SetAtGrow(ucPos, pkIndexRegion);
}
//---------------------------------------------------------------------------
inline NiDataStream::Region* NiTerrainSectorData::GetIndexRegionByStitchIndex(
    NiUInt8 ucStitchingIndex) const
{
    NIASSERT(ucStitchingIndex <= 0xF);
    return GetIndexRegion(m_akIndexRegionsMap[ucStitchingIndex]);
}
//---------------------------------------------------------------------------
inline NiDataStream* NiTerrainSectorData::GetStaticPositionStream(
    NiUInt32 uiDetailLevel) const
{
    if (uiDetailLevel < m_kStaticVertexStreams.GetSize())
    {
        return m_kStaticVertexStreams[uiDetailLevel];
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetStaticVertexStream(
    NiUInt32 uiDetailLevel, NiDataStream* pkStream)
{
    m_kStaticVertexStreams.SetAtGrow(uiDetailLevel, pkStream);
}
//---------------------------------------------------------------------------
inline NiDataStream* NiTerrainSectorData::GetStaticNormalTangentStream(
    NiUInt32 uiDetailLevel) const
{
    if (uiDetailLevel < m_kStaticNormalTangentStreams.GetSize())
    {
        return m_kStaticNormalTangentStreams[uiDetailLevel];
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetStaticNormalTangentStream(
    NiUInt32 uiDetailLevel, NiDataStream* pkStream)
{
    m_kStaticNormalTangentStreams.SetAtGrow(uiDetailLevel, pkStream);
}
//---------------------------------------------------------------------------
inline NiDataStream* NiTerrainSectorData::GetStaticUVStream() const
{
    return m_spStaticUVStream;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetStaticUVStream(NiDataStream* pkStream)
{
    m_spStaticUVStream = pkStream;
}
//---------------------------------------------------------------------------
inline NiDynamicStreamCache* NiTerrainSectorData::GetDynamicStreamCache() const
{
    return m_spDynamicStreamCache;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetDynamicStreamCache(
    NiDynamicStreamCache* pkCache)
{
    m_spDynamicStreamCache = pkCache;
}
//---------------------------------------------------------------------------
inline NiCullingProcess* NiTerrainSectorData::GetCullingProcess() const
{
    return m_pkCullingProcess;
}
//---------------------------------------------------------------------------
inline NiCamera* NiTerrainSectorData::GetLODCamera() const
{
    return m_pkLODCamera;
}
//---------------------------------------------------------------------------
inline float NiTerrainSectorData::GetCameraLODSqr() const
{
    return m_fCameraLODSqr;
}
//---------------------------------------------------------------------------
inline const NiFrustumPlanes& NiTerrainSectorData::GetFrustumPlanes() const
{
    return m_kFrustumPlanes;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetCullingProcess(
    NiCullingProcess* pkCuller)
{
    m_pkCullingProcess = pkCuller;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetLODCamera(NiCamera* pkCamera)
{
    m_pkLODCamera = pkCamera;
    if (pkCamera) 
    {
        m_fCameraLODSqr =NiSqr(pkCamera->GetLODAdjust());
        m_kFrustumPlanes.Set(*pkCamera);
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::GetVisibleBlocks(
    const NiTQuickReleasePrimitivePool<const NiTerrainBlock*>*& 
    pkVisibleBlocks) const
{
    pkVisibleBlocks = &m_kVisibleBlocks;
}
//---------------------------------------------------------------------------
inline bool NiTerrainSectorData::GetDeformable() const
{
    return m_bIsDeformable;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetDeformable(bool bDeformable)
{
    m_bIsDeformable = bDeformable;
}
//---------------------------------------------------------------------------
inline NiUInt16* NiTerrainSectorData::GetHeightMap()
{
    return m_pusHeightMap;
}
//---------------------------------------------------------------------------
inline const NiUInt16* NiTerrainSectorData::GetHeightMap() const
{
    return m_pusHeightMap;
}
//---------------------------------------------------------------------------
inline int NiTerrainSectorData::GetMinHeight() const
{
    return m_iMinHeight;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetMinHeight(int iMinHeight)
{
    m_iMinHeight = iMinHeight;
}
//---------------------------------------------------------------------------
inline int NiTerrainSectorData::GetMaxHeight() const
{
    return m_iMaxHeight;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetMaxHeight (int iMaxHeight)
{
    m_iMaxHeight = iMaxHeight;
}
//---------------------------------------------------------------------------
inline float NiTerrainSectorData::GetHeightScale() const
{
    return m_fHeightScaleFactor;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetHeightScale(float fHeightScale)
{
    m_fHeightScaleFactor = fHeightScale;
}
//---------------------------------------------------------------------------
inline float NiTerrainSectorData::GetHeightShift() const
{
    return m_fHeightShift;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetHeightShift(float fHeightShift)
{
    m_fHeightShift = fHeightShift;
}
//---------------------------------------------------------------------------
inline NiInt16 NiTerrainSectorData::GetSectorIndexX()
{
    return m_sSectorIndexX;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetSectorIndexX(NiInt16 uiXIndex)
{
    m_sSectorIndexX = uiXIndex;
}
//---------------------------------------------------------------------------
inline NiInt16 NiTerrainSectorData::GetSectorIndexY()
{
    return m_sSectorIndexY;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetSectorIndexY(NiInt16 uiYIndex)
{
    m_sSectorIndexY = uiYIndex;
}
//---------------------------------------------------------------------------
inline const NiTerrainConfiguration& NiTerrainSectorData::GetConfiguration() 
    const
{
    return m_kTerrainConfiguration;
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorData::SetConfiguration(
    NiTerrainConfiguration kConfiguration)
{
    if (kConfiguration.ValidateConfiguration())
        m_kTerrainConfiguration = kConfiguration;
}
//---------------------------------------------------------------------------