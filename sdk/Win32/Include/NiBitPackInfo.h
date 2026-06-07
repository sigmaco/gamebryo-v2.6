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

#ifndef NIBITPACKINFO_H
#define NIBITPACKINFO_H

#include <NiSystem.h>
#include <NiDataStreamElement.h>
#include "NiTAdjustValue.h"

//---------------------------------------------------------------------------

/// @cond EMERGENT_INTERNAL
/**
    Interface for retreiving information on how how data is packed in the 
    form of bit fields.

    @note Assumes that the data type is no larger than 32 bits (data types
        of size 8 bits, 16, and 32 are acceptable).
    @note Requires that there are at least 2 distinct fields. This
        requirement allows an NiInt32 to be returned for the
        GetUnpackedValue method.
*/
/// @endcond
class NiBitPackInfo
{
public:
    virtual bool FillNextField(NiUInt8 uiNumBits, bool bSigned = false,
        bool bNormalized = false, bool bForceLiteral = false) = 0;
    virtual NiUInt8 GetNumFields() = 0;
    virtual NiUInt8 GetBitsUsed(NiUInt8 uiField) = 0;
    virtual bool IsSigned(NiUInt8 uiField) = 0;
    virtual bool IsNormalized(NiUInt8 uiField) = 0;
    virtual NiUInt32 GetSize() = 0;
    virtual NiUInt32 GetFieldMask(NiUInt8 uiField) = 0;
    virtual NiInt32 GetMin(NiUInt8 uiField) = 0;
    virtual NiInt32 GetMax(NiUInt8 uiField) = 0;
    virtual NiInt32 GetUnpackedValue(NiUInt8 uiField, NiUInt32
        uiPackedValue) = 0;

    // This pack will only perform bit-field max/min clamping. It will not do
    // any normalization regardless of the bitfield normalization setting.
    virtual NiUInt32 PackValueAsIs(NiUInt8 uiField,
        double dUnpackedValue, NiUInt32 uiPackedData) = 0;

    // These packs will normalize the data if the bitfield is specified as
    // normalized.
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiInt32 iUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiInt16 iUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiInt8 iUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiUInt32 uiUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiUInt16 uiUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiUInt8 uiUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, float fUnpackedValue,
        NiUInt32 uiPackedData) = 0;
    virtual NiUInt32 PackValue(NiUInt8 uiField, NiFloat16 fUnpackedValue,
        NiUInt32 uiPackedData) = 0;

    // eType is used to interpret pkIn
    virtual bool PackStream(NiDataStreamElement::Type eType, const void* pkIn,
        NiUInt32 uiComp, NiUInt32 uiCount, void* pkPacked, 
        NiUInt32 uiPackedSize) = 0;

    virtual NiUInt32 ClearField(NiUInt8 uiField, NiUInt32 uiPackedData) = 0;
};

//---------------------------------------------------------------------------

/// @cond EMERGENT_INTERNAL
/**
    Templated class that implements the NiBitPackInfo interface.

    @param TBytes Specifies the number of bytes that will be used for the
        bitfield. Values of 1, 2, and 4 bytes are acceptable.
    @param TNumFields Specifies the number of fields that the data type will
        be partitioned into.
*/
/// @endcond
template<NiUInt8 TBytes, NiUInt8 TNumFields>
class NiTBitPackInfo : public NiBitPackInfo
{
public:
    NiTBitPackInfo()
    {
        // Must have at least 2 fields and can be a 8, 16, or 32 bit length
        NIASSERT(TNumFields >= 2);
        NIASSERT(TBytes == 1 || TBytes == 2 || TBytes == 4);

        memset(m_uiBits, 0, sizeof(NiUInt8) * TNumFields);
        memset(m_uiShifts, 0, sizeof(NiUInt8) * TNumFields);
        memset(m_uiFieldMask, 0, sizeof(NiUInt32) * TNumFields);
        memset(m_uiLiteralValue, 1, sizeof(NiUInt8) * TNumFields);

        m_bFillMissingDataWithLiteral = true;
    }

    inline virtual bool FillNextField(NiUInt8 uiNumBits, bool bSigned = false,
        bool bNormalized = false, bool bForceLiteral = false)
    {
        NiUInt8 uiField = 0;
    
        do
        {
            if (m_uiBits[uiField] > 0)
                uiField++;
            else 
                break;
        } while (uiField < GetNumFields());

        if (uiField >= GetNumFields())
            return false;

        m_uiBits[uiField] = uiNumBits;

        // Calculate the shifts and field mask
        NiUInt32 uiMask = 1;

        for(NiUInt32 uiBit = 1; uiBit < uiNumBits; uiBit++)
        {
            uiMask <<= 1;
            uiMask |= 1;
        }

        if (bSigned)
        {           
            m_iMax[uiField] = uiMask >> 1;
            m_iMin[uiField] = -(m_iMax[uiField] + 1);
        }
        else
        {
            m_iMin[uiField] = 0;
            m_iMax[uiField] = uiMask;
        }

        m_abNormalized[uiField] = bNormalized;
        m_bForceLiteral[uiField] = bForceLiteral;

        NiUInt8 uiShifts = 0;
        for(NiUInt8 uiCurr = 0; uiCurr < uiField; uiCurr++)
            uiShifts = (NiUInt8)(uiShifts + m_uiBits[uiCurr]);

        m_uiShifts[uiField] = uiShifts;
        
        uiMask <<= uiShifts;
        m_uiFieldMask[uiField] = uiMask;

        return true;
    }

    inline virtual NiUInt8 GetNumFields()
    {
        return TNumFields;
    }

    inline virtual NiUInt8 GetBitsUsed(NiUInt8 uiField)
    {
        NIASSERT(uiField < GetNumFields());
        return m_uiBits[uiField];
    }

    inline virtual bool IsSigned(NiUInt8 uiField)
    {
        NIASSERT(uiField < GetNumFields());
        if (m_iMin[uiField] < 0)
            return true;
        else
            return false;
    }

    inline virtual bool IsNormalized(NiUInt8 uiField)
    {
        NIASSERT(uiField < GetNumFields());
        return m_abNormalized[uiField];
    }

    inline virtual NiUInt32 GetSize()
    {
        return TBytes;
    }

    inline virtual NiUInt32 GetFieldMask(NiUInt8 uiField)
    {
        NIASSERT(uiField < GetNumFields());
        return m_uiFieldMask[uiField];
    }

    inline virtual NiInt32 GetMin(NiUInt8 uiField)
    {
        NIASSERT(uiField < GetNumFields());
        return m_iMin[uiField];
    }

    inline virtual NiInt32 GetMax(NiUInt8 uiField)
    {
        NIASSERT(uiField < GetNumFields());
        return m_iMax[uiField];
    }

    inline virtual NiInt32 GetUnpackedValue(NiUInt8 uiField, NiUInt32 uiPackedValue)
    {
        NIASSERT(uiField < GetNumFields());
        NiUInt32 uiFieldMask = GetFieldMask(uiField);

        if (m_iMin[uiField] < 0)
        {
            NiUInt32 uiSignMask = 1;
            uiSignMask <<= (m_uiShifts[uiField] + (m_uiBits[uiField] - 1));
            NiUInt32 uiSign = (uiPackedValue & uiSignMask);
         
            if (uiSign)
            {
                NiInt32 iUnpackedValue =
                    (uiPackedValue & uiFieldMask) >> m_uiShifts[uiField];
                uiFieldMask >>= m_uiShifts[uiField];
                uiFieldMask ^= 0xffffffff;
                iUnpackedValue = (NiInt32)(iUnpackedValue | uiFieldMask);
                return iUnpackedValue;
            }
            else
            {
                return (uiPackedValue & uiFieldMask) >> m_uiShifts[uiField];
            }
        }
        else
        {
            return (uiPackedValue & uiFieldMask) >> m_uiShifts[uiField];
        }
    }

    inline virtual NiUInt32 PackValueAsIs(NiUInt8 uiField, double dUnpackedValue,
        NiUInt32 uiPackedData)
    {
        if (m_bForceLiteral[uiField])
        {
            // Intercept and pack the literal
            return PackValueClamp(uiField, (double)m_uiLiteralValue[uiField],
                uiPackedData, (double)m_iMin[uiField],
                (double)m_iMax[uiField]);
        }

        return PackValueClamp(uiField, dUnpackedValue, uiPackedData,
                (double)m_iMin[uiField], (double)m_iMax[uiField]);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiInt32 iUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiInt32>(uiField, iUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiInt16 iUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiInt16>(uiField, iUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiInt8 iUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiInt8>(uiField, iUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiUInt32 uiUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiUInt32>(uiField, uiUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiUInt16 uiUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiUInt16>(uiField, uiUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiUInt8 uiUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiUInt8>(uiField, uiUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, float fUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<float>(uiField, fUnpackedValue, uiPackedData);
    }

    inline virtual NiUInt32 PackValue(NiUInt8 uiField, NiFloat16 fUnpackedValue,
        NiUInt32 uiPackedData)
    {
        return TPackValue<NiFloat16>(uiField, fUnpackedValue, uiPackedData);
    }

    inline virtual bool PackStream(NiDataStreamElement::Type eType, const void* pkIn,
        NiUInt32 uiComp, NiUInt32 uiCount, void* pkPacked, 
        NiUInt32 uiPackedSize)
    {
        switch(eType)
        {
            case NiDataStreamElement::T_FLOAT32:
                return TPackStreamResolvePackSize<float>((float*)pkIn, uiComp,
                    uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_FLOAT16:
                return TPackStreamResolvePackSize<NiFloat16>((NiFloat16*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_INT32:
                return TPackStreamResolvePackSize<NiInt32>((NiInt32*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_INT16:
                return TPackStreamResolvePackSize<NiInt16>((NiInt16*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_INT8:
                return TPackStreamResolvePackSize<NiInt8>((NiInt8*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_UINT32:
                return TPackStreamResolvePackSize<NiUInt32>((NiUInt32*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_UINT16:
                return TPackStreamResolvePackSize<NiUInt16>((NiUInt16*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
            case NiDataStreamElement::T_UINT8:
                return TPackStreamResolvePackSize<NiUInt8>((NiUInt8*)pkIn,
                    uiComp, uiCount, pkPacked, uiPackedSize);
        }
        return false;
    }

    inline virtual NiUInt32 ClearField(NiUInt8 uiField, NiUInt32 uiPackedData)
    {
        NIASSERT(uiField < GetNumFields());

        NiUInt32 uiMask = GetFieldMask(uiField);
        uiMask ^= NiUInt32(-1);

        return (uiPackedData & uiMask);
    }

protected:
    inline NiUInt32 PackValueClamp(NiUInt8 uiField,
        double dUnpackedValue, NiUInt32 uiPackedData, double dMin, double dMax)
    {
        NIASSERT(uiField < GetNumFields());
        // If not normalized, then the value is packed with type-clamping.
        if (dUnpackedValue > dMax)
            dUnpackedValue = dMax;
        else if (dUnpackedValue < dMin)
            dUnpackedValue = dMin;

        // Data has been clamped or normalized - cast it to NiUInt32
        NiUInt32 uiUnpackedValue = (NiUInt32)dUnpackedValue;

        NiUInt32 uiUnpackMask = (GetFieldMask(uiField) >> m_uiShifts[uiField]);
        uiUnpackedValue &= uiUnpackMask; 

        NiUInt32 uiData = (uiUnpackedValue << m_uiShifts[uiField]);
        uiPackedData = ClearField(uiField, uiPackedData);

        return (uiData |= uiPackedData);
    }

    inline NiUInt32 PackValueNormalize(NiUInt8 uiField,
        double dUnpackedValue, NiUInt32 uiPackedData, double dSrcMin,
        double dSrcRange, double dDestMin, double dDestRange)
    {
        NIASSERT(uiField < GetNumFields());
        NIASSERT(m_abNormalized[uiField]);
        double dT = (dUnpackedValue - dSrcMin) / dSrcRange;
        dUnpackedValue = dDestMin + dDestRange * dT;

        if (dUnpackedValue < m_iMin[uiField])
            dUnpackedValue = (double)m_iMin[uiField];
        else if (dUnpackedValue > m_iMax[uiField])
            dUnpackedValue = (double)m_iMax[uiField];

        // Data has been clamped or normalized - cast it to NiUInt32
        NiUInt32 uiUnpackedValue = (NiUInt32)dUnpackedValue;

        NiUInt32 uiUnpackMask = (GetFieldMask(uiField) >> m_uiShifts[uiField]);
        uiUnpackedValue &= uiUnpackMask; 

        NiUInt32 uiData = (uiUnpackedValue << m_uiShifts[uiField]);
        uiPackedData = ClearField(uiField, uiPackedData);

        return (uiData |= uiPackedData);
    }

    template <typename T>
    inline NiUInt32 TPackValue(NiUInt8 uiField, double dUnpackedValue,
        NiUInt32 uiPackedData)
    {
        NIASSERT(uiField < GetNumFields());

        if (!m_abNormalized[uiField] || m_bForceLiteral[uiField])
        {
            if (m_bForceLiteral[uiField])
            {
                return PackValueClamp(uiField,
                    (double)m_uiLiteralValue[uiField], uiPackedData,
                    (double)m_iMin[uiField], (double)m_iMax[uiField]);
            }
            else
            {
                return PackValueClamp(uiField, dUnpackedValue, uiPackedData,
                    (double)m_iMin[uiField], (double)m_iMax[uiField]);
            }
        }

        // If normalized, the packed value is linearly remapped.
        double dSrcMin, dSrcMax;

        // Floats and Float16's are a special case. It's src interpretation
        // depends on if the destination is signed or unsigned.
        if (IsFloatingPoint<T>())
        {
            if (IsSigned(uiField))
            {
                // If the source is floating point and the dest is signed
                dSrcMin = -1.0f;
                dSrcMax = 1.0f;
            }
            else
            {
                // If the source is floating point and the dest is unsigned
                dSrcMin = 0.0f;
                dSrcMax = 1.0f;
            }
        }
        else
        {
            GetTypeRange<T>(dSrcMin, dSrcMax);
        }

        double dSrcRange = dSrcMax - dSrcMin;
        double dDestRange = (double)(m_iMax[uiField] - m_iMin[uiField]);
        double dDestMin = (double)m_iMin[uiField];
        return PackValueNormalize(uiField, dUnpackedValue,
            uiPackedData, dSrcMin, dSrcRange, dDestMin, dDestRange);
    }

    template <typename T>
    inline bool TPackStreamResolvePackSize(T* pkIn, NiUInt32 uiComp, NiUInt32 uiCount,
        void* pkPacked, NiUInt32 uiPackedSize)
    {
        // Ensure allocated packedSize is sufficient.
        if (uiPackedSize != (uiCount * TBytes))
            return false;

        // Packed is assumed to have:
        //       TBytes sized component
        //       uiComp = 1
        //       element count = uiCount

        // Number of components must be less than or equal to the number of
        // fields. If the components are less than the number of fieldss, then
        // extra fields will be packed with zero or one depending on the value
        // of m_bFillMissingDataWithOne.
        if (TNumFields < uiComp)
            return false;

        switch(TBytes)
        {
            case 1:
                return TPackStream<T, NiUInt8>(pkIn, uiComp, uiCount, 
                    (NiUInt8*)pkPacked);
                break;
            case 2:
                return TPackStream<T, NiUInt16>(pkIn, uiComp, uiCount,
                    (NiUInt16*)pkPacked);
                break;
            case 4:
                return TPackStream<T, NiUInt32>(pkIn, uiComp, uiCount,
                    (NiUInt32*)pkPacked);
                break;
            default:
                NIASSERT(!"Bytes specified not supported!");
        }
        return false;
    }

    template <typename TIn, typename TOut>
    inline bool TPackStream(TIn* pkIn, NiUInt32 uiComp, NiUInt32 uiCount,
        TOut* pkPacked)
    {
        for(NiUInt32 ui=0; ui<uiCount; ui++)
        {
            NiUInt32 uiPacked = 0;
            for(NiUInt32 uiField=0; uiField < GetNumFields(); uiField++)
            {
                if (uiField < uiComp)
                {
                    uiPacked = TPackValue<TIn>((NiUInt8)uiField, (double)*pkIn,
                        uiPacked);
                    pkIn++;
                }
                else if (m_bFillMissingDataWithLiteral)
                {
                    // Ran out of components, fill field with literal (default
                    // is 1). Pack without normalization (as is)
                    uiPacked = PackValueClamp((NiUInt8)uiField,
                        m_uiLiteralValue[uiField], uiPacked,
                        (double)m_iMin[uiField], (double)m_iMax[uiField]);
                }
            }
            *pkPacked++ = (TOut)uiPacked;
        }
        return true;
    }
    NiUInt8 m_uiBits[TNumFields]; // number of bits allocated for field
    NiUInt8 m_uiShifts[TNumFields]; // number of shifts to field position
    NiUInt32 m_uiFieldMask[TNumFields]; // bit mask for field
    NiInt32 m_iMin[TNumFields]; // the min value range
    NiInt32 m_iMax[TNumFields]; // the max value range
    NiUInt8 m_uiLiteralValue[TNumFields]; // for cases where literal is needed
    bool m_bForceLiteral[TNumFields]; // for cases where literal is needed
    bool m_abNormalized[TNumFields]; // indicates if field is normalized or not
    bool m_bFillMissingDataWithLiteral;
};


#endif // NIBITPACKINFO_H
