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

#include <NiDataStreamElement.h>
#include <NiDataStreamLock.h>
#include <NiToolDataStream.h>
#include "NiMeshProfileProcessor.h"
#include "NiMeshProfileOperations.h"
#include "NiComponentInput.h"
#include "NiComponentStreamInput.h"
#include "NiComponentLiteralInput.h"

//---------------------------------------------------------------------------
// NiMeshProfileOperations Helpers for MergeAndSwizzle
//---------------------------------------------------------------------------
template<typename T, NiUInt8 TComps>
void TMergeAndSwizzle(NiComponentInput** ppInputs,
    NiDataStream* pkDataStream, NiUInt32 uiCount)
{
    // 0 is offset, should be no offset sence only 1 element
    NiDataStreamLock kLock(pkDataStream, 0, NiDataStream::LOCK_TOOL_WRITE);

    NiTStridedRandomAccessIterator<T[TComps]> kIter =
        kLock.begin<T[TComps]>();

    // Lock all inputs
    for(NiUInt32 ui = 0; ui < TComps; ui++)
        ppInputs[ui]->LockInput();

    for(NiUInt32 uiItem=0; uiItem < uiCount; uiItem++)
    {
        for(NiUInt32 ui=0; ui < TComps; ui++)
        {
            T value;
            NIVERIFY(ppInputs[ui]->GetValue(uiItem, value));
            ((T*)(kIter[uiItem]))[ui] = value;
        }
    }

    // Unlock all inputs
    for(NiUInt32 ui = 0; ui < TComps; ui++)
        ppInputs[ui]->UnlockInput();

}
//---------------------------------------------------------------------------
template<typename T>
void TMergeAndSwizzle(NiComponentInput** ppInputs, NiUInt32 uiInputs,
    NiDataStream* pkDataStream, NiUInt32 uiCount)
{
    NIASSERT(uiInputs > 0 && uiInputs < 5);

    switch(uiInputs)
    {
        case 1:
            return TMergeAndSwizzle<T, 1>(ppInputs, pkDataStream, uiCount);
        case 2:
            return TMergeAndSwizzle<T, 2>(ppInputs, pkDataStream, uiCount);
        case 3:
            return TMergeAndSwizzle<T, 3>(ppInputs, pkDataStream, uiCount);
        case 4:
            return TMergeAndSwizzle<T, 4>(ppInputs, pkDataStream, uiCount);
        default:
            break;
    }
}
//---------------------------------------------------------------------------
NiDataStreamRef NiMeshProfileOperations::MergeAndSwizzle(
    NiFixedString& kNewSemantic, NiUInt8 uiNewIndex, 
    NiComponentInput* pkIn0, NiComponentInput* pkIn1, NiComponentInput* pkIn2,
    NiComponentInput* pkIn3)
{
    if (pkIn0 == NULL)
        return NiDataStreamRef();

    NiComponentInput* apInputs[4];
    apInputs[0] = pkIn0;
    apInputs[1] = pkIn1;
    apInputs[2] = pkIn2;
    apInputs[3] = pkIn3;

    // First, determine the actual number of inputs
    NiUInt32 uiInputs = 0;
    for(uiInputs = 0; uiInputs < 4; uiInputs++)
    {
        if (apInputs[uiInputs] == NULL)
            break;
    }

    // First, find a stream basis
    NiComponentInput::StreamBasis kBasis;
    bool bFoundBasis = false;
    for(NiUInt32 ui=0; ui<uiInputs; ui++)
    {
        if (apInputs[ui]->AssignBasis(kBasis))
        {
            bFoundBasis = true;
            break;
        }
    }

    // At least 1 stream input is required for a basis
    if (bFoundBasis == false)
        return NiDataStreamRef();

    // Ensure that all inputs are compatible with the basis
    for(NiUInt32 ui=0; ui<uiInputs; ui++)
    {
        if (apInputs[ui]->IsCompatible(kBasis) == false)
        {
            // Must be compatible with the basis
            return NiDataStreamRef();
        }
    }

    // Time to create the output stream
    NiDataStreamElement::Format eFormat = NiDataStreamElement::
        GetPredefinedFormat( kBasis.m_kElement.GetType(), (NiUInt8)uiInputs,
        kBasis.m_kElement.IsNormalized());

    NIASSERT(eFormat != NiDataStreamElement::F_UNKNOWN);
    if (eFormat == NiDataStreamElement::F_UNKNOWN)
        return NiDataStreamRef();

    // Create regionless data stream w/o initialization
    NiDataStreamPtr spDataStream = NiDataStream::CreateSingleElementDataStream(
        eFormat, kBasis.m_uiCount, kBasis.m_uiAccessMask, kBasis.m_eUsage,
        NULL, false, true);
    NIASSERT(spDataStream);

    spDataStream->SetCloningBehavior(kBasis.m_eCloningBehavior);

    // Create the regions as found on basis
    for(NiUInt32 uiRegion = 0; uiRegion < kBasis.m_kRegions.GetSize();
        uiRegion++)
    {
        // 1 is subtracted off since we stored an extra count to avoid "0"
        // being treated as empty.
        NiDataStream::Region kRegion = kBasis.m_kRegions.GetAt(uiRegion);
        spDataStream->AddRegion(kRegion);
    }

    // Create StreamRef with new semantic name and index
    NiDataStreamRef kStreamRef;
    kStreamRef.SetDataStream(spDataStream);
    kStreamRef.BindSemanticToElementDescAt(0, kNewSemantic, uiNewIndex);

    // It is assumed that each regionRef uniquely references a region and
    // is consecutive
    for(NiUInt32 uiSubmesh = 0; uiSubmesh < kBasis.m_uiNumSubmeshes;
        uiSubmesh++)
    {
        NiUInt32 uiRegion = kBasis.m_kRegionRefs.GetAt(uiSubmesh);
        kStreamRef.BindRegionToSubmesh(uiSubmesh, uiRegion);
    }

    // Set frequency and function
    kStreamRef.SetPerInstance(kBasis.m_bIsPerInstance);

    switch(kBasis.m_kElement.GetType())
    {
        case NiDataStreamElement::T_FLOAT32:
            TMergeAndSwizzle<float>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_FLOAT16:
            TMergeAndSwizzle<NiFloat16>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_UINT32:
            TMergeAndSwizzle<NiUInt32>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_UINT16:
            TMergeAndSwizzle<NiUInt16>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_UINT8:
            TMergeAndSwizzle<NiUInt8>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_INT32:
            TMergeAndSwizzle<NiInt32>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_INT16:
            TMergeAndSwizzle<NiInt16>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        case NiDataStreamElement::T_INT8:
            TMergeAndSwizzle<NiInt8>(apInputs, uiInputs, spDataStream,
                kBasis.m_uiCount);
            break;
        default:
            NIASSERT(!"Unknown type!");
            break;
    }

    return kStreamRef;
}
//---------------------------------------------------------------------------

