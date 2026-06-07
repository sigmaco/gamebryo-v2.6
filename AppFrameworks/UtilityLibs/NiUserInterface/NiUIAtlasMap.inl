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
inline NiSourceTexture* NiUIAtlasMap::GetAtlas() const
{
    return m_spAtlas;
}
//---------------------------------------------------------------------------
inline int NiUIAtlasMap::ParseGroup(char*& pcGroup) const
{
    unsigned int uiIdx;
    size_t stLen = strlen(pcGroup);
    if (pcGroup[0] == '[' && pcGroup[stLen - 1] == ']')
    {
        for (uiIdx = 0; uiIdx < NUM_GROUPS; ++uiIdx)
        {
            if (NiStrnicmp(ms_apcGroupNames[uiIdx], pcGroup + 1, 
                stLen - 2) == 0)
            {
                return uiIdx;
            }
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
inline int NiUIAtlasMap::ParseElement(char*& pcElementName) const
{
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < NUM_ELEMENTS; ++uiIdx)
    {
        size_t stLen = strlen(ms_apcElementNames[uiIdx]);
        if (NiStrnicmp(ms_apcElementNames[uiIdx], pcElementName,
            stLen) == 0)
        {
            pcElementName += stLen;
            return uiIdx;
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
inline int NiUIAtlasMap::ParseTextColor(char*& pcTextColorName) const
{
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < NUM_TEXT_COLORS; ++uiIdx)
    {
        size_t stLen = strlen(ms_apcTextColorNames[uiIdx]);
        if (NiStrnicmp(ms_apcTextColorNames[uiIdx], pcTextColorName,
            stLen) == 0)
        {
            pcTextColorName += stLen;
            return uiIdx;
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
inline int NiUIAtlasMap::ParseTextSize(char*& pcTextSizeName) const
{
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < NUM_TEXT_SIZES; ++uiIdx)
    {
        size_t stLen = strlen(ms_apcTextSizeNames[uiIdx]);
        if (NiStrnicmp(ms_apcTextSizeNames[uiIdx], pcTextSizeName,
            stLen) == 0)
        {
            pcTextSizeName += stLen;
            return uiIdx;
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
inline bool NiUIAtlasMap::ParseFilename(char*& pcFilename, 
    NiString& kValue) const
{
    while ((pcFilename[0] == ' ' || pcFilename[0] == '\t') && 
        pcFilename[0] != '\0')
    {
        pcFilename++;
    }

    if (pcFilename[0] == '\0')
        return false;

    size_t stLen = strlen(pcFilename);
    while ((pcFilename[stLen-1] == ' ' || pcFilename[stLen-1] == '\t') && 
        stLen != 0)
    {
        pcFilename[stLen-1] = '\0';
    }

    if (pcFilename[0] == '\0')
        return false;

    kValue = pcFilename;
    return true;
}
//---------------------------------------------------------------------------
inline bool NiUIAtlasMap::ParseInt(char*& pcString, int& iValue) const
{
    while ((pcString[0] == ' ' || pcString[0] == '\t' || pcString[0] == ',')
        && pcString[0] != '\0')
    {
        pcString++;
    }

    if (pcString[0] == '\0')
        return false;

    iValue = atoi(pcString);

    while ((pcString[0] >= '0' && pcString[0] <= '9'))
        pcString++;
    return true;
}
//---------------------------------------------------------------------------
inline bool NiUIAtlasMap::ParseFloat(char*& pcString, float& fValue) const
{
    while ((pcString[0] == ' ' || pcString[0] == '\t' || pcString[0] == ',') 
        && pcString[0] != '\0')
    {
        pcString++;
    }

    if (pcString[0] == '\0')
        return false;

    double dValue = atof(pcString);
    fValue = (float) dValue;

    while (pcString[0] == '.' || (pcString[0] >= '0' && pcString[0] <= '9'))
        pcString++;

    return true;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiUIAtlasMap::Translate(unsigned int eElement, 
    const NiPoint2& kOrigUV) const
{
    NIASSERT(eElement < NUM_ELEMENTS);
    NiPoint2 kTranslatedUV(m_akBounds[eElement].m_left, 
        m_akBounds[eElement].m_top);

    if (m_akBounds[eElement].m_left != 
        m_akBounds[eElement].m_right)
    {
        kTranslatedUV.x = m_akBounds[eElement].m_left + 
            (kOrigUV.x * (m_akBounds[eElement].m_right - 
            m_akBounds[eElement].m_left));
    }
    if (m_akBounds[eElement].m_top != 
        m_akBounds[eElement].m_bottom)
    {
        kTranslatedUV.y = m_akBounds[eElement].m_top + 
            (kOrigUV.y * (m_akBounds[eElement].m_bottom - 
            m_akBounds[eElement].m_top));
    }
    
    return kTranslatedUV;
}
//---------------------------------------------------------------------------
inline const NiRect<float> NiUIAtlasMap::GetOuterBounds(UIElement eElement,
    CoordinateSystem eSystem) const
{
    NIASSERT(eElement >= 0);
    NIASSERT(eElement < NUM_ELEMENTS);
    switch (eSystem)
    {
        default:
        case ATLAS_UV_SPACE:
            return m_akBounds[eElement];
        case ATLAS_PIXEL_SPACE:
            return ConvertAtlasUVToPixels(m_akBounds[eElement]);
        case DEVICE_NDC_SPACE:
            return ConvertAtlasUVToNDC(m_akBounds[eElement]);
    }
}
//---------------------------------------------------------------------------
inline const NiRect<float> NiUIAtlasMap::GetInnerBounds(
    UIElement eElement, CoordinateSystem eSystem) const
{
    NIASSERT(eElement >= 0);
    NIASSERT(eElement < NUM_ELEMENTS);

    switch (eSystem)
    {
        default:
        case ATLAS_UV_SPACE:
            return m_akInternalBounds[eElement];
        case ATLAS_PIXEL_SPACE:
            return ConvertAtlasUVToPixels(m_akInternalBounds[eElement]);
        case DEVICE_NDC_SPACE:
            return ConvertAtlasUVToNDC(m_akInternalBounds[eElement]);
    }
}
//---------------------------------------------------------------------------
inline const NiColor NiUIAtlasMap::GetTextColor(TextColor eUITextColor) 
    const
{
    NIASSERT(eUITextColor >= 0);
    NIASSERT(eUITextColor < NUM_TEXT_COLORS);
    NiColor kRetColor(m_akTextColors[eUITextColor].r, 
        m_akTextColors[eUITextColor].g, m_akTextColors[eUITextColor].b);
    return kRetColor;
}
//---------------------------------------------------------------------------
inline const NiString NiUIAtlasMap::GetTextFilename(TextSize eTextSize) 
    const
{
    NIASSERT(eTextSize >= 0);
    NIASSERT(eTextSize < NUM_TEXT_SIZES);

    return m_akTextFilenames[eTextSize];
}
//---------------------------------------------------------------------------

