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
//  NiColorExtraData inline functions

//---------------------------------------------------------------------------
inline NiColorExtraData::NiColorExtraData()
{
    m_kColorA = NiColorA::BLACK;    // Alpha component is opaque.
}
//---------------------------------------------------------------------------
inline NiColorExtraData::~NiColorExtraData()
{
}
//---------------------------------------------------------------------------
inline const NiColorA NiColorExtraData::GetValue() const
{
    return m_kColorA;
}
//---------------------------------------------------------------------------
inline const NiColor NiColorExtraData::GetValueColor3() const
{
    NiColor kColor(m_kColorA.r, m_kColorA.g, m_kColorA.b);
    return kColor;
}
//---------------------------------------------------------------------------
inline void NiColorExtraData::SetRed(const float fRed)
{
    m_kColorA.r = fRed;
}
//---------------------------------------------------------------------------
inline void NiColorExtraData::SetGreen(const float fGreen)
{
    m_kColorA.g = fGreen;
}
//---------------------------------------------------------------------------
inline void NiColorExtraData::SetBlue(const float fBlue)
{
    m_kColorA.b = fBlue;
}
//---------------------------------------------------------------------------
inline void NiColorExtraData::SetAlpha(const float fAlpha)
{
    m_kColorA.a = fAlpha;
}
//---------------------------------------------------------------------------
inline float NiColorExtraData::GetRed() const
{
    return m_kColorA.r;
}
//---------------------------------------------------------------------------
inline float NiColorExtraData::GetGreen() const
{
    return m_kColorA.g;
}
//---------------------------------------------------------------------------
inline float NiColorExtraData::GetBlue() const
{
    return m_kColorA.b;
}
//---------------------------------------------------------------------------
inline float NiColorExtraData::GetAlpha() const
{
    return m_kColorA.a;
}
//---------------------------------------------------------------------------
