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

// Precompiled Header
#include "NiMainPCH.h"

#include <NiSearchPath.h>
#include <NiSystem.h>
#include "NiImageConverter.h"
#include "NiRenderer.h"
#include "NiSourceTexture.h"

NiImplementRTTI(NiSourceTexture, NiTexture);

//---------------------------------------------------------------------------
// Class static data members
//---------------------------------------------------------------------------
NiSourceTexture::FormatPrefs NiSourceTexture::ms_kDefaultPrefs;
bool NiSourceTexture::ms_bPreload = true;
bool NiSourceTexture::ms_bMipmap = true;
bool NiSourceTexture::ms_bDestroyAppData = true;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiSourceTexture* NiSourceTexture::Create(const NiFixedString& kFilename,
    FormatPrefs& kPrefs, bool bLoadDirectToRendererHint,
    bool bSrcRendererDataIsPersistent)
{
    NiSourceTexture* pkTexture = NiNew NiSourceTexture;
    NIASSERT(pkTexture);

    pkTexture->m_kFormatPrefs = kPrefs;
    pkTexture->m_bLoadDirectToRendererHint = bLoadDirectToRendererHint;

    pkTexture->m_bSrcRendererDataIsPersistent = bSrcRendererDataIsPersistent;

    pkTexture->m_kFilename = kFilename;
    NiStandardizeFilePath(pkTexture->m_kFilename);

    NiImageConverter::ConvertFilenameToPlatformSpecific(
        pkTexture->m_kFilename, pkTexture->m_kPlatformFilename);

    if (ms_bPreload && !pkTexture->CreateRendererData())
    {
        NiDelete pkTexture;
        return 0;
    }
    else
    {
        return pkTexture;
    }
}
//---------------------------------------------------------------------------
NiSourceTexture* NiSourceTexture::Create(NiPixelData* pkPixelData,
   FormatPrefs& kPrefs)
{
    NiSourceTexture* pkTexture = NiNew NiSourceTexture;
    NIASSERT(pkTexture);

    pkTexture->m_kFormatPrefs = kPrefs;
    pkTexture->m_spSrcPixelData = pkPixelData;

    if (ms_bPreload && !pkTexture->CreateRendererData())
    {
        NiDelete pkTexture;
        return 0;
    }
    else
    {
        return pkTexture;
    }
}
//---------------------------------------------------------------------------
bool NiSourceTexture::CreateRendererData()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer && !pkRenderer->CreateSourceTextureRendererData(this))
        return false;

    if (ms_bDestroyAppData && GetRendererData() && m_bStatic)
        DestroyAppPixelData();

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiSourceTexture::GetWidth() const
{
    if (m_pkRendererData)
        return m_pkRendererData->GetWidth();
    else if (m_spPersistentSrcRendererData)
        return m_spPersistentSrcRendererData->GetWidth();
    else if (m_spSrcPixelData)
        return m_spSrcPixelData->GetWidth();
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiSourceTexture::GetHeight() const
{
    if (m_pkRendererData)
        return m_pkRendererData->GetHeight();
    else if (m_spPersistentSrcRendererData)
        return m_spPersistentSrcRendererData->GetHeight();
    else if (m_spSrcPixelData)
        return m_spSrcPixelData->GetHeight();
    else
        return 0;
}
//---------------------------------------------------------------------------
void NiSourceTexture::LoadPixelDataFromFile()
{
    if (m_kFilename.Exists() && !m_spSrcPixelData &&
        !m_spPersistentSrcRendererData)
    {
        // Because the application cannot change the filename, we can 
        // safely assume that, if the source NiPixelData instance exists,
        // that the dimensions are correct.  So there is no need to 
        // destroy the pixel data.

        // This will fill the existing pixel data if available and matching
        NiPixelDataPtr spFileData 
            = NiImageConverter::GetImageConverter()->ReadImageFile(
            m_kPlatformFilename, m_spSrcPixelData);

        // If the existing image data could not be reused or did not
        // exist, point the source pixel data pointer to the new data
        if (m_spSrcPixelData)
        {
            if (spFileData != m_spSrcPixelData)
            {
                // We should only get here if the image could not be 
                // loaded into the existing data object - this would
                // seem to imply that the file or filename have changed
                m_spSrcPixelData = spFileData;
            }
        }
        else
        {
            m_spSrcPixelData = spFileData;
        }
    }
}
//---------------------------------------------------------------------------
void NiSourceTexture::DestroyAppPixelData()
{
    m_spSrcPixelData = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSourceTexture);
//---------------------------------------------------------------------------
void NiSourceTexture::LoadBinary(NiStream& kStream)
{
    NiTexture::LoadBinary(kStream);

    // IncRefCount() guarantees that "this" won't be deleted by
    // kStream.ChangeObject if this texture is a shared texture.
    IncRefCount();

    NiBool bExternalTexture = false;
        
    NiStreamLoadBinary(kStream, bExternalTexture);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kFilename);
    }
    else
    {
        kStream.LoadFixedString(m_kFilename);
    }

    kStream.SetNumberOfLinkIDs(1);

    // if ((kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 4))
    //     || (!m_bSrcRendererDataIsPersistent))
    //   then this LinkID resolves to:  m_spSrcPixelData
    //   else this LinkID resolves to:  m_spPersistentSrcRendererData
    // (See also LinkObject().)
    kStream.ReadLinkID();

    if (m_kFilename)
    {
        NiImageConverter::ConvertFilenameToPlatformSpecific(m_kFilename,
            m_kPlatformFilename);
    }

    if (bExternalTexture)
    {
        // Grab the search path object and prepare to search for the file.
        NiSearchPath& kPath = *kStream.GetSearchPath();
        kPath.SetFilePath(m_kPlatformFilename);
        kPath.Reset();

        // First, look for the file in the texture palette without even
        // touching the disk.
        char acFile[NI_MAX_PATH];
        NiTexture* pkShared = NULL;
        while (!pkShared && kPath.GetNextSearchPath(acFile, NI_MAX_PATH))
            pkShared = kStream.GetTexture(acFile, this);

        if (pkShared)
        {
            // Image with the given filename found in the texture palette.
            m_kPlatformFilename = acFile;

            // "this" will be deleted when DecRefCount() is called at the end
            // of LoadBinary().
            kStream.ChangeObject((NiSourceTexture*) pkShared, false);
        }
        else
        {    
            // Image not found in texture palette under any of the possible
            // paths - search for the image on disk at all of the same paths.
            kPath.Reset();
            while (kPath.GetNextSearchPath(acFile, NI_MAX_PATH))
            {
                if (NiFile::Access(acFile, NiFile::READ_ONLY))
                {
                    // Found the image on disk - use it.
                    m_kPlatformFilename = acFile;
                    kStream.SetTexture(m_kPlatformFilename, this);
                    break;
                }
            }
        }
    }
    else
    {
        if (m_kPlatformFilename.Exists())
        { 
            NiTexture* pkShared = kStream.GetTexture(
                m_kPlatformFilename, this);
            if (!pkShared)      
            {
                kStream.SetTexture(m_kPlatformFilename, this);
            }
            else
            {
                kStream.ChangeObject((NiSourceTexture*) pkShared, false);
            }
        }
    }

    NiStreamLoadEnum(kStream, m_kFormatPrefs.m_ePixelLayout);
    NiStreamLoadEnum(kStream, m_kFormatPrefs.m_eMipMapped);
    NiStreamLoadEnum(kStream, m_kFormatPrefs.m_eAlphaFmt);

    NiBool bStatic;
    NiStreamLoadBinary(kStream, bStatic);
    m_bStatic = bStatic ? true : false;

    // Prior to 10.1.0.103, there was no LoadDirectToRenderer hint.
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 103))
    {
        NiBool bLoadDirectToRendererHint;
        NiStreamLoadBinary(kStream, bLoadDirectToRendererHint);
        m_bLoadDirectToRendererHint = bLoadDirectToRendererHint ? true : false;
    }

    // Prior to Nif version 20.2.0.4, there was no SrcRendererDataIsPersistent
    // flag.  If the texture is an internal texture, load the flag from the Nif
    // file.
    if ((kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 4)))
    {
        NiBool bSrcRendererDataIsPersistent;
        NiStreamLoadBinary(kStream, bSrcRendererDataIsPersistent);
        m_bSrcRendererDataIsPersistent =
            bSrcRendererDataIsPersistent ? true : false;
    }

    NIASSERT(!((bExternalTexture) && (m_bSrcRendererDataIsPersistent)));

    DecRefCount();
}
//---------------------------------------------------------------------------
void NiSourceTexture::LinkObject(NiStream& kStream)
{
    NiTexture::LinkObject(kStream);

    if (kStream.GetNumberOfLinkIDs())
    {
        if ((kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 4))
            || (!m_bSrcRendererDataIsPersistent))
        {
            // This LinkID resolves to:  m_spSrcPixelData
            m_spSrcPixelData = (NiPixelData*)kStream.GetObjectFromLinkID();
            m_spPersistentSrcRendererData = 0;
        }
        else
        {
            // This LinkID resolves to:  m_spPersistentSrcRendererData
            m_spPersistentSrcRendererData =
                (NiPersistentSrcTextureRendererData*)
                kStream.GetObjectFromLinkID();
            m_spSrcPixelData = 0;
        }
    }
}
//---------------------------------------------------------------------------
void NiSourceTexture::PostLinkObject(NiStream& kStream)
{
    // This cannot be done in LinkObject, as we must assume that the 
    // NiPixelData object (if any) has been linked as well, in case the 
    // NiPixelData object contains a palette.
    if (ms_bPreload)
    {
        // Only precache the texture that will survive the destruction
        // of the stream, not the copies that have been replaced.
        NiTexture* pkStreamTexture = NULL;
        if (m_kPlatformFilename.Exists())
        { 
            pkStreamTexture = kStream.GetTexture(
                m_kPlatformFilename, this);
        }            

        // It may be the case that the source texture was never found,
        // or that it was procedurally generated and has no filename, 
        // in which case pkStreamTexture will be NULL. We should go ahead
        // and allow these textures to be precached, even if they
        // just load an error texture.
        if (pkStreamTexture == NULL || pkStreamTexture == this)
        {
            //Creating the renderer data may delete the pixels on a shared
            //texture, so we must protect the cleanup code in streaming.
            NiStream::LockCleanupSection();

            NIVERIFY(CreateRendererData());

            NiStream::UnlockCleanupSection();
        }
    }
}
//---------------------------------------------------------------------------
bool NiSourceTexture::RegisterStreamables(NiStream& kStream)
{
    if (!NiTexture::RegisterStreamables(kStream))
        return false;

    kStream.RegisterFixedString(m_kFilename);

    // Register the NiPersistentSrcTextureRendererDataPt only if it exists.
    if (m_spPersistentSrcRendererData)
        return m_spPersistentSrcRendererData->RegisterStreamables(kStream);

    // Else register the NiPixelData only if it exists.
    if (m_spSrcPixelData)
        return m_spSrcPixelData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiSourceTexture::SaveBinary(NiStream& kStream)
{
    NiTexture::SaveBinary(kStream);

    NiBool bExternalImage = ((m_spPersistentSrcRendererData == NULL) &&
        (m_spSrcPixelData == NULL));
    NiStreamSaveBinary(kStream, bExternalImage);

    kStream.SaveFixedString(m_kFilename);

    if (m_bSrcRendererDataIsPersistent)
        kStream.SaveLinkID(m_spPersistentSrcRendererData);
    else
        kStream.SaveLinkID(m_spSrcPixelData);

    // Stream individual components of FormatPrefs
    NiStreamSaveEnum(kStream, m_kFormatPrefs.m_ePixelLayout);
    NiStreamSaveEnum(kStream, m_kFormatPrefs.m_eMipMapped);
    NiStreamSaveEnum(kStream, m_kFormatPrefs.m_eAlphaFmt);

    NiBool bStatic = m_bStatic;
    NiStreamSaveBinary(kStream, bStatic);

    NiBool bLoadDirectToRendererHint = m_bLoadDirectToRendererHint;
    NiStreamSaveBinary(kStream, bLoadDirectToRendererHint);

    NiBool bSrcRendererDataIsPersistent = m_bSrcRendererDataIsPersistent;
    NiStreamSaveBinary(kStream, bSrcRendererDataIsPersistent);
}
//---------------------------------------------------------------------------
bool NiSourceTexture::IsEqual(NiObject* pkObject)
{
    if (!NiTexture::IsEqual(pkObject))
        return false;

    NiSourceTexture* pkTexture = (NiSourceTexture*)pkObject;

    if (m_kFilename != pkTexture->m_kFilename)
    {
        return false;
    }

    if ((m_spSrcPixelData && !pkTexture->m_spSrcPixelData) || 
        (!m_spSrcPixelData && pkTexture->m_spSrcPixelData) ||
        (m_spSrcPixelData && 
        !m_spSrcPixelData->IsEqual(pkTexture->m_spSrcPixelData)))
    {
        return false;
    }

    if ((m_spPersistentSrcRendererData &&
        !pkTexture->m_spPersistentSrcRendererData) || 
        (!m_spPersistentSrcRendererData &&
        pkTexture->m_spPersistentSrcRendererData) ||
        (m_spPersistentSrcRendererData && 
        !m_spPersistentSrcRendererData->IsEqual(
        pkTexture->m_spPersistentSrcRendererData)))
    {
        return false;
    }

    if ((m_kFormatPrefs.m_ePixelLayout != 
        pkTexture->m_kFormatPrefs.m_ePixelLayout) ||
        (m_kFormatPrefs.m_eMipMapped != 
        pkTexture->m_kFormatPrefs.m_eMipMapped) ||
        (m_kFormatPrefs.m_eAlphaFmt != 
        pkTexture->m_kFormatPrefs.m_eAlphaFmt) ||
        (m_bStatic != pkTexture->m_bStatic) ||
        (m_bLoadDirectToRendererHint != 
        pkTexture->m_bLoadDirectToRendererHint) ||
        (m_bSrcRendererDataIsPersistent != 
        pkTexture->m_bSrcRendererDataIsPersistent))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSourceTexture::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTexture::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiSourceTexture::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
void NiSourceTexture::SetFilename(const NiFixedString& kFilename)
{
    m_kFilename = kFilename;
    NiStandardizeFilePath(m_kFilename);

    NiImageConverter::ConvertFilenameToPlatformSpecific(m_kFilename,
        m_kPlatformFilename);
}
//---------------------------------------------------------------------------
