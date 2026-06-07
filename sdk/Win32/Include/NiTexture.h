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


#ifndef NITEXTURE_H
#define NITEXTURE_H

#include "NiObjectNET.h"
#include "NiMainLibType.h"
#include "NiColor.h"
#include "NiPixelFormat.h"
#include "NiPixelData.h"

NiSmartPointer(NiImageConverter);
NiSmartPointer(NiPixelData);
NiSmartPointer(NiTexture);

class NIMAIN_ENTRY NiTexture : public NiObjectNET
{
    NiDeclareRTTI;
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    NiTexture();
    virtual ~NiTexture();

    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    class FormatPrefs  : public NiMemObject
    {
    public:
        // Creates a format preferences instance with DEFAULT values
        // for all data members
        FormatPrefs();

        // Preferred "high-level" pixel layout -- does not dictate
        // the exact pixel *format* (i.e. NiPixelFormat) for the image
        // but rather an application level of pixel setup.
        enum PixelLayout
        {
            PALETTIZED_8,
            HIGH_COLOR_16,
            TRUE_COLOR_32,
            COMPRESSED,
            BUMPMAP,
            PALETTIZED_4,
            PIX_DEFAULT,
            SINGLE_COLOR_8,
            SINGLE_COLOR_16,
            SINGLE_COLOR_32,
            DOUBLE_COLOR_32,
            DOUBLE_COLOR_64,
            FLOAT_COLOR_32,
            FLOAT_COLOR_64,
            FLOAT_COLOR_128,
            SINGLE_COLOR_4
        };
        PixelLayout m_ePixelLayout;
        
        // Alpha preference
        enum AlphaFormat
        {
            NONE, 
            BINARY, // 1-bit
            SMOOTH, // multi-bit
            ALPHA_DEFAULT
        };
        AlphaFormat m_eAlphaFmt;

        // Mip-mapping preference
        enum MipFlag
        {
            NO = false,
            YES = true,
            MIP_DEFAULT
        }; 
        MipFlag m_eMipMapped;
    };

    // Get a reference to the application format preferences.
    inline const FormatPrefs& GetFormatPreferences() const;

    static bool GetMipmapByDefault();
    static void SetMipmapByDefault(bool bMipmap);

    // Get a pointer to a pixel descriptor that matches the format that will
    // be used by the renderer.
    inline const NiPixelFormat* GetPixelFormat() const;

    /*** begin Emergent internal use only ***/

    // Renderer specific data class
    class NIMAIN_ENTRY RendererData : public NiObject
    {
    public:
        RendererData(NiTexture* pkTexture);
        virtual ~RendererData();

        inline unsigned int GetWidth() const;
        inline unsigned int GetHeight() const;
        inline const NiPixelFormat* GetPixelFormat() const;
        NiTexture* GetTexture(){ return m_pkTexture; }

        enum CreationFailure
        {
            NONE                            = 0x00000000,
            GENERIC                         = 0x80000000,
            NO2DMATCH                       = 0x80000001,
            NOCUBEMAPMATCH                  = 0x80000002,
            NOCONVERTER                     = 0x80000003,
            FAILEDCONVERT                   = 0x80000004,
            FILENOTFOUND                    = 0x80000005,
            BADDIMENSIONS                   = 0x80000006,
            FILEFORMATNOTSUPPORTED          = 0x80000007,
            FILELOADFAILED                  = 0x80000008,
            CUBEMAPFAILEDCONVERT            = 0x80000009,
            CUBEMAPFILENOTFOUND             = 0x8000000a,
            CUBEMAPBADDIMENSIONS            = 0x8000000b,
            CUBEMAPFILEFORMATNOTSUPPORTED   = 0x8000000c,
            CUBEMAPFILELOADFAILED           = 0x8000000d,
            
            // New failures go ABOVE this enum, just for readability
            // Make sure to update the COUNT value below. It should
            // be the (last value & 0x7fffffff) + 1
            CREATEFAIL_COUNT    = (CUBEMAPFILELOADFAILED & 0x7fffffff) + 1
        };

        static void _SDMInit();
        static void _SDMShutdown();

        typedef NiPixelData* (*TEXTUREFAILUREFUNC)(NiTexture* pkTexture, 
            CreationFailure eFailure);

        static void SetTextureFailureCallback(TEXTUREFAILUREFUNC pfnFunc);

        static NiPixelData* DefaultTextureFailureCallback(NiTexture* pkTexture,
            CreationFailure eFailure);

        static void SetTextureReplacementFormat(const NiPixelFormat& kFormat);

    protected:
        RendererData()  {}  ;   // Constructor for streaming.

        static NiPixelData* GetReplacementData(NiTexture* pkTexture, 
            CreationFailure eFailure, bool bReport = true);
        static NiPixelData* MakeDataFromColors(const NiColorA& kTopLeftColor,
            const NiColorA& kBottomLeftColor, const NiPixelFormat& kDesc, 
            unsigned int uiNumFaces);
        static void ReportDataCreationError(CreationFailure eFailure);

        NiTexture* m_pkTexture;
        unsigned int m_uiWidth;
        unsigned int m_uiHeight;
        NiPixelFormat m_kPixelFormat;

        // Default TEXTUREFAILUREFUNC function, called when a texture fails to
        // open or convert.
        static TEXTUREFAILUREFUNC ms_pfnTextureFailureCallback;

        static NiPixelDataPtr ms_aspReplacementData[CREATEFAIL_COUNT];
        static NiPixelFormat ms_kReplacementFormat;
    };

    inline void SetRendererData(RendererData* pkRendererData);
    RendererData* GetRendererData() const;

    inline NiTexture* GetListNext();
    inline NiTexture* GetListPrev();

    static NiTexture* GetListHead();
    static NiTexture* GetListTail();

    static void LockTextureList();
    static void UnlockTextureList();

    /*** end Emergent internal use only ***/

protected:
    inline void AddTextureToList();
    inline void RemoveTextureFromList();

    FormatPrefs m_kFormatPrefs;
    RendererData* m_pkRendererData;

    // Doubly linked list traversal members for list of all textures
    NiTexture* m_pkPrev;
    NiTexture* m_pkNext;
    static NiTexture* ms_pkHead;
    static NiTexture* ms_pkTail;
    static NiCriticalSection ms_kTextureListCriticalSection;

    static bool ms_bMipmapByDefault;
};

#include "NiTexture.inl"

#endif
