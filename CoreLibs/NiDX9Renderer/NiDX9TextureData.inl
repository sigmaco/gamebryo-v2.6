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
//  NiDX9TextureData Inline Functions
//---------------------------------------------------------------------------
inline NiDX9RenderedTextureData* NiDX9TextureData::GetAsRenderedTexture()
{
    return (IsRenderedTexture() ? (NiDX9RenderedTextureData*)this : NULL);
}
//---------------------------------------------------------------------------
inline NiDX9SourceTextureData* NiDX9TextureData::GetAsSourceTexture()
{
    return (IsSourceTexture() ? (NiDX9SourceTextureData*)this : NULL);
}
//---------------------------------------------------------------------------
inline NiDX9DynamicTextureData* NiDX9TextureData::GetAsDynamicTexture()
{
    return (IsDynamicTexture() ? (NiDX9DynamicTextureData *)this : NULL);
}
//---------------------------------------------------------------------------
inline bool NiDX9TextureData::IsRenderedTexture() const
{
    return (m_usTextureType & TEXTURETYPE_RENDERED) != 0;
}
//---------------------------------------------------------------------------
inline bool NiDX9TextureData::IsSourceTexture() const
{
    return (m_usTextureType & TEXTURETYPE_SOURCE) != 0;
}
//---------------------------------------------------------------------------
inline bool NiDX9TextureData::IsDynamicTexture() const
{
    return (m_usTextureType & TEXTURETYPE_DYNAMIC) != 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9TextureData::GetLevels() const
{
    return m_usLevels;
}
//---------------------------------------------------------------------------
inline D3DBaseTexturePtr NiDX9TextureData::GetD3DTexture() const
{
    return m_pkD3DTexture;
}
//---------------------------------------------------------------------------
