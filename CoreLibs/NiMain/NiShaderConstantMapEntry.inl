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
//---------------------------------------------------------------------------
// NiShaderConstantMapEntry inline functions
//---------------------------------------------------------------------------
inline NiShaderConstantMapEntry::NiShaderConstantMapEntry() :
    m_uiFlags(0), 
    m_uiExtra(0), 
    m_uiShaderRegister(0), 
    m_uiRegisterCount(0), 
    m_uiDataSize(0), 
    m_uiDataStride(0), 
    m_pvDataSource(0), 
    m_bOwnData(false),
    m_bVariableHookupValid(false),
    m_bColumnMajor(false)
#if defined(_XENON) || defined(WIN32)
    , m_ucRegisterSet(0xFF)
#endif
{
}
//---------------------------------------------------------------------------
inline NiShaderConstantMapEntry::NiShaderConstantMapEntry(
    const NiFixedString& kKey, unsigned int uiFlags, unsigned int uiExtra, 
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    unsigned int uiDataSize, unsigned int uiDataStride, 
    void* pvDataSource, bool bCopyData) :
    m_kKey(kKey), 
    m_uiFlags(0), 
    m_uiExtra(0), 
    m_uiShaderRegister(0), 
    m_uiRegisterCount(0), 
    m_uiDataSize(0), 
    m_uiDataStride(0), 
    m_pvDataSource(0), 
    m_bOwnData(false),
    m_bVariableHookupValid(false),
    m_bColumnMajor(false)
#if defined(_XENON) || defined(WIN32)
    , m_ucRegisterSet(0xFF)
#endif
{
    m_uiFlags = uiFlags;
    m_uiExtra = uiExtra;
    m_uiShaderRegister = uiShaderRegister;
    m_uiRegisterCount = uiRegisterCount;
    SetData(uiDataSize, uiDataStride, pvDataSource, bCopyData);
}    
//---------------------------------------------------------------------------
inline NiShaderConstantMapEntry::~NiShaderConstantMapEntry()
{
    if (m_bOwnData)
        NiFree(m_pvDataSource);
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiShaderConstantMapEntry::GetKey() const
{
    return m_kKey;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetKey(const NiFixedString& kKey)
{
    m_kKey = kKey;
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsConstant() const
{
    return ((m_uiFlags & SCME_MAP_MASK) == SCME_MAP_CONSTANT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsDefined() const
{
    return ((m_uiFlags & SCME_MAP_MASK) == SCME_MAP_DEFINED);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsAttribute() const
{
    return ((m_uiFlags & SCME_MAP_MASK) == SCME_MAP_ATTRIBUTE);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsGlobal() const
{
    return ((m_uiFlags & SCME_MAP_MASK) == SCME_MAP_GLOBAL);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsOperator() const
{
    return ((m_uiFlags & SCME_MAP_MASK) == SCME_MAP_OPERATOR);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsObject() const
{
    return ((m_uiFlags & SCME_MAP_MASK) == SCME_MAP_OBJECT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsConstant(unsigned int uiFlags)
{
    return ((uiFlags & SCME_MAP_MASK) == SCME_MAP_CONSTANT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsDefined(unsigned int uiFlags)
{
    return ((uiFlags & SCME_MAP_MASK) == SCME_MAP_DEFINED);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsAttribute(unsigned int uiFlags)
{
    return ((uiFlags & SCME_MAP_MASK) == SCME_MAP_ATTRIBUTE);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsGlobal(unsigned int uiFlags)
{
    return ((uiFlags & SCME_MAP_MASK) == SCME_MAP_GLOBAL);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsOperator(unsigned int uiFlags)
{
    return ((uiFlags & SCME_MAP_MASK) == SCME_MAP_OPERATOR);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsObject(unsigned int uiFlags)
{
    return ((uiFlags & SCME_MAP_MASK) == SCME_MAP_OBJECT);
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::AttributeType 
    NiShaderConstantMapEntry::GetAttributeType() const
{
    return GetAttributeType(m_uiFlags);
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::ObjectType
    NiShaderConstantMapEntry::GetObjectType() const
{
    return GetObjectType(m_uiFlags);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsBool() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_BOOL);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsString() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_STRING);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsUnsignedInt() const
{
    return (GetAttributeType() == 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsFloat() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsPoint2() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsPoint3() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_POINT3);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsPoint4() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_POINT4);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsMatrix3() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsMatrix4() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsColor() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_COLOR);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsTexture() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsArray() const
{
    return (GetAttributeType() == NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsBool(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsString(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_STRING);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsUnsignedInt(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsFloat(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsPoint2(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT2);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsPoint3(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT3);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsPoint4(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_POINT4);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsMatrix3(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsMatrix4(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsColor(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsTexture(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE);
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::IsArray(unsigned int uiFlags)
{
    return (GetAttributeType(uiFlags) == 
        NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY);
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetExtra() const
{
    return m_uiExtra;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetExtra(unsigned int uiExtra)
{
    m_uiExtra = uiExtra;
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetShaderRegister() const
{
    return m_uiShaderRegister;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetShaderRegister(
    unsigned int uiShaderRegister)
{
    m_uiShaderRegister = uiShaderRegister;
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetRegisterCount() const
{
    return m_uiRegisterCount;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetRegisterCount(
    unsigned int uiRegisterCount)
{
    m_uiRegisterCount = uiRegisterCount;
}
//---------------------------------------------------------------------------
inline const NiFixedString& 
NiShaderConstantMapEntry::GetVariableName() const
{
    return m_kVariableName;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetVariableName(
    const NiFixedString& kVariableName)
{
    m_kVariableName = kVariableName;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::GetRegisterInfo(
    unsigned int& uiShaderRegister, unsigned int& uiRegisterCount, 
    NiFixedString& kVariableName) const
{
    uiShaderRegister = m_uiShaderRegister;
    uiRegisterCount = m_uiRegisterCount;
    kVariableName = m_kVariableName;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetRegisterInfo(
    unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
    const NiFixedString& kVariableName)
{
    m_uiShaderRegister = uiShaderRegister;
    m_uiRegisterCount = uiRegisterCount;
    SetVariableName(kVariableName);
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetDataSize() const
{
    return m_uiDataSize;
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetDataStride() const
{
    return m_uiDataStride;
}
//---------------------------------------------------------------------------
inline void* NiShaderConstantMapEntry::GetDataSource() const
{
    return m_pvDataSource;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::GetData(unsigned int& uiDataSize, 
    unsigned int& uiDataStride, void*& pvDataSource) const
{
    uiDataSize = m_uiDataSize;
    uiDataStride = m_uiDataStride;
    pvDataSource = m_pvDataSource;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetData(unsigned int uiDataSize, 
    unsigned int uiDataStride, void* pvDataSource, bool bCopyData)
{
    m_uiDataSize = uiDataSize;
    m_uiDataStride = uiDataStride;

    if (bCopyData)
    {
        // Make a copy of the data.
        m_bOwnData = true;
        NiFree(m_pvDataSource);
        m_pvDataSource = NiAlloc(unsigned char, uiDataSize);
        NIASSERT(m_pvDataSource);
        unsigned int uiByteSize = uiDataSize * sizeof(unsigned char);
        NiMemcpy(m_pvDataSource, pvDataSource, uiByteSize);
    }
    else
    {
        m_bOwnData = false;
        m_pvDataSource = pvDataSource;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetInternal() const
{
    return m_uiInternal;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetInternal(unsigned int uiInternal)
{
    m_uiInternal = uiInternal;
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::AttributeType 
    NiShaderConstantMapEntry::GetAttributeType(unsigned int uiFlags)
{
    NIASSERT((uiFlags & SCME_ATTRIB_TYPE_MASK) < 
        NiShaderAttributeDesc::ATTRIB_TYPE_COUNT);

    NIASSERT(ms_bAttribTableInitialized);

    return ms_aeAttribType[uiFlags & SCME_ATTRIB_TYPE_MASK];
}
//---------------------------------------------------------------------------
inline unsigned char NiShaderConstantMapEntry::GetAttributeFlags(
    NiShaderAttributeDesc::AttributeType eType)
{
    return ms_aucAttribFlags[(unsigned int)eType];
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetAttributeMask()
{
    return SCME_ATTRIB_TYPE_MASK;
}
//---------------------------------------------------------------------------
inline NiShaderAttributeDesc::ObjectType
    NiShaderConstantMapEntry::GetObjectType(unsigned int uiFlags)
{
    unsigned int uiIndex = (uiFlags & SCME_OBJECT_TYPE_MASK) >>
        SCME_OBJECT_TYPE_SHIFT;
    NIASSERT(uiIndex < NiShaderAttributeDesc::OT_COUNT);

    NIASSERT(ms_bObjectTableInitialized);

    return ms_aeObjectType[uiIndex];
}
//---------------------------------------------------------------------------
inline unsigned short NiShaderConstantMapEntry::GetObjectFlags(
    NiShaderAttributeDesc::ObjectType eType)
{
    return ms_ausObjectFlags[(unsigned int) eType];
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderConstantMapEntry::GetObjectMask()
{
    return SCME_OBJECT_TYPE_MASK;
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::GetVariableHookupValid() const
{
    return m_bVariableHookupValid;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetVariableHookupValid(bool bValid)
{
    m_bVariableHookupValid = bValid;
}
//---------------------------------------------------------------------------
inline bool NiShaderConstantMapEntry::GetColumnMajor() const
{
    return m_bColumnMajor;
}
//---------------------------------------------------------------------------
inline void NiShaderConstantMapEntry::SetColumnMajor(bool bColumnMajor)
{
    m_bColumnMajor = bColumnMajor;
}
//---------------------------------------------------------------------------
#if defined(_XENON) || defined(WIN32)
inline NiUInt32 NiShaderConstantMapEntry::GetRegisterSet() const
{
    return m_ucRegisterSet;
}
#endif
//---------------------------------------------------------------------------
#if defined(_XENON) || defined(WIN32)
inline void NiShaderConstantMapEntry::SetRegisterSet(NiUInt32 uiRegSet)
{
    NIASSERT(uiRegSet < 256);
    m_ucRegisterSet = (NiUInt8)uiRegSet;
}
#endif
//---------------------------------------------------------------------------
