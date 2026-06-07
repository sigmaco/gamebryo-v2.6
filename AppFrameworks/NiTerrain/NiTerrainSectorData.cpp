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

#include "NiTerrainPCH.h"

#include "NiTerrainSectorData.h"
#include "NiTerrainBlock.h"
#include "NiTerrainInteractor.h"

//---------------------------------------------------------------------------
NiTerrainSectorData::NiTerrainSectorData() :
    m_uiBlockSize(0),
    m_uiBlockWidthInVerts(0),
    m_uiSectorSize(0),
    m_uiSectorWidthInVerts(0),
    m_bIsDeformable(false),
    m_uiNumLOD(0),
    m_sSectorIndexX(0),
    m_sSectorIndexY(0),
    m_iHighestLoadedLOD(-1),
    m_iTargetLoadedLOD(0),
    m_spStaticIndexStream(0),
    m_spStaticUVStream(0),
    m_kStaticVertexStreams(0),
    m_kStaticNormalTangentStreams(0),
    m_spDynamicStreamCache(0),
    m_pkCullingProcess(0),
    m_pkLODCamera(0),
    m_fCameraLODSqr(0.0f),
    m_fTerrainLODscale(2.8f),
    m_fTerrainLODshift(0.0f),
    m_uiTerrainLODmode(LOD_MODE_2D | LOD_MORPH_ENABLE),
    m_spEnvMap(0),
    m_spDefaultEnvMap(0),
    m_pusHeightMap(NULL)
{
    // Ensure the array of index regions is initialized
    m_kIndexRegions.SetSize(NUM_INDEX_REGIONS);
    
    // Initialize the region map - maps border combinations to regions
    for (NiUInt8 uc = 0; uc < 16; ++uc)
        m_akIndexRegionsMap[uc] = 0;    

    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_NONE] = 0;

    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_BOTTOM] = 1;
    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_RIGHT] = 2;
    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_TOP] = 3;
    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_LEFT] = 4;

    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_BOTTOM | 
        NiTerrainDataLeaf::BORDER_RIGHT] = 5;
    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_RIGHT | 
        NiTerrainDataLeaf::BORDER_TOP] = 6;
    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_TOP | 
        NiTerrainDataLeaf::BORDER_LEFT] = 7;
    m_akIndexRegionsMap[NiTerrainDataLeaf::BORDER_LEFT | 
        NiTerrainDataLeaf::BORDER_BOTTOM] = 8;
}
//---------------------------------------------------------------------------
NiTerrainSectorData::~NiTerrainSectorData()
{ 
    if (m_pusHeightMap)
        NiFree(m_pusHeightMap);
}
//---------------------------------------------------------------------------
void NiTerrainSectorData::AddToVisible(const NiTerrainBlock* pkBlock) const
{
    NIASSERT(pkBlock);
    m_pkCullingProcess->GetVisibleSet()->Add(
        pkBlock->GetMesh());

    m_kVisibleBlocks.SetValue(m_kVisibleBlocks.GetNew(), pkBlock);
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainSectorData::ResetVisible()
{
    NiUInt32 uiOldVis;
    m_kVisibleBlocks.GetSize(uiOldVis);
    m_kVisibleBlocks.Reset();

    return uiOldVis;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainSectorData::GetNumVisible()
{
    NiUInt32 uiSize;
    m_kVisibleBlocks.GetSize(uiSize);
    return uiSize;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorData::InToolMode()
{
    return NiTerrainInteractor::InToolMode();
}
//---------------------------------------------------------------------------
void NiTerrainSectorData::AllocateHeightMap()
{
    if (m_pusHeightMap)
    {
        NiFree(m_pusHeightMap);
    }
    m_pusHeightMap = NiAlloc (NiUInt16,
        m_uiSectorWidthInVerts * m_uiSectorWidthInVerts);
}
