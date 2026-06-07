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

#include "NiMeshTools.h"
#include "NiMeshVertexOperators.h"
#include "NiToolDataStream.h"
#include "NiTStridedRandomAccessIterator.h"
#include "NiDataStreamElementLock.h"
#include "NiDataStreamLock.h"
#include "NiMeshAlgorithms.h"
#include "NiToolPipelineCloneHelper.h"
#include "NiSkinningUtilities.h"

#include <NiSkinningMeshModifier.h>
#include <NiTNodeTraversal.h>
#include <NiMesh.h>
#include <NiGeometricUtils.h>


//-------------------------------------------------------------------------
NiMeshTools::MeshElementHelper::MeshElementHelper(
    const NiDataStreamElement& kElement)
{
    m_uiCount = kElement.GetComponentCount();
    m_uiOffsetInBytes = kElement.GetOffset();

    m_pFunc = NiMeshVertexOperators::GetInstance()->GetEqualsOp(
        kElement.GetType());
}    
//-------------------------------------------------------------------------
bool NiMeshTools::MeshElementHelper::Compare(void* pVertex1, 
                                                 void* pVertex2)
{
    return m_pFunc((char*)pVertex1 + m_uiOffsetInBytes, 
        (char*)pVertex2 + m_uiOffsetInBytes,
        m_uiCount);
}
//-------------------------------------------------------------------------
NiMeshTools::MeshHelper::MeshHelper(NiDataStream* pkStream, 
                                        char ucLockMask)
{
    Init(pkStream, ucLockMask);
}
//-------------------------------------------------------------------------
NiMeshTools::MeshHelper::MeshHelper(MeshHelper* pkFrom)
{
    NiUInt8 ucLockMask = NiDataStream::LOCK_TOOL_READ | 
        NiDataStream::LOCK_TOOL_WRITE;
    
    m_spDS = NiSmartPointerCast(NiDataStream,
        pkFrom->m_spDS->CreateDeepCopy());

    Init(m_spDS, ucLockMask);
}
//-------------------------------------------------------------------------
NiMeshTools::MeshHelper::~MeshHelper()
{
    m_spDS->Unlock(m_ucLockMask);
}
//-------------------------------------------------------------------------
void NiMeshTools::MeshHelper::Init(NiDataStream* pkStream, char ucLockMask)
{
    NIASSERT(pkStream);
    m_ucLockMask = ucLockMask;
    m_spDS = pkStream;
    m_uiStride = pkStream->GetStride();
    m_pDSStart = pkStream->Lock(ucLockMask);
    m_pRegionStart = NULL;

    for (NiUInt32 ui = 0; ui < pkStream->GetElementDescCount(); ui++)
    {
        const NiDataStreamElement& kElement = pkStream->GetElementDescAt(ui);
        m_kElementFuncs.Add(MeshElementHelper(kElement));
    }
}
//-------------------------------------------------------------------------
bool NiMeshTools::MeshHelper::IsValid()
{
    return m_pDSStart != NULL;
}
//-------------------------------------------------------------------------
float* NiMeshTools::MeshHelper::GetVertex(NiUInt32 uiVertexIdx)
{
    return (float*)((char*)m_pRegionStart + uiVertexIdx * m_uiStride);
}
//-------------------------------------------------------------------------
bool NiMeshTools::MeshHelper::CompareVertex(NiUInt32 uiA, NiUInt32 uiB)
{
    void* pDataA = (char*)m_pRegionStart + uiA * m_uiStride;
    void* pDataB = (char*)m_pRegionStart + uiB * m_uiStride;

    for (NiUInt32 ui = 0; ui < m_kElementFuncs.GetSize(); ui++)
    {
        MeshElementHelper& kRef = m_kElementFuncs.GetAt(ui);
        if (!kRef.Compare(pDataA, pDataB))
        {
            return false;
        }
    }

    return true;
}
//-------------------------------------------------------------------------
void NiMeshTools::MeshHelper::RemoveVertex(NiUInt32 uiVertexIdx)
{
    NiDataStream::Region& kRegion = m_spDS->GetRegion(m_uiActiveRegion);
    NiUInt32 uiRegionCount = kRegion.GetRange();
    char* pcStart = (char*)m_pRegionStart + uiVertexIdx * m_uiStride;
    
    // Copy all the data from the starting data offset and decrease region
    // size.
    for (NiUInt32 ui = uiVertexIdx + 1; ui < uiRegionCount; ui++)
    {
        char* pcData = (char*)m_pRegionStart + ui * m_uiStride;
        NiMemcpy(pcStart, pcData, m_uiStride);
        pcStart += m_uiStride;
    }               
}
//-------------------------------------------------------------------------
void NiMeshTools::MeshHelper::SetVertex(NiUInt32 uiVertexIdx, 
                                            float* pfVertex)
{
    char* pcData = (char*)m_pRegionStart + uiVertexIdx * m_uiStride;
    NiMemcpy(pcData, (char*)pfVertex, m_uiStride);
}
//-------------------------------------------------------------------------
void NiMeshTools::MeshHelper::Reset(NiUInt32 uiRegion)
{
    NiDataStream::Region& kRegion = m_spDS->GetRegion(uiRegion);
    m_pRegionStart = kRegion.ComputeStart(m_pDSStart, m_uiStride);
    m_uiActiveRegion = uiRegion;
}
//-------------------------------------------------------------------------
NiUInt32 NiMeshTools::MeshHelper::GetRange(NiUInt32 uiRegion)
{
    return m_spDS->GetRegion(uiRegion).GetRange();
}
//-------------------------------------------------------------------------
bool NiMeshTools::SharesDataStreams(NiMesh* pkMeshA, 
    NiMesh* pkMeshB)
{
    NIASSERT(pkMeshA);
    NIASSERT(pkMeshB);

    NiUInt32 uiStreamRefCountA = pkMeshA->GetStreamRefCount();
    NiUInt32 uiStreamRefCountB = pkMeshB->GetStreamRefCount();

    for (NiUInt32 ui = 0; ui < uiStreamRefCountA; ui++)
    {
        const NiDataStreamRef* pkStreamRefA = 
            pkMeshA->GetStreamRefAt(ui);
        if (!pkStreamRefA)
            continue;

        for (NiUInt32 uj = 0; uj < uiStreamRefCountB; uj++)
        {
            const NiDataStreamRef* pkStreamRefB = 
                pkMeshB->GetStreamRefAt(uj);

            if (!pkStreamRefB)
                continue;

            if (pkStreamRefA->GetDataStream() == pkStreamRefB->GetDataStream())
                return true;
        }
    }

    return false;
}
//-------------------------------------------------------------------------
void NiMeshTools::WeldVertices(NiMesh* pkMesh)
{
    NiUInt8 ucLockMask = NiDataStream::LOCK_TOOL_READ | 
        NiDataStream::LOCK_TOOL_WRITE;
    NIASSERT(pkMesh);   

    // Build a list of data streams that we need to lock.
    NiTPrimitiveSet<MeshHelper*> kVertexDataStreams;
    NiDataStreamPtr spIndexBuffer;

    NiUInt32 uiStreamRefCount = pkMesh->GetStreamRefCount();
    for (NiUInt32 ui = 0; ui < uiStreamRefCount; ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);

        if (pkStreamRef->GetDataStream()->GetUsage() == 
            NiDataStream::USAGE_VERTEX &&
            !pkStreamRef->IsPerInstance())
        {
            kVertexDataStreams.Add(NiNew MeshHelper(
                pkStreamRef->GetDataStream(), ucLockMask));            
        }
        else if (pkStreamRef->GetDataStream()->GetUsage() ==
            NiDataStream::USAGE_VERTEX_INDEX)
        {
            spIndexBuffer = pkStreamRef->GetDataStream();
        }
    }

    // No index buffer, no remapping
    if (spIndexBuffer == NULL)
    {
        return;
    }

    // foreach submesh, compare each vertex against each other vertex to see 
    // if they are the same. If all vertex elements of a vertex match all 
    // vertex elements of another vertex, then they are the same and the remap 
    // buffer will point to the lowest index for both vertices. If a vertex 
    // does not have a twin, it's index will be the same.

    for (NiUInt32 uiSubmesh = 0; uiSubmesh < pkMesh->GetSubmeshCount(); 
        uiSubmesh++)
    {
        NiUInt32* puiIndices = (NiUInt32*)
            spIndexBuffer->LockRegion(uiSubmesh, ucLockMask);
        NiUInt32 uiIndexCount = spIndexBuffer->GetRegion(uiSubmesh).GetRange();

        NiUInt32 uiVertexCount = pkMesh->GetVertexCount(uiSubmesh);
        NiUInt32* puiRemap = NiAlloc(NiUInt32, uiVertexCount);
        for (NiUInt32 uiDS = 0; uiDS < kVertexDataStreams.GetSize(); uiDS++)
        {
            MeshHelper& kHelper = *kVertexDataStreams.GetAt(uiDS);
            kHelper.Reset(uiSubmesh);
        }

        // Set the remap buffer to not remap any index
        for (NiUInt32 uiVertex = 0; uiVertex < uiVertexCount; uiVertex++)
        {
            puiRemap[uiVertex] = uiVertex;
        }

        // foreach vertex, remap any subsequent vertex that matches me
        for (NiUInt32 uiVertexA = 0; uiVertexA < uiVertexCount; uiVertexA++)
        {
            // If this value is different than uiVertexA, we've already 
            // remapped it
            if (puiRemap[uiVertexA] != uiVertexA)
                continue;

            for (NiUInt32 uiVertexB = uiVertexA + 1; 
                uiVertexB < uiVertexCount; uiVertexB++)
            {
                bool bSame = true;
                for (NiUInt32 uiDS = 0; bSame && uiDS < 
                    kVertexDataStreams.GetSize(); uiDS++)
                {
                    MeshHelper& kHelper = *kVertexDataStreams.GetAt(uiDS);
                    bSame &= kHelper.CompareVertex(uiVertexA, uiVertexB);
                }

                if (bSame)
                    puiRemap[uiVertexB] = uiVertexA;
            }
        }

        // foreach submesh, foreach index in the indexbuffer, lookup the 
        // vertex index in the remap buffer. If the value differs, reset the 
        // value in the indexbuffer to the remapped value.
        for (NiUInt32 uiIndex = 0; uiIndex < uiIndexCount; uiIndex++)
        {
            NiUInt32 uiVertexIdx = puiIndices[uiIndex];
            if (puiRemap[uiVertexIdx] != uiVertexIdx)
                puiIndices[uiIndex] = puiRemap[uiVertexIdx];
        }

        NiFree(puiRemap);
        spIndexBuffer->Unlock(ucLockMask);       
    }    

    for (NiUInt32 ui = 0; ui < kVertexDataStreams.GetSize(); ui++)   
        NiDelete kVertexDataStreams.GetAt(ui);
}
//-------------------------------------------------------------------------
void NiMeshTools::RemoveUnusedVertices(NiMesh* pkMesh)
{
    // foreach index in remap buffer, if an index was remapped, remove that 
    // vertex and slide all subsequent vertices to the left. Adjust the 
    // remap buffer to point to the correct vertex if the remap index was 
    // higher than the vertex index that was removed.

    NiUInt8 ucLockMask = NiDataStream::LOCK_TOOL_READ | 
        NiDataStream::LOCK_TOOL_WRITE;
    NIASSERT(pkMesh);   

    // Build a list of data streams that we need to lock.
    NiTPrimitiveSet<MeshHelper*> kVertexDataStreams;
    NiDataStreamPtr spIndexBuffer;

    NiUInt32 uiStreamRefCount = pkMesh->GetStreamRefCount();
    for (NiUInt32 ui = 0; ui < uiStreamRefCount; ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);

        if (pkStreamRef->GetDataStream()->GetUsage() == 
            NiDataStream::USAGE_VERTEX &&
            !pkStreamRef->IsPerInstance())
        {
            kVertexDataStreams.Add(NiNew MeshHelper(
                pkStreamRef->GetDataStream(), ucLockMask));            
        }
        else if (pkStreamRef->GetDataStream()->GetUsage() ==
            NiDataStream::USAGE_VERTEX_INDEX)
        {
            spIndexBuffer = pkStreamRef->GetDataStream();
        }
    }

    // No index buffer, nothing to be done.
    if (spIndexBuffer == NULL)
    {
        return;
    }


    // foreach submesh, build a flag list indicating what vertices are used
    // by the index buffer.
    for (NiUInt32 uiSubmesh = 0; uiSubmesh < pkMesh->GetSubmeshCount(); 
        uiSubmesh++)
    {
        NiUInt32* puiIndices = 
            (NiUInt32*)spIndexBuffer->LockRegion(uiSubmesh, ucLockMask);
        NiUInt32 uiIndexCount = spIndexBuffer->GetRegion(uiSubmesh).GetRange();
        NiUInt32 uiVertexCount = pkMesh->GetVertexCount(uiSubmesh);

        for (NiUInt32 uiDS = 0; uiDS < kVertexDataStreams.GetSize(); uiDS++)
        {
            MeshHelper& kHelper = *kVertexDataStreams.GetAt(uiDS);
            kHelper.Reset(uiSubmesh);
        }

        bool* pbVisited = NiAlloc(bool, uiVertexCount);
        memset(pbVisited, 0, sizeof(bool) * uiVertexCount);
        
        // Loop through the index buffer and mark the visited array.
        for (NiUInt32 ui = 0; ui < uiIndexCount; ui++)
        {
            pbVisited[puiIndices[ui]] = true;
        }

        // Now based on the visited flags we can remove an entry from each
        // of the vertex streams if the vertex was not visited then re-number
        // the indices that are greater than the removed vertex index.        
        NiUInt32 uiRemovedCount = 0;
        for (NiUInt32 uiVertex = 0; uiVertex < uiVertexCount; uiVertex++)
        {
            if (!pbVisited[uiVertex])
            {
                for (NiUInt32 uiDS = 0; uiDS < kVertexDataStreams.GetSize(); 
                    uiDS++)
                {
                    MeshHelper& kHelper = *kVertexDataStreams.GetAt(uiDS);
                    kHelper.RemoveVertex(uiVertex);                    
                }

                // Remap all indices that greater than the vertex index.
                for (NiUInt32 ui = 0; ui < uiIndexCount; ui++)
                {
                    if (puiIndices[ui] > uiVertex)
                        puiIndices[ui]--;
                }

                uiRemovedCount++;
            }
        }

        // Now we need to compact the vertex streams since we removed vertices.
        for (NiUInt32 uiDS = 0; uiDS < kVertexDataStreams.GetSize(); uiDS++)
        {
            MeshHelper& kHelper = *kVertexDataStreams.GetAt(uiDS);
            NiDataStream::Region& kRegion = 
                kHelper.m_spDS->GetRegion(uiSubmesh);
            kRegion.SetRange(kRegion.GetRange() - uiRemovedCount);
        }       
        
        spIndexBuffer->Unlock(ucLockMask);        
        NiFree(pbVisited);
    }

    for (NiUInt32 ui = 0; ui < kVertexDataStreams.GetSize(); ui++)   
        NiDelete kVertexDataStreams.GetAt(ui);
}
//---------------------------------------------------------------------------
void NiMeshTools::WeightSort::Sort(NiMeshTools::WeightSort* pkData, 
    const NiUInt32 uiSize)
{
    qsort(pkData, uiSize, sizeof(WeightSort), CompareWeights);
}
//---------------------------------------------------------------------------
int NiMeshTools::WeightSort::CompareWeights(
    const void* pv0, const void* pv1)
{
    // compare function used by qsort
    float fWeight0, fWeight1;

    fWeight0 = ((WeightSort*) pv0)->m_fWeight;
    fWeight1 = ((WeightSort*) pv1)->m_fWeight;

    // Compare such that larger weights will be first
    if (fWeight0 > fWeight1)
        return -1;
    if (fWeight0 < fWeight1)
        return 1;
    return 0;
}
//-------------------------------------------------------------------------
void NiMeshTools::ViewIndependentOptimization(
    NiMesh* pkMesh, const float fSizeModifier, 
    const float fNormalModifier, const float fPositionModifier)
{
    // Quick out we only support triangle lists
    if (!pkMesh || 
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES)
    {
        return;
    }

    // Quick out to see if we need to do something
    if (fSizeModifier == 0.0f &&
        fNormalModifier == 0.0f &&
        fPositionModifier == 0.0f)
    {
        return;
    }

    // Lock the position stream
    NiDataStreamElementLock kPosLock(pkMesh, NiCommonSemantics::POSITION(), 0,
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);
    if (!kPosLock.IsLocked())
        return;

    // Lock the index stream
    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT32_1, 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    if (!kIndexLock.IsLocked())
        return;

    // foreach submesh, build a flag list indicating what vertices are used
    // by the index buffer.
    for (NiUInt32 uiSubmesh = 0; uiSubmesh < pkMesh->GetSubmeshCount(); 
        uiSubmesh++)
    {
        NiTStridedRandomAccessIterator<NiPoint3> kPosIter = 
            kPosLock.begin<NiPoint3>(uiSubmesh);

        NiTStridedRandomAccessIterator<NiUInt32> kIndexIter = 
            kIndexLock.begin<NiUInt32>(uiSubmesh);

        NiUInt32 uiIndexCount = kIndexLock.count(uiSubmesh);
        NiUInt32 uiVertexCount = kPosLock.count(uiSubmesh);
        NiUInt32 uiPrimitiveCount = (uiIndexCount) ? uiIndexCount / 3 : 
            uiVertexCount / 3;
        float* pfSizeWeights = NiAlloc(float, uiPrimitiveCount);
        memset(pfSizeWeights, 0, sizeof(float) * uiPrimitiveCount);
        float* pfNormalWeights = NiAlloc(float, uiPrimitiveCount);
        memset(pfNormalWeights, 0, sizeof(float) * uiPrimitiveCount);
        float* pfPositionWeights = NiAlloc(float, uiPrimitiveCount);
        memset(pfPositionWeights, 0, sizeof(float) * uiPrimitiveCount);

        // First step is to calculate the weight value for the size of the
        // triangle.

        // Store the center position
        const NiPoint3& kMeshCenter = pkMesh->GetModelBound().GetCenter();

        // Used to normalize the size weight
        float fMinArea = FLT_MAX;
        float fMaxArea = -FLT_MAX;
        float fMinDot = FLT_MAX;
        float fMaxDot = -FLT_MAX;
        float fMinDist = FLT_MAX;
        float fMaxDist = -FLT_MAX;

        // Loop through the index buffer and store the area
        NiUInt32 uiCount = 0;
        for (NiUInt32 ui = 0; ui < uiIndexCount; ui+=3)
        {
            NiUInt32 uiIndex0 = kIndexIter[ui];
            NiUInt32 uiIndex1 = kIndexIter[ui + 1];
            NiUInt32 uiIndex2 = kIndexIter[ui + 2];

            // Get the positions of this triangle
            NiPoint3 kPosition0(kPosIter[uiIndex0]);
            NiPoint3 kPosition1(kPosIter[uiIndex1]);
            NiPoint3 kPosition2(kPosIter[uiIndex2]);

            // Get the area of the triangle
            float fArea = 0.0f;
            if (fSizeModifier != 0.0f)
            {
                fArea = NiGeometricUtils::GetTriArea(kPosition0, 
                    kPosition1, kPosition2);
                pfSizeWeights[uiCount] = fArea;
                fMinArea = (fArea < fMinArea) ? fArea : fMinArea;
                fMaxArea = (fArea > fMaxArea) ? fArea : fMaxArea;
            }

            // Make sure we need to do the following
            if (fNormalModifier != 0.0f ||
                fPositionModifier != 0.0f)
            {
                // Get the center used by both normal and position
                NiPoint3 kCenter = NiGeometricUtils::GetTriCenter(
                    kPosition0, kPosition1, kPosition2);

                // Get the normal of the triangle
                float fDot = 0.0f;
                if (fNormalModifier != 0.0f)
                {
                    NiPoint3 kNormal = NiGeometricUtils::GetTriNormal(
                        kPosition0, kPosition1, kPosition2);
                    NiPoint3 kDirection = kCenter - kMeshCenter;
                    kDirection.Unitize();
                    fDot = kNormal.Dot(kDirection);
                    pfNormalWeights[uiCount] = fDot;
                    fMinDot = (fDot < fMinDot) ? fDot : fMinDot;
                    fMaxDot = (fDot > fMaxDot) ? fDot : fMaxDot;
                }

                // Get the distance from the up axis
                float fDist = 0.0f;
                if (fPositionModifier != 0.0f)
                {
                    fDist = NiGeometricUtils::GetDistSqrToSeg(
                        kMeshCenter, NiPoint3(0,0,1), kCenter);
                    pfPositionWeights[uiCount] = fDist;
                    fMinDist = (fDist < fMinDist) ? fDist : fMinDist;
                    fMaxDist = (fDist > fMaxDist) ? fDist : fMaxDist;
                }
            }

            // Increment the counter
            uiCount++;
        }

        // Normalize the weights
        float fSizeInvRange = 0.0f; // Bigger is better.
        if (fSizeModifier != 0)
            fSizeInvRange = fSizeModifier / (fMaxArea - fMinArea);

        float fNormalInvRange = 0.0f; // Closer to 0 is better.
        if (fNormalModifier != 0.0f)
            fNormalInvRange = fNormalModifier / (fMaxDot - fMinDot);

        float fPositionInvRange = 0.0f; // Farther from center is better.
        if (fPositionModifier != 0.0f)
            fPositionInvRange = fPositionModifier / (fMaxDist - fMinDist);

        // Sort the triangles from the highest to the lowest
        WeightSort* pkWeights = NiNew WeightSort[uiPrimitiveCount];
        for (NiUInt32 ui = 0; ui < uiPrimitiveCount; ui++)
        {
            float fWeight = (pfSizeWeights[ui] - fMinArea) * fSizeInvRange;
            fWeight += 1.0f - 
                ((pfNormalWeights[ui] - fMinDot) * fNormalInvRange);
            fWeight += (pfPositionWeights[ui] - fMinDist) * fPositionInvRange;

            pkWeights[ui].m_fWeight = fWeight;
            pkWeights[ui].m_uiIndex = ui;
        }
        WeightSort::Sort(pkWeights, uiPrimitiveCount);

        // We can delete the old weights
        NiFree(pfPositionWeights);
        NiFree(pfNormalWeights);
        NiFree(pfSizeWeights);

        // Rearrange the index buffer to match the new order

        // Copy the index buffer
        NiUInt32 * puiCopyIndices = NiAlloc(NiUInt32, uiIndexCount);
        for (NiUInt32 ui = 0; ui < uiIndexCount; ui++)
        {
            puiCopyIndices[ui] = kIndexIter[ui];
        }

        for (NiUInt32 ui = 0; ui < uiPrimitiveCount; ui++)
        {
            NiUInt32 uiDestIndex = ui * 3;
            NiUInt32 uiSourceIndex = pkWeights[ui].m_uiIndex * 3;
            NIASSERT(uiSourceIndex < uiIndexCount);
            kIndexIter[uiDestIndex] = 
                puiCopyIndices[uiSourceIndex];
            kIndexIter[uiDestIndex + 1] = 
                puiCopyIndices[uiSourceIndex + 1];
            kIndexIter[uiDestIndex + 2] = 
                puiCopyIndices[uiSourceIndex + 2];
        }

        NiFree(puiCopyIndices);
        NiDelete [] pkWeights;
    }
}
//-------------------------------------------------------------------------
const float cfVertexCacheWeight_CacheDecayPower = 1.5f;
const float cfVertexCacheWeight_LastTriScore = 0.75f;
const float cfVertexCacheWeight_ValenceBoostScale = 2.0f;
const float cfVertexCacheWeight_ValenceBoostPower = 0.5f;
//-------------------------------------------------------------------------
float NiMeshTools::GetVertexCacheWeight(
    const VertexWeightData* pkData, 
    const NiUInt32 uiVertexCacheSize)
{
    // Check to see if we are being used.
    if (!pkData->m_uiRefCount)
    {
        return 0.0f;
    }
 
    // Calculate the weight value
    float fWeight = 0.0f;
    NiUInt32 uiCachePosition = pkData->m_uiCacheIndex;
    if (uiCachePosition != 0xffffffff) // Make sure we are in the cache
    {  
        if (uiCachePosition < 3)
        {
            // Was just used right before us so give a constant weight
            fWeight = cfVertexCacheWeight_LastTriScore;
        }
        else
        {
            // The more recently it was used the higher the value
            const float fScaler = 1.0f / (float)(uiVertexCacheSize - 3);
            fWeight = 1.0f - ((float)(uiCachePosition - 3) * fScaler);
            fWeight = powf(fWeight, cfVertexCacheWeight_CacheDecayPower);
        }
    }

    // We give higher weights to verts that do not have many tris still
    // referencing it.
    fWeight += cfVertexCacheWeight_ValenceBoostScale * 
        powf((float)pkData->m_uiRefCount,
            -cfVertexCacheWeight_ValenceBoostPower);

    // Return this weight
    return fWeight;

}
//-------------------------------------------------------------------------
void NiMeshTools::InitVertexCache(
    const NiUInt32 uiPrimitiveCount,
    const PrimitiveWeightData* pkPrimitiveWeights,
    const NiUInt32 uiVertexCacheSize,
    NiTPrimitiveArray<VertexWeightData*>& kVertexCache)
{
    // Run through all vertices and add the primitives in the correct order
    for (NiUInt32 ui = 0; ui < uiPrimitiveCount; ui++)
    {
        // Run through all the vertices and add it to the cache
        for (NiUInt32 uiVert = 0; uiVert < 3; uiVert++)
        {
            VertexWeightData* pkVertexWeight = 
                pkPrimitiveWeights[ui].m_apVerts[uiVert];
            NIASSERT(pkVertexWeight);
            
            // Remove it if it is in the cache
            if (pkVertexWeight->m_uiCacheIndex != 0xffffffff)
            {
                NIASSERT(kVertexCache[pkVertexWeight->m_uiCacheIndex] == 
                    pkVertexWeight);
                kVertexCache.Delete(pkVertexWeight->m_uiCacheIndex);
            }

            // Set it in the correct position
            kVertexCache.Insert(0, pkVertexWeight);

            // Make sure the cache is in order
            for (NiUInt32 uiIndex = 0; 
                uiIndex < kVertexCache.GetSize(); uiIndex++)
            {
                kVertexCache[uiIndex]->m_uiCacheIndex = uiIndex;
            }

            // If we have reached our capacity return
            if (kVertexCache.GetSize() >= uiVertexCacheSize)
                return;
        }
    }
}      
//-------------------------------------------------------------------------
bool NiMeshTools::AddBestPrimitive(const NiUInt32 uiVertexCacheSize,
   const NiUInt32 uiVertexCount, const NiUInt32 uiPrimitiveCount,
   NiTPrimitiveArray<VertexWeightData*>& kVertexCache,
   VertexWeightData* pkVertexWeights, PrimitiveWeightData* pkPrimitiveWeights,
   NiTStridedRandomAccessIterator<NiUInt32>& kIndexIter,
   NiUInt32& uiVertexCounter, NiUInt32& uiPrimitiveCounter)
{
    // Max weight
    float fMaxWeight = -FLT_MAX;
    PrimitiveWeightData* pkBestPrimitive = NULL;

    // Init the weight values
    for (NiUInt32 uiVert = 0; uiVert < uiVertexCount; uiVert++)
    {
        pkVertexWeights[uiVert].m_fWeight = GetVertexCacheWeight(
            &pkVertexWeights[uiVert], uiVertexCacheSize);
    }

    // Find the best primitive that has not already been added
    for (NiUInt32 ui = 0; ui < uiPrimitiveCount; ui++)
    {  
        // Check to make sure it is not already added and it has the
        // highest weight.
        if (!pkPrimitiveWeights[ui].m_bIsAdded)
        {
            float fWeight = pkPrimitiveWeights[ui].GetTotalWeight();
            if (fWeight > fMaxWeight)
            {
                fMaxWeight = fWeight;
                pkBestPrimitive = &pkPrimitiveWeights[ui];
            }
        }
    }

    // Make sure we found one
    if (pkBestPrimitive)
    {
        // Now add all the vertices for this primitive that have not already
        // been added
        for (NiUInt32 ui = 0; ui < 3; ui++)
        {
            VertexWeightData* pkVertexWeight = pkBestPrimitive->m_apVerts[ui];

            // Check to see if we need to add a vertex
            if (pkVertexWeight->m_uiNewIndex == 0xffffffff)
            {
                // Do this so we know we don't need to add it again
                pkVertexWeight->m_uiNewIndex = uiVertexCounter;

                // Increment the counter
                uiVertexCounter++;
            }

            // Write this index to the new location in the index buffer
            kIndexIter[(uiPrimitiveCounter * 3) + ui] = 
                pkVertexWeight->m_uiNewIndex;

            // Decrement the ref count
            NIASSERT(pkVertexWeight->m_uiRefCount);
            pkVertexWeight->m_uiRefCount--;

            // Remove it if it is in the cache
            if (pkVertexWeight->m_uiCacheIndex != 0xffffffff)
            {
                NIASSERT(kVertexCache[pkVertexWeight->m_uiCacheIndex] == 
                    pkVertexWeight);
                kVertexCache.Delete(pkVertexWeight->m_uiCacheIndex);
            }

            // Set it in the correct position
            kVertexCache.Insert(0, pkVertexWeight);

            // Remove the vertices if the cache is too big
            for (NiUInt32 uiIndex = 0; 
                uiIndex < kVertexCache.GetSize(); uiIndex++)
            {
                // Make sure we fit the cache size
                if (uiIndex >= uiVertexCacheSize)
                {
                    // At this point we want to add the vertex
                    kVertexCache[uiIndex]->m_uiCacheIndex = 0xffffffff;
                    kVertexCache.Delete(uiIndex); 
                    uiIndex--;
                }
                // Increment the index for the remainder in the cache
                else
                {
                    kVertexCache[uiIndex]->m_uiCacheIndex = uiIndex;
                }
            }
        }

        // Set the primitive index
        pkBestPrimitive->m_bIsAdded = true;

        // Increment the counter
        uiPrimitiveCounter++;        
    }

    // Return true if we found a primitive to add
    return pkBestPrimitive != NULL;
}               
//-------------------------------------------------------------------------
void NiMeshTools::VertexCacheOptimization(NiMesh* pkMesh,
    const NiUInt32 uiVertexCacheSize)
{
    // Quick out we only support triangle lists
    if (!pkMesh || 
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES)
    {
        return;
    }

    // Quick out if we set the cache size to be 0
    if (uiVertexCacheSize==0)
        return;

    // Lock the index stream
    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT32_1, 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    if (!kIndexLock.IsLocked())
        return;

    // Run through all the sub-meshes
    for (NiUInt32 uiSubmesh = 0; uiSubmesh < pkMesh->GetSubmeshCount(); 
        uiSubmesh++)
    {
        NiUInt32 uiVertexCount = pkMesh->GetVertexCount(uiSubmesh);
        if (uiVertexCount <= uiVertexCacheSize)
            continue;

        // Lock the index buffer to read and write
        NiUInt32 uiIndexCount = kIndexLock.count(uiSubmesh);

        NiUInt32 uiPrimitiveCount = uiIndexCount / 3;

        NiTStridedRandomAccessIterator<NiUInt32> kIndexIter = 
            kIndexLock.begin<NiUInt32>(uiSubmesh);

        // Initialize the weights of each vertex
        VertexWeightData* pkVertexWeights = 
            NiNew VertexWeightData[uiVertexCount];
        PrimitiveWeightData* pkPrimitiveWeights = 
            NiNew PrimitiveWeightData[uiPrimitiveCount];
        NiTPrimitiveArray<VertexWeightData*> kVertexCache;

        // Set the grow by size to twice as much as the cache size to eliminate
        // reallocation.
        kVertexCache.SetGrowBy(uiVertexCacheSize * 2);

        // Set all the vertices in order that the index buffer specifies
        for (NiUInt32 ui = 0; ui < uiPrimitiveCount; ui++)
        {
            NiUInt32 uiIndex0 = kIndexIter[ui * 3];
            NiUInt32 uiIndex1 = kIndexIter[(ui * 3) + 1];
            NiUInt32 uiIndex2 = kIndexIter[(ui * 3) + 2];

            if (uiIndex0 >= uiVertexCount ||
                uiIndex1 >= uiVertexCount ||
                uiIndex2 >= uiVertexCount)
            {
                NIASSERT(0 && "Invalid index given");
                continue;
            }

            pkVertexWeights[uiIndex0].m_uiOldIndex = uiIndex0;
            pkVertexWeights[uiIndex0].m_uiRefCount++;
            pkVertexWeights[uiIndex1].m_uiOldIndex = uiIndex1;
            pkVertexWeights[uiIndex1].m_uiRefCount++;
            pkVertexWeights[uiIndex2].m_uiOldIndex = uiIndex2;
            pkVertexWeights[uiIndex2].m_uiRefCount++;

            pkPrimitiveWeights[ui].m_apVerts[0] = &pkVertexWeights[uiIndex0];
            pkPrimitiveWeights[ui].m_apVerts[1] = &pkVertexWeights[uiIndex1];
            pkPrimitiveWeights[ui].m_apVerts[2] = &pkVertexWeights[uiIndex2];
        }

        // Init the vertex cache
        InitVertexCache(uiPrimitiveCount, pkPrimitiveWeights,
            uiVertexCacheSize, kVertexCache);

        NiUInt32 uiVertexCounter = 0;
        NiUInt32 uiPrimitiveCounter = 0;

        // Run through until all the primitives have been added
        for (NiUInt32 ui = 0; ui < uiPrimitiveCount; ui++)
        {
            // Find the highest rated primitive that has not
            // been added and add it
            AddBestPrimitive(
                uiVertexCacheSize,
                uiVertexCount,
                uiPrimitiveCount,
                kVertexCache,
                pkVertexWeights,
                pkPrimitiveWeights,
                kIndexIter,
                uiVertexCounter,
                uiPrimitiveCounter);
        }

        // Restructure the model to fit the new layout
        NiUInt32 uiStreamRefCount = pkMesh->GetStreamRefCount();
        for (NiUInt32 uiStream = 0; uiStream <uiStreamRefCount; uiStream++)
        {
            NiDataStreamRef* pkStreamRef = pkMesh->GetStreamRefAt(uiStream);
            if (pkStreamRef->GetUsage() == NiDataStream::USAGE_VERTEX &&
                !pkStreamRef->IsPerInstance())
            {
                NIASSERT(uiVertexCount == pkStreamRef->GetTotalCount());

                NiDataStream* pkDataStream = pkStreamRef->GetDataStream();
                NiUInt32 uiStride = pkStreamRef->GetStride();

                const NiDataStream::Region& kRegion = 
                    pkStreamRef->GetRegionForSubmesh(uiSubmesh);

                NiUInt8* pcData = (NiUInt8*)pkDataStream->Lock(
                    NiDataStream::LOCK_TOOL_READ |
                    NiDataStream::LOCK_TOOL_WRITE);
                if (!pcData)
                {
                    NIASSERT(0 && "Could not lock the data stream.\n");
                    continue;
                }

                NiUInt32 uiElementCount = pkStreamRef->GetElementDescCount();
                for (NiUInt32 uiElement = 0; uiElement < uiElementCount;
                    uiElement++)
                {
                    const NiDataStreamElement& kElement = 
                        pkStreamRef->GetElementDescAt(uiElement);

                    NiUInt8* pcStart = (NiUInt8*)kRegion.ComputeStart(pcData, 
                        uiStride, kElement.GetOffset());

                    NiUInt8* pcBuffer = NiAlloc(NiUInt8, 
                        uiStride * uiVertexCount);
                    memcpy(pcBuffer, pcStart, uiStride * uiVertexCount);

                    // Init the weight values
                    for (NiUInt32 uiVert = 0; uiVert < uiVertexCount; uiVert++)
                    {
                        // Copy the old values over the new ones
                        memcpy(&pcStart[
                            pkVertexWeights[uiVert].m_uiNewIndex * uiStride],
                            &pcBuffer[
                            pkVertexWeights[uiVert].m_uiOldIndex * uiStride],
                            uiStride);
                    }

                    NiFree(pcBuffer);
                }

                pkDataStream->Unlock(
                    NiDataStream::LOCK_TOOL_READ |
                    NiDataStream::LOCK_TOOL_WRITE);
            }
        }

        NiDelete [] pkPrimitiveWeights;
        NiDelete [] pkVertexWeights;
    }
}
//-------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Internal Declarations for classes used with NiMeshAlgorithms to compute
// normals
//---------------------------------------------------------------------------
class CalcNormalLoop
{
public:
    CalcNormalLoop(NiTStridedRandomAccessIterator<NiPoint3>& kVertIter,
        NiTStridedRandomAccessIterator<NiPoint3>& kNormalIter,
        NiTStridedRandomAccessIterator<NiUInt32>& kShareGroupsIter,
        NiTStridedRandomAccessIterator<NiUInt32>& kShareIndexesIter) :
        m_kVertIter(kVertIter), m_kNormalIter(kNormalIter), 
        m_kShareGroupsIter(kShareGroupsIter),
        m_kShareIndexesIter(kShareIndexesIter) {};

    inline bool operator()(const NiUInt32* pIndices, NiUInt32, NiUInt32)
    {
        NiUInt32 ui0 = pIndices[0];
        NiUInt32 ui1 = pIndices[1];
        NiUInt32 ui2 = pIndices[2];

        if (ui0 == ui1 ||
            ui1 == ui2 ||
            ui2 == ui0)
        {
            return true;
        }

        // compute unit length triangle normal
        NiPoint3 kE1 = m_kVertIter[ui1] - m_kVertIter[ui0];
        NiPoint3 kE2 = m_kVertIter[ui2] - m_kVertIter[ui1];
        NiPoint3 kNormal = kE1.UnitCross(kE2);

        // update the running sum of normals at the various vertices
        m_kNormalIter[ui0] += kNormal;
        m_kNormalIter[ui1] += kNormal;
        m_kNormalIter[ui2] += kNormal;

        if (m_kShareGroupsIter.Exists() && m_kShareIndexesIter.Exists())
        {
            NiUInt32 uiInd = m_kShareIndexesIter[ui0];
            NiUInt32 uiNumSharing = m_kShareGroupsIter[uiInd++];
            while (uiNumSharing--)
                m_kNormalIter[m_kShareGroupsIter[uiInd++]] += kNormal;

            uiInd = m_kShareIndexesIter[ui1];
            uiNumSharing = m_kShareGroupsIter[uiInd++];
            while (uiNumSharing--)
                m_kNormalIter[m_kShareGroupsIter[uiInd++]] += kNormal;

            uiInd = m_kShareIndexesIter[ui2];
            uiNumSharing = m_kShareGroupsIter[uiInd++];
            while (uiNumSharing--)
                m_kNormalIter[m_kShareGroupsIter[uiInd++]] += kNormal;
        }

        return true;
    }

    // Data
    NiTStridedRandomAccessIterator<NiPoint3>& m_kVertIter;
    NiTStridedRandomAccessIterator<NiPoint3>& m_kNormalIter;
    NiTStridedRandomAccessIterator<NiUInt32>& m_kShareGroupsIter;
    NiTStridedRandomAccessIterator<NiUInt32>& m_kShareIndexesIter;

private:
    CalcNormalLoop & operator=( const CalcNormalLoop & );
};
//---------------------------------------------------------------------------
void NiMeshTools::ConvertStreamsToPlatformSpecific(
    NiToolPipelineCloneHelper& kCloneHelper)
{
    NiTPrimitiveSet<NiDataStream*> kDataStreamSet;
    kCloneHelper.GetDataStreams(kDataStreamSet);
    NiUInt32 uiStreamCnt = kDataStreamSet.GetSize();
    for(NiUInt32 ui=0; ui<uiStreamCnt; ui++)
    {
        NiDataStream* pkDS = kDataStreamSet.GetAt(ui);
        NIVERIFY(ConvertStreamToPlatformSpecific(kCloneHelper, pkDS));
    }

    // Reset the modifiers after the new data streams have been constructed.
    NiTPrimitiveSet<NiMesh*>& kMeshSet = kCloneHelper.GetCompleteMeshSet();
    NiUInt32 uiMeshCnt = kMeshSet.GetSize();
    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        kMeshSet.GetAt(ui)->ResetModifiers();
    }
}
//---------------------------------------------------------------------------
bool NiMeshTools::ConvertStreamToPlatformSpecific(
    NiToolPipelineCloneHelper& kCloneHelper, NiDataStream* pkDS)
{
    if (!pkDS)
        return false;

    if (NiIsKindOf(NiToolDataStream, pkDS) == false)
        return false;

    NiDataStream* pkReplacement = pkDS->CreateFullCopy();

    if (!pkReplacement)
        return false;

    if (!kCloneHelper.ReplaceDataStream(pkDS, pkReplacement))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshTools::GenerateNormalSharing(NiMesh* pkMesh,
    const NiFixedString& kPosnSemantic, const NiUInt32 uiPosnIndex,
    const NiFixedString& kNormSemantic, const NiUInt32 uiNormIndex)
{
    // This function finds vertices that have the same location and normal
    // vector. It is used to enable normal recomputation for morphing, so it
    // makes all the assumptions that morphing makes.
    
    // Two streams are added to the mesh. The first stream records the
    // sharing groups. The first entry is 0, indicating that group 0 has no
    // sharing. Each group then follows in linear order, with a count followed
    // by the vertices that are in the group. Then the next count, next group,
    // and so on.
    
    // The second stream has an entry for every vertex, with an index
    // into the first stream. If 0, the vertex is not in a group. Otherwise,
    // the index is the count for the group.
    
    // Get the sub-mesh count which is needed to know how many regions we'll
    // need.
    NiUInt32 uiSubMeshCount = pkMesh->GetSubmeshCount();
    
    // Storage for region information
    NiUInt32* puiRegionOffset = NiAlloc(NiUInt32, uiSubMeshCount);
    
    // Declared to be outside the scope of the locks, below.
    NiUInt32 uiVertexCount = 0;
    NiUInt32* puwIndices = 0;
    NiUInt32* puwGroups = 0;

    // Tracks the size of the groups array.
    NiUInt32 uiGroupArraySize = 0;

    // Scope the locks so that they are deleted before the streams are changed
    {
        NiDataStreamElementLock kPosnLock(pkMesh, kPosnSemantic, uiPosnIndex,
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);
        NiDataStreamElementLock kNormLock(pkMesh, kNormSemantic, uiNormIndex,
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);

        if (!kPosnLock.IsLocked() || !kNormLock.IsLocked())
        {
            NiFree(puiRegionOffset);
            return false;
        }
    
        uiVertexCount = kPosnLock.GetDataStream()->GetTotalCount();
        NIASSERT(uiVertexCount == kNormLock.GetDataStream()->GetTotalCount());
        NIASSERT(uiVertexCount < 0x8FFFFFFF);    
    
        // Create the index array.
        puwIndices = NiAlloc(NiUInt32, uiVertexCount);
        memset(puwIndices, 0, sizeof(NiUInt32) * uiVertexCount);

        // Create a temporary sharing array. The worst case is for every vertex
        // to be in a group with one other, with one count for each group,
        // leaving 3/2 size.
        puwGroups = NiAlloc(NiUInt32, uiVertexCount * 3 / 2 + 1);
        
        for (NiUInt32 uiSubMesh = 0; uiSubMesh < uiSubMeshCount; uiSubMesh++)
        {
            // The first group always stores 0, which means no other vertices
            // are shared by vertices in this group. It is the group for
            // vertices that don't share.
            puwGroups[uiGroupArraySize] = 0;
            NiUInt32 uiNoShareGroup = uiGroupArraySize;
            puiRegionOffset[uiSubMesh] = uiGroupArraySize;
            
            uiGroupArraySize++;
            
            NiUInt32 uiPosnCount = kPosnLock.count(uiSubMesh);
            
            NiTStridedRandomAccessIterator<NiPoint3> kPointIter =
                kPosnLock.begin<NiPoint3>(uiSubMesh); 
            NiTStridedRandomAccessIterator<NiPoint3> kNormIter =
                kNormLock.begin<NiPoint3>(uiSubMesh); 

            for (NiUInt32 ui = 0; ui < uiPosnCount; ui++)
            {
                // If we've already found a partner the group index will be
                // non-zero.
                if (puwIndices[ui])
                    continue;
                
                // Default is no-share
                puwIndices[ui] = uiNoShareGroup;

                // If there is to be a group, it will be located at the
                // next open position in the group array.
                NiUInt32 uiCountLocation = uiGroupArraySize;

                NiPoint3 kVert1 = kPointIter[ui];
                NiPoint3 kNorm1 = kNormIter[ui];
                for (unsigned int uj = ui + 1; uj < uiPosnCount; uj++)
                {
                    NiPoint3 kVert2 = kPointIter[uj];
                    NiPoint3 kNorm2 = kNormIter[uj];
                    if (kVert1 == kVert2 && kNorm1 == kNorm2)
                    {
                        // These vertices should share
                        if (!puwIndices[ui])
                        {
                            puwIndices[ui] = uiCountLocation;
                            puwGroups[uiGroupArraySize++] = 1;
                            puwGroups[uiGroupArraySize++] = ui;
                        }
                        puwIndices[uj] = uiCountLocation;
                        puwGroups[uiGroupArraySize++] = uj;
                        puwGroups[uiCountLocation]++;
                    }
                }
            }
        }
    }

    NIASSERT(puwIndices);
    NIASSERT(puwGroups);

    // Remove any existing share index stream.
    NiDataStreamRef* pkShareIndexRef = pkMesh->FindStreamRef(
        NiCommonSemantics::NORMALSHAREINDEX(), 0);
    if (pkShareIndexRef)
    {
        // Delete and replace, because the original stream was
        // generated WRITE_STATIC
        pkMesh->RemoveStreamRef(pkShareIndexRef);
    }

    // Create the share index data stream but do not assign regions
    NiDataStreamPtr spShareIndexStream = 
        NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1, uiVertexCount,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, puwIndices, false, false);
    NIASSERT(spShareIndexStream);
    
    // Assign regions for the share indexes
    NiDataStream::Region kRegion;
    NiUInt32 uiRegionStart = 0;
    for (NiUInt32 uiSubMesh = 0; uiSubMesh < uiSubMeshCount; uiSubMesh++)
    {
        kRegion.SetStartIndex(uiRegionStart);
        NiUInt32 uiSubmeshVertexCount = pkMesh->GetVertexCount(uiSubMesh);
        kRegion.SetRange(uiSubmeshVertexCount);
        uiRegionStart += uiSubmeshVertexCount;
        
        spShareIndexStream->AddRegion(kRegion);
    }
    pkShareIndexRef = pkMesh->AddStreamRef(spShareIndexStream,
        NiCommonSemantics::NORMALSHAREINDEX(), 0); 
    for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        pkShareIndexRef->BindRegionToSubmesh(ui, ui);
    }

    // Remove any existing share group stream.
    NiDataStreamRef* pkShareGroupRef = pkMesh->FindStreamRef(
        NiCommonSemantics::NORMALSHAREGROUP(), 0);
    if (pkShareGroupRef)
    {
        // Delete and replace, because the original stream was
        // generated WRITE_STATIC
        pkMesh->RemoveStreamRef(pkShareGroupRef);
    }

    // Create the share group data stream
    NiDataStreamPtr spShareGroupStream = 
        NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1, uiGroupArraySize,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_USER, puwGroups, false, false);
    NIASSERT(spShareGroupStream);
    
    // Assign regions for the share groups
    for (NiUInt32 uiSubMesh = 0; uiSubMesh < uiSubMeshCount; uiSubMesh++)
    {
        kRegion.SetStartIndex(puiRegionOffset[uiSubMesh]);
        NiUInt32 uiEndPosn = uiSubMesh == uiSubMeshCount - 1 ?
            uiGroupArraySize : puiRegionOffset[uiSubMesh + 1];
        kRegion.SetRange(uiEndPosn - puiRegionOffset[uiSubMesh]);
        
        spShareGroupStream->AddRegion(kRegion);
    }
    pkShareGroupRef = pkMesh->AddStreamRef(spShareGroupStream,
        NiCommonSemantics::NORMALSHAREGROUP(), 0); 
    for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        pkShareGroupRef->BindRegionToSubmesh(ui, ui);
    }

    NiFree(puiRegionOffset);
    NiFree(puwGroups);
    NiFree(puwIndices);

    return true;
}
//----------------------------------------------------------------------------
bool NiMeshTools::MakeSubmeshesForGPUSkinning(NiToolPipelineCloneHelper&
    kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet, 
    unsigned char ucBonesPerSubmesh)
{
    // NiSkinningUtilities is a heafty function. A thin wrapper is provided 
    // here so that is is documented as a NiMeshTool function, however the
    // implementation is in NiSkinningUtilites proper.
    return NiSkinningUtilities::MakeSubmeshesForGPUSkinning(kCloneHelper,
        spCloneSet, ucBonesPerSubmesh);
}
//----------------------------------------------------------------------------
bool NiMeshTools::RemapSkinningMeshModifier(NiSkinningMeshModifier* pkSkinMod,
    NiUInt32 uiNewBoneCount, NiUInt32* puiIndices)
{
    if (!pkSkinMod)
        return false;

    NiUInt32 uiOldBoneCount = pkSkinMod->GetBoneCount();

    // Validate index range
    NiUInt32 ui;
    for (ui = 0; ui < uiNewBoneCount; ui++)
    {
        if (puiIndices[ui] >= uiOldBoneCount)
            return false;
    }

    NiTransform* pkTransforms = pkSkinMod->GetSkinToBoneTransforms();
    NiTransform* pkCopyTransforms = NiAlloc(NiTransform, uiNewBoneCount);

    NiAVObject** ppkBones = pkSkinMod->GetBones();
    NiAVObject** ppkCopyBones = NiAlloc(NiAVObject*, uiNewBoneCount);

    NiBound* pkBounds = pkSkinMod->GetBoneBounds();
    NiBound* pkCopyBounds = NULL;
    if (pkBounds)
        pkCopyBounds = NiAlloc(NiBound, uiNewBoneCount);
        
    // Remap and copy data
    for (ui = 0; ui < uiNewBoneCount; ui++)
    {
        NiUInt32 uiOldIndex = puiIndices[ui];
        pkCopyTransforms[ui] = pkTransforms[uiOldIndex];
        ppkCopyBones[ui] = ppkBones[uiOldIndex];
        if (pkBounds)
            pkCopyBounds[ui] = pkBounds[uiOldIndex];
    }

    if (uiNewBoneCount != uiOldBoneCount)
        pkSkinMod->Resize(uiNewBoneCount);

    // Get new arrays
    pkTransforms = pkSkinMod->GetSkinToBoneTransforms();
    ppkBones = pkSkinMod->GetBones();
    pkBounds = pkSkinMod->GetBoneBounds();

    // Fill new arrays with remapped data
    for (ui = 0; ui < uiNewBoneCount; ui++)
    {
        pkTransforms[ui] = pkCopyTransforms[ui];
        ppkBones[ui] = ppkCopyBones[ui];
        if (pkBounds)
            pkBounds[ui] = pkCopyBounds[ui];
    }

    NiFree(ppkCopyBones);
    NiFree(pkCopyTransforms);
    NiFree(pkCopyBounds);

    return true;
}
//----------------------------------------------------------------------------
