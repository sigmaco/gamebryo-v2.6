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
// NiUnicodeFontCFG inline functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
inline int NiUnicodeFontCFG::GetNumOfUniqueCharacters() const
{
    return (int)m_kSortedData.GetSize();
}
//---------------------------------------------------------------------------
inline const char* NiUnicodeFontCFG::GetFontName() const
{
    return m_kHeader.m_acFontName;
}
//---------------------------------------------------------------------------
inline void NiUnicodeFontCFG::SetHeader(const UnicodeFontCfgHeader& header)
{
    m_kHeader.m_uiVersion = header.m_uiVersion;
    strcpy_s(m_kHeader.m_acFontName, NIUNICODEFONTCFG_NAMELEN,
        header.m_acFontName);
    m_kHeader.m_usFontSize = header.m_usFontSize;
    m_kHeader.m_usFontStyle = header.m_usFontStyle;
}
//---------------------------------------------------------------------------
inline const NiUnicodeFontCFG::UnicodeFontCfgHeader&
    NiUnicodeFontCFG::GetHeader() const
{
    return m_kHeader;
}
//---------------------------------------------------------------------------
inline unsigned short NiUnicodeFontCFG::GetCharEncoding(unsigned int uiIndex)
    const
{
    if( uiIndex < m_kSortedData.GetSize() )
    {
        CharData* data = m_kSortedData.GetAt(uiIndex);
        if( data != NULL )
            return data->m_usChar;
    }
    return (unsigned short)0;
}
//---------------------------------------------------------------------------
inline bool NiUnicodeFontCFG::CharExists(unsigned short usChar) const
{
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_kSortedData.GetSize(); uiLoop++)
    {
        CharData* data = m_kSortedData.GetAt(uiLoop);
        if ((data != NULL ) && (data->m_usChar == usChar))
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
