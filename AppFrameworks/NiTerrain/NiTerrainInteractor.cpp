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

#include "NiTerrainInteractor.h"
#include "NiTerrain.h"

bool NiTerrainInteractor::ms_bInTool = false;

//---------------------------------------------------------------------------
NiTerrainInteractor::NiTerrainInteractor(NiUInt32 uiPoolStartSize,
    NiUInt32 uiNumCaches, NiUInt32 uiMaxCacheSize) : 
    m_uiPoolStartSize(uiPoolStartSize), m_pkTerrain(0)
{
    NIASSERT(uiNumCaches < uiMaxCacheSize);
    m_uiPoolFreeListSize = uiMaxCacheSize;

    // Initialize the caches used by terrain deformation.
    for (NiUInt32 ui = 0; ui < uiNumCaches; ui++)
    {
        m_kOriginalPointPools[NiDeformablePoint::VERTEX].AddHead(
            NiNew NiTQuickReleaseObjectPool<NiTerrainVertex>(
            m_uiPoolStartSize));
    }

    for (NiUInt32 ui = 0; ui < uiNumCaches; ui++)
    {
        m_kOriginalPointPools[NiDeformablePoint::MASK_PIXEL].AddHead(
            NiNew NiTQuickReleaseObjectPool<NiTerrainVertex>(
            m_uiPoolStartSize));
    }
}
//---------------------------------------------------------------------------
NiTerrainInteractor::~NiTerrainInteractor()
{
    while (!m_kOriginalPointPools[NiDeformablePoint::VERTEX].IsEmpty()) 
    {
        NiDelete static_cast<NiTQuickReleaseObjectPool<NiTerrainVertex>*>(
            m_kOriginalPointPools[NiDeformablePoint::VERTEX].RemoveHead());
    }

    while (!m_kOriginalPointPools[NiDeformablePoint::MASK_PIXEL].IsEmpty()) 
    {
        NiDelete static_cast<NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*>(
            m_kOriginalPointPools[NiDeformablePoint::MASK_PIXEL].
            RemoveHead());
    }
}
//---------------------------------------------------------------------------
NiNode* NiTerrainInteractor::GetRootNode() const
{
    return (NiNode*)m_pkTerrain;
}
//---------------------------------------------------------------------------
NiNode* NiTerrainInteractor::GetSectorNode(NiUInt32 x, NiUInt32 y) const
{
    // Figure out the name of the sector we want:
    NiString kSectorName;
    kSectorName = "Sector_";
    kSectorName += NiString::FromInt(x);
    kSectorName += "_";
    kSectorName += NiString::FromInt(y);

    // Fetch that sector from the root node:
    NiAVObject* pkSectorObject = m_pkTerrain->GetObjectByName(
        NiFixedString(kSectorName));

    return NiDynamicCast(NiNode, pkSectorObject);
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::GetBound(NiBound& kBound) const
{
    bool bFirst = true;
	NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        if (bFirst) 
		{
            bFirst = false;
            kBound = NiDynamicCast(NiAVObject, pkSector)->GetWorldBound();
		}
		else
		{
			kBound.Merge(
                &NiDynamicCast(NiAVObject, pkSector)->GetWorldBound());
        }
    }
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::Collide(NiRay& kRay,
    NiUInt32 uiDetailLOD) const
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    NiBool bFound = false;
    
    //Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

	    if (pkSector && pkSector->CollideWithRay(kRay, uiDetailLOD)) 
        {
            bFound = true;
        }
    }       

    return bFound;
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::Collide(NiRay& kRay, 
    NiTerrainSector*& pkSectorOut,
    NiUInt32 uiDetailLOD) const
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    NiBool bFound = false;
    
    //Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

	    if (pkSector && pkSector->CollideWithRay(kRay, uiDetailLOD)) 
        {
            pkSectorOut = pkSector;
            
            bFound = true;
        }
    }       

    return bFound;
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::Test(NiRay& kRay, NiUInt32 uiDetailLOD) const
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
       
    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

	    if (pkSector && pkSector->CollideWithRay(kRay, uiDetailLOD)) 
            return true;
    }    
    return false;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainInteractor::GetVerticesInBound(const NiBound& kBound, 
    NiDeformablePointSet* pkPointSet,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    int uiFound = 0;

    NIASSERT(pkPointSet->GetPointType() == NiDeformablePoint::VERTEX);
    InitPointPool(pkPointSet);

    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
               
        if (pkSector) 
        {
            uiFound = pkSector->GetVertsInBound(
                kBound, pkPointSet, 
                (NiTQuickReleaseObjectPool<NiTerrainVertex>*
                    )pkPointSet->GetOriginalPointPool(), 
                uiDetailLevel, bIncludeLowerDetail);
        }
    }  

    return uiFound;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainInteractor::GetVerticesInBound(
    const NiBoundingVolume& kBound, 
    NiDeformablePointSet* pkPointSet,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    int uiFound = 0;

    NIASSERT(pkPointSet->GetPointType() == NiDeformablePoint::VERTEX);
    InitPointPool(pkPointSet);

    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
               
        if (pkSector) 
        {
            uiFound += pkSector->GetVertsInBound(
                kBound, pkPointSet,
                (NiTQuickReleaseObjectPool<NiTerrainVertex>*
                    )pkPointSet->GetOriginalPointPool(), 
                uiDetailLevel, bIncludeLowerDetail);
        }
    }  

    return uiFound;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainInteractor::GetVerticesInBound2D(const NiBound& kBound,
    NiDeformablePointSet* pkPointSet,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    int uiFound = 0;

    NIASSERT(pkPointSet->GetPointType() == NiDeformablePoint::VERTEX);
    InitPointPool(pkPointSet);
    
    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
               
        if (pkSector) 
        {
            uiFound += pkSector->GetVertsInBound2D(
                kBound, pkPointSet,
                (NiTQuickReleaseObjectPool<NiTerrainVertex>*
                )pkPointSet->GetOriginalPointPool(), 
                uiDetailLevel, bIncludeLowerDetail);
        }
    }  

    if (!uiFound)
        InvalidatePointSet(pkPointSet);

    return uiFound;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainInteractor::GetPixelsInBound(const NiBound& kBound, 
    NiDeformablePointSet* pkPointSet, const NiSurface* pkSurface,
    bool bCreateMasksIfNotExist,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    int uiFound = 0;
    
    NIASSERT(pkPointSet->GetPointType() == NiDeformablePoint::MASK_PIXEL);
    InitPointPool(pkPointSet);
    
    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
               
        if (pkSector) 
        {
            uiFound += pkSector->GetPixelsInBound(kBound, pkPointSet, 
                (NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*
                    )pkPointSet->GetOriginalPointPool(), 
                pkSurface, bCreateMasksIfNotExist, 
                uiDetailLevel, bIncludeLowerDetail);
        }
    }  

    return uiFound;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainInteractor::GetPixelsInBound2D(const NiBound& kBound, 
    NiDeformablePointSet* pkPointSet, const NiSurface* pkSurface,
    bool bCreateMasksIfNotExist,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NIASSERT(m_pkTerrain);
    NiTMapIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    int uiFound = 0;
    
    NIASSERT(pkPointSet->GetPointType() == NiDeformablePoint::MASK_PIXEL);
    InitPointPool(pkPointSet);
    
    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
        bool bDeformable =
            pkSector->GetSectorData()->GetDeformable();

        if (!bDeformable)
            continue;
               
        if (pkSector) 
        {
            uiFound += pkSector->GetPixelsInBound2D(kBound, pkPointSet,
                (NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*
                    )pkPointSet->GetOriginalPointPool(), 
                pkSurface, bCreateMasksIfNotExist,
                uiDetailLevel, bIncludeLowerDetail);
        }
    }  

    return uiFound;
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::CommitChanges(
    NiDeformablePointSet* pkPointSet,
    bool bModifyParentLOD)
{
    bool bWarned = false;

    NiUInt32 uiNumVertices = pkPointSet->GetNumPoints();

    if (uiNumVertices == 0) 
    {
        InvalidatePointSet(pkPointSet);
        return false;
    }

    NiDeformablePoint* pkCurPoint;
    NiTerrainVertex kVertex;
    NiSurfaceMaskPixel kPixel;

    NiTerrainSector* pkSector = 0;

    for (NiUInt32 uiIndex = 0; uiIndex < uiNumVertices; ++uiIndex) 
    {

        pkCurPoint = pkPointSet->GetAt(uiIndex);
        if (!pkCurPoint->IsActive())
            continue;

        // Make sure the data is of the correct type!
        if (pkCurPoint->GetType() == NiDeformablePoint::VERTEX)
        {
            NiTQuickReleaseObjectPool<NiTerrainVertex>* pkPool = 
                (NiTQuickReleaseObjectPool<NiTerrainVertex>*)
                (pkPointSet->GetOriginalPointPool());
            
            pkPool->GetValue(pkCurPoint->GetOriginalIndex(), kVertex);

            pkSector = kVertex.GetFirstContainingLeaf()->GetContainingSector();
        }
        else if (pkCurPoint->GetType() == NiDeformablePoint::MASK_PIXEL)
        {
            NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkPool = 
                (NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*)
                (pkPointSet->GetOriginalPointPool());

            pkPool->GetValue(pkCurPoint->GetOriginalIndex(), kPixel);

            pkSector = kPixel.GetFirstContainingLeaf()->GetContainingSector();
        }
        else
        {
            // Invalid point type
            InvalidatePointSet(pkPointSet);
            return false;
        }

        // Make sure that the sector is still deformable
        bool bDeformable = pkSector->GetSectorData()->GetDeformable();
        if (!bDeformable)
        {
            if (!bWarned)
            {
                NiOutputDebugString(
                    "Attempting to deform a non-deformable sector\n");
            }

            bWarned = true;
            continue;
        }
            
        if (pkCurPoint->GetType() == NiDeformablePoint::VERTEX)
        {
            float fNewHeight = pkCurPoint->GetValue();
            NiTerrain* pkTerrain = pkSector->GetTerrain();
            if (pkTerrain)
            {
                pkTerrain->ModifyVertexHeightFrom(pkSector, kVertex, 
                    fNewHeight, bModifyParentLOD);
            }
            else
            {
                pkSector->ModifyVertexHeightFrom(
                    kVertex, kVertex.GetFirstContainingLeaf(), 
                    pkCurPoint->GetValue(),
                    bModifyParentLOD);
            }
        }        
        else if (pkCurPoint->GetType() == NiDeformablePoint::MASK_PIXEL)
        {
            // Get a non const version of the data leaf
            const NiTerrainDataLeaf* pkConstDataLeaf = 
                kPixel.GetFirstContainingLeaf();
            NiTerrainDataLeaf* pkDataLeaf = 
                pkConstDataLeaf->GetContainingSector()->GetLeaf(
                    pkConstDataLeaf->GetBlockID());

            pkSector->ModifySurfaceMaskFrom(
                &kPixel, 
                pkDataLeaf, 
                (NiUInt8)NiClamp(pkCurPoint->GetValue(), 0.0f, 255.0f),
                0,
                bModifyParentLOD);
        }
    }

    InvalidatePointSet(pkPointSet);
    return true;
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::UpdateSectors(float fTime, 
    NiEntityErrorInterface*, NiExternalAssetManager*)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();

    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        pkSector->Update(fTime);
    }
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::RebuildLighting(bool bAllLighting)
{
    NIASSERT(m_pkTerrain);
    NiTMap<NiTerrainSector*, NiTPrimitiveArray<NiTerrainDataLeaf*>*> kChanged(
        31); 

    NiTPrimitiveArray<NiTerrainDataLeaf*>* pkChangedList = NULL;
    NiTerrainSector* pkSector = NULL;
    NiTMapIterator kIterator;

    // For each sector
    //  - Find the list of leafs to change
    //  - Execute first pass on those leafs
    kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    while (kIterator)
    {        
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
        
        if (bAllLighting || pkSector->RequiresLightingRebuild())
        {
            pkChangedList = NiNew NiTPrimitiveArray<NiTerrainDataLeaf*>();
            pkSector->GetOutdatedLightingLeafList(pkChangedList, bAllLighting);

            // Skip this sector if there is nothing that requires updating
            if (pkChangedList->GetEffectiveSize())
            {
                kChanged.SetAt(pkSector, pkChangedList);
                pkSector->CalculateNormalsTangentsPhase1(*pkChangedList);
            }
            else
            {
                NiDelete pkChangedList;
            }
        }
    }

    // For each sector 
    //  - Execute second pass on those leafs
    kIterator = kChanged.GetFirstPos();
    while (kIterator)
    {
        kChanged.GetNext(kIterator, pkSector, pkChangedList);        
        pkSector->CalculateNormalsTangentsPhase2(*pkChangedList);
    }

    // For each sector 
    //  - Execute third pass on those leafs
    //  - Regenerate Geomorphing values
    kIterator = kChanged.GetFirstPos();
    while (kIterator)
    {
        kChanged.GetNext(kIterator, pkSector, pkChangedList);        
        pkSector->CalculateNormalsTangentsPhase3(*pkChangedList);
        pkSector->CalculateGeoMorph(*pkChangedList);

        NiDelete pkChangedList;
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainInteractor::RebuildAllLighting()
{
    RebuildLighting(true);
}
//---------------------------------------------------------------------------
inline void NiTerrainInteractor::RebuildChangedLighting()
{
    RebuildLighting(false);
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::MarkSurfaceChanged(const NiSurface* pkSurface)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();

    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        pkSector->MarkSurfaceChanged(pkSurface);
    }
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::RemoveSurface(const NiSurface* pkSurface)
{
    NIASSERT(m_pkTerrain);
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();

    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        pkSector->RemoveSurface(pkSurface);
    }
}
//---------------------------------------------------------------------------
NiTerrainDecal* NiTerrainInteractor::CreateDecal(
    NiTexture* pkDecalTexture, NiPoint3 kPosition, NiUInt32 uiSize, 
    NiUInt32 uiRatio, float fTimeOfDeath, float fDecayLength, 
    float fDepthBiasOffset)
{
    NIASSERT(m_pkTerrain);
 
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    NiRay kRay(NiPoint3(kPosition.x, kPosition.y, NI_INFINITY), 
        NiPoint3(0,0,-1));
    
    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        if (pkSector && pkSector->CollideWithRay(kRay, 5)) 
        {
            NiTerrainDecal* pkDecal = NiNew NiTerrainDecal();

            pkDecal->CreateDecal(pkDecalTexture, kPosition, pkSector, uiSize, 
                (float)uiRatio, fTimeOfDeath, fTimeOfDeath -fDecayLength,
                fDepthBiasOffset);

            // The fading distance should always be set to the morph distance
            // in order to avoid z fighting issues
            NiUInt32 uiBlockWidth = pkSector->GetSectorData()->GetBlockSize();

            static const float fSqrt2 = NiSqrt(2.0f);
            float fMaxDist = NiSqr(float(uiBlockWidth) * 
                pkSector->GetSectorData()->GetLODScale() * fSqrt2);
            float fMorphDistance = fSqrt2 * (uiBlockWidth / 2);

            float fLODThres = NiSqrt(fMaxDist) - fMorphDistance;
            pkDecal->SetFadingDistance(fLODThres);
            pkSector->AddDecal(pkDecal);

            return pkDecal;
        }
    }
    
    return NULL;
}
//---------------------------------------------------------------------------
int NiTerrainInteractor::GetNumTriangles() const
{
    int iNumTriangles = 0;
    NiTListIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();

    // Loop through all sectors
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        iNumTriangles += pkSector->GetNumTriangles();

    }
    return iNumTriangles;
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::GetSmoothedValue(
    NiDeformablePointSet* pkPointSet, const NiDeformablePoint*& pkPoint, 
    float& fValue, const NiSurface* pkSurface) const
{
    NiTerrainVertex kVertex;
    NiSurfaceMaskPixel kPixel;
    const NiTerrainSector* pkSector;

    // Make sure the data is of the correct type!
    if (pkPoint->GetType() == NiDeformablePoint::VERTEX)
    {
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkPool = 
            (NiTQuickReleaseObjectPool<NiTerrainVertex>*)
            (pkPointSet->GetOriginalPointPool());
        
        pkPool->GetValue(pkPoint->GetOriginalIndex(), kVertex);
        pkSector = kVertex.GetFirstContainingLeaf()->GetContainingSector();

        // Get values of all surrounding points (Top, Bottom, Left, Right)
        NiIndex kLocal;
        kVertex.GetParentLeaf()->IndexToLocal(kVertex.GetIndex(), kLocal);

        float fAccum = pkPoint->GetValue();
        float fCount = 1.0f;
        NiIndex kAdjIndex;
        NiTerrainDataLeaf* pkAdjacent;
        NiTerrainDataLeaf* pkLeaf = kVertex.GetParentLeaf();
        NiUInt32 uiRightEdge = pkSector->GetSectorData()->GetBlockSize();

        // Top / Bottom
        if (kLocal.y == uiRightEdge)
        {
            // Top
            pkAdjacent = pkLeaf->GetAdjacentFast(
                NiTerrainDataLeaf::BORDER_TOP);
            if (pkAdjacent)
            {
                kAdjIndex.x = kLocal.x;
                kAdjIndex.y = 1;
                fAccum += pkAdjacent->GetHeightAt(kAdjIndex);

                fCount += 2.0f;
            }
            else
            {
                fCount += 1.0f;
            }

            // Bottom
            kAdjIndex.x = kLocal.x;
            kAdjIndex.y = kLocal.y - 1;
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);
        }
        else if (kLocal.y == 0)
        {
            // Bottom
            pkAdjacent = pkLeaf->GetAdjacentFast(
                NiTerrainDataLeaf::BORDER_BOTTOM);
            if (pkAdjacent)
            {
                kAdjIndex.x = kLocal.x;
                kAdjIndex.y = uiRightEdge - 1;
                fAccum += pkAdjacent->GetHeightAt(kAdjIndex);

                fCount += 2.0f;
            }
            else    
            {
                fCount += 1.0f;
            }

            // Top
            kAdjIndex.x = kLocal.x;
            kAdjIndex.y = kLocal.y + 1;
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);
        }
        else
        {
            // Top  
            kAdjIndex.x = kLocal.x;
            kAdjIndex.y = kLocal.y + 1;        
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);

            // Bottom
            kAdjIndex.x = kLocal.x;
            kAdjIndex.y = kLocal.y - 1;
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);

            fCount += 2.0f;
        }

        // Left / Right
        if (kLocal.x == uiRightEdge)
        {
            // Right
            pkAdjacent = pkLeaf->GetAdjacentFast(
                NiTerrainDataLeaf::BORDER_RIGHT);
            if (pkAdjacent)
            {
                kAdjIndex.x = 1;
                kAdjIndex.y = kLocal.y;
                fAccum += pkAdjacent->GetHeightAt(kAdjIndex);

                fCount += 2.0f;
            }
            else
            {
                fCount += 1.0f;
            }

            // Left
            kAdjIndex.x = kLocal.x - 1;
            kAdjIndex.y = kLocal.y;
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);
        }
        else if (kLocal.x == 0)
        {
            // Left
            pkAdjacent = pkLeaf->GetAdjacentFast(
                NiTerrainDataLeaf::BORDER_LEFT);
            if (pkAdjacent)
            {
                kAdjIndex.x = uiRightEdge - 1;
                kAdjIndex.y = kLocal.y;
                fAccum += pkAdjacent->GetHeightAt(kAdjIndex);

                fCount += 2.0f;
            }
            else             
            {
                fCount += 1.0f;
            }

            // Right
            kAdjIndex.x = kLocal.x + 1;
            kAdjIndex.y = kLocal.y;     
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);
        }
        else
        {       
            // Left
            kAdjIndex.x = kLocal.x - 1;
            kAdjIndex.y = kLocal.y;
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);

            // Right
            kAdjIndex.x = kLocal.x + 1;
            kAdjIndex.y = kLocal.y;     
            fAccum += pkLeaf->GetHeightAt(kAdjIndex);

            fCount += 2.0f;
        }

        // Divide by number of entries added
        fValue = fAccum / fCount;
    }
    else if (pkPoint->GetType() == NiDeformablePoint::MASK_PIXEL)
    {
        NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkPool = 
            (NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*)
            (pkPointSet->GetOriginalPointPool());

        pkPool->GetValue(pkPoint->GetOriginalIndex(), kPixel);

        pkSector = kPixel.GetFirstContainingLeaf()->GetContainingSector();

        NiUInt32 uiAccum = kPixel.GetValue();
        float fCount = 1.0f;
        NiUInt8 ucAdjacentValue;

        // Work out what surface we want to read, if its not already given
        if (pkSurface == 0)
        {
            NiUInt32 uiNumSurfaces = 
                kPixel.GetFirstContainingLeaf()->GetSurfaceCount();
            const NiSurfaceMask* pkMask;

            for (NiUInt32 ui = 0; ui < uiNumSurfaces; ++ui)
            {
                pkMask = kPixel.GetFirstContainingLeaf()->GetSurfaceMask((NiUInt16)ui);
                if (pkMask == kPixel.GetMask())
                {
                    pkSurface = 
                        kPixel.GetFirstContainingLeaf()->GetSurface((NiUInt16)ui);
                    break;
                }
            }

            if (!pkSurface)
                return false;
        }

        for (NiUInt8 uc = 0; uc < 4; ++uc)
        {
            if (pkSector->GetAdjacentSurfaceOpacity(
                kPixel, pkSurface,
                NiTerrainDataLeaf::BORDERS(1 << uc), ucAdjacentValue))
            {
                fCount += 1.0f;
                uiAccum = (NiUInt32)(uiAccum + ucAdjacentValue);
            }
        }

        fValue = float(uiAccum) / fCount;
    }
    else
    {
        return NiBool(false);
    }

    return NiBool(true);
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::GetSurfaceOpacity(
    NiRay& kRay, const NiSurface* pkSurface, NiUInt8& ucOpacity,
    NiUInt32 uiDetailLevel, bool bIncludeLowerDetail)
    const
{
    NIASSERT(m_pkTerrain);
        
    NiBool bFound = Collide(kRay, uiDetailLevel);

    // Loop through all sectors
    if (bFound)
    {
        NiPoint3 kLocation, kNormal;
        kRay.GetIntersection(kLocation, kNormal);

        const NiTerrainDataLeaf *pkLeaf = kRay.GetCollidedLeaf();

        NiUInt32 uiNumSurfaces = pkLeaf->GetSurfaceCount();
        while (uiNumSurfaces == 0 && bIncludeLowerDetail)
        {
            pkLeaf = pkLeaf->GetParent();
            if (!pkLeaf)
                break;

            uiNumSurfaces = pkLeaf->GetSurfaceCount();
            uiDetailLevel = pkLeaf->GetLevel();
        }

        return NiTerrainUtils::GetSurfaceOpacity(pkSurface, kLocation, 
            ucOpacity, uiDetailLevel, bIncludeLowerDetail, pkLeaf);
    }  

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::GetMetaData(NiRay& kRay, 
    NiTFixedStringMap<NiMetaData>& kMetaData) const
{
    NIASSERT(m_pkTerrain);
    NiBool bFound = Collide(kRay);

    if (bFound)
    {
        NiPoint3 kLocation, kNormal;
        kRay.GetIntersection(kLocation, kNormal);
        GetMetaData(kLocation, kRay.GetCollidedLeaf(), kMetaData);
    }  

    return bFound;
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::GetMetaData(const NiPoint3& kIntersectionPt,
    const NiTerrainDataLeaf *pkLeaf, 
    NiTFixedStringMap<NiMetaData>& kMetaData) const
{
    NIASSERT(pkLeaf);
    kMetaData.RemoveAll();

    NiUInt32 uiNumSurfaces = pkLeaf->GetSurfaceCount();
    NiUInt32 uiDetailLevel = 0;
    while (uiNumSurfaces == 0)
    {
        pkLeaf = pkLeaf->GetParent();
        if (!pkLeaf)
            break;

        uiNumSurfaces = pkLeaf->GetSurfaceCount();
        uiDetailLevel = pkLeaf->GetLevel();
    }

    for (NiUInt32 ui = 0; ui < uiNumSurfaces; ui++)
    {
        const NiSurface *pkSurface = pkLeaf->GetSurface(ui);

        NiUInt8 ucOpacity;
        NiTerrainUtils::GetSurfaceOpacity(
            pkSurface, kIntersectionPt, ucOpacity, uiDetailLevel, true,  
            pkLeaf);

        float fWeight = ucOpacity / 255.0f;
        if (fWeight > 0.001f)
        {
            NiFixedString kSurfaceName = pkSurface->GetName();
            NiMetaData kSurfaceMeta = pkSurface->GetMetaData();
            kSurfaceMeta.UpdateWeights(fWeight);
            kMetaData.SetAt(kSurfaceName, kSurfaceMeta);
        }
    }
}
//---------------------------------------------------------------------------
NiBool NiTerrainInteractor::GetBlendedMetaData(NiRay& kRay, 
    NiTFixedStringMap<NiMetaData>& kMetaData,
    NiMetaData& kBlendedMetaData) const
{
    NIASSERT(m_pkTerrain);
    NiBool bFound = Collide(kRay);

    if (bFound)
    {
        NiPoint3 kLocation, kNormal;
        kRay.GetIntersection(kLocation, kNormal);
        GetBlendedMetaData(kLocation, kRay.GetCollidedLeaf(), 
            kMetaData,
            kBlendedMetaData);
    }  

    return bFound;
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::GetBlendedMetaData(
    const NiPoint3& kIntersectionPt, 
    const NiTerrainDataLeaf *pkLeaf, 
    NiTFixedStringMap<NiMetaData>& kMetaData,
    NiMetaData& kBlendedMetaData) const
{
    GetMetaData(kIntersectionPt, pkLeaf, kMetaData);
   
    NiMetaData kCurData;
    NiFixedString kCurKey;
    
    NiTMapIterator kIter = kMetaData.GetFirstPos();
    while (kIter != NULL)
    {
        kMetaData.GetNext(kIter, kCurKey, kCurData);
        kBlendedMetaData.Blend(&kCurData, kCurData.GetWeight());
    }
}
//---------------------------------------------------------------------------
bool NiTerrainInteractor::GetPointSpacing(NiDeformablePoint::POINT_TYPE eType, 
    NiUInt32 uiDetailLevel, float& fSpacing) const
{
	if (m_pkTerrain->m_kSectors.GetCount() == 0)
        return false;

    if (eType == NiDeformablePoint::VERTEX)
    {
        // All sectors MUST have the same vertex spacing, so we can just ask 
        // the first sector
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        NiTMapIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
		m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
        pkSector->GetPointSpacing(eType, fSpacing, uiDetailLevel);
    }
    else
    {
        // Since different sectors (and even different blocks within those
        // sectors) can have different detail levels, we must find the highest
        // resolution that exists. (i.e., smallest spacing)
        fSpacing = NI_INFINITY;
        NiTListIterator kIterator = 
            m_pkTerrain->m_kSectors.GetFirstPos();
		while (kIterator)
		{
            NiTerrainSector* pkSector = NULL;
            NiUInt32 ulIndex;
            m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);

            float fSectorSpacing;
			pkSector->GetPointSpacing(
                eType, fSectorSpacing, uiDetailLevel);

            fSpacing = NiMin(fSectorSpacing, fSpacing);
        }

        if (fSpacing == NI_INFINITY) 
        {
            // Unable to retrieve spacing for any sectors at the given detail 
            // level.
            fSpacing = 0.0f;
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainInteractor::GetHeightMapValues(float*& pfMap, 
    size_t stBufferSize, NiUInt32& uiNumVal, float& fMinVal, float& fMaxVal)
{
    if (!m_pkTerrain)
        return false;

    if (!m_pkTerrain->m_kSectors.GetCount())
        return false;

    NiTerrainSector* pkSector = NULL;
    NiUInt32 ulIndex;
    NiTMapIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
    return pkSector->GetHeightMapValues(pfMap, stBufferSize, uiNumVal, 
        fMinVal, fMaxVal);
}
//---------------------------------------------------------------------------
bool NiTerrainInteractor::SetHeightFromMap(float* pfMap, size_t stBufferSize, 
    NiUInt32 uiNumVal)
{
    if (!m_pkTerrain)
        return false;

    if (!m_pkTerrain->m_kSectors.GetCount())
        return false;

    NiTerrainSector* pkSector = NULL;
    NiUInt32 ulIndex;
    NiTMapIterator kIterator = m_pkTerrain->m_kSectors.GetFirstPos();
    m_pkTerrain->m_kSectors.GetNext(kIterator, ulIndex, pkSector);
    return pkSector->SetHeightFromMap(pfMap, stBufferSize, uiNumVal);
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::SetTargetLoadedLOD(NiInt16 iSectorX, NiInt16 iSectorY, 
	NiInt32 iTargetLOD)
{
	NIASSERT(m_pkTerrain);
    NiTerrainSector* pkSector = m_pkTerrain->GetSector(iSectorX, iSectorY);
    if (pkSector)
        pkSector->SetTargetLoadedLOD(iTargetLOD, true);
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::InvalidatePointSet(
    NiDeformablePointSet* pkPointSet)
{
    // No need to assert here. In some cases this method may get called on a 
    // point set that has already been invalidated.
    if (!pkPointSet)
        return;

    NiMemObject* pkOriginalPool = pkPointSet->GetOriginalPointPool();
    NiDeformablePoint::POINT_TYPE eType = pkPointSet->GetPointType();
    NiMemObject* pkOldPointPool = pkPointSet->SetOriginalPointPool(NULL);
        
    if (!pkOriginalPool || !pkOldPointPool)
    {
        pkPointSet->InvalidateData();
        return;
    }
   
    switch (eType)
    {
    case NiDeformablePoint::VERTEX:
        static_cast<NiTQuickReleaseObjectPool<NiTerrainVertex>*>
            (pkOriginalPool)->Reset();
        break;

    case NiDeformablePoint::MASK_PIXEL:
        static_cast<NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*>
            (pkOriginalPool)->Reset();
        break;

    default:
        NIASSERT(!"Invalid point pool type.");
    }

    // Insert the "freed" point pool into the free list if we haven't reached
    // the maximum number of entries.
    if (m_kOriginalPointPools[eType].GetSize() < m_uiPoolFreeListSize)
    {
        m_kOriginalPointPools[eType].AddTail(pkOriginalPool);
    }
    else
    {
        switch (eType)
        {
        case NiDeformablePoint::VERTEX:
            NiDelete static_cast<NiTQuickReleaseObjectPool<NiTerrainVertex>*>
                (pkOriginalPool);
            break;

        case NiDeformablePoint::MASK_PIXEL:
            NiDelete static_cast<NiTQuickReleaseObjectPool<
                NiSurfaceMaskPixel>*>(pkOriginalPool);
            break;
        default:
            // No need to assert here, an invalid type is handled above
            break;
        }
    }

    pkPointSet->InvalidateData();
}
//---------------------------------------------------------------------------
void NiTerrainInteractor::InitPointPool(NiDeformablePointSet* pkPointSet)
{
    if (pkPointSet->GetOriginalPointPool())
        return;
        
    NiMemObject* pkPool = NULL;
    NiDeformablePoint::POINT_TYPE eType = pkPointSet->GetPointType();

    // First see if we have anything available in the free list.
    if (m_kOriginalPointPools[eType].GetSize() > 0)
    {
        pkPool = m_kOriginalPointPools[eType].RemoveHead();
        NIASSERT(pkPool);
    }
    else
    {
        // We need to allocate memory for this point pool which will then be 
        // added back to the free list when deallocated.
        switch (eType)
        {
        case NiDeformablePoint::VERTEX:
            pkPool = NiNew NiTQuickReleaseObjectPool<NiTerrainVertex>(
                m_uiPoolStartSize);
            break;

        case NiDeformablePoint::MASK_PIXEL:
            pkPool = NiNew NiTQuickReleaseObjectPool<
                NiSurfaceMaskPixel>(m_uiPoolStartSize);
            break;

        default:
            NIASSERT(!"Invalid deformation type");  
            break;
        }
    }

    pkPointSet->SetOriginalPointPool(pkPool);
}
