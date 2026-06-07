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

//  NiMeshProfile inline functions

//---------------------------------------------------------------------------
inline const NiFixedString& NiMeshProfileStream::GetName() const
{
    return m_strName;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiMeshProfileStream::GetAccessFlags() const
{
    return m_uiAccessFlags;
}
//---------------------------------------------------------------------------
inline NiDataStream::Usage NiMeshProfileStream::GetUsage() const
{
    return m_eUsage;
}
//---------------------------------------------------------------------------
inline NiSystemDesc::RendererID NiMeshProfileStream::GetRenderer() const
{
    return m_eRenderer;
}
//---------------------------------------------------------------------------
inline unsigned int NiMeshProfileStream::GetElementCount() const
{
    return m_kElements.GetSize();
}
//---------------------------------------------------------------------------
inline const NiMeshProfileElement* 
    NiMeshProfileStream::GetElement(unsigned int uiIndex) const
{
    return m_kElements[uiIndex];
}
//---------------------------------------------------------------------------
inline const NiTObjectArray <NiMeshProfileElementPtr>* 
    NiMeshProfileStream::GetElements() const
{
    return &m_kElements;
}
//---------------------------------------------------------------------------
inline const char* NiMeshProfileStream::GetRefStream() const
{
    return (m_uiModifiedFlags & MODIFIED_REFSTREAM) ? 
        (const char*)m_strRefStream : NULL;
}
//---------------------------------------------------------------------------
inline void NiMeshProfileStream::SetName(const char* pName)
{
    m_strName = pName;
    m_uiModifiedFlags |= MODIFIED_NAME;
}
//---------------------------------------------------------------------------