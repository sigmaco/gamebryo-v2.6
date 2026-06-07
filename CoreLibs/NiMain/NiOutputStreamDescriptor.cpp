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

#include "NiMainPCH.h"
#include "NiOutputStreamDescriptor.h"
#include <NiBinaryLoadSave.h>

//---------------------------------------------------------------------------
NiOutputStreamDescriptor::VertexFormatEntry::VertexFormatEntry(
    NiUInt32) : 
    NiMemObject(), 
    m_eDataType(NiOutputStreamDescriptor::DATATYPE_MAX), 
    m_uiComponentCount(0), 
    m_kSemanticName(NULL), 
    m_uiSemanticIndex(0xFFFFFFFF)
{
    // Ignore input paramter; it's only provided so 
    // "VertexFormatEntry kEntry = 0;"
    // is valid. This is needed so the class can be in an NiTArray.
}
//---------------------------------------------------------------------------
NiOutputStreamDescriptor::VertexFormatEntry::VertexFormatEntry(
    const NiOutputStreamDescriptor::VertexFormatEntry& kEntry) :
    m_eDataType(kEntry.m_eDataType),
    m_uiComponentCount(kEntry.m_uiComponentCount),
    m_kSemanticName(kEntry.m_kSemanticName),
    m_uiSemanticIndex(kEntry.m_uiSemanticIndex)
{
    /* */
}
//---------------------------------------------------------------------------
NiOutputStreamDescriptor::VertexFormatEntry& 
NiOutputStreamDescriptor::VertexFormatEntry::operator= (
    const NiOutputStreamDescriptor::VertexFormatEntry& kEntry)
{
    m_eDataType = kEntry.m_eDataType;
    m_uiComponentCount = kEntry.m_uiComponentCount;
    m_kSemanticName = kEntry.m_kSemanticName;
    m_uiSemanticIndex = kEntry.m_uiSemanticIndex;
    return *this;
}
//---------------------------------------------------------------------------
bool NiOutputStreamDescriptor::VertexFormatEntry::operator== (
    const NiOutputStreamDescriptor::VertexFormatEntry& kEntry) const
{
    return (m_eDataType == kEntry.m_eDataType &&
        m_uiComponentCount == kEntry.m_uiComponentCount &&
        m_kSemanticName == kEntry.m_kSemanticName &&
        m_uiSemanticIndex == kEntry.m_uiSemanticIndex);
}
//---------------------------------------------------------------------------
bool NiOutputStreamDescriptor::VertexFormatEntry::operator!= (
    const NiOutputStreamDescriptor::VertexFormatEntry& kEntry) const
{
    return !(*this == kEntry);
}
//---------------------------------------------------------------------------
bool NiOutputStreamDescriptor::VertexFormatEntry::SaveBinary(
    NiBinaryStream& kStream) const
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.

    NiFixedString::SaveFixedStringAsCString(kStream, m_kSemanticName);

    NiStreamSaveBinary(kStream, m_uiSemanticIndex);
    NiStreamSaveBinary(kStream, m_uiComponentCount);
    NiBinaryStreamSaveEnum(kStream, m_eDataType);
   
    return true;
}
//---------------------------------------------------------------------------
bool NiOutputStreamDescriptor::VertexFormatEntry::LoadBinary(
    NiBinaryStream& kStream)
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.

    NiFixedString::LoadCStringAsFixedString(kStream, m_kSemanticName);
    
    NiStreamLoadBinary(kStream, m_uiSemanticIndex);
    NiStreamLoadBinary(kStream, m_uiComponentCount);    
    NiBinaryStreamLoadEnum(kStream, &m_eDataType);

    return true;
}

//---------------------------------------------------------------------------
// Implementation of NiOutputStreamDescriptor
//---------------------------------------------------------------------------
NiOutputStreamDescriptor::NiOutputStreamDescriptor(NiUInt32) : 
    NiRefObject(), 
    m_kName(NULL), 
    m_uiMaxVertexCount(0), 
    m_ePrimType(NiPrimitiveType::PRIMITIVE_MAX)
{
    // Ignore input parameter; it's only provided so 
    // "NiOutputStreamDescriptor kDescriptor = 0;" is valid. 
    // This is needed so the class can be in an NiTArray.
}
//---------------------------------------------------------------------------
NiOutputStreamDescriptor::~NiOutputStreamDescriptor()
{
    /* */
}
//---------------------------------------------------------------------------
NiOutputStreamDescriptor::NiOutputStreamDescriptor(
    const NiOutputStreamDescriptor& kDescriptor) : 
    NiRefObject(), 
    m_kName(kDescriptor.m_kName), 
    m_uiMaxVertexCount(kDescriptor.m_uiMaxVertexCount),
    m_ePrimType(kDescriptor.m_ePrimType)    
{
    SetVertexFormat(kDescriptor.m_kVertexFormat);
}
//---------------------------------------------------------------------------
NiOutputStreamDescriptor& NiOutputStreamDescriptor::operator=(
    const NiOutputStreamDescriptor& kDescriptor)
{
    m_kName = kDescriptor.m_kName;
    m_uiMaxVertexCount = kDescriptor.m_uiMaxVertexCount;
    m_ePrimType = kDescriptor.m_ePrimType;
    SetVertexFormat(kDescriptor.m_kVertexFormat);

    return *this;
}
//---------------------------------------------------------------------------
bool NiOutputStreamDescriptor::operator==(
    const NiOutputStreamDescriptor& kDescriptor) const
{
    if (m_kName != kDescriptor.m_kName ||
        m_uiMaxVertexCount != kDescriptor.m_uiMaxVertexCount ||
        m_ePrimType != kDescriptor.m_ePrimType ||
        m_kVertexFormat.GetSize() != kDescriptor.m_kVertexFormat.GetSize())
    {
        return false;
    }

    unsigned int uiCount = m_kVertexFormat.GetSize();
    for (unsigned int i = 0; i < uiCount; i++)
    {
        if (m_kVertexFormat[i] != kDescriptor.m_kVertexFormat[i])
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOutputStreamDescriptor::operator!= (
    const NiOutputStreamDescriptor& kDescriptor) const
{
    return !(*this == kDescriptor);
}
//---------------------------------------------------------------------------
void NiOutputStreamDescriptor::SetVertexFormat(
    const VertexFormat& kVertexFormat)
{
    unsigned int uiNumEntries = kVertexFormat.GetSize();
    m_kVertexFormat.RemoveAll();
    for (unsigned int i = 0; i < uiNumEntries; i++)
        m_kVertexFormat.Add(kVertexFormat[i]);
}
//---------------------------------------------------------------------------
void NiOutputStreamDescriptor::LoadBinary(NiBinaryStream& kStream)
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.    
    
    NiFixedString::LoadCStringAsFixedString(kStream, m_kName);
    NiStreamLoadBinary(kStream, m_uiMaxVertexCount);
    NiBinaryStreamLoadEnum(kStream, &m_ePrimType);

    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    for (unsigned int i = 0; i < uiValue; i++)
    {
        VertexFormatEntry kEntry;
        kEntry.LoadBinary(kStream);
        m_kVertexFormat.Add(kEntry);
    }
}
//---------------------------------------------------------------------------
void NiOutputStreamDescriptor::SaveBinary(NiBinaryStream& kStream) const
{
    // NOTE: This function uses an NiBinaryStream (rather than the
    //   typical NiStream used throughout NiMain) because some NSB*
    //   classes reuse this class.    

    NiFixedString::SaveFixedStringAsCString(kStream, m_kName);
    NiStreamSaveBinary(kStream, m_uiMaxVertexCount);
    NiBinaryStreamSaveEnum(kStream, m_ePrimType);

    unsigned int uiValue = m_kVertexFormat.GetSize();
    NiStreamSaveBinary(kStream, uiValue);
    for (unsigned int i = 0; i < uiValue; i++)
        m_kVertexFormat[i].SaveBinary(kStream);
}
//---------------------------------------------------------------------------
