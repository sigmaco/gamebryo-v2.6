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
//  NiMaterialProperty inline functions

//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetAmbientColor(const NiColor& c)
{
    m_amb = c;
    SetChanged(true);
}
//---------------------------------------------------------------------------
inline const NiColor& NiMaterialProperty::GetAmbientColor() const
{ 
    return m_amb;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetDiffuseColor(const NiColor& c)
{
    m_diff = c;
    SetChanged(true);
}
//---------------------------------------------------------------------------
inline const NiColor& NiMaterialProperty::GetDiffuseColor() const
{
    return m_diff;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetSpecularColor(const NiColor& c)
{
    m_spec = c;
    SetChanged(true);
}
//---------------------------------------------------------------------------
inline const NiColor& NiMaterialProperty::GetSpecularColor() const
{
    return m_spec;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetEmittance(const NiColor& c)
{
    m_emit = c;
    SetChanged(true);
}
//---------------------------------------------------------------------------
inline const NiColor& NiMaterialProperty::GetEmittance() const
{
    return m_emit;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetShineness(float c)
{
    m_fShine = c;
    SetChanged(true);
}
//---------------------------------------------------------------------------
inline float NiMaterialProperty::GetShineness() const
{
    return m_fShine;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetAlpha(float c)
{
    m_fAlpha = c;
    SetChanged(true);
}
//---------------------------------------------------------------------------
inline float NiMaterialProperty::GetAlpha() const
{
    return m_fAlpha;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetRevisionID(unsigned int uiRev)
{
    m_uiRevID = uiRev;
}
//---------------------------------------------------------------------------
inline unsigned int NiMaterialProperty::GetRevisionID() const
{
    return m_uiRevID;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetChanged(bool)
{
    m_uiRevID++;
}
//---------------------------------------------------------------------------
inline int NiMaterialProperty::GetIndex() const
{
    return m_iIndex;
}
//---------------------------------------------------------------------------
#if defined(WIN32) || defined(_XENON)
inline void* NiMaterialProperty::GetRendererData() const
{
    return m_pvRendererData;
}
//---------------------------------------------------------------------------
inline void NiMaterialProperty::SetRendererData(void* pvRendererData)
{
    m_pvRendererData = pvRendererData;
}
//---------------------------------------------------------------------------
#endif // defined(WIN32) || defined(_XENON)
inline bool NiMaterialProperty::IsEqualFast(
    const NiMaterialProperty& prop) const
{
    return m_iIndex == prop.m_iIndex;
}
//---------------------------------------------------------------------------
inline int NiMaterialProperty::Type() const
{
    return NiProperty::MATERIAL;
}
//---------------------------------------------------------------------------
inline int NiMaterialProperty::GetType()
{
    return NiProperty::MATERIAL;
}
//---------------------------------------------------------------------------
inline NiMaterialProperty *NiMaterialProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
