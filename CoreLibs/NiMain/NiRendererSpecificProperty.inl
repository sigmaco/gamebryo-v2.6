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
//  NiMultiTextureProperty inline functions

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline int NiRendererSpecificProperty::Type() const
{ 
    return NiProperty::REND_SPEC; 
}
//---------------------------------------------------------------------------
inline int NiRendererSpecificProperty::GetType()
{
    return NiProperty::REND_SPEC;
}
//---------------------------------------------------------------------------
inline NiRendererSpecificProperty *NiRendererSpecificProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
inline bool NiRendererSpecificProperty::IsEqualFast(
    const NiRendererSpecificProperty& kProp) const
{
    // For now, return that any two distinct properties are different.
    return this == &kProp;
}
//---------------------------------------------------------------------------
