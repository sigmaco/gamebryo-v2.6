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

#ifndef NID3D10SOURCETEXTUREDATA_H
#define NID3D10SOURCETEXTUREDATA_H

#include "NiD3D10RendererLibType.h"

#include "NiD3D10PersistentSrcTextureRendererData.h"
#include "NiD3D10TextureData.h"

class NiSourceTexture;

class NID3D10RENDERER_ENTRY NiD3D10SourceTextureData : 
    public NiD3D10TextureData
{
public:
    static NiD3D10SourceTextureData* Create(NiSourceTexture* pkTexture);
    virtual ~NiD3D10SourceTextureData();

    virtual void Update();

    static bool SetMipmapSkipLevel(unsigned short usSkip);
    static unsigned short GetMipmapSkipLevel();

protected:
    NiD3D10SourceTextureData(NiSourceTexture* pkTexture);

    bool LoadTexture();

    bool InitializeTexture(const NiPixelData* pkPixels, bool bCube, 
        bool bStatic);
    bool NiD3D10SourceTextureData::InitializeTexture(
        const NiD3D10PersistentSrcTextureRendererData* pkData,
        bool bCube, bool bStatic);
    bool UpdateTexture(const NiPixelData* pkPixels);
    bool UpdatePalette(NiPalette* pkNewPalette);
    //bool LoadTextureFile(const char* pcFilename, NiD3D10Renderer* pkRenderer,
    //    const NiTexture::FormatPrefs& kPrefs);

    bool m_bReplacementData;
    bool m_bMipmap;

    unsigned int m_uiSourceRevID;   // can be compared with source PixelData
    unsigned int m_uiFormattedSize;

    unsigned short m_usLevelsSkipped;
    unsigned short m_usNumTextures;   // Texture array/cube map support

    NiPalettePtr m_spPalette;
    unsigned int m_uiPaletteRevID; 

    static unsigned short ms_usSkipLevels;
};

//#include "NiD3D10SourceTextureData.inl"

#endif  //#ifndef NID3D10SOURCETEXTUREDATA_H
