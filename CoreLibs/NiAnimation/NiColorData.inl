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
inline NiColorData::NiColorData()
{
    m_uiNumKeys = 0;
    m_pkKeys = 0;
    m_eType = NiAnimationKey::NOINTERP;
}
//---------------------------------------------------------------------------
inline unsigned int NiColorData::GetNumKeys() const
{
    return m_uiNumKeys;
}
//---------------------------------------------------------------------------
inline NiColorKey* NiColorData::GetAnim(unsigned int& uiNumKeys,
    NiColorKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumKeys;
    eType = m_eType;
    ucSize = m_ucKeySize;
    return m_pkKeys;
}
//---------------------------------------------------------------------------
