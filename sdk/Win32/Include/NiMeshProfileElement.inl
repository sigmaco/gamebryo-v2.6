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
inline const NiFixedString& NiMeshProfileElement::GetName() const
{
    return m_strName;
}
//---------------------------------------------------------------------------
inline NiCommonSemantics::Enumeration NiMeshProfileElement::GetType() const
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiMeshProfileElement::GetIndex() const
{
    return m_uiIndex;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiMeshProfileElement::GetStrictFlags() const
{
    return m_uiStrictFlags;
}
//---------------------------------------------------------------------------
inline NiMeshProfileOperations::CastConvertType 
NiMeshProfileElement::GetConvertType() const
{
    return m_eConvertType;
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Type NiMeshProfileElement::GetConvertFormat() const
{
    return m_eConvertFormat;
}
//---------------------------------------------------------------------------
inline NiDataStreamElement::Format NiMeshProfileElement::GetSpecialFormat()
    const
{
    return m_eSpecialFormat;
}
//---------------------------------------------------------------------------
inline NiSystemDesc::RendererID NiMeshProfileElement::GetRenderer() const
{
    return m_eRenderer;
}
//---------------------------------------------------------------------------
inline NiMeshProfileElement::RequiredCode 
    NiMeshProfileElement::GetRequired() const
{
    return m_eRequired;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfileElement::NeedsConverting() const
{
    return (m_uiModifiedFlags & MODIFIED_CONVERTFORMAT) ? true : false;
}
//---------------------------------------------------------------------------
inline bool NiMeshProfileElement::NeedsSpecialConverting() const
{
    return (m_uiModifiedFlags & MODIFIED_SPECIALFORMAT) ? true : false;
}
//---------------------------------------------------------------------------
inline const char* NiMeshProfileElement::GetRefElement() const
{
    return (m_uiModifiedFlags & MODIFIED_REFELEMENT) ? 
        (const char*)m_strRefElement : NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiMeshProfileElement::GetComponentCount() const
{
    return m_kComponents.GetSize();
}
//---------------------------------------------------------------------------
inline const NiMeshProfileComponent* 
    NiMeshProfileElement::GetComponent(unsigned int uiIndex) const
{
    return m_kComponents[uiIndex];
}
//---------------------------------------------------------------------------
inline const NiTObjectArray <NiMeshProfileComponentPtr>* 
    NiMeshProfileElement::GetComponents() const
{
    return &m_kComponents;
}
//---------------------------------------------------------------------------
inline void NiMeshProfileElement::SetName(const char* pName)
{
    m_strName = pName;
    m_uiModifiedFlags |= MODIFIED_NAME;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiMeshProfileElement::GetSemanticName() const
{
    return (m_uiModifiedFlags & MODIFIED_TYPE) ? 
        NiCommonSemantics::GetString(m_eType) : m_strName;
}
//---------------------------------------------------------------------------
