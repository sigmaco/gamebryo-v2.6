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
//  NiWireframeProperty inline functions

//---------------------------------------------------------------------------
inline NiWireframeProperty::NiWireframeProperty()
{
    m_uFlags = 0;
    SetWireframe(false);
}
//---------------------------------------------------------------------------
inline void NiWireframeProperty::SetWireframe(bool bWireframe)
{
    SetBit(bWireframe, WIREFRAME_MASK);
}
//---------------------------------------------------------------------------
inline bool NiWireframeProperty::GetWireframe() const
{
    return GetBit(WIREFRAME_MASK);
}
//---------------------------------------------------------------------------
inline bool NiWireframeProperty::IsEqualFast(const NiWireframeProperty& kProp)
    const
{
    return m_uFlags == kProp.m_uFlags;
}
//---------------------------------------------------------------------------
inline int NiWireframeProperty::Type() const
{
    return NiProperty::WIREFRAME;
}
//---------------------------------------------------------------------------
inline int NiWireframeProperty::GetType()
{
    return NiProperty::WIREFRAME;
}
//---------------------------------------------------------------------------
inline NiWireframeProperty* NiWireframeProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
