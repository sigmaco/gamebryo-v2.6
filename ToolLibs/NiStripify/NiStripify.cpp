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

#include "NiStripify.h"
#include "NvTriStrip.h"
#include <NiCommonSemantics.h>
#include <NiDataStream.h>
#include <NiDataStreamElementLock.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiMesh.h>
#include <NiNode.h>
#include <NiObjectNET.h>
#include <NiRoom.h>
#include <NiStencilProperty.h>
#include <NiStringExtraData.h>
#include <NiSystem.h>
#include <NiRoom.h>

// static class data
NiStripify::OkayCallback NiStripify::ms_pfnOkayCallback = NULL;
NiStripify::FinishedCallback NiStripify::ms_pfnFinishedCallback = NULL;
NiStripify::ErrorCallback NiStripify::ms_pfnErrorCallback = NULL;

void* NiStripify::ms_pvCallbackData;
NiStripify::Stats NiStripify::ms_kStats;
NiStripify::Stats NiStripify::ms_knVStats;

unsigned int NiStripify::ms_uiIndexSum = 0;

//---------------------------------------------------------------------------
void NiStripify::Init()
{
}
//---------------------------------------------------------------------------
void NiStripify::Shutdown()
{
}
//---------------------------------------------------------------------------
bool NiStripify::Stripify(NiMesh* pkMesh, unsigned int uiCacheSize, 
    DataStreamMap& kMeshMap)
{
    if (pkMesh->GetPrimitiveType() == NiPrimitiveType::PRIMITIVE_TRISTRIPS)
        return true;
    
    NiDataStreamRef* pkIndexStreamRef = (NiDataStreamRef*)
        pkMesh->GetFirstStreamRef(NiDataStream::USAGE_VERTEX_INDEX);

    if (!pkIndexStreamRef)
        return false;

    NiDataStream* pkIndexBufferStream = pkIndexStreamRef->GetDataStream();    

    if (!pkIndexBufferStream)
        return false;

    NiDataStream* pkStrippedIndexBufferStream = NULL;
   
    if (!kMeshMap.GetAt(pkIndexBufferStream, pkStrippedIndexBufferStream))    
    {
        if (!Stripify(pkMesh, uiCacheSize, kMeshMap, pkIndexStreamRef))
        {
            NILOG("Failed to convert mesh.\n");
            return false;
        }
    }    
    else
    {
        if (!ConvertClone(pkMesh, kMeshMap))
        {
            NILOG("Failed to convert cloned mesh.\n");
            return false;
        }
    }

    pkMesh->ResetModifiers();
    return true;
}
//---------------------------------------------------------------------------
void NiStripify::Stripify(NiNode* pkNode, unsigned int uiCacheSize,
    DataStreamMap& kDataMap)
{
    NiUInt32 ui;

    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild != NULL)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                if (ms_pfnOkayCallback == NULL ||
                    ms_pfnOkayCallback(pkChild, ms_pvCallbackData))
                {
                    if (NoStripifyExtraDataIsNotPresent(pkChild))
                        Stripify((NiNode*) pkChild, uiCacheSize, kDataMap);
                }
            }
            else if (NiIsExactKindOf(NiMesh, pkChild) && 
                NoStripifyExtraDataIsNotPresent(pkChild))
            {                                
                NiMesh* pkMesh = (NiMesh*)pkChild;
                bool result = Stripify(pkMesh, uiCacheSize, kDataMap);
                if (result && ms_pfnFinishedCallback != NULL)
                {
                    ms_pfnFinishedCallback(pkMesh);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiStripify::NoStripifyExtraDataIsNotPresent(NiObjectNET* pkObject)
{
    if (pkObject == NULL)
        return false;

    for (unsigned int ui = 0; ui < pkObject->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkED = pkObject->GetExtraDataAt((unsigned short)ui);
        if (NiIsKindOf(NiStringExtraData, pkED))
        {
            NiStringExtraData* pkStringED = (NiStringExtraData*) pkED;
            const char* pcValue = pkStringED->GetValue();
            const char* pcFoundLocation = strstr(pcValue, "NiNoStripify");
            if (pcFoundLocation != NULL)
                return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
void NiStripify::Stripify(NiNode* pkNode, unsigned int uiCacheSize)
{
    DataStreamMap kMap;
    if (NoStripifyExtraDataIsNotPresent(pkNode))
        Stripify(pkNode, uiCacheSize, kMap);
}
//---------------------------------------------------------------------------
bool NiStripify::Stripify(NiMesh* pkMesh, unsigned int uiCacheSize, 
    DataStreamMap& kMeshMap, NiDataStreamRef* pkIndexBufferRef)
{
    NIASSERT(NiIsExactKindOf(NiMesh, pkMesh));

    if (ms_pfnOkayCallback != NULL &&
        !ms_pfnOkayCallback(pkMesh, ms_pvCallbackData))
    {
        return false;
    }

    // Index buffer should just have one element
    if (pkIndexBufferRef->GetElementDescCount() != 1)
        return false;

    // Index buffer should have a 32 bit unsigned integer index buffer
    if (pkIndexBufferRef->GetElementDescAt(0).GetFormat() != 
        NiDataStreamElement::F_UINT32_1)
    {
        return false;
    }

    NiUInt32 uiElementOffsetInBytes = 
        pkIndexBufferRef->GetElementDescAt(0).GetOffset();

    if (uiElementOffsetInBytes != 0)
        return false;

    SetCacheSize(uiCacheSize);
    SetStitchStrips(true);
    SetMinStripSize(0);
    SetListsOnly(false);

    NiUInt32 uiSubmeshCount = pkMesh->GetSubmeshCount();

    // allocate the submesh arrays
    NiUInt32* puiNumPrimGroups = NiAlloc(NiUInt32, uiSubmeshCount);
    PrimitiveGroup** ppkPrimGroups = NiAlloc(PrimitiveGroup*, uiSubmeshCount);
    PrimitiveGroup** ppkRemappedPrimGroups = NiAlloc(PrimitiveGroup*, 
        uiSubmeshCount);
    NiUInt32** ppuiVertexRemappings = NiAlloc(NiUInt32*, uiSubmeshCount);

    NiDataStreamPtr spSrcIndexBuffer = pkIndexBufferRef->GetDataStream();
    void* pvSrcIndices = spSrcIndexBuffer->Lock(
        NiDataStream::LOCK_TOOL_READ);

    NiUInt32 uiTotalDestIndexCount = 0;

    // Generate strips for each submesh
    for (NiUInt32 uiSubmeshIdx = 0; uiSubmeshIdx < uiSubmeshCount; 
        uiSubmeshIdx++)
    {
        NiUInt32 uiTriCount = pkMesh->GetPrimitiveCount(uiSubmeshIdx);
        NiUInt32 uiVertexCount = pkMesh->GetVertexCount(uiSubmeshIdx);

        // Compute the sizes for this submesh
        NiDataStream::Region kRegion = pkIndexBufferRef->GetRegionForSubmesh(
            uiSubmeshIdx);
        NiUInt32 uiNumSrcIndices = kRegion.GetRange();

#ifdef NIDEBUG
        // Uncomment the following line to validate the stripification outputs.
        // bool bValidate = true;

        bool bValidate = false;
#else
        bool bValidate = false;
#endif

        unsigned int* puiSrcIndices = 
            (NiUInt32*) kRegion.ComputeStart(pvSrcIndices,
                spSrcIndexBuffer->GetStride(), uiElementOffsetInBytes);

        // Generate the strips for this submesh
        bool bGenerated = 
            GenerateStrips(puiSrcIndices, uiNumSrcIndices, 
            &(ppkPrimGroups[uiSubmeshIdx]),
            &(puiNumPrimGroups[uiSubmeshIdx]), 
            bValidate);
        NI_UNUSED_ARG(bGenerated);
        NIASSERT(bGenerated);

        // We expect to only have one primitive group due to the stitching
        // of strips
        NIASSERT(puiNumPrimGroups[uiSubmeshIdx] == 1);

        // Now remap the index buffer to be more cache efficients
        RemapIndices(ppkPrimGroups[uiSubmeshIdx], 
            puiNumPrimGroups[uiSubmeshIdx], uiVertexCount,
            &(ppkRemappedPrimGroups[uiSubmeshIdx]));

        // Build the remap buffer
        ppuiVertexRemappings[uiSubmeshIdx] = NiAlloc(NiUInt32, 
            uiVertexCount);
        memset(ppuiVertexRemappings[uiSubmeshIdx], 0xFFFFFFFF, 
            sizeof(NiUInt32)*uiVertexCount);

        // Note that we assume only one primitive group per submesh when 
        // we generate the vertex remap buffer. This is tested in the assertion
        // above.
        for (NiUInt32 uiIndex = 0;
            uiIndex < ppkRemappedPrimGroups[uiSubmeshIdx][0].numIndices; 
            uiIndex++)
        {
            //  Old index
            NiUInt32 uiOldIndex = 
                ppkPrimGroups[uiSubmeshIdx][0].indices[uiIndex];

            //  New index
            NiUInt32 uiNewIndex = 
                ppkRemappedPrimGroups[uiSubmeshIdx][0].indices[uiIndex];

            // Set remap index
            ppuiVertexRemappings[uiSubmeshIdx][uiOldIndex] = uiNewIndex;
        }

        uiTotalDestIndexCount += 
            ppkRemappedPrimGroups[uiSubmeshIdx][0].numIndices;
    
        // Update stats
        NiUInt32 uiStripTriCount = 
            ppkRemappedPrimGroups[uiSubmeshIdx][0].numIndices - 2;
        ms_kStats.m_uiStrips++;     
        ms_kStats.m_uiTriangles += uiTriCount;
        ms_kStats.m_uiStripsTriangles += uiStripTriCount;
    }

    // Unlock the index buffer
    spSrcIndexBuffer->Unlock(NiDataStream::LOCK_TOOL_READ);

    // Create the new index buffer
    NiDataStreamPtr spDestIndexBuffer = 
        NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1, uiTotalDestIndexCount, 
        spSrcIndexBuffer->GetAccessMask(), 
        spSrcIndexBuffer->GetUsage(), 0, false, true);
    NIASSERT(spDestIndexBuffer != NULL);

    // Regenerate the index and vertex buffers.
    void* pvDestIndices = spDestIndexBuffer->Lock(
        NiDataStream::LOCK_TOOL_WRITE);
    pkIndexBufferRef->SetDataStream(spDestIndexBuffer);
    NiUInt32 uiRegionBeginIdx = 0;
    NiUInt32 uiIndexStride = pkIndexBufferRef->GetStride();

    for (NiUInt32 uiSubmeshIdx = 0; uiSubmeshIdx < uiSubmeshCount; 
        uiSubmeshIdx++)
    {
        // Get the new index buffer for this submesh
        const NiUInt32* puiRemapIndices = 
            ppkRemappedPrimGroups[uiSubmeshIdx][0].indices;

        // Build a region to encompass the new buffer
        NiDataStream::Region kRegion(uiRegionBeginIdx,
            ppkRemappedPrimGroups[uiSubmeshIdx][0].numIndices);

        // Add the new region to the datastream & ref
        pkIndexBufferRef->BindRegionToSubmesh(uiSubmeshIdx, kRegion);

        // Compute the starting point for this submesh
        NiUInt32* puiDestIndices = (NiUInt32*)kRegion.ComputeStart(
            pvDestIndices, uiIndexStride);

        // Compute the size of the whole region to copy, since we will
        // be copying wholesale from the input array.
        size_t stDestSize = kRegion.ComputeSizeInBytes(uiIndexStride);
        NIASSERT(stDestSize == sizeof(NiUInt32)*kRegion.GetRange());

        // 1:1 copy data over
        NiMemcpy(puiDestIndices, stDestSize, puiRemapIndices, 
            stDestSize);

        uiRegionBeginIdx += kRegion.GetRange();
    }
    spDestIndexBuffer->Unlock(NiDataStream::LOCK_TOOL_WRITE);

    // Make sure the index buffer is in the clone map
    kMeshMap.SetAt(spSrcIndexBuffer, spDestIndexBuffer);

    // Remap and copy over the vertex buffers on this mesh
    if (!RemapVertices(pkMesh, kMeshMap, ppuiVertexRemappings))
        return false;
    
    // Clean up the data
    for (NiUInt32 uiSubmeshIdx = 0; uiSubmeshIdx < uiSubmeshCount; 
        uiSubmeshIdx++)
    {
        NiExternalDelete [] ppkRemappedPrimGroups[uiSubmeshIdx];
        NiExternalDelete [] ppkPrimGroups[uiSubmeshIdx];
        NiFree(ppuiVertexRemappings[uiSubmeshIdx]);
    }
    
    NiFree(puiNumPrimGroups);
    NiFree(ppkRemappedPrimGroups);
    NiFree(ppkPrimGroups);
    NiFree(ppuiVertexRemappings);


    // update stats
    ms_kStats.m_uiConvertedMeshes++;

    // Set the primitive type
    pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRISTRIPS);

    return true;
}
//---------------------------------------------------------------------------
bool NiStripify::ConvertClone(NiMesh* pkMesh, DataStreamMap& kMeshMap)
{
    if (ms_pfnOkayCallback != NULL &&
        !ms_pfnOkayCallback(pkMesh, ms_pvCallbackData))
    {
        return false;
    }

    NIASSERT(NiIsExactKindOf(NiMesh, pkMesh));
    for (unsigned int ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkRef = pkMesh->GetStreamRefAt(ui);

        // Only convert vertex and index buffers
        if ((pkRef->GetUsage() != NiDataStream::USAGE_VERTEX &&
            pkRef->GetUsage() != NiDataStream::USAGE_VERTEX_INDEX) ||
            pkRef->IsPerInstance())
        {
            continue;
        }

        NiDataStream* pkReplaceStream = NULL;
        if (kMeshMap.GetAt(pkRef->GetDataStream(), pkReplaceStream) &&
            pkReplaceStream != NULL)
        {
            pkRef->SetDataStream(pkReplaceStream);
        }
        else
        {
            NILOG("This was supposed to be a clone, but some of it's streams"
                " were unknown. Leaving old streams as-is.\n");
        }
    }

    
    // update stats
    ms_kStats.m_uiConvertedMeshes++;

    pkMesh->SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRISTRIPS);

    return true;
}
//---------------------------------------------------------------------------
bool NiStripify::RemapVertices(NiMesh* pkMesh, DataStreamMap& kMeshMap, 
    NiUInt32** ppuiVertexRemappings)
{
    for (unsigned int ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkRef = pkMesh->GetStreamRefAt(ui);
        NIASSERT(pkRef);

        // We're only remapping vertex streams
        if (pkRef->GetUsage() != NiDataStream::USAGE_VERTEX ||
            pkRef->IsPerInstance())
            continue;

        NiDataStreamPtr spSrcDS = pkRef->GetDataStream();
        NIASSERT(spSrcDS != NULL);

        NiDataStream* pkTemp = NULL;
        NI_UNUSED_ARG(pkTemp);
        NIASSERT(kMeshMap.GetAt(spSrcDS, pkTemp) == false && pkTemp == NULL);
        
        // Create the duplicate copy, but don't copy the buffers. We'll do
        // that manually
        NiDataStreamPtr spDestDS = spSrcDS->CreateFullCopyEx(
            spSrcDS->GetAccessMask(), spSrcDS->GetUsage(), true, false);
        NIASSERT(spDestDS != NULL);

        // Lock the buffers
        void* pvSrcData = spSrcDS->Lock(NiDataStream::LOCK_TOOL_READ);
        void* pvDestData = spDestDS->Lock(NiDataStream::LOCK_TOOL_WRITE);

        NiUInt32 uiVertexStride = spSrcDS->GetStride();

        for (NiUInt32 uiSubmeshIdx = 0; 
            uiSubmeshIdx < pkMesh->GetSubmeshCount(); uiSubmeshIdx++)
        {
            // Get the region associated with this submesh.
            NiDataStream::Region kRegion = 
                pkRef->GetRegionForSubmesh(uiSubmeshIdx);

            // Compute the starting pointers for this submesh
            void* pvSrcDataSubmesh = kRegion.ComputeStart(pvSrcData, 
                uiVertexStride);
            void* pvDestDataSubmesh = kRegion.ComputeStart(pvDestData,
                uiVertexStride);

            // Get the vertex remapping buffer for this submesh
            const NiUInt32* puiVertexRemappings = 
                ppuiVertexRemappings[uiSubmeshIdx];

            for (NiUInt32 ui = 0; ui < kRegion.GetRange(); ui++)
            {
                // Look up the new location of this vertex
                NiUInt32 uiNewIndex = puiVertexRemappings[ui];

                // This vertex was unused in the remapping. It's values
                // will be garbage in the new buffer.
                if (uiNewIndex == 0xFFFFFFFF)
                    continue;

                // Copy the vertex at the old index into the new index 
                void* pvDestForIndex = ((char*)pvDestDataSubmesh) + 
                    uiVertexStride * uiNewIndex;
                void* pvSrcForIndex = ((char*)pvSrcDataSubmesh) + 
                    uiVertexStride * ui;

                NiMemcpy(pvDestForIndex, uiVertexStride, pvSrcForIndex, 
                    uiVertexStride);
            }
        }
        
        // Clean up the locked buffers
        spSrcDS->Unlock(NiDataStream::LOCK_TOOL_READ);
        spDestDS->Unlock(NiDataStream::LOCK_TOOL_WRITE);

        // Set to the new datastream
        pkRef->SetDataStream(spDestDS);

        // Set this datastream up for sharing
        kMeshMap.SetAt(spSrcDS, spDestDS);
    }

    return true;

}
//---------------------------------------------------------------------------
void NiStripify::SetOkayCallback(OkayCallback pfnCallback, void *pvData)
{
    ms_pfnOkayCallback = pfnCallback;
    ms_pvCallbackData = pvData;
}
//---------------------------------------------------------------------------
void NiStripify::GetOkayCallback(OkayCallback& pfnCallback, void*& pvData)
{
    pfnCallback = ms_pfnOkayCallback;
    pvData = ms_pvCallbackData;
}
//---------------------------------------------------------------------------
void NiStripify::SetFinishedCallback(FinishedCallback pfnCallback)
{
    ms_pfnFinishedCallback = pfnCallback;
}

//---------------------------------------------------------------------------
void NiStripify::GetFinishedCallback(FinishedCallback& pfnCallback)
{
    pfnCallback = ms_pfnFinishedCallback;
}
//---------------------------------------------------------------------------
void NiStripify::SetErrorCallback(ErrorCallback pfnCallback)
{
    ms_pfnErrorCallback = pfnCallback;
}
//---------------------------------------------------------------------------
void NiStripify::GetErrorCallback(ErrorCallback& pfnCallback)
{
    pfnCallback = ms_pfnErrorCallback;
}
//---------------------------------------------------------------------------
void NiStripify::GetStats(Stats& kStats)
{
    kStats = ms_kStats;
}
//---------------------------------------------------------------------------
void NiStripify::GetnVStats(Stats& kStats)
{
    kStats = ms_knVStats;
}
//---------------------------------------------------------------------------
void NiStripify::ResetStats()
{
    ms_kStats.Reset();
}
//---------------------------------------------------------------------------
void NiStripify::ResetnVStats()
{
    ms_knVStats.Reset();
}
//---------------------------------------------------------------------------
NiStripify::Stats::Stats()
{
    Reset();
}

//---------------------------------------------------------------------------
void NiStripify::Stats::Reset()
{    
    m_uiConvertedMeshes = 0;
    m_uiTriangles = 0;
    m_uiStripsTriangles = 0;
    m_uiStrips = 0;
}

//---------------------------------------------------------------------------
void NiStripify::ResetIndexSum(void)
{
    ms_uiIndexSum = 0;
}
//---------------------------------------------------------------------------
unsigned int NiStripify::GetIndexSum(void)
{
    return ms_uiIndexSum;
}
//---------------------------------------------------------------------------
