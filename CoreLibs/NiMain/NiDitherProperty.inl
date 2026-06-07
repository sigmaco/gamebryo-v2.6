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
//  NiDitherProperty inline functions

//---------------------------------------------------------------------------
inline NiDitherProperty::NiDitherProperty()
{
    m_uFlags = 0;
    SetDithering(false);
}
//---------------------------------------------------------------------------
inline void NiDitherProperty::SetDithering(bool bDither)
{
    SetBit(bDither, DITHER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiDitherProperty::GetDithering() const
{
    return GetBit(DITHER_MASK);
}
//---------------------------------------------------------------------------
inline bool NiDitherProperty::IsEqualFast(const NiDitherProperty& kProp) const
{
    return m_uFlags == kProp.m_uFlags;
}
//---------------------------------------------------------------------------
inline int NiDitherProperty::Type() const
{
    return NiProperty::DITHER;
}
//---------------------------------------------------------------------------
inline int NiDitherProperty::GetType()
{
    return NiProperty::DITHER;
}
//---------------------------------------------------------------------------
inline NiDitherProperty* NiDitherProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
