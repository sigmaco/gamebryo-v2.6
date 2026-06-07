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
#ifndef NIDX9TEXTUREDATA_H
#define NIDX9TEXTUREDATA_H

#include "NiDX9PixelFormat.h"
#include "NiSourceTexture.h"

class NiDX9Renderer;
class NiDX9RenderedTextureData;
class NiDX9SourceTextureData;
class NiDX9DynamicTextureData;

class NIDX9RENDERER_ENTRY NiDX9TextureData : public NiTexture::RendererData
{
public:
    NiDX9TextureData(NiTexture* pkTexture, NiDX9Renderer* pkRenderer);
    virtual ~NiDX9TextureData();

    inline unsigned int GetLevels() const;

    inline D3DBaseTexturePtr GetD3DTexture() const;

    inline NiDX9RenderedTextureData* GetAsRenderedTexture();
    inline NiDX9SourceTextureData* GetAsSourceTexture();
    inline NiDX9DynamicTextureData* GetAsDynamicTexture();

    inline bool IsRenderedTexture() const;
    inline bool IsSourceTexture() const;
    inline bool IsDynamicTexture() const;

    static const NiPixelFormat* FindClosestPixelFormat(
        const NiTexture::FormatPrefs& kPrefs,
        NiPixelFormat* const* ppkDestFmts);

    static void ClearTextureData(NiDX9Renderer* pkRenderer);

protected:
    const NiPixelFormat* FindMatchingPixelFormat(
        const NiPixelFormat& kSrcFmt, const NiTexture::FormatPrefs& kPrefs,
        NiPixelFormat** ppkDestFmts);

    virtual bool InitializeFromD3DTexture(
        D3DBaseTexturePtr pkD3DTexture);

    NiDX9Renderer* m_pkRenderer;

    D3DBaseTexturePtr m_pkD3DTexture;

    unsigned short m_usLevels;

    enum TextureTypeFlags
    {
        TEXTURETYPE_SOURCE      = 1 << 0,
        TEXTURETYPE_RENDERED    = 1 << 1,
        TEXTURETYPE_DYNAMIC     = 1 << 2
    };
    unsigned short m_usTextureType;
};

typedef NiPointer<NiDX9TextureData> NiDX9TextureDataPtr;

#include "NiDX9TextureData.inl"

#endif  //#ifndef NiDX9TEXTUREDATA_H
