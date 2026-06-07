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

#include "NiTerrainDataLeaf.h"
#include "NiMetaData.h"
#include "NiSurfaceMask.h"
#include "NiTerrainBlock.h"
#include "NiTerrainSector.h"

#include "NiTTerrainRandomAccessIterator.h"

//---------------------------------------------------------------------------
NiTerrainDataLeaf::NiTerrainDataLeaf(NiUInt32 uiLevel,
    NiMetaDataStore* pkMetaDataStore) :
    m_uiLevel(uiLevel),
    m_uiRegionID(0),
    m_uiBlockID(0),
    m_pkQuadMesh(NULL),    
    m_pkSectorData(NULL),
    m_pkParent(NULL),
    m_kMetaData(pkMetaDataStore),
    m_uiNumUsedSurfaces(0),
    m_bUseCustomSurfacePriority(false)
{
    // Initialize the children array
    m_apkChildren[0] = NULL;
    m_apkChildren[1] = NULL;
    m_apkChildren[2] = NULL;
    m_apkChildren[3] = NULL;

    // initialize the surface alphas array
    for (NiUInt32 ui = 0; ui < 8; ++ui)
    {
        m_pkSurfaces[ui] = NULL;
        m_pkSurfaceMasks[ui] = NULL;
        m_auiSurfacePriority[ui] = 0;
    }
}
//---------------------------------------------------------------------------
NiTerrainDataLeaf::~NiTerrainDataLeaf() 
{
    // We do not want to free ANY memory, since we are just an index. All our 
    // data is assigned from outside.
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::SetContainingSector(
    NiTerrainSector* pkContainingSector)
{
    m_pkContainingSector = pkContainingSector;
    m_pkSectorData = pkContainingSector->GetSectorData();
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::GetVertexAt(
    NiTerrainVertex& kVertex, const NiIndex& kLocal) const 
{
    GetVertexAt(kVertex.m_kWorldLocation, kLocal);
    kVertex.SetHeight(kVertex.m_kWorldLocation.z);

    // multiply by transform to get world position - AFTER setting vertex
    // height
    kVertex.m_kWorldLocation = m_pkSectorData->GetWorldTransform() *
        kVertex.m_kWorldLocation;

    // Index
    kVertex.SetIndex(NiIndex(
            m_kDataBottomLeftIndex.x + (kLocal.x << GetNumSubDivisions()),
            m_kDataBottomLeftIndex.y + (kLocal.y << GetNumSubDivisions()))
        );

    // First containing
    NiUInt32 uiShallowestLevel = 0;
    NiUInt32 uiDivisor = 1 << m_pkSectorData->GetNumLOD();

    const NiIndex& kIndex = kVertex.GetIndex();

    // Find the shallowest level that this vertex appears within at least one
    // child
    for (; uiDivisor > 0; uiDivisor /= 2)
    {
        if (kIndex.x % uiDivisor == 0 || kIndex.y % uiDivisor == 0)
            break;
        uiShallowestLevel++;
    }

    // This really is a sanity check
    NIASSERT(GetLevel() >= uiShallowestLevel);

    kVertex.SetFirstContainingLeaf(GetContainingSector()->GetLeafContaining(
            kVertex.GetIndex(), uiShallowestLevel));

    // Parent leaf
    if (kIndex.x == GetBlockSize() || kIndex.y == GetBlockSize())
    {
        // The parent leaf is actually adjacent
        kVertex.SetParentLeaf(GetContainingSector()->GetLeafContaining(
                kVertex.GetIndex(), GetLevel()));
    }
    else
    {
        kVertex.SetParentLeaf(GetContainingSector()->GetLeaf(GetBlockID()));
    }
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::GetNormalAt(
    NiPoint3& kNormal, const NiIndex& kLocal) const
{
    NIASSERT(kLocal.x <= GetWidthInVerts());
    NIASSERT(kLocal.y <= GetWidthInVerts());
    NIASSERT(m_pkQuadMesh && !m_pkQuadMesh->IsLightingInProgress());

    m_kNormalIterator.GetHighDetail(kLocal.x + kLocal.y * GetWidthInVerts(),
        kNormal);
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainDataLeaf::GetVertexBorders( 
    const NiTerrainVertex& kVertex) const
{
    NiUInt32 uiTouchingBorders = 0;
    NiUInt32 uiSize = GetBlockSize();
    
    NiIndex kIndexMin = SectorIndexFromLocal(NiIndex::ZERO);
    NiIndex kIndexMax = SectorIndexFromLocal(
        NiIndex(uiSize, uiSize));

    // Are we inside the correct range?
    if (kVertex.GetIndex().y < kIndexMin.y || 
        kVertex.GetIndex().y > kIndexMax.y ||
        kVertex.GetIndex().x < kIndexMin.x || 
        kVertex.GetIndex().x > kIndexMax.x)
    {
        return false;
    }

    /* 
     * Deal with Y
     */
    // Left, right edge
    if (kVertex.GetIndex().x == kIndexMin.x)
        uiTouchingBorders |= BORDER_LEFT;
    else if (kVertex.GetIndex().x == kIndexMax.x) 
        uiTouchingBorders |= BORDER_RIGHT;
    
    /* 
     * Deal with X
     */
    // Bottom, top edge
    if (kVertex.GetIndex().y == kIndexMin.y)
        uiTouchingBorders |= BORDER_BOTTOM;
    else if (kVertex.GetIndex().y == kIndexMax.y) 
        uiTouchingBorders |= BORDER_TOP;

    // We were inside our data set... usTouchingBorders has the bit code
    // to indicate which edge, if any, we were touching.
    return uiTouchingBorders;
}
//---------------------------------------------------------------------------
NiTerrainDataLeaf* NiTerrainDataLeaf::DoGetAdjacent(NiUInt32 uiBorder) const
{
    NiIndex kIndex;
    GetBottomLeftIndex(kIndex);
    NiUInt32 uiIncrement = m_pkSectorData->GetBlockSize() << GetNumSubDivisions();
    NiUInt32 uiMaxIndex = m_pkSectorData->GetSectorSize() - uiIncrement;
    NiUInt32 uiWidthInBlocks = 1 << GetLevel();
    
    const NiTerrainSector *pkSector = GetContainingSector();
    
    NiUInt32 uiSectorBorder = 0;
    if (uiBorder & BORDER_LEFT && kIndex.x == 0)
    {
        uiSectorBorder |= BORDER_LEFT;
        uiBorder ^= BORDER_LEFT;
        kIndex.x += uiMaxIndex;
    }
    if (uiBorder & BORDER_RIGHT && kIndex.x >= uiMaxIndex)
    {
        uiSectorBorder |= BORDER_RIGHT;
        uiBorder ^= BORDER_RIGHT;
        kIndex.x -= uiMaxIndex;
    }
    if (uiBorder & BORDER_TOP && kIndex.y >= uiMaxIndex)
    {
        uiSectorBorder |= BORDER_TOP;
        uiBorder ^= BORDER_TOP;
        kIndex.y -= uiMaxIndex;
    }
    if (uiBorder & BORDER_BOTTOM && kIndex.y == 0)
    {
        uiSectorBorder |= BORDER_BOTTOM;
        uiBorder ^= BORDER_BOTTOM;
        kIndex.y += uiMaxIndex;
    }

    if (uiSectorBorder)
    {
        pkSector = pkSector->GetAdjacentSector(uiSectorBorder);
        if (pkSector == 0)
        {
            return 0;
        }
    }    
    
    kIndex /= uiIncrement;
    if (uiBorder & BORDER_LEFT)
        kIndex.x--;
    else if (uiBorder & BORDER_RIGHT)
        kIndex.x++;   
    if (uiBorder & BORDER_TOP)
        kIndex.y++;
    else if (uiBorder & BORDER_BOTTOM)
        kIndex.y--;

    NiUInt32 uiBlockID = pkSector->GetLeafOffset(GetLevel()) +
        kIndex.x + kIndex.y * uiWidthInBlocks;

    return pkSector->GetLeaf(uiBlockID);
}
//---------------------------------------------------------------------------
NiTerrainDataLeaf* NiTerrainDataLeaf::DoGetAdjacentFast(NiUInt32 uiBorder) 
    const
{
    NiIndex kIndex;
    GetBottomLeftIndex(kIndex);
    NiUInt32 uiIncrement = m_pkSectorData->GetBlockSize() << GetNumSubDivisions();
    NiUInt32 uiMaxIndex = m_pkSectorData->GetSectorSize() - uiIncrement;
    NiUInt32 usWidthInBlocks = 1 << GetLevel();

    const NiTerrainSector *pkSector = GetContainingSector();

    NiUInt32 uiSectorBorder = 0;
    if (uiBorder & BORDER_LEFT && kIndex.x == 0)
    {
        uiSectorBorder |= BORDER_LEFT;
        uiBorder ^= BORDER_LEFT;
        kIndex.x += uiMaxIndex;
    }
    if (uiBorder & BORDER_RIGHT && kIndex.x >= uiMaxIndex)
    {
        uiSectorBorder |= BORDER_RIGHT;
        uiBorder ^= BORDER_RIGHT;
        kIndex.x -= uiMaxIndex;
    }
    if (uiBorder & BORDER_TOP && kIndex.y >= uiMaxIndex)
    {
        uiSectorBorder |= BORDER_TOP;
        uiBorder ^= BORDER_TOP;
        kIndex.y -= uiMaxIndex;
    }
    if (uiBorder & BORDER_BOTTOM && kIndex.y == 0)
    {
        uiSectorBorder |= BORDER_BOTTOM;
        uiBorder ^= BORDER_BOTTOM;
        kIndex.y += uiMaxIndex;
    }

    if (uiSectorBorder)
    {
        pkSector = pkSector->GetAdjacentSector(uiSectorBorder);
        if (pkSector == 0)
        {
            return 0;
        }
    }

    kIndex /= uiIncrement;
    if (uiBorder & BORDER_LEFT)
        kIndex.x--;
    else if (uiBorder & BORDER_RIGHT)
        kIndex.x++;   
    if (uiBorder & BORDER_TOP)
        kIndex.y++;
    else if (uiBorder & BORDER_BOTTOM)
        kIndex.y--;

    NiUInt32 uiBlockID = pkSector->GetLeafOffset(GetLevel()) +
        kIndex.x + kIndex.y * usWidthInBlocks;

    return pkSector->GetLeaf(uiBlockID);
}
//---------------------------------------------------------------------------
bool NiTerrainDataLeaf::IndexToLocal(
    const NiIndex& kSector, NiIndex& kLocal) const
{
    NiUInt32 uiSize = GetBlockSize();
    
    const NiIndex& kIndexMin = m_kDataBottomLeftIndex;

    // Are we inside the correct range?
    if (kSector.x < kIndexMin.x ||
        kSector.y < kIndexMin.y)
    {
        return false;
    }

    NiIndex kIndexMax(
        kIndexMin.x + (uiSize << GetNumSubDivisions()),
        kIndexMin.y + (uiSize << GetNumSubDivisions()));
    if (kSector.x > kIndexMax.x || 
        kSector.y > kIndexMax.y)
    {
        return false;
    }

    // Spacing is always a power of two.
    NiUInt32 uiSpacing = 1 << GetNumSubDivisions();

    // Bitwise OR operation. If spacing is 32, and vertex is a multiple of
    // 32... then vertex & (31) == 0
    if ((kSector.x & (uiSpacing - 1)) != 0 || 
        (kSector.y & (uiSpacing - 1)) != 0)
    {
        return false;
    }
    
    // Ok, we are a valid index... but what IS our index?
    IndexToLocalFast(kSector, kLocal);

    return true;
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::CreateIterators(
    NiDataStreamLock* pkPositionLock,
    NiDataStreamLock* pkNormalLock, 
    NiDataStreamLock* pkTangentLock,
    NiDataStreamLock* pkUVLock,
    NiDataStreamLock* pkIndexLock)
{
    NiTerrainBlock* pkBlock = GetQuadMesh();

    // Position, normal, tangent stream
    if (pkBlock && pkBlock->HasDynamicVertexStreams())
    {
        // Positions
        m_kPositionIterator = NiTerrainPositionRandomAccessIterator(
            pkBlock->GetDynamicPositionLockRead(), 0, 
            GetContainingSector()->GetConfiguration());

        // Normals and tangents
        m_kNormalIterator = NiTerrainNormalRandomAccessIterator(
            pkBlock->GetDynamicNormalLockRead(), 0,
            GetContainingSector()->GetConfiguration());
        
        if (GetContainingSector()->GetConfiguration().IsTangentDataEnabled())
        {
            m_kTangentIterator = NiTerrainTangentRandomAccessIterator(
                pkBlock->GetDynamicTangentLockRead(), 0,
                GetContainingSector()->GetConfiguration());
        }
    }
    else
    {
        if (pkPositionLock)
        {
            // Positions
            m_kPositionIterator = NiTerrainPositionRandomAccessIterator(
                pkPositionLock, GetRegionID(),
                GetContainingSector()->GetConfiguration()); 
        }

        if (pkNormalLock)            
        {
            // Normals and tangents
            m_kNormalIterator = NiTerrainNormalRandomAccessIterator(
                pkNormalLock, GetRegionID(),
                GetContainingSector()->GetConfiguration());
        }

        if (pkTangentLock && GetContainingSector()->GetConfiguration(
            ).IsTangentDataEnabled())
        {
            m_kTangentIterator = NiTerrainTangentRandomAccessIterator(
                pkTangentLock, GetRegionID(),
                GetContainingSector()->GetConfiguration());
        }
    }

    // Index stream
    if (pkBlock && pkBlock->HasDynamicIndexStream())
    {
        const NiDataStreamLock* pkDynamicIndexLock = 
            pkBlock->GetDynamicIndexLockRead();

        // Only one of these are ever 'valid'
        m_kIndexIteratorNiUInt16 = 
            pkDynamicIndexLock->begin_region<NiUInt16>(0);
        m_kIndexIteratorNiUInt32 = 
            pkDynamicIndexLock->begin_region<NiUInt32>(0);
    }
    else if (pkIndexLock)
    {
        // Only one of these are ever 'valid'
        m_kIndexIteratorNiUInt16 = pkIndexLock->begin_region<NiUInt16>(0);
        m_kIndexIteratorNiUInt32 = pkIndexLock->begin_region<NiUInt32>(0);
    }

    // UV Stream
    if (pkBlock && pkBlock->HasDynamicUVStream())
    {
        m_kUVIterator = pkBlock->GetDynamicUVLockRead()->begin<NiPoint2>();
    }
    else if (pkUVLock)
    {
        m_kUVIterator = pkUVLock->begin<NiPoint2>();
    }
}
//---------------------------------------------------------------------------
bool NiTerrainDataLeaf::AddSurfaceMask(const NiSurface* pkSurface, 
    const NiSurfaceMask* pkSurfaceMask, NiUInt32 uiNewPriority)
{
    if (m_uiNumUsedSurfaces == ms_uiMaxNumSurfaces)
        return false;

    // Make sure that the surface mask is of the correct size
    if (m_uiNumUsedSurfaces) 
    {
        if (m_pkSurfaceMasks[0]->GetWidth() != pkSurfaceMask->GetWidth())
            return false;
    }

    // Add the surface mask
    if (m_uiNumUsedSurfaces > 0)
    {
        for (NiUInt32 ui = m_uiNumUsedSurfaces; ui > uiNewPriority; --ui) 
        {
            m_auiSurfacePriority[ui] = m_auiSurfacePriority[ui - 1];

            if (ui == 0)
                break;
        }
    }

    m_pkSurfaces[m_uiNumUsedSurfaces] = pkSurface;
    m_pkSurfaceMasks[m_uiNumUsedSurfaces] = pkSurfaceMask;
    m_auiSurfacePriority[uiNewPriority] = m_uiNumUsedSurfaces;

    ++m_uiNumUsedSurfaces;

    m_pkContainingSector->UpdateBestPixelSpacing(this);

    if (m_pkQuadMesh)
        m_pkQuadMesh->MarkSurfaceMasksChanged(true);

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainDataLeaf::SetSurfaceMask(const NiSurface* pkSurface, 
    const NiSurfaceMask* pkSurfaceMask)
{
    // Make sure that the surface mask is of the correct size
    if (m_uiNumUsedSurfaces) 
    {
        if (m_pkSurfaceMasks[0]->GetWidth() != pkSurfaceMask->GetWidth())
        {
            return false;
        }
    }

    for (NiUInt32 ui = 0; ui < m_uiNumUsedSurfaces; ++ui) 
    {
        if (m_pkSurfaces[ui] == pkSurface) 
        {
            m_pkSurfaceMasks[ui] = pkSurfaceMask;

            m_pkContainingSector->UpdateBestPixelSpacing(this);
            
            if (m_pkQuadMesh)
                m_pkQuadMesh->MarkSurfaceMasksChanged(true);

            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
const NiSurfaceMask* NiTerrainDataLeaf::GetSurfaceMask(
    const NiSurface* pkSurface)
    const
{
    for (NiUInt32 ui = 0; ui < m_uiNumUsedSurfaces; ++ui)
    {
        if (m_pkSurfaces[ui] == pkSurface)
            return m_pkSurfaceMasks[ui];
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::UpdateSurfaceMaskAddresses(NiUInt8* pusOldAddress,
    NiUInt8* pusNewAddress)
{
    for (NiUInt32 ui = 0; ui < m_uiNumUsedSurfaces; ++ui)
    {
        m_pkSurfaceMasks[ui] = (NiSurfaceMask*)((
            (NiUInt8*)(m_pkSurfaceMasks[ui]) - pusOldAddress) + pusNewAddress);
    }
}
//---------------------------------------------------------------------------
const NiSurfaceMask* NiTerrainDataLeaf::GetSurfaceMask(NiUInt32 uiPriority) 
    const
{
    if (uiPriority >= GetSurfaceCount())
        return 0;

    return m_pkSurfaceMasks[m_auiSurfacePriority[uiPriority]];
    
}
//---------------------------------------------------------------------------
NiUInt8 NiTerrainDataLeaf::GetAverageAlpha(
    const NiTerrainVertex& kCurVertex, const NiSurface* pkSurface) const
{
    NiUInt32 uiTotalAlpha = 0;
    NiUInt8 ucNumFound = 0;
    NiUInt8 ucAlpha = 0;

    // Check our children?
    if (GetChildAt(0))
    {
        for (NiUInt8 uc = 0; uc < 4; ++uc)
        {
            // Does this child even contain the given vertex?
            if (GetChildAt(uc)->GetSurfaceCount() == 0 ||
                !GetChildAt(uc)->TreeContains(kCurVertex)
                )
            {
                continue;
            }

            ucAlpha = GetChildAt(uc)->GetAverageAlpha(kCurVertex, pkSurface);

            uiTotalAlpha = (NiUInt32)(uiTotalAlpha + ucAlpha);
            ++ucNumFound;
        }
    }

    const NiSurfaceMask* pkMask = GetSurfaceMask(pkSurface);
    if (pkMask)
    {
        NiIndex kLocal;

        if (IndexToLocal(kCurVertex.GetIndex(), kLocal))
        {
            float fPixelX = (float(kLocal.x) / float(GetBlockSize())) * 
                (pkMask->GetWidth() - 1);
            float fPixelY = (float(kLocal.y) / float(GetBlockSize())) * 
                (pkMask->GetWidth() - 1);

            ucAlpha = pkMask->GetAt(
                (NiUInt32)fPixelX, (NiUInt32)fPixelY);

            uiTotalAlpha = (NiUInt32)(uiTotalAlpha + ucAlpha);
            ++ucNumFound;
        }
    }

    if (ucNumFound)
        return (NiUInt8)(uiTotalAlpha / ucNumFound);
    else
        return 0;
}
//---------------------------------------------------------------------------
const NiUInt32 NiTerrainDataLeaf::GetSurfaceMaskWidth() const
{
    
    // Make sure that the surface mask is of the correct size
    if (m_uiNumUsedSurfaces) 
        return m_pkSurfaceMasks[0]->GetWidth();

    return  0;
}
//---------------------------------------------------------------------------
bool NiTerrainDataLeaf::RemoveSurfaceMask(const NiSurface* pkSurface, 
        bool bRecalculateBestPixelSpacing)
{
    if (m_uiNumUsedSurfaces == 0)
        return false;

    // Find the entry in the priority list
    NiUInt32 uiPriority;
    bool bFound = false;
    for (uiPriority = 0; uiPriority < m_uiNumUsedSurfaces; ++uiPriority)
    {
        if (m_pkSurfaces[m_auiSurfacePriority[uiPriority]] == pkSurface) 
        {
            bFound = true;
            break;
        }
    }

    if (bFound)
    {
        NiUInt32 uiSurfaceIndex;

        // remove the mask reference, and move the remaining references up one
        NiUInt32 uiSurfaceID = m_auiSurfacePriority[uiPriority];
        for (uiSurfaceIndex = uiSurfaceID + 1; 
            uiSurfaceIndex < m_uiNumUsedSurfaces; ++uiSurfaceIndex) 
        {            
            m_pkSurfaces[uiSurfaceIndex - 1] = m_pkSurfaces[uiSurfaceIndex];
            m_pkSurfaceMasks[uiSurfaceIndex - 1] = 
                m_pkSurfaceMasks[uiSurfaceIndex] - 1;
        }

        m_pkSurfaces[m_uiNumUsedSurfaces - 1] = 0;
        m_pkSurfaceMasks[m_uiNumUsedSurfaces - 1] = 0;
            
        --m_uiNumUsedSurfaces;

        // remove from the priority queue
        for (uiSurfaceIndex = uiPriority; uiSurfaceIndex < m_uiNumUsedSurfaces;
            ++uiSurfaceIndex)
        {
            NIASSERT(uiSurfaceIndex < ms_uiMaxNumSurfaces - 1);
            m_auiSurfacePriority[uiSurfaceIndex] = 
                m_auiSurfacePriority[uiSurfaceIndex + 1];
        }

        m_auiSurfacePriority[m_uiNumUsedSurfaces] = 0;

        // adjust the values of the surface ID's to reflect the removal
        for (uiSurfaceIndex = 0; uiSurfaceIndex < m_uiNumUsedSurfaces; 
            ++uiSurfaceIndex)
        {
            NIASSERT(m_auiSurfacePriority[uiSurfaceIndex] != uiSurfaceID);
            if (m_auiSurfacePriority[uiSurfaceIndex] > uiSurfaceID)
                m_auiSurfacePriority[uiSurfaceIndex]--;
        }

        // Optionally recalculate best pixel spacing at this level. Added as an
        // option to allow for efficient bulk surface removal from a terrain
        if (bRecalculateBestPixelSpacing)
            GetContainingSector()->RecalculateBestPixelSpacing(uiSurfaceIndex);
    
        if (m_pkQuadMesh)
            m_pkQuadMesh->MarkSurfaceMasksChanged(true);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
const NiSurface* NiTerrainDataLeaf::GetSurface(NiUInt32 uiPriority) const
{
    if (uiPriority >= GetSurfaceCount())
        return 0;

    return m_pkSurfaces[m_auiSurfacePriority[uiPriority]];
}
//---------------------------------------------------------------------------
bool NiTerrainDataLeaf::SetSurfacePriority(const NiSurface* pkSurface, 
    NiUInt32 uiNewPriority)
{
    // Find the current priority
    NiUInt32 uiPriority;
    NiUInt32 uiIndex = 0;
    bool bFound = false;
    for (uiPriority = 0; uiPriority < m_uiNumUsedSurfaces; ++uiPriority) 
    {
        if (m_pkSurfaces[m_auiSurfacePriority[uiPriority]] == pkSurface) 
        {
            bFound = true;
            uiIndex = m_auiSurfacePriority[uiPriority];
            break;
        }
    }

    if (!bFound)
        return false;

    if (uiNewPriority == uiPriority) 
    {
        return true;
    }
    else if (uiNewPriority < uiPriority) 
    {
        for (NiInt32 i = uiPriority; i > (NiInt32)uiNewPriority; --i)
            m_auiSurfacePriority[i] = m_auiSurfacePriority[i - 1];

        m_auiSurfacePriority[uiNewPriority] = uiIndex;
    } 
    else 
    {
        for (NiUInt32 ui = uiPriority; ui < uiNewPriority; ++ui)
            m_auiSurfacePriority[ui] = m_auiSurfacePriority[ui + 1];

        m_auiSurfacePriority[uiNewPriority] = uiIndex;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainDataLeaf::GetSurfacePriority(const NiSurface* pkSurface, 
    NiUInt32& uiPriority) const
{
    for (uiPriority = 0; uiPriority < m_uiNumUsedSurfaces; ++uiPriority)
    {
        if (m_pkSurfaces[m_auiSurfacePriority[uiPriority]] == pkSurface)
            return true;
    }

    uiPriority = m_uiNumUsedSurfaces - 1;
    return false;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainDataLeaf::GetSurfaceCount() const
{
    return m_uiNumUsedSurfaces;
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::Update()
{
    // We now need to find the bounding radius
    float fMaxDistSqr = 0.0f;
    float fDistSqr = 0.0f;

    NiPoint3 kPoint;
    NiPoint3 kCenter = 
        m_kSumOfVertexLocations / NiSqr((float)GetWidthInVerts());

    if (kCenter != m_kBound.GetCenter())
    {
        for (NiUInt32 y = 0; y < GetWidthInVerts(); ++y) 
        {
            for (NiUInt32 x = 0; x < GetWidthInVerts(); ++x)
            {   
                GetVertexAt(kPoint, NiIndex(x, y));
                fDistSqr = NiSqr(kPoint.x - kCenter.x) +
                    NiSqr(kPoint.y - kCenter.y) +
                    NiSqr(kPoint.z - kCenter.z);
                    
                if (fDistSqr > fMaxDistSqr)
                    fMaxDistSqr = fDistSqr;
            }
        }

        // Part 2 of 2 of box bounds update
        m_kBound.SetCenterAndRadius(kCenter, NiSqrt(fMaxDistSqr));

        // Allow a little margin for floating point error (0.51)
        m_kBoxBound.SetCenter(kCenter);
        NiPoint3 kTopRight;
        NiPoint3 kBottomLeft;

        GetVertexAt(kTopRight, NiIndex(GetBlockSize(), GetBlockSize()));
        GetVertexAt(kBottomLeft, NiIndex::ZERO);

        // Slight overlap to allow for floating point errors
        float fExtent = (kTopRight.x - kBottomLeft.x) * 0.51f;

        m_kBoxBound.SetExtent(0, fExtent); // x
        m_kBoxBound.SetExtent(1, fExtent); // y
        m_kBoxBound.SetExtent(2, NiSqrt(fMaxDistSqr)); // z        
    }

    // Since we are a balanced tree, if one of our children exists they all do.
    if (GetChildAt(0)) 
    {
        // Update and Merge any children's bounds
        NiBound kChildBound;
        if (GetChildAt(0)->RequiresUpdate())
            GetChildAt(0)->Update();
        kChildBound = GetChildAt(0)->GetLocalBound();
        m_kBound.Merge(&kChildBound);

        if (GetChildAt(1)->RequiresUpdate())
            GetChildAt(1)->Update();
        kChildBound = GetChildAt(1)->GetLocalBound();
        m_kBound.Merge(&kChildBound);

        if (GetChildAt(2)->RequiresUpdate())
            GetChildAt(2)->Update();
        kChildBound = GetChildAt(2)->GetLocalBound();
        m_kBound.Merge(&kChildBound);

        if (GetChildAt(3)->RequiresUpdate())
            GetChildAt(3)->Update();
        kChildBound = GetChildAt(3)->GetLocalBound();
        m_kBound.Merge(&kChildBound);
    }

    // Calculate the bounding box:
    if (m_kBoxBound.GetCenter() != m_kBound.GetCenter())
    {
        // Recalculate maximum possible Z bound
        float fHeightExtent = m_kBound.GetRadius() + 
            NiAbs(m_kBound.GetCenter().z - m_kBoxBound.GetCenter().z);
        m_kBoxBound.SetExtent(2, fHeightExtent); // z
    }
    
    m_bRequiresUpdate = false;    
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::SetBoundData(NiPoint3 kCenter, float fRadius, 
    NiPoint3 kPositionSum)
{
    m_kSumOfVertexLocations = kPositionSum;
    m_kBound.SetCenterAndRadius(kCenter, fRadius);
    m_pkQuadMesh->SetWorldBound(m_kBound);

    // Allow a little margin for floating point error (0.51)
    m_kBoxBound.SetCenter(kCenter);
    NiPoint3 kTopRight;
    NiPoint3 kBottomLeft;

    GetVertexAt(kTopRight, NiIndex(GetBlockSize(), GetBlockSize()));
    GetVertexAt(kBottomLeft, NiIndex::ZERO);

    float fExtent = (kTopRight.x - kBottomLeft.x) * 0.51f;

    m_kBoxBound.SetExtent(0, fExtent); // x
    m_kBoxBound.SetExtent(1, fExtent); // y
    m_kBoxBound.SetExtent(2, fRadius); // z
}
//---------------------------------------------------------------------------
void NiTerrainDataLeaf::CreateVertexAt(
    NiPoint3& pVertex,
    const NiIndex& local,
    NiUInt32) const
{    
    const NiTerrainSectorData* pkData = GetContainingSector()->GetSectorData();
    NiIndex index = SectorIndexFromLocal(local);
    NiUInt32 yOffset = (pkData->GetSectorSize() - index.y)
        * pkData->GetSectorWidthInVerts();
    NiUInt32 bufferIndex = (NiUInt32(index.x) + yOffset);
    float positionOffset = float(pkData->GetSectorSize()) * -0.5f;

    pVertex.x = float(index.x) + positionOffset;
    pVertex.y = float(index.y) + positionOffset;
    pVertex.z = pkData->GetHeightMap()[bufferIndex]
        * pkData->GetHeightScale()
        - pkData->GetHeightShift();
}
//---------------------------------------------------------------------------
