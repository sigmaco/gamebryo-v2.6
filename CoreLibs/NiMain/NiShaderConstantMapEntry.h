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

#ifndef NISHADERCONSTANTMAPENTRY_H
#define NISHADERCONSTANTMAPENTRY_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiFixedString.h>

#include <NiShaderAttributeDesc.h>

class NIMAIN_ENTRY NiShaderConstantMapEntry : public NiRefObject
{
public:
    // Type of mapping
    enum Mapping
    {
        SCME_MAP_CONSTANT   = 0x10000000,   // Constant value
        SCME_MAP_DEFINED    = 0x20000000,   // Pre-defined mapping
        SCME_MAP_ATTRIBUTE  = 0x30000000,   // Custom attribute mapping
        SCME_MAP_GLOBAL     = 0x40000000,   // Global attribute mapping
        SCME_MAP_OPERATOR   = 0x50000000,   // Operator attribute mapping
        SCME_MAP_OBJECT     = 0x60000000,   // Object mapping
        SCME_MAP_MASK       = 0xF0000000    // Mapping Mask
    };

    enum OperatorType
    {
        SCME_OPERATOR_MULTIPLY          = 0x10000000,
        SCME_OPERATOR_DIVIDE            = 0x20000000,
        SCME_OPERATOR_ADD               = 0x30000000,
        SCME_OPERATOR_SUBTRACT          = 0x40000000,
        SCME_OPERATOR_MASK              = 0xF0000000,
        SCME_OPERATOR_RESULT_INVERSE    = 0x01000000,
        SCME_OPERATOR_RESULT_TRANSPOSE  = 0x02000000,
        SCME_OPERATOR_RESULT_MASK       = 0x0F000000,
        SCME_OPERATOR_ENTRY1_MASK       = 0x00000FFF,
        SCME_OPERATOR_ENTRY2_MASK       = 0x00FFF000,
        SCME_OPERATOR_ENTRY2_SHIFT      = 12
    };

    NiShaderConstantMapEntry();
    NiShaderConstantMapEntry(const NiFixedString& kKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiShaderRegister,
        unsigned int uiRegisterCount, unsigned int uiDataSize = 0,
        unsigned int uiDataStride = 0, void* pvDataSource = 0,
        bool bCopyData = false);
private:
    // Disallow copy constructor
    NiShaderConstantMapEntry(const NiShaderConstantMapEntry&);
public:
    ~NiShaderConstantMapEntry();

    //*** Access functions
    inline const NiFixedString& GetKey() const;
    inline void SetKey(const NiFixedString& kKey);

    inline unsigned int GetFlags() const;
    inline void SetFlags(unsigned int uiFlags);

    inline bool IsConstant() const;
    inline bool IsDefined() const;
    inline bool IsAttribute() const;
    inline bool IsGlobal() const;
    inline bool IsOperator() const;
    inline bool IsObject() const;
    static bool IsConstant(unsigned int uiFlags);
    static bool IsDefined(unsigned int uiFlags);
    static bool IsAttribute(unsigned int uiFlags);
    static bool IsGlobal(unsigned int uiFlags);
    static bool IsOperator(unsigned int uiFlags);
    static bool IsObject(unsigned int uiFlags);

    inline NiShaderAttributeDesc::AttributeType GetAttributeType() const;
    inline NiShaderAttributeDesc::ObjectType GetObjectType() const;
    inline bool IsBool() const;
    inline bool IsString() const;
    inline bool IsUnsignedInt() const;
    inline bool IsFloat() const;
    inline bool IsPoint2() const;
    inline bool IsPoint3() const;
    inline bool IsPoint4() const;
    inline bool IsMatrix3() const;
    inline bool IsMatrix4() const;
    inline bool IsColor() const;
    inline bool IsTexture() const;
    inline bool IsArray() const;
    static bool IsBool(unsigned int uiFlags);
    static bool IsString(unsigned int uiFlags);
    static bool IsUnsignedInt(unsigned int uiFlags);
    static bool IsFloat(unsigned int uiFlags);
    static bool IsPoint2(unsigned int uiFlags);
    static bool IsPoint3(unsigned int uiFlags);
    static bool IsPoint4(unsigned int uiFlags);
    static bool IsMatrix3(unsigned int uiFlags);
    static bool IsMatrix4(unsigned int uiFlags);
    static bool IsColor(unsigned int uiFlags);
    static bool IsTexture(unsigned int uiFlags);
    static bool IsArray(unsigned int uiFlags);

    inline unsigned int GetExtra() const;
    inline void SetExtra(unsigned int uiExtra);

    inline unsigned int GetShaderRegister() const;
    inline void SetShaderRegister(unsigned int uiShaderRegister);
    inline unsigned int GetRegisterCount() const;
    inline void SetRegisterCount(unsigned int uiRegisterCount);
    inline const NiFixedString& GetVariableName() const;
    inline void SetVariableName(const NiFixedString& kVariableName);
    inline void GetRegisterInfo(unsigned int& uiShaderRegister,
        unsigned int& uiRegisterCount, NiFixedString& pcVariableName) const;
    inline void SetRegisterInfo(unsigned int uiShaderRegister, 
        unsigned int uiRegisterCount, const NiFixedString& kVariableName);

    inline unsigned int GetDataSize() const;
    inline unsigned int GetDataStride() const;
    inline void* GetDataSource() const;
    inline void GetData(unsigned int& uiDataSize, unsigned int& uiDataStride, 
        void*& pvDataSource) const;
    inline void SetData(unsigned int uiDataSize, unsigned int uiDataStride, 
        void* pvDataSource, bool bCopyData);

    // *** begin Emergent internal use only
    inline unsigned int GetInternal() const;
    inline void SetInternal(unsigned int uiInternal);

    inline bool GetVariableHookupValid() const;
    inline void SetVariableHookupValid(bool bValid);

    inline bool GetColumnMajor() const;
    inline void SetColumnMajor(bool bColumnMajor);

#if defined(_XENON) || defined(WIN32)
    NiUInt32 GetRegisterSet() const;
    void SetRegisterSet(NiUInt32 uiRegSet);
#endif

    inline static NiShaderAttributeDesc::AttributeType GetAttributeType(
        unsigned int uiFlags);
    static unsigned char GetAttributeFlags(
        NiShaderAttributeDesc::AttributeType eType);
    static unsigned int GetAttributeMask();

    inline static NiShaderAttributeDesc::ObjectType GetObjectType(
        unsigned int uiFlags);
    static unsigned short GetObjectFlags(
        NiShaderAttributeDesc::ObjectType eType);
    static unsigned int GetObjectMask();

    static void _SDMInit();
    // *** end Emergent internal use only

protected:
    // The name of the attribute this constant is mapped to,
    // or the name of the pre-defined/constant entry.
    NiFixedString m_kKey;

    // Internal value
    unsigned int m_uiInternal;

    // Flags contains multiple sets of information
    // POSITION
    //  The position is contained in the lower 4 bits, and it indicates 
    //  the constant register components to fill in with the data. For 
    //  example, SCME_POS_XY indicates the data should be put into only the 
    //  X and Y components of the register. The default for this value is 
    //  SCME_POS_XYZW - all components.
    // MAPPING
    //  The top 4 bits of the data represent the type, using the SCME_MAP_XXX
    //  flags. The allowed types are CONSTANT, DEFINED, and ATTRIBUTE.
    //
    unsigned int m_uiFlags;

    // Extra data. Used for misc. things, such as the bone index for the case
    // of the BoneMatrix pre-defined constant. Could also be used as an index
    // for which light to use in those pre-defined cases.
    unsigned int m_uiExtra;

    // The register the data is to be packed into.
    unsigned int m_uiShaderRegister;
    // The number of registers required for the data to fit.
    unsigned int m_uiRegisterCount;
    // The name of the variable the data will be packed into.
    NiFixedString m_kVariableName;

    // The data itself - only used in CONSTANT and some DEFINED mappings.
    // If the data is a constant, the entry will delete, otherwise, it is
    // left up to the one who set it.
    unsigned int m_uiDataSize;      // Size of the data element
    unsigned int m_uiDataStride;    // Stride between data elements
    void* m_pvDataSource;           // Pointer to source of data

    bool m_bOwnData;
    bool m_bVariableHookupValid;
    bool m_bColumnMajor;
    
#if defined(_XENON) || defined(WIN32)
    NiUInt8 m_ucRegisterSet;
#endif

    enum
    {
        SCME_ATTRIB_TYPE_MASK = 0x000000FF,
        SCME_OBJECT_TYPE_MASK = 0x0000FF00,
        SCME_OBJECT_TYPE_SHIFT = 8
    };

    static void InitializeAttribTable();
    static NiShaderAttributeDesc::AttributeType ms_aeAttribType[
        NiShaderAttributeDesc::ATTRIB_TYPE_COUNT];
    static unsigned char ms_aucAttribFlags[
        NiShaderAttributeDesc::ATTRIB_TYPE_COUNT];
    static bool ms_bAttribTableInitialized;

    static void InitializeObjectTable();
    static NiShaderAttributeDesc::ObjectType ms_aeObjectType[
        NiShaderAttributeDesc::OT_COUNT];
    static unsigned short ms_ausObjectFlags[
        NiShaderAttributeDesc::OT_COUNT];
    static bool ms_bObjectTableInitialized;
};

typedef NiPointer<NiShaderConstantMapEntry> 
    NiShaderConstantMapEntryPtr;

#include "NiShaderConstantMapEntry.inl"

#endif  //#ifndef NISHADERCONSTANTMAPENTRY_H
