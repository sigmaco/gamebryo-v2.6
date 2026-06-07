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


#ifndef NIPERSISTENTSRCTEXTURERENDERERDATA_H
#define NIPERSISTENTSRCTEXTURERENDERERDATA_H

#include "NiDevImageConverter.h"
#include "NiTexture.h"
#include "NiPalette.h"
#include "NiPixelFormat.h"

class NiSourceTexture;

class NIMAIN_ENTRY NiPersistentSrcTextureRendererData :
    public NiTexture::RendererData
{
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareViewerStrings;

public:
    typedef enum
    {
        NI_ANY = 0, // Generic case (default).
        NI_XENON,
        NI_PS3,
        NI_DX9, // Win32 may not always be unique; e.g., if multiple DX
                // versions coexist in a release.
        NI_WII,
        NI_D3D10,
        NI_NUM_PLATFORM_IDS
    } PlatformID;

    static NiPersistentSrcTextureRendererData* Create(
        NiSourceTexture* pkTexture, const PlatformID eTargetPlatform);
    ~NiPersistentSrcTextureRendererData();

    // The base image is counted as a mip-map level.
    inline unsigned int GetNumMipmapLevels() const;

    inline unsigned int GetNumFaces() const;

    inline const PlatformID GetTargetPlatform() const;
    PlatformID GetTargetPlatform();

    inline unsigned int GetWidth(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0) const;
    inline unsigned int GetHeight(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0) const;

    inline unsigned int GetPixelStride() const;

    inline size_t GetSizeInBytes(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0) const;
    inline size_t GetUsedSizeInBytes(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0) const;
    inline ptrdiff_t GetOffsetInBytes(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0) const;

    inline size_t GetTotalSizeInBytes() const;
    inline size_t GetFaceSizeInBytes() const;

    inline NiPixelFormat& GetPixelFormat();
    inline const NiPixelFormat& GetPixelFormat() const;

    inline const NiPalette* GetPalette() const;
    inline NiPalette* GetPalette();

    inline const unsigned char* GetPixels(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0) const;
    inline unsigned char* GetPixels(unsigned int uiMipmapLevel = 0,
        unsigned int uiFace = 0);

    inline void SetTexture(NiSourceTexture* pkTexture);
    inline const NiSourceTexture* GetTexture() const;
    inline NiSourceTexture* GetTexture();

    inline unsigned int GetPadOffsetInBytes();

    inline static void SetToolModeStreaming(bool bToolModeStreaming)
        {ms_bToolModeStreaming = bToolModeStreaming;} ;
    inline static bool GetToolModeStreaming() {return ms_bToolModeStreaming;} ;

    // *** begin Emergent internal use only ***
    NiPersistentSrcTextureRendererData(NiSourceTexture* pkTexture,
        unsigned int uiWidth, unsigned int uiHeight, unsigned int uiFaces,
        unsigned int uiMipmapLevels, unsigned int uiPixelStride,
        NiPixelFormat& kPixelFormat, NiPalette* pkPal,
        PlatformID eTargetPlatform);

    inline void SetPixels(unsigned char* pucPixels);
    inline void SetWidths(unsigned int* puiWidths);
    inline void SetHeights(unsigned int* puiHeights);
    inline void SetOffsetsInBytes(unsigned int* puiOffsetsInBytes);
    inline void SetPadOffsetInBytes(unsigned int uiPadOffsetInBytes);

    static void ResetStreamingFunctions();
    // *** end Emergent internal use only ***

protected:
    NiPersistentSrcTextureRendererData();

    const NiPixelFormat* FindClosestPixelFormat(
        const PlatformID eTargetPlatform, const NiTexture::FormatPrefs& kPrefs,
        const NiPixelFormat& kSrcFmt, NiDevImageConverter* pkConverter);

    virtual void AllocateData(unsigned int uiLevels, unsigned int uiFaces,
        unsigned int uiBytes);
    virtual void FreeData();

    unsigned char* m_pucPixels; // [pixels for all miplevels]

    unsigned int* m_puiWidth;   // [miplevels] 
    unsigned int* m_puiHeight;  // [miplevels] 

    unsigned int* m_puiOffsetInBytes;   // [miplevels+1] 
    // Note that m_puiOffsetInBytes[m_uiMipmapLevels] is the same as the total
    // size of a given face's entire mipmap pyramid.

    // Padding required for alignment on some platforms.  The amount of padding
    // is:  m_puiOffsetInBytes[m_uiMipmapLevels] - m_uiPadOffsetInBytes.
    unsigned int m_uiPadOffsetInBytes;    
    
    unsigned int m_uiMipmapLevels;
    unsigned int m_uiPixelStride;
    unsigned int m_uiFaces;

    // Target platform specified by the user at export time.
    PlatformID m_eTargetPlatform;

    // Pristine copies of data to guarantee data may be streamed out that is
    // identical to data that is streamed in.  This approach may be needed in
    // case the platform SDK manipulates the data on load.  This approach is
    // used only if ms_bToolModeStreaming is true.

    unsigned char* m_pucPristinePixels; // [pixels for all miplevels]

    // This max. offset is the same as the total size of a given face's entire
    // mipmap pyramid.
    unsigned int m_uiPristineMaxOffsetInBytes;

    unsigned int m_uiPristinePadOffsetInBytes;    

    NiPalettePtr m_spPalette;

    static bool ms_bToolModeStreaming;
};

NiSmartPointer(NiPersistentSrcTextureRendererData);

#include "NiPersistentSrcTextureRendererData.inl"

#endif
