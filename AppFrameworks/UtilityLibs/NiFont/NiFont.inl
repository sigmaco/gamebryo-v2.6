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
inline unsigned int NiFont::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetCharHeight() const
{
    return m_uiCharHeight;
}
//---------------------------------------------------------------------------
inline unsigned short NiFont::GetMaxPixelHeight() const
{
    return m_usMaxPixelHeight;
}
//---------------------------------------------------------------------------
inline unsigned short NiFont::GetMaxPixelWidth() const
{
    return m_usMaxPixelWidth;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsBold() const
{
    return (m_uiFlags & BOLD) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsItalic() const
{
    return (m_uiFlags & ITALIC) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsStrikeOut() const
{
    return (m_uiFlags & STRIKEOUT) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsUnderline() const
{
    return (m_uiFlags & UNDERLINE) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsAntiAliased() const
{
    return (m_uiFlags & ANTIALIASED) != 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetGlyphCount() const
{
    return m_uiGlyphCount;
}
//---------------------------------------------------------------------------
inline const char* NiFont::GetName() const
{
    return m_acName;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetNumTextures() const
{
    return m_spTextures.GetSize();
}
//---------------------------------------------------------------------------
inline NiSourceTexture* NiFont::GetTexture(NiWChar kChar /*= 0*/) const
{
    // Return the first texture if the char is null OR the texture location
    // is NULL
    if ((kChar == 0) || (m_pucTextureLocation == NULL))
        return m_spTextures.GetAt(0);

    // Read the character from the glpyh map to determine what texture
    // it will be found on
    unsigned short usIndex = GetGlyphMapIndex(kChar);
    return m_spTextures.GetAt(m_pucTextureLocation[usIndex]);
}
//---------------------------------------------------------------------------
inline unsigned char NiFont::GetTextureIndex(NiWChar kChar /*= 0*/) const
{
    // Return the first texture if the char is null OR the texture location
    // is NULL
    if ((kChar == 0) || (m_pucTextureLocation == NULL))
        return 0;

    // Read the character from the glpyh map to determine what texture
    // it will be found on
    unsigned short usIndex = GetGlyphMapIndex(kChar);
    return m_pucTextureLocation[usIndex];
}
//---------------------------------------------------------------------------
inline NiSourceTexture* NiFont::GetTextureAtIndex(
    unsigned char ucIndex /*= 0*/) const
{
    return m_spTextures.GetAt(ucIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetTextureWidth(NiWChar kChar /*= 0*/) const
{
    return GetTexture(kChar)->GetWidth();
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetTextureHeight(NiWChar kChar /*= 0*/) const
{
    return GetTexture(kChar)->GetHeight();
}
//---------------------------------------------------------------------------
inline unsigned short NiFont::GetGlyphMapIndex(NiWChar kChar) const
{
    NIASSERT(m_pkGlyphMap);

    unsigned short usIndex = 0;

    // Look for the character in the glyph map
    if (!m_pkGlyphMap->GetAt(kChar, usIndex))
    {
        // Use index 0
        return 0;
    }

    return usIndex;
}
//---------------------------------------------------------------------------
inline const NiPixelData* NiFont::GetPixelData(
    unsigned char ucIndex /*= 0*/) const
{
    // Return the pixel data from the texture at Index. This function exists
    // for legacy reasons only.
    if (ucIndex >= m_spTextures.GetSize())
        return NULL;

    return m_spTextures.GetAt(ucIndex)->GetSourcePixelData();
}
//---------------------------------------------------------------------------
inline void NiFont::SetPixelData(NiPixelData*)
{
    NiOutputDebugString("NiFont::SetPixelData This function has been "
        "fully deprecated.");
}
//---------------------------------------------------------------------------
inline int NiFont::GetRightOverhang(char cChar) const
{
    unsigned short usIndex = GetGlyphMapIndex(cChar);
    int iRetval = -(int)m_uiAtlasPadRight;
    if (m_piOverhangRight)
        iRetval += m_piOverhangRight[usIndex];
    return iRetval;
}
//---------------------------------------------------------------------------
inline int NiFont::GetRightOverhang(NiWChar kChar) const
{
    unsigned short usIndex = GetGlyphMapIndex(kChar);
    int iRetval = -(int)m_uiAtlasPadRight;
    if (m_piOverhangRight)
        iRetval += m_piOverhangRight[usIndex];
    return iRetval;
}
//---------------------------------------------------------------------------
inline int NiFont::GetLeftOverhang(char cChar) const
{
    unsigned short usIndex = GetGlyphMapIndex(cChar);
    int iRetval = -(int)m_uiAtlasPadLeft;
    if (m_piOverhangLeft)
        iRetval += m_piOverhangLeft[usIndex];
    return iRetval;
}
//---------------------------------------------------------------------------
inline int NiFont::GetLeftOverhang(NiWChar kChar) const
{
    unsigned short usIndex = GetGlyphMapIndex(kChar);
    int iRetval = -(int)m_uiAtlasPadLeft;
    if (m_piOverhangLeft)
        iRetval += m_piOverhangLeft[usIndex];
    return iRetval;
}
//---------------------------------------------------------------------------
inline int NiFont::GetTopOverhang(void) const
{
    return -(int)m_uiAtlasPadTop;
}
//---------------------------------------------------------------------------
inline int NiFont::GetBottomOverhang(void) const
{
    return -(int)m_uiAtlasPadBottom;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetAtlasPadRight(void) const
{
    return m_uiAtlasPadRight;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetAtlasPadLeft(void) const
{
    return m_uiAtlasPadLeft;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetAtlasPadTop(void) const
{
    return m_uiAtlasPadTop;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetAtlasPadBottom(void) const
{
    return m_uiAtlasPadBottom;
}
//---------------------------------------------------------------------------
inline NiFont::Error NiFont::GetLastError()
{
    return m_eLastError;
}
//---------------------------------------------------------------------------
