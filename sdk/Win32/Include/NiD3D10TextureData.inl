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
//  NiD3D10TextureData Inline Functions
//---------------------------------------------------------------------------

inline NiD3D10RenderedTextureData* NiD3D10TextureData::GetAsRenderedTexture()
{
    return (IsRenderedTexture() ? (NiD3D10RenderedTextureData*)this : NULL);
}
//---------------------------------------------------------------------------
inline NiD3D10SourceTextureData* NiD3D10TextureData::GetAsSourceTexture()
{
    return (IsSourceTexture() ? (NiD3D10SourceTextureData*)this : NULL);
}
//---------------------------------------------------------------------------
inline NiD3D10DynamicTextureData* NiD3D10TextureData::GetAsDynamicTexture()
{
    return (IsDynamicTexture() ? (NiD3D10DynamicTextureData *)this : NULL);
}

//---------------------------------------------------------------------------
inline ID3D10Resource* NiD3D10TextureData::GetD3DTexture() const
{
    return m_pkD3D10Texture;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsRenderedTexture() const
{
    return (m_usTextureType & TEXTURETYPE_RENDERED) != 0;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsSourceTexture() const
{
    return (m_usTextureType & TEXTURETYPE_SOURCE) != 0;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsDynamicTexture() const
{
    return (m_usTextureType & TEXTURETYPE_DYNAMIC) != 0;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsCubeMap() const
{
    return (m_usTextureType & TEXTURETYPE_CUBE) != 0;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsTexture1D() const
{
    return (m_usTextureType & TEXTURETYPE_1D) != 0;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsTexture2D() const
{
    return (m_usTextureType & TEXTURETYPE_2D) != 0;
}
//---------------------------------------------------------------------------
inline bool NiD3D10TextureData::IsTexture3D() const
{
    return (m_usTextureType & TEXTURETYPE_3D) != 0;
}
//---------------------------------------------------------------------------
inline unsigned short NiD3D10TextureData::GetLevels() const
{
    return m_usLevels;
}

//---------------------------------------------------------------------------
inline ID3D10ShaderResourceView* NiD3D10TextureData::GetResourceView() const
{
    return m_pkResourceView;
}
//---------------------------------------------------------------------------
inline ID3D10Resource* NiD3D10TextureData::GetResource() const
{
    return m_pkD3D10Texture;
}

