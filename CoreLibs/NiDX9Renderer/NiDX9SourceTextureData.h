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
#ifndef NIDX9SOURCETEXTUREDATA_H
#define NIDX9SOURCETEXTUREDATA_H

#include "NiDX9PersistentSrcTextureRendererData.h"
#include "NiDX9TextureData.h"

class NIDX9RENDERER_ENTRY NiDX9SourceTextureData : 
    public NiDX9TextureData
{
public:
    static NiDX9SourceTextureData* Create(NiSourceTexture* pkTexture, 
        NiDX9Renderer* pkRenderer);
    virtual ~NiDX9SourceTextureData();

    virtual void Update();
    virtual NiPalette* GetPalette();

    static bool SetMipmapSkipLevel(unsigned int uiSkip);
    static unsigned int GetMipmapSkipLevel();

protected:
    NiDX9SourceTextureData(NiSourceTexture* pkTexture, 
        NiDX9Renderer* pkRenderer);

    virtual bool InitializeFromD3DTexture(
        D3DBaseTexturePtr pkD3DTexture);

    bool CreateSurf(const NiPixelData& kPixels);
    bool CreateSurfFromRendererData(const
        NiDX9PersistentSrcTextureRendererData* pkPersistentSrcRendererData,
        D3DFORMAT eD3DFmt);

    void CopyDataToSurface(const NiPixelData& kPixels);
    void CopyRendererDataToSurface(const NiDX9PersistentSrcTextureRendererData*
        pkPersistentSrcRendererData);

    static void CopyDataToSurfaceLevel(const NiPixelData& kPixels,
        unsigned int uiLevel, D3DSurfacePtr pkMipLevel,
        unsigned int uiFace = 0);
    static void CopyRendererDataToSurfaceLevel(const
        NiDX9PersistentSrcTextureRendererData* pkPersistentSrcRendererData,
        unsigned int uiLevel, D3DSurfacePtr pkMipLevel,
        unsigned int uiFace = 0);

    bool UpdatePalette(NiPalette* pkNewPalette);

    bool m_bReplacementData;
    bool m_bMipmap;
    unsigned int m_uiFormattedSize;

    NiPalettePtr m_spPalette;

    unsigned int m_uiLevelsSkipped;

    static unsigned int ms_uiSkipLevels;

    bool LoadTextureFile(const char* pcFilename, NiDX9Renderer* pkRenderer,
        const NiTexture::FormatPrefs& kPrefs);

    unsigned int m_uiSourceRevID;   // can be compared with source PixelData
    unsigned int m_uiPalRevID;      // used w/ palettized src & RGB(A) dest
};

typedef NiPointer<NiDX9SourceTextureData> NiDX9SourceTextureDataPtr;

#include "NiDX9SourceTextureData.inl"

#endif  //#ifndef NIDX9SOURCETEXTUREDATA_H
