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
#include "NiBitPackInfo.h"

NiDataStreamRef NiMeshProfileOperations::ComponentPack(
    NiDataStreamRef& kSrcRef, bool bSigned, bool bNormalized,
    bool bLastFieldIsLiteral, NiUInt8 uiF0, NiUInt8 uiF1, NiUInt8 uiF2,
    NiUInt8 uiF3)
{
    if (!IsValidStreamRef(kSrcRef))
        return NiDataStreamRef();

    NiDataStream* pkStream = kSrcRef.GetDataStream();
    NiDataStreamElement kElement = pkStream->GetElementDescAt(0);

    if (kElement.IsCustom())
        return NiDataStreamRef(); // unsucessful - stream can not be custom

    if (kElement.IsPacked())
        return NiDataStreamRef(); // unsucessful - stream can not be packed

    if (kElement.IsSwizzled())
        return NiDataStreamRef(); // unsucessful - stream can not be swizzled

    NiDataStreamElement::Format ePackFormat;
    ePackFormat = NiDataStreamElement::GetPredefinedPackedFormat(bNormalized,
        bSigned, bLastFieldIsLiteral, uiF0, uiF1, uiF2, uiF3);

    if (ePackFormat == NiDataStreamElement::F_UNKNOWN)
        return NiDataStreamRef(); // unsuccessful - no predefined type

    NiUInt32 uiFieldCount = NiDataStreamElement::GetFieldCount(ePackFormat);
    NiUInt32 uiSrcCompCount = kElement.GetComponentCount();

    // Number of components must be less than or equal to the number of
    // fields. If the components are less than the number of fieldss, then
    // extra fields will be packed with zero or one depending on the value
    // of m_bFillMissingDataWithOne.
    if (uiSrcCompCount > uiFieldCount)
        return NiDataStreamRef(); // unsucessful - components must be <= fields

    // Create the packed data stream
    // Replace the streamref with one of the new format
    NiDataStreamRef kNewStreamRef = kSrcRef;
    NiDataStream* pkSrcDS = kSrcRef.GetDataStream();
    NiUInt32 uiCount = pkSrcDS->GetTotalCount();
    NiDataStreamPtr spkNewDS =  NiDataStream::CreateSingleElementDataStream(
        ePackFormat, uiCount, pkSrcDS->GetAccessMask(), pkSrcDS->GetUsage(),
        NULL, false, true);
    NIASSERT(spkNewDS);

    spkNewDS->SetCloningBehavior(pkSrcDS->GetCloningBehavior());

    // Add regions back
    NiUInt32 uiSrcRegionCnt = pkSrcDS->GetRegionCount();
    for(NiUInt32 uiRegion=0; uiRegion<uiSrcRegionCnt; uiRegion++)
    {
        NiDataStream::Region kRegion = pkSrcDS->GetRegion(uiRegion);
        spkNewDS->AddRegion(kRegion);
    }

    void* paOut = spkNewDS->Lock(
        NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
    const void* paIn = pkSrcDS->Lock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);
    NIASSERT(paIn);

    // At present, we only support 4-byte types.
    NiBitPackInfo* pkPackInfo = NULL;
    if (uiFieldCount == 2)
        pkPackInfo = NiExternalNew NiTBitPackInfo<4, 2>;
    else if (uiFieldCount == 3)
        pkPackInfo = NiExternalNew NiTBitPackInfo<4, 3>;
    else if (uiFieldCount == 4)
        pkPackInfo = NiExternalNew NiTBitPackInfo<4, 4>;
    NIASSERT(pkPackInfo);

    for (NiUInt32 ui=0; ui < uiFieldCount; ui++)
    {
        if (ui == uiFieldCount-1 && bLastFieldIsLiteral)
            pkPackInfo->FillNextField(NiDataStreamElement::GetFieldBits(
                ePackFormat, (NiUInt8)ui), bSigned, bNormalized, true);
        else
            pkPackInfo->FillNextField(NiDataStreamElement::GetFieldBits(
                ePackFormat, (NiUInt8)ui), bSigned, bNormalized);
    }

    if (pkPackInfo->PackStream(kElement.GetType(), paIn, uiSrcCompCount,
        uiCount, paOut, spkNewDS->GetSize()))
    {
        kNewStreamRef.SetDataStream(spkNewDS);
    }

    spkNewDS->Unlock(
        NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
    pkSrcDS->Unlock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

    NiDelete pkPackInfo;

    return kNewStreamRef;
}