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

#include <NiMath.h>

#include "NiTerrainSector.h"
#include "NiSurfaceMask.h"
#include "NiSurfaceMaskPixel.h"
#include "NiTerrainVertex.h"
#include "NiDynamicStreamCache.h"
#include "NiTerrainDecal.h"
#include "NiTerrainDecalManager.h"
#include "NiTerrain.h"
#include "NiTerrainInteractor.h"
#include "NiTTerrainRandomAccessIterator.h"

// Include DX9 Renderer for device reset callbacks
#ifdef WIN32 
#include <NiDX9Renderer.h>
#endif

NiImplementRTTI(NiTerrainSector, NiNode);

//---------------------------------------------------------------------------
NiTerrainSector::NiTerrainSector(bool) :
    m_bUpdating(false),
    m_bRecycleDynamicStreams(true),
    m_bUsingShortIndexBuffer(true),
    m_pkTerrain(NULL),
    m_pkSectorData(NULL),
    m_uiNumRenderedTriangles(0),
    m_puiLeafOffsets(NULL),
    m_pkLocalCamera(NULL),
    m_spDecalManager(NULL),
    m_spSurfacePalette(NULL),
    m_pkMetaDataStore(NULL),
    m_pkIndexLock(NULL),
    m_pkUVLock(NULL),
    m_pkQuadData(NULL),
    m_pkQuadMesh(NULL),
    m_pkSurfaceMaskStack(NULL),
    m_uiSurfaceMaskStackPos(0),
    m_uiSurfaceMaskStackSize(0),
    m_pfBestPixelSpacingCache(NULL),
    m_uiDefaultMaskSize(64),
    m_bHasShapeChangedLastUpdate(false)
{
    m_pkSectorData = NiNew NiTerrainSectorData();
    m_spDecalManager = NiNew NiTerrainDecalManager();
    m_pkLocalCamera = NiNew NiCamera();

    SetMetaDataStore(NULL);

    SubscribeToDXDeviceResetNotification();
}
//---------------------------------------------------------------------------
NiTerrainSector::~NiTerrainSector() 
{    
    UnsubscribeToDXDeviceResetNotification();

    NiOutputDebugString("-[ Unloading Data ]- - - - - - - - - -\n");

    if (m_puiLeafOffsets)
    {
        NiOutputDebugString("Unloading Quad Mesh\n");    
        UnloadLODData(0);
    }
    
    DestroySupportingData();
    m_spSurfacePalette = NULL;

    NiFree(m_puiLeafOffsets);
    m_puiLeafOffsets = NULL;

    NiDelete m_pkSectorData;
    NiDelete m_pkLocalCamera;
    m_spDecalManager = NULL;

    NiOutputDebugString("- - - - - - - - - - - - - - - - - - -\n");
}
//---------------------------------------------------------------------------
void NiTerrainSector::DoUpdate(NiUpdateProcess& kUpdate) 
{
    // Prevent something attempting to update while we are loading
    if (m_bUpdating)
        return;
    m_bUpdating = true;

    if (!m_pkQuadData)
    {
        m_bUpdating = false;
        return;
    }

    // Update transforms?
    if (m_pkQuadMesh && 
        GetWorldTransform() != m_pkSectorData->GetWorldTransform())
    {
        m_pkSectorData->SetWorldTransform(GetWorldTransform());

        NiUInt32 uiNumLeaves = m_kLeafArray.GetSize();
        NiTerrainBlock* pkBlock;
        NiTransform kWorldTransform = GetWorldTransform();
        for (NiUInt32 ui = 0; ui < uiNumLeaves; ++ui)
        {
            pkBlock = m_kLeafArray[ui]->GetQuadMesh();
            NIASSERT(pkBlock);

            pkBlock->SetWorldTranslate(kWorldTransform.m_Translate);
            pkBlock->SetWorldScale(kWorldTransform.m_fScale);
            pkBlock->SetWorldRotate(kWorldTransform.m_Rotate);

            pkBlock->GetMesh().SetWorldTranslate(kWorldTransform.m_Translate);
            pkBlock->GetMesh().SetWorldScale(kWorldTransform.m_fScale);
            pkBlock->GetMesh().SetWorldRotate(kWorldTransform.m_Rotate);
        }

        SetShapeChangedLastUpdate(true);
    }

    // Update the local bounds?
    if (m_pkQuadData->RequiresUpdate())
    {
        m_pkQuadData->Update();
        
        // Re-assign the root node bound
        if (m_pkQuadMesh)
        {
            NiBound kBound = m_pkQuadData->GetLocalBound();
            kBound.SetRadius(kBound.GetRadius() * GetWorldScale());
            kBound.SetCenter(GetWorldTransform() * kBound.GetCenter());
            m_pkQuadMesh->SetWorldBound(kBound);
        }
    }

    // Reset the triangle counter
    m_uiNumRenderedTriangles = 0;

    // Update the meshes
    float fTime = kUpdate.GetTime();
    if (m_pkQuadMesh->RequiresUpdate()) 
    {
        m_pkQuadMesh->Update(fTime);
    }

    // Update any decals that have been applied
    m_spDecalManager->UpdateDecals(this, fTime);
    
    if (HasShapeChangedLastUpdate() && m_pkTerrain)
    {
        m_pkTerrain->SetShapeChangedLastUpdate(true);
        SetShapeChangedLastUpdate(false);
    }

    m_bUpdating = false;

    return;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::ProcessLOD(NiCullingProcess &kCuller)
{
    bool bResult = false;
    if (m_pkQuadMesh && !m_bUpdating) 
    {   
        // Reset the LOD information:
        m_pkQuadMesh->ResetLOD();

        // Need to convert the camera to terrain coordinate space
        // create the transformation matrix to go from world space to terrain 
        // model space:
        const NiCamera* pkCamera = kCuller.GetCamera();
        m_pkLocalCamera->SetTranslate(GetWorldRotate().Inverse() * (
            (pkCamera->GetWorldTranslate() - GetWorldTranslate())
            / GetWorldScale()));
        m_pkLocalCamera->SetRotate(GetWorldRotate().Inverse() * 
            pkCamera->GetWorldRotate());
        m_pkLocalCamera->SetScale(pkCamera->GetWorldScale());

        NiFrustum kFrustum = pkCamera->GetViewFrustum();
        kFrustum.m_fFar /= GetWorldScale();
        kFrustum.m_fNear /= GetWorldScale();
        m_pkLocalCamera->SetViewFrustum(kFrustum);

        m_pkLocalCamera->Update(0.0f);
        
        // Set this sectors culling and camera data - shared throughout our
        // block tree during rendering
        m_pkSectorData->SetCullingProcess(&kCuller);  
        m_pkSectorData->SetLODCamera(m_pkLocalCamera);
        bResult = m_pkQuadMesh->ProcessLOD();
        
        // Update the number of triangles that have been drawn
        m_uiNumRenderedTriangles += m_pkSectorData->ResetVisible() * 2 * 
            m_pkSectorData->GetBlockSize() * m_pkSectorData->GetBlockSize();
    }

    return bResult;
}
//---------------------------------------------------------------------------
void NiTerrainSector::ProcessBorders()
{
    // Update the stitching information
    if (m_pkQuadMesh)
        m_pkQuadMesh->ProcessBorders();
}
//---------------------------------------------------------------------------
void NiTerrainSector::ProcessDecals(NiCullingProcess& kCuller)
{
    // Cull the decals
    m_spDecalManager->Cull(kCuller);
}
//---------------------------------------------------------------------------
void NiTerrainSector::OnVisible(NiCullingProcess& kCuller)
{
    // Calculate block visibility and stitching
    if (ProcessLOD(kCuller))
    {
        ProcessBorders();

        // Cull decals according to the blocks they are attached to
        ProcessDecals(kCuller);
    }
}
//---------------------------------------------------------------------------
NiInt32 NiTerrainSector::GetNumTriangles() 
{
    return m_uiNumRenderedTriangles;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::GetPointSpacing(
    NiDeformablePoint::POINT_TYPE eType, 
    float& fSpacing, NiUInt32 uiDetailLevel) const
{
    if (uiDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiDetailLevel = GetSectorData()->GetNumLOD();
    }

    if (eType == NiDeformablePoint::VERTEX)
    {        
        fSpacing = (1 << (m_pkSectorData->GetNumLOD() - uiDetailLevel)) * 
            m_pkSectorData->GetWorldScale();
        return true;
    }
    else if (eType == NiDeformablePoint::MASK_PIXEL)
    {
        // Note: all surfaces within a single block always have the same mask
        // resolution
        NIASSERT(m_pfBestPixelSpacingCache);
        fSpacing = m_pfBestPixelSpacingCache[uiDetailLevel];
        return fSpacing > 0.0f;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiTerrainSector::SetIsDeformable(bool bIsDeformable) 
{
    bIsDeformable |= NiTerrainInteractor::InToolMode();

    if (m_pkSectorData->GetDeformable() != bIsDeformable)
    {
        m_pkSectorData->SetDeformable(bIsDeformable);

        // If no dynamic stream cache exists, build one.
        if (!m_pkSectorData->GetDynamicStreamCache() && bIsDeformable && 
            m_pkQuadData)
        {
            NIASSERT(m_pkSectorData->GetStaticPositionStream(0));
            NIASSERT(m_pkSectorData->GetStaticNormalTangentStream(0));

            if (!m_pkSectorData->GetStaticPositionStream(0) ||
                !m_pkSectorData->GetStaticNormalTangentStream(0))
            {
                m_pkSectorData->SetDeformable(false);
                return;
            }

            NiUInt32 uiNumDesc = 0;
            NiDataStream* pkStream;
            NiDataStreamElementSet kElementSetP;
            NiDataStreamElementSet kElementSetNT;

            pkStream = m_pkSectorData->GetStaticPositionStream(0);
            uiNumDesc = pkStream->GetElementDescCount();
            for (NiUInt32 ui = 0; ui < uiNumDesc; ++ui)
                kElementSetP.Add(pkStream->GetElementDescAt(ui));
            kElementSetP.m_uiStride = pkStream->GetStride();

            pkStream = m_pkSectorData->GetStaticNormalTangentStream(0);
            uiNumDesc = pkStream->GetElementDescCount();
            for (NiUInt32 ui = 0; ui < uiNumDesc; ++ui)
                kElementSetNT.Add(pkStream->GetElementDescAt(ui));
            kElementSetNT.m_uiStride = pkStream->GetStride();

            NiUInt32 uiSize = m_pkSectorData->GetNumBlocks() / 4;

            // If we failed to create the cache, we can't be deformable.
            bool bRes = CreateDeformationCache(uiSize, 
                kElementSetP,kElementSetNT);
            if (!bRes)
            {
                m_pkSectorData->SetDeformable(false);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::SetTerrain(NiTerrain* pkTerrain)
{
    if (m_pkTerrain == pkTerrain)
        return;

    if (m_pkTerrain)
    {
        m_pkTerrain->RemoveSector(this);
    }

    m_pkTerrain = pkTerrain;

    if (m_pkTerrain)
    {
        m_pkTerrain->AddSector(this);
        m_pkSectorData->SetConfiguration(m_pkTerrain->GetConfiguration());
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::SetSectorIndex(
    NiInt16 sXIndex, NiInt16 sYIndex)
{
    if (m_pkSectorData->GetSectorIndexX() != sXIndex ||
        m_pkSectorData->GetSectorIndexY() != sYIndex)
    {
        m_pkSectorData->SetSectorIndexX(sXIndex);
        m_pkSectorData->SetSectorIndexY(sYIndex);
    }
}
//---------------------------------------------------------------------------
NiTerrainSector* NiTerrainSector::GetAdjacentSector(NiUInt32 uiBorder)
    const
{
    NIASSERT(m_pkTerrain);
    NiInt16 sIndexX;
    NiInt16 sIndexY;
    GetSectorIndex(sIndexX, sIndexY);

    if (uiBorder & NiTerrainDataLeaf::BORDER_LEFT)
        sIndexX --;
    if (uiBorder & NiTerrainDataLeaf::BORDER_RIGHT)
        sIndexX ++;
    if (uiBorder & NiTerrainDataLeaf::BORDER_TOP)
        sIndexY ++;
    if (uiBorder & NiTerrainDataLeaf::BORDER_BOTTOM)
        sIndexY --;

    return m_pkTerrain->GetSector(sIndexX, sIndexY);
}
//---------------------------------------------------------------------------
void NiTerrainSector::ModifyVertexHeightFrom(const NiTerrainVertex& kVertex,
    NiTerrainDataLeaf* pkCurrentLeaf, float fNewHeight, bool bModifyParentLOD)
{
    NiIndex kLocalIndex;

    if (pkCurrentLeaf->GetNumSubDivisions() > 
        kVertex.GetParentLeaf()->GetNumSubDivisions())
    {
        // The vertex is on the seam of at least one block, so the shallowest 
        // parent block was moved up at least one level in the tree. This means
        // that the current leaf does not actually have an instance of the 
        // vertex itself, however at least one of our children does.
        NiTerrainDataLeaf* pkChild;
        for (NiUInt32 ui = 0; ui < 4; ++ui)
        {
            pkChild = pkCurrentLeaf->GetChildAt(ui);

            // A child could contain the vertex
            if (pkChild->TreeContains(kVertex))
            {
                ModifyVertexHeightFrom(kVertex, pkChild, fNewHeight, 
                    bModifyParentLOD);   
            }
        }

        // Will propagate changes upwards only if either:
        //  * parent block is not marked as deformable
        //  * modifyParentLOD = true
        if (pkCurrentLeaf->GetQuadMesh()->HasDynamicVertexStreams() && 
            !bModifyParentLOD)
        {
            return;
        }

        // The vertex may not exist at this level, if the 'firstContaining' 
        // leaf was provided for border resolution only
        NiUInt32 uiDivisor = 1 << pkCurrentLeaf->GetNumSubDivisions();
        if (kVertex.GetIndex().x % uiDivisor || 
            kVertex.GetIndex().y % uiDivisor)
        {
            return;
        }
    }

    // Get the associated block. It is safe to assume it exists.
    NiTerrainBlock* pkBlock = pkCurrentLeaf->GetQuadMesh();
    NIASSERT(pkBlock);

    // The vertex is not on the seam of two blocks, therefore only one 
    // parent block needs to be told about this change.
    // We can tell this, since it is actually present in this block.
    if (!pkBlock->HasDynamicVertexStreams())
    {
        // Recycle dynamic streams if we have used all the cache?
        NiDynamicStreamCache* pkStreamCache = 
            m_pkSectorData->GetDynamicStreamCache();
        NiDataStream* pkPositionStream;
        NiDataStream* pkNormalTangentStream;

        if (m_bRecycleDynamicStreams && (
            !pkStreamCache->GetNumAvailable(NiDynamicStreamCache::POSITION) ||
            !pkStreamCache->GetNumAvailable(
                NiDynamicStreamCache::NORMAL_TANGENT)))
        {
            NiTerrainBlock* pkOldestBlock = m_kDynamicBlockQueue.GetHead();
            m_kDynamicBlockQueue.RemoveHead();

            pkPositionStream = 
                pkOldestBlock->GetDynamicPositionLockRead()->GetDataStream();
            pkNormalTangentStream = 
                pkOldestBlock->GetDynamicNormalLockRead()->GetDataStream();

            pkOldestBlock->RemoveDynamicVertexStreams();

            NiTerrainDataLeaf* pkOldestLeaf = GetLeaf(
                pkOldestBlock->GetDataLeaf()->GetBlockID());

            NiUInt32 uiLevel = pkOldestLeaf->GetLevel();

            NiDataStreamLock* pkTangentLock = NULL;
            if (GetConfiguration().IsTangentDataEnabled())
                pkTangentLock = m_kTangentLocks[uiLevel];

            pkOldestLeaf->CreateIterators(
                m_kPositionLocks[uiLevel], 
                m_kNormalLocks[uiLevel], 
                pkTangentLock,
                m_pkUVLock, m_pkIndexLock);
        }
        else
        {    
            pkPositionStream = 
                m_pkSectorData->GetDynamicStreamCache()->RequestStream(
                    NiDynamicStreamCache::POSITION);

            pkNormalTangentStream = 
                m_pkSectorData->GetDynamicStreamCache()->RequestStream(
                    NiDynamicStreamCache::NORMAL_TANGENT);
        }

        m_kDynamicBlockQueue.AddTail(pkBlock);
        
        bool bRes = pkBlock->SetDynamicVertexStreams(
            pkPositionStream, pkNormalTangentStream);

        if (!bRes)
            return;

        NiUInt32 uiLevel = pkCurrentLeaf->GetLevel();

        NiDataStreamLock* pkTangentLock = NULL;
        if (GetConfiguration().IsTangentDataEnabled())
            pkTangentLock = m_kTangentLocks[uiLevel];

        pkCurrentLeaf->CreateIterators(
            m_kPositionLocks[uiLevel], 
            m_kNormalLocks[uiLevel], 
            pkTangentLock,
            m_pkUVLock, m_pkIndexLock);
    }
    
    pkCurrentLeaf->IndexToLocalFast(kVertex.GetIndex(), kLocalIndex);
    float fOldHeight = pkCurrentLeaf->GetHeightAt(kLocalIndex);

    pkBlock->MarkVertexChanged(kLocalIndex, fNewHeight);
    m_bLightingOutdated = true;

    // Notify the data tree that it needs to rebuild bounds
    pkCurrentLeaf->RecenterCountainingBound(kLocalIndex, 
        fOldHeight, fNewHeight);

    // Notify dependant objects of a deformation
    SetShapeChangedLastUpdate(true);
}
//---------------------------------------------------------------------------
void NiTerrainSector::ModifySurfaceMaskFrom(
    const NiSurfaceMaskPixel* pkPixelData,
    NiTerrainDataLeaf* pkFirstLeaf, NiUInt8 ucNewValue, 
    const NiSurface* pkSurfaceToPaint, bool)
{
    // Update the meshes themselves
    NiUInt32 uiPriority;

    // Get the number of surfaces.
    NiUInt32 uiNumSurfaces = pkFirstLeaf->GetSurfaceCount();

    // Work out what surface we want to paint, if its not already given
    if (pkSurfaceToPaint == 0)
    {
        const NiSurfaceMask* pkMask;
        for (NiUInt32 ui = 0; ui < uiNumSurfaces; ++ui)
        {
            pkMask = pkFirstLeaf->GetSurfaceMask((NiUInt16)ui);
            if (pkMask == pkPixelData->GetMask())
            {
                pkSurfaceToPaint = pkFirstLeaf->GetSurface((NiUInt16)ui);
                break;
            }
        }

        if (!pkSurfaceToPaint)
            return;
    }

    if (pkFirstLeaf->GetSurfacePriority(pkSurfaceToPaint, uiPriority))
    {
        NiUInt32 uiX = pkPixelData->GetX();
        NiUInt32 uiY = pkPixelData->GetY();

        NiSurfaceMask* pkThisMask = &m_pkSurfaceMaskStack[
            pkPixelData->GetMask()->m_uiMaskIndex];

        NiUInt32 uiMaskSum = 0;
        NiSurfaceMask* pkMask = NULL;
        for (NiUInt32 ui = 0; ui < m_uiSurfaceMaskStackPos; ++ui)
        {
            pkMask = &m_pkSurfaceMaskStack[ui];
            if (pkThisMask != pkMask)
            {
                NiUInt8 ucValue = pkMask->GetAt(uiX, uiY);
                uiMaskSum += ucValue;
            }
        }

        // Make sure we are not painting black
        if (uiMaskSum > 0 || ucNewValue > 0)
        {
            pkThisMask->SetAt(uiX, uiY, ucNewValue);
            pkFirstLeaf->GetQuadMesh()->MarkSurfaceMasksChanged();
        }
    }
    else
    {
        // Mask doesn't exist, Error
        NIASSERT(!"Attempted modification of invalid mask");
    }
}
//---------------------------------------------------------------------------
NiSurfaceMask* NiTerrainSector::CreateMask(NiPixelData* pkPixelData)
{
    // Is the stack full? we will need to make it bigger.
    if (!m_pkSurfaceMaskStack || 
        m_uiSurfaceMaskStackPos >= m_uiSurfaceMaskStackSize) 
    {
        InitMaskStack((NiUInt32)(m_uiSurfaceMaskStackSize * 1.2f) + 1);
    }

    if (pkPixelData->GetWidth() != pkPixelData->GetHeight())
    {
        NIASSERT(!"Width and height of mask must match.");
        return 0;
    }

    if (!NiIsPowerOf2(pkPixelData->GetWidth()))
    {
        NIASSERT(!"Terrain mask dimensions must be a power of 2.");
        return 0;
    }

    // Create the texture
    NiUInt32 uiWidth = pkPixelData->GetWidth();
    NiUInt32 uiSize = uiWidth * uiWidth;
    NiUInt8* pucMask = m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_pucMask;
    if(!pucMask)
    {
        pucMask = NiAlloc(NiUInt8, uiSize);
    } 
    else if (m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_uiWidth != 
        uiWidth)
    {
        pucMask = (NiUInt8*)(NiRealloc(pucMask, uiSize * sizeof (NiUInt8)));
    }

    int iStride = pkPixelData->GetPixelStride();
    const NiUInt8* pucPixels = pkPixelData->GetPixels();

    NiUInt32 uiMaxY = uiSize - uiWidth;
    NiUInt32 uiMaxX = uiWidth - 1;

    for (NiUInt32 uiY = 0; uiY <= uiMaxY; uiY += uiWidth) 
    {
        for (NiUInt32 uiX = 0; uiX <= uiMaxX; ++uiX) 
        {
            pucMask[(uiMaxY-uiY) + uiX] = pucPixels[(uiY + uiX) * iStride];
        }
    }

    m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_pucMask = pucMask;
    m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_uiWidth = uiWidth;

    return &m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos++];
}
//---------------------------------------------------------------------------
NiSurfaceMask* NiTerrainSector::CreateMask(NiUInt32 uiWidth)
{
    if (!NiIsPowerOf2(uiWidth))
    {
        NIASSERT(!"Terrain mask dimensions must be a power of 2.");
        return 0;
    }

    // Is the stack full? we will need to make it bigger.
    if (!m_pkSurfaceMaskStack ||
        m_uiSurfaceMaskStackPos >= m_uiSurfaceMaskStackSize) 
    {
        InitMaskStack((NiUInt32)(m_uiSurfaceMaskStackSize * 1.2f) + 1);
    }

    // Create the texture
    NiUInt32 uiSize = uiWidth * uiWidth;
    NiUInt8* pcMask = m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_pucMask;

    if(!pcMask)
    {
        pcMask = NiAlloc(NiUInt8, uiSize);
    } 
    else if (m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_uiWidth != 
        uiWidth)
    {
        pcMask = (NiUInt8*)(NiRealloc(pcMask, uiSize * sizeof (NiUInt8)));
    }

    // If this is our first surface, fill with 255, otherwise 0
    NiUInt8 cDefaultMask = (m_uiSurfaceMaskStackPos) ? 0 : 255;
    memset(pcMask, cDefaultMask, uiSize * sizeof(NiUInt8));
    
    m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_pucMask = pcMask;
    m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_uiWidth = uiWidth;

    return &m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos++];
}
//---------------------------------------------------------------------------
NiSurfaceMask* NiTerrainSector::GetMask(NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_uiSurfaceMaskStackPos);
    return &m_pkSurfaceMaskStack[uiIndex];
}
//---------------------------------------------------------------------------
void NiTerrainSector::InitMaskStack(NiUInt32 uiStackSize)
{
    // Must not reduce the size of the stack to below number of things in it
    NIASSERT(m_uiSurfaceMaskStackPos < uiStackSize);

    if (uiStackSize == 0) 
    {
        RemoveAllMasks();
        NiFree(m_pkSurfaceMaskStack);
        m_pkSurfaceMaskStack = 0;
    } 
    else 
    { 
        size_t stMaskSize = sizeof(NiSurfaceMask);          

        NiUInt8* pusOldAddress = (NiUInt8*)(m_pkSurfaceMaskStack);
        m_pkSurfaceMaskStack = (NiSurfaceMask*)NiRealloc(
            m_pkSurfaceMaskStack, 
            uiStackSize * stMaskSize);
        
        NiUInt8* pusNewAddress = (NiUInt8*)(m_pkSurfaceMaskStack);

        for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize() 
            && pusOldAddress != 0; ++ui)
        {
            m_kLeafArray.GetAt(ui)->UpdateSurfaceMaskAddresses(pusOldAddress,
                pusNewAddress);
        }

        // Fill the stack with default data
        for (NiUInt32 ui = m_uiSurfaceMaskStackPos; ui < uiStackSize; ++ui)
        {
            m_pkSurfaceMaskStack[ui] = NiSurfaceMask();
            m_pkSurfaceMaskStack[ui].m_uiMaskIndex = ui;
        }
    }

    m_uiSurfaceMaskStackSize = uiStackSize;

    // Initialize the cache that stores the highest mask resolution in use, at
    // each detail level
    if (!m_pfBestPixelSpacingCache)
    {
        NiUInt32 uiNumLOD = m_pkSectorData->GetNumLOD();
        m_pfBestPixelSpacingCache = NiAlloc(float, uiNumLOD + 1);
        for (NiUInt32 ui = 0; ui < uiNumLOD; ++ui)
        {
            m_pfBestPixelSpacingCache[ui] = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::SubscribeToDXDeviceResetNotification()
{
    m_uiDXDeviceResetCallbackIndex = 0;
    m_bRegisteredDXDeviceResetCallback = false;

#ifdef WIN32
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    if (pkRenderer)
    {
        m_uiDXDeviceResetCallbackIndex = pkRenderer->AddResetNotificationFunc(
            (NiDX9Renderer::RESETNOTIFYFUNC)&HandleDXDeviceReset, this);
        m_bRegisteredDXDeviceResetCallback = true;
    }
#endif
}
//---------------------------------------------------------------------------
void NiTerrainSector::UnsubscribeToDXDeviceResetNotification()
{
#ifdef WIN32
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    if (pkRenderer && m_bRegisteredDXDeviceResetCallback)
    {
        pkRenderer->RemoveResetNotificationFunc(
            m_uiDXDeviceResetCallbackIndex);
    }
#endif
}
//---------------------------------------------------------------------------
bool NiTerrainSector::HandleDXDeviceReset(bool bBeforeReset, void* pkVoid)
{
    if (!bBeforeReset) // Wait until after device reset
    {
        NiTerrainSector* pkSector = (NiTerrainSector*)pkVoid;
        NIASSERT(pkSector);

        pkSector->RegenerateBlendMaskTextures();
    }
    return true;
}
//---------------------------------------------------------------------------
void NiTerrainSector::RegenerateBlendMaskTextures()
{
    // Tell all blocks that their masks have changed to trigger a rebuild
    // of their blend mask textures. 
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
    {
        NiTerrainDataLeaf* pkLeaf = m_kLeafArray.GetAt(ui);
        NiTerrainBlock* pkBlock = pkLeaf->GetQuadMesh();

        pkBlock->MarkSurfaceMasksChanged(false);
    }

    if (m_pkQuadMesh)
    {
        m_pkQuadMesh->Update(0.0f);    
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::EnsureMaskExists(const NiBound& kBound, 
    const NiSurface* pkSurface, NiUInt32 uiDetailLevel,
    NiTerrainDataLeaf* pkCurrentLeaf)
{
    // If no data leaf was given, go to the top of the tree
    if (pkCurrentLeaf == 0)
    {
        pkCurrentLeaf = m_pkQuadData;
        NIASSERT(pkCurrentLeaf);
    }

    // Is this leaf at the correct level?
    if (pkCurrentLeaf->GetLevel() == uiDetailLevel)
    {
        // We require a mask for this quad leaf. Does it exist?
        if (!pkCurrentLeaf->GetSurfaceMask(pkSurface))
        {
            
            if (pkCurrentLeaf->GetSurfaceCount() >= 4)
            {
                return;
            }

            // It does not exist, so add it.

            // What size mask should it be?
            NiUInt32 uiWidth = 0;
            NiTerrainDataLeaf* pkMasterLeaf = pkCurrentLeaf;
            while (pkMasterLeaf)
            {
                uiWidth = pkMasterLeaf->GetSurfaceMaskWidth();
                if (uiWidth)
                    break;

                pkMasterLeaf = pkMasterLeaf->GetParent();
            }

            if (!uiWidth)
                uiWidth = m_uiDefaultMaskSize;

            // Create the mask data
            NiSurfaceMask* pkMask = CreateMask(uiWidth);

            // Now add the mask to the quad leaf.
            pkCurrentLeaf->AddSurfaceMask(pkSurface, pkMask);

            // Ensure that the surface is compiled
            if (!pkSurface->IsCompiled())
            {
                NIASSERT(!"Attempt to paint a non compiled surface");
            }
        }
    }
    else
    {
        // Make sure a valid level was given
        NIASSERT(pkCurrentLeaf->GetLevel() < uiDetailLevel);

        // Check any child leaves that are within the bound
        if (!pkCurrentLeaf->GetChildAt(0))
        {
            return;
        }

        NiTerrainDataLeaf* pkChild;
        for (NiUInt32 ui = 0; ui < 4; ++ui)
        {
            pkChild = pkCurrentLeaf->GetChildAt(ui);
            if (NiTerrainUtils::TestBound2D(kBound, 
                pkCurrentLeaf))
            {
                EnsureMaskExists(kBound, pkSurface, uiDetailLevel, pkChild);
            }
        }
    }    
}
//---------------------------------------------------------------------------
void NiTerrainSector::MarkSurfaceChanged(const NiSurface* pkSurface)
{
    NiTerrainDataLeaf* pkLeaf;
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
    {
        pkLeaf = m_kLeafArray[ui];
        if (pkLeaf->GetSurfaceMask(pkSurface))
            pkLeaf->GetQuadMesh()->MarkSurfaceMasksChanged(true);
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::RemoveSurface(const NiSurface* pkSurface)
{
    NiTerrainDataLeaf* pkLeaf;
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
    {
        pkLeaf = m_kLeafArray[ui];

        // Get the stack position of this mask:
        const NiSurfaceMask* pkLeafSurfaceMask = 
            pkLeaf->GetSurfaceMask(pkSurface);
        
        NiUInt32 uiPriority; 
        pkLeaf->GetSurfacePriority(pkSurface, uiPriority);
        const NiSurfaceMask* pkReplacingMask = NULL;
        
        bool bBaseLayerRemoved = false;
        if (uiPriority == pkLeaf->GetSurfaceCount() - 1 && uiPriority >= 1)
        {
            pkReplacingMask = 
                pkLeaf->GetSurfaceMask(pkLeaf->GetSurface(uiPriority - 1));
            bBaseLayerRemoved = true;
        }
        else
        {
            pkReplacingMask = 
                pkLeaf->GetSurfaceMask(pkLeaf->GetSurface(
                pkLeaf->GetSurfaceCount() - 1));
        }

        if(pkLeafSurfaceMask && pkLeaf->RemoveSurfaceMask(pkSurface, false))
        {   
            // Shift all the entries in the stack
            NiUInt32 uiStartStackPos = pkLeafSurfaceMask->m_uiMaskIndex;
            NiSurfaceMask kTempMask = m_pkSurfaceMaskStack[uiStartStackPos];
            NiUInt32 uiStackPos = 0;
            NiSurfaceMask kNewBottomMask;
            
            
            // check whether we are removing the bottom mask or not
            if (pkReplacingMask)
            {
                uiStackPos = pkReplacingMask->m_uiMaskIndex;                
                kNewBottomMask = 
                    m_pkSurfaceMaskStack[uiStackPos];
            }
            
            
            for (NiUInt32 uiX = 0; uiX < kTempMask.GetWidth(); ++uiX)
            {
                for (NiUInt32 uiY = 0; uiY < kTempMask.GetWidth(); ++uiY)
                {
                    NiUInt8 ucBottomValue = kTempMask.GetAt(uiX, uiY);
                    if (pkReplacingMask)
                    {
                        // copy values if we are deleting the bottom layer                        
                        NiUInt8 ucUpperValue = kNewBottomMask.GetAt(uiX, uiY);
                        kNewBottomMask.SetAt(uiX, uiY, NiClamp(ucUpperValue + 
                            ucBottomValue, 0, 255)); 
                    }
                    
                    // Reset the deleted mask values
                    kTempMask.SetAt(uiX, uiY, 0);                   
                }
            }            

            // Adjust the next position in the stack:
            --m_uiSurfaceMaskStackPos;

            for (NiUInt32 uiIndex = uiStartStackPos; 
                uiIndex < m_uiSurfaceMaskStackPos; ++uiIndex)
            {
                m_pkSurfaceMaskStack[uiIndex] = 
                    m_pkSurfaceMaskStack[uiIndex+1];
                m_pkSurfaceMaskStack[uiIndex].m_uiMaskIndex = uiIndex;
            }

            m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos] = kTempMask;
            m_pkSurfaceMaskStack[m_uiSurfaceMaskStackPos].m_uiMaskIndex = 
                m_uiSurfaceMaskStackPos;
        }
    }

    for (NiUInt32 ui = 0; ui <= m_pkSectorData->GetNumLOD(); ++ui)
        RecalculateBestPixelSpacing(ui);
}
//---------------------------------------------------------------------------
void NiTerrainSector::RemoveAllMasks()
{
    if (!m_pkSurfaceMaskStack)
        return;

    // Need to remove references
    for (NiUInt32 ui = 0; ui < m_uiSurfaceMaskStackSize; ++ui)
    {
        NiFree(m_pkSurfaceMaskStack[ui].m_pucMask);
        m_pkSurfaceMaskStack[ui].m_pucMask = 0;
    }

    m_uiSurfaceMaskStackPos = 0;
}
//---------------------------------------------------------------------------
void NiTerrainSector::RecalculateBestPixelSpacing(NiUInt32 uiDetailLevel)
{
    if (!m_pfBestPixelSpacingCache)
        return;

    // Reset the spacing
    m_pfBestPixelSpacingCache[uiDetailLevel] = NI_INFINITY;

    // The width of each leaf at this level, in index space. A power of 2
    NiUInt32 uiVertexSpacing = 1 << (m_pkSectorData->GetNumLOD() - 
        uiDetailLevel);
    NiUInt32 uiLeafWidth = m_pkSectorData->GetBlockSize() * uiVertexSpacing;
    
    NiUInt32 uiSectorSize = m_pkSectorData->GetSectorSize();

    NiTerrainDataLeaf* pkLeaf = 0;
    NiTerrainVertex kVertex;
    NiIndex kIndex;

    // Probe the vertex array for references to parent leaves. We will need to
    // only probe vertices that are first introduced at this level.
    for (NiUInt32 y = 0; y < uiSectorSize; y += uiLeafWidth)
    {
        kIndex.y = y + uiVertexSpacing;
        for (NiUInt32 x = 0; x < uiSectorSize; x += uiLeafWidth)
        {
            kIndex.x = x + uiVertexSpacing;
            
            GetVertexAt(kVertex, kIndex, uiDetailLevel);

            pkLeaf = kVertex.GetFirstContainingLeaf();
            NIASSERT(pkLeaf);

            UpdateBestPixelSpacing(pkLeaf);
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::UpdateBestPixelSpacing(const NiTerrainDataLeaf* pkLeaf)
{
    if (pkLeaf->GetSurfaceCount() == 0)
        return;

    // What is the pixel spacing of the given leaf?
    float fLeafWidth = float(m_pkSectorData->GetBlockSize());
    float fSpacing = fLeafWidth / float(pkLeaf->GetSurfaceMaskWidth());
    float fBestSpacing =  m_pfBestPixelSpacingCache[pkLeaf->GetLevel()];

    m_pfBestPixelSpacingCache[pkLeaf->GetLevel()] = 
        NiMin(fSpacing, fBestSpacing);
}
//---------------------------------------------------------------------------
void NiTerrainSector::ReformatData(
    NiUInt32 uiBlockSize, NiUInt32 uiNumLOD)
{    
    // Have we loaded any data yet?
    if (m_pkQuadData)
    {
        UnloadLODData(0);
        DestroySupportingData();
    }    

    NiUInt32 uiOldNumLOD = GetSectorData()->GetNumLOD();

    m_pkSectorData->SetBlockSize(uiBlockSize);
    m_pkSectorData->SetNumLOD(uiNumLOD);
    
    for (NiInt32 uiLevel = m_kPositionLocks.GetSize() - 1;
        uiLevel > (NiInt32)uiNumLOD; --uiLevel)
    {
        NiDelete m_kPositionLocks[uiLevel];
        NiDelete m_kNormalLocks[uiLevel];
        NiDelete m_kTangentLocks[uiLevel];
    }
            
    m_kPositionLocks.SetSize(uiNumLOD + 1);
    m_kNormalLocks.SetSize(uiNumLOD + 1);
    m_kTangentLocks.SetSize(uiNumLOD + 1);

    // Make sure the Leaf offsets information is accurate with the 
    // change in the number of LODs
    m_puiLeafOffsets = (NiUInt32*)NiRealloc(m_puiLeafOffsets, 
        sizeof(NiUInt32) * (m_pkSectorData->GetNumLOD() + 1));
    m_puiLeafOffsets[0] = 0;

    for (NiUInt32 ui = 1; ui <= m_pkSectorData->GetNumLOD(); ++ui)
    {
        NiUInt32 uiBlocksPerRow = 1 << (ui - 1);
        NiUInt32 uiVal = m_puiLeafOffsets[ui - 1] + 
            uiBlocksPerRow * uiBlocksPerRow;
        m_puiLeafOffsets[ui] = uiVal;
    }

    if (GetSectorData()->GetTargetLoadedLOD() 
        == (NiInt32)uiOldNumLOD)
    {
        m_pkSectorData->SetTargetLoadedLOD(uiNumLOD, false);
    }

    // Recreate the indexes
    BuildMesh();
}
//---------------------------------------------------------------------------
bool NiTerrainSector::RequiresLightingRebuild() const
{
    return m_bLightingOutdated;
}
//---------------------------------------------------------------------------
void NiTerrainSector::GetOutdatedLightingLeafList(
    NiTPrimitiveArray<NiTerrainDataLeaf*>* pkChanged, bool bAllLeaves)
{
    NiTerrainDataLeaf* pkLeaf;    
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
    {
        pkLeaf = m_kLeafArray.GetAt(ui);

        if (bAllLeaves)
            m_kLeafArray[ui]->GetQuadMesh()->RequestLightingRebuild();
        else if (!pkLeaf->GetQuadMesh()->RequiresLightingRebuild())
            continue;

        if (m_kLeafArray[ui]->GetQuadMesh()->HasDynamicVertexStreams())
        {
            // Lock any relevant dynamic streams
            m_kLeafArray[ui]->GetQuadMesh()->LockDynamicStream(
                NiTerrainBlock::STREAM_POSITION, NiDataStream::LOCK_WRITE);
            m_kLeafArray[ui]->GetQuadMesh()->LockDynamicStream(
                NiTerrainBlock::STREAM_NORMALTANGENT, 
                NiDataStream::LOCK_WRITE);
        }

        pkChanged->Add(pkLeaf);
    }

    m_bLightingOutdated = false;
}
//---------------------------------------------------------------------------
void NiTerrainSector::RebuildAllLighting()
{
    NiTPrimitiveArray<NiTerrainDataLeaf*> kChanged;
    GetOutdatedLightingLeafList(&kChanged, true);   

    // Discover normals and tangents
    CalculateNormalsTangents(kChanged);

    // Create morphing values
    CalculateGeoMorph(kChanged);
}
//---------------------------------------------------------------------------
void NiTerrainSector::RebuildChangedLighting()
{
    NiTPrimitiveArray<NiTerrainDataLeaf*> kChanged;
    GetOutdatedLightingLeafList(&kChanged, false);   
    if (kChanged.GetSize() == 0)
        return;

    // Discover normals and tangents
    CalculateNormalsTangents(kChanged);

    // Create morphing values
    CalculateGeoMorph(kChanged);
}
//---------------------------------------------------------------------------
void NiTerrainSector::RebuildChangedMorph()
{
    NiTerrainDataLeaf* pkLeaf;
    NiTPrimitiveArray<NiTerrainDataLeaf*> kChanged;
    
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
    {
        pkLeaf = m_kLeafArray.GetAt(ui);

        if (!pkLeaf->GetQuadMesh()->RequiresMorphRebuild())
            continue;

        m_kLeafArray[ui]->GetQuadMesh()->LockDynamicStream(
            NiTerrainBlock::STREAM_NORMALTANGENT, NiDataStream::LOCK_WRITE);

        kChanged.Add(pkLeaf);
    }

    if (kChanged.GetSize() == 0)
        return;

    CalculateGeoMorph(kChanged);
}
//---------------------------------------------------------------------------
bool NiTerrainSector::CreateIndexStream()
{
    NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();
    NiUInt32 uiLockType = NiDataStream::LOCK_WRITE;
    NiUInt32 uiBlockWidth = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiNumVerts =  uiBlockWidth* uiBlockWidth;

    // Delete any previous copy of the index stream
    NiDelete m_pkIndexLock;

    // NOTE: Each block will only ever have a maximum of 2 sides which need 
    // stitching and those two sides must be adjoining. This allows us to use
    // up to 65x65 blocks with a 16 bit index buffer
    
    // We will support both 16 and 32bit index buffers, using smallest where 
    // possible

    // [indices per tri] * [tri's per square] * [num squares]
    NiUInt32 auiNumIndicesPerNumBorders[3];
    auiNumIndicesPerNumBorders[0] = 3 * 2 * uiBlockSize * uiBlockSize;
    auiNumIndicesPerNumBorders[1] = 3 * 2 * uiBlockSize * (uiBlockSize-1) + 
        3 * (3 * (uiBlockSize / 2));
    auiNumIndicesPerNumBorders[2] = 3 * 2 * (uiBlockSize-1) * (uiBlockSize-1) +
        3 * (6 * (uiBlockSize / 2) - 2);

    // 1 set of full LOD
    NiUInt32 uiNumIndices = 1 * auiNumIndicesPerNumBorders[0];
    // 4 sets of One Border
    uiNumIndices += 4 * auiNumIndicesPerNumBorders[1];
    // 4 sets of Two Borders
    uiNumIndices += 4 * auiNumIndicesPerNumBorders[2];
    
    // If we can, support 16 bit index buffer
    NiDataStreamElement::Format eIndexType;
    if (uiNumVerts > USHRT_MAX)
    {
#if defined(_WII)
        NIASSERT(!"The Wii platform does not support 32 bit index buffers");
#endif
        eIndexType = NiDataStreamElement::F_UINT32_1;
        m_bUsingShortIndexBuffer = false;
    }
    else
    {
        eIndexType = NiDataStreamElement::F_UINT16_1;
        m_bUsingShortIndexBuffer = true;
    }
    
    NiDataStream* pkIndexStream = NiDataStream::CreateSingleElementDataStream(
        eIndexType,
        uiNumIndices,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC |
        NiDataStream::ACCESS_CPU_READ,
        NiDataStream::USAGE_VERTEX_INDEX,
        0, true, false);

    // Get a lock to the index buffer
    m_pkIndexLock = NiNew NiDataStreamLock(pkIndexStream, 0, 
        (NiUInt8)uiLockType);
    
    if (!pkIndexStream->GetLocked() || 
        pkIndexStream->GetCount(0) != uiNumIndices)
    {
        NIASSERT(!"Failed to create valid index stream");

        NiDelete m_pkIndexLock;
        NiDelete pkIndexStream;
        m_pkIndexLock = 0;

        return false;
    }

    // Create the regions.
    pkIndexStream->RemoveAllRegions();
    NiDataStream::Region* pkRegion;
    NiUInt32 uiRegionStart = 0;
    for (NiUInt8 i = 0; i < NiTerrainSectorData::NUM_INDEX_REGIONS; ++i)
    {
        pkRegion = m_pkSectorData->GetIndexRegion(i);
        if (!pkRegion)
        {
            pkRegion = NiNew NiDataStream::Region();
            m_pkSectorData->SetIndexRegion(i, pkRegion);
        }

        NiUInt32 uiRegionSize;
        if (i > 4)
            uiRegionSize = auiNumIndicesPerNumBorders[2];
        else if (i > 0)
            uiRegionSize = auiNumIndicesPerNumBorders[1];
        else
            uiRegionSize = auiNumIndicesPerNumBorders[0];

        pkRegion->SetRange(uiRegionSize);

        pkRegion->SetStartIndex(uiRegionStart);
        uiRegionStart += uiRegionSize;

        pkIndexStream->AddRegion(*pkRegion);
    }

    m_pkSectorData->SetStaticIndexStream(pkIndexStream);

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::CreateUVStream()
{
    NiUInt32 uiBlockWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiVertsPerBlock = uiBlockWidthInVerts * uiBlockWidthInVerts;
    NiUInt8 ucLockType = NiDataStream::LOCK_WRITE;

    NiDataStream* pkUVStream = NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_FLOAT32_2, 
        uiVertsPerBlock,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX,
        0, true, false);
    m_pkUVLock = NiNew NiDataStreamLock(pkUVStream, 0, ucLockType);
    m_pkSectorData->SetStaticUVStream(pkUVStream);

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::CreatePNTStream(NiUInt32 uiMaxDetailLevel)
{
    // Initialize the detail level to load to if required:
    if(uiMaxDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
        uiMaxDetailLevel = GetSectorData()->GetNumLOD();

    // Figure out what LODs we have already created:
    NiUInt32 uiNextLevel = GetSectorData()->GetHighestLoadedLOD() + 1;

    // Initialize the values:
    NiUInt32 uiNumBlocks = m_pkSectorData->GetNumBlocks();
    NiUInt32 uiLockType = NiDataStream::LOCK_WRITE;
    NiUInt32 uiBlockWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiVertsPerBlock = uiBlockWidthInVerts * uiBlockWidthInVerts;
    
    // Define the layout of the streams
    // POSITION
    NiDataStreamElementSet kElementSetV;
    kElementSetV.AddElement(GetPositionStreamFormat());
    
    // NORMAL/TANGENT
    NiDataStreamElementSet kElementSetNT;
    kElementSetNT.AddElement(GetNormalStreamFormat());
    if (GetConfiguration().IsTangentDataEnabled())
        kElementSetNT.AddElement(GetTangentStreamFormat());

    if (m_pkSectorData->InToolMode())
    {
        // Check if an old stream cache exists:
        if (m_pkSectorData->GetDynamicStreamCache())
        {
            return true;
        }

        // Create a new stream cache
        bool bRes = CreateDeformationCache(
            uiNumBlocks, kElementSetV, kElementSetNT);

        if (!bRes)
        {
            NIASSERT(!"Failed to create deformation cache");
            return false;
        }
    }
    else
    {
        // Create a static stream for each LOD Level
        for (NiUInt32 uiLevel = uiNextLevel; uiLevel < uiMaxDetailLevel + 1; 
            ++uiLevel)
        {
            uiNumBlocks = 1 << uiLevel;
            uiNumBlocks *= uiNumBlocks;
            NiUInt32 uiNumVertices = uiVertsPerBlock * uiNumBlocks;

            // These -should- be zero
            NIASSERT(!m_kPositionLocks[uiLevel]);

            NiDelete m_kPositionLocks[uiLevel];
            NiDelete m_kNormalLocks[uiLevel];
            NiDelete m_kTangentLocks[uiLevel];

            // Create the streams    
            NiDataStream* pkVertexStream;
            NiDataStream* pkNormalTangentStream;
            if (m_pkSectorData->GetStaticPositionStream(uiLevel))
            {
                pkVertexStream = m_pkSectorData->GetStaticPositionStream(
                    uiLevel);
                pkVertexStream->Resize(uiNumVertices);
                pkNormalTangentStream = 
                    m_pkSectorData->GetStaticNormalTangentStream(uiLevel);
                pkNormalTangentStream->Resize(uiNumVertices);
            }
            else
            {
                pkVertexStream = NiDataStream::CreateDataStream(
                    kElementSetV, 
                    uiNumVertices,
                    NiDataStream::ACCESS_GPU_READ |
                    NiDataStream::ACCESS_CPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_STATIC,
                    NiDataStream::USAGE_VERTEX,
                    false);
    
                pkNormalTangentStream = NiDataStream::CreateDataStream(
                    kElementSetNT, 
                    uiNumVertices,
                    NiDataStream::ACCESS_CPU_READ |
                    NiDataStream::ACCESS_GPU_READ |
                    NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
                    NiDataStream::USAGE_VERTEX,
                    false);
            }
            
            // Create regions
            NiUInt32 uiRegionIndex = 0;
            for (NiUInt32 ui = 0; ui < uiNumBlocks; ++ui)
            {
                NiDataStream::Region kRegion;
                kRegion.SetStartIndex(uiRegionIndex);
                kRegion.SetRange(uiVertsPerBlock);
                uiRegionIndex += uiVertsPerBlock;
    
                pkVertexStream->AddRegion(kRegion);
                pkNormalTangentStream->AddRegion(kRegion);
            }
            
            // Get locks, and validate the vertex streams
            NiDataStreamLock* pkPositionLock = NiNew NiDataStreamLock(
                pkVertexStream, 0, (NiUInt8)uiLockType);
            m_kPositionLocks.SetAtGrow(uiLevel, pkPositionLock);                
    
            NiDataStreamLock* pkNormalLock = NiNew NiDataStreamLock(
                pkNormalTangentStream, 0, (NiUInt8)uiLockType);
            m_kNormalLocks.SetAtGrow(uiLevel, pkNormalLock);    
    
            NiDataStreamLock* pkTangentLock = NULL;
            if (GetConfiguration().IsTangentDataEnabled())
            {
                pkTangentLock = NiNew NiDataStreamLock(
                    pkNormalTangentStream, 
                    sizeof(float) * GetConfiguration().GetNumNormalComponents(), 
                    (NiUInt8)uiLockType);
            }

            m_kTangentLocks.SetAtGrow(uiLevel, pkTangentLock);
    
            if (!pkPositionLock->IsLocked() || 
                pkVertexStream->GetTotalCount() != uiNumVertices ||
                !pkNormalLock->IsLocked() || 
                (GetConfiguration().IsTangentDataEnabled() && 
                    !pkTangentLock->IsLocked()) ||
                pkNormalTangentStream->GetTotalCount() != uiNumVertices)
            {
                NIASSERT(!"Failed to create streams");
    
                NiDelete m_kPositionLocks[uiLevel];
                NiDelete m_kNormalLocks[uiLevel];
                NiDelete m_kTangentLocks[uiLevel];
                NiDelete pkVertexStream;
                NiDelete pkNormalTangentStream;
                m_kPositionLocks[uiLevel] = NULL;
                m_kNormalLocks[uiLevel] = NULL;
                m_kTangentLocks[uiLevel] = NULL;
    
                return false;
            }
    
            m_pkSectorData->SetStaticVertexStream(uiLevel, pkVertexStream);
            m_pkSectorData->SetStaticNormalTangentStream(uiLevel, 
                pkNormalTangentStream);
        }

        //=======================//
        // Dynamic Block Streams //
        //=======================//

        if (m_pkSectorData->GetDeformable())
        {
            // Check if an old stream cache exists:
            if (m_pkSectorData->GetDynamicStreamCache())
                return true;
            
            CreateDeformationCache(uiNumBlocks / 5, kElementSetV, 
                kElementSetNT);
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::CreateStreams(NiUInt32 uiMaxDetailLevel)
{
    //==============//
    // Index Stream //
    //==============//
    if (!m_pkIndexLock)
    {
        if(!CreateIndexStream())
            return false;
    }

    //=====================================//
    // Position and Normal/Tangent streams //
    //=====================================//
    if (!CreatePNTStream(uiMaxDetailLevel))
        return false;

    //==================//
    // Shared UV Stream //
    //==================//
    if (!m_pkUVLock)
    {
        if (!CreateUVStream())
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::CreateDeformationCache(NiUInt32 uiCacheSize,
    const NiDataStreamElementSet& kElementSetV, 
    const NiDataStreamElementSet& kElementSetNT)
{    
    // Should not recreate the stream cache if it already exists, since blocks
    // may already be using the streams. Similarly, we can't change the element
    // sets in the existing cache to match the new sets.
    NIASSERT(!m_pkSectorData->GetDynamicStreamCache());
    if (m_pkSectorData->GetDynamicStreamCache())
        return false;

    bool bRes = true;

    NiUInt32 uiVertsPerBlock = m_pkSectorData->GetBlockWidthInVerts() * 
        m_pkSectorData->GetBlockWidthInVerts();

    NiDynamicStreamCache* pkDynamicStreamsCache = NiNew NiDynamicStreamCache(
        uiVertsPerBlock);

    bRes &= pkDynamicStreamsCache->InitializeStreamCache(
        NiDynamicStreamCache::POSITION,
        uiCacheSize,
        50,
        kElementSetV,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX);

    bRes &= pkDynamicStreamsCache->InitializeStreamCache(
        NiDynamicStreamCache::NORMAL_TANGENT,
        uiCacheSize,
        50, 
        kElementSetNT,
        NiDataStream::ACCESS_GPU_READ |
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_VERTEX);

    if (bRes)
    {
        m_pkSectorData->SetDynamicStreamCache(pkDynamicStreamsCache);
    }
    else
    {
        NiDelete pkDynamicStreamsCache;
        pkDynamicStreamsCache = NULL;
    }

    return bRes;
}
//---------------------------------------------------------------------------
void NiTerrainSector::BuildMesh(NiUInt32 uiMaxDetailLevel)
{
    // Initialize the Default Detail level if required:
    if(uiMaxDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiMaxDetailLevel = GetSectorData()->GetNumLOD();
    }

    NiUInt32 uiBeginIndex = m_puiLeafOffsets[
        GetSectorData()->GetHighestLoadedLOD() + 1];
    NiUInt32 uiEndIndex = m_kLeafArray.GetSize();

    if (m_pkQuadData)
    {
        // We will be assigning new regions for all the blocks
        for (NiUInt32 ui = uiBeginIndex; ui < uiEndIndex; ++ui)
        {
            if (!m_kLeafArray[ui])
                continue;

            m_kLeafArray[ui]->GetQuadMesh()->CreateMesh();
        }

        UpdateWorldData();
        UpdateProperties();
        UpdateEffects();
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::CreateBlockHierarchy()
{
    // Iterate over the leaves that need creating and attaching to the tree3
    NiInt32 iLastLevel = GetSectorData()->GetHighestLoadedLOD();
    NiUInt32 uiBeginIndex = m_puiLeafOffsets[iLastLevel + 1];

    // (Only create these blocks for the the first LOD created
    // the rest of the levels will be done using recursion)
    NiUInt32 uiEndIndex = m_kLeafArray.GetSize();
    if (iLastLevel + 2 <= (NiInt32)GetSectorData()->GetNumLOD())
    {
        uiEndIndex = GetLeafOffset(iLastLevel + 2);
    }

    for (NiUInt32 uiBlockID = uiBeginIndex; uiBlockID < uiEndIndex; ++uiBlockID)
    {
        NiTerrainDataLeaf* pkLeaf = GetLeaf(uiBlockID);
        NiTerrainBlock* pkBlock = NiNew NiTerrainBlock(m_pkSectorData);

        if (uiBlockID == 0)
        {
            m_pkQuadMesh = pkBlock;
            AttachChild(m_pkQuadMesh);
        }

        pkBlock->CreateHierarchy(pkLeaf);
        pkBlock->UpdateEffects();
        pkBlock->RequestUpdate();
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::BuildData(NiUInt32 uiMaxDetailLevel)
{
    // Initialize the Default Detail level if required:
    if(uiMaxDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiMaxDetailLevel = GetSectorData()->GetNumLOD();
    }

    // Create the shared index buffer
    CalculateIndices(m_pkIndexLock);

    // Create some shared UV coordinates
    CalculateUV(m_pkUVLock);

    // Build required lighting: (And hence morph data)
    if (m_pkTerrain)
        m_pkTerrain->GetInteractor()->RebuildLighting();
    else
        RebuildChangedLighting();

    // Calculate any morph data required:
    RebuildChangedMorph();

    // Create some read locks on the data, that will persist for the life of
    // the mesh
    NiUInt8 uiLockType = NiDataStream::LOCK_READ;

    for (NiUInt32 uiLevel = 0; uiLevel < m_kPositionLocks.GetSize(); ++uiLevel)
    {
        // Delete locks
        NiDelete m_kPositionLocks[uiLevel];
        NiDelete m_kNormalLocks[uiLevel];
        NiDelete m_kTangentLocks[uiLevel];

        m_kPositionLocks[uiLevel] = NULL; 
        m_kNormalLocks[uiLevel] = NULL;
        m_kTangentLocks[uiLevel] = NULL;

        // Create new read locks:
        NiDataStream* pkPositionStream = 
            m_pkSectorData->GetStaticPositionStream(uiLevel);
        NiDataStream* pkNormalTangentStream = 
            m_pkSectorData->GetStaticNormalTangentStream(uiLevel);
        if (pkPositionStream)
        {
            m_kPositionLocks[uiLevel] = NiNew NiDataStreamLock(
                pkPositionStream, 0, uiLockType);
        }
        if (pkNormalTangentStream)
        {
            m_kNormalLocks[uiLevel] = NiNew NiDataStreamLock(
                pkNormalTangentStream, 0, 
                uiLockType);

            if (GetConfiguration().IsTangentDataEnabled())
            {
                m_kTangentLocks[uiLevel] = NiNew NiDataStreamLock(
                    pkNormalTangentStream, 
                    sizeof(float) * GetConfiguration().GetNumNormalComponents(), 
                    uiLockType);
            }
            else
                m_kTangentLocks[uiLevel] = NULL;
        }
    }

    // Convert the index and UV static stream locks into read locks
    NiDelete m_pkIndexLock;
    NiDelete m_pkUVLock;

    m_pkIndexLock = NiNew NiDataStreamLock(
        m_pkSectorData->GetStaticIndexStream(), 0, (NiUInt8)uiLockType);
    m_pkUVLock = NiNew NiDataStreamLock(
        m_pkSectorData->GetStaticUVStream(), 0, (NiUInt8)uiLockType);

    // Update the world transforms for these leaves that we just loaded:
    NiUInt32 uiBeginIndex = m_puiLeafOffsets[
        GetSectorData()->GetHighestLoadedLOD() + 1];
    NiUInt32 uiEndIndex = m_kLeafArray.GetSize();
        
    m_pkSectorData->SetWorldTransform(GetWorldTransform());
    NiTerrainBlock* pkBlock;
    for (NiUInt32 ui = uiBeginIndex; ui < uiEndIndex; ++ui)
    {
        pkBlock = GetLeaf(ui)->GetQuadMesh();
        pkBlock->SetWorldTranslate(GetWorldTranslate());
        pkBlock->SetWorldScale(GetWorldScale());
        pkBlock->SetWorldRotate(GetWorldRotate());

        pkBlock->GetMesh().SetWorldTranslate(GetWorldTranslate());
        pkBlock->GetMesh().SetWorldScale(GetWorldScale());
        pkBlock->GetMesh().SetWorldRotate(GetWorldRotate());
    }        
}
//---------------------------------------------------------------------------
void NiTerrainSector::BuildBounds()
{
    if (!m_pkQuadMesh)
        return;

    // This is a fast function, to create bounds in bulk
    NiUInt32 uiNumBlocks = m_pkSectorData->GetNumBlocks();

    NiTerrainDataLeaf* pkLeaf;
    const NiDataStreamLock* pkStreamLock;
    NiTStridedRandomAccessIterator<NiPoint4> kStaticPositions;

    NiUInt32 uiRowSize = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiNumPositions = uiRowSize * uiRowSize;

    for (NiInt32 uiLeafID = uiNumBlocks - 1; uiLeafID >= 0; --uiLeafID)
    {
        // Using a dynamic position stream from a specific block
        pkLeaf = m_kLeafArray[uiLeafID];
        NiPoint3 kBoundCenter = NiPoint3::ZERO;
        NiPoint4 kPointSum = NiPoint4::ZERO;
        NiBound kBound;

        if (pkLeaf->GetQuadMesh()->HasDynamicVertexStreams())
        {
            pkStreamLock = pkLeaf->GetQuadMesh()->GetDynamicPositionLockRead();
            NiTStridedRandomAccessIterator<NiPoint4> kPositions = 
                pkStreamLock->begin<NiPoint4>();

            for (NiUInt32 ui = 0; ui < uiNumPositions; ++ui)
                kPointSum += kPositions[ui];

            kBoundCenter = 
                NiPoint3(kPointSum.X(), kPointSum.Y(), kPointSum.Z()) / 
                float(uiNumPositions);

            float fMaxDistSqr = 0;
            for (NiUInt32 ui = 0; ui < uiNumPositions; ++ui)
            {
                NiPoint4& kPosition = kPositions[ui];
                fMaxDistSqr = NiMax((kBoundCenter - NiPoint3(
                    kPosition.X(), kPosition.Y(), kPosition.Z())).SqrLength(),
                    fMaxDistSqr);
            }

            kBound.SetCenterAndRadius(kBoundCenter, NiSqrt(fMaxDistSqr));            
        }
        else
        {
            pkStreamLock = m_kPositionLocks[pkLeaf->GetLevel()];
            kStaticPositions = pkStreamLock->begin<NiPoint4>();
            NIASSERT(pkStreamLock);

            // Use the static position stream, with offsets
            NiIndex kBottomLeft = pkLeaf->SectorIndexFromLocal(NiIndex::ZERO);
            NiIndex kPoint;

            NiUInt32 uiRegionOffset = 
                pkLeaf->GetQuadMesh()->GetStaticPositionRegion().GetStartIndex();

            for (NiUInt32 y = 0; y < uiRowSize; ++y)
            {
                kPoint.y = kBottomLeft.y + 
                    (y * (1 << pkLeaf->GetNumSubDivisions()));
                for (NiUInt32 x = 0; x < uiRowSize; ++x)
                {
                    kPoint.x = kBottomLeft.x + 
                        (x * (1 << pkLeaf->GetNumSubDivisions()));
                    NiPoint4& kPosition = kStaticPositions[
                        uiRegionOffset + x + y * 
                        m_pkSectorData->GetBlockWidthInVerts()];
                    kPointSum += kPosition;                
                }
            }
            kBoundCenter = kPointSum / float(uiNumPositions);

            float fMaxDistSqr = 0;
            for (NiUInt32 y = 0; y < uiRowSize; ++y)
            {
                kPoint.y = kBottomLeft.y + 
                    (y * (1 << pkLeaf->GetNumSubDivisions()));
                for (NiUInt32 x = 0; x < uiRowSize; ++x)
                {
                    kPoint.x = kBottomLeft.x + 
                        (x * (1 << pkLeaf->GetNumSubDivisions()));
                    NiPoint4& kPosition = kStaticPositions[
                        uiRegionOffset + x + y * 
                        m_pkSectorData->GetBlockWidthInVerts()];

                    fMaxDistSqr = NiMax(
                        (kBoundCenter - NiPoint3(
                            kPosition.X(), 
                            kPosition.Y(), 
                            kPosition.Z())).SqrLength(), 
                        fMaxDistSqr);             
                }
            }

            kBound.SetCenterAndRadius(kBoundCenter, NiSqrt(fMaxDistSqr));
        }

        // If there are child leaves, then add their bounds to this leaf
        if (pkLeaf->GetChildAt(0))
        {
            NiBound kChildBound;
            kChildBound = pkLeaf->GetChildAt(0)->GetLocalBound();
            kBound.Merge(&kChildBound);            
            kChildBound = pkLeaf->GetChildAt(1)->GetLocalBound();
            kBound.Merge(&kChildBound);            
            kChildBound = pkLeaf->GetChildAt(2)->GetLocalBound();
            kBound.Merge(&kChildBound);            
            kChildBound = pkLeaf->GetChildAt(3)->GetLocalBound();
            kBound.Merge(&kChildBound);            
        }

        pkLeaf->SetBoundData(kBound.GetCenter(), kBound.GetRadius(), kPointSum);
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    DoUpdate(kUpdate);
    // Use the AV Object version of the function so that bounds are
    // not calculated based on the children of this node. 
    NiAVObject::UpdateDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrainSector::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
        DoUpdate(kUpdate);

    // Use the AV Object version of the function so that bounds are
    // not calculated based on the children of this node. 
    NiAVObject::UpdateSelectedDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrainSector::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
        DoUpdate(kUpdate);

    // Use the AV Object version of the function so that bounds are
    // not calculated based on the children of this node. 
    NiAVObject::UpdateRigidDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrainSector::UpdateNodeBound()
{
    if (m_pkQuadData)
    {
        m_kBound = m_pkQuadData->GetLocalBound();
    }
    else
    {
        m_kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    }

    m_kWorldBound.Update(m_kBound, m_kWorld);
}
//---------------------------------------------------------------------------
void NiTerrainSector::UpdateWorldBound()
{
    UpdateNodeBound();
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateIndices(NiDataStreamLock* pkIndexLock)
{
    NIASSERT(pkIndexLock && pkIndexLock->IsLocked());
    NiTStridedRandomAccessIterator<NiUInt16> kIndicesIter16;
    NiTStridedRandomAccessIterator<NiUInt32> kIndicesIter32;

    // Need to get a lock appropriate to the type of stream we are using
    // Now create the triangles for each region
    for (NiUInt8 uc = 0; uc < NiTerrainSectorData::NUM_INDEX_REGIONS; ++uc)
    {
        if (m_bUsingShortIndexBuffer) 
            kIndicesIter16 = pkIndexLock->begin_region<NiUInt16>(uc);
        else
            kIndicesIter32 = pkIndexLock->begin_region<NiUInt32>(uc);

        NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();
        bool bFlipped = false;
        bool bDrewBorder = false;

        // Block size minus one
        NiUInt32 uiBkSzMinOne = uiBlockSize - 1;

        for (NiUInt32 y = 0; y < uiBlockSize; ++y) 
        {
            for (NiUInt32 x = 0; x < uiBlockSize; ++x) 
            {
                bDrewBorder = false;
                
                // Bottom border
                if (y == 0 && (uc == 1 || uc == 8 || uc == 5))
                {
                    bDrewBorder = true;
                    if (x % 2 == 1)
                    {
                        if (m_bUsingShortIndexBuffer)
                        {
                            // Stitching triangle
                            // (x-1, y) (x+1, y) (x, y+1)
                            AddTri<NiUInt16>(x-1, y, x+1, y, x, y+1, 
                                kIndicesIter16);

                            // do not draw if left is being stitched
                            if (x > 1 || uc != 8) 
                            {
                                // (x-1, y) (x, y+1) (x-1, y+1)
                                AddTri<NiUInt16>(x-1, y, x, y+1, x-1, y+1,
                                    kIndicesIter16);
                            }

                            // do not draw if right is being stitched
                            if (x < uiBkSzMinOne || uc != 5) 
                            {
                                // (x+1, y) (x+1, y+1) (x, y+1)
                                AddTri<NiUInt16>(x+1, y, x+1, y+1, x, y+1,
                                    kIndicesIter16);
                            }
                        }    
                        else
                        {
                            // Stitching triangle
                            // (x-1, y) (x+1, y) (x, y+1)
                            AddTri<NiUInt32>(x-1, y, x+1, y, x, y+1,
                                kIndicesIter32);

                            // do not draw if left is being stitched
                            if (x > 1 || uc != 8) 
                            {
                                // (x-1, y) (x, y+1) (x-1, y+1)
                                AddTri<NiUInt32>(x-1, y, x, y+1, x-1, y+1,
                                    kIndicesIter32);
                            }

                            // do not draw if right is being stitched
                            if (x < uiBkSzMinOne || uc != 5) 
                            {
                                // (x+1, y) (x+1, y+1) (x, y+1)
                                AddTri<NiUInt32>(x+1, y, x+1, y+1, x, y+1,
                                    kIndicesIter32);
                            }
                        }
                    }
                }
                // Top border
                else if (y == uiBkSzMinOne && (uc == 3 || uc == 6 || uc == 7))
                {
                    bDrewBorder = true;
                    if (x % 2 == 1)
                    {
                        if (m_bUsingShortIndexBuffer)
                        {
                            // Stitching triangle
                            // (x-1, y+1) (x, y) (x+1, y+1)
                            AddTri<NiUInt16>(x-1, y+1, x, y, x+1, y+1,
                                kIndicesIter16);

                            // do not draw if left is being stitched
                            if (x > 1 || uc != 7) 
                            {
                                // (x-1, y) (x, y) (x-1, y+1)
                                AddTri<NiUInt16>(x-1, y, x, y, x-1, y+1,
                                    kIndicesIter16);
                            }

                            // do not draw if right is being stitched
                            if (x < uiBkSzMinOne || uc != 6) 
                            {
                                // (x, y) (x+1, y) (x+1, y+1)
                                AddTri<NiUInt16>(x, y, x+1, y, x+1, y+1,
                                    kIndicesIter16);
                            }
                        }
                        else
                        {
                            // Stitching triangle
                            // (x-1, y+1) (x, y) (x+1, y+1)
                            AddTri<NiUInt32>(x-1, y+1, x, y, x+1, y+1,
                                kIndicesIter32);

                            // do not draw if left is being stitched
                            if (x > 1 || uc != 7) 
                            {
                                // (x-1, y) (x, y) (x-1, y+1)
                                AddTri<NiUInt32>(x-1, y, x, y, x-1, y+1,
                                    kIndicesIter32);
                            }

                            // do not draw if right is being stitched
                            if (x < uiBkSzMinOne || uc != 6) 
                            {
                                // (x, y) (x+1, y) (x+1, y+1)
                                AddTri<NiUInt32>(x, y, x+1, y, x+1, y+1,
                                    kIndicesIter32);
                            }
                        }                        
                    }
                }

                // Right border
                if (x == uiBkSzMinOne && (uc == 2 || uc == 5 || uc == 6))
                {
                    bDrewBorder = true;
                    if (y % 2 == 1)
                    {
                        if (m_bUsingShortIndexBuffer)
                        {
                            // Stitching triangle
                            // (x+1, y-1) (x+1, y+1) (x, y)
                            AddTri<NiUInt16>(x+1, y-1, x+1, y+1, x, y,
                                kIndicesIter16);

                            // do not draw if top is being stitched
                            if (y < uiBkSzMinOne || uc != 6) 
                            {
                                // (x, y) (x+1, y+1) (x, y+1)
                                AddTri<NiUInt16>(x, y, x+1, y+1, x, y+1,
                                    kIndicesIter16);
                            }

                            // do not draw if bottom is being stitched
                            if (y > 1 || uc != 5) 
                            {
                                // (x, y) (x+1, y) (x, y+1)
                                AddTri<NiUInt16>(x, y, x, y-1, x+1, y-1,
                                    kIndicesIter16);
                            }
                        }
                        else
                        {
                            // Stitching triangle
                            // (x+1, y-1) (x+1, y+1) (x, y)
                            AddTri<NiUInt32>(x+1, y-1, x+1, y+1, x, y,
                                kIndicesIter32);

                            // do not draw if top is being stitched
                            if (y < uiBkSzMinOne || uc != 6) 
                            {
                                // (x, y) (x+1, y+1) (x, y+1)
                                AddTri<NiUInt32>(x, y, x+1, y+1, x, y+1,
                                    kIndicesIter32);
                            }

                            // do not draw if bottom is being stitched
                            if (y > 1 || uc != 5) 
                            {
                                // (x, y) (x+1, y) (x, y+1)
                                AddTri<NiUInt32>(x, y, x, y-1, x+1, y-1,
                                    kIndicesIter32);
                            }
                        }                        
                    }
                }
                // Left border
                else if (x == 0 && (uc == 4 || uc == 7 || uc == 8))
                {
                    bDrewBorder = true;
                    if (y % 2 == 1)
                    {
                        if (m_bUsingShortIndexBuffer)
                        {
                            // Stitching triangle
                            // (x, y-1) (x+1, y) (x, y+1)
                            AddTri<NiUInt16>(x, y-1, x+1, y, x, y+1,
                                kIndicesIter16);

                            // do not draw if top is being stitched
                            if (y < uiBkSzMinOne || uc != 7) 
                            {
                                // (x+1, y) (x+1, y+1) (x, y+1)
                                AddTri<NiUInt16>(x+1, y, x+1, y+1, x, y+1,
                                    kIndicesIter16);
                            }

                            // do not draw if bottom is being stitched
                            if (y > 1 || uc != 8) 
                            {
                                // (x, y) (x+1, y) (x+1, y+1)
                                AddTri<NiUInt16>(x, y-1, x+1, y-1, x+1, y,
                                    kIndicesIter16);
                            }
                        }
                        else
                        {
                            // Stitching triangle
                            // (x, y-1) (x+1, y) (x, y+1)
                            AddTri<NiUInt32>(x, y-1, x+1, y, x, y+1,
                                kIndicesIter32);

                            // do not draw if top is being stitched
                            if (y < uiBkSzMinOne || uc != 7) 
                            {
                                // (x+1, y) (x+1, y+1) (x, y+1)
                                AddTri<NiUInt32>(x+1, y, x+1, y+1, x, y+1,
                                    kIndicesIter32);
                            }

                            // do not draw if bottom is being stitched
                            if (y > 1 || uc != 8) 
                            {
                                // (x, y) (x+1, y) (x+1, y+1)
                                AddTri<NiUInt32>(x, y-1, x+1, y-1, x+1, y,
                                    kIndicesIter32);
                            }
                        }                        
                    }
                }

                if (!bDrewBorder)
                {
                    // Just a normal triangle
                    if (bFlipped)
                    {
                        if (m_bUsingShortIndexBuffer)
                        {
                            // (x, y) (x+1, y) (x, y+1)
                            AddTri<NiUInt16>(x, y, x+1, y, x, y+1,
                                kIndicesIter16);

                            // (x+1, y) (x+1, y+1) (x, y+1)
                            AddTri<NiUInt16>(x+1, y, x+1, y+1, x, y+1,
                                kIndicesIter16);
                        }
                        else
                        {
                            // (x, y) (x+1, y) (x, y+1)
                            AddTri<NiUInt32>(x, y, x+1, y, x, y+1,
                                kIndicesIter32);

                            // (x+1, y) (x+1, y+1) (x, y+1)
                            AddTri<NiUInt32>(x+1, y, x+1, y+1, x, y+1,
                                kIndicesIter32);
                        }                        
                    }
                    else
                    {
                        if (m_bUsingShortIndexBuffer)
                        {
                            // (x, y) (x+1, y) (x+1, y+1)
                            AddTri<NiUInt16>(x, y, x+1, y, x+1, y+1,
                                kIndicesIter16);

                            // (x, y) (x+1, y+1) (x, y+1)
                            AddTri<NiUInt16>(x, y, x+1, y+1, x, y+1,
                                kIndicesIter16);
                        }
                        else
                        {
                            // (x, y) (x+1, y) (x+1, y+1)
                            AddTri<NiUInt32>(x, y, x+1, y, x+1, y+1,
                                kIndicesIter32);

                            // (x, y) (x+1, y+1) (x, y+1)
                            AddTri<NiUInt32>(x, y, x+1, y+1, x, y+1,
                                kIndicesIter32);
                        }                        
                    }
                }

                bFlipped = !bFlipped;
            }

            bFlipped = !bFlipped;
        }
    }    
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateNormalsTangents(
    const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves)
{
    CalculateNormalsTangentsPhase1(kLeaves);
    CalculateNormalsTangentsPhase2(kLeaves);
    CalculateNormalsTangentsPhase3(kLeaves);
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateNormalsTangentsPhase1(
    const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves)
{
    /**
        All calculations in this function must be done on normal streams with 
        at least 3 components, since we are working with raw un-compressed 
        normals. Tangents do not share this limitation, since the y value is
        always = 0 allowing us to perform all calculations on the compressed 
        values.

        When the terrain configuration dictates that the normal stream is 
        compressed yet morphing is disabled, the component size will be 2. 
        In this case, we must attach a temporary normal stream to the mesh to
        hold the current normal 'sum' - which will be compressed into the
        primary normal stream in a later phase, then discarded.
     */

    // Initialize some variables
    NiUInt32 uiBlockWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiVertsPerBlock = uiBlockWidthInVerts * uiBlockWidthInVerts;

    // Create some iterators
    NiTerrainPositionRandomAccessIterator kPositionIter;
    NiTerrainNormalRandomAccessIterator kNormalIter; 
    NiTerrainTangentRandomAccessIterator kTangentIter;
    NiTStridedRandomAccessIterator<NiPoint2> kUVIter;

    NiTerrainNormalRandomAccessIterator kAdjNormalIter; 
    NiTerrainTangentRandomAccessIterator kAdjTangentIter;

    // Note: Only ONE of these iterators will ever be valid!
    NiTStridedRandomAccessIterator<NiUInt16> kIndexIter16;
    NiTStridedRandomAccessIterator<NiUInt32> kIndexIter32;
    if (m_bUsingShortIndexBuffer)
        kIndexIter16 = m_pkIndexLock->begin_region<NiUInt16>(0);
    else
        kIndexIter32 = m_pkIndexLock->begin_region<NiUInt32>(0);

    // Texture coords
    kUVIter = m_pkUVLock->begin<NiPoint2>();
    
    // Pre-declare some variables
    NiPoint3 kNormal;
    NiPoint3 kTangent;
    NiPoint3 kBiNormal; // dummy variable
    NiPoint2 kTextCoords[3];
    NiTerrainDataLeaf* pkThisLeaf;

    NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();

    // Highest detail index region only
    NiUInt32 uiIndexSize = 3 * 2 * uiBlockSize * uiBlockSize;

    // First pass - each leaf
    NiUInt32 uiIndex0, uiIndex1, uiIndex2;
    bool bRequireTempNormalStream = false;
    NiDataStreamLock kTempNormalStreamLock;
    if (GetConfiguration().GetNumNormalComponents() <= 2)
        bRequireTempNormalStream = true;

    NiTerrainConfiguration kTempNormalStreamConfig = GetConfiguration();
    kTempNormalStreamConfig.EnableMorphingData(false);
    kTempNormalStreamConfig.EnableLightingDataCompression(false);

    for (NiUInt32 ui = 0; ui < kLeaves.GetSize(); ++ui)
    {
        pkThisLeaf = kLeaves.GetAt(ui);

        NIASSERT(pkThisLeaf->GetQuadMesh()->RequiresLightingRebuild());
        pkThisLeaf->GetPositionIterator(kPositionIter);

        if (bRequireTempNormalStream)
        {
            NiMesh& kMesh = pkThisLeaf->GetQuadMesh()->GetMesh();
            NiDataStreamRef* pkRef = kMesh.AddStream(
                NiCommonSemantics::NORMAL(), 1, 
                GetNormalStreamFormat(&kTempNormalStreamConfig), 
                uiVertsPerBlock, 
                NiDataStream::ACCESS_CPU_READ |
                NiDataStream::ACCESS_CPU_WRITE_STATIC,
                NiDataStream::USAGE_USER);
            kTempNormalStreamLock = NiDataStreamLock(pkRef->GetDataStream(), 0,
                NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);
            kNormalIter = NiTerrainNormalRandomAccessIterator(
                &kTempNormalStreamLock, (NiUInt32)0, kTempNormalStreamConfig);
        }
        else
        {
            pkThisLeaf->GetNormalIterator(kNormalIter);
        }

        for (NiUInt32 uiIndex = 0; uiIndex < uiVertsPerBlock; ++uiIndex)
        {
            kNormalIter.Set(uiIndex, NiPoint3::ZERO);
        }

        if (GetConfiguration().IsTangentDataEnabled())
        {
            pkThisLeaf->GetTangentIterator(kTangentIter);
        
            for (NiUInt32 uiIndex = 0; uiIndex < uiVertsPerBlock; ++uiIndex)
            {
                kTangentIter.Set(uiIndex, NiPoint2::ZERO);
            }
        }

        NiPoint3 kNormalScratch;
        NiPoint3 kTangentScratch;
        for (NiUInt32 uiIndex = 0; uiIndex < uiIndexSize; )
        {
            if (m_bUsingShortIndexBuffer)
            {
                uiIndex0 = NiUInt32(kIndexIter16[uiIndex++]);
                uiIndex1 = NiUInt32(kIndexIter16[uiIndex++]);
                uiIndex2 = NiUInt32(kIndexIter16[uiIndex++]);
            }
            else
            {
                uiIndex0 = kIndexIter32[uiIndex++];
                uiIndex1 = kIndexIter32[uiIndex++];
                uiIndex2 = kIndexIter32[uiIndex++];
            }

            NiPoint3 kPosition0;
            kPositionIter.GetHighDetail(uiIndex0, kPosition0);
            
            NiPoint3 kPosition1;
            kPositionIter.GetHighDetail(uiIndex1, kPosition1);
            
            NiPoint3 kPosition2;
            kPositionIter.GetHighDetail(uiIndex2, kPosition2);

            kNormal = NiGeometricUtils::GetTriNormal(
                kPosition0, 
                kPosition1,
                kPosition2);

            // We need texture coordinate estimates to calculate the tangent.
            kTextCoords[0] = kUVIter[uiIndex0];
            kTextCoords[1] = kUVIter[uiIndex1];
            kTextCoords[2] = kUVIter[uiIndex2];

            NiMeshUtilities::FindBT(
                kPosition0, kPosition1, kPosition2, 
                kTextCoords[0], kTextCoords[1], 
                kTextCoords[2], kBiNormal, kTangent);

            kNormalIter.Get(uiIndex0, kNormalScratch);
            kNormalIter.Set(uiIndex0, kNormalScratch + kNormal);
            kNormalIter.Get(uiIndex1, kNormalScratch);
            kNormalIter.Set(uiIndex1, kNormalScratch + kNormal);
            kNormalIter.Get(uiIndex2, kNormalScratch);
            kNormalIter.Set(uiIndex2, kNormalScratch + kNormal);

            if (GetConfiguration().IsTangentDataEnabled())
            {
                // The Y component MUST be zero to work with our compression
                NIASSERT(kTangent.y == 0.0f)

                kTangentIter.GetHighDetail(uiIndex0, kTangentScratch);
                kTangentIter.SetHighDetail(uiIndex0, kTangentScratch + kTangent);
                kTangentIter.GetHighDetail(uiIndex1, kTangentScratch);
                kTangentIter.SetHighDetail(uiIndex1, kTangentScratch + kTangent);
                kTangentIter.GetHighDetail(uiIndex2, kTangentScratch);
                kTangentIter.SetHighDetail(uiIndex2, kTangentScratch + kTangent);
            }

            if (kTempNormalStreamLock.IsLocked())
                kTempNormalStreamLock.Unlock();
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateNormalsTangentsPhase2(
    const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves)
{
    NiUInt32 bs = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 rs = m_pkSectorData->GetBlockWidthInVerts();

    // Second pass - share values between leaf seams
    // Define some variables to make things easier to understand
    const NiUInt8 B = NiTerrainDataLeaf::BORDER_BOTTOM;
    const NiUInt8 R = NiTerrainDataLeaf::BORDER_RIGHT;
    const NiUInt8 T = NiTerrainDataLeaf::BORDER_TOP;
    const NiUInt8 L = NiTerrainDataLeaf::BORDER_LEFT;
    const NiUInt8 BL = B | L; 
    const NiUInt8 BR = B | R;
    const NiUInt8 TR = T | R;
    const NiUInt8 TL = T | L;
            
    const NiUInt8 aucBorders[] =         { B, R, T, L,BR,TR,BL,TL};
    const NiInt32 aiOurOffsetX[] =       { 0, 1, 0, 0, 1, 1, 0, 0};
    const NiInt32 aiOurOffsetY[] =       { 0, 0, 1, 0, 0, 1, 0, 1};
    const NiInt32 aiAdjOffsetX[] =       { 0, 0, 0, 1, 0, 0, 1, 1};
    const NiInt32 aiAdjOffsetY[] =       { 1, 0, 0, 0, 1, 0, 1, 0};
    const NiInt32 aiInc[] =              { 1,rs, 1,rs, 1, 1, 1, 1};
    const NiInt32 aiIncMax[] =           {bs,bs,bs,bs, 1, 1, 1, 1};

    NiUInt32 uiCornerVerts[] = {0, rs - 1, rs * (rs  -1), rs * rs - 1}; 
    
    NiTerrainNormalRandomAccessIterator kNormalIter; 
    NiTerrainTangentRandomAccessIterator kTangentIter;
    NiTerrainNormalRandomAccessIterator kAdjNormalIter; 
    NiTerrainTangentRandomAccessIterator kAdjTangentIter;

    // For blocks with uncompressed streams, will they contain a temporary 
    // stream which we need to read?
    bool bRequireTempNormalStream = false;
    NiDataStreamLock kTempNormalStreamLock;
    NiDataStreamLock kAdjTempNormalStreamLock;
    NiTerrainConfiguration kTempNormalStreamConfig = GetConfiguration();
    kTempNormalStreamConfig.EnableMorphingData(false);
    kTempNormalStreamConfig.EnableLightingDataCompression(false);
    if (GetConfiguration().GetNumNormalComponents() <= 2)
        bRequireTempNormalStream = true;

    NiTerrainDataLeaf* pkThisLeaf = 0;
    NiTerrainDataLeaf* pkAdjacent = 0;
    bool bIsAdjacentCompressed;
    bool bCopyAdjacent;
    NiPoint2 kTangentScratch;
    NiPoint3 kNormalScratch;
    NiPoint4 kOriginalNormal;
    NiPoint4 kOriginalTangent;
    NiUInt32 uiOurOffset = 0;
    NiUInt32 uiAdjOffset = 0;
    NiUInt32 uiOur;
    NiUInt32 uiAdj;
    for (NiUInt32 uiIndex = 0; uiIndex < kLeaves.GetSize(); ++uiIndex)
    {
        pkThisLeaf = kLeaves.GetAt(uiIndex);
    
        // Indicate that this block is calculating its seams
        pkThisLeaf->GetQuadMesh()->MarkLightingInProgress();

        if (bRequireTempNormalStream)
        {
            NiDataStreamRef* pkRef = 
                pkThisLeaf->GetQuadMesh()->GetMesh().FindStreamRef(
                    NiCommonSemantics::NORMAL(), 1, 
                    GetNormalStreamFormat(&kTempNormalStreamConfig));
            kTempNormalStreamLock = NiDataStreamLock(pkRef->GetDataStream(), 0,
                NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);
            kNormalIter = NiTerrainNormalRandomAccessIterator(
                &kTempNormalStreamLock, (NiUInt32)0, kTempNormalStreamConfig);
        }
        else
        {
            pkThisLeaf->GetNormalIterator(kNormalIter);
        }

        pkThisLeaf->GetTangentIterator(kTangentIter);

        bool abCopiedCorner[] = {false, false, false, false};

        // Borders
        for(NiUInt32 uiBorder = 0; uiBorder < 8; ++uiBorder)
        {
            pkAdjacent = pkThisLeaf->GetAdjacent(aucBorders[uiBorder]);
            if (!pkAdjacent)
                continue;

            /* Do we need to modify the adjacent leaf? 
            Yes if: it is TOP, TOPRIGHT, RIGHT of us, and will be processed by
                this function.

            Additionally, we may need to read values from the adjacent if it is
            not going to be processed by this function.
            */
        
            // Is the adjacent block's data compressed?
            bIsAdjacentCompressed = 
                !pkAdjacent->GetQuadMesh()->RequiresLightingRebuild();
            // Has the adjacent block already calculated it's values?
            bCopyAdjacent = 
                pkAdjacent->GetQuadMesh()->IsLightingInProgress();
        
            // Setup the constants and iterators for this pass
            pkAdjacent->GetTangentIterator(kAdjTangentIter);
            if (!bIsAdjacentCompressed && bRequireTempNormalStream)
            {
                NiDataStreamRef* pkRef = 
                    pkAdjacent->GetQuadMesh()->GetMesh().FindStreamRef(
                    NiCommonSemantics::NORMAL(), 1, 
                    GetNormalStreamFormat(&kTempNormalStreamConfig));
                kTempNormalStreamLock = NiDataStreamLock(
                    pkRef->GetDataStream(), 0,
                    NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);
                kAdjNormalIter = NiTerrainNormalRandomAccessIterator(
                    &kTempNormalStreamLock, (NiUInt32)0, 
                    kTempNormalStreamConfig);
            }
            else
            {
                pkAdjacent->GetNormalIterator(kAdjNormalIter);
            }
            
            uiOurOffset  = aiOurOffsetX[uiBorder] * 
                m_pkSectorData->GetBlockSize();
            uiOurOffset += aiOurOffsetY[uiBorder] * 
                m_pkSectorData->GetBlockSize() * 
                m_pkSectorData->GetBlockWidthInVerts();

            uiAdjOffset  = aiAdjOffsetX[uiBorder] * 
                m_pkSectorData->GetBlockSize();
            uiAdjOffset += aiAdjOffsetY[uiBorder] * 
                m_pkSectorData->GetBlockSize() *
                m_pkSectorData->GetBlockWidthInVerts();

            // Loop through all the relevant vertices
            for (NiUInt16 us = 0; us < aiIncMax[uiBorder]; ++us)
            {
                uiOur = aiInc[uiBorder] * us + uiOurOffset;
                uiAdj = aiInc[uiBorder] * us + uiAdjOffset;

                // Has this corner already been copied from another block?
                NiUInt8 ucCorner;
                for (ucCorner = 4; ucCorner > 0; --ucCorner)
                {
                    if (uiCornerVerts[ucCorner - 1] == uiOur)
                        break;
                }
                if (ucCorner && abCopiedCorner[ucCorner - 1])
                    continue;

                if (bIsAdjacentCompressed)
                {
                    // Since our adjacent is compressed, we must simply 
                    // copy their value. This will ensure that no seams occur.
                    // We cannot compute a new average, since we cannot 
                    // propagate the new average value to the adjacent.
                    // It is worth noting that if the adjacent's data is
                    // compressed then the seam normals on this edge should
                    // not have changed anyway and this is acceptable.

                    // A side effect of the adjacent not being modifiable, is
                    // that all its values must be decompressed before reading.
                    kAdjNormalIter.GetHighDetail(uiAdj, kNormalScratch);
                    kNormalIter.Set(uiOur, kNormalScratch);
                    
                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kAdjTangentIter.GetHighDetail(uiAdj, kTangentScratch);
                        kTangentIter.SetHighDetail(uiOur, kTangentScratch);
                    }

                    // Mark this value as copied
                    if (ucCorner)
                        abCopiedCorner[ucCorner - 1] = true;
                }
                else if (bCopyAdjacent)
                {
                    // The other block has already calculated the seam data,
                    // and it is in uncompressed mode, so it is safe to do a
                    // direct copy (we are in uncompressed mode too)
                    kAdjNormalIter.Get(uiAdj, kNormalScratch);
                    kNormalIter.Set(uiOur, kNormalScratch);

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kAdjTangentIter.GetHighDetail(uiAdj, kTangentScratch);
                        kTangentIter.SetHighDetail(uiOur, kTangentScratch);
                    }

                    // Mark this value as copied
                    if (ucCorner)
                        abCopiedCorner[ucCorner - 1] = true;
                }
                else // Sum the vertices
                {
                    NiPoint3 kOurNormal;
                    NiPoint2 kOurTangent;

                    // This adjacent is in the array of leaves to modify by 
                    // this function. This means that we can optimize the
                    // seam averaging by sharing our value with the appropriate
                    // seam of the adjacent.

                    // Note, the adjacent normal is in uncompressed mode.
                    kAdjNormalIter.Get(uiAdj, kNormalScratch);
                    kNormalIter.Get(uiOur, kOurNormal);
                    kNormalIter.Set(uiOur, kOurNormal + kNormalScratch);

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kAdjTangentIter.GetHighDetail(uiAdj, kTangentScratch);
                        kTangentIter.GetHighDetail(uiOur, kOurTangent);
                        kTangentIter.SetHighDetail(uiOur, 
                            kOurTangent + kTangentScratch);
                    }
                }
            }

            if (kAdjTempNormalStreamLock.IsLocked())
                kAdjTempNormalStreamLock.Unlock();
        }

        if (kTempNormalStreamLock.IsLocked())
            kTempNormalStreamLock.Unlock();
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateNormalsTangentsPhase3(
    const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves)
{
    // Third pass - compute averages and compress values to X and Y components
    // of the data streams as appropriate
    NiUInt32 uiNumVerts = m_pkSectorData->GetBlockWidthInVerts() * 
        m_pkSectorData->GetBlockWidthInVerts();

    NiTerrainNormalRandomAccessIterator kUncompressedNormalIter;
    NiTerrainNormalRandomAccessIterator kCompressedNormalIter;
    NiTerrainTangentRandomAccessIterator kTangentIter;
    NiTerrainDataLeaf* pkThisLeaf = 0;

    // For blocks with uncompressed streams, will they contain a temporary 
    // stream which we need to read?
    bool bRequireTempNormalStream = false;
    NiDataStreamLock kTempNormalStreamLock;
    NiTerrainConfiguration kTempNormalStreamConfig = GetConfiguration();
    kTempNormalStreamConfig.EnableMorphingData(false);
    kTempNormalStreamConfig.EnableLightingDataCompression(false);
    if (GetConfiguration().GetNumNormalComponents() <= 2)
        bRequireTempNormalStream = true;

    // Iterate through the streams, compressing data if appropriate.
    for (NiUInt32 uiIndex = 0; uiIndex < kLeaves.GetSize(); ++uiIndex)
    {
        pkThisLeaf = kLeaves.GetAt(uiIndex);

        // Compressed normal iterator
        pkThisLeaf->GetNormalIterator(kCompressedNormalIter);

        // Uncompressed normal iterator
        if (bRequireTempNormalStream)
        {
            NiDataStreamRef* pkRef = 
                pkThisLeaf->GetQuadMesh()->GetMesh().FindStreamRef(
                NiCommonSemantics::NORMAL(), 1, 
                GetNormalStreamFormat(&kTempNormalStreamConfig));
            kTempNormalStreamLock = NiDataStreamLock(pkRef->GetDataStream(), 0,
                NiDataStream::LOCK_WRITE | NiDataStream::LOCK_READ);
            kUncompressedNormalIter = NiTerrainNormalRandomAccessIterator(
                &kTempNormalStreamLock, (NiUInt32)0, kTempNormalStreamConfig);
        }
        else
        {
            pkThisLeaf->GetNormalIterator(kUncompressedNormalIter);
        }

        // Tangent iterator
        pkThisLeaf->GetTangentIterator(kTangentIter);

        // Cycle through the vertices
        NiPoint3 kScratch;
        for (NiUInt32 uiVert = 0; uiVert < uiNumVerts; ++uiVert)
        {
            kUncompressedNormalIter.Get(uiVert, kScratch);
            kScratch.Unitize();
            kCompressedNormalIter.SetHighDetail(uiVert, kScratch);

            if (GetConfiguration().IsTangentDataEnabled())
            {
                kTangentIter.GetHighDetail(uiVert, kScratch);
                kScratch.Unitize();
                kTangentIter.SetHighDetail(uiVert, kScratch);
            }
        }

        if (pkThisLeaf->GetQuadMesh())
            pkThisLeaf->GetQuadMesh()->MarkLightingBuilt();

        if (kTempNormalStreamLock.IsLocked())
            kTempNormalStreamLock.Unlock();
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateGeoMorph(
    const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves)
{
    if (!GetConfiguration().IsMorphingDataEnabled())
        return;

    NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();
    NiUInt32 uiHalfBlockSize = uiBlockSize >> 1;
    NiUInt32 uiRowSize = m_pkSectorData->GetBlockWidthInVerts();
    
    for (NiUInt32 ui = 0; ui < kLeaves.GetSize(); ++ui)
    {
        NiTerrainDataLeaf* pkLeaf = kLeaves.GetAt(ui);

        NiTerrainPositionRandomAccessIterator kPositions;
        pkLeaf->GetPositionIterator(kPositions);

        NiTerrainNormalRandomAccessIterator kNormals;
        pkLeaf->GetNormalIterator(kNormals);

        NiTerrainTangentRandomAccessIterator kTangents;
        pkLeaf->GetTangentIterator(kTangents);

        NiTerrainDataLeaf* pkParent = pkLeaf->GetParent();
        if (!pkParent)
        {
            // Simply copy data for the worst detail block
            NiUInt32 uiVertsPerBlock = m_pkSectorData->GetBlockWidthInVerts() *
                m_pkSectorData->GetBlockWidthInVerts();

            for (NiUInt32 uiIndex = 0; uiIndex < uiVertsPerBlock; ++uiIndex)
            {
                kPositions.SetComponent(uiIndex, 
                    NiTerrainPositionRandomAccessIterator::W, 
                    kPositions.GetComponent(uiIndex, 
                        NiTerrainPositionRandomAccessIterator::Z));

                NiPoint2 kNormal;
                kNormals.GetHighDetail(uiIndex, kNormal);
                kNormals.SetLowDetail(uiIndex, kNormal);

                if (GetConfiguration().IsTangentDataEnabled())
                {
                    kTangents.GetHighDetail(uiIndex, kNormal);
                    kTangents.SetLowDetail(uiIndex, kNormal);
                }
            }
            continue;
        }

        NiTerrainPositionRandomAccessIterator kParentPositions;
        pkParent->GetPositionIterator(kParentPositions);

        NiTerrainNormalRandomAccessIterator kParentNormals;
        pkParent->GetNormalIterator(kParentNormals);

        NiTerrainTangentRandomAccessIterator kParentTangents;
        pkParent->GetTangentIterator(kParentTangents);

        NiIndex kOurBottomLeft;
        NiIndex kParentBottomLeft;
        pkLeaf->GetBottomLeftIndex(kOurBottomLeft);
        GetLeaf(pkParent->GetBlockID())->GetBottomLeftIndex(
            kParentBottomLeft);

        NiIndex kParentIndexOffset;
        if (kOurBottomLeft.x > kParentBottomLeft.x)
            kParentIndexOffset.x = uiHalfBlockSize;
        if (kOurBottomLeft.y > kParentBottomLeft.y)
            kParentIndexOffset.y = uiHalfBlockSize;

        NiUInt32 uiOffset;
        float fAveragePosition;
        NiPoint3 kAverageNormal;
        NiPoint3 kAverageTangent;
        NiPoint3 kScratch;
        NiPoint3 kTempTangent;

        for (NiUInt32 y = 0; y <= uiBlockSize; ++y)
        {
            for (NiUInt32 x = 0; x <= uiBlockSize; ++x)
            {
                fAveragePosition = 0.0f;
                kAverageNormal = NiPoint3::ZERO;
                kAverageTangent = NiPoint3::ZERO;

                bool xOdd = x % 2 != 0;
                bool yOdd = y % 2 != 0;

                // On a diagonal?
                if (xOdd && yOdd)
                {
                    // Which face is it?
                    if ((x % 4) == (y % 4))
                    {
                        // bottom left
                        uiOffset = ((x-1) >> 1) + kParentIndexOffset.x +
                            (((y-1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                        fAveragePosition += kParentPositions.GetComponent(
                            uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                        
                        kParentNormals.GetHighDetail(uiOffset, kScratch);
                        kAverageNormal += kScratch;

                        if (GetConfiguration().IsTangentDataEnabled())
                        {
                            kParentTangents.GetHighDetail(
                                uiOffset, kTempTangent);
                            kAverageTangent += kTempTangent;
                        }

                        // top right
                        uiOffset = ((x+1) >> 1) + kParentIndexOffset.x +
                            (((y+1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                        fAveragePosition += kParentPositions.GetComponent(
                            uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                        
                        kParentNormals.GetHighDetail(uiOffset, kScratch);
                        kAverageNormal += kScratch;

                        if (GetConfiguration().IsTangentDataEnabled())
                        {
                            kParentTangents.GetHighDetail(
                                uiOffset, kTempTangent);
                            kAverageTangent += kTempTangent;
                        }
                    }
                    else
                    {
                        // bottom right
                        uiOffset = ((x+1) >> 1) + kParentIndexOffset.x +
                            (((y-1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                        fAveragePosition += kParentPositions.GetComponent(
                            uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                        
                        kParentNormals.GetHighDetail(uiOffset, kScratch);
                        kAverageNormal += kScratch;

                        if (GetConfiguration().IsTangentDataEnabled())
                        {
                            kParentTangents.GetHighDetail(
                                uiOffset, kTempTangent);
                            kAverageTangent += kTempTangent;
                        }

                        // top left
                        uiOffset = ((x-1) >> 1) + kParentIndexOffset.x +
                            (((y+1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                        fAveragePosition += kParentPositions.GetComponent(
                            uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                        
                        kParentNormals.GetHighDetail(uiOffset, kScratch);
                        kAverageNormal += kScratch;

                        if (GetConfiguration().IsTangentDataEnabled())
                        {
                            kParentTangents.GetHighDetail(uiOffset, kTempTangent);
                            kAverageTangent += kTempTangent;
                        }
                    }
                }
                // Horizontal?
                else if (xOdd)
                {
                    // Left
                    uiOffset = ((x-1) >> 1) + kParentIndexOffset.x +
                            (((y) >> 1) + kParentIndexOffset.y) * uiRowSize;
                    fAveragePosition += kParentPositions.GetComponent(
                        uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                    kParentNormals.GetHighDetail(uiOffset, kScratch);
                    kAverageNormal += kScratch;

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kParentTangents.GetHighDetail(uiOffset, kTempTangent);
                        kAverageTangent += kTempTangent;
                    }

                    // Right
                    uiOffset = ((x+1) >> 1) + kParentIndexOffset.x +
                            (((y) >> 1) + kParentIndexOffset.y) * uiRowSize;
                    fAveragePosition += kParentPositions.GetComponent(
                        uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                    kParentNormals.GetHighDetail(uiOffset, kScratch);
                    kAverageNormal += kScratch;

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kParentTangents.GetHighDetail(uiOffset, kTempTangent);
                        kAverageTangent += kTempTangent;
                    }
                }
                // Vertical
                else if (yOdd)
                {
                    // Bottom
                    uiOffset = ((x) >> 1) + kParentIndexOffset.x +
                            (((y-1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                    fAveragePosition += kParentPositions.GetComponent(
                        uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                    kParentNormals.GetHighDetail(uiOffset, kScratch);
                    kAverageNormal += kScratch;

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kParentTangents.GetHighDetail(uiOffset, kTempTangent);
                        kAverageTangent += kTempTangent;
                    }

                    // Top
                    uiOffset = ((x) >> 1) + kParentIndexOffset.x +
                            (((y+1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                    fAveragePosition += kParentPositions.GetComponent(
                        uiOffset, NiTerrainPositionRandomAccessIterator::Z);
                    kParentNormals.GetHighDetail(uiOffset, kScratch);
                    kAverageNormal += kScratch;

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kParentTangents.GetHighDetail(uiOffset, kTempTangent);
                        kAverageTangent += kTempTangent;
                    }
                }
                else
                {
                    uiOffset = ((x) >> 1) + kParentIndexOffset.x +
                            (((y) >> 1) + kParentIndexOffset.y) * uiRowSize;

                    kPositions.SetComponent(x + y * uiRowSize,
                        NiTerrainPositionRandomAccessIterator::W,
                        kParentPositions.GetComponent(uiOffset, 
                            NiTerrainPositionRandomAccessIterator::Z));                        
                                       
                    NiPoint2 kNormal;
                    kParentNormals.GetHighDetail(uiOffset, kNormal);
                    kNormals.SetLowDetail(x + y * uiRowSize, kNormal);

                    if (GetConfiguration().IsTangentDataEnabled())
                    {
                        kParentTangents.GetHighDetail(uiOffset, kNormal);
                        kTangents.SetLowDetail(x + y * uiRowSize, kNormal);
                    }

                    continue;
                }

                fAveragePosition *= 0.5f;
                kAverageNormal *= 0.5f;
                kAverageTangent *= 0.5f;

                kPositions.SetComponent(x + y * uiRowSize,
                    NiTerrainPositionRandomAccessIterator::W, fAveragePosition);
                
                kNormals.SetLowDetail(x + y * uiRowSize, kAverageNormal);

                if (GetConfiguration().IsTangentDataEnabled())
                {
                    kTangents.SetLowDetail(x + y * uiRowSize, kAverageTangent);
                }
            }
        }
        
        if (pkLeaf->GetQuadMesh())
            pkLeaf->GetQuadMesh()->MarkMorphBuilt();
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::SetLODScale(float fScale, float fShift)
{
    m_pkSectorData->SetLODScale(fScale, fShift);
    
    NiTerrainBlock* pkBlock;
    NiTerrainDataLeaf* pkLeaf;
    for (NiUInt32 uiIndex = 0; uiIndex < m_kLeafArray.GetSize(); ++uiIndex)
    {
        pkLeaf = GetLeaf(uiIndex);
        pkBlock = pkLeaf->GetQuadMesh();

        pkBlock->UpdateMorphConstants();
    }
}
//---------------------------------------------------------------------------
bool NiTerrainSector::SetLODMode(NiUInt32 uiMode)
{
    if (uiMode == GetLODMode())
        return true;

    if (m_pkSectorData->SetLODMode(uiMode))
    {
        NiTerrainBlock* pkBlock;
        NiTerrainDataLeaf* pkLeaf;
        for (NiUInt32 uiIndex = 0; uiIndex < m_kLeafArray.GetSize(); ++uiIndex)
        {
            pkLeaf = GetLeaf(uiIndex);
            pkBlock = pkLeaf->GetQuadMesh();

            pkBlock->UpdateMorphConstants();
            pkBlock->GetMesh().SetMaterialNeedsUpdate(true);            
        }
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::CalculateUV(NiDataStreamLock* pkUVStream)
{
    // We need to store UV in a small -0.5 -> 0.5 range, to avoid floating
    // point error within the shader
    NiTStridedRandomAccessIterator<NiPoint2> kUVs = 
        pkUVStream->begin<NiPoint2>();

    NiUInt32 uiWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    float fInvWidthInVerts = 1.0f / float(m_pkSectorData->GetBlockSize());
    for (NiUInt32 y = 0; y < uiWidthInVerts; ++y)
    {
        for (NiUInt32 x = 0; x < uiWidthInVerts; ++x)
        {
            NiPoint2& kPoint = kUVs[x + y * uiWidthInVerts];
            kPoint.x = float(x) * fInvWidthInVerts - 0.5f;
            kPoint.y = (float(y) * fInvWidthInVerts - 0.5f) * -1.0f;
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainSector::BuildQuadTree(NiUInt32 uiMaxDetailLevel)
{
    NiTerrainDataLeaf* pkLeaf = 0;
    NiTerrainDataLeaf* pkChild = 0;

    // Initialise the Default Detail level if required:
    if(uiMaxDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiMaxDetailLevel = GetSectorData()->GetNumLOD();
    }
    NiInt32 iLastLevel = GetSectorData()->GetHighestLoadedLOD();

    // Figure out the amount by which the tree needs resizing:
    NiUInt32 uiNumBlocks = 0;
    for (NiUInt32 ui = 0; ui <= uiMaxDetailLevel; ++ui)
        uiNumBlocks += (1 << ui) * (1 << ui);
    m_kLeafArray.SetSize(uiNumBlocks);
    m_kLeafRegionArray.SetSize(uiNumBlocks);

    // Iterate over the leaves that need creating and attaching to the tree
    NiUInt32 uiBeginIndex = m_puiLeafOffsets[iLastLevel + 1];
    NiUInt32 uiEndIndex = uiNumBlocks;
    const NiUInt32 ucOffsetMultiplyX[4] = {0, 1, 1, 0};
    const NiUInt32 ucOffsetMultiplyY[4] = {0, 0, 1, 1};

    for (NiUInt32 uiRegionID = uiBeginIndex; uiRegionID < uiEndIndex; ++uiRegionID)
    {
        NiIndex kBottomLeft;
        NiIndex kSectorIndex;

        if (uiRegionID == 0)
        {
            // Create the base level of detail:
            pkLeaf = NiNew NiTerrainDataLeaf(0, m_pkMetaDataStore);
            pkLeaf->SetRegionID(0);
            pkLeaf->SetContainingSector(this);
            m_pkQuadData = pkLeaf;
        }
        else
        {
            // Fetch the parent of this leaf (It will already exist)
            pkLeaf = GetLeafByRegion((uiRegionID - 1) / 4);
            NiUInt32 uiChildID = (uiRegionID - 1) % 4;

            NiUInt32 uiBlockIndexOffset = (
                m_pkSectorData->GetBlockSize() << pkLeaf->GetNumSubDivisions()) 
                / 2;

            // Create this new leaf:
            pkChild = NiNew NiTerrainDataLeaf(pkLeaf->GetLevel() + 1, 
                m_pkMetaDataStore);
            pkChild->SetParent(pkLeaf);
            pkLeaf->SetChildAt(uiChildID, pkChild);

            pkChild->SetContainingSector(this);

            // Setup the indexing information for this leaf:
            pkLeaf->GetBottomLeftIndex(kBottomLeft);
            kBottomLeft.x += ucOffsetMultiplyX[uiChildID] * uiBlockIndexOffset;
            kBottomLeft.y += ucOffsetMultiplyY[uiChildID] * uiBlockIndexOffset;
            pkChild->SetBottomLeftIndex(kBottomLeft);

            // Figure out the region index of this leaf:
            pkChild->SetRegionID(uiRegionID);

            pkLeaf = pkChild;
        }

        pkLeaf->GetBottomLeftIndex(kBottomLeft);
        NiUInt32 uiBlocksPerSectorSide = 1 << pkLeaf->GetLevel();

        // Data
        kSectorIndex = kBottomLeft / 
            (m_pkSectorData->GetBlockSize() << pkLeaf->GetNumSubDivisions());
        NiUInt32 uiLeafID = kSectorIndex.x +
            kSectorIndex.y * uiBlocksPerSectorSide;

        // The blockID used to store in the array is in LOD space, this means
        // siblings are NOT stored sequentially in the array, instead the whole
        // LOD level is stored sequentially together. In order to parse this
        // information into a proper quad-tree layout, math must be done on this
        // block ID, an example of this can be found in the Save/Load functions
        // It is done this way to speed the lookup of vertices based on their 
        // index into the sector as a whole. 

        // Add this leaf to the correct place in the block array      
        NiUInt32 uiBlockID = m_puiLeafOffsets[pkLeaf->GetLevel()] + uiLeafID;
        pkLeaf->SetBlockID(uiBlockID);

        // Sanity checks
        NIASSERT(uiBlockID < uiNumBlocks);
        NIASSERT(uiRegionID < uiNumBlocks);
        NIASSERT(m_kLeafArray[uiBlockID] == 0);
        NIASSERT(m_kLeafRegionArray[uiRegionID] == 0);

        m_kLeafArray.SetAt(uiBlockID, pkLeaf);
        m_kLeafRegionArray.SetAt(uiRegionID, pkLeaf);

        // Tell the leaf only it's offset from the beginning of it's static stream
        pkLeaf->SetRegionID(uiRegionID - m_puiLeafOffsets[pkLeaf->GetLevel()]);
    }

    // Create the block hierarchy.
    CreateBlockHierarchy();

    // Create the stream iterators. This should be done after blocks are 
    // created, to support 'tool mode'
    uiEndIndex = uiNumBlocks;
    for (NiUInt32 uiBlockID = uiBeginIndex; uiBlockID < uiEndIndex; ++uiBlockID)
    {
        pkLeaf = m_kLeafArray[uiBlockID];

        // Static stream iterators
        NiDataStreamLock* pkTangentLock = NULL;
        if (GetConfiguration().IsTangentDataEnabled())
            pkTangentLock = m_kTangentLocks[pkLeaf->GetLevel()];
        pkLeaf->CreateIterators(
            m_kPositionLocks[pkLeaf->GetLevel()], 
            m_kNormalLocks[pkLeaf->GetLevel()], 
            pkTangentLock, 
            m_pkUVLock, m_pkIndexLock);
    }    
}
//--------------------------------------------------------------------------
void NiTerrainSector::DestroyMesh()
{
    // Destroy the meshes belonging to the blocks
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
    {
        if (!m_kLeafArray[ui])
            continue;
        
        m_kLeafArray[ui]->SetQuadMesh(0);
    }

    m_pkQuadMesh = 0;
    DetachChild(m_pkQuadMesh);

    // Reset the dynamic stream block cache
    m_kDynamicBlockQueue.RemoveAll();

}
//---------------------------------------------------------------------------
void NiTerrainSector::DestroySupportingData()
{
    // Remove any locks
    for (NiUInt32 uiLevel = 0;
        uiLevel < m_kPositionLocks.GetSize(); ++uiLevel)
    {
        // Delete locks
        NiDelete m_kPositionLocks[uiLevel];
        NiDelete m_kNormalLocks[uiLevel];
        NiDelete m_kTangentLocks[uiLevel];

        m_kPositionLocks[uiLevel] = NULL; 
        m_kNormalLocks[uiLevel] = NULL;
        m_kTangentLocks[uiLevel] = NULL;
    }
    NiDelete m_pkIndexLock;
    NiDelete m_pkUVLock;
    m_pkIndexLock = NULL;
    m_pkUVLock = NULL;

    if (m_pkQuadMesh)
        DestroyMesh();

    // Remove stream references
    m_pkSectorData->SetStaticIndexStream(NULL);
    m_pkSectorData->SetStaticUVStream(NULL);
    m_pkSectorData->SetDynamicStreamCache(NULL);
    for (NiUInt32 uiLevel = 0; uiLevel < m_pkSectorData->GetNumLOD() + 1; 
        ++uiLevel)
    {
        m_pkSectorData->SetStaticVertexStream(uiLevel, 0);
        m_pkSectorData->SetStaticNormalTangentStream(uiLevel, 0);
    }
    
    // Delete the quad data tree
    for (NiUInt32 ui = 0; ui < m_kLeafArray.GetSize(); ++ui)
        NiDelete(m_kLeafArray.RemoveAt(ui));
    m_kLeafRegionArray.RemoveAll();

    m_pkQuadData = 0;

    // Delete the shared index regions
    for (NiUInt32 uiLevel = 0; uiLevel < m_pkSectorData->GetNumLOD(); 
        ++uiLevel)
    {
        NiUInt8 uc;
        for (uc = 0; uc < NiTerrainSectorData::NUM_INDEX_REGIONS; ++uc)
        {
            NiDelete m_pkSectorData->GetIndexRegion(uc);
            m_pkSectorData->SetIndexRegion(uc, NULL);
        }
    }

    // Surfaces
    RemoveAllMasks();
    if (m_pkSurfaceMaskStack) 
        NiFree(m_pkSurfaceMaskStack);

    m_pkSurfaceMaskStack = NULL;
    
    if (m_pfBestPixelSpacingCache)
        NiFree(m_pfBestPixelSpacingCache);

    m_pfBestPixelSpacingCache = NULL;
}
//---------------------------------------------------------------------------
void NiTerrainSector::UnloadLODData(NiUInt32 uiUnloadLODLevel)
{
    m_pkSectorData->SetHighestLoadedLOD(NiInt32(uiUnloadLODLevel) - 1);

    // Destroy the data stored at these levels:    
    NiInt32 iLowestIndex = GetLeafOffset(uiUnloadLODLevel);
    NiInt32 iHighestIndex = m_kLeafArray.GetSize() - 1;

    for (NiInt32 iIndex = iHighestIndex; iIndex >= iLowestIndex; --iIndex)
    {
        NiTerrainDataLeaf* pkLeaf = m_kLeafArray.RemoveAt(iIndex); 
        m_kLeafRegionArray.RemoveAt(iIndex);

        // Get this leaf to detach itself from it's parent:
        NiTerrainDataLeaf* pkParent = pkLeaf->GetParent();
        if (pkParent)
        {
            NiTerrainBlock* pkBlock = pkParent->GetQuadMesh();

            for (NiUInt32 ui = 0; ui < 4; ++ui)
            {
                pkParent->SetChildAt(ui, NULL);
                pkBlock->SetBlockChildAt(ui, NULL);
            }

            pkBlock->DetachChild(pkLeaf->GetQuadMesh());
        }
        else
        {
            DetachChild(m_pkQuadMesh);
            m_pkQuadMesh = NULL;
            m_pkQuadData = NULL;
        }

        // Delete the block
        pkLeaf->SetQuadMesh(NULL);

        // Delete the data leafs
        NiDelete(pkLeaf);
    }

    // Close all locks on the streams
    for (NiUInt32 usLevel = uiUnloadLODLevel;
        usLevel < m_kPositionLocks.GetSize(); ++usLevel)
    {
        // Delete locks
        NiDelete m_kPositionLocks[usLevel];
        NiDelete m_kNormalLocks[usLevel];
        NiDelete m_kTangentLocks[usLevel];

        m_kPositionLocks[usLevel] = NULL; 
        m_kNormalLocks[usLevel] = NULL;
        m_kTangentLocks[usLevel] = NULL;
    }

    // Close the streams
    for (NiUInt32 uiLevel = uiUnloadLODLevel; uiLevel < 
        m_pkSectorData->GetNumLOD() + 1; ++uiLevel)
    {
        m_pkSectorData->SetStaticVertexStream(uiLevel, NULL);
        m_pkSectorData->SetStaticNormalTangentStream(uiLevel, NULL);
    }
}
//---------------------------------------------------------------------------
bool NiTerrainSector::LoadLODData(NiUInt32 uiLoadLODLevel, 
    NiUInt32* puiErrorCode, const char* pcArchive)
{
    // Initialize the Default Detail level if required:
    if(uiLoadLODLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiLoadLODLevel = GetSectorData()->GetNumLOD();
    }

    NiUInt32 uiDummyErrorCode = 0;
    if (!puiErrorCode)
        puiErrorCode = &uiDummyErrorCode;

    // Load the appropriate amount of data:
    NiFixedString kArchivePath;
    if (pcArchive)
        kArchivePath = pcArchive;
    else
        kArchivePath = GetArchivePath();

    // Figure out where to load from:
    bool bRes = false;
    if (NiFile::DirectoryExists(kArchivePath))
    {
        // Load from terrain file format:
        bRes = LoadFromFile(kArchivePath, uiLoadLODLevel, puiErrorCode);
        if (bRes)
        {
            // Load the Texture/Metadata information
            LoadSurfaces(GetSurfacePalette(), kArchivePath, uiLoadLODLevel,
                puiErrorCode);
        }        
    }
    else
    {
        *puiErrorCode |= NiTerrainSector::EC_INVALID_ARCHIVE_PATH;
    }
    
    // Reset all the mesh flags to 0
    if (m_pkQuadMesh)
    {
        m_pkQuadMesh->ResetLOD();

        // Signal this max LOD as having been loaded!
        m_pkSectorData->SetHighestLoadedLOD(uiLoadLODLevel);
    }

    return bRes;
}
//---------------------------------------------------------------------------
void NiTerrainSector::AddDecal(NiTerrainDecal* pkDecal)
{
    m_spDecalManager->AddDecal(pkDecal);
}
//---------------------------------------------------------------------------
NiBool NiTerrainSector::GetUsingShortIndexBuffer()
{
    return m_bUsingShortIndexBuffer;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSector::GetUsingShortIndexBuffer() const
{
    return m_bUsingShortIndexBuffer;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSector::HasChanged()
{
    return m_bHasShapeChangedLastUpdate;
}
