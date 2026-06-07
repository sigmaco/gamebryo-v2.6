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

#include "NiTerrainSector.h"
#include "NiTerrainDecal.h"
#include "NiTerrainBlock.h"
#include "NiSurface.h"
#include "NiSurfaceMask.h"
#include "NiTerrain.h"

#ifdef _WII
#include "NiWiiTerrainShader.h"
#endif

NiImplementRTTI(NiTerrainBlock, NiNode);

//---------------------------------------------------------------------------
NiTerrainBlock::NiTerrainBlock(const NiTerrainSectorData* pkSectorData) : 
    NiNode(0),
    m_fMaxDistanceSqr(0),
    m_uiDrawnPoolIndex(0),
    m_pkDrawnPool(0),
    m_bMaskChanged(true),
    m_bMaskLayersChanged(true),
    m_bRequiresUpdate(true),
    m_ucLightingStatus(LIGHTING_OUTDATED),
    m_bMorphOutdated(true),
    m_ucStreamsLockedForWrite(0),
    m_ucStitchingIndex(0),
    m_pkCustomPositionStream(0),
    m_pkCustomNormalTangentStream(0),
    m_pkCustomIndexStream(0),
    m_pkCustomUVStream(0),
    m_pkCustomPositionLock(0),
    m_pkCustomNormalLock(0),
    m_pkCustomTangentLock(0),
    m_pkCustomIndexLock(0),
    m_pkCustomUVLock(0),
    m_pkParent(0),
    m_spMesh(0),
    m_pkSectorData(pkSectorData),
    m_pkDataLeaf(0)
{
    // Initialize child array
    for (NiUInt32 ui = 0; ui < NUM_CHILDREN; ++ui)
        m_apkChildren[ui] = 0;

    for (NiUInt32 ui = 0; ui < STREAM_MAX; ++ui)        
        m_aucLockStatus[ui] = 0;

    m_kSurfaceBlendMasks.SetSize(1);
    m_kSurfaceBlendMasks.SetGrowBy(1);
    m_pkDistributionMask = NULL;

    // Set a zero bound, so that the default application doesn't try to render
    // all existing blocks
    m_kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
}
//---------------------------------------------------------------------------
NiTerrainBlock::NiTerrainBlock(NiTerrainBlock* pkParent, 
    const NiTerrainSectorData* pkSectorData) : 
    NiNode(0),
    m_fMaxDistanceSqr(0),
    m_uiDrawnPoolIndex(0),
    m_pkDrawnPool(0),
    m_bMaskChanged(true),
    m_bMaskLayersChanged(true),
    m_bRequiresUpdate(true),
    m_ucLightingStatus(LIGHTING_OUTDATED),
    m_bMorphOutdated(true),    
    m_ucStreamsLockedForWrite(0),
    m_ucStitchingIndex(0),
    m_pkCustomPositionStream(0),
    m_pkCustomNormalTangentStream(0),
    m_pkCustomIndexStream(0),
    m_pkCustomUVStream(0),
    m_pkCustomPositionLock(0),
    m_pkCustomNormalLock(0),
    m_pkCustomTangentLock(0),
    m_pkCustomIndexLock(0),
    m_pkCustomUVLock(0),
    m_pkParent(pkParent),
    m_spMesh(0),
    m_pkSectorData(pkSectorData),
    m_pkDataLeaf(0)
{
    // Initialize child array
    for (NiUInt32 ui = 0; ui < NUM_CHILDREN; ++ui)
        m_apkChildren[ui] = 0;

    for (NiUInt32 ui = 0; ui < STREAM_MAX; ++ui)
        m_aucLockStatus[ui] = 0;

    m_kSurfaceBlendMasks.SetSize(0);
    m_kSurfaceBlendMasks.SetGrowBy(1);

    m_pkDistributionMask = NULL;

    // Set a zero bound, so that the default application doesn't try to render
    // all existing blocks
    m_kBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
}
//---------------------------------------------------------------------------
NiTerrainBlock::~NiTerrainBlock() 
{
    if (m_ucStreamsLockedForWrite)
    {
        for (NiUInt8 uc = 0; uc < STREAM_MAX; ++uc)
        {
            UnlockDynamicStream(STREAMS(uc), NiDataStream::LOCK_TOOL_WRITE);
        }
    }

    NiDelete m_pkCustomIndexLock;
    NiDelete m_pkCustomPositionLock;
    NiDelete m_pkCustomNormalLock;
    NiDelete m_pkCustomTangentLock;
    NiDelete m_pkCustomUVLock;
    
    m_spMesh = 0;

    // If we are at the top of the tree, we are responsible for destroying
    // some cache objects.
    m_pkDrawnPool->ReleaseValue();
    if (m_pkParent == 0)
        NiDelete m_pkDrawnPool;
    else
        m_pkDrawnPool = 0;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::CreateHierarchy(NiTerrainDataLeaf* pkData)
{
    pkData->SetQuadMesh(this);
    m_pkDataLeaf = pkData;

    m_bRequiresUpdate = true;

    // Check if we need to set our parent:
    if (!GetBlockParent() && pkData->GetParent())
    {
        m_pkParent = pkData->GetParent()->GetQuadMesh();
        NiUInt32 ucChildIndex = (pkData->GetRegionID()) % NUM_CHILDREN;
        
        m_pkParent->m_kChildren.SetSize(NUM_CHILDREN + 1);
        m_pkParent->m_apkChildren[ucChildIndex] = this;
        m_pkParent->AttachChild(this,true);
    }

    // Are we at the top of the hierarchy?
    if (!GetBlockParent())
    {
        // We are the tree root, so we are responsible for the drawn flag pool
        m_pkDrawnPool = NiNew NiUCharPool();
    }
    else
    {
        // Inherit the drawn flag pool from our parent
        m_pkDrawnPool = GetBlockParent()->m_pkDrawnPool;
    }

    // Pre-calculate our distance (local)
    m_fMaxDistanceSqr = NiSqr(
        (float)(m_pkSectorData->GetBlockSize()) * 
        NiPow(2.0f, (float)m_pkDataLeaf->GetNumSubDivisions()) * 
        m_pkSectorData->GetLODScale() * NiSqrt(2.0f));

    // If in tool mode, give ourselves a dynamic stream
    if (m_pkSectorData->InToolMode())
    {
        NiDataStream* pkPositionStream;
        NiDataStream* pkNormalTangentStream;
        
        pkPositionStream = m_pkSectorData->GetDynamicStreamCache()->
            RequestStream(NiDynamicStreamCache::POSITION);
        pkNormalTangentStream = m_pkSectorData->GetDynamicStreamCache()->
            RequestStream(NiDynamicStreamCache::NORMAL_TANGENT);

        NIVERIFY(
            SetDynamicVertexStreams(pkPositionStream, pkNormalTangentStream));
    }
    
    if (m_pkDataLeaf->GetChildAt(0))
    {
        m_kChildren.SetSize(NUM_CHILDREN + 1); 
        for (NiUInt8 ui = 0; ui < NUM_CHILDREN; ++ui)
        {
            m_apkChildren[ui] = NiNew NiTerrainBlock(this, m_pkSectorData);
            AttachChild(m_apkChildren[ui]);
            m_apkChildren[ui]->CreateHierarchy(pkData->GetChildAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainBlock::CreateMesh()
{
    NIASSERT(!m_spMesh);

    m_spMesh = NiNew NiMesh();

    // Attach only to the node children list
    AttachChild(m_spMesh);

    // Assign ourselves a draw pool unique index 
    // (Allocate here so that the indicies are in Level order)
    m_uiDrawnPoolIndex = m_pkDrawnPool->GetNew(); 

#ifdef NIDEBUG    
    // Give the mesh a distinguishable name to assist in debugging:
    NiString kName("");

    NiIndex kIndex;
    m_pkDataLeaf->GetBottomLeftIndex(kIndex);

    kName.Format("Block {ID %d, X %d, Y %d, L %d}", 
        m_pkDataLeaf->GetBlockID(), 
        kIndex.x /(m_pkSectorData->GetBlockSize() << m_pkDataLeaf->GetNumSubDivisions()),
        kIndex.y /(m_pkSectorData->GetBlockSize() << m_pkDataLeaf->GetNumSubDivisions()),
        m_pkDataLeaf->GetNumSubDivisions());

    NiFixedString kFixedName(kName);

    SetName(kFixedName);
    m_spMesh->SetName(kFixedName);
#endif

    m_spMesh->SetSubmeshCount(1);
    m_spMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES);

    NiDataStreamRef* pkStreamRef;

    if (HasDynamicVertexStreams())
    {
        NIASSERT(GetDynamicPositionLockRead());
        NIASSERT(GetDynamicNormalLockRead());

        if (GetConfiguration().IsTangentDataEnabled())
            NIASSERT(GetDynamicTangentLockRead());

        pkStreamRef = m_spMesh->AddStreamRef();
        pkStreamRef->SetDataStream(
            GetDynamicPositionLockRead()->GetDataStream());
        pkStreamRef->BindSemanticToElementDescAt(0, 
            NiCommonSemantics::POSITION(), 0);
        pkStreamRef->BindRegionToSubmesh(0, 0);
        
        pkStreamRef = m_spMesh->AddStreamRef();
        pkStreamRef->SetDataStream(
            GetDynamicNormalLockRead()->GetDataStream());
        pkStreamRef->BindSemanticToElementDescAt(0, 
            NiCommonSemantics::NORMAL(), 0);

        if (GetConfiguration().IsTangentDataEnabled())
        {
            pkStreamRef->BindSemanticToElementDescAt(1, 
                NiCommonSemantics::TANGENT(), 0);
        }

        pkStreamRef->BindRegionToSubmesh(0, 0);
    }
    else
    {
        NiUInt32 uiDetailLevel = m_pkDataLeaf->GetLevel();
        
        NIASSERT(m_pkSectorData->GetStaticPositionStream(uiDetailLevel));        
        NIASSERT(m_pkSectorData->GetStaticNormalTangentStream(uiDetailLevel));

        // Position StreamRef
        pkStreamRef = m_spMesh->AddStreamRef();
        pkStreamRef->SetDataStream(
            m_pkSectorData->GetStaticPositionStream(uiDetailLevel));
        pkStreamRef->BindSemanticToElementDescAt(0,
            NiCommonSemantics::POSITION(), 0);
        pkStreamRef->BindRegionToSubmesh(0, m_pkDataLeaf->GetRegionID());

        // Tangent and normal streamrefs
        pkStreamRef = m_spMesh->AddStreamRef();
        pkStreamRef->SetDataStream(
            m_pkSectorData->GetStaticNormalTangentStream(uiDetailLevel));
        pkStreamRef->BindSemanticToElementDescAt(0, 
            NiCommonSemantics::NORMAL(), 0);

        if (GetConfiguration().IsTangentDataEnabled())
        {
            pkStreamRef->BindSemanticToElementDescAt(1,
                NiCommonSemantics::TANGENT(), 0);
        }

        pkStreamRef->BindRegionToSubmesh(0, m_pkDataLeaf->GetRegionID());
    }

    // Index StreamRef
    pkStreamRef = m_spMesh->AddStreamRef();
    pkStreamRef->SetDataStream(m_pkSectorData->GetStaticIndexStream());
    pkStreamRef->BindSemanticToElementDescAt(
        0, NiCommonSemantics::INDEX(), 0);
    pkStreamRef->BindRegionToSubmesh(0, 0);

    // UV StreamRef
    pkStreamRef = m_spMesh->AddStreamRef();
    pkStreamRef->SetDataStream(m_pkSectorData->GetStaticUVStream());
    pkStreamRef->BindSemanticToElementDescAt(
        0, NiCommonSemantics::TEXCOORD(), 0);
    pkStreamRef->BindRegionToSubmesh(0, 0);

    // Force a zero sized bound so that a client application doesn't try to
    // draw us itself.
    NiBound kZeroBound;
    kZeroBound.SetCenterAndRadius(NiPoint3::ZERO, 0.0f);
    m_spMesh->SetWorldBound(kZeroBound);

    // Eye position extra data
    float fAdjustedEyePos[] = {0.0f, 0.0f, 0.0f};
    m_spMesh->AddExtraData("g_AdjustedEyePos",
        NiNew NiFloatsExtraData(3, fAdjustedEyePos));

    // Surfaces
    m_bMaskChanged = true;
    m_bMaskLayersChanged = true;

    CreateShaderData(false);
}
//---------------------------------------------------------------------------
bool NiTerrainBlock::SetDynamicVertexStreams(NiDataStream* pkPositionStream, 
    NiDataStream* pkNormalTangentStream)
{
    const NiTerrainDataLeaf* pkLeaf = GetDataLeaf();
    NiUInt32 uiDetailLevel = pkLeaf->GetLevel();
    NiUInt32 uiBlockWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiVertsPerBlock = uiBlockWidthInVerts * uiBlockWidthInVerts;

    if (!(pkPositionStream && pkNormalTangentStream))
        return false;

    // Position Stream:
    {
        // Create a region
        NiDataStream::Region kRegion;
        kRegion.SetStartIndex(0);
        kRegion.SetRange(uiVertsPerBlock);
        pkPositionStream->RemoveAllRegions();
        pkPositionStream->AddRegion(kRegion);
    
        // Create the standard read lock
        m_pkCustomPositionLock = NiNew NiDataStreamLock(pkPositionStream, 0, 
            NiDataStream::LOCK_READ);
    
        // Get a lock, and validate the stream
        NiDataStream* pkOldStream = m_pkCustomPositionStream;
        m_pkCustomPositionStream = pkPositionStream;

        NIVERIFY(LockDynamicStream(STREAM_POSITION, NiDataStream::LOCK_WRITE));

        if (!pkPositionStream->GetLocked() || 
            pkPositionStream->GetCount(0) != uiVertsPerBlock)
        {
            UnlockDynamicStream(STREAM_POSITION, NiDataStream::LOCK_WRITE);
            m_pkCustomPositionStream = pkOldStream;
            NiDelete(pkPositionStream);
            return false;
        }
    
        // Attach the stream to the mesh?
        if (m_spMesh)
        {
            NiDataStreamRef* pkStreamRef = m_spMesh->GetStreamRefAt(0);
            NIASSERT(pkStreamRef->GetSemanticNameAt(0).Equals(
                NiCommonSemantics::POSITION()));
            pkStreamRef->Reset();
            pkStreamRef->SetDataStream(pkPositionStream);
            pkStreamRef->BindSemanticToElementDescAt(0,
                NiCommonSemantics::POSITION(), 0);
            pkStreamRef->BindRegionToSubmesh(0, 0);
        }
            
        NiTerrainPositionRandomAccessIterator kDynamicPositionIter =
            NiTerrainPositionRandomAccessIterator(m_pkCustomPositionLock, 0,
            GetConfiguration());

        if (m_pkSectorData->GetStaticPositionStream(uiDetailLevel))
        {
            NiTerrainPositionRandomAccessIterator kStaticPositionIter;
            pkLeaf->GetPositionIterator(kStaticPositionIter);
        
            // Copy data from the static stream to the new dynamic stream
            for (NiUInt32 ui = 0; ui < uiVertsPerBlock; ++ui)
            {
                NiPoint3 kHiDetail = NiPoint3::ZERO;
                NiPoint3 kLoDetail = NiPoint3::ZERO;
                kStaticPositionIter.GetHighDetail(ui, kHiDetail);
                kStaticPositionIter.GetLowDetail(ui, kLoDetail);

                kDynamicPositionIter.SetHighDetail(ui, kHiDetail);
                kDynamicPositionIter.SetLowDetail(ui, kLoDetail);
            }
        }
    }
    
    // Normal/Tangent Stream:
    {
        // Create a region
        NiDataStream::Region kRegion;
        kRegion.SetStartIndex(0);
        kRegion.SetRange(uiVertsPerBlock);
        pkNormalTangentStream->RemoveAllRegions();
        pkNormalTangentStream->AddRegion(kRegion);
    
        // Create the standard read lock
        m_pkCustomNormalLock = NiNew NiDataStreamLock(pkNormalTangentStream, 0, 
            NiDataStream::LOCK_READ);

        if (GetConfiguration().IsTangentDataEnabled())
        {
            m_pkCustomTangentLock = NiNew NiDataStreamLock(
                pkNormalTangentStream, 
                sizeof(float) * GetConfiguration().GetNumNormalComponents(), 
                NiDataStream::LOCK_READ);
        }
    
        // Get a lock, and validate the stream
        NiDataStream* pkOldStream = m_pkCustomNormalTangentStream;
        m_pkCustomNormalTangentStream = pkNormalTangentStream;

        NIVERIFY(LockDynamicStream(STREAM_NORMALTANGENT, 
            NiDataStream::LOCK_WRITE));
        if (!pkNormalTangentStream->GetLocked() || 
            pkNormalTangentStream->GetCount(0) != uiVertsPerBlock)
        {
            UnlockDynamicStream(STREAM_NORMALTANGENT, 
                NiDataStream::LOCK_WRITE);
            m_pkCustomNormalTangentStream = pkOldStream;

            NiDelete(pkNormalTangentStream);
            return false;
        }
    
        // Attach the stream to the mesh
        if (m_spMesh)
        {
            NiDataStreamRef* pkStreamRef = m_spMesh->GetStreamRefAt(1);
            NIASSERT(pkStreamRef->GetSemanticNameAt(0).Equals(
                NiCommonSemantics::NORMAL()));
            pkStreamRef->Reset();
            pkStreamRef->SetDataStream(pkNormalTangentStream);
            pkStreamRef->BindSemanticToElementDescAt(0, 
                NiCommonSemantics::NORMAL(), 0);
            
            if (GetConfiguration().IsTangentDataEnabled())
            {
                pkStreamRef->BindSemanticToElementDescAt(1, 
                    NiCommonSemantics::TANGENT(), 0);
            }

            pkStreamRef->BindRegionToSubmesh(0, 0);
        }
            
        // Since we are storing 2 elements in the stream, the buffer will be
        // interlaced.
        NiTerrainNormalRandomAccessIterator kDynamicNormalIter =
            NiTerrainNormalRandomAccessIterator(m_pkCustomNormalLock, 0,
            GetConfiguration());

        NiTerrainTangentRandomAccessIterator kDynamicTangentIter;
        if (GetConfiguration().IsTangentDataEnabled())
        {
             kDynamicTangentIter = NiTerrainTangentRandomAccessIterator(
                 m_pkCustomTangentLock, 0, GetConfiguration());
        }
    
        if (m_pkSectorData->GetStaticNormalTangentStream(uiDetailLevel))
        {
            NiTerrainNormalRandomAccessIterator kStaticNormalIter;
            NiTerrainTangentRandomAccessIterator kStaticTangentIter;
            pkLeaf->GetNormalIterator(kStaticNormalIter);
            pkLeaf->GetTangentIterator(kStaticTangentIter);
        
            // Copy data from the static stream to the new dynamic stream
            for (NiUInt32 ui = 0; ui < uiVertsPerBlock; ++ui)
            {
                NiPoint3 kHiDetail = NiPoint3::ZERO;
                NiPoint3 kLoDetail = NiPoint3::ZERO;
                kStaticNormalIter.GetHighDetail(ui, kHiDetail);
                kStaticNormalIter.GetLowDetail(ui, kLoDetail);

                kDynamicNormalIter.SetHighDetail(ui, kHiDetail);
                kDynamicNormalIter.SetLowDetail(ui, kLoDetail);

                if (GetConfiguration().IsTangentDataEnabled())
                {
                    kStaticTangentIter.GetHighDetail(ui, kHiDetail);
                    kStaticTangentIter.GetLowDetail(ui, kLoDetail);

                    kDynamicTangentIter.SetHighDetail(ui, kHiDetail);
                    kDynamicTangentIter.SetLowDetail(ui, kLoDetail);
                }
            }
        }
    }
    
    RequestUpdate();

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainBlock::SetDynamicIndexStream(NiDataStream*)
{
    // This will create a clone (copy, not reference) of the static index
    // stream, so that triangles can be flipped?

    // When a terrain is saved out, maybe save this information somehow so that
    // it can be put in the standard index stream when loaded again
    return false;
}
//---------------------------------------------------------------------------
bool NiTerrainBlock::SetDynamicUVStream(NiDataStream*)
{
    return false;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::RemoveDynamicVertexStreams()
{
    NiUInt32 uiDetailLevel = m_pkDataLeaf->GetLevel();

    NIASSERT(m_pkSectorData->GetStaticPositionStream(uiDetailLevel));        
    NIASSERT(m_pkSectorData->GetStaticNormalTangentStream(uiDetailLevel));

    NiUInt8 ucLockType = NiDataStream::LOCK_WRITE;
    UnlockDynamicStream(STREAM_POSITION, ucLockType);
    UnlockDynamicStream(STREAM_NORMALTANGENT, ucLockType);

    NiDelete m_pkCustomPositionLock;
    NiDelete m_pkCustomNormalLock;
    NiDelete m_pkCustomTangentLock;
    m_pkCustomPositionLock = m_pkCustomNormalLock = m_pkCustomTangentLock = 0;

    m_pkCustomPositionStream = 0;
    m_pkCustomNormalTangentStream = 0;

    // We request an update for the decals so they follow the new height 
    // variation
    RequestDecalsUpdate();

    if (!m_spMesh)
        return;
    
    // Now we need to re-bind our mesh to the streams
    NiDataStreamRef* pkStreamRef;

    // Position StreamRef
    pkStreamRef = m_spMesh->GetStreamRefAt(0);
    NIASSERT(pkStreamRef->GetSemanticNameAt(0).Equals(
        NiCommonSemantics::POSITION()));
    pkStreamRef->Reset();
    pkStreamRef->SetDataStream(
        m_pkSectorData->GetStaticPositionStream(uiDetailLevel));
    pkStreamRef->BindSemanticToElementDescAt(
        0, NiCommonSemantics::POSITION(), 0);
    pkStreamRef->BindRegionToSubmesh(0, m_pkDataLeaf->GetRegionID());

    // Tangent and normal streamrefs
    pkStreamRef = m_spMesh->GetStreamRefAt(1);
    NIASSERT(pkStreamRef->GetSemanticNameAt(0).Equals(
        NiCommonSemantics::NORMAL()));
    pkStreamRef->Reset();
    pkStreamRef->SetDataStream(
        m_pkSectorData->GetStaticNormalTangentStream(uiDetailLevel));
    pkStreamRef->BindSemanticToElementDescAt(
        0, NiCommonSemantics::NORMAL(), 0);

    if (GetConfiguration().IsTangentDataEnabled())
    {
        pkStreamRef->BindSemanticToElementDescAt(
            1, NiCommonSemantics::TANGENT(), 0);
    }
    pkStreamRef->BindRegionToSubmesh(0, m_pkDataLeaf->GetRegionID());
}
//---------------------------------------------------------------------------
void NiTerrainBlock::RemoveDynamicIndexStream()
{
    NIASSERT(m_pkSectorData->GetStaticIndexStream());

    NiDelete m_pkCustomIndexLock;
    m_pkCustomIndexLock = 0;
    m_pkCustomIndexStream = 0;

    // We request an update for the decals so they follow the new height 
    // variation
    RequestDecalsUpdate();

    if (!m_spMesh)
        return;
    
    // Now we need to re-bind our mesh to the static stream
    NiDataStreamRef* pkStreamRef;

    // Index StreamRef
    pkStreamRef = m_spMesh->GetStreamRefAt(2);
    NIASSERT(pkStreamRef->GetSemanticNameAt(0).Equals(
        NiCommonSemantics::INDEX()));
    pkStreamRef->Reset();
    pkStreamRef->SetDataStream(m_pkSectorData->GetStaticIndexStream());
    pkStreamRef->BindSemanticToElementDescAt(
        0, NiCommonSemantics::INDEX(), 0);
    pkStreamRef->BindRegionToSubmesh(0, 0);
}
//---------------------------------------------------------------------------
void NiTerrainBlock::RemoveDynamicUVStream()
{
    NIASSERT(m_pkSectorData->GetStaticUVStream());

    NiDelete m_pkCustomUVLock;
    m_pkCustomUVLock = 0;
    m_pkCustomUVStream = 0;

    if (!m_spMesh)
        return;
    
    // Now we need to re-bind our mesh to the static stream
    NiDataStreamRef* pkStreamRef;

    // UV StreamRef
    pkStreamRef = m_spMesh->GetStreamRefAt(3);
    NIASSERT(pkStreamRef->GetSemanticNameAt(0).Equals(
        NiCommonSemantics::TEXCOORD()));
    pkStreamRef->Reset();
    pkStreamRef->SetDataStream(m_pkSectorData->GetStaticUVStream());
    pkStreamRef->BindSemanticToElementDescAt(
        0, NiCommonSemantics::TEXCOORD(), 0);
    pkStreamRef->BindRegionToSubmesh(0, 0);
}
//---------------------------------------------------------------------------
NiDataStream* NiTerrainBlock::GetDynamicStream(STREAMS eStream)
{
    if (eStream == STREAM_POSITION)
        return m_pkCustomPositionStream;
    else if (eStream == STREAM_NORMALTANGENT)
        return m_pkCustomNormalTangentStream;
    else if (eStream == STREAM_INDEX)
        return m_pkCustomIndexStream;
    else if (eStream == STREAM_UV)
        return m_pkCustomUVStream;
    else
        return 0;
}
//---------------------------------------------------------------------------
void* NiTerrainBlock::LockDynamicStream(STREAMS eStream, NiUInt8 ucLockMask)
{
    NiDataStream* pkStream = GetDynamicStream(eStream);
    if (!pkStream)
        return 0;

    ucLockMask -= m_aucLockStatus[eStream] & ucLockMask;

    if (!ucLockMask)
        return 0;
    
    void* pvData = pkStream->Lock(ucLockMask);
    if (!pvData)
        return 0;

    RequestUpdate();

    m_aucLockStatus[eStream] |= ucLockMask;

    if (ucLockMask & NiDataStream::LOCK_WRITE)
    {
        m_ucStreamsLockedForWrite |= 1 << eStream;
    }

    return pvData;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::UnlockDynamicStream(STREAMS eStream, NiUInt8 ucLockMask)
{
    NiDataStream* pkStream = GetDynamicStream(eStream);
    if (!pkStream)
        return;

    ucLockMask = ucLockMask & m_aucLockStatus[eStream];
    if (!ucLockMask)
        return;

    pkStream->Unlock(ucLockMask);
    m_aucLockStatus[eStream] &= ~ucLockMask;

    if (ucLockMask & NiDataStream::LOCK_WRITE)
    {
        m_ucStreamsLockedForWrite &= 
            ~((NiUInt8)(m_ucStreamsLockedForWrite & (1 << eStream)));
    }
}
//--------------------------------------------------------------------------
void NiTerrainBlock::DestroyMesh()
{
    NiDelete m_pkCustomIndexLock;
    NiDelete m_pkCustomPositionLock;
    NiDelete m_pkCustomNormalLock;
    NiDelete m_pkCustomTangentLock;
    m_pkCustomIndexLock = m_pkCustomPositionLock = 0;
    m_pkCustomNormalLock = m_pkCustomTangentLock = 0;

    // Surface blend masks
    NiUInt8 ucNumBlendMasks = (NiUInt8)m_kSurfaceBlendMasks.GetSize();
    NiDynamicTexturePtr spTexture = 0;
    for (NiUInt8 uc = 0; uc < ucNumBlendMasks; ++uc)
    {
        // By using a smart pointer, we can intelligently delete the mask
        // if it isn't reference by the mesh
        spTexture = m_kSurfaceBlendMasks.RemoveAt(uc);
        spTexture = 0;
    }

    m_spMesh = 0;
}
//---------------------------------------------------------------------------
const NiTerrainConfiguration& NiTerrainBlock::GetConfiguration() const
{
    return GetDataLeaf()->GetContainingSector()->GetConfiguration();
}
//--------------------------------------------------------------------------
NiTexturingProperty* NiTerrainBlock::GetTexturingProperty() 
{
    NiTexturingProperty* pkProperty = 
        (NiTexturingProperty*)m_spMesh->GetProperty(
            NiTexturingProperty::GetType());

    if (!pkProperty)
    {
        NIVERIFY(CreateShaderData(false));
        pkProperty = (NiTexturingProperty*)m_spMesh->GetProperty(
            NiTexturingProperty::GetType());

        NIASSERT(pkProperty);
    }

    return pkProperty;
}
//--------------------------------------------------------------------------
bool NiTerrainBlock::CreateShaderData(bool bInheritingMask)
{
    NIASSERT(m_spMesh);

    // Handle the fog property if fog is enabled on the terrain only if we 
    // are not in tool mode.
    if (!m_pkSectorData->InToolMode())
    {
        NiFogProperty* pkFogProperty = NiDynamicCast(NiFogProperty, 
            m_spMesh->GetProperty(NiFogProperty::GetType()));
        if (!pkFogProperty)
            m_spMesh->AttachProperty(
            NiTerrain::GetGlobalFogProperty());
    }    

    if (m_spMesh->GetExtraDataSize() <= 1)
    {
        // Initialize the shader constants
        float fOffsetBlend[] = { 0.5f, 0.5f };
        float fScaleBlend[] = { 1.0f, 1.0f };
        
        m_spMesh->AddExtraData(
            NiTerrainMaterial::UVBLENDOFFSET_SHADER_CONSTANT, 
            NiNew NiFloatsExtraData(2, fOffsetBlend));
        m_spMesh->AddExtraData(NiTerrainMaterial::UVBLENDSCALE_SHADER_CONSTANT,
            NiNew NiFloatsExtraData(2, fScaleBlend));
        
        // Since UV are shared across all blocks, we need to create a scale
        // factor.
        float fGlobalScaleSurface[] = { 1.0f, 1.0f };
        float fGlobalOffsetSurface[] = { 0.0f, 0.0f };
            
        m_spMesh->AddExtraData(
            NiTerrainMaterial::UVSURFACEOFFSET_SHADER_CONSTANT,
            NiNew NiFloatsExtraData(2, fGlobalOffsetSurface));
        m_spMesh->AddExtraData(
            NiTerrainMaterial::UVSURFACESCALE_SHADER_CONSTANT, 
            NiNew NiFloatsExtraData(2, fGlobalScaleSurface));

        // LOD information 
        float fStitchingInfo[] = {0,0,0,0};
        m_spMesh->AddExtraData(
            NiTerrainMaterial::STITCHINGINFO_SHADER_CONSTANT, 
            NiNew NiFloatsExtraData(4, fStitchingInfo));
        m_spMesh->AddExtraData(NiTerrainMaterial::MORPHMODE_SHADER_CONSTANT, 
            NiNew NiIntegerExtraData(m_pkSectorData->GetLODMode()));

        NiUInt32 uiLodLevel = m_pkDataLeaf->GetNumSubDivisions();
        float fBlockWorldWidth = 
            float(m_pkSectorData->GetBlockSize() << uiLodLevel);

        float fMorphDistance = NiSqrt(2.0f) * (fBlockWorldWidth * 0.5f);
        float fThresholdDistance = NiSqrt(m_fMaxDistanceSqr) - fMorphDistance;

        m_spMesh->AddExtraData(NiTerrainMaterial::LODTHRESHOLD_SHADER_CONSTANT,
            NiNew NiFloatExtraData(fThresholdDistance));
        m_spMesh->AddExtraData(
            NiTerrainMaterial::LODMORPHDISTANCE_SHADER_CONSTANT,
            NiNew NiFloatExtraData(fMorphDistance));
        
        // Add each surface's constants to the shader:
        const NiInt32 iNumSurfaces = 4;
        
        float afSurfaceUVModifiers[4 * iNumSurfaces];
        float afDistMaskStr[iNumSurfaces];
        for (NiInt32 ui = 0; ui < iNumSurfaces; ++ui)
        {
            // Default values to set the constants to:
            //UV Scale and Offset        
            NiPoint2 kScale(1,1); 
            NiPoint2 kOffset(0,0);
            afSurfaceUVModifiers[ui * 4 + 0] = kScale.x;
            afSurfaceUVModifiers[ui * 4 + 1] = kScale.y;
            afSurfaceUVModifiers[ui * 4 + 2] = kOffset.x;
            afSurfaceUVModifiers[ui * 4 + 3] = kOffset.y;

            afDistMaskStr[ui] = 1.0f;
        }

        m_spMesh->AddExtraData(
            NiTerrainMaterial::UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT,
            NiNew NiFloatsExtraData(4 * iNumSurfaces, afSurfaceUVModifiers));

        m_spMesh->AddExtraData(
            NiTerrainMaterial::DISTMASKSTR_SHADER_CONSTANT,
            NiNew NiFloatsExtraData(iNumSurfaces, afDistMaskStr));
    }

    if (m_spMesh->GetProperty(NiTexturingProperty::TEXTURING) == NULL)
    {
        NiTexturingProperty* pkTextureProp;

        // We don't inherit texturing properties when at maximum detail level,
        // only masks. This is to support shadows at max detail.
        if (bInheritingMask && m_pkDataLeaf->GetLevel())    
            pkTextureProp = m_pkParent->GetTexturingProperty();
        else    
            pkTextureProp = NiNew NiTexturingProperty;

        // Finally, attach the property to the mesh
        m_spMesh->AttachProperty(pkTextureProp);
        m_spMesh->UpdateProperties();
    }

#ifdef _WII
    NiMaterial* pkMaterial = NiMaterial::GetMaterial("NiWiiTerrainShader");
    if (!pkMaterial)
        pkMaterial = NiSingleShaderMaterial::Create(NiNew NiWiiTerrainShader);
#else
    NiTerrainMaterial* pkMaterial = NiTerrainMaterial::Create();
#endif

    NIASSERT(pkMaterial);  
    m_spMesh->ApplyAndSetActiveMaterial(pkMaterial);

    return true;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::UpdateMasks() 
{
    NiDynamicTexture* pkBlendTexture = NULL;
    NiSourceTexture* pkDistMaskTexture = NULL;
    NiTerrainBlock* pkMasterBlock = this;
    NiUInt32 uiSurfaceCount = pkMasterBlock->GetDataLeaf()->GetSurfaceCount();

    if (m_spMesh)
        m_spMesh->SetMaterialNeedsUpdate(true);

    // Find the first block above us with a valid mask
    while (uiSurfaceCount == 0)
    {   
        if (!pkMasterBlock->GetBlockParent())
            break;

        pkMasterBlock = pkMasterBlock->GetBlockParent();
        uiSurfaceCount = pkMasterBlock->GetDataLeaf()->GetSurfaceCount();
    }

    NIASSERT(pkMasterBlock);
    const NiTerrainDataLeaf* pkMasterData = pkMasterBlock->GetDataLeaf();
    bool bInheritingMask = pkMasterBlock != this;
    
    bool bInheritingTextureProp = bInheritingMask && 
        m_pkDataLeaf->GetNumSubDivisions() > 0;

    if (bInheritingTextureProp)
    {
        // Only proceed if our parent succeeded in creating textures, to
        // prevent re-displaying error messages
        NiProperty* pkTextureProp = pkMasterBlock->m_spMesh->GetProperty(
            NiTexturingProperty::GetType());

        if (pkTextureProp)
        {
            m_spMesh->DetachProperty(GetTexturingProperty());
            m_spMesh->AttachProperty(pkTextureProp);
            m_spMesh->UpdateProperties();
        }
        else
        {
            return;
        }
    }      

    if (!bInheritingMask) 
    {
        if (m_bMaskLayersChanged)
        {
            // We need to merge all of the distribute masks from each surface into 
            // a single 4 channel texture. That four channel texture is then 
            // assigned to the texturing property. The distribution masks for each 
            // layer need to be the same size in order for the technique to work. 
            // This basically implies all textures for all layers need to be the 
            // same size.
            const NiSurface* pkSurface = pkMasterData->GetSurface(0);
            
            // Used to track which layers have valid mask channels.
            NiInt32 aiDistMaskLayers[4] = {0, 0, 0, 0};
#ifndef _WII
            if (pkSurface)
            {
                NiTexture* pkBaseTex = pkSurface->GetTexture(
                    NiSurface::SURFACE_MAP_DIFFUSE);

                NIASSERT(pkBaseTex);
                NiUInt32 uiMapWidth = pkBaseTex->GetWidth();
                NiUInt32 uiMapHeight = pkBaseTex->GetHeight();
                NiPixelDataPtr spPixelData = NiNew NiPixelData(uiMapWidth, 
                    uiMapHeight, NiPixelFormat::RGBA32);

                NiUInt32 uiSurfaceCount = pkMasterData->GetSurfaceCount();
                bool bHasDistMask = false;

                for (NiUInt32 uiCurrSurface = 0; uiCurrSurface < uiSurfaceCount;
                    uiCurrSurface++)
                {
                    pkSurface = pkMasterData->GetSurface(uiCurrSurface);
                    NIASSERT(pkSurface);

                    NiSourceTexture* pkDistMask = NiDynamicCast(NiSourceTexture, 
                        pkSurface->GetTexture(NiSurface::SURFACE_MAP_DISTRIBUTION));
                    if (!pkDistMask)
                        continue;

                    // If the dimensions are not equal to the base texture in 
                    // surface zero, it is invalid.
                    NiUInt32 uiMaskWidth = pkDistMask->GetWidth();
                    NiUInt32 uiMaskHeight = pkDistMask->GetHeight();
                    if (uiMaskWidth != uiMapWidth || uiMaskHeight != uiMapHeight)
                        continue;

                    // If we get here, at least one surface has a distribution mask.
                    bHasDistMask = true;
                    aiDistMaskLayers[uiCurrSurface] = 1;

                    // Copy the pixels from the mask channel to the correct channel 
                    // in the combined mask texture.
                    NIASSERT(pkDistMask->GetSourcePixelData());
                    NiUInt8* pkMaskPixels = 
                        pkDistMask->GetSourcePixelData()->GetPixels();
                    NIASSERT(pkMaskPixels);

                    NiUInt8* pkCombinedMaskPixels = 
                        spPixelData->GetPixels() + uiCurrSurface;
                    NIASSERT(pkCombinedMaskPixels);

                    NiUInt32 uiSrcStride = 
                        pkDistMask->GetSourcePixelData()->GetPixelStride();
                    NiUInt32 uiDestStride = spPixelData->GetPixelStride();
                    NIASSERT(uiDestStride == 4);                
                    for (NiUInt32 uiY = 0; uiY < uiMapHeight; uiY++)
                    {
                        for (NiUInt32 uiX = 0; uiX < uiMapWidth; uiX++)
                        {
                            *pkCombinedMaskPixels = *pkMaskPixels;
                            pkMaskPixels += uiSrcStride;
                            pkCombinedMaskPixels += uiDestStride;
                        }
                    }
                }

                if (!bHasDistMask)
                {
                    spPixelData = NULL;
                }
                else
                {
                    NiTexture::FormatPrefs kFormatPrefs;

                    kFormatPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;
                    kFormatPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
                    kFormatPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
                    pkDistMaskTexture = NiSourceTexture::Create(
                        spPixelData, kFormatPrefs);

                    pkDistMaskTexture->AddExtraData(NiNew NiIntegersExtraData(
                        4, &aiDistMaskLayers[0])); 
                    NIASSERT(pkDistMaskTexture);

                    SetCombinedDistributionMask(pkDistMaskTexture);
                }
            }
#endif // #ifndef _WII
        }

        // We have a unique set of masks for this leaf

        // Has the mask changed? This happens when any mask we are using has 
        // been painted too.
        if (m_bMaskChanged)
        {
            // Were we previously inheriting our mask and shader from a parent?
            if (!GetBlendTexture(0))
            {
                m_spMesh->DetachProperty(GetTexturingProperty());
                m_spMesh->RemoveAllExtraData();

                // Eye position extra data
                float fAdjustedEyePos[] = {0.0f, 0.0f, 0.0f};
                m_spMesh->AddExtraData("g_AdjustedEyePos",
                    NiNew NiFloatsExtraData(3, fAdjustedEyePos));

                bool bShaderCreated = CreateShaderData(false);
                NIASSERT(bShaderCreated);
                if (!bShaderCreated)
                    return;
            }

            // We have our own set of masks, generate the blend.
            pkBlendTexture = GetBlendTexture(0);

            // Make sure the blend texture is of high enough resolution
            if (pkBlendTexture)
            {
                if (pkBlendTexture->GetWidth() != 
                    m_pkDataLeaf->GetSurfaceMaskWidth())
                {
                    // Resize the bled texture to fit the mask dimensions
                    pkBlendTexture = ResizeBlendTexture(0);
                }
            } 
            else 
            {
                // Make sure we have a valid texturing property
                GetTexturingProperty();

                // Create the blend texture
                pkBlendTexture = CreateBlendTexture(0);
            }

            NIASSERT(pkBlendTexture);

            // Get the pixels
            NiInt32 iPitch = 0;
            NiUInt32* puiPtr = (NiUInt32*)(pkBlendTexture->Lock(iPitch));
            NIASSERT(puiPtr);

            const NiPixelFormat* pkFmt = pkBlendTexture->GetPixelFormat();

#if defined(_WII)
            // The Wii packing logic assumes RGBA32 formatted dynamic texture
            NIASSERT( pkFmt->FunctionallyIdentical(NiPixelFormat::RGBA32, true));
#endif
            static const NiUInt32 uiChannels[] = 
            {
                NiPixelFormat::COMP_RED,
                NiPixelFormat::COMP_GREEN,
                NiPixelFormat::COMP_BLUE,
                NiPixelFormat::COMP_ALPHA
            };

            /*
                This code simply places the value in the mask into the texture.
                This code assumes that the values have already been processed
                when added to the surface mask in terrain sector.
             */
            NiUInt32 uiMaxY = pkBlendTexture->GetHeight() - 1;
#if defined(_WII)
            NiUInt32 uiWidth = pkBlendTexture->GetWidth();
#endif

            for (NiInt32 uiY = uiMaxY; uiY >= 0; --uiY) 
            {
                for (NiUInt32 uiX = 0; uiX <= uiMaxY; ++uiX) 
                {
                    NiUInt32 uiPixelValue = 0;
                    NiUInt32 uiMaskSum = 0;
                    for (NiUInt32 uiC = 0; uiC < uiSurfaceCount; ++uiC)
                    {
                        uiMaskSum += pkMasterData->GetSurfaceMask(uiC)->GetAt(
                            uiX, uiY);
                    }

                    float fInvMaskSum = 1.0f;
                    if (uiMaskSum > 0)
                        fInvMaskSum = 1.0f / (float)uiMaskSum;

                    for (NiUInt32 uiC = 0; uiC < uiSurfaceCount; ++uiC)
                    {
                        float fFactor = fInvMaskSum * (float)
                            pkMasterData->GetSurfaceMask(uiC)->GetAt(uiX, uiY);
                        NiUInt8 ucValue = (NiUInt8)(fFactor * 255.0f);

                        uiPixelValue |= (ucValue << pkFmt->GetShift((
                            NiPixelFormat::Component)uiChannels[uiC]));
                    }

#if !defined(_WII)
                    *puiPtr++ = uiPixelValue;
#else
                    // Pack the pixel into the correct tiles for the Wii
                    unsigned int uiTile = (uiY / 4) * (uiWidth / 2) + (uiX / 4 * 2);
                    unsigned short* pTile = (unsigned short*)((unsigned char*)puiPtr +
                        (uiTile * 32) + 8 * (uiY % 4) + 2 * (uiX % 4));

                    // Convert RGBA to ARGB since tiling divides colors into AR and GB
                    // Assumes RGBA32 format (see NIASSERT above)
                    unsigned long uiARGB = (uiPixelValue>>8) | ((uiPixelValue & 0x00FF)<<24);

                    *(pTile) = uiARGB >> 16;
                    *(pTile + 16) = uiARGB;
#endif
                }
            }

            pkBlendTexture->UnLock();
        }
    }
    else if (!bInheritingTextureProp)
    {
        // We are inheriting our mask but we have a unique texture property.
        pkBlendTexture = pkMasterBlock->GetBlendTexture(0);
    }

    if (!pkDistMaskTexture && pkMasterData->GetSurfaceCount())
        pkDistMaskTexture = pkMasterBlock->GetCombinedDistributionMask();

    if (m_bMaskLayersChanged)    
    {  
        // Figure out the texture offset for this block relative to the
        // Block that holds the blend texture.
        NiIndex kLocalIndex = m_pkDataLeaf->SectorIndexFromLocal(
            NiIndex::ZERO);
        NiIndex kMasterIndex = pkMasterData->SectorIndexFromLocal(
            NiIndex::ZERO);

        NiUInt32 uiSize = m_pkSectorData->GetBlockSize();

        NiUInt32 uiBlendScale = 
            1 << (pkMasterData->GetNumSubDivisions() - 
            m_pkDataLeaf->GetNumSubDivisions());

        NiUInt32 uiMasterIndexWidth = 
            (1 << (pkMasterData->GetNumSubDivisions())) * uiSize;

        float fLeafScale = (float) uiBlendScale;
        
        NiPoint2 kBlendOffset = NiPoint2(
            (float)(kLocalIndex.x - kMasterIndex.x),
            (float)(kLocalIndex.y - kMasterIndex.y));
        kBlendOffset /= (float)uiMasterIndexWidth;
        kBlendOffset.y = 1.0f - kBlendOffset.y;

        // Account for UV ranging from -0.5 to 0.5
        kBlendOffset.x += 1.0f / (2.0f * fLeafScale);
        kBlendOffset.y -= 1.0f / (2.0f * fLeafScale);
        
        // Ensure that our shader has actually been created - for cases where
        // we are inheriting a mask
        bool bShaderCreated = CreateShaderData(bInheritingMask);
        if (!bShaderCreated)
            return;

        NiFloatsExtraData* pkOffsetData;
        NiFloatsExtraData* pkScaleData;
        NiFloatsExtraData* pkSurfaceUVModifiers;
        NiFloatsExtraData* pkDistMaskStrength;

        // Blend texture coordinates and scale
        pkOffsetData = (NiFloatsExtraData*)m_spMesh->GetExtraData(
            NiTerrainMaterial::UVBLENDOFFSET_SHADER_CONSTANT);

        pkScaleData = (NiFloatsExtraData*)m_spMesh->GetExtraData(
            NiTerrainMaterial::UVBLENDSCALE_SHADER_CONSTANT);

        pkSurfaceUVModifiers = (NiFloatsExtraData*)m_spMesh->GetExtraData(
            NiTerrainMaterial::UVSURFACEUVMODIFIERARRAY_SHADER_CONSTANT);

        pkDistMaskStrength = (NiFloatsExtraData*)m_spMesh->GetExtraData(
            NiTerrainMaterial::DISTMASKSTR_SHADER_CONSTANT);

        NIASSERT(pkOffsetData && pkScaleData && pkDistMaskStrength && 
            "Shader must be initialized before updating texture coordinates");

        // Blend texture coordinates and scale
        pkOffsetData->SetValue(0, kBlendOffset.x);
        pkOffsetData->SetValue(1, kBlendOffset.y);
        pkScaleData->SetValue(0, 1.0f / fLeafScale);
        pkScaleData->SetValue(1, 1.0f / fLeafScale);

        // Specular color
        NiUInt32 uiNumSurfaces = pkMasterData->GetSurfaceCount();
        float* pfSurfaceUVModifiers = NiAlloc(float, 4 * uiNumSurfaces);
        float afDistMaskStrengths[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        for (NiUInt32 uiC = 0; uiC < uiNumSurfaces; ++uiC)
        {
            const NiSurface* pkSurface = pkMasterData->GetSurface(uiC);
    
            //UV Scale and Offset        
            NiPoint2 kScale = pkSurface->GetUVScale();
            NiPoint2 kOffset = pkSurface->GetUVOffset();
            pfSurfaceUVModifiers[uiC * 4 + 0] = kScale.x;
            pfSurfaceUVModifiers[uiC * 4 + 1] = kScale.y;
            pfSurfaceUVModifiers[uiC * 4 + 2] = kOffset.x;
            pfSurfaceUVModifiers[uiC * 4 + 3] = kOffset.y;

            NIASSERT(uiC < 4);
            if (pkSurface->GetTexture(NiSurface::SURFACE_MAP_DISTRIBUTION))
                afDistMaskStrengths[uiC] = pkSurface->GetDistributionMaskStrength();
        }

        pkSurfaceUVModifiers->SetArray(4 * uiNumSurfaces, 
            pfSurfaceUVModifiers);

        pkDistMaskStrength->SetArray(4, &afDistMaskStrengths[0]);

        NiFree(pfSurfaceUVModifiers);
    }    

    if (!bInheritingTextureProp && m_bMaskChanged)
    {
        // Set all the surfaces and the mask to be sent to the shader.
        NiTexturingProperty* pkTextureProp = GetTexturingProperty();
        NIASSERT(pkTextureProp);

        NiUInt32 uiShaderArrayCount = pkTextureProp->GetShaderArrayCount();
        for (NiUInt32 ui = 0; ui < uiShaderArrayCount; ui++)
            pkTextureProp->SetShaderMap(ui, NULL);

        // Set all the appropriate blend maps: (maximum 1 currently)
        NiUInt32 uiCurrShaderMap = 0;
        NiUInt32 uiNumBlendMaps = 0;
        if (uiSurfaceCount)
            uiNumBlendMaps = 1;
        for (NiUInt32 uiCurrBlendMap = 0; uiCurrBlendMap < uiNumBlendMaps; 
            ++uiCurrBlendMap)
        {
            NiTexturingProperty::ShaderMap* pkBlendShaderMap = 
                NiNew NiTexturingProperty::ShaderMap(pkBlendTexture, 0,
                NiTexturingProperty::CLAMP_S_CLAMP_T, 
                NiTexturingProperty::FILTER_BILERP, 
                NiTerrainMaterial::BLEND_MAP);

            pkTextureProp->SetShaderMap(uiCurrShaderMap++, pkBlendShaderMap);
        }

        if (pkDistMaskTexture)
        {
            NiTexturingProperty::ShaderMap* pkDistributionMaskMap = 
                NiNew NiTexturingProperty::ShaderMap(pkDistMaskTexture, 0, 
                NiTexturingProperty::WRAP_S_WRAP_T, 
                NiTexturingProperty::FILTER_TRILERP, 
                NiTerrainMaterial::COMBINED_DISTRIBUTION_MASK);

            pkTextureProp->SetShaderMap(uiCurrShaderMap++, pkDistributionMaskMap);
        }
        else
        {
            pkTextureProp->SetShaderMap(uiCurrShaderMap++, NULL);
        }

        // Set the shader maps of the textures to be blended.
        for (NiUInt32 uiCurrSurface = 0; uiCurrSurface < uiSurfaceCount;
            uiCurrSurface++)
        { 
            // Apply existing textures. If a texture does not exist for a given
            // layer, a NULL shader map is inserted instead.
            const NiSurface* pkSurface = pkMasterData->GetSurface(
                uiCurrSurface);
            NIASSERT(pkSurface);

            // We have a normal map and base map for each layer, plus 2 additional
            // shader maps for all layers. (blend mask and packed distribution mask)
            if (pkDistMaskTexture)
                uiCurrShaderMap = (uiCurrSurface * 3) + 2;
            else
                uiCurrShaderMap = (uiCurrSurface * 3) + 1;

            NiTexture* pkBaseMap = pkSurface->GetTexture(
                NiSurface::SURFACE_MAP_DIFFUSE);

            // Must at least have a base map for this layer to be valid.
            if (!pkBaseMap)
            {
                pkTextureProp->SetShaderMap(uiCurrShaderMap++, NULL);
                pkTextureProp->SetShaderMap(uiCurrShaderMap++, NULL);
                continue;
            }

            NiTexturingProperty::ShaderMap* pkShaderMap = 
                NiNew NiTexturingProperty::ShaderMap(pkBaseMap, 0, 
                NiTexturingProperty::WRAP_S_WRAP_T, 
                NiTexturingProperty::FILTER_TRILERP, 
                NiTerrainMaterial::BASE_MAP);

            pkTextureProp->SetShaderMap(uiCurrShaderMap++, pkShaderMap);

            // Check for the normal map.
            NiTexture* pkNormalMap = pkSurface->GetTexture(
                NiSurface::SURFACE_MAP_NORMAL);
            if (pkNormalMap)
            {
                pkShaderMap = NiNew NiTexturingProperty::ShaderMap(pkNormalMap,
                    0, NiTexturingProperty::WRAP_S_WRAP_T, 
                    NiTexturingProperty::FILTER_TRILERP, 
                    NiTerrainMaterial::NORMAL_MAP);
                
                pkTextureProp->SetShaderMap(uiCurrShaderMap++, pkShaderMap);
            }
            else
            {
                pkTextureProp->SetShaderMap(uiCurrShaderMap++, NULL);
            }
        }

        // Set the remaining shader map slots to 0 (ie. remove layers)
        for ( ; uiCurrShaderMap < pkTextureProp->GetShaderArrayCount(); 
            ++uiCurrShaderMap)
        {
            pkTextureProp->SetShaderMap(uiCurrShaderMap, NULL);
        }
    }
}
//---------------------------------------------------------------------------
NiDynamicTexture* NiTerrainBlock::GetBlendTexture(NiUInt32 uiIndex)
{
    if (uiIndex >= m_kSurfaceBlendMasks.GetSize())
        return 0;

    return m_kSurfaceBlendMasks.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiDynamicTexture* NiTerrainBlock::CreateBlendTexture(NiUInt32 uiIndex)
{    
    NIASSERT(uiIndex == 0 && "Terrain currently only supports 1 blend mask.");

    NiDynamicTexture* pkBlendTexture = GetBlendTexture(uiIndex);
    if (pkBlendTexture)
        return pkBlendTexture;

    // Create the blend texture
    NiUInt32 uiBlendMaskWidth = m_pkDataLeaf->GetSurfaceMaskWidth();

    // Cases where there are no surfaces on a block, but we still need to
    // generate a material
    if (uiBlendMaskWidth == 0)
        uiBlendMaskWidth = 2;
    
    NiTexture::FormatPrefs kFormatPrefs;
    kFormatPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;
    kFormatPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kFormatPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;

#if defined(_WII)
    // Dynamic textures on the Wii must be tiled
    const bool bTiled = true;
#else
    const bool bTiled = false;
#endif

    pkBlendTexture = NiDynamicTexture::Create(uiBlendMaskWidth, 
        uiBlendMaskWidth, kFormatPrefs, bTiled);

    m_kSurfaceBlendMasks.SetAtGrow(0, pkBlendTexture);
    return pkBlendTexture;
}
//---------------------------------------------------------------------------
NiDynamicTexture* NiTerrainBlock::ResizeBlendTexture(NiUInt32 uiIndex)
{
    NiDynamicTexture* pkTexture = GetBlendTexture(uiIndex);

    if (pkTexture)
    {
        NiUInt32 uiBlendMaskWidth = m_pkDataLeaf->GetSurfaceMaskWidth();
        if (uiBlendMaskWidth == 0)
            uiBlendMaskWidth = 2;

        if (uiBlendMaskWidth == pkTexture->GetWidth())
            return pkTexture;
        
        // Attempt to keep valid pointers, by swapping renderer data.
        NiTexture::FormatPrefs kFormatPrefs;

        kFormatPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
        kFormatPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
        kFormatPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;

#if defined(_WII)
        // Dynamic textures on the Wii must be tiled
        const bool bTiled = true;
#else
        const bool bTiled = false;
#endif

        NiDynamicTexture* pkNewTexture = NiDynamicTexture::Create(
            uiBlendMaskWidth, uiBlendMaskWidth, kFormatPrefs, bTiled);

        m_kSurfaceBlendMasks.SetAt(uiIndex, pkNewTexture);

        return pkNewTexture;
    }
                
    return CreateBlendTexture(uiIndex);
}
//---------------------------------------------------------------------------
void NiTerrainBlock::MarkSurfaceMasksChanged(bool bCalcUV)
{
    m_bMaskChanged = true;

    // Now, children only update if there the child is inheriting our mask.
    if (bCalcUV)
    {
        m_bMaskLayersChanged = true;
        for (NiUInt32 ui = 0; ui < 4; ui++)
        {
            if (GetBlockChildAt(ui) && 
                GetBlockChildAt(ui)->GetDataLeaf()->GetSurfaceCount() == 0)
            {
                GetBlockChildAt(ui)->MarkSurfaceMasksChanged(bCalcUV);
            }
        }
    }

    // Request an update
    if (!RequiresUpdate())
        RequestUpdate();
}
//---------------------------------------------------------------------------
void NiTerrainBlock::RequestUpdate()
{
    m_bRequiresUpdate = true;
    if (!m_pkParent || m_pkParent->RequiresUpdate()) {
        return;
    } else {
        m_pkParent->RequestUpdate();
    }
}
//---------------------------------------------------------------------------
void NiTerrainBlock::Update(float fTime) 
{
    // Update the masks
    if (m_bMaskChanged || m_bMaskLayersChanged)
    {
        UpdateMasks();
        m_bMaskChanged = false;
        m_bMaskLayersChanged = false;
    }
    
    if (m_ucStreamsLockedForWrite)
    {
        // Update the normals/tangents/morph data of the modified vertices.
        if (m_aucLockStatus[STREAM_POSITION] & NiDataStream::LOCK_WRITE)
        {
            UpdatePositionMorphData();

            // Unlock those streams:
            UnlockDynamicStream(STREAM_POSITION, NiDataStream::LOCK_WRITE);

            // We request an update for the decals so they follow the new 
            // height variation
            RequestDecalsUpdate();
        }

        // Unlock any remaining streams
        for (NiUInt8 uc = STREAM_NORMALTANGENT; uc < STREAM_MAX; ++uc)
        {
            if (m_ucStreamsLockedForWrite == 0)
                break;

            UnlockDynamicStream(STREAMS(uc), NiDataStream::LOCK_WRITE);
        }

        NIASSERT(m_ucStreamsLockedForWrite == 0);
    }

    // Check if our children require an update 
    NiTerrainBlock* pkChild = GetBlockChildAt(0);
    if (pkChild) 
    {
        if (pkChild->RequiresUpdate())
            pkChild->Update(fTime);

        pkChild = GetBlockChildAt(1);
        if (pkChild->RequiresUpdate())
            pkChild->Update(fTime);

        pkChild = GetBlockChildAt(2);
        if (pkChild->RequiresUpdate())
            pkChild->Update(fTime);

        pkChild = GetBlockChildAt(3);
        if (pkChild->RequiresUpdate())
            pkChild->Update(fTime);
    }

    m_bRequiresUpdate = false;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::UpdatePositionMorphData()
{
    NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();
    NiUInt32 uiHalfBlockSize = uiBlockSize >> 1;
    NiUInt32 uiRowSize = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiOffset;

    NiUInt32 uiNumModifiedVerts;
    m_kRecentlyModifiedVerts.GetSize(uiNumModifiedVerts);
    NiTerrainPositionRandomAccessIterator kPositions;
    NiTerrainPositionRandomAccessIterator kParentPositions;
    m_pkDataLeaf->GetPositionIterator(kPositions);

    for (NiUInt32 ui = 0; ui < uiNumModifiedVerts; ++ui)
    {
        NiUInt32 uiBufferIndex;
        m_kRecentlyModifiedVerts.GetValue(ui, uiBufferIndex);
        float fAveragePosition = 0.0f;

        NiIndex kLocal;
        kLocal.x = (NiUInt16)(uiBufferIndex % uiRowSize);
        kLocal.y = (NiUInt16)(uiBufferIndex / uiRowSize);
        
        // Update the vertex itself
        NiPoint3 kPosition;
        kPositions.GetHighDetail(uiBufferIndex, kPosition);
        NiTerrainBlock *pkParent = GetBlockParent();
        if (pkParent)
        {
            pkParent->m_pkDataLeaf->GetPositionIterator(kParentPositions);

            NiIndex kOurBottomLeft;
            NiIndex kParentBottomLeft;
            m_pkDataLeaf->GetBottomLeftIndex(kOurBottomLeft);
            pkParent->m_pkDataLeaf->GetBottomLeftIndex(kParentBottomLeft);

            NiIndex kParentIndexOffset(0,0);
            if (kOurBottomLeft.x > kParentBottomLeft.x)
                kParentIndexOffset.x = (NiUInt32)uiHalfBlockSize;
            if (kOurBottomLeft.y > kParentBottomLeft.y)
                kParentIndexOffset.y = (NiUInt32)uiHalfBlockSize;

            bool xOdd = kLocal.x % 2 != 0;
            bool yOdd = kLocal.y % 2 != 0;

            // On a diagonal?
            if (xOdd && yOdd)
            {
                // Which face is it?
                if ((kLocal.x % 4) == (kLocal.y % 4))
                {
                    // bottom left
                    NiPoint3 kParentPos;
                    uiOffset = ((kLocal.x-1) >> 1) + kParentIndexOffset.x +
                        (((kLocal.y-1) >> 1) + kParentIndexOffset.y) * 
                        uiRowSize;   
                    
                    kParentPositions.GetHighDetail(uiOffset, kParentPos);
                    fAveragePosition += kParentPos.z;

                    // top right
                    uiOffset = ((kLocal.x+1) >> 1) + kParentIndexOffset.x +
                        (((kLocal.y+1) >> 1) + kParentIndexOffset.y) * 
                        uiRowSize;
                    kParentPositions.GetHighDetail(uiOffset, kParentPos);
                    fAveragePosition += kParentPos.z;
                }
                else
                {
                    // bottom right
                    NiPoint3 kParentPos;
                    uiOffset = ((kLocal.x-1) >> 1) + kParentIndexOffset.x +
                        (((kLocal.y+1) >> 1) + kParentIndexOffset.y) * 
                        uiRowSize;                    
                    kParentPositions.GetHighDetail(uiOffset, kParentPos);
                    fAveragePosition += kParentPos.z;

                    // top left
                    uiOffset = ((kLocal.x+1) >> 1) + kParentIndexOffset.x +
                        (((kLocal.y-1) >> 1) + kParentIndexOffset.y) * 
                        uiRowSize;
                    kParentPositions.GetHighDetail(uiOffset, kParentPos);
                    fAveragePosition += kParentPos.z;
                }
            }
            // Horizontal?
            else if (xOdd)
            {
                // Left
                NiPoint3 kParentPos;
                uiOffset = ((kLocal.x-1) >> 1) + kParentIndexOffset.x +
                    (((kLocal.y) >> 1) + kParentIndexOffset.y) * uiRowSize;
                kParentPositions.GetHighDetail(uiOffset, kParentPos);
                fAveragePosition += kParentPos.z;

                // Right
                uiOffset = ((kLocal.x+1) >> 1) + kParentIndexOffset.x +
                    (((kLocal.y) >> 1) + kParentIndexOffset.y) * uiRowSize;
                kParentPositions.GetHighDetail(uiOffset, kParentPos);
                fAveragePosition += kParentPos.z;
            }
            // Vertical
            else if (yOdd)
            {
                // Top
                NiPoint3 kParentPos;
                uiOffset = ((kLocal.x) >> 1) + kParentIndexOffset.x +
                    (((kLocal.y-1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                kParentPositions.GetHighDetail(uiOffset, kParentPos);
                fAveragePosition += kParentPos.z;

                // Bottom
                uiOffset = ((kLocal.x) >> 1) + kParentIndexOffset.x +
                    (((kLocal.y+1) >> 1) + kParentIndexOffset.y) * uiRowSize;
                kParentPositions.GetHighDetail(uiOffset, kParentPos);
                fAveragePosition += kParentPos.z;
            }
            else
            {
                NiPoint3 kParentPos;
                uiOffset = ((kLocal.x) >> 1) + kParentIndexOffset.x +
                    (((kLocal.y) >> 1) + kParentIndexOffset.y) * uiRowSize;

                kParentPositions.GetHighDetail(uiOffset, kParentPos);
                fAveragePosition += 2.0f * kParentPos.z;
            }
        }
        else
        {
            fAveragePosition = 2.0f * kPosition.z;
        }

        fAveragePosition *= 0.5f;
        kPosition.z = fAveragePosition;
        kPositions.SetLowDetail(uiBufferIndex, kPosition);
    }

    m_kRecentlyModifiedVerts.Reset();
}
//---------------------------------------------------------------------------
bool NiTerrainBlock::ProcessLOD() 
{
    NIASSERT(!m_ucStreamsLockedForWrite && 
        "Terrain must be updated after deformation has occured");

    if (m_pkSectorData->GetHighestLoadedLOD() < 
        (NiInt32)m_pkDataLeaf->GetLevel())
    {
        return false;
    }
    
    // Are we within the frustum? if not, do not even check our children.
    // Test for bounding sphere intersection with the view frustum.

    // Calculate our distances to each of the planes
    const NiBound* pkBound = &m_pkDataLeaf->GetLocalBound();
    NiInt32 iWhichSide = 0;
    NiPlane kPlane;
    for (NiInt32 i = 0; i < NiFrustumPlanes::MAX_PLANES; ++i)
    {
        // find the distance to this plane - Note that the camera has already
        // been transformed into terrain coordinate space
        kPlane = m_pkSectorData->GetFrustumPlanes().GetPlane(i);
        iWhichSide = kPlane.WhichSide((*pkBound).GetCenter() + 
            (kPlane.GetNormal() * (*pkBound).GetRadius()));

        if (iWhichSide == NiPlane::NEGATIVE_SIDE) 
            return false;
    }

    NiTerrainBlock* pkChild = GetBlockChildAt(0);
    if (pkChild) 
    {
        // Process children - using a binary mask so we know which 
        // children were successful
        NiUInt32 ucVis = 0;

        // These are used in the binary mask
        const NiUInt32 cusChild1 = 0x1;
        const NiUInt32 cusChild2 = 0x2;
        const NiUInt32 cusChild3 = 0x4;
        const NiUInt32 cusChild4 = 0x8;
        
        if (pkChild->IsInRange() && pkChild->ProcessLOD())
            ucVis |= cusChild1;

        pkChild = GetBlockChildAt(1);
        if (pkChild->IsInRange() && pkChild->ProcessLOD())
            ucVis |= cusChild2;

        pkChild = GetBlockChildAt(2);
        if (pkChild->IsInRange() && pkChild->ProcessLOD())
            ucVis |= cusChild3;

        pkChild = GetBlockChildAt(3);
        if (pkChild->IsInRange() && pkChild->ProcessLOD())
            ucVis |= cusChild4;
        
        // Any children visible?
        if (ucVis) 
        {
            // Did any blocks figure themselves unimportant?
            // Force them to display.
            if (ucVis != 0xf) // binary 1111
            {
                if ((ucVis & cusChild1) == 0)
                    GetBlockChildAt(0)->AddToVisible(); 

                if ((ucVis & cusChild2) == 0)
                    GetBlockChildAt(1)->AddToVisible(); 

                if ((ucVis & cusChild3) == 0)
                    GetBlockChildAt(2)->AddToVisible(); 

                if ((ucVis & cusChild4) == 0)
                    GetBlockChildAt(3)->AddToVisible(); 
            }

            // Stitching flag - this node has children
            m_pkDrawnPool->CombineValue(m_uiDrawnPoolIndex, 
                NiTerrainBlock::CHILDREN);

            // Don't want to add our mesh, since we drew some children
            return true;
        }
    }

    // We want to add our mesh, since we are within range but did not draw 
    // any children
    AddToVisible();
    return true;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::ProcessBorders() 
{
    NiUInt8 ucMask = 0;
    const NiTerrainDataLeaf* pkLeaf;

    // We only need to process our borders if we were added to the visible set
    if (!(m_pkDrawnPool->GetValue(m_uiDrawnPoolIndex) &
        NiTerrainBlock::CHILDREN))
    {
        // For each side:
        NiUInt8 ucBorder;
        for (NiUInt8 uc = 0; uc < 4; ++uc)
        {
            ucBorder = 1 << uc;

            // Look at the sibling block on this border. If that sibling was 
            // not drawn and none of its children were drawn, we need to
            // stitch
            pkLeaf = GetDataLeaf()->GetAdjacent(ucBorder);

            if (!pkLeaf) 
                continue;

            // Check all in one hit since it is a bit mask - children drawn or 
            // self drawn.
            // If the block is lower detail than us, we need to stitch
            if (pkLeaf->GetQuadMesh()->CulledByLOD())
                ucMask |= ucBorder;
        }

        SetStitchingIndex(ucMask);
    }
    else
    {
        NiTerrainBlock* pkChild = GetBlockChildAt(0);

        // We either have 0 or 4 children
        if (pkChild) 
        {     
            if (!pkChild->CulledByLOD())
                pkChild->ProcessBorders();

            pkChild = GetBlockChildAt(1);
            if (!pkChild->CulledByLOD())
                pkChild->ProcessBorders();

            pkChild = GetBlockChildAt(2);
            if (!pkChild->CulledByLOD())
                pkChild->ProcessBorders();

            pkChild = GetBlockChildAt(3);
            if (!pkChild->CulledByLOD())
                pkChild->ProcessBorders();
        }
    }

    return;
}
//---------------------------------------------------------------------------
void NiTerrainBlock::SetStitchingIndex(NiUInt8 ucStitchingIndex)
{
    m_ucStitchingIndex = ucStitchingIndex;
    NiDataStreamRef* pkIndexStream = m_spMesh->GetStreamRefAt(2);
    
    NIASSERT(pkIndexStream->GetSemanticNameAt(0).Equals(
        NiCommonSemantics::INDEX()));

    pkIndexStream->BindRegionToSubmesh(
        0, *m_pkSectorData->GetIndexRegionByStitchIndex(ucStitchingIndex));

    // Update the stitching information for the shader:
    if (!(m_pkSectorData->GetLODMode() & 
        NiTerrainSectorData::LOD_MORPH_ENABLE))
    {
        NiFloatsExtraData* pkStitchingInfo = 
            (NiFloatsExtraData*)m_spMesh->GetExtraData(
            NiTerrainMaterial::STITCHINGINFO_SHADER_CONSTANT);
        NIASSERT(pkStitchingInfo);

        NiIndex kBottomLeftIndex;
        m_pkDataLeaf->GetBottomLeftIndex(kBottomLeftIndex);

        NiPoint2 kMinXY((float)kBottomLeftIndex.x, (float)kBottomLeftIndex.y);
        kMinXY.x -= m_pkSectorData->GetSectorSize() >> 1;
        kMinXY.y -= m_pkSectorData->GetSectorSize() >> 1;
        NiPoint2 kMaxXY = kMinXY;
        NiUInt32 uiSubDivisions = m_pkDataLeaf->GetNumSubDivisions();
        kMaxXY.x += m_pkDataLeaf->GetBlockSize() << uiSubDivisions;           
        kMaxXY.y += m_pkDataLeaf->GetBlockSize() << uiSubDivisions;

        // Shift according to the borders sets
        if (ucStitchingIndex & NiTerrainDataLeaf::BORDER_LEFT)
            kMinXY.x ++;
        else if (ucStitchingIndex & NiTerrainDataLeaf::BORDER_RIGHT)
            kMaxXY.x --;
        if (ucStitchingIndex & NiTerrainDataLeaf::BORDER_BOTTOM)
            kMinXY.y ++;
        else if (ucStitchingIndex & NiTerrainDataLeaf::BORDER_TOP)
            kMaxXY.y --;

        NIASSERT(kMinXY.x < kMaxXY.x && kMinXY.y < kMaxXY.y);

        // Update stitch data
        pkStitchingInfo->SetValue(0, kMinXY.x);
        pkStitchingInfo->SetValue(1, kMinXY.y);
        pkStitchingInfo->SetValue(2, kMaxXY.x);
        pkStitchingInfo->SetValue(3, kMaxXY.y);
    }
}
//---------------------------------------------------------------------------
void NiTerrainBlock::AddDecal(NiTerrainDecal* pkDecal)
{
    NIASSERT(pkDecal);
    if (!m_kDecals.FindPos(pkDecal))
        m_kDecals.AddTail(pkDecal);
}   
//---------------------------------------------------------------------------
void NiTerrainBlock::RequestDecalsUpdate()
{
    NiTListIterator kIterator = m_kDecals.GetHeadPos();
    while (kIterator)
    {
        NiTerrainDecal* pkDecal = m_kDecals.GetNext(kIterator);

        if (pkDecal->GetRefCount() == 0)
        {
            m_kDecals.Remove(pkDecal);
        }
        else
        {
            pkDecal->SetRequiresUpdate();
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainBlock::MarkVertexChanged(const NiIndex& kLocalIndex,
    float fNewHeight)
{
     
    NIASSERT(m_pkCustomPositionStream);

    // Check that the stream is locked for write
    if (!(m_aucLockStatus[STREAM_POSITION] & NiDataStream::LOCK_WRITE))
    {
        if (!LockDynamicStream(STREAM_POSITION, NiDataStream::LOCK_WRITE))
        {
            return;
        }

        RequestUpdate();
    }
    
    // Update the vertex itself
    NiUInt32 uiBufferIndex = kLocalIndex.x + kLocalIndex.y * 
        (m_pkSectorData->GetBlockWidthInVerts());
    m_pkDataLeaf->m_kPositionIterator.SetComponent(uiBufferIndex, 
        NiTerrainPositionRandomAccessIterator::Z, fNewHeight);

    // NOTE: Normals, Tangents and their Morphing data are updated in "Update"
    // Add this index to the list we want to update:
    NiUInt32 uiNextIndex = m_kRecentlyModifiedVerts.GetNew();
    m_kRecentlyModifiedVerts.SetValue(uiNextIndex, uiBufferIndex);
    RequestLightingRebuild();

    // Check if this vertex changing will affect the normals of an adjacent
    // block, and if so, mark it for a lighting update too.
    NiUInt16 uiMinBorder = 1;
    NiUInt32 uiMaxBorder = m_pkSectorData->GetBlockSize() - 1;

    const NiTerrainDataLeaf* pkAdjacentLeaf = 0;
    if (kLocalIndex.x == uiMinBorder)
    {
        pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
            NiTerrainDataLeaf::BORDER_LEFT);
    }
    else if (kLocalIndex.x == uiMaxBorder)
    {
        pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
            NiTerrainDataLeaf::BORDER_RIGHT);
    }
    if (pkAdjacentLeaf)
    {
        pkAdjacentLeaf->m_pkQuadMesh->RequestLightingRebuild();
        pkAdjacentLeaf->m_pkContainingSector->SetLightingOutdated(true);
        pkAdjacentLeaf = 0;
    }

    if (kLocalIndex.y == uiMinBorder)
    {
        pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
            NiTerrainDataLeaf::BORDER_BOTTOM);
    }
    else if (kLocalIndex.y == uiMaxBorder)
    {
        pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
            NiTerrainDataLeaf::BORDER_TOP);
    }
    if (pkAdjacentLeaf)
    {
        pkAdjacentLeaf->m_pkQuadMesh->RequestLightingRebuild();
        pkAdjacentLeaf->m_pkContainingSector->SetLightingOutdated(true);
        pkAdjacentLeaf = 0;
    }
    
    // Corner cases
    if (kLocalIndex.x == uiMinBorder)
    {
        if (kLocalIndex.y == uiMinBorder)
        {
            pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
                NiTerrainDataLeaf::BORDER_LEFT |
                NiTerrainDataLeaf::BORDER_BOTTOM);
        }
        else if(kLocalIndex.y == uiMaxBorder)
        {
            pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
                NiTerrainDataLeaf::BORDER_LEFT |
                NiTerrainDataLeaf::BORDER_TOP);
        }
    }
    else if (kLocalIndex.x == uiMaxBorder)
    {
        if (kLocalIndex.y == uiMinBorder)
        {
            pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
                NiTerrainDataLeaf::BORDER_RIGHT |
                NiTerrainDataLeaf::BORDER_BOTTOM);
        }
        else if(kLocalIndex.y == uiMaxBorder)
        {
            pkAdjacentLeaf = m_pkDataLeaf->GetAdjacent(
                NiTerrainDataLeaf::BORDER_RIGHT |
                NiTerrainDataLeaf::BORDER_TOP);
        }
    }
    if (pkAdjacentLeaf)
    {
        pkAdjacentLeaf->m_pkQuadMesh->RequestLightingRebuild();
        pkAdjacentLeaf->m_pkContainingSector->SetLightingOutdated(true);
        pkAdjacentLeaf = 0;
    }    
}
