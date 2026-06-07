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
inline NiTextKeyExtraData::NiTextKeyExtraData()
{
    m_uiNumKeys = 0;
    m_pKeys = 0;
}
//---------------------------------------------------------------------------
inline NiTextKeyExtraData::NiTextKeyExtraData(NiTextKey* pKeys,
    unsigned int uiNumKeys)
{
    m_uiNumKeys = uiNumKeys;
    m_pKeys = pKeys;
}
//---------------------------------------------------------------------------
inline NiTextKeyExtraData::~NiTextKeyExtraData ()
{
    NiDelete[] m_pKeys;
}
//---------------------------------------------------------------------------
inline void NiTextKeyExtraData::SetKeys(NiTextKey* pKeys,
    unsigned int uiNumKeys)
{
    m_uiNumKeys = uiNumKeys;
    m_pKeys = pKeys;
}
//---------------------------------------------------------------------------
inline NiTextKey* NiTextKeyExtraData::GetKeys(unsigned int& uiNumKeys) const
{
    uiNumKeys = m_uiNumKeys;
    return m_pKeys;
}
//---------------------------------------------------------------------------
