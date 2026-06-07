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

// Precompiled Header
#include "NiMeshPCH.h"

#include "NiDataStreamElement.h"

#include "NiEnumerationStrings.h"
#include "NiSystem.h"

const NiDataStreamElement NiDataStreamElement::ms_kInvalidElement;

#define ENCODE_DATAFIELD_BITS(F0, F1, F2, F3) (F3<<24 | F2<<16 | F1<<8 | F0)

//---------------------------------------------------------------------------
// NiDataStreamElement Helper class
//---------------------------------------------------------------------------
class FormatTable
{
public:
    class FormatEntry
    {
    public:
        bool m_bPacked;
        bool m_bSigned;
        bool m_bNormalized;
        bool m_bLastFieldIsLiteral;
        NiUInt8 m_uiLayout;

        union ComponentInfo
        {
            NiDataStreamElement::Type m_eType; // used for non-packed
            NiUInt32 m_uiFields; // packed: embeds bits per field [1-4]
        };

        ComponentInfo m_ComponentInfo;

        void SetPackedField(bool bNormalized, bool bSigned,
            NiDataStreamElement::Layout eLayout, NiUInt8 uiField0,
            NiUInt8 uiField1, NiUInt8 uiField2 = 0, NiUInt8 uiField3 = 0,
            bool bLastFieldIsLiteral = false)
        {
            NIASSERT(uiField0 < 32 && uiField1 < 32);
            NIASSERT(uiField0 > 0 && uiField1 > 0);

            int iNumFields = 2;
            if (uiField2 > 0)
            {
                NIASSERT(uiField2 < 32);
                iNumFields++;
                if (uiField3 > 0)
                {
                    NIASSERT(uiField3 < 32);
                    iNumFields++;
                }
            }

            // verify that all bits are accounted for
            NIASSERT(uiField0 + uiField1 + uiField2 + uiField3 == 32);

            // verify the layout matches the number of fields
            NIASSERT((iNumFields == 2 && (eLayout >= NiDataStreamElement::L_XY
                && eLayout <= NiDataStreamElement::L_YX)) ||
                (iNumFields == 3 && (eLayout >= NiDataStreamElement::L_XYZ
                && eLayout <= NiDataStreamElement::L_ZYX)) ||
                (iNumFields == 4 && (eLayout >= NiDataStreamElement::L_XYZW
                && eLayout <= NiDataStreamElement::L_ZYWX)));

            m_bPacked = true;
            m_bNormalized = bNormalized;
            m_bSigned = bSigned;
            m_bLastFieldIsLiteral = bLastFieldIsLiteral;
            m_uiLayout = (NiUInt8) eLayout;
            m_ComponentInfo.m_uiFields = 
                ENCODE_DATAFIELD_BITS(uiField0, uiField1, uiField2, uiField3);
        }

        void SetField(bool bNormalized, NiDataStreamElement::Layout eLayout,
            NiDataStreamElement::Type eType)
        {
            m_bPacked = false;
            m_bNormalized = bNormalized;
            if (eType == NiDataStreamElement::T_UINT8 ||
                eType == NiDataStreamElement::T_UINT16 ||
                eType == NiDataStreamElement::T_UINT32)
            {
                m_bSigned = false;
            }
            else
            {
                m_bSigned = true;
            }
            m_uiLayout = (NiUInt8) eLayout;
            m_ComponentInfo.m_eType = eType;
        }
    };

    FormatTable()
    {
        // F_INT8_1, F_INT8_2, F_INT8_3, F_INT8_4
        m_aEntry[1].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_INT8);
        m_aEntry[2].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_INT8);
        m_aEntry[3].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_INT8);
        m_aEntry[4].SetField(
            false, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_INT8);

        // F_UINT8_1, F_UINT8_2, F_UINT8_3, F_UINT8_4
        m_aEntry[5].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_UINT8);
        m_aEntry[6].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_UINT8);
        m_aEntry[7].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_UINT8);
        m_aEntry[8].SetField(
            false, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_UINT8);

        // F_NORMINT8_1, F_NORMINT8_2, F_NORMINT8_3, F_NORMINT8_4
        m_aEntry[9].SetField(
            true, NiDataStreamElement::L_X, NiDataStreamElement::T_INT8);
        m_aEntry[10].SetField(
            true, NiDataStreamElement::L_XY, NiDataStreamElement::T_INT8);
        m_aEntry[11].SetField(
            true, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_INT8);
        m_aEntry[12].SetField(
            true, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_INT8);

        // F_NORMUINT8_1, F_NORMUINT8_2, F_NORMUINT8_3, F_NORMUINT8_4
        m_aEntry[13].SetField(
            true, NiDataStreamElement::L_X, NiDataStreamElement::T_UINT8);
        m_aEntry[14].SetField(
            true, NiDataStreamElement::L_XY, NiDataStreamElement::T_UINT8);
        m_aEntry[15].SetField(
            true, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_UINT8);
        m_aEntry[16].SetField(
            true, NiDataStreamElement::L_XYZW,NiDataStreamElement::T_UINT8);

        // F_INT16_1, F_INT16_2, F_INT16_3, F_INT16_4
        m_aEntry[17].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_INT16);
        m_aEntry[18].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_INT16);
        m_aEntry[19].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_INT16);
        m_aEntry[20].SetField(
            false, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_INT16);

        // F_UINT16_1, F_UINT16_2, F_UINT16_3, F_UINT16_4
        m_aEntry[21].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_UINT16);
        m_aEntry[22].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_UINT16);
        m_aEntry[23].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_UINT16);
        m_aEntry[24].SetField(
            false, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_UINT16);

        // F_NORMINT16_1, F_NORMINT16_2, F_NORMINT16_3, F_NORMINT16_4
        m_aEntry[25].SetField(
            true, NiDataStreamElement::L_X, NiDataStreamElement::T_INT16);
        m_aEntry[26].SetField(
            true, NiDataStreamElement::L_XY, NiDataStreamElement::T_INT16);
        m_aEntry[27].SetField(
            true, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_INT16);
        m_aEntry[28].SetField(
            true, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_INT16);

        // F_NORMUINT16_1, F_NORMUINT16_2, F_NORMUINT16_3, F_NORMUINT16_4
        m_aEntry[29].SetField(
            true, NiDataStreamElement::L_X, NiDataStreamElement::T_UINT16);
        m_aEntry[30].SetField(
            true, NiDataStreamElement::L_XY, NiDataStreamElement::T_UINT16);
        m_aEntry[31].SetField(
            true, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_UINT16);
        m_aEntry[32].SetField(
            true, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_UINT16);

        // F_INT32_1, F_INT32_2, F_INT32_3, F_INT32_4
        m_aEntry[33].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_INT32);
        m_aEntry[34].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_INT32);
        m_aEntry[35].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_INT32);
        m_aEntry[36].SetField(
            false, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_INT32);

        // F_UINT32_1, F_UINT32_2, F_UINT32_3, F_UINT32_4
        m_aEntry[37].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_UINT32);
        m_aEntry[38].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_UINT32);
        m_aEntry[39].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_UINT32);
        m_aEntry[40].SetField(
            false, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_UINT32);

        // F_NORMINT32_1, F_NORMINT32_2, F_NORMINT32_3, F_NORMINT32_4
        m_aEntry[41].SetField(
            true, NiDataStreamElement::L_X, NiDataStreamElement::T_INT32);
        m_aEntry[42].SetField(
            true, NiDataStreamElement::L_XY, NiDataStreamElement::T_INT32);
        m_aEntry[43].SetField(
            true, NiDataStreamElement::L_XYZ,NiDataStreamElement::T_INT32);
        m_aEntry[44].SetField(
            true, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_INT32);

        // F_NORMUINT32_1, F_NORMUINT32_2, F_NORMUINT32_3, F_NORMUINT32_4
        m_aEntry[45].SetField(
            true, NiDataStreamElement::L_X, NiDataStreamElement::T_UINT32);
        m_aEntry[46].SetField(
            true, NiDataStreamElement::L_XY,NiDataStreamElement::T_UINT32);
        m_aEntry[47].SetField(
            true, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_UINT32);
        m_aEntry[48].SetField(
            true, NiDataStreamElement::L_XYZW, NiDataStreamElement::T_UINT32);

        // F_FLOAT16_1, F_FLOAT16_2, F_FLOAT16_3, F_FLOAT16_4
        m_aEntry[49].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_FLOAT16);
        m_aEntry[50].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_FLOAT16);
        m_aEntry[51].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_FLOAT16);
        m_aEntry[52].SetField(
            false, NiDataStreamElement::L_XYZW,NiDataStreamElement::T_FLOAT16);

        // F_FLOAT32_1, F_FLOAT32_2, F_FLOAT32_3, F_FLOAT32_4
        m_aEntry[53].SetField(
            false, NiDataStreamElement::L_X, NiDataStreamElement::T_FLOAT32);
        m_aEntry[54].SetField(
            false, NiDataStreamElement::L_XY, NiDataStreamElement::T_FLOAT32);
        m_aEntry[55].SetField(
            false, NiDataStreamElement::L_XYZ, NiDataStreamElement::T_FLOAT32);
        m_aEntry[56].SetField(
            false, NiDataStreamElement::L_XYZW,NiDataStreamElement::T_FLOAT32);

        // F_UINT_10_10_10_L1, F_NORMINT_10_10_10_L1, F_NORMINT_11_11_10
        m_aEntry[57].SetPackedField(
            false, false, NiDataStreamElement::L_XYZW, 10, 10, 10, 2, true);
        m_aEntry[58].SetPackedField(
            true, true, NiDataStreamElement::L_XYZW, 10, 10, 10, 2, true);
        m_aEntry[59].SetPackedField(
            true, true, NiDataStreamElement::L_XYZ, 11, 11, 10);

        // F_NORMUINT8_4_BGRA
        m_aEntry[60].SetField(true,
            NiDataStreamElement::L_ZYXW, NiDataStreamElement::T_UINT8);

        // F_NORMINT_10_10_10_2
        m_aEntry[61].SetPackedField(
            true, true, NiDataStreamElement::L_XYZW, 10, 10, 10, 2);

        // F_UINT_10_10_10_2
        m_aEntry[62].SetPackedField(
            false, false, NiDataStreamElement::L_XYZW, 10, 10, 10, 2);
    }

    FormatEntry m_aEntry[NiDataStreamElement::F_TYPE_COUNT];
};
FormatTable gkFormatTable;

//---------------------------------------------------------------------------
NiDataStreamElement::NiDataStreamElement() : m_uiOffset(UINT_MAX), 
    m_eFormat(F_UNKNOWN)
{
}
//---------------------------------------------------------------------------
NiDataStreamElement::Format NiDataStreamElement::GetPredefinedPackedFormat(
    bool bNormalized, bool bSigned, bool bLastFieldIsLiteral,
    NiUInt8 uiF0, NiUInt8 uiF1, NiUInt8 uiF2, NiUInt8 uiF3)
{
    NiUInt32 uiDataFields = ENCODE_DATAFIELD_BITS(uiF0, uiF1, uiF2, uiF3);

    if (bNormalized)
    {
        if (bSigned)
        {
            switch(uiDataFields)
            {
                case ENCODE_DATAFIELD_BITS(11, 11, 10, 0):
                {
                    if (bLastFieldIsLiteral == false)
                        return NiDataStreamElement::F_NORMINT_11_11_10;
                }
                case ENCODE_DATAFIELD_BITS(10, 10, 10, 2):
                {
                    if (bLastFieldIsLiteral)
                        return NiDataStreamElement::F_NORMINT_10_10_10_L1;
                    else
                        return NiDataStreamElement::F_NORMINT_10_10_10_2;
                }
            }
        }
    }
    else
    {
        if (!bSigned)
        {
            switch(uiDataFields)
            {
                case ENCODE_DATAFIELD_BITS(10, 10, 10, 2):
                {
                    if (bLastFieldIsLiteral)
                        return NiDataStreamElement::F_UINT_10_10_10_L1;
                    else
                        return NiDataStreamElement::F_UINT_10_10_10_2;
                }
            }
        }
    }
    return NiDataStreamElement::F_UNKNOWN;
}
//---------------------------------------------------------------------------
NiDataStreamElement::Format NiDataStreamElement::GetPredefinedFormat(
    NiDataStreamElement::Type eType, NiUInt8 uiComponentCount, 
    bool bNormalized, Layout eLayout)
{
    if (eLayout == L_X || eLayout == L_XY || eLayout == L_XYZ ||
        eLayout == L_XYZW)
    {
        // Not swizzled
        return GetPredefinedFormat(eType, uiComponentCount, bNormalized);
    }

    // Only 1 swizzled format is currently predefined.
    if (eLayout == L_ZYXW && eType == T_UINT8 && bNormalized &&
        uiComponentCount == 4)
    {
        return F_NORMUINT8_4_BGRA;
    }

    return F_UNKNOWN;
}
//---------------------------------------------------------------------------
NiDataStreamElement::Format NiDataStreamElement::GetPredefinedFormat(
    NiDataStreamElement::Type eType, NiUInt8 uiComponentCount, 
    bool bNormalized)
{
    switch (eType)
    {
    case T_INT8:
        if (bNormalized)
        {
            if (uiComponentCount == 1)
                return F_NORMINT8_1;
            else if (uiComponentCount == 2)
                return F_NORMINT8_2;
            else if (uiComponentCount == 3)
                return F_NORMINT8_3;
            else if (uiComponentCount == 4)
                return F_NORMINT8_4;
        }
        else
        {
            if (uiComponentCount == 1)
                return F_INT8_1;
            else if (uiComponentCount == 2)
                return F_INT8_2;
            else if (uiComponentCount == 3)
                return F_INT8_3;
            else if (uiComponentCount == 4)
                return F_INT8_4;
        }
        break;
    case T_UINT8:
        if (bNormalized)
        {
            if (uiComponentCount == 1)
                return F_NORMUINT8_1;
            else if (uiComponentCount == 2)
                return F_NORMUINT8_2;
            else if (uiComponentCount == 3)
                return F_NORMUINT8_3;
            else if (uiComponentCount == 4)
                return F_NORMUINT8_4;
        }
        else
        {
            if (uiComponentCount == 1)
                return F_UINT8_1;
            else if (uiComponentCount == 2)
                return F_UINT8_2;
            else if (uiComponentCount == 3)
                return F_UINT8_3;
            else if (uiComponentCount == 4)
                return F_UINT8_4;
        }
        break;
    case T_INT16:
        if (bNormalized)
        {
            if (uiComponentCount == 1)
                return F_NORMINT16_1;
            else if (uiComponentCount == 2)
                return F_NORMINT16_2;
            else if (uiComponentCount == 3)
                return F_NORMINT16_3;
            else if (uiComponentCount == 4)
                return F_NORMINT16_4;
        }
        else
        {
            if (uiComponentCount == 1)
                return F_INT16_1;
            else if (uiComponentCount == 2)
                return F_INT16_2;
            else if (uiComponentCount == 3)
                return F_INT16_3;
            else if (uiComponentCount == 4)
                return F_INT16_4;
        }
        break;
    case T_UINT16:
        if (bNormalized)
        {
            if (uiComponentCount == 1)
                return F_NORMUINT16_1;
            else if (uiComponentCount == 2)
                return F_NORMUINT16_2;
            else if (uiComponentCount == 3)
                return F_NORMUINT16_3;
            else if (uiComponentCount == 4)
                return F_NORMUINT16_4;
        }
        else
        {
            if (uiComponentCount == 1)
                return F_UINT16_1;
            else if (uiComponentCount == 2)
                return F_UINT16_2;
            else if (uiComponentCount == 3)
                return F_UINT16_3;
            else if (uiComponentCount == 4)
                return F_UINT16_4;
        }
        break;
    case T_INT32:
        if (bNormalized)
        {
            if (uiComponentCount == 1)
                return F_NORMINT32_1;
            else if (uiComponentCount == 2)
                return F_NORMINT32_2;
            else if (uiComponentCount == 3)
                return F_NORMINT32_3;
            else if (uiComponentCount == 4)
                return F_NORMINT32_4;
        }
        else
        {
            if (uiComponentCount == 1)
                return F_INT32_1;
            else if (uiComponentCount == 2)
                return F_INT32_2;
            else if (uiComponentCount == 3)
                return F_INT32_3;
            else if (uiComponentCount == 4)
                return F_INT32_4;
        }
        break;
    case T_UINT32:
        if (bNormalized)
        {
            if (uiComponentCount == 1)
                return F_NORMUINT32_1;
            else if (uiComponentCount == 2)
                return F_NORMUINT32_2;
            else if (uiComponentCount == 3)
                return F_NORMUINT32_3;
            else if (uiComponentCount == 4)
                return F_NORMUINT32_4;
        }
        else
        {
            if (uiComponentCount == 1)
                return F_UINT32_1;
            else if (uiComponentCount == 2)
                return F_UINT32_2;
            else if (uiComponentCount == 3)
                return F_UINT32_3;
            else if (uiComponentCount == 4)
                return F_UINT32_4;
        }
        break;
    case T_FLOAT16:
        if (uiComponentCount == 1)
            return F_FLOAT16_1;
        else if (uiComponentCount == 2)
            return F_FLOAT16_2;
        else if (uiComponentCount == 3)
            return F_FLOAT16_3;
        else if (uiComponentCount == 4)
            return F_FLOAT16_4;
        break;
    case T_FLOAT32:
        if (uiComponentCount == 1)
            return F_FLOAT32_1;
        else if (uiComponentCount == 2)
            return F_FLOAT32_2;
        else if (uiComponentCount == 3)
            return F_FLOAT32_3;
        else if (uiComponentCount == 4)
            return F_FLOAT32_4;
        break;
    default:
        break;
    }

    return F_UNKNOWN;
}
//---------------------------------------------------------------------------
const char* NiDataStreamElement::GetTypeString(Type eType)
{
    switch (eType)
    {
    case T_INT8: 
        return "INT8";
    case T_UINT8: 
        return "UINT8";
    case T_INT16: 
        return "INT16";
    case T_UINT16: 
        return "UINT16";
    case T_INT32: 
        return "INT32";
    case T_UINT32: 
        return "UINT32";
    case T_FLOAT16: 
        return "FLOAT16";
    case T_FLOAT32: 
        return "FLOAT32";
    default: 
        return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
NiString NiDataStreamElement::GetFormatString() const
{
    const char* pcStr = NULL;

    ENUM_STRING_BEGIN(m_eFormat, pcStr);
    ENUM_STRING_ITEM(F_INT8_1);
    ENUM_STRING_ITEM(F_INT8_2);
    ENUM_STRING_ITEM(F_INT8_3);
    ENUM_STRING_ITEM(F_INT8_4);
    ENUM_STRING_ITEM(F_UINT8_1);
    ENUM_STRING_ITEM(F_UINT8_2);
    ENUM_STRING_ITEM(F_UINT8_3);
    ENUM_STRING_ITEM(F_UINT8_4);
    ENUM_STRING_ITEM(F_NORMINT8_1);
    ENUM_STRING_ITEM(F_NORMINT8_2);
    ENUM_STRING_ITEM(F_NORMINT8_3);
    ENUM_STRING_ITEM(F_NORMINT8_4);
    ENUM_STRING_ITEM(F_NORMUINT8_1);
    ENUM_STRING_ITEM(F_NORMUINT8_2);
    ENUM_STRING_ITEM(F_NORMUINT8_3);
    ENUM_STRING_ITEM(F_NORMUINT8_4);
    ENUM_STRING_ITEM(F_INT16_1);
    ENUM_STRING_ITEM(F_INT16_2);
    ENUM_STRING_ITEM(F_INT16_3);
    ENUM_STRING_ITEM(F_INT16_4);
    ENUM_STRING_ITEM(F_UINT16_1);
    ENUM_STRING_ITEM(F_UINT16_2);
    ENUM_STRING_ITEM(F_UINT16_3);
    ENUM_STRING_ITEM(F_UINT16_4);
    ENUM_STRING_ITEM(F_NORMINT16_1);
    ENUM_STRING_ITEM(F_NORMINT16_2);
    ENUM_STRING_ITEM(F_NORMINT16_3);
    ENUM_STRING_ITEM(F_NORMINT16_4);
    ENUM_STRING_ITEM(F_NORMUINT16_1);
    ENUM_STRING_ITEM(F_NORMUINT16_2);
    ENUM_STRING_ITEM(F_NORMUINT16_3);
    ENUM_STRING_ITEM(F_NORMUINT16_4);
    ENUM_STRING_ITEM(F_INT32_1);
    ENUM_STRING_ITEM(F_INT32_2);
    ENUM_STRING_ITEM(F_INT32_3);
    ENUM_STRING_ITEM(F_INT32_4);
    ENUM_STRING_ITEM(F_UINT32_1);
    ENUM_STRING_ITEM(F_UINT32_2);
    ENUM_STRING_ITEM(F_UINT32_3);
    ENUM_STRING_ITEM(F_UINT32_4);
    ENUM_STRING_ITEM(F_NORMINT32_1);
    ENUM_STRING_ITEM(F_NORMINT32_2);
    ENUM_STRING_ITEM(F_NORMINT32_3);
    ENUM_STRING_ITEM(F_NORMINT32_4);
    ENUM_STRING_ITEM(F_NORMUINT32_1);
    ENUM_STRING_ITEM(F_NORMUINT32_2);
    ENUM_STRING_ITEM(F_NORMUINT32_3);
    ENUM_STRING_ITEM(F_NORMUINT32_4);
    ENUM_STRING_ITEM(F_FLOAT16_1);
    ENUM_STRING_ITEM(F_FLOAT16_2);
    ENUM_STRING_ITEM(F_FLOAT16_3);
    ENUM_STRING_ITEM(F_FLOAT16_4);
    ENUM_STRING_ITEM(F_FLOAT32_1);
    ENUM_STRING_ITEM(F_FLOAT32_2);
    ENUM_STRING_ITEM(F_FLOAT32_3);
    ENUM_STRING_ITEM(F_FLOAT32_4);
    ENUM_STRING_ITEM(F_UINT_10_10_10_L1);
    ENUM_STRING_ITEM(F_NORMINT_10_10_10_L1);
    ENUM_STRING_ITEM(F_NORMINT_11_11_10);
    ENUM_STRING_ITEM(F_NORMUINT8_4_BGRA);
    ENUM_STRING_ITEM(F_NORMINT_10_10_10_2);
    ENUM_STRING_ITEM(F_UINT_10_10_10_2);
    ENUM_STRING_END__ALLOW_MISSING_ENUM(m_eFormat, pcStr);

    NiString kResult;
    if (pcStr)
    {
        kResult = pcStr;
    }
    else
    {
        char acTempString[64];
        NiSprintf(acTempString, 64, "CustomFormat:%Xh", m_eFormat);
        kResult = acTempString;
    }

    return kResult;
}
//---------------------------------------------------------------------------
NiDataStreamElement::Type NiDataStreamElement::TypeFromString(
    const char* pcType)
{
    for (NiUInt32 ui = 0; ui < T__EnumerationCount; ui++)
    {
        if (NiStricmp(pcType, GetTypeString((Type)ui)) == 0)
        {
            return (Type)ui;
        }
    }
    return T_UNKNOWN;
}
//---------------------------------------------------------------------------
NiDataStreamElement::Type NiDataStreamElement::GetType(Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return T_UNKNOWN;

    NiUInt8 uiIndex = NiDataStreamElement::IndexOf(eFormat);
    NIASSERT(uiIndex > 0 && uiIndex < NiDataStreamElement::F_TYPE_COUNT);

    if (gkFormatTable.m_aEntry[uiIndex].m_bPacked)
        return T_UINT32;
    else
        return gkFormatTable.m_aEntry[uiIndex].m_ComponentInfo.m_eType;
}
//---------------------------------------------------------------------------
bool NiDataStreamElement::IsNormalized(NiDataStreamElement::Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return false;

    NiUInt8 uiIndex = NiDataStreamElement::IndexOf(eFormat);
    NIASSERT(uiIndex > 0 && uiIndex < NiDataStreamElement::F_TYPE_COUNT);

    return gkFormatTable.m_aEntry[uiIndex].m_bNormalized;
}
//---------------------------------------------------------------------------
bool NiDataStreamElement::IsPacked(NiDataStreamElement::Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return false;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < NiDataStreamElement::F_TYPE_COUNT);
    return gkFormatTable.m_aEntry[uiIndex].m_bPacked;
}
//---------------------------------------------------------------------------
bool NiDataStreamElement::IsLastFieldLiteral(NiDataStreamElement::Format
    eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return false;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < NiDataStreamElement::F_TYPE_COUNT);
    return gkFormatTable.m_aEntry[uiIndex].m_bLastFieldIsLiteral;
}
//---------------------------------------------------------------------------
bool NiDataStreamElement::IsSwizzled(Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return false;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);
    NiUInt8 uiLayout = gkFormatTable.m_aEntry[uiIndex].m_uiLayout;

    if (uiLayout == L_X || uiLayout == L_XY || uiLayout == L_XYZ ||
        uiLayout == L_XYZW)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiDataStreamElement::Layout NiDataStreamElement::GetLayout(Format eFormat)
{
    if (IsCustom(eFormat))
        return L_UNKNOWN;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);
    return (Layout)gkFormatTable.m_aEntry[uiIndex].m_uiLayout;
}
//---------------------------------------------------------------------------
bool NiDataStreamElement::IsSigned(NiDataStreamElement::Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return false;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);
    return gkFormatTable.m_aEntry[uiIndex].m_bSigned;
}
//---------------------------------------------------------------------------
size_t NiDataStreamElement::SizeOfType(Type eType)
{
    switch (eType)
    {
        case T_INT8: 
            return sizeof(NiInt8);
        case T_UINT8: 
            return sizeof(NiUInt8);
        case T_INT16: 
            return sizeof(NiInt16);
        case T_UINT16: 
            return sizeof(NiUInt16);
        case T_INT32: 
            return sizeof(NiInt32);
        case T_UINT32: 
            return sizeof(NiUInt32);
        case T_FLOAT16:
            return sizeof(NiFloat16);
        case T_FLOAT32: 
            return sizeof(float);
        case T__EnumerationCount:
        case T_UNKNOWN:
        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
size_t NiDataStreamElement::SizeOfFormat(Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return 0;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < NiDataStreamElement::F_TYPE_COUNT);

    // Packed types are always NiUInt32 x 1 component.
    if (gkFormatTable.m_aEntry[uiIndex].m_bPacked)
        return sizeof(NiUInt32);

    NIASSERT(gkFormatTable.m_aEntry[uiIndex].m_ComponentInfo.m_eType !=
        NiDataStreamElement::T_UNKNOWN);

    NiUInt8 uiNumComponents = GetComponentCount(eFormat);
    size_t szSizeOfType = SizeOfType(
        gkFormatTable.m_aEntry[uiIndex].m_ComponentInfo.m_eType);

    return szSizeOfType * uiNumComponents;
}
//---------------------------------------------------------------------------
NiUInt8 NiDataStreamElement::GetComponentCount(Format eFormat)
{
    if (NiDataStreamElement::IsCustom(eFormat))
        return 0;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);

    // Packed types are always NiUInt32 x 1 component.
    if (gkFormatTable.m_aEntry[uiIndex].m_bPacked)
        return 1;

    NiUInt8 uiLayout = gkFormatTable.m_aEntry[uiIndex].m_uiLayout;
    if (uiLayout == L_X)
        return 1;
    if (uiLayout == L_XY || uiLayout == L_YX)
        return 2;
    if (uiLayout >= L_XYZ && uiLayout <= L_ZYX)
        return 3;
    return 4;
}
//---------------------------------------------------------------------------
NiUInt8 NiDataStreamElement::GetComponentSize(Format eFormat)
{
    if (IsCustom(eFormat))
        return 0;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);

    // Packed types are always NiUInt32 x 1 component.
    if (gkFormatTable.m_aEntry[uiIndex].m_bPacked)
        return sizeof(NiUInt32);

    Type type = gkFormatTable.m_aEntry[uiIndex].m_ComponentInfo.m_eType;
    return (NiUInt8)(SizeOfType(type));
}
//---------------------------------------------------------------------------
NiUInt8 NiDataStreamElement::GetFieldCount(Format eFormat)
{
    if (IsCustom(eFormat))
        return 0;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);

    if (!gkFormatTable.m_aEntry[uiIndex].m_bPacked)
        return 0;

    NiUInt8 uiLayout = gkFormatTable.m_aEntry[uiIndex].m_uiLayout;
    if (uiLayout == L_X)
        return 1;
    if (uiLayout == L_XY || uiLayout == L_YX)
        return 2;
    if (uiLayout >= L_XYZ && uiLayout <= L_ZYX)
        return 3;
    return 4;
}
//---------------------------------------------------------------------------
NiUInt8 NiDataStreamElement::GetFieldBits(Format eFormat, NiUInt8 uiField)
{
    if (IsCustom(eFormat))
        return 0;

    if (uiField >= GetFieldCount(eFormat))
        return 0;

    NiUInt8 uiIndex = IndexOf(eFormat);
    NIASSERT(uiIndex < F_TYPE_COUNT);

    NiUInt32 uiDataField =
        gkFormatTable.m_aEntry[uiIndex].m_ComponentInfo.m_uiFields;

    return static_cast<NiUInt8>(
        ((uiDataField >> (8 * uiField)) & 0xFF));
}
//---------------------------------------------------------------------------
