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

#ifndef NIDATASTREAMELEMENT_H
#define NIDATASTREAMELEMENT_H

#include "NiMeshLibType.h"
#include <NiMemObject.h>
#include <NiString.h>
#include <NiTSet.h>
#include <NiUniversalTypes.h>

/// Helps build Vertex Element Format
#define NIENCODE_DSEF(i, count, size) \
    (((count << F_COMP_COUNT_SHIFT) & F_COMP_COUNT_MASK) | \
    ((size << F_COMP_SIZE_SHIFT) & F_COMP_SIZE_MASK) | \
    ((i << F_INDEX_SHIFT) & F_INDEX_MASK))

/**
    Describes an element of a NiDataStream object, 
    by data type, format, and offset.
*/
class NIMESH_ENTRY NiDataStreamElement : public NiMemObject
{
public:

    /// Primitive C++ type of this element.
    enum Type
    {
        /// Invalid/Indeterminate type.
        T_UNKNOWN = 0,

        /// Signed 8-bit integer (NiInt8).
        T_INT8,

        /// Unsigned 8-bit integer (NiUInt8).
        T_UINT8,

        /// Signed 16-bit integer (NiInt16).
        T_INT16,

        /// Unsigned 16-bit integer (NiUInt16).
        T_UINT16,

        /// Signed 32-bit integer (NiInt32).
        T_INT32,

        /// Unsigned 32-bit integer (NiUInt32).
        T_UINT32,

        /// 16-bit floating point value (No type exists).
        T_FLOAT16,

        /// 32-bit floating point value (float).
        T_FLOAT32,

        /// Maximum Type.
        T__EnumerationCount
    };

    /**
        Component count, size in bytes, type, and custom data for this 
        element.
        
        Fixed point (integer) types are defined for each of the following:
            - 8bit, 16bit, 32bit
            - 1, 2, 3, 4 components
            - signed, unsigned interpretation
            - unnormalized, normalized interpretation

        Signed Float types are defined for each of the following:
            - 16bit, 32bit
            - 1, 2, 3, 4 components
        
        Additional packed types are defined. Component count is 1 for user
        code but expanded when decompressed.
        
        @note These values are subject to change in future releases. 
        Applications should not depend on the contents of 
        these enumerations to be invariant across releases.
    */
    enum Format
    {
        // If updating, also update NiDataStreamElement::GetFormatString()

        /// Custom type bit. Bit 31 (Most Significant Bit). 
        F_CUSTOM_SHIFT      = 31,
        /// Masks off most significant bit.
        F_CUSTOM_MASK       = 0x80000000,

        /// Component count in bytes. Bits 16-23.
        F_COMP_COUNT_SHIFT  = 16,
        /// Masks off most bits 16-23.
        F_COMP_COUNT_MASK   = 0x00FF0000,

        /// Component type size in bytes. Bits 8-15.
        F_COMP_SIZE_SHIFT   = 8,
        /// Masks off most bits 8-15.
        F_COMP_SIZE_MASK    = 0x0000FF00,

        /// Component type index. 
        /// Allows table lookups indexed by these enumerations. Bits 0-7.
        F_INDEX_SHIFT       = 0,
        /// Masks off most bits 0-7.
        F_INDEX_MASK        = 0x000000FF,

        /// Unknown, or don't care, format.
        F_UNKNOWN                  = NIENCODE_DSEF(0, 0, 0),

        F_INT8_1                   = NIENCODE_DSEF(1, 1, 1),
        F_INT8_2                   = NIENCODE_DSEF(2, 2, 1),
        F_INT8_3                   = NIENCODE_DSEF(3, 3, 1),
        F_INT8_4                   = NIENCODE_DSEF(4, 4, 1),
        F_UINT8_1                  = NIENCODE_DSEF(5, 1, 1),
        F_UINT8_2                  = NIENCODE_DSEF(6, 2, 1),
        F_UINT8_3                  = NIENCODE_DSEF(7, 3, 1),
        F_UINT8_4                  = NIENCODE_DSEF(8, 4, 1),
        F_NORMINT8_1               = NIENCODE_DSEF(9, 1, 1),
        F_NORMINT8_2               = NIENCODE_DSEF(10, 2, 1),
        F_NORMINT8_3               = NIENCODE_DSEF(11, 3, 1),
        F_NORMINT8_4               = NIENCODE_DSEF(12, 4, 1),
        F_NORMUINT8_1              = NIENCODE_DSEF(13, 1, 1),
        F_NORMUINT8_2              = NIENCODE_DSEF(14, 2, 1),
        F_NORMUINT8_3              = NIENCODE_DSEF(15, 3, 1),
        F_NORMUINT8_4              = NIENCODE_DSEF(16, 4, 1),

        F_INT16_1                  = NIENCODE_DSEF(17, 1, 2),
        F_INT16_2                  = NIENCODE_DSEF(18, 2, 2),
        F_INT16_3                  = NIENCODE_DSEF(19, 3, 2),
        F_INT16_4                  = NIENCODE_DSEF(20, 4, 2),
        F_UINT16_1                 = NIENCODE_DSEF(21, 1, 2),
        F_UINT16_2                 = NIENCODE_DSEF(22, 2, 2),
        F_UINT16_3                 = NIENCODE_DSEF(23, 3, 2),
        F_UINT16_4                 = NIENCODE_DSEF(24, 4, 2),
        F_NORMINT16_1              = NIENCODE_DSEF(25, 1, 2),
        F_NORMINT16_2              = NIENCODE_DSEF(26, 2, 2),
        F_NORMINT16_3              = NIENCODE_DSEF(27, 3, 2),
        F_NORMINT16_4              = NIENCODE_DSEF(28, 4, 2),
        F_NORMUINT16_1             = NIENCODE_DSEF(29, 1, 2),
        F_NORMUINT16_2             = NIENCODE_DSEF(30, 2, 2),
        F_NORMUINT16_3             = NIENCODE_DSEF(31, 3, 2),
        F_NORMUINT16_4             = NIENCODE_DSEF(32, 4, 2),

        F_INT32_1                  = NIENCODE_DSEF(33, 1, 4),
        F_INT32_2                  = NIENCODE_DSEF(34, 2, 4),
        F_INT32_3                  = NIENCODE_DSEF(35, 3, 4),
        F_INT32_4                  = NIENCODE_DSEF(36, 4, 4),
        F_UINT32_1                 = NIENCODE_DSEF(37, 1, 4),
        F_UINT32_2                 = NIENCODE_DSEF(38, 2, 4),
        F_UINT32_3                 = NIENCODE_DSEF(39, 3, 4),
        F_UINT32_4                 = NIENCODE_DSEF(40, 4, 4),
        F_NORMINT32_1              = NIENCODE_DSEF(41, 1, 4),
        F_NORMINT32_2              = NIENCODE_DSEF(42, 2, 4),
        F_NORMINT32_3              = NIENCODE_DSEF(43, 3, 4),
        F_NORMINT32_4              = NIENCODE_DSEF(44, 4, 4),
        F_NORMUINT32_1             = NIENCODE_DSEF(45, 1, 4),
        F_NORMUINT32_2             = NIENCODE_DSEF(46, 2, 4),
        F_NORMUINT32_3             = NIENCODE_DSEF(47, 3, 4),
        F_NORMUINT32_4             = NIENCODE_DSEF(48, 4, 4),

        
        F_FLOAT16_1                = NIENCODE_DSEF(49, 1, 2),
        F_FLOAT16_2                = NIENCODE_DSEF(50, 2, 2),
        F_FLOAT16_3                = NIENCODE_DSEF(51, 3, 2),
        F_FLOAT16_4                = NIENCODE_DSEF(52, 4, 2),
        F_FLOAT32_1                = NIENCODE_DSEF(53, 1, 4),
        F_FLOAT32_2                = NIENCODE_DSEF(54, 2, 4),
        F_FLOAT32_3                = NIENCODE_DSEF(55, 3, 4),
        F_FLOAT32_4                = NIENCODE_DSEF(56, 4, 4),

        // Additional types; component count is 1.

        /**
            Fixed point packed type. Component count is returned as 1. 
            When decompressed, it will expand to x,y,z,1. The first three
            components each use 10 bits and are interpreted as an unsigned
            integer. The L1 designation indicates that despite 2 unused bits,
            the fourth component is always a literal value of 1.
        */
        F_UINT_10_10_10_L1         = NIENCODE_DSEF(57, 1, 4),

        /**
            Fixed point packed type. Component count is treated as 1 in user
            code. When decompressed, it will expand to x,y,z,1. The first three
            components each use 10 bits and are interpreted as a normalized
            integer between -1 and 1. The L1 designation indicates that 
            despite 2 unused bits, the fourth component is always a literal 
            value of 1.
        */
        F_NORMINT_10_10_10_L1      = NIENCODE_DSEF(58, 1, 4),

        /**
            Fixed point packed type. Component count is treated as 1 in user 
            code. When decompressed, it will expand to x,y,z. The three
            components use 11, 11, and 10 bits respectively and are 
            interpreted as a normalized integer between -1 and 1.
        */
        F_NORMINT_11_11_10         = NIENCODE_DSEF(59, 1, 4),

        /// F_NORMUINT8_4_BGRA same as F_NORMUINT8_4 where XYZW -> ZYXW.
        F_NORMUINT8_4_BGRA         = NIENCODE_DSEF(60, 4, 1),

        /**
            Fixed point packed type. Component count is treated as 1 in user
            code. When decompressed, it will expand to x,y,z, w. The first 
            three components each use 10 bits with the fourth component using 
            the remaining 2 bits. Each component is interpreted as a 
            normalized integer between -1 and 1.
        */
        F_NORMINT_10_10_10_2       = NIENCODE_DSEF(61, 1, 4),

        /**
            Fixed point packed type. Component count is returned as 1 in user
            code. When decompressed, it will expand to x,y,z, w. The first 
            three components each use 10 bits with the fourth component using 
            the remaining 2 bits. Each component is interpreted as an 
            unsigned integer.
        */
        F_UINT_10_10_10_2          = NIENCODE_DSEF(62, 1, 4),

        /// update F_TYPE_COUNT (must be < 256) when adding standard types.
        F_TYPE_COUNT = 63,

        /// Max component size. 32 bits (4 bytes) x 4 components = 16 bytes.
        F_MAX_COMP_SIZE = 16
    };

    /// Enumerates the layout of a predefined format.
    enum Layout
    {
        L_X = 0,
        L_XY,
        L_YX,
        L_XYZ,
        L_YXZ,
        L_XZY,
        L_ZXY,
        L_YZX,
        L_ZYX,
        L_XYZW,
        L_YXZW,
        L_XZYW,
        L_ZXYW,
        L_YZXW,
        L_ZYXW,
        L_XYWZ,
        L_YXWZ,
        L_XWYZ,
        L_WXYZ,
        L_YWXZ,
        L_WYXZ,
        L_XWZY,
        L_WXZY,
        L_XZWY,
        L_ZXWY,
        L_WZXY,
        L_ZWXY,
        L_WYZX,
        L_YWZX,
        L_WZYX,
        L_ZWYX,
        L_YZWX,
        L_ZYWX,
        L_UNKNOWN
    };

    /// Get the element format.
    Format GetFormat() const;

    /// Set the element format.
    inline void SetFormat(Format eFormat);

    /// Retrieves the layout of a predefined format. If the format is not
    /// one of the predefined formats, L_UNKNOWN is returned.
    static Layout GetLayout(Format eFormat);

    /// Retrieves the layout of a predefined format. If the format is not
    /// one of the predefined formats, L_UNKNOWN is returned.
    Layout GetLayout() const;

    /**
        Get the offset in bytes to the first instance of this element in the 
        data stream.
        
        Can be used with NiDataStream::GetStride to get the offset
        to the nth instance of this element as: GetStride() x n + GetOffset().
    **/
    inline NiUInt32 GetOffset() const;

    /// Set the offset in bytes to the first instance of this element in the 
    /// data stream.
    inline void SetOffset(NiUInt32 uiOffset);

    /// Returns the predefined packed format matching the description. No
    /// swizzling is assumed. If no predefined format can be matched, F_UNKNOWN
    /// will be returned.
    static Format GetPredefinedPackedFormat(bool bNormalized, bool bSigned,
        bool bLastFieldIsLiteral, NiUInt8 uiF0, NiUInt8 uiF1,
        NiUInt8 uiF2 = 0, NiUInt8 uiF3 = 0);

    /// Returns the predefined type matching the description. If no predefined
    /// format can be matched, F_UNKNOWN will be returned.
    static Format GetPredefinedFormat(Type eType, NiUInt8 uiComponentCount, 
        bool bNormalized, Layout eLayout);

    /// Returns the predefined type matching the description. No swizzling
    /// is assumed. If no predefined format can be matched, F_UNKNOWN will
    /// be returned.
    static Format GetPredefinedFormat(Type eType, NiUInt8 uiComponentCount,
        bool bNormalized);

    /// Returns true if the predefined format is packed and false otherwise.
    static bool IsPacked(Format eFormat);

    /// Returns true if the predefined format is packed and false otherwise.
    inline bool IsPacked() const;

    /// If the format is packed, returns the number of fields. Otherwise, a
    /// value of 0 is returned.
    static NiUInt8 GetFieldCount(Format eFormat);

    /// If the format is packed, returns the number of fields. Otherwise, a
    /// value of 0 is returned.
    inline NiUInt8 GetFieldCount() const;

    /// If the format is packed, returns the number of bits allocated to the
    /// field specified. If the format is not packed, not one of the predefined
    /// formats, or if the field specified is out of range, 0 is returned.
    static NiUInt8 GetFieldBits(Format eFormat, NiUInt8 uiField);

    /// If the format is packed, returns the number of bits allocated to the
    /// field specified. If the format is not packed, not one of the predefined
    /// formats, or if the field specified is out of range, 0 is returned.
    inline NiUInt8 GetFieldBits(NiUInt8 uiField) const;

    /// Returns true if the format is one of the predefined swizzle formats.
    /// In all other cases, false is returned.
    static bool IsSwizzled(Format eFormat);

    /// Returns true if the format is one of the predefined swizzle formats.
    /// In all other cases, false is returned.
    inline bool IsSwizzled() const;

    /// Returns true if the last field is forced to literal value of 1
    static bool IsLastFieldLiteral(Format eFormat);

    /// Returns true if the last field is forced to literal value of 1
    inline bool IsLastFieldLiteral() const;

    /// Returns true if the predefined format is signed and false otherwise.
    static bool IsSigned(Format eFormat);

    /// Returns true if the predefined format is signed and false otherwise.
    inline bool IsSigned() const;

    /// Converts the given Type to a string. Valid for predefined types.
    static const char* GetTypeString(Type eType);

    /// Converts the Format to a string.
    ///
    /// Returns predefined enumeration name or hex code for custom types.
    NiString GetFormatString() const;

    /// Converts the given Format from a string.
    ///
    /// Valid for predefined types.
    static Type TypeFromString(const char* pcType);

    /// Returns the size in bytes represented by the input type.
    static size_t SizeOfType(Type eType);

    /// Returns the size in bytes represented by the input format.
    ///
    /// Valid for predefined or custom formats.
    static size_t SizeOfFormat(Format eFormat);

    /// Returns the size in bytes.
    inline size_t SizeOf() const;

    /// Returns the index associated with the format.
    ///
    /// Valid for predefined formats.
    static NiUInt8 IndexOf(Format eFormat);

    /// Returns the index associated with the format.
    inline NiUInt8 IndexOf() const;

    /// Returns true if the custom bit is set.
    static bool IsCustom(Format eFormat);

    /// Returns true if the custom bit is set.
    inline bool IsCustom() const;

    /// Returns the number of components represented by the format.
    ///
    /// Valid for predefined or custom formats.
    static NiUInt8 GetComponentCount(Format eFormat);

    /// Returns the number of components represented by the format.
    inline NiUInt8 GetComponentCount() const;


    /// Returns the size of one component in the format. 
    //
    /// Valid for predefined or custom format.
    static NiUInt8 GetComponentSize(Format eFormat);

    /// Returns the size of one component in the format.
    inline NiUInt8 GetComponentSize() const;

    /// Gets the base type for the format.
    static Type GetType(Format eFormat);

    /// Gets the base type for the format.
    Type GetType() const;

    /// Gets whether the format is normalized.
    static bool IsNormalized(Format eFormat);

    /// Gets whether the format is normalized.
    inline bool IsNormalized() const;

    /// Compares elements for equality.
    inline bool IsEqual(const NiDataStreamElement& rhs) const;

    /// Returns true if the format of the element is valid and not F_UNKNOWN,
    /// false otherwise.
    inline bool IsValid() const;

    /// Default constructor, sets all internal data to invalid values.
    NiDataStreamElement();

    /// Invalid data stream element.
    ///
    /// Useful as error sentinel return value.
    static const NiDataStreamElement ms_kInvalidElement;

protected:

    /// Offset in bytes to the first instance of this element in the data 
    /// stream.
    NiUInt32 m_uiOffset;

    /// Format of this element.
    Format m_eFormat;

};

/**
    Helper collection class for containing NiDataStreamElements.
*/
class NiDataStreamElementSet : public NiTObjectSet<NiDataStreamElement>
{
public:
    /// Default Constructor.
    NiDataStreamElementSet();

    /// Copy Constructor.
    NiDataStreamElementSet(const NiDataStreamElementSet&);

    /// Assignment operator.
    inline NiDataStreamElementSet& operator=(const NiDataStreamElementSet&);

    /// Adds an element to the set.
    inline void AddElement(NiDataStreamElement::Format eFormat);
    
    /// Get the stride of the elements in the set.
    inline NiUInt32 GetStride() const;

    /// Stride of the elements in the set.
    NiUInt32 m_uiStride;     
};
 
#include "NiDataStreamElement.inl"

#endif // #ifndef NIDATASTREAMELEMENT_H
