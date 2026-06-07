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
inline NiTextKey::NiTextKey()
{
}
//---------------------------------------------------------------------------
inline void NiTextKey::SetTime(float fTime)
{
    m_fTime = fTime;
}
//---------------------------------------------------------------------------
inline float NiTextKey::GetTime() const
{
    return m_fTime;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTextKey::GetText() const
{
    return m_kText;
}
//---------------------------------------------------------------------------
inline bool NiTextKey::operator== (const NiTextKey& key)
{
    return m_fTime == key.m_fTime && m_kText == key.m_kText;
}
//---------------------------------------------------------------------------
inline bool NiTextKey::operator!= (const NiTextKey& key)
{
    return m_fTime != key.m_fTime || m_kText != key.m_kText;
}
//---------------------------------------------------------------------------
