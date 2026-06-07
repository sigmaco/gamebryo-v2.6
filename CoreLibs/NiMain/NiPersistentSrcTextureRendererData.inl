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
inline unsigned int NiPersistentSrcTextureRendererData::GetNumMipmapLevels()
    const
{
    return m_uiMipmapLevels;
}
//---------------------------------------------------------------------------
inline unsigned int NiPersistentSrcTextureRendererData::GetNumFaces() const
{
    return m_uiFaces;
}
//---------------------------------------------------------------------------
inline const NiPersistentSrcTextureRendererData::PlatformID
    NiPersistentSrcTextureRendererData::GetTargetPlatform() const
{
    return m_eTargetPlatform;
}
//---------------------------------------------------------------------------
inline NiPersistentSrcTextureRendererData::PlatformID
    NiPersistentSrcTextureRendererData::GetTargetPlatform()
{
    return m_eTargetPlatform;
}
//---------------------------------------------------------------------------
inline unsigned int NiPersistentSrcTextureRendererData::GetWidth(
    unsigned int uiMipmapLevel, 
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_puiWidth[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned int NiPersistentSrcTextureRendererData::GetHeight(
    unsigned int uiMipmapLevel, 
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_puiHeight[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned int NiPersistentSrcTextureRendererData::GetPixelStride() const
{
    return m_uiPixelStride;
}
//---------------------------------------------------------------------------
inline size_t NiPersistentSrcTextureRendererData::GetSizeInBytes(
    unsigned int uiMipmapLevel, 
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_puiOffsetInBytes[uiMipmapLevel + 1] -
        m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline size_t NiPersistentSrcTextureRendererData::GetUsedSizeInBytes(
    unsigned int uiMipmapLevel, 
    unsigned int uiFace) const
{
    NI_UNUSED_ARG(uiFace);
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    if (uiMipmapLevel == m_uiMipmapLevels - 1)
    {
        // Compensate for the fact that m_puiOffsetInBytes[m_uiMipmapLevels],
        // which represents the total face size, is sometimes larger than
        // expected, due to alignment padding for some platforms.
        return (m_uiPadOffsetInBytes - m_puiOffsetInBytes[uiMipmapLevel]);
    }
    else
    {
        return m_puiOffsetInBytes[uiMipmapLevel + 1] -
            m_puiOffsetInBytes[uiMipmapLevel];
    }
}
//---------------------------------------------------------------------------
inline size_t NiPersistentSrcTextureRendererData::GetTotalSizeInBytes() const
{
    return m_puiOffsetInBytes[m_uiMipmapLevels] * m_uiFaces;
}
//---------------------------------------------------------------------------
inline size_t NiPersistentSrcTextureRendererData::GetFaceSizeInBytes() const
{
    return m_puiOffsetInBytes[m_uiMipmapLevels];
}
//---------------------------------------------------------------------------
inline ptrdiff_t NiPersistentSrcTextureRendererData::GetOffsetInBytes(
    unsigned int uiMipmapLevel, unsigned int uiFace) const
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return uiFace*GetFaceSizeInBytes() + m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned int NiPersistentSrcTextureRendererData::GetPadOffsetInBytes()
{
    return m_uiPadOffsetInBytes;
}
//---------------------------------------------------------------------------
inline NiPixelFormat& NiPersistentSrcTextureRendererData::GetPixelFormat()
{
    return m_kPixelFormat;
}
//---------------------------------------------------------------------------
inline const NiPixelFormat& NiPersistentSrcTextureRendererData::GetPixelFormat(
    ) const
{
    return m_kPixelFormat;
}
//---------------------------------------------------------------------------
inline const NiPalette* NiPersistentSrcTextureRendererData::GetPalette() const
{
    return m_spPalette;
}
//---------------------------------------------------------------------------
inline NiPalette* NiPersistentSrcTextureRendererData::GetPalette()
{
    return m_spPalette;
}
//---------------------------------------------------------------------------
inline const unsigned char* NiPersistentSrcTextureRendererData::GetPixels(
    unsigned int uiMipmapLevel, unsigned int uiFace) const
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_pucPixels + uiFace*m_puiOffsetInBytes[m_uiMipmapLevels] + 
        m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline unsigned char* NiPersistentSrcTextureRendererData::GetPixels(
    unsigned int uiMipmapLevel, unsigned int uiFace)
{
    NIASSERT(uiFace < m_uiFaces);
    NIASSERT(uiMipmapLevel < m_uiMipmapLevels);
    return m_pucPixels + uiFace*m_puiOffsetInBytes[m_uiMipmapLevels] +
        m_puiOffsetInBytes[uiMipmapLevel];
}
//---------------------------------------------------------------------------
inline const NiSourceTexture* NiPersistentSrcTextureRendererData::GetTexture()
    const
{
    return (NiSourceTexture*)m_pkTexture;
}
//---------------------------------------------------------------------------
inline NiSourceTexture* NiPersistentSrcTextureRendererData::GetTexture()
{
    return (NiSourceTexture*)m_pkTexture;
}
//---------------------------------------------------------------------------
inline void NiPersistentSrcTextureRendererData::SetPixels(
    unsigned char* pucPixels)
{
    m_pucPixels = pucPixels;
}
//---------------------------------------------------------------------------
inline void NiPersistentSrcTextureRendererData::SetWidths(
    unsigned int* puiWidths)
{
    m_puiWidth = puiWidths;
}
//---------------------------------------------------------------------------
inline void NiPersistentSrcTextureRendererData::SetHeights(
    unsigned int* puiHeights)
{
    m_puiHeight = puiHeights;
}
//---------------------------------------------------------------------------
inline void NiPersistentSrcTextureRendererData::SetOffsetsInBytes(
    unsigned int* puiOffsetsInBytes)
{
    m_puiOffsetInBytes = puiOffsetsInBytes;
}
//---------------------------------------------------------------------------
inline void NiPersistentSrcTextureRendererData::SetPadOffsetInBytes(
    unsigned int uiPadOffsetInBytes)
{
    m_uiPadOffsetInBytes = uiPadOffsetInBytes;
}
//---------------------------------------------------------------------------
inline void NiPersistentSrcTextureRendererData::SetTexture(
    NiSourceTexture* pkTexture)
{
    m_pkTexture = (NiTexture*)pkTexture;
}
//---------------------------------------------------------------------------
