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
inline NiBezFloatKey::NiBezFloatKey()
{
}
//---------------------------------------------------------------------------
inline NiBezFloatKey::NiBezFloatKey(float fTime, float fVal, float fInTan,
    float fOutTan)
    :
    NiFloatKey(fTime,fVal)
{
    m_fInTan = fInTan;
    m_fOutTan = fOutTan;
}
//---------------------------------------------------------------------------
inline void NiBezFloatKey::SetInTan(float fInTan)
{
    m_fInTan = fInTan;
}
//---------------------------------------------------------------------------
inline void NiBezFloatKey::SetOutTan(float fOutTan)
{
    m_fOutTan = fOutTan;
}
//---------------------------------------------------------------------------
inline float NiBezFloatKey::GetInTan() const
{
    return m_fInTan;
}
//---------------------------------------------------------------------------
inline float NiBezFloatKey::GetOutTan() const
{
    return m_fOutTan;
}
//---------------------------------------------------------------------------
