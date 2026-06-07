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
inline const NiFixedString& NiOutputStreamDescriptor::GetName() const
{ 
    return m_kName;           
}
//---------------------------------------------------------------------------
inline unsigned int NiOutputStreamDescriptor::GetMaxVertexCount() const
{ 
    return m_uiMaxVertexCount; 
}
//---------------------------------------------------------------------------
inline NiPrimitiveType::Type NiOutputStreamDescriptor::GetPrimType() const
{ 
    return m_ePrimType;        
}
//---------------------------------------------------------------------------
inline const NiOutputStreamDescriptor::VertexFormat& 
    NiOutputStreamDescriptor::GetVertexFormat() const
{ 
    return m_kVertexFormat;    
}
//---------------------------------------------------------------------------
inline void NiOutputStreamDescriptor::SetMaxVertexCount(
    unsigned int uiMaxVertexCount) 
{ 
    m_uiMaxVertexCount = uiMaxVertexCount; 
}
//---------------------------------------------------------------------------
inline void NiOutputStreamDescriptor::AppendVertexFormat(
    const VertexFormatEntry& kVertexFormatEntry)
{
    m_kVertexFormat.Add(kVertexFormatEntry);
}
//---------------------------------------------------------------------------
inline void NiOutputStreamDescriptor::SetPrimType(
    NiPrimitiveType::Type ePrimType) 
{ 
    m_ePrimType = ePrimType; 
}
//---------------------------------------------------------------------------
inline void NiOutputStreamDescriptor::SetName(const NiFixedString& kName)
{ 
    m_kName = kName;
}
//---------------------------------------------------------------------------
