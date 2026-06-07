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
inline unsigned int NiPixelData::GetNumMipmapLevels() const
{
    return m_uiMipmapLevels;
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelData::GetNumFaces() const
{
    return m_uiFaces;
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelData::GetWidth(unsigned int uiMipmapLevel,
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_puiWidth[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelData::GetHeight(unsigned int uiMipmapLevel,
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_puiHeight[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelData::GetPixelStride() const
{
    return m_uiPixelStride;
}
//---------------------------------------------------------------------------
inline size_t 
    NiPixelData::GetSizeInBytes(unsigned int uiMipmapLevel,
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_puiOffsetInBytes[uiMipmapLevel + 1] - 
        m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline size_t NiPixelData::GetTotalSizeInBytes() const
{
    return m_puiOffsetInBytes[m_uiMipmapLevels]*m_uiFaces;
}
//---------------------------------------------------------------------------
inline size_t NiPixelData::GetFaceSizeInBytes() const
{
    return m_puiOffsetInBytes[m_uiMipmapLevels];
}
//---------------------------------------------------------------------------
inline ptrdiff_t 
    NiPixelData::GetOffsetInBytes(unsigned int uiMipmapLevel,
    unsigned int uiFace) const
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return uiFace*GetFaceSizeInBytes() + m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline const NiPixelFormat& NiPixelData::GetPixelFormat() const
{
    return m_kPixelFormat;
}
//---------------------------------------------------------------------------
inline void NiPixelData::SetPalette(NiPalette* pkPal)
{
    m_spPalette = pkPal;
}
//---------------------------------------------------------------------------
inline const NiPalette* NiPixelData::GetPalette() const
{
    return m_spPalette;
}
//---------------------------------------------------------------------------
inline NiPalette* NiPixelData::GetPalette()
{
    return m_spPalette;
}
//---------------------------------------------------------------------------
inline const unsigned char* 
    NiPixelData::GetPixels(unsigned int uiMipmapLevel,
    unsigned int uiFace) const
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_pucPixels + uiFace*m_puiOffsetInBytes[m_uiMipmapLevels] + 
        m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned char* NiPixelData::GetPixels(unsigned int uiMipmapLevel,
    unsigned int uiFace)
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_pucPixels + uiFace*m_puiOffsetInBytes[m_uiMipmapLevels] +
        m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline void NiPixelData::MarkAsChanged()
{
    m_uiRevID++;
}
//---------------------------------------------------------------------------
inline unsigned int NiPixelData::GetRevisionID() const
{
    return m_uiRevID;
}
//---------------------------------------------------------------------------
