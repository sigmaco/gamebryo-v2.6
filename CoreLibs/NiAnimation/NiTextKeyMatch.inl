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
inline NiTextKeyMatch::NiTextKeyMatch(const NiFixedString& kMatchKey)
{
    m_kMatchKey = kMatchKey;
};

//---------------------------------------------------------------------------
inline NiTextKeyMatch::~NiTextKeyMatch() 
{ 
};

//---------------------------------------------------------------------------
inline bool NiTextKeyMatch::IsKeyMatch(const NiFixedString& kTextKey)
{ 
    if ( kTextKey == m_kMatchKey)
    {
        SetLastMatchedKey( kTextKey );
        return true;
    }
    return false;
};
//---------------------------------------------------------------------------
inline NiTextKeyMatch::operator const char*(void) 
{ 
    return (const char*)m_kMatchKey; 
};
//---------------------------------------------------------------------------
inline const NiFixedString& NiTextKeyMatch::GetLastMatchedKey() const
{
    return m_kLastKeyMatched;
}
//---------------------------------------------------------------------------
inline void NiTextKeyMatch::SetLastMatchedKey(const NiFixedString& kTextKey)
{
    NIASSERT(kTextKey.Exists());
    m_kLastKeyMatched = kTextKey;
}
//---------------------------------------------------------------------------
