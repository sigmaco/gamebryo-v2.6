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
// NiFontString inline functions
//---------------------------------------------------------------------------
inline const char* NiFontString::GetText()
{
    return m_pcString;
}
//---------------------------------------------------------------------------
inline const NiWChar* NiFontString::GetUnicodeText()
{
    return m_pkString;
}
//---------------------------------------------------------------------------
inline const NiColorA& NiFontString::GetColor() const
{
    return m_kColor;
}
//---------------------------------------------------------------------------
inline unsigned char NiFontString::GetDirection() const
{
    return m_ucDirection;
}
//---------------------------------------------------------------------------
inline void NiFontString::SetDirection(unsigned char ucDirection)
{
    if (m_bStatic)
        return;

    if (m_ucDirection == ucDirection)
        return;

    m_ucDirection = ucDirection;

    m_bModified = true;
}
//---------------------------------------------------------------------------
inline float NiFontString::GetLeading() const
{
    return m_fLeading;
}
//---------------------------------------------------------------------------
inline void NiFontString::SetLeading (float fLeading)
{
    if (m_bStatic)
        return;

    if (m_fLeading == fLeading)
        return;
    m_fLeading = fLeading;
    SplitString();  // update cache of text size
    m_bModified = true;
}
//---------------------------------------------------------------------------
inline float NiFontString::GetTracking() const
{
    return m_fTracking;
}
//---------------------------------------------------------------------------
inline void NiFontString::SetTracking (float fTracking)
{
    if (m_bStatic)
        return;

    if (m_fTracking == fTracking)
        return;
    m_fTracking = fTracking;
    SplitString();  // update cache of text size
    m_bModified = true;
}
//---------------------------------------------------------------------------
