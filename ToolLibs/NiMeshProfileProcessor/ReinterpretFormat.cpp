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
#include <NiToolDataStream.h>
#include "NiMeshProfileOperations.h"

//---------------------------------------------------------------------------
NiDataStreamRef NiMeshProfileOperations::ReinterpretFormat(
    const NiDataStreamRef& kSrcRef,
    NiDataStreamElement::Format eDstFormat)
{
    if (!IsValidStreamRef(kSrcRef))
        return NiDataStreamRef();

    NiDataStream* pkSrcDS = (NiDataStream*)kSrcRef.GetDataStream();
    NIASSERT(pkSrcDS);

    NiDataStreamElement::GetType(eDstFormat);
    NiUInt8 uiDstCompCount = NiDataStreamElement::
        GetComponentCount(eDstFormat);
    NiUInt32 uiDstCompSize =
        NiDataStreamElement::GetComponentSize(eDstFormat);
    NiUInt32 uiDstFormatSize = (uiDstCompCount * uiDstCompSize);

    NIASSERT(pkSrcDS->GetElementDescCount() == 1);
    NiDataStreamElement kSrcElement = pkSrcDS->GetElementDescAt(0);
    NiDataStreamElement::Format eSrcFormat = kSrcElement.GetFormat();
    kSrcElement.GetType(eSrcFormat);
    NiUInt8 uiSrcCompCount =
        NiDataStreamElement::GetComponentCount(eSrcFormat);
    NiUInt32 uiSrcCompSize =
        NiDataStreamElement::GetComponentSize(eSrcFormat);
    NiUInt32 uiSrcFormatSize = (uiSrcCompCount * uiSrcCompSize);

    NiUInt32 uiRegionCnt = pkSrcDS->GetRegionCount();

    NiTPrimitiveArray<NiUInt32> akRegionStart;
    NiTPrimitiveArray<NiUInt32> akRegionRange;

    for (NiUInt32 ui=0; ui<uiRegionCnt; ui++)
    {
        const NiDataStream::Region& kRegion = pkSrcDS->GetRegion(ui);
        NiUInt32 uiItemCnt = kRegion.GetRange();

        NiUInt32 uiSrcRegionBytes = uiItemCnt * uiSrcFormatSize;

        // The number of bytes for the region must be evenly divisable
        // by the size of (uiDstCompCount * uiDstCompSize) and if not
        // we must fail.
        if (uiSrcRegionBytes % uiDstFormatSize != 0)
            return NiDataStreamRef();

        // otherwise, store the new range...
        NiUInt32 uiStartIndex = (kRegion.GetStartIndex() * uiSrcFormatSize) /
            uiDstFormatSize;
        NiUInt32 uiRange = uiSrcRegionBytes / uiDstFormatSize;

        // Add 1 to avoid array T(0) side affect
        akRegionStart.Add(uiStartIndex + 1);
        akRegionRange.Add(uiRange + 1);
    }

    // Must have at least one region.
    if (akRegionStart.GetSize() < 1)
        return NiDataStreamRef();

    // Do final check of entire datastream
    NiUInt32 uiSrcBytes = pkSrcDS->GetSize();

    if (uiSrcBytes % uiDstCompCount != 0)
        return NiDataStreamRef();

    NiUInt32 uiDstCount = uiSrcBytes / uiDstCompCount;

    NiDataStreamRef kNewRef = kSrcRef;

    // Replace the streamref with one of the new format
    const void* paIn = pkSrcDS->Lock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NIASSERT(paIn);

    NiDataStream* pkNewDS = NiDataStream::CreateSingleElementDataStream(
        eDstFormat, uiDstCount, pkSrcDS->GetAccessMask(), pkSrcDS->GetUsage(),
        paIn, true, true);
    pkSrcDS->Unlock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    kNewRef.SetDataStream(pkNewDS);

    pkNewDS->SetCloningBehavior(pkSrcDS->GetCloningBehavior());

    // Early exit in the case where there is only 1 region
    if (akRegionStart.GetSize() == 1)
        return kNewRef;

    NIASSERT(akRegionStart.GetSize() == akRegionRange.GetSize());
    pkNewDS->RemoveAllRegions();
    for(NiUInt32 ui=0; ui<akRegionStart.GetSize(); ui++)
    {
        NiUInt32 uiStart = akRegionStart.GetAt(ui) - 1;
        NiUInt32 uiRange = akRegionRange.GetAt(ui) - 1;

        NiDataStream::Region kRegion(uiStart, uiRange);
        pkNewDS->AddRegion(kRegion);
    }

    return kNewRef;
}
//---------------------------------------------------------------------------
