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
// NiMeshProfileOperations Helpers for CastConvert
//---------------------------------------------------------------------------
template <typename TType, NiUInt8 TNumComponents>
static void ApplyClamp(NiDataStreamLock& kLock,
    double dMin, double dMax, NiInt32 iComponent)
{
    // Only 1-4 component size expected.
    NIASSERT(TNumComponents > 0 && TNumComponents < 5);
    NIASSERT((iComponent >= 0 && iComponent < 4) || iComponent == -1);

    NIASSERT(iComponent < (NiInt32)TNumComponents);
    if (iComponent >= (NiInt32)TNumComponents)
        return;

    NIASSERT(kLock.IsLocked());

    if (dMax < dMin)
    {
        dMin = dMax;
        dMax = dMin;
    }

    NiTStridedRandomAccessIterator<TType[TNumComponents]> kIter = 
        kLock.begin<TType[TNumComponents]>();
    NiTStridedRandomAccessIterator<TType[TNumComponents]> kIterEnd =
        kLock.end<TType[TNumComponents]>();

    if (iComponent == -1)
    {
        while(kIter != kIterEnd)
        {
            TType* kElem = (TType*)(*kIter);
            for (NiUInt32 uiComp = 0; uiComp < TNumComponents; uiComp++)
            {
                double dValue;

                // Clamp if necessary
                if (kElem[uiComp] < dMin)
                    dValue = dMin;
                else if (kElem[uiComp] > dMax)
                    dValue = dMax;
                else
                    dValue = kElem[uiComp];

                // There may be additional clamping due to format
                kElem[uiComp] = AdjustValue<TType>(dValue);
            }

            ++kIter;
        }
    }
    else
    {
        while(kIter != kIterEnd)
        {
            TType* kElem = (TType*)(*kIter);

            double dValue;

            // Clamp if necessary
            if (kElem[iComponent] < dMin)
                dValue = dMin;
            else if (kElem[iComponent] > dMax)
                dValue = dMax;
            else
                dValue = kElem[iComponent];

            // There may be additional clamping due to format
            kElem[iComponent] = AdjustValue<TType>(dValue);

            ++kIter;
        }
    }
}
//---------------------------------------------------------------------------
template <typename TType, NiUInt8 TNumComponents>
static void ApplyClamp(NiDataStreamLock& kLock,
    double dMin, double dMax, NiInt32 iComponent, NiBitPackInfo* pkInfo)
{
    NIASSERT(sizeof(TType) == pkInfo->GetSize());
    NIASSERT(TNumComponents == 1);
    NIASSERT((iComponent >= 0 && iComponent < pkInfo->GetNumFields())
        || iComponent == -1);
    NIASSERT(kLock.IsLocked());

    if (iComponent >= (NiInt32)(pkInfo->GetNumFields()))
        return;

    NiTStridedRandomAccessIterator<NiUInt32> kIter =
        kLock.begin<NiUInt32>();
    NiTStridedRandomAccessIterator<NiUInt32> kIterEnd =
        kLock.end<NiUInt32>();

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

                // Clamp if necessary
                double dValue;
                if (iUnpackedComp < dMin)
                    dValue = dMin;
                else if (iUnpackedComp > dMax)
                    dValue = dMax;
                else
                    dValue = iUnpackedComp;

                kPackedElem = pkInfo->PackValueAsIs((NiUInt8)uiComp,dValue,kPackedElem);
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

            // Clamp if necessary
            double dValue;
            if (iUnpackedComp < dMin)
                dValue = dMin;
            else if (iUnpackedComp > dMax)
                dValue = dMax;
            else
                dValue = iUnpackedComp;

            kPackedElem = pkInfo->PackValueAsIs((NiUInt8)iComponent,
                dValue,kPackedElem);
            ++kIter;
        }
    }
}
//---------------------------------------------------------------------------
template <typename TType>
static void ApplyClamp(NiDataStreamRef& kStreamRef,
    double dMin, double dMax, NiInt32 iComponent)
{
    NiDataStreamPtr spStream = kStreamRef.GetDataStream();
    NiDataStreamElement kElement = spStream->GetElementDescAt(0);

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

        for (NiUInt32 uiField=0; uiField < uiFieldCnt; uiField++)
        {
            // GetFieldBits will return zero for anything pass the field cnt
            NiUInt8 uiBits = NiDataStreamElement::GetFieldBits(eFormat,
                (NiUInt8)uiField);
            pkPackInfo->FillNextField(uiBits, bSigned);
        }
        
        ApplyClamp<NiUInt32, 1>(kLock, dMin, dMax, iComponent,
            pkPackInfo);
        NiExternalDelete pkPackInfo;
        return;
    }

    switch (uiComponents)
    {
        case 1:
            return ApplyClamp<TType, 1>(kLock, dMin, dMax, iComponent);
        case 2:
            return ApplyClamp<TType, 2>(kLock, dMin, dMax, iComponent);
        case 3:
            return ApplyClamp<TType, 3>(kLock, dMin, dMax, iComponent);
        case 4:
            return ApplyClamp<TType, 4>(kLock, dMin, dMax, iComponent);
        default:
            NIASSERT(!"Unexpected number of components.");
            break;
    }
}
//---------------------------------------------------------------------------
NiDataStreamRef NiMeshProfileOperations::Clamp(const NiDataStreamRef& kSrcRef, 
    NiInt32 iComponent, double dMin, double dMax)
{
    if (!IsValidStreamRef(kSrcRef))
        return NiDataStreamRef();

    NiDataStreamRef kNewStreamRef = kSrcRef.CreateFullCopy();

    ApplyClampInPlace(kNewStreamRef, dMin, dMax, iComponent);

    return kNewStreamRef;
}
//---------------------------------------------------------------------------
void NiMeshProfileOperations::ApplyClampInPlace(NiDataStreamRef& kStreamRef,
    double dMin, double dMax, NiInt32 iComponent)
{
    NIASSERT(kStreamRef.IsValid());
    
    NiDataStream* pkStream = kStreamRef.GetDataStream();
    NIASSERT(pkStream);

    NiDataStreamElement kElement = pkStream->GetElementDescAt(0);

    NiDataStreamElement::Type eType = kElement.GetType();

    switch(eType)
    {   
        case NiDataStreamElement::T_FLOAT32:
            return ApplyClamp<float>(kStreamRef, dMin, dMax, iComponent);
        case NiDataStreamElement::T_FLOAT16:
            return ApplyClamp<NiFloat16>(kStreamRef, dMin, dMax, iComponent);
        case NiDataStreamElement::T_UINT32:
            return ApplyClamp<NiUInt32>(kStreamRef, dMin, dMax, iComponent);
        case NiDataStreamElement::T_UINT16:
            return ApplyClamp<NiUInt16>(kStreamRef, dMin, dMax, iComponent);
        case NiDataStreamElement::T_UINT8:
            return ApplyClamp<NiUInt8>(kStreamRef, dMin, dMax, iComponent);

        case NiDataStreamElement::T_INT32:
            return ApplyClamp<NiInt32>(kStreamRef, dMin, dMax, iComponent);
        case NiDataStreamElement::T_INT16:
            return ApplyClamp<NiInt16>(kStreamRef, dMin, dMax, iComponent);
        case NiDataStreamElement::T_INT8:
            return ApplyClamp<NiInt8>(kStreamRef, dMin, dMax, iComponent);
        default:
            NIASSERT(!"Unknown Type. Can not perform clamping operation");
            return;
    }
}
//---------------------------------------------------------------------------
