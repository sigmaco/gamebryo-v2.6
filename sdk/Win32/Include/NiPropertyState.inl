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
//  NiPropertyState inline functions

//---------------------------------------------------------------------------
inline void NiPropertyState::SetProperty(NiProperty* pkProp)
{
    // can't set a property state to NULL - besides, without a real object,
    // the type cannot be determined anyway
    if(!pkProp)
        return;

    m_aspProps[pkProp->Type()] = pkProp;
}
//---------------------------------------------------------------------------
inline NiPropertyPtr &NiPropertyState::operator[](int iType)
{
    return m_aspProps[iType];
}
//---------------------------------------------------------------------------
inline void NiPropertyState::SwapProperties(NiPropertyList& kProplist)
{
    if ( kProplist.IsEmpty() )
        return;

    NiTListIterator kPos = kProplist.GetHeadPos();
    while (kPos)
    {
        NiTListIterator kRemovePos = kPos;
        NiPropertyPtr spProperty = kProplist.GetNext(kPos);
        int iType = spProperty->Type();

        // swap in new property and save old property
        kProplist.InsertBefore(kRemovePos, m_aspProps[iType]);
        kProplist.RemovePos(kRemovePos);
        m_aspProps[iType] = spProperty;
    }
}
//---------------------------------------------------------------------------
// used to simplify the code below
#define NiPropStateGetProp(e) (NiProperty*)m_aspProps[e]
//---------------------------------------------------------------------------
inline NiAlphaProperty* NiPropertyState::GetAlpha() const
{
    return (NiAlphaProperty*)NiPropStateGetProp(NiProperty::ALPHA);
}
//---------------------------------------------------------------------------
inline NiDitherProperty* NiPropertyState::GetDither() const
{
    return (NiDitherProperty*)NiPropStateGetProp(NiProperty::DITHER);
}
//---------------------------------------------------------------------------
inline NiFogProperty* NiPropertyState::GetFog() const
{
    return (NiFogProperty*)NiPropStateGetProp(NiProperty::FOG);
}
//---------------------------------------------------------------------------
inline NiMaterialProperty* NiPropertyState::GetMaterial() const
{
    return (NiMaterialProperty*)NiPropStateGetProp(NiProperty::MATERIAL);
}
//---------------------------------------------------------------------------
inline NiRendererSpecificProperty* 
    NiPropertyState::GetRendererSpecific() const
{
    return (NiRendererSpecificProperty*)
        NiPropStateGetProp(NiProperty::REND_SPEC);
}
//---------------------------------------------------------------------------
inline NiShadeProperty* NiPropertyState::GetShade() const
{
    return (NiShadeProperty*)NiPropStateGetProp(NiProperty::SHADE);
}
//---------------------------------------------------------------------------
inline NiSpecularProperty* NiPropertyState::GetSpecular() const
{
    return (NiSpecularProperty*)NiPropStateGetProp(NiProperty::SPECULAR);
}
//---------------------------------------------------------------------------
inline NiStencilProperty* NiPropertyState::GetStencil() const
{
    return (NiStencilProperty*)NiPropStateGetProp(NiProperty::STENCIL);
}
//---------------------------------------------------------------------------
inline NiTexturingProperty* NiPropertyState::GetTexturing() const
{
    return (NiTexturingProperty*)
        NiPropStateGetProp(NiProperty::TEXTURING);
}
//---------------------------------------------------------------------------
inline NiVertexColorProperty* NiPropertyState::GetVertexColor() const
{
    return (NiVertexColorProperty*)
        NiPropStateGetProp(NiProperty::VERTEX_COLOR);
}
//---------------------------------------------------------------------------
inline NiWireframeProperty* NiPropertyState::GetWireframe() const
{
    return (NiWireframeProperty*)NiPropStateGetProp(NiProperty::WIREFRAME);
}
//---------------------------------------------------------------------------
inline NiZBufferProperty* NiPropertyState::GetZBuffer() const
{
    return (NiZBufferProperty*)NiPropStateGetProp(NiProperty::ZBUFFER);
}
//---------------------------------------------------------------------------

