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
#include "NiTAdjustValue.h"

//---------------------------------------------------------------------------
// NiMeshProfileOperations Helpers for CastConvert
//---------------------------------------------------------------------------
template <typename TIn, typename TOut>
static bool TCastConvertNormalize(const TIn* paIn, TOut* paOut, 
    NiUInt32 uiCompByCount, double dSrcMin, double dSrcMax, double dDestMin, 
    double dDestMax)
{
    double dDestRange = dDestMax - dDestMin;
    double dSrcRange = dSrcMax - dSrcMin;

    for (NiUInt32 ui = 0; ui < uiCompByCount; ui++)
    {
        double dInVal = (double)(*paIn++);
        // Remap dInVal to dst range
        double dT = (dInVal - dSrcMin) / dSrcRange;
        *paOut++ = AdjustValue<TOut>(dDestMin + dDestRange * dT);
    }
    return true;
}
//---------------------------------------------------------------------------
template <typename TIn, typename TOut>
static bool TCastConvertTypeClamp(const TIn* paIn, TOut* paOut, 
    NiUInt32 uiCompByCount)
{
    for (NiUInt32 ui=0; ui<uiCompByCount; ui++)
    {
        double dInVal = (double)(*paIn++);
        *paOut++ = AdjustValue<TOut>(dInVal);
    }
    return true;
}
//---------------------------------------------------------------------------
template <typename TIn, typename TOut>
static bool TCastConvert(const TIn* paIn, TOut* paOut, 
    NiUInt32 uiCompByCount)
{
    for (NiUInt32 ui=0; ui<uiCompByCount; ui++)
    {
        // Extra float cast to avoid warning c4244 in case of NiFloat16
        *paOut++ = (TOut)((float)(*paIn++));
    }
    return true;
}
//---------------------------------------------------------------------------
template <typename TIn, typename TOut>
static bool TSmartCastConvert(const TIn* paIn, TOut* paOut, 
    NiUInt32 uiCompByCount, double dDestMin, double dDestMax)
{
    for (NiUInt32 ui=0; ui<uiCompByCount; ui++)
    {
        double dValue = (double)(*paIn);
        if (dValue < dDestMin || dValue > dDestMax)
        {
            return false;
        }

        // Extra float cast to avoid warning c4244 in case of NiFloat16
        *paOut++ = (TOut)((float)(*paIn++));
    }
    return true;
}
//---------------------------------------------------------------------------
template <typename TIn, typename TOut>
static bool TResolveConvertType(const TIn* paIn, TOut* paOut, 
    NiUInt32 uiCompByCount, NiMeshProfileOperations::CastConvertType
    eConvertType, double dSrcMin, double dSrcMax, double dDestMin, 
    double dDestMax)
{
    switch(eConvertType)
    {
        case NiMeshProfileOperations::CT_SMARTCLAMP:
            return TSmartCastConvert<TIn, TOut>(paIn, paOut,
                uiCompByCount, dDestMin, dDestMax);
        case NiMeshProfileOperations::CT_NORMALIZE:
            return TCastConvertNormalize<TIn, TOut>(paIn, paOut,
                uiCompByCount, dSrcMin, dSrcMax, dDestMin, dDestMax);
        case NiMeshProfileOperations::CT_TYPECLAMP:
            return TCastConvertTypeClamp<TIn, TOut>(paIn, paOut,
                uiCompByCount);
        case NiMeshProfileOperations::CT_BLINDCAST:
        default:
            return TCastConvert<TIn, TOut>(paIn, paOut, uiCompByCount);
    }
}
//---------------------------------------------------------------------------
template <typename TIn>
static bool TResolveSecondEnum(const TIn* paIn,
    void* paOut, NiDataStreamElement::Type eOutType,
    NiUInt32 uiNumComp, NiUInt32 uiNumOfItems,
    NiMeshProfileOperations::CastConvertType eConvertType, double dSrcMin,
    double dSrcMax, double dDestMin, double dDestMax)
{
    NiUInt32 uiCompByCount = uiNumComp * uiNumOfItems;

    switch(eOutType)
    {
        case NiDataStreamElement::T_FLOAT32:
             return TResolveConvertType<TIn, float>(paIn, (float*)paOut, 
                 uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                 dDestMax);
        case NiDataStreamElement::T_FLOAT16:
             return TResolveConvertType<TIn, NiFloat16>(paIn,(NiFloat16*)paOut,
                 uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                 dDestMax);
        case NiDataStreamElement::T_UINT32:
            return TResolveConvertType<TIn, NiUInt32>(paIn, (NiUInt32*)paOut, 
                uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                dDestMax);
        case NiDataStreamElement::T_UINT16:
            return TResolveConvertType<TIn, NiUInt16>(paIn, (NiUInt16*)paOut, 
                uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                dDestMax);
        case NiDataStreamElement::T_UINT8:
            return TResolveConvertType<TIn, NiUInt8>(paIn, (NiUInt8*)paOut, 
                uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                dDestMax);
        case NiDataStreamElement::T_INT32:
            return TResolveConvertType<TIn, NiInt32>(paIn, (NiInt32*)paOut, 
                uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                dDestMax);
        case NiDataStreamElement::T_INT16:
            return TResolveConvertType<TIn, NiInt16>(paIn, (NiInt16*)paOut, 
                uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                dDestMax);
        case NiDataStreamElement::T_INT8:
            return TResolveConvertType<TIn, NiInt8>(paIn, (NiInt8*)paOut, 
                uiCompByCount, eConvertType, dSrcMin, dSrcMax, dDestMin, 
                dDestMax);
        default:
            return false;
    }
}
//---------------------------------------------------------------------------
static bool ResolveEnumAndRange(NiDataStreamElement::Type eSrcType, 
    NiDataStreamElement::Type eDestType, const void* pvIn, void* pvOut, 
    NiUInt32 uiNumComponents, NiUInt32 uiNumItems, 
    NiMeshProfileOperations::CastConvertType eConvertType)
{
    double dSrcMin = 0;
    double dSrcMax = 0;
    double dDestMin = 0;
    double dDestMax = 0;

    // while we still have the enumerations, figure out the range of floating
    // point mapping if we are normalizing
    if (EnumIsFloatingPoint(eSrcType))
    {
        if (EnumIsSigned(eDestType))
        {
            dSrcMin = -1.0;
            dSrcMax = 1.0;
        }
        else
        {
            dSrcMin = 0.0;
            dSrcMax = 1.0;
        }
    }
    else
    {
        GetEnumTypeRange(eSrcType, dSrcMin, dSrcMax);
    }

    if (EnumIsFloatingPoint(eDestType))
    {
        if (EnumIsSigned(eSrcType))
        {
            dDestMin = -1.0;
            dDestMax = 1.0;
        }
        else
        {
            dDestMin = 0.0;
            dDestMax = 1.0;
        }
    }
    else
    {
        GetEnumTypeRange(eDestType, dDestMin, dDestMax);
    }

    switch(eSrcType)
    {
        case NiDataStreamElement::T_FLOAT32:
            return TResolveSecondEnum<float>((float*)pvIn, pvOut, eDestType,
                uiNumComponents, uiNumItems, eConvertType, dSrcMin, dSrcMax, 
                dDestMin, dDestMax);
        case NiDataStreamElement::T_FLOAT16:
            return TResolveSecondEnum<NiFloat16>((NiFloat16*)pvIn, pvOut,
                eDestType, uiNumComponents, uiNumItems, eConvertType, dSrcMin,
                dSrcMax, dDestMin, dDestMax);
        case NiDataStreamElement::T_UINT32:
            return TResolveSecondEnum<NiUInt32>((NiUInt32*)pvIn, pvOut, 
                eDestType, uiNumComponents, uiNumItems, eConvertType, dSrcMin,
                dSrcMax, dDestMin, dDestMax);
        case NiDataStreamElement::T_UINT16:
            return TResolveSecondEnum<NiUInt16>((NiUInt16*)pvIn, pvOut, 
                eDestType, uiNumComponents, uiNumItems, eConvertType, dSrcMin,
                dSrcMax, dDestMin, dDestMax);
        case NiDataStreamElement::T_UINT8:
            return TResolveSecondEnum<NiUInt8>((NiUInt8*)pvIn, pvOut, 
                eDestType, uiNumComponents, uiNumItems, eConvertType, dSrcMin,
                dSrcMax, dDestMin, dDestMax);
        case NiDataStreamElement::T_INT32:
            return TResolveSecondEnum<NiInt32>((NiInt32*)pvIn, pvOut, 
                eDestType, uiNumComponents, uiNumItems, eConvertType, dSrcMin, 
                dSrcMax, dDestMin, dDestMax);
        case NiDataStreamElement::T_INT16:
            return TResolveSecondEnum<NiInt16>((NiInt16*)pvIn, pvOut, 
                eDestType, uiNumComponents, uiNumItems, eConvertType, dSrcMin,
                dSrcMax, dDestMin, dDestMax);
        case NiDataStreamElement::T_INT8:
            return TResolveSecondEnum<NiInt8>((NiInt8*)pvIn, pvOut, eDestType,
                uiNumComponents, uiNumItems, eConvertType, dSrcMin, dSrcMax, 
                dDestMin, dDestMax);
        default:
            NIASSERT(!"UNKNOWN Type");
            return false;
    }
}
//---------------------------------------------------------------------------
NiDataStreamRef NiMeshProfileOperations::CastConvert(
    const NiDataStreamRef& kSrcRef, NiDataStreamElement::Type eDstType, 
    CastConvertType eConvertType)
{
    if (!IsValidStreamRef(kSrcRef))
        return NiDataStreamRef();

    NiDataStream* pkSrcDS = (NiDataStream*)kSrcRef.GetDataStream();
    NiUInt32 uiCount = pkSrcDS->GetTotalCount();

    NiDataStreamElement kSrcElement = pkSrcDS->GetElementDescAt(0);
    NiDataStreamElement::Format eSrcFormat = kSrcElement.GetFormat();

    // Only the "standard" predefined formats may be cast converted
    if (NiDataStreamElement::IsSwizzled(eSrcFormat) == true)
        return NiDataStreamRef();
    if (NiDataStreamElement::IsPacked(eSrcFormat) == true)
        return NiDataStreamRef();

    NiDataStreamElement::Type eSrcType = kSrcElement.GetType(eSrcFormat);
    NiUInt8 uiSrcCompCount = kSrcElement.GetComponentCount(eSrcFormat);

    NiDataStreamElement::Format eDstFormat =
        NiDataStreamElement::F_UNKNOWN;

    if (eConvertType == CT_NORMALIZE)
        eDstFormat = NiDataStreamElement::GetPredefinedFormat(eDstType,
            uiSrcCompCount, true);
    else
        eDstFormat = NiDataStreamElement::GetPredefinedFormat(eDstType,
            uiSrcCompCount, false);

    if (eDstFormat == NiDataStreamElement::F_UNKNOWN)
    {
        NIASSERT(!"Can not determing format conversion");
        return NiDataStreamRef();
    }

    NiDataStreamRef kNewStreamRef = kSrcRef;

    // Replace the streamref with one of the new format
    NiDataStreamPtr spkNewDS =  NiDataStream::CreateSingleElementDataStream(
        eDstFormat, uiCount, pkSrcDS->GetAccessMask(), pkSrcDS->GetUsage(),
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

    // the new data gets filled in during this function call
    if (ResolveEnumAndRange(eSrcType, eDstType, paIn, paOut, uiSrcCompCount,
        uiCount, eConvertType))
    {
        kNewStreamRef.SetDataStream(spkNewDS);
    }

    spkNewDS->Unlock(
        NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
    pkSrcDS->Unlock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

    return kNewStreamRef;
}
//---------------------------------------------------------------------------
