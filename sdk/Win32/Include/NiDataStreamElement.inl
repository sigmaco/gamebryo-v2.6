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
inline NiDataStreamElementSet::NiDataStreamElementSet() 
    : m_uiStride(0)
{
}
//---------------------------------------------------------------------------
inline NiDataStreamElementSet::NiDataStreamElementSet(
    const NiDataStreamElementSet& rhs)
    : NiTObjectSet<NiDataStreamElement>(rhs.GetSize())
    , m_uiStride(rhs.m_uiStride)
{ 
    NiUInt32 uiSize = rhs.GetSize();
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        Add(rhs.GetAt(i));
    }
}
//---------------------------------------------------------------------------
inline NiDataStreamElementSet& NiDataStreamElementSet::operator=(
    const NiDataStreamElementSet& rhs)
{
    RemoveAll();
    NiUInt32 uiSize = rhs.GetSize();
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        Add(rhs.GetAt(i));
    }
    m_uiStride = rhs.m_uiStride;
    return (*this);
}
//---------------------------------------------------------------------------
inline void NiDataStreamElementSet::AddElement(
    NiDataStreamElement::Format eFormat)
{
    NiDataStreamElement kElement;
    kElement.SetFormat(eFormat);
    kElement.SetOffset(m_uiStride);
    m_uiStride =
        (NiUInt32)(m_uiStride + NiDataStreamElement::SizeOfFormat(eFormat));
    Add(kElement);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamElementSet::GetStride() const
{
    return m_uiStride;
}
//---------------------------------------------------------------------------
// DataStreamElement
//---------------------------------------------------------------------------
inline NiDataStreamElement::Format NiDataStreamElement::GetFormat() const
{
    return m_eFormat;
}
//---------------------------------------------------------------------------
inline void NiDataStreamElement::SetFormat(NiDataStreamElement::Format eFormat)
{
    m_eFormat = eFormat;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiDataStreamElement::GetOffset() const
{
    return m_uiOffset;
}
//---------------------------------------------------------------------------
inline void NiDataStreamElement::SetOffset(NiUInt32 uiOffset)
{
    m_uiOffset = uiOffset;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsCustom(NiDataStreamElement::Format eFormat)
{
    return (eFormat & F_CUSTOM_MASK) != 0;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsCustom() const
{
    return IsCustom(m_eFormat);
}
//---------------------------------------------------------------------------
inline size_t NiDataStreamElement::SizeOf() const
{
    return SizeOfFormat(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElement::IndexOf(Format eFormat)
{
    return static_cast<NiUInt8>((eFormat & F_INDEX_MASK) >> F_INDEX_SHIFT);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElement::IndexOf() const
{
    return IndexOf(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElement::GetComponentCount() const
{
    return GetComponentCount(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElement::GetComponentSize() const
{
    return GetComponentSize(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Type NiDataStreamElement::GetType() const
{
    return GetType(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Layout NiDataStreamElement::GetLayout() const
{
    return GetLayout(m_eFormat);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsNormalized() const
{
    return IsNormalized(m_eFormat);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsPacked() const
{
    return IsPacked(m_eFormat);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsLastFieldLiteral() const
{
    return IsLastFieldLiteral(m_eFormat);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsSwizzled() const
{
    return IsSwizzled(m_eFormat);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsSigned() const
{
    return IsSigned(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElement::GetFieldCount() const
{
    return GetFieldCount(m_eFormat);
}
//---------------------------------------------------------------------------
inline NiUInt8 NiDataStreamElement::GetFieldBits(NiUInt8 uiField) const
{
    return GetFieldBits(m_eFormat, uiField);
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsEqual(const NiDataStreamElement& rhs) const
{
    return m_eFormat == rhs.m_eFormat && m_uiOffset == rhs.m_uiOffset;
}
//---------------------------------------------------------------------------
inline bool NiDataStreamElement::IsValid() const
{
    return (m_eFormat != F_UNKNOWN) && (m_uiOffset != UINT_MAX);
}
//---------------------------------------------------------------------------
