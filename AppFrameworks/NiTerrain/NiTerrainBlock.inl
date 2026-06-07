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
inline NiDataStream::Region& NiTerrainBlock::GetStaticPositionRegion() const
{
    NIASSERT(m_pkSectorData->GetStaticPositionStream(
		GetDataLeaf()->GetLevel()));
    return m_pkSectorData->GetStaticPositionStream(
		GetDataLeaf()->GetLevel())->GetRegion(m_pkDataLeaf->GetRegionID());
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::HasDynamicVertexStreams()
{
    return m_pkCustomPositionStream != 0 && m_pkCustomNormalTangentStream != 0;
}
//---------------------------------------------------------------------------
NiSourceTexture* NiTerrainBlock::GetCombinedDistributionMask()
{
    return m_pkDistributionMask;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::SetCombinedDistributionMask(NiSourceTexture* pkTexture)
{
    NIASSERT(pkTexture);
    m_pkDistributionMask = pkTexture;
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::HasDynamicUVStream()
{
    return m_pkCustomUVStream != 0;
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::HasDynamicIndexStream()
{
    return m_pkCustomIndexStream != 0;
}
//---------------------------------------------------------------------------
inline NiMesh& NiTerrainBlock::GetMesh() const
{
    NIASSERT(m_spMesh);
    return *m_spMesh;
}
//---------------------------------------------------------------------------
inline const NiDataStreamLock* NiTerrainBlock::GetDynamicPositionLockRead() 
    const
{
    return m_pkCustomPositionLock;
}
//---------------------------------------------------------------------------
inline const NiDataStreamLock* NiTerrainBlock::GetDynamicNormalLockRead() 
    const
{
    return m_pkCustomNormalLock;
}
//---------------------------------------------------------------------------
inline const NiDataStreamLock* NiTerrainBlock::GetDynamicTangentLockRead() 
    const
{
    return m_pkCustomTangentLock;
}
//---------------------------------------------------------------------------
inline const NiDataStreamLock* NiTerrainBlock::GetDynamicIndexLockRead() const
{
    return m_pkCustomIndexLock;
}
//---------------------------------------------------------------------------
inline const NiDataStreamLock* NiTerrainBlock::GetDynamicUVLockRead() const
{
    return m_pkCustomUVLock;
}
//---------------------------------------------------------------------------
inline NiDataStreamLock* NiTerrainBlock::GetDynamicPositionLockRead() 
{
    return m_pkCustomPositionLock;
}
//---------------------------------------------------------------------------
inline NiDataStreamLock* NiTerrainBlock::GetDynamicNormalLockRead() 
{
    return m_pkCustomNormalLock;
}
//---------------------------------------------------------------------------
inline NiDataStreamLock* NiTerrainBlock::GetDynamicTangentLockRead() 
{
    return m_pkCustomTangentLock;
}
//---------------------------------------------------------------------------
inline NiDataStreamLock* NiTerrainBlock::GetDynamicIndexLockRead()
{
    return m_pkCustomIndexLock;
}
//---------------------------------------------------------------------------
inline NiDataStreamLock* NiTerrainBlock::GetDynamicUVLockRead() 
{
    return m_pkCustomUVLock;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::SetBlockChildAt(
    NiUInt32 uiPosition, NiTerrainBlock* pkBlock) 
{
    NIASSERT(uiPosition < NUM_CHILDREN);    
    m_apkChildren[uiPosition] = pkBlock;
}
//---------------------------------------------------------------------------
inline NiTerrainBlock* NiTerrainBlock::GetBlockChildAt(
    NiUInt32 uiPosition) 
{
    return m_apkChildren[uiPosition];    
}
//---------------------------------------------------------------------------
inline const NiTerrainBlock* NiTerrainBlock::GetBlockChildAt(
    NiUInt32 uiPosition) const 
{
    return m_apkChildren[uiPosition];    
}
//---------------------------------------------------------------------------
inline NiTerrainBlock* NiTerrainBlock::GetBlockParent() 
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
inline const NiTerrainBlock* NiTerrainBlock::GetBlockParent() const
{
    return m_pkParent;
}
//---------------------------------------------------------------------------
inline const NiTerrainDataLeaf* NiTerrainBlock::GetDataLeaf() const
{
    return m_pkDataLeaf;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::AddToVisible() 
{
    m_pkSectorData->AddToVisible(this);
    m_pkDrawnPool->CombineValue(m_uiDrawnPoolIndex, NiTerrainBlock::SELF);

    // Update adjusted eye position
    NiFloatsExtraData* pkExtraData = (NiFloatsExtraData*)(
        m_spMesh->GetExtraData("g_AdjustedEyePos"));
    NIASSERT(pkExtraData);

    const NiPoint3& kCameraLoc = 
        m_pkSectorData->GetLODCamera()->GetWorldLocation();

    pkExtraData->SetValue(0, kCameraLoc.x);
    pkExtraData->SetValue(1, kCameraLoc.y);
    pkExtraData->SetValue(2, kCameraLoc.z);
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::ResetLOD()
{
    m_pkDrawnPool->Reset();
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::CulledByLOD() const
{
    return m_pkDrawnPool->GetValue(m_uiDrawnPoolIndex) == 0;
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::RequiresUpdate() const
{
    return m_bRequiresUpdate;
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::RequiresLightingRebuild() const
{
    return m_ucLightingStatus != LIGHTING_BUILT;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::RequestLightingRebuild()
{
    m_ucLightingStatus = LIGHTING_OUTDATED;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::MarkLightingBuilt()
{
    m_ucLightingStatus = LIGHTING_BUILT;
    if (!RequiresUpdate())
        RequestUpdate();
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::IsLightingInProgress()
{
    return m_ucLightingStatus == LIGHTING_IN_PROGRESS;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::MarkLightingInProgress()
{
    m_ucLightingStatus = LIGHTING_IN_PROGRESS;
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::RequiresMorphRebuild() const
{
    return m_bMorphOutdated;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::RequestMorphRebuild()
{
    m_bMorphOutdated = true;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::MarkMorphBuilt()
{
    m_bMorphOutdated = false;
    if (!RequiresUpdate())
        RequestUpdate();
}
//---------------------------------------------------------------------------
inline NiUInt8 NiTerrainBlock::GetStitchingIndex() const
{
    return m_ucStitchingIndex;
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::UpdateMorphConstants()
{
    static const float fSquareRoot2 = NiSqrt(2.0f);

    m_fMaxDistanceSqr = NiMax(0.0f,
        (float)(m_pkSectorData->GetBlockSize()) * 
        NiPow(2.0f, (float)m_pkDataLeaf->GetNumSubDivisions()) * 
        m_pkSectorData->GetLODScale() * fSquareRoot2 + 
        m_pkSectorData->GetLODShift());
    m_fMaxDistanceSqr = NiSqr(m_fMaxDistanceSqr);

    // Reset the morphing data in the shader. This is in theory not required
    // any more, since we are using a transformed camera in the shader now.
    NiUInt32 uiLodLevel = m_pkDataLeaf->GetNumSubDivisions();
    float fBlockWorldWidth = float(m_pkSectorData->GetBlockSize() << uiLodLevel);
    float fMorphDistance = fSquareRoot2 * (fBlockWorldWidth * 0.5f);
    float fThresholdDistance = NiSqrt(m_fMaxDistanceSqr) - fMorphDistance;

    NiFloatExtraData* pkTHDistanceData = 
        (NiFloatExtraData*)m_spMesh->GetExtraData(
        NiTerrainMaterial::LODTHRESHOLD_SHADER_CONSTANT);
    NiFloatExtraData* pkMorphDistanceData = 
        (NiFloatExtraData*)m_spMesh->GetExtraData(
        NiTerrainMaterial::LODMORPHDISTANCE_SHADER_CONSTANT);
    NiIntegerExtraData* pkMorphModeData = 
        (NiIntegerExtraData*)m_spMesh->GetExtraData(
        NiTerrainMaterial::MORPHMODE_SHADER_CONSTANT);

    NIASSERT(pkTHDistanceData);
    NIASSERT(pkMorphDistanceData);
    NIASSERT(pkMorphModeData);
    
    if (pkTHDistanceData && pkMorphDistanceData && pkMorphModeData)
    {
        pkTHDistanceData->SetValue(fThresholdDistance);
        pkMorphDistanceData->SetValue(fMorphDistance);
        pkMorphModeData->SetValue(m_pkSectorData->GetLODMode());
    }    
}
//---------------------------------------------------------------------------
inline void NiTerrainBlock::SetWorldScale(float fScale)
{
    NiNode::SetWorldScale(fScale);
    UpdateMorphConstants();
}
//---------------------------------------------------------------------------
inline bool NiTerrainBlock::IsInRange() 
{
    const NiPoint3& kCameraLocation = 
        m_pkSectorData->GetLODCamera()->GetWorldLocation();

    const NiBound& kBound = m_pkDataLeaf->GetLocalBound();
    float fDist = 0;

    switch(m_pkSectorData->GetLODMode() 
        & ~NiTerrainSectorData::LOD_MORPH_ENABLE)
    {   
        case NiTerrainSectorData::LOD_MODE_3D:
        {
            fDist = (kCameraLocation - kBound.GetCenter()).Length();
        }break;
        case NiTerrainSectorData::LOD_MODE_25D: 
            {
                if (kCameraLocation.z * kCameraLocation.z 
                    > m_fMaxDistanceSqr)
                    return false;
            }
        case NiTerrainSectorData::LOD_MODE_2D:
        default:    
        {
            fDist = NiSqrt(NiSqr(kCameraLocation.x - kBound.GetCenter().x) +
                NiSqr(kCameraLocation.y - kBound.GetCenter().y));
        }
    }
    
    static const float fSquareRoot2 = NiSqrt(2.0f);
    NiUInt32 uiLodLevel = m_pkDataLeaf->GetNumSubDivisions();
    float fBlockWidth = float(m_pkSectorData->GetBlockSize() << uiLodLevel);

    fDist -= fSquareRoot2 * (fBlockWidth * 0.5f);

    return (m_fMaxDistanceSqr < 0) || ((fDist * fDist) < m_fMaxDistanceSqr);
}
