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

#include "NiInterleaver.h"
#include <NiMeshRequirements.h>
#include <NiMeshModifier.h>
#include <NiToolPipelineCloneHelper.h>
#include <NiNode.h>
#include <NiDataStream.h>
#include <NiToolDataStream.h>
#include <NiTArray.h>
#include <NiDataStreamLock.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiGeometryConverter.h>
#include "NiMPPMessages.h"

typedef NiTStridedRandomAccessIterator<NiInt8>  CharIter;
typedef NiInterleaveProfile::StreamLayout      StreamLayout;
typedef NiInterleaveProfile::StreamLayoutSet   StreamLayoutSet;
typedef NiInterleaveProfile::Element           Element;

//--------------------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------------------
bool IsElementMatch(Element& kElement, Element& kExistingElement)
{
    NiDataStreamRef kCurrRef = kElement.m_kStreamRef;
    NiDataStream* pkCurrDS = kCurrRef.GetDataStream();
    NiDataStream::Usage eCurrUsage = pkCurrDS->GetUsage();
    NiUInt8 uiCurrAccessMask = pkCurrDS->GetAccessMask();
    NiUInt32 uiCurrSubmeshRemapCount = kCurrRef.GetSubmeshRemapCount();
    NiUInt32 uiCurrRegionCnt = pkCurrDS->GetRegionCount();
    bool bPerInstance = kCurrRef.IsPerInstance();

    NiDataStreamRef& kExistRef =
        kExistingElement.m_kStreamRef;
    NiDataStream* pkExistDS = kExistRef.GetDataStream();

    // Must have same usage
    if (eCurrUsage != pkExistDS->GetUsage())
        return false;

    // Must have same access flags
    if (uiCurrAccessMask != pkExistDS->GetAccessMask())
        return false;

    // Must have the same Frequency and Function
    if (bPerInstance != kExistRef.IsPerInstance())
        return false;

    // Must have the same RegionRef count and Region counts
    if ((uiCurrSubmeshRemapCount != kExistRef.GetSubmeshRemapCount())
        || (uiCurrRegionCnt != pkExistDS->GetRegionCount()))
    {
        return false;
    }

    // Must have exactly the same regionrefs
    bool bMatched = true;
    for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiCurrSubmeshRemapCount; 
        uiSubmesh++)
    {
        if (!kCurrRef.GetRegionForSubmesh(uiSubmesh).IsEqual(
            kExistRef.GetRegionForSubmesh(uiSubmesh)))
        {
            bMatched = false;
            break;
        }
    }

    if (!bMatched)
        return false;

    // Must have exactly the same regions
    for (NiUInt32 uiRegion = 0; uiRegion < uiCurrRegionCnt; uiRegion++)
    {
        NiDataStream::Region kCurrRegion = pkCurrDS->GetRegion(
            uiRegion);
        NiDataStream::Region kExistRegion = pkExistDS->GetRegion(
            uiRegion);

        if ((kCurrRegion.GetStartIndex() !=
            kExistRegion.GetStartIndex()) || (kCurrRegion.GetRange() !=
            kExistRegion.GetRange()))
        {
            bMatched = false;
            break;
        }
    }
    if (!bMatched)
        return false;

    return true;
}
//--------------------------------------------------------------------------
bool PlaceStrictIntoLayout(StreamLayoutSet& kNewLayoutSet, Element& kElement)
{
    NIASSERT(kElement.m_bStrictInterleave);

    NiUInt32 uiCount = kNewLayoutSet.GetSize();

    // If uiCount is greater than 1, an error should have occurred.
    NIASSERT(uiCount <= 1);

    if (uiCount == 0)
    {
        StreamLayout kNewLayout;
        kNewLayout.Add(kElement);
        kNewLayoutSet.Add(kNewLayout);
        return true;
    }

    StreamLayout& kStreamLayout = kNewLayoutSet.GetAt(0);
    NIASSERT(kStreamLayout.GetSize() > 0);

    Element& kExistingElement = kStreamLayout.GetAt(0);
    NIASSERT(kExistingElement.m_kStreamRef.IsValid());

    if (IsElementMatch(kElement, kExistingElement))
    {
        kStreamLayout.Add(kElement);
        return true;
    }

    // Element was strict and could not be reconciled.
    return false;
}
//--------------------------------------------------------------------------
void PlaceNonStrictIntoLayout(StreamLayoutSet& kNewLayoutSet,
    Element& kElement)
{
    NiUInt32 uiCount = kNewLayoutSet.GetSize();

    for (NiUInt32 i = 0; i < uiCount; i++)
    {
        StreamLayout& kStreamLayout = kNewLayoutSet.GetAt(i);
        NIASSERT(kStreamLayout.GetSize() > 0);

        Element& kExistingElement = kStreamLayout.GetAt(0);
        NIASSERT(kExistingElement.m_kStreamRef.IsValid());

        if (IsElementMatch(kElement, kExistingElement))
        {
            kStreamLayout.Add(kElement);
            return;
        }

        // Force own datastream
        if (kElement.m_bCoalesceOnSplit == false)
            break;
    }

    // Could not find matching attributes, so must break apart
    StreamLayout kNewLayout;
    kNewLayout.Add(kElement);
    kNewLayoutSet.Add(kNewLayout);
}
//--------------------------------------------------------------------------
void SetElements(NiInterleaveProfile::StreamLayout& kStreamLayout,
    NiDataStreamRef& kPackedStreamRef)
{
    NiUInt32 uiElementIndex = 0;
    for (NiUInt32 i = 0; i < kStreamLayout.GetSize(); ++i)
    {
        // Process next element
        Element& kElement = kStreamLayout.GetAt(i);

        // Bind the semantics to the current element
        kPackedStreamRef.BindSemanticToElementDescAt(uiElementIndex++,
            kElement.m_kSemantic, kElement.m_uiSemanticIndex);
    }
}
//--------------------------------------------------------------------------
NiUInt32 BuildElements(NiInterleaveProfile::StreamLayout& kStreamLayout,
    NiDataStreamElementSet& pkPackedElementSet)
{
    // Builds the elements in an interleaved stream

    // Add element references to packed stream ref from existing stream refs
    // Add elements to packed data stream
    // Accumulate file size of packed data stream
    NiUInt32 uiPackedStride = 0;
    for (NiUInt32 i = 0; i < kStreamLayout.GetSize(); ++i)
    {
        // Process next element
        Element& kElement = kStreamLayout.GetAt(i);

        // If stream ref or element ref are invalid then ignore them
        NiDataStreamRef kStreamRef = kElement.m_kStreamRef;
        NIASSERT(kStreamRef.IsValid());

        // Get the element from the source data stream
        const NiDataStreamElement& kElem = kElement.m_kElement;
        NiDataStreamElement::Format eFormat = kElem.GetFormat();

        // Add the element to the packed stream
        pkPackedElementSet.AddElement(eFormat);

        // Update the stride of the packed stream
        uiPackedStride = (NiUInt32)(uiPackedStride + 
            NiDataStreamElement::SizeOfFormat(eFormat));
    }
    return uiPackedStride;
}
//--------------------------------------------------------------------------
void ConvertData( NiInt8* pcSrc, NiDataStreamElement::Format eSrcFormat,
    NiInt8* pcDest, NiDataStreamElement::Format eDstFormat)
{
    // Converts a single element

    // Compute size of source data element
    NiUInt32 uiSrcSize = (NiUInt32)NiDataStreamElement::SizeOfFormat(eSrcFormat);
    
    // If the source and destination types are the same then direct copy
    if (eSrcFormat == eDstFormat)
    {
        memcpy(pcDest, pcSrc, uiSrcSize);
    }
    else
    {
        NIASSERT(0 && "Type mismatch. Conversion not supported yet.");
    }
}
//--------------------------------------------------------------------------
void CopyStreamData( NiInterleaveProfile::StreamLayout& kStreamLayout,
    NiDataStreamRef&, NiDataStream* pkPackedDataStream)
{
    // Copies the data from stream to stream

    // Lock packed data stream for write. This outer lock is needed 
    // in case pkPackedDataStream is static because static streams
    // may only be locked for write once.
    NIVERIFY(pkPackedDataStream->Lock(NiDataStream::LOCK_TOOL_WRITE) != NULL);

    // Iterate over all source elements
    NiUInt32 uiPackedElementIndex = 0;
    NiUInt32 uiElementCount = kStreamLayout.GetSize();
    for (NiUInt32 i = 0; i < uiElementCount; ++i)
    {
        // Obtain source stream and element refer and check for validity
        Element& kElement = kStreamLayout.GetAt(i);
        NiDataStreamRef& kStreamRef = kElement.m_kStreamRef;
        NIASSERT(kStreamRef.IsValid());

        // Obtain and lock source data stream for read
        NiDataStreamPtr spDataStream = kStreamRef.GetDataStream();
        const NiDataStreamElement& kSourceElement = kElement.m_kElement;
        NiDataStreamElement::Format eSrcFormat = kSourceElement.GetFormat();
        NiDataStreamLock kLockedSrc(spDataStream, 0,
            NiDataStream::LOCK_TOOL_READ);
        NIASSERT(kLockedSrc.IsLocked());

        // Lock the destination (packed) data stream with the offset of the 
        // element corresponding to the element in the current stream 
        const NiDataStreamElement& kDataStreamElement = 
            pkPackedDataStream->GetElementDescAt(uiPackedElementIndex++);
        NiUInt32 uiOffset = kDataStreamElement.GetOffset();
        NiDataStreamElement::Format eDstFormat = 
            kDataStreamElement.GetFormat();
        NiDataStreamLock kLockedDest(pkPackedDataStream, uiOffset,
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
        NIASSERT(kLockedDest.IsLocked());

        // Iterate over data converting and copying        
        for (CharIter kOutIt = kLockedDest.begin<NiInt8>(),
            kInIt = kLockedSrc.begin<NiInt8>();
            kOutIt != kLockedDest.end<NiInt8>();
            kOutIt++, kInIt++)
        {
            ConvertData(&*kInIt, eSrcFormat, &*kOutIt, eDstFormat);
        }
    }

    // Unlock outer packed data stream lock.
    pkPackedDataStream->Unlock(NiDataStream::LOCK_TOOL_WRITE);
}
//--------------------------------------------------------------------------
NiDataStreamRef CreateStreamRef(
    const NiDataStreamRef& kProtoStreamRef)
{
    // Creates a new stream reference
    NiDataStreamRef kRef;
    kRef.SetPerInstance(kProtoStreamRef.IsPerInstance());
    kRef.SetDataStream((NiDataStream*)kProtoStreamRef.GetDataStream());

    // Copy Region binding information.
    for (NiUInt32 i = 0; i < kProtoStreamRef.GetSubmeshRemapCount(); ++i)
    {
        kRef.BindRegionToSubmesh(i, kProtoStreamRef.GetRegionForSubmesh(i));
    }

    return kRef;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// NiInterleaver
//--------------------------------------------------------------------------
NiInterleaver::NiInterleaver()
    : m_pkMesh(NULL)
    , m_pkProfile(NULL)
{
}
//--------------------------------------------------------------------------
NiInterleaver::~NiInterleaver()
{
}
//--------------------------------------------------------------------------
bool NiInterleaver::Interleave(NiToolPipelineCloneHelper::CloneSetPtr&
    spCloneSet, NiToolPipelineCloneHelper& kCloneHelper,
    NiInterleaveProfile* pkProfile, NiMeshErrorInterface* pkLog)
{
    NIASSERT(spCloneSet);
    NIASSERT(pkProfile);

    if (pkProfile->GetStreamLayoutSet().GetSize() == 0)
    {
        pkLog->ReportError(NiMPPMessages::ERROR_NO_STREAM_FOUND);
        return false;
    }

    m_pkCloneSet = spCloneSet;
    m_pkCloneHelper = &kCloneHelper;
    m_pkProfile = pkProfile;

    NIASSERT(spCloneSet->GetSize() > 0);
    if (spCloneSet->GetSize() < 1)
        return false;

    m_pkMesh = m_pkCloneSet->GetAt(0);

    // Ensure that all streams are toolstreams
    for(NiUInt32 ui = 0; ui < m_pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkSR = m_pkMesh->GetStreamRefAt(ui);
        if (NiIsKindOf(NiToolDataStream, pkSR->GetDataStream()) == false)
        {
            NIASSERT(!"Interleave on Non-tool datastreams failed.\n");
            pkLog->ReportError(
                NiMPPMessages::ERROR_NON_TOOL_DATASTREAM_ENCOUNTERED);
            return false;
        }
    }

    // Process the mesh
    if (!Initialize())
    {
        if (pkLog)
            pkLog->ReportError(NiMPPMessages::ERROR_INTERLEAVE_INIT);
        goto INTERLEAVE_ERROR;
    }

    if (!Interleave(pkLog))
    {
        // Errors reported in Interleave
        goto INTERLEAVE_ERROR;
    }

    if (!Finalize(pkLog))
    {
        if (pkLog)
            pkLog->ReportError(NiMPPMessages::ERROR_INTERLEAVE_FINALIZE);
        goto INTERLEAVE_ERROR;
    }

    return true;

INTERLEAVE_ERROR:
    return false;
}
//--------------------------------------------------------------------------
bool NiInterleaver::Initialize()
{
    // Clear existing streams
    m_kStreamRefs.RemoveAll();
    m_kFinishedStreamRefs.RemoveAll();

    NiTPrimitiveSet<NiDataStream*> kDataStreamSet;
    m_pkCloneHelper->GetDataStreams(kDataStreamSet,m_pkCloneSet);

    for (NiUInt32 i = 0; i < m_pkMesh->GetStreamRefCount(); ++i)
    {
        // Get stream reference
        NiDataStreamRef* pkStreamRef = m_pkMesh->GetStreamRefAt(i);
        NIASSERT(pkStreamRef);

        for (NiUInt32 uiDS=0; uiDS<kDataStreamSet.GetSize(); uiDS++)
        {
            NiDataStream* pkDS = kDataStreamSet.GetAt(uiDS);
            if (pkDS == pkStreamRef->GetDataStream())
            {
                m_kStreamRefs.Add(*pkStreamRef);
                break;
            }
        }
    }

    // require at least one streamref
    if (m_kStreamRefs.GetSize() < 1)
        return false;

    return true;
}
//--------------------------------------------------------------------------
bool NiInterleaver::Finalize(NiMeshErrorInterface*)
{
    // Empty remaining streamrefs into the finished stream refs
    for (NiUInt32 i = 0; i < m_kStreamRefs.GetSize(); i++)
    {
        // Extra work, but ensures needed streamRefs are not deleted
        NiDataStreamRef& kStreamRef = m_kStreamRefs.GetAt(i);
        m_kFinishedStreamRefs.Add(kStreamRef);
    }
    m_kStreamRefs.RemoveAll();

    // Release every data stream on every mesh in the set
    NiToolPipelineCloneHelper::CloneSetPtr spCloneSet = m_pkCloneSet;
    m_pkCloneHelper->ReleaseAllStreamRefs(spCloneSet);

    // Place final streams in the stream group
    for (NiUInt32 i = 0; i < m_kFinishedStreamRefs.GetSize(); ++i)
    {
        NiDataStreamRef& kStreamRef = m_kFinishedStreamRefs.GetAt(i);
        m_pkCloneHelper->AddStreamRef(spCloneSet, &kStreamRef);
    }

    m_kFinishedStreamRefs.RemoveAll();

    return true;
}
//--------------------------------------------------------------------------
bool NiInterleaver::CreateAdjustedLayout(NiMeshErrorInterface* pkLog)
{
    m_kAdjustedLayout.RemoveAll();

    // Construct Share Group from defaut profile
    StreamLayoutSet& kStreamLayoutSet = m_pkProfile->GetStreamLayoutSet();
    NiUInt32 uiStreamCount = kStreamLayoutSet.GetSize();
    for (NiUInt32 i = 0; i < uiStreamCount; i++)
    {
        // Update elements with stream ref and element ref
        StreamLayout& kStreamLayout = kStreamLayoutSet.GetAt(i);

        // Goal is for this new layout set to have eactly one layout,
        // however, more layouts may be added to the set if the elements
        // are not strict and splits are required.
        StreamLayoutSet kNewLayoutSet;

        NiUInt32 uiEntryCount = kStreamLayout.GetSize();

        // Strict-Interleaved Elements are handled first to ensure that their
        // precedence is observed.
        for (NiUInt32 j = 0; j < uiEntryCount; ++j)
        {
            // Skip entries that already have valid streams
            Element& kElement = kStreamLayout.GetAt(j);

            if (!kElement.m_bStrictInterleave)
                continue;

            // Find the stream and element
            NiDataStreamRef* pkRef;
            NiDataStreamElement kElem;
            if (m_pkMesh->FindStreamRefAndElementBySemantic(
                kElement.m_kSemantic, kElement.m_uiSemanticIndex,
                NiDataStreamElement::F_UNKNOWN, pkRef, kElem))
            {
                NiDataStream* pkDS = pkRef->GetDataStream();
                NI_UNUSED_ARG(pkDS);
                NIASSERT(pkDS);

                kElement.m_kStreamRef = *pkRef;
                kElement.m_kElement = kElem;
                kElement.m_uiElementIdx = pkRef->FindElementDescIndex(
                    kElement.m_kSemantic, kElement.m_uiSemanticIndex);
                NIASSERT(kElement.m_kElement.IsValid());

                int iFind = m_kStreamRefs.Find(kElement.m_kStreamRef);
                NIASSERT(iFind != -1);
                m_kStreamRefs.RemoveAt(iFind);
                if (PlaceStrictIntoLayout(kNewLayoutSet, kElement) == false)
                {
                    if (pkLog)
                    {
                        pkLog->ReportError(NiMPPMessages::
                            ERROR_STRICTINTERLEAVE_WITH_2ARGS,
                        kElement.m_kSemantic, kElement.m_uiSemanticIndex);
                    }
                    return false;
                }
            }
            else
            {
                if (pkLog)
                {
                    pkLog->ReportError(NiMPPMessages::
                        ERROR_INTERLEAVE_MISSING_SEMANTIC_2ARGS,
                        kElement.m_kSemantic, kElement.m_uiSemanticIndex);
                }
                return false;
            }
        }

        // Non-Strict-Interleave Elements are handled after strict ones.
        for (NiUInt32 j = 0; j < uiEntryCount; ++j)
        {
            // Skip entries that already have valid streams
            Element& kElement = kStreamLayout.GetAt(j);

            if (kElement.m_bStrictInterleave)
                continue;

            // Find the stream and element
            NiDataStreamRef* pkRef;
            NiDataStreamElement kElem;
            if (m_pkMesh->FindStreamRefAndElementBySemantic(
                kElement.m_kSemantic, kElement.m_uiSemanticIndex,
                NiDataStreamElement::F_UNKNOWN, pkRef, kElem))
            {
                NiDataStream* pkDS = pkRef->GetDataStream();
                NI_UNUSED_ARG(pkDS);
                NIASSERT(pkDS);

                kElement.m_kStreamRef = *pkRef;
                kElement.m_kElement = kElem;
                kElement.m_uiElementIdx = pkRef->FindElementDescIndex(
                    kElement.m_kSemantic, kElement.m_uiSemanticIndex);
                NIASSERT(kElement.m_kElement.IsValid());

                int iFind = m_kStreamRefs.Find(kElement.m_kStreamRef);
                NIASSERT(iFind != -1);
                m_kStreamRefs.RemoveAt(iFind);
                PlaceNonStrictIntoLayout(kNewLayoutSet, kElement);
            }
            else
            {
                if (pkLog)
                {
                    pkLog->ReportError(NiMPPMessages::
                        ERROR_INTERLEAVE_MISSING_SEMANTIC_2ARGS,
                        kElement.m_kSemantic, kElement.m_uiSemanticIndex);
                }
                return false;
            }
        }

        NiUInt32 uiNewSetCount = kNewLayoutSet.GetSize();

        for (NiUInt32 j=0; j < uiNewSetCount; j++)
        {
            // Transfer each layout to the adjustedlayoutset
            StreamLayout& kNewLayout = kNewLayoutSet.GetAt(j);
            m_kAdjustedLayout.Add(kNewLayout);
        }
    }

    return true;
}
//--------------------------------------------------------------------------
bool NiInterleaver::Interleave(NiMeshErrorInterface* pkLog)
{
    // Adjustments to layout may be allowed if the elements allow for it.
    if (CreateAdjustedLayout(pkLog) == false)
    {
        m_kAdjustedLayout.RemoveAll();
        return false;
    }

    // Interleave the streams with the packing definition
    // specified by the calling application
    bool bResult = InterleaveStreams();
    m_kAdjustedLayout.RemoveAll();
    return bResult;
}
//--------------------------------------------------------------------------
bool NiInterleaver::InterleaveStreams()
{
    NiUInt32 uiStreamCount = m_kAdjustedLayout.GetSize();
    for (NiUInt32 i = 0; i < uiStreamCount; i++)
    {
        StreamLayout& kStreamLayout = m_kAdjustedLayout.GetAt(i);

        // For the prototype, the first elements data stream us used.
        NiDataStreamRef& kProtoStreamRef =
            kStreamLayout.GetAt(0).m_kStreamRef;
        NiDataStreamPtr spProtoDS = kProtoStreamRef.GetDataStream();

        // Create output stream reference
        NiDataStreamRef kPackedStreamRef = CreateStreamRef(kProtoStreamRef);
        NIASSERT(kPackedStreamRef.IsValid());

        // Build element information for the stream. If returned
        // stride is zero then there was no data so early out.
        NiDataStreamElementSet kElementSet;
        if (BuildElements(kStreamLayout, kElementSet) == 0)
        {
            return false;
        }

        // Allocate packed data stream and add to stream ref
        NiDataStreamPtr spPackedDataStream = NiDataStream::CreateDataStream(
            kElementSet, spProtoDS->GetTotalCount(),
            spProtoDS->GetAccessMask(), spProtoDS->GetUsage());
        NIASSERT(spPackedDataStream);
        
        // Copy cloning behavior
        spPackedDataStream->SetCloningBehavior(
            spProtoDS->GetCloningBehavior());

        NiUInt32 uiProtoRegions = spProtoDS->GetRegionCount();
        for(NiUInt32 uiRegion = 0; uiRegion < uiProtoRegions; uiRegion++)
            spPackedDataStream->AddRegion(spProtoDS->GetRegion(uiRegion));

        kPackedStreamRef.SetDataStream(spPackedDataStream);

        SetElements(kStreamLayout, kPackedStreamRef);

        // Copy data from source stream to destination stream
        CopyStreamData(kStreamLayout, kPackedStreamRef, spPackedDataStream);

        // Add this stream to the array of packed streams
        m_kFinishedStreamRefs.Add(kPackedStreamRef);
    }
    return true;
}
//--------------------------------------------------------------------------
bool NiInterleaver::DeInterleave(NiToolPipelineCloneHelper::CloneSetPtr&
    kCloneSet, NiToolPipelineCloneHelper& kCloneHelper,
    NiMeshErrorInterface* pkLog)
{

    NiTPrimitiveSet<NiDataStream*> kDataStreamSet;
    kCloneHelper.GetDataStreams(kDataStreamSet, kCloneSet);

    bool bFoundInterleavedStreams = false;
    for(NiUInt32 ui=0; ui<kDataStreamSet.GetSize(); ui++)
    {
        NiDataStream* pkDS = kDataStreamSet.GetAt(ui);
        NiUInt32 uiElementCount = pkDS->GetElementDescCount();
        if (uiElementCount > 1)
        {
            bFoundInterleavedStreams = true;
            break;
        }
    }

    if (bFoundInterleavedStreams == false)
        return true;

    // Retrieved StreamRefs that are interleaved within this cloneset 
    NiMesh* pkMesh0 = kCloneSet->GetAt(0);
    NIASSERT(pkMesh0);
    NiTPrimitiveSet<NiDataStreamRef*> kInterleavedSRs;
    for(NiUInt32 ui=0; ui<kDataStreamSet.GetSize(); ui++)
    {
        NiDataStream* pkDS = kDataStreamSet.GetAt(ui);
        NiUInt32 uiElementCount = pkDS->GetElementDescCount();
        if (uiElementCount == 1)
            continue;

        // Match to streamRef's that use the datastream
        NiUInt32 uiStreamRefCnt = pkMesh0->GetStreamRefCount();
        for(NiUInt32 uiSR=0; uiSR<uiStreamRefCnt; uiSR++)
        {
            NiDataStreamRef* pkSR = pkMesh0->GetStreamRefAt(uiSR);
            if (pkSR->GetDataStream() == pkDS)
                kInterleavedSRs.Add(pkSR);
        }
    }

    NiUInt32 uiInterleavedSRs = kInterleavedSRs.GetSize();

    if (pkLog)
    {
        char acBuf[1024] = "";
        for(NiUInt32 uiSR=0; uiSR<uiInterleavedSRs; uiSR++)
        {
            NiStrcat(&acBuf[0], sizeof(acBuf),"    { ");

            NiDataStreamRef* pkSR = kInterleavedSRs.GetAt(uiSR);
            NiUInt32 uiElemCnt = pkSR->GetElementDescCount();
            for(NiUInt32 uiElem=0; uiElem < uiElemCnt; uiElem++)
            {
                char acBuf2[256];

                if (uiElem < uiElemCnt - 1)
                {
                    NiSprintf(&acBuf2[0], sizeof(acBuf2),"%s:%d, ",
                        pkSR->GetSemanticNameAt(uiElem),
                        pkSR->GetSemanticIndexAt(uiElem));
                }
                else
                {
                    NiSprintf(&acBuf2[0], sizeof(acBuf2),"%s:%d",
                        pkSR->GetSemanticNameAt(uiElem),
                        pkSR->GetSemanticIndexAt(uiElem));
                }
                NiStrcat(&acBuf[0], sizeof(acBuf), &acBuf2[0]);
            }
            NiStrcat(&acBuf[0], sizeof(acBuf)," }\n");
        }

        // We should not report this warning. The person who did this does not
        // care that we have to de-interleave to re-interleave again. If this
        // warning is activated it will hit every time the mesh instancing
        // plug-in is run.
        //pkLog->ReportWarning(
        //    NiMPPMessages::WARNING_DEINTERLEAVING_STREAMS_1ARG, acBuf);
    }

    NiTObjectSet<NiDataStreamRef> kDeinterleavedSRs;
    for(NiUInt32 uiSR=0; uiSR<uiInterleavedSRs; uiSR++)
    {
        NiDataStreamRef* pkDSR = kInterleavedSRs.GetAt(uiSR);
        NIASSERT(pkDSR->IsValid());

        NiDataStream* pkDS = pkDSR->GetDataStream();
        NIASSERT(pkDS);

        NiUInt32 uiElementDescCnt = pkDS->GetElementDescCount();
        for(NiUInt32 uiElemIndex=0; uiElemIndex<uiElementDescCnt;
            uiElemIndex++)
        {
            // Get the semantic element
            const NiDataStreamElement& kElement = pkDSR->GetElementDescAt(
                uiElemIndex);


            // Build a new stream based on this single element
            // Create the stream using single element and original params
            NiDataStreamElementSet kElements;
            kElements.AddElement(kElement.GetFormat());
            NiDataStream* pkNewDataStream = NiDataStream::CreateDataStream(
                kElements, 
                pkDS->GetTotalCount(), 
                pkDS->GetAccessMask(), 
                pkDS->GetUsage());
            NIASSERT(pkNewDataStream && "Allocation of NiDataStream failed!");

            // Build stream reference for mesh data
            NiDataStreamRef kNewStreamRef;
            kNewStreamRef.SetDataStream(pkNewDataStream);
            kNewStreamRef.BindSemanticToElementDescAt(0, 
                pkDSR->GetSemanticNameAt(uiElemIndex), 
                pkDSR->GetSemanticIndexAt(uiElemIndex));
            kNewStreamRef.SetPerInstance( pkDSR->IsPerInstance() );

            // Copy Regions
            NiUInt32 uiSubmeshRemapCount = pkDSR->GetSubmeshRemapCount();
            for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshRemapCount; 
                uiSubmesh++)
            {
                kNewStreamRef.BindRegionToSubmesh(uiSubmesh, 
                    pkDSR->GetRegionForSubmesh(uiSubmesh));
            }

            // Build the stream layout for this single stream
            StreamLayout kStreamLayout;
            Element kEntry;
            kEntry.m_kSemantic = pkDSR->GetSemanticNameAt(uiElemIndex);
            kEntry.m_uiSemanticIndex = pkDSR->GetSemanticIndexAt(uiElemIndex);
            kEntry.m_kStreamRef = *pkDSR;
            kEntry.m_kElement = kElement;
            kEntry.m_uiElementIdx = 0; //uiElemIndex;
            kStreamLayout.Add(kEntry);

            // Copy data to new stream
            CopyStreamData(kStreamLayout, kNewStreamRef, pkNewDataStream);

            // Add this new stream to the set
            NIASSERT(kNewStreamRef.IsValid());
            kDeinterleavedSRs.Add(kNewStreamRef);
        }
    }

    // Remove previous...
    for(NiUInt32 uiSR=0; uiSR<uiInterleavedSRs; uiSR++)
    {
        NiDataStreamRef* pkDSR = kInterleavedSRs.GetAt(uiSR);
        kCloneHelper.ReleaseStreamRef(pkDSR);
    }

    // Add back the de-interleaved streams
    for(NiUInt32 uiSR=0; uiSR<kDeinterleavedSRs.GetSize(); uiSR++)
    {
        NiDataStreamRef* pkSR = &kDeinterleavedSRs.GetAt(uiSR);
        bool bResult = 
            kCloneHelper.AddStreamRef(kCloneSet, pkSR);
        NI_UNUSED_ARG(bResult);
        NIASSERT(bResult);
    }

    return true;
}
//--------------------------------------------------------------------------
void NiInterleaver::Reset()
{
    // Resets all member data, most significant is the Processed clone sets.
    m_kAdjustedLayout.RemoveAll();
    m_pkMesh = NULL;
    m_pkProfile = NULL;
    m_pkCloneSet = NULL;
    m_pkCloneHelper = NULL;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Helper routine for debugging purposes
// Enabled PRINT_DATA_STREAM_ENABLED to use if needed
//--------------------------------------------------------------------------
#undef PRINT_DATA_STREAM_ENABLED
#if PRINT_DATA_STREAM_ENABLED
static void PrintDataStream(NiDataStreamRef& kStreamRef)
{
    NiString strLabel;
    for (NiUInt32 i = 0; i < kStreamRef.GetElementDescCount(); ++i)
    {
        strLabel += kStreamRef.GetSemanticNameAt(i);
        strLabel += kStreamRef.GetSemanticIndexAt(i);
        strLabel += ":";
    }
    strLabel += "\n";
    NILOG(strLabel);

    NiDataStreamPtr spDataStream = kStreamRef.GetDataStream();
    NiDataStreamLock kLockedSrc(spDataStream, 0, NiDataStream::LOCK_READ);
    CharIter kSrcIter = kLockedSrc.begin<char>();

    NiUInt32 uiStride = pkDataStream->GetStride();
    NiUInt32 uiLineCount = 4;

    // Iterate over data printing
    NiUInt32 uiCount = uiLineCount;
    for (CharIter kIter = kLockedSrc.begin<char>();
        kIter != kLockedSrc.end<char>();
        kIter++)
    {
        NiString strElement;
        char cByte[4];
        char* ip = &*kIter;
        for (NiUInt32 i = 0; i < uiStride; i++)
        {
            NiSprintf(cByte, 4, " %.2X", (unsigned int)*ip++);            
            strElement += cByte;
        }
        strElement += "    ";
        if (--uiCount == 0)
        {
            strElement += "\n";
            uiCount = uiLineCount;
        }
        NILOG(strElement);
        strElement = "";
    }
    NILOG("---------------------------------------------------------------\n");
}
#endif
