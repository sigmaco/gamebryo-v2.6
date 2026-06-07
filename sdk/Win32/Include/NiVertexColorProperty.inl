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
//  NiVertexColorProperty inline functions

//---------------------------------------------------------------------------
inline NiVertexColorProperty::NiVertexColorProperty() :
    m_uFlags(0)
{
    SetSourceMode(SOURCE_IGNORE);
    SetLightingMode(LIGHTING_E_A_D);
}
//---------------------------------------------------------------------------
inline void NiVertexColorProperty::SetSourceMode(SourceVertexMode eSource)
{
    SetField(NiInt32ToUInt16(eSource), SOURCEVERTEXMODE_MASK,
        SOURCEVERTEXMODE_POS);
}
//---------------------------------------------------------------------------
inline NiVertexColorProperty::SourceVertexMode 
    NiVertexColorProperty::GetSourceMode() const
{
    return (SourceVertexMode)
        GetField(SOURCEVERTEXMODE_MASK, SOURCEVERTEXMODE_POS);
}
//---------------------------------------------------------------------------
inline void NiVertexColorProperty::SetLightingMode(LightingMode eLighting)
{
    SetField(NiInt32ToUInt16(eLighting), LIGHTINGMODE_MASK, 
        LIGHTINGMODE_POS);
}
//---------------------------------------------------------------------------
inline NiVertexColorProperty::LightingMode 
    NiVertexColorProperty::GetLightingMode() const
{
    return (LightingMode)
        GetField(LIGHTINGMODE_MASK, LIGHTINGMODE_POS);
}
//---------------------------------------------------------------------------
inline bool NiVertexColorProperty::IsEqualFast(
    const NiVertexColorProperty& kProp) const
{
    return (m_uFlags == kProp.m_uFlags);
}
//---------------------------------------------------------------------------
inline int NiVertexColorProperty::Type() const
{
    return NiProperty::VERTEX_COLOR;
}
//---------------------------------------------------------------------------
inline int NiVertexColorProperty::GetType()
{
    return NiProperty::VERTEX_COLOR;
}
//---------------------------------------------------------------------------
inline NiVertexColorProperty *NiVertexColorProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
