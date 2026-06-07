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
#include "NiMeshProfileOperations.h"
#include "NiTAdjustValue.h"
#include "NiBitPackInfo.h"

//---------------------------------------------------------------------------
// NiMeshProfileOperations Helpers for Remap
//---------------------------------------------------------------------------
template <typename TType, NiUInt8 TNumComponents>
static void ApplyRangeRemap(NiDataStreamLock& kLock,
    double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
    NiInt32 iComponent)
{
    // Only 1-4 component size expected.
    NIASSERT(TNumComponents > 0 && TNumComponents < 5);
    NIASSERT((iComponent >= 0 && iComponent < 4) || iComponent == -1);

    NIASSERT(iComponent < (NiInt32)TNumComponents);
    if (iComponent >= (NiInt32)TNumComponents)
        return;

    NIASSERT((dSrcBegin != dSrcEnd && dDstBegin != dDstEnd) ||
        (dSrcBegin == dSrcEnd && dDstBegin == dDstEnd));

    // Treat as a parametric equation: (P2 - (P2 - P1) * t)
    //    where t is the src range mapped to a value between 0.0f and 1.0f
    double dP2 = dDstBegin;
    double dP1 = dDstEnd;
    double dP2_minus_dP1 = dP2 - dP1;
    double dSrcRange = dSrcEnd - dSrcBegin;

    NiTStridedRandomAccessIterator<TType[TNumComponents]> kIter = 
        kLock.begin<TType[TNumComponents]>();
    NiTStridedRandomAccessIterator<TType[TNumComponents]> kIterEnd =
        kLock.end<TType[TNumComponents]>();

    if (dSrcRange != 0)
    {
        // The typical path where dSrcRange is non-zero

        if (iComponent == -1)
        {
            while(kIter != kIterEnd)
            {
                TType* kElem = (TType*)(*kIter);
                for (NiUInt32 uiComp = 0; uiComp < TNumComponents;
                    uiComp++)
                {
                    // Normalize value
                    double dT = (kElem[uiComp] - dSrcBegin) / dSrcRange;
                    kElem[uiComp] = AdjustValue<TType>(
                        dP2 - dP2_minus_dP1 * dT);
                }

                ++kIter;
            }
        }
        else
        {
            while(kIter != kIterEnd)
            {
                TType* kElem = (TType*)(*kIter);
                // Normalize value
                double dT = (kElem[iComponent] - dSrcBegin) / dSrcRange;
                kElem[iComponent] = AdjustValue<TType>(
                    dP2 - dP2_minus_dP1 * dT);

                ++kIter;
            }
        }
    }
    else
    {
        // This is an atypical path where a single value is being
        // replaced.
        if (iComponent == -1)
        {
            while(kIter != kIterEnd)
            {
                TType* kElem = (TType*)(*kIter);
                for (NiUInt32 uiComp = 0; uiComp < TNumComponents;
                    uiComp++)
                {
                    if ((TType)kElem[uiComp] != dSrcBegin)
                        continue;

                    kElem[uiComp] = AdjustValue<TType>(dDstBegin);
                }

                ++kIter;
            }
        }
        else
        {
            while(kIter != kIterEnd)
            {
                TType* kElem = (TType*)(*kIter);
                
                if ((TType)kElem[iComponent] == dSrcBegin)
                    kElem[iComponent] = AdjustValue<TType>(dDstBegin);

                ++kIter;
            }
        }
    }
}
//---------------------------------------------------------------------------
template <typename TType, NiUInt8 TNumComponents>
static void ApplyRangeRemap(NiDataStreamLock& kLock,
    double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
    NiInt32 iComponent, NiBitPackInfo* pkInfo)
{
    NIASSERT(sizeof(TType) == pkInfo->GetSize());
    NIASSERT(TNumComponents == 1);
    NIASSERT((iComponent >= 0 && iComponent < pkInfo->GetNumFields())
        || iComponent == -1);

    if (iComponent >= (NiInt32)(pkInfo->GetNumFields()))
        return;

    NIASSERT((dSrcBegin != dSrcEnd && dDstBegin != dDstEnd) ||
        (dSrcBegin == dSrcEnd && dDstBegin == dDstEnd));

    // Treat as a parametric equation: (P2 - (P2 - P1) * t)
    //    where t is the src range mapped to a value between 0.0f and 1.0f
    double dP2 = dDstBegin;
    double dP1 = dDstEnd;
    double dP2_minus_dP1 = dP2 - dP1;
    double dSrcRange = dSrcEnd - dSrcBegin;

    NIASSERT(kLock.IsLocked());
    NiTStridedRandomAccessIterator<NiUInt32> kIter =
        kLock.begin<NiUInt32>();
    NiTStridedRandomAccessIterator<NiUInt32> kIterEnd =
        kLock.end<NiUInt32>();

    if (dSrcRange != 0)
    {
        // The typical path where dSrcRange is non-zero

        if (iComponent == -1)
        {
            while(kIter != kIterEnd)
            {
                TType& kPackedElem = (TType&)(*kIter);
                for (NiUInt32 uiComp = 0; uiComp < pkInfo->GetNumFields();
                    uiComp++)
                {
                    NiInt32 iUnpackedComp = pkInfo->GetUnpackedValue(
                        (NiUInt8)uiComp, kPackedElem); 
                    double dT = (iUnpackedComp - dSrcBegin) / dSrcRange;
                    double dValue = dP2 - dP2_minus_dP1 * dT;
                    kPackedElem = pkInfo->PackValueAsIs((NiUInt8)uiComp,dValue,
                        kPackedElem);
                }

                ++kIter;
            }
        }
        else
        {
            while(kIter != kIterEnd)
            {
                TType& kPackedElem = (TType&)(*kIter);
                NiInt32 iUnpackedComp = pkInfo->GetUnpackedValue(
                    (NiUInt8)iComponent, kPackedElem); 
                double dT = (iUnpackedComp - dSrcBegin) / dSrcRange;
                double dValue = dP2 - dP2_minus_dP1 * dT;
                kPackedElem = pkInfo->PackValueAsIs((NiUInt8)iComponent, 
                    dValue, kPackedElem);
                ++kIter;
            }
        }
    }
    else
    {
        // This is an atypical path where a single value is being
        // replaced.
        if (iComponent == -1)
        {
            while(kIter != kIterEnd)
            {
                TType& kPackedElem = (TType&)(*kIter);
                for (NiUInt32 uiComp = 0; uiComp < pkInfo->GetNumFields();
                    uiComp++)
                {
                    NiInt32 iUnpackedComp = pkInfo->GetUnpackedValue(
                        (NiUInt8)uiComp, kPackedElem); 

                    if (iUnpackedComp != dSrcBegin)
                        continue;

                    kPackedElem = pkInfo->PackValueAsIs((NiUInt8)uiComp,
                        dDstBegin, kPackedElem);
                }

                ++kIter;
            }
        }
        else
        {
            while(kIter != kIterEnd)
            {
                TType& kPackedElem = (TType&)(*kIter);

                NiInt32 iUnpackedComp = pkInfo->GetUnpackedValue(
                        (NiUInt8)iComponent, kPackedElem); 

                if (iUnpackedComp == dSrcBegin)
                    kPackedElem = pkInfo->PackValueAsIs((NiUInt8)iComponent,
                        dDstBegin, kPackedElem);

                ++kIter;
            }
        }
    }
}
//---------------------------------------------------------------------------
template <typename TType>
static void ApplyRangeRemap(NiDataStreamRef& kStreamRef,
    double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
    NiInt32 iComponent)
{
    NiDataStreamPtr spStream = kStreamRef.GetDataStream();
    NiDataStreamElement kElement = kStreamRef.GetElementDescAt(0);

    NiDataStreamElement::Format eFormat = kElement.GetFormat();
    NiInt8 uiComponents = NiDataStreamElement::GetComponentCount(eFormat);
    NiUInt32 uiElementOffset = kElement.GetOffset();

    NiDataStreamLock kLock(spStream, uiElementOffset,
        NiDataStream::LOCK_TOOL_WRITE);

    // Unsupported
    if (eFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        NIASSERT(!"Special format NORMUINT8_4_BGRA not supported.");
        return;
    }

    if (NiDataStreamElement::IsPacked(eFormat))
    {
        bool bSigned = NiDataStreamElement::IsSigned(eFormat);
        bool bLastFieldIsLiteral = NiDataStreamElement::
            IsLastFieldLiteral(eFormat);

        NiUInt8 uiFieldCnt = NiDataStreamElement::GetFieldCount(eFormat);

        NiBitPackInfo* pkPackInfo = NULL;
        
        if (uiFieldCnt == 4)
            pkPackInfo = NiExternalNew NiTBitPackInfo<4,4>();
        else if (uiFieldCnt == 3)
            pkPackInfo = NiExternalNew NiTBitPackInfo<4,3>();
        else if (uiFieldCnt == 2)
            pkPackInfo = NiExternalNew NiTBitPackInfo<4,2>();
        else if (uiFieldCnt == 1)
            pkPackInfo = NiExternalNew NiTBitPackInfo<4,1>();
        NIASSERT(pkPackInfo);

        for (NiUInt32 uiField=0; uiField< uiFieldCnt; uiField++)
        {
            // GetFieldBits will return zero for anything pass the field cnt
            NiUInt8 uiBits = NiDataStreamElement::GetFieldBits(eFormat,
                (NiUInt8)uiField);

            if (uiField == (NiUInt32)(uiFieldCnt - 1) && bLastFieldIsLiteral)
                pkPackInfo->FillNextField(uiBits, bSigned, false, true);
            else
                pkPackInfo->FillNextField(uiBits, bSigned);
        }
        
        ApplyRangeRemap<NiUInt32, 1>(
            kLock, dSrcBegin, dSrcEnd, dDstBegin, dDstEnd, iComponent,
            pkPackInfo);
        NiExternalDelete pkPackInfo;
        return;
    }

    switch (uiComponents)
    {
        case 1:
            return ApplyRangeRemap<TType, 1>(kLock, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case 2:
            return ApplyRangeRemap<TType, 2>(kLock, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case 3:
            return ApplyRangeRemap<TType, 3>(kLock, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case 4:
            return ApplyRangeRemap<TType, 4>(kLock, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        default:
            NIASSERT(!"Unexpected number of components.");
            break;
    }
}
//---------------------------------------------------------------------------
NiDataStreamRef NiMeshProfileOperations::Remap(const NiDataStreamRef& kSrcRef,
    double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
    NiInt32 iComponent)
{
    if (!IsValidStreamRef(kSrcRef))
        return NiDataStreamRef();

    NiDataStreamRef kNewStreamRef = kSrcRef.CreateFullCopy();

    ApplyRemapInPlace(kNewStreamRef, dSrcBegin, dSrcEnd, dDstBegin, dDstEnd,
        iComponent);

    return kNewStreamRef;
}
//---------------------------------------------------------------------------
void NiMeshProfileOperations::ApplyRemapInPlace(NiDataStreamRef& kStreamRef,
    double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
    NiInt32 iComponent)
{
    NIASSERT(kStreamRef.IsValid());

    if (dSrcBegin == dSrcEnd)
    {
        if (dDstBegin != dDstEnd)
        {
            // A one to many remapping does not make sense.
            return;
        }

        // This is a remapping of a single value. It can be considered
        // a "replacement". It is not clear when this would be useful,
        // but support for it will be allowed.
    }

    NiDataStream* pkStream = kStreamRef.GetDataStream();
    NIASSERT(pkStream);

    NiDataStreamElement kElement = pkStream->GetElementDescAt(0);
    NiDataStreamElement::Type eType = kElement.GetType();

    switch(eType)
    {
        case NiDataStreamElement::T_FLOAT32:
            return ApplyRangeRemap<float>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_FLOAT16:
            return ApplyRangeRemap<NiFloat16>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_UINT32:
            return ApplyRangeRemap<NiUInt32>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_UINT16:
            return ApplyRangeRemap<NiUInt16>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_UINT8:
            return ApplyRangeRemap<NiUInt8>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_INT32:
            return ApplyRangeRemap<NiInt32>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_INT16:
            return ApplyRangeRemap<NiInt16>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        case NiDataStreamElement::T_INT8:
            return ApplyRangeRemap<NiInt8>(kStreamRef, dSrcBegin, dSrcEnd,
                dDstBegin, dDstEnd, iComponent);
        default:
            NIASSERT(!"Unknown Type. Can not perform clamping operation");
            return;
    }
}
//---------------------------------------------------------------------------
