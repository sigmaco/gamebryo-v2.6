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

#ifndef NIMAXTEXTURECONVERTER_H
#define NIMAXTEXTURECONVERTER_H

#include <NiFlipController.h>
#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiTHash.h"
#include "NiDDSReader.h"

#define NI_TEXTURE_COORD_GEN_KEY "TextureCoordGen"
//---------------------------------------------------------------------------
class NiMAXTextureConverter
{
public:
    NiMAXTextureConverter(TimeValue kAnimStart, TimeValue kAnimEnd);
    ~NiMAXTextureConverter() {};
    
    
    // Possible flags returned by NfConvertTexture. They indicate
    // some additional properties required for a given texture map.
    enum Flags { HAS_ALPHA = 0x01,
                 VERTCOL_MAP = 0x02 };

    int Convert(
        Texmap* pkTm,
        NiTexturingProperty::Map*& pkMap, 
        NiTexturingProperty::MapClassID eMapClassID,
        NiFlipController*& pkFlipper,
        NiTexture::FormatPrefs& kPrefs,
        unsigned int& uiFlags,
        unsigned int& uiWidth,
        unsigned int& uiHeight,
        bool bIsntReflectionMap = true);

    static void SetSaveTexturesSeparately(bool bSaveTexuresSeparately);
    static bool GetSaveTexturesSeparately();

    static void SetUseOrigExt(bool bUseOrigExt);
    static bool GetUseOrigExt();

    static void SetUseOrigExtVRAM(bool bUseOrigExtVRAM);
    static bool GetUseOrigExtVRAM();

    static NiDevImageConverter::Platform GetPlatform();
    static void SetPlatform(NiDevImageConverter::Platform ePlatform);

    static NiTexture::FormatPrefs::PixelLayout GetPixelLayout();
    static void SetPixelLayout(
        NiTexture::FormatPrefs::PixelLayout 
        ePixelLayout);

    static void SetPerTextureOverride(bool override);
    static bool GetPerTextureOverride();

    static void SetLocalPixelLayout(
        NiTexture::FormatPrefs::PixelLayout 
        ePixelLayout);
    static NiTexture::FormatPrefs::PixelLayout GetLocalPixelLayout();

    static void Init();
    static void Shutdown();

    static bool GetDevImageQuantizerFound();

private:
    static NiPixelData *ShrinkColor64(
        Bitmap *pkSourceBitMap, 
        NiPixelFormat eFormat);
    static NiPixelData *ConvertColor64(
        Bitmap *pkSourceBitMap, 
        NiPixelFormat eFormat);
    static NiPixelData *ConvertHDR(Bitmap *pkSourceBitMap);
    static NiPixelData *ShrinkHDR(Bitmap *pkSourceBitMap);

    int NiMAXTextureConverter::ConvertBitmapTexture(
        BitmapTex* pkBm, 
        NiTexturingProperty::Map*& pkMap, 
        NiTexturingProperty::MapClassID eMapClassID,
        NiFlipController*& pkFlipper, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiFlags, unsigned int& uiWidth, 
        unsigned int& uiHeight);

    int NiMAXTextureConverter::ConvertSampledTexture(
        Texmap* pkTm, 
        NiTexturingProperty::Map*& pkMap, 
        NiTexturingProperty::MapClassID eMapClassID,
        NiFlipController*& pkFlipper, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiFlags, unsigned int& uiWidth, 
        unsigned int& uiHeight);

    int NiMAXTextureConverter::ConvertReflectionMapTexture(
        Texmap* pkTm, 
        NiTexturingProperty::Map*& pkMap, 
        NiTexturingProperty::MapClassID eMapClassID,
        NiFlipController*& pkFlipper, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiFlags, unsigned int& uiWidth, 
        unsigned int& uiHeight);

    int ConvertBitmap(
        Bitmap* pkSourceBitMap,
        BitmapInfo* pkBI,
        const TCHAR* pcFilePath,
        const TCHAR* pcFileName,
        NiTexture::FormatPrefs& kPrefs,
        NiTexture*& pkTexture,
        NiPixelFormat eFormat,
        unsigned int& uiSizeDivideBy, 
        NiTObjectArray<NiString>*& pkFilenames,
        bool bSaveTexturesSeparately, 
        bool bUseOrigExt,
        bool bUseOrigExtVRAM, 
        bool bAutoWriteTextures,
        bool bUseAlpha);

    bool ConvertBitmapByKnownDevice(
        Bitmap* pkSourceBitMap,
        BitmapInfo* pkBI,
        const TCHAR* pcFilePath,
        const TCHAR* pcFileName,
        NiTexture::FormatPrefs& kPrefs,
        NiTexture*& pkTexture,
        NiPixelFormat eFormat,
        unsigned int& uiSizeDivideBy, 
        NiTObjectArray<NiString>*& pkFilenames,
        bool bSaveTexturesSeparately, 
        bool bUseOrigExt,
        bool bUseOrigExtVRAM, 
        bool bAutoWriteTextures, 
        bool bUseAlpha);

    bool ConvertBitmapByUnknownDevice(
        Bitmap* pkSourceBitMap, 
        BitmapInfo* pkBI,
        const TCHAR* pcFilePath,
        const TCHAR* pcFileName,
        NiTexture::FormatPrefs& kPrefs,
        NiTexture*& pkTexture,
        NiPixelFormat kDestFormat,
        unsigned int& uiSizeDivideBy,
        NiTObjectArray<NiString>*& pkFilenames,
        bool bSaveTexturesSeparately,
        bool bUseOrigExt,
        bool bUseOrigExtVRAM,
        bool bAutoWriteTextures,
        bool bUseAlpha);


    int GetTexture(
        Bitmap* pkBitMap,
        BitmapInfo& kBI,
        const TCHAR* pcFilePath, 
        const TCHAR* pcFileName, 
        NiTexture*& pkTex, 
        NiTexture::FormatPrefs& kPrefs, 
        NiPixelFormat eFormat,
        unsigned int& uiSizeDivideBy, 
        NiTObjectArray<NiString>*& pkFilenames,
        bool bSaveTexturesSeparately, 
        bool bUseOrigExt, 
        bool bUseOrigExtVRAM,
        bool bAutoWriteTextures,  
        bool bUseAlpha);

    int GetAnimatedTexture(
        Bitmap* pkBitMap, 
        BitmapInfo& kBI,
        NiTexture*& pkTex, 
        NiTexture::FormatPrefs& kPrefs, 
        NiPixelFormat eFormat, 
        NiFlipController*& pkFlipper, 
        float fStart, 
        float fRate, 
        int iOOB, 
        unsigned int& uiSizeDivideBy,
        NiTObjectArray<NiString>*& pkFilenames, 
        bool bSaveTexturesSeparately, 
        bool bUseOrigExt,
        bool bUseOrigExtVRAM,
        bool bAutoWriteTextures,
        bool bUseAlpha);

    bool SupportLegacyMipMapSpecification(
        Bitmap* pkSourceBitMap, 
        BitmapInfo* pkBI, 
        NiTexture::FormatPrefs& kPrefs, 
        NiTexture*& pkTexture,
        NiPixelFormat eFormat,
        unsigned int& uiSizeDivideBy, 
        NiTObjectArray<NiString>*& pkFilenames,
        bool& bSaveTexturesSeparately, 
        bool& bUseOrigExt,  
        bool& bUseOrigExtVRAM);

    bool ComputeDefaultPixelLayout(
        Texmap* pkTm, 
        NiTexture::FormatPrefs& kPrefs, 
        unsigned int& uiSizeDivideBy, 
        NiTObjectArray<NiString>*& pkFilenames, 
        bool& bExternalImages,
        bool& bOrignalImages, 
        bool& bUseOrigExtVRAM, 
        NiTexturingProperty::FilterMode& eFilterMode, 
        NiPixelFormat& ePixelFormat,
        bool bImageHasAlpha);

    static TSTR NiMAXTextureConverter::GetFrameFilename(
        TSTR kFullname,
        int iFrame);

    static bool FormatSupportsAlpha(NiPixelFormat eFormat);

    static bool TextureHasAlpha(const NiTexture* pkTex);
    
    TimeValue m_kAnimStart, m_kAnimEnd;

    static NiTHash<NiTexturePtr>* ms_pkTexHTab;
    static NiTHash<NiSourceCubeMapPtr>* ms_pkCubeHTab;

    static bool ms_bSaveTexturesSeparately;
    static bool ms_bUseOrigExt;
    static bool ms_bUseOrigExtVRAM;
    static bool ms_bPerTextureOverride;
    static NiDevImageConverter::Platform ms_ePlatform;
    static NiTexture::FormatPrefs::PixelLayout ms_ePixelLayout;
    static NiTexture::FormatPrefs::PixelLayout ms_eLocalPixelLayout;
    static bool ms_bDevImageQuantizerFound;
    static NiDDSReader* ms_pkDDSReader;

    static void *ms_pvUserData;
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXTextureConverter.inl"

//---------------------------------------------------------------------------
#endif

