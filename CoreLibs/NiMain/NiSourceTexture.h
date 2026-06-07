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

#ifndef NISOURCETEXTURE_H
#define NISOURCETEXTURE_H

#include "NiBool.h"
#include "NiPersistentSrcTextureRendererData.h"
#include "NiPixelData.h"
#include "NiTexture.h"

class NIMAIN_ENTRY NiSourceTexture : public NiTexture
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

protected:
    static FormatPrefs ms_kDefaultPrefs;
public:
    // Create from a filename
    static NiSourceTexture* Create(const NiFixedString& kFilename,
        FormatPrefs& kPrefs = ms_kDefaultPrefs,
        bool bLoadDirectToRendererHint = false,
        bool bSrcRendererDataIsPersistent = false);

    // Create from application pixel data
    static NiSourceTexture* Create(NiPixelData* pkRaw,
        FormatPrefs& kPrefs = ms_kDefaultPrefs);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;

    // Filename will be null if created from pixel data
    inline const NiFixedString& GetFilename() const;
    void SetFilename(const NiFixedString& kFilename);

    // This is the filename that is actually used to read from disk
    inline const NiFixedString& GetPlatformSpecificFilename() const;

    // PixelData will be null if image was created from PixelData and
    // has been marked as static OR if image was created from a file 
    // and LoadPixelDataFromFile has not been called.
    inline NiPixelData* GetSourcePixelData();
    inline const NiPixelData* GetSourcePixelData() const;

    // NiPersistentSrcTextureRendererData will be null if no platform-specific
    // source texture renderer data was streamed in (or otherwise exists).
    inline NiPersistentSrcTextureRendererData* GetPersistentSourceRendererData();
    inline const NiPersistentSrcTextureRendererData*
        GetPersistentSourceRendererData() const;
    inline void SetPersistentSourceRendererData(
        NiPersistentSrcTextureRendererData* pkPersistentSrcRendererData);

    // Create the application level backing store for an image
    // loaded from file.  By default this backing store is not 
    // created, instead only the renderer specific store is built.
    virtual void LoadPixelDataFromFile();

    // Destroy the application level backing store -- this can be done
    // if an application has no need to retain access to a version of
    // the pixel data for its own modification or otherwise.
    virtual void DestroyAppPixelData();

    // Gives texture/renderer freedom to assume that a texture (i.e.
    // its pixel data) will not change.  This can offer a sizable
    // optimization for renderer image setup costs.  
    inline void SetStatic(bool bStatic);
    inline bool GetStatic() const;

    // Supplies a hint to the renderer indicating whether to load the
    // texture directly into the internal data structures, bypassing
    // the application-accessible NiPixelData, as well as all texture
    // conversion, entirely.
    inline void SetLoadDirectToRendererHint(bool bDirect);
    inline bool GetLoadDirectToRendererHint() const;

    // Indicates whether there is persistent platform-specific source texture
    // renderer data in the Nif file which may be streamed.  If present, then
    // Gamebryo is able to load the texture (already in optimal format for a
    // specific platform) directly into renderer-specific data structures.
    // This step bypasses the application-level NiPixelData object, and all
    // texture conversion, entirely.
    inline void SetSourceRendererDataIsPersistent(bool bSrcRendererDataIsPersistent);
    inline bool GetSourceRendererDataIsPersistent() const;

    // Default behavior is to always preload.  Application must turn
    // off preloading if they want otherwise
    static void SetUsePreloading(bool bPreload);
    static bool GetUsePreloading();

    // Default behavior is to always mipmap.  Application must turn
    // off mipmapping if they want otherwise
    static void SetUseMipmapping(bool bMipmap);
    static bool GetUseMipmapping();

    // Default behavior is to destroy the NiPixelData object once the
    // texture is loaded into the renderer.  Application must turn
    // off this feature if they want otherwise.
    static void SetDestroyAppDataFlag(bool bDestroy);
    static bool GetDestroyAppDataFlag();

    /*** begin Emergent internal use only ***/

    // streaming support
    virtual void PostLinkObject(NiStream& kStream);

    /*** end Emergent internal use only ***/

protected:
    NiSourceTexture();
    virtual ~NiSourceTexture();

    virtual bool CreateRendererData();

    NiFixedString m_kFilename;
    NiFixedString m_kPlatformFilename;
    NiPersistentSrcTextureRendererDataPtr m_spPersistentSrcRendererData;
    NiPixelDataPtr m_spSrcPixelData;

    bool m_bStatic;
    bool m_bLoadDirectToRendererHint;
    bool m_bSrcRendererDataIsPersistent;

    static bool ms_bPreload;
    static bool ms_bMipmap;
    static bool ms_bDestroyAppData;
};

typedef NiPointer<NiSourceTexture> NiSourceTexturePtr;

//---------------------------------------------------------------------------
#include "NiSourceTexture.inl"
//---------------------------------------------------------------------------

#endif
