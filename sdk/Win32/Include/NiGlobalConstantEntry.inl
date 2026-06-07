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
// NiGlobalConstantEntry inline functions
//---------------------------------------------------------------------------
inline NiGlobalConstantEntry::NiGlobalConstantEntry() :
    m_eType(NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED), 
    m_uiDataSize(0), 
    m_pvDataSource(0)
{
}
//---------------------------------------------------------------------------
inline NiGlobalConstantEntry::NiGlobalConstantEntry(const char* pcKey, 
    NiShaderAttributeDesc::AttributeType eType, 
    unsigned int uiDataSize, const void* pvDataSource) :
    m_eType(NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED), 
    m_uiDataSize(0), 
    m_pvDataSource(0)
{
    SetKey(pcKey);
    SetType(eType);
    SetData(uiDataSize, pvDataSource);
}
//---------------------------------------------------------------------------
inline NiGlobalConstantEntry::~NiGlobalConstantEntry()
{
    NiFree(m_pvDataSource);
}
//---------------------------------------------------------------------------
inline NiFixedString NiGlobalConstantEntry::GetKey() 
{
    return m_kKey;
}
//---------------------------------------------------------------------------
inline void NiGlobalConstantEntry::SetKey(const NiFixedString& kKey)
{
    m_kKey = kKey;
}
//---------------------------------------------------------------------------
inline const NiShaderAttributeDesc::AttributeType 
    NiGlobalConstantEntry::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline void NiGlobalConstantEntry::SetType(
    NiShaderAttributeDesc::AttributeType eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsBool() const
{
    return NiShaderConstantMapEntry::IsBool((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsString() const
{
    return NiShaderConstantMapEntry::IsString((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsUnsignedInt() const
{
    return NiShaderConstantMapEntry::IsUnsignedInt((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsFloat() const
{
    return NiShaderConstantMapEntry::IsFloat((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsPoint2() const
{
    return NiShaderConstantMapEntry::IsPoint2((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsPoint3() const
{
    return NiShaderConstantMapEntry::IsPoint3((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsPoint4() const
{
    return NiShaderConstantMapEntry::IsPoint4((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsMatrix3() const
{
    return NiShaderConstantMapEntry::IsMatrix3((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsMatrix4() const
{
    return NiShaderConstantMapEntry::IsMatrix4((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsColor() const
{
    return NiShaderConstantMapEntry::IsColor((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline bool NiGlobalConstantEntry::IsTexture() const
{
    return NiShaderConstantMapEntry::IsTexture((unsigned int)m_eType);
}
//---------------------------------------------------------------------------
inline unsigned int NiGlobalConstantEntry::GetDataSize() const
{
    return m_uiDataSize;
}
//---------------------------------------------------------------------------
inline void* NiGlobalConstantEntry::GetDataSource() const
{
    return m_pvDataSource;
}
//---------------------------------------------------------------------------
inline void NiGlobalConstantEntry::GetData(unsigned int& uiDataSize, 
    const void*& pvDataSource) const
{
    uiDataSize = m_uiDataSize;
    pvDataSource = m_pvDataSource;
}
//---------------------------------------------------------------------------
