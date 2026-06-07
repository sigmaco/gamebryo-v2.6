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

#ifndef NID3D10TEXTUREDATA_H
#define NID3D10TEXTUREDATA_H

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"
#include "NiSourceTexture.h"
#include <NiTexture.h>

class NiD3D10RenderedTextureData;
class NiD3D10SourceTextureData;
class NiD3D10DynamicTextureData;

class NID3D10RENDERER_ENTRY NiD3D10TextureData : public NiTexture::RendererData
{
public:
    NiD3D10TextureData(NiTexture* pkTexture);
    virtual ~NiD3D10TextureData();

    inline unsigned short GetLevels() const;

    inline bool IsRenderedTexture() const;
    inline bool IsSourceTexture() const;
    inline bool IsDynamicTexture() const;
    inline bool IsCubeMap() const;

    // Return the appropriate resource type
    inline bool IsTexture1D() const;
    inline bool IsTexture2D() const;
    inline bool IsTexture3D() const;

    static void ClearTextureData();

    inline ID3D10Resource* GetD3DTexture() const;

    inline NiD3D10RenderedTextureData* GetAsRenderedTexture();
    inline NiD3D10SourceTextureData* GetAsSourceTexture();
    inline NiD3D10DynamicTextureData* GetAsDynamicTexture();


    inline ID3D10ShaderResourceView* GetResourceView() const;
    inline ID3D10Resource* GetResource() const;

    void SetResourceView(ID3D10ShaderResourceView* pkResourceView);

protected:
    const NiPixelFormat* FindMatchingPixelFormat(
        const NiPixelFormat& kSrcFmt, const NiTexture::FormatPrefs& kPrefs,
        D3D10_FORMAT_SUPPORT eSupportTest);
    
    const NiPixelFormat* FindMatchingPixelFormat(
        const NiTexture::FormatPrefs& kPrefs, 
        D3D10_FORMAT_SUPPORT eSupportTest);

    bool InitializeFromD3D10Resource(ID3D10Resource* pkD3D10Texture, 
        ID3D10ShaderResourceView* pkResourceView);

    unsigned short m_usLevels;

    enum TextureTypeFlags
    {
        TEXTURETYPE_SOURCE      = 1 << 0,
        TEXTURETYPE_RENDERED    = 1 << 1,
        TEXTURETYPE_DYNAMIC     = 1 << 2,
        TEXTURETYPE_CUBE        = 1 << 3,
        TEXTURETYPE_1D          = 1 << 13, // Supports texture arrays as well
        TEXTURETYPE_2D          = 1 << 14, // Supports texture arrays as well
        TEXTURETYPE_3D          = 1 << 15,
    };
    unsigned short m_usTextureType;


    ID3D10ShaderResourceView* m_pkResourceView;
    ID3D10Resource* m_pkD3D10Texture;
};

typedef NiPointer<NiD3D10TextureData> NiD3D10TextureDataPtr;

#include "NiD3D10TextureData.inl"

#endif  //#ifndef NiD3D10TEXTUREDATA_H
