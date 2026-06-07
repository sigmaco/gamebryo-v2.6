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
#include "NiSourceCubeMap.h"

NiImplementRTTI(NiSourceCubeMap, NiSourceTexture);

//---------------------------------------------------------------------------
NiSourceCubeMap* NiSourceCubeMap::Create(const NiFixedString& kFilename,
    NiRenderer* pkRenderer, FormatPrefs& kPrefs)
{
    NiSourceCubeMap* pkCubeMap = NiNew NiSourceCubeMap;
    NIASSERT(pkCubeMap);

    pkCubeMap->m_kFormatPrefs = kPrefs;

    pkCubeMap->m_kFilename = kFilename;
    NiStandardizeFilePath(pkCubeMap->m_kFilename);

    NiImageConverter::ConvertFilenameToPlatformSpecific(
            pkCubeMap->m_kFilename, pkCubeMap->m_kPlatformFilename);

    pkCubeMap->SetCurrentCubeFace(FACE_POS_X);
    if (ms_bPreload)
    {
        pkCubeMap->LoadPixelDataFromFile();

        if (pkRenderer && 
            !pkRenderer->CreateSourceCubeMapRendererData(pkCubeMap))
        {
            NiDelete pkCubeMap;
            return NULL;
        }
    }

    return pkCubeMap;
}
//---------------------------------------------------------------------------
NiSourceCubeMap* NiSourceCubeMap::Create(NiPixelData* pkCubeMapPixels, 
    NiRenderer* pkRenderer, FormatPrefs& kPrefs)
{
    if (pkCubeMapPixels->GetNumFaces() != 6)
        return NULL;

    NiSourceCubeMap* pkCubeMap = NiNew NiSourceCubeMap;
    NIASSERT(pkCubeMap);

    pkCubeMap->m_kFormatPrefs = kPrefs;

    pkCubeMap->SetCurrentCubeFace(FACE_POS_X);
    pkCubeMap->m_spSrcPixelData = pkCubeMapPixels;

    if (pkRenderer && !pkRenderer->CreateSourceCubeMapRendererData(pkCubeMap))
    {
        NiDelete pkCubeMap;
        return NULL;
    }

    return pkCubeMap;
}
//---------------------------------------------------------------------------
NiSourceCubeMap* NiSourceCubeMap::Create(const char* pcPosXFilename,
    const char* pcNegXFilename, const char* pcPosYFilename,
    const char* pcNegYFilename, const char* pcPosZFilename,
    const char* pcNegZFilename, NiRenderer* pkRenderer,
    FormatPrefs& kPrefs)
{
    NiOutputDebugString("WARNING! - Calling deprecated"
        " NiSourceCubeMap::Create.\n");

    NiSourceCubeMap* pkTexture = NiNew NiSourceCubeMap;
    NIASSERT(pkTexture);

    pkTexture->m_kFormatPrefs = kPrefs;
    NiSearchPath kPath;
    pkTexture->LoadFromFiles(pcPosXFilename, pcNegXFilename, pcPosYFilename,
        pcNegYFilename, pcPosZFilename, pcNegZFilename, kPath);
    pkTexture->SetCurrentCubeFace(FACE_POS_X);

    if (pkRenderer && !pkRenderer->CreateSourceCubeMapRendererData(pkTexture))
    {
        NiDelete pkTexture;
        return NULL;
    }

    return pkTexture;
}
//---------------------------------------------------------------------------
NiSourceCubeMap* NiSourceCubeMap::Create(NiPixelData* pkPosXRaw, 
    NiPixelData* pkNegXRaw, NiPixelData* pkPosYRaw, 
    NiPixelData* pkNegYRaw, NiPixelData* pkPosZRaw, 
    NiPixelData* pkNegZRaw, NiRenderer* pkRenderer,
    FormatPrefs& kPrefs)
{
    NiSourceCubeMap* pkTexture = NiNew NiSourceCubeMap;
    NIASSERT(pkTexture);

    pkTexture->m_kFormatPrefs = kPrefs;
    pkTexture->LoadFromPixelData(pkPosXRaw, pkNegXRaw, pkPosYRaw, pkNegYRaw,
        pkPosZRaw, pkNegZRaw);
    pkTexture->SetCurrentCubeFace(FACE_POS_X);

    if (pkRenderer && !pkRenderer->CreateSourceCubeMapRendererData(pkTexture))
    {
        NiDelete pkTexture;
        return NULL;
    }

    return pkTexture;
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::LoadPixelDataFromFile()
{
    if (!m_spSrcPixelData && m_kPlatformFilename.Exists())
    {
        NiImageConverter* pkConverter = NiImageConverter::GetImageConverter();
        NIASSERT(pkConverter);
        m_spSrcPixelData = pkConverter->ReadImageFile(m_kPlatformFilename,
            NULL);
    }
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::DestroyAppPixelData()
{
    m_spSrcPixelData = 0;
}
//---------------------------------------------------------------------------
NiSourceCubeMap::NiSourceCubeMap()
{
    m_eFace = FACE_POS_X;
}
//---------------------------------------------------------------------------
NiSourceCubeMap::~NiSourceCubeMap()
{
    m_spSrcPixelData = 0;
}
//---------------------------------------------------------------------------
bool NiSourceCubeMap::CreateRendererData()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();

    if (pkRenderer && !pkRenderer->CreateSourceCubeMapRendererData(this))
        return false;

    return true;   
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::LoadFromFiles(const char* pcPosXFilename,
    const char* pcNegXFilename, const char* pcPosYFilename,
    const char* pcNegYFilename, const char* pcPosZFilename,
    const char* pcNegZFilename, NiSearchPath& kPath)
{
    const char* apcNames[FACE_NUM];
    NiPixelDataPtr aspFaceSrcPixelDatas[FACE_NUM];

    apcNames[FACE_POS_X] = pcPosXFilename;
    apcNames[FACE_NEG_X] = pcNegXFilename;
    apcNames[FACE_POS_Y] = pcPosYFilename;
    apcNames[FACE_NEG_Y] = pcNegYFilename;
    apcNames[FACE_POS_Z] = pcPosZFilename;
    apcNames[FACE_NEG_Z] = pcNegZFilename;

    unsigned int i;
    NiImageConverter* pkConverter = NiImageConverter::GetImageConverter();
    NIASSERT(pkConverter);
    char acFile[NI_MAX_PATH];
    for (i = 0; i < FACE_NUM; i++)
    {
        NIASSERT(apcNames[i]);
        size_t stLen = strlen(apcNames[i]) + 1;
        char* pcName = NiAlloc(char, stLen);
        NiStrcpy(pcName, stLen, apcNames[i]);

        NiPath::Standardize(pcName);

        char* pcPlaformName = 
            NiImageConverter::ConvertFilenameToPlatformSpecific(pcName);

        kPath.SetFilePath(pcPlaformName);
        kPath.Reset();
        while (kPath.GetNextSearchPath(acFile, NI_MAX_PATH))
        {
            if (NiFile::Access(acFile, NiFile::READ_ONLY))
            {
                aspFaceSrcPixelDatas[i] = 
                    pkConverter->ReadImageFile(acFile, NULL);
                NIASSERT(aspFaceSrcPixelDatas[i] != NULL);
                NiFree(pcPlaformName);
                NiFree(pcName);
                break;
            }
        }
    }

    LoadFromPixelData(aspFaceSrcPixelDatas[FACE_POS_X],  
        aspFaceSrcPixelDatas[FACE_NEG_X], aspFaceSrcPixelDatas[FACE_POS_Y],
        aspFaceSrcPixelDatas[FACE_NEG_Y], aspFaceSrcPixelDatas[FACE_POS_Z],
        aspFaceSrcPixelDatas[FACE_NEG_Z]);
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::LoadFromPixelData(NiPixelData* pkPosXRaw, 
    NiPixelData* pkNegXRaw, NiPixelData* pkPosYRaw, 
    NiPixelData* pkNegYRaw, NiPixelData* pkPosZRaw, 
    NiPixelData* pkNegZRaw)
{
    // All Faces of a cube map must be square and the same size
    NIASSERT(pkPosXRaw->GetWidth() == pkPosXRaw->GetHeight());
    NIASSERT(pkPosXRaw->GetWidth() == pkNegXRaw->GetWidth());
    NIASSERT(pkPosXRaw->GetWidth() == pkNegXRaw->GetHeight());
    NIASSERT(pkPosXRaw->GetWidth() == pkPosYRaw->GetWidth());
    NIASSERT(pkPosXRaw->GetWidth() == pkPosYRaw->GetHeight());
    NIASSERT(pkPosXRaw->GetWidth() == pkNegYRaw->GetWidth());
    NIASSERT(pkPosXRaw->GetWidth() == pkNegYRaw->GetHeight());
    NIASSERT(pkPosXRaw->GetWidth() == pkPosZRaw->GetWidth());
    NIASSERT(pkPosXRaw->GetWidth() == pkPosZRaw->GetHeight());
    NIASSERT(pkPosXRaw->GetWidth() == pkNegZRaw->GetWidth());
    NIASSERT(pkPosXRaw->GetWidth() == pkNegZRaw->GetHeight());

    // All Faces of a cube map must be the same format
    NIASSERT(pkPosXRaw->GetPixelFormat() == pkNegXRaw->GetPixelFormat());
    NIASSERT(pkPosXRaw->GetPixelFormat() == pkPosYRaw->GetPixelFormat());
    NIASSERT(pkPosXRaw->GetPixelFormat() == pkNegYRaw->GetPixelFormat());
    NIASSERT(pkPosXRaw->GetPixelFormat() == pkPosZRaw->GetPixelFormat());
    NIASSERT(pkPosXRaw->GetPixelFormat() == pkNegZRaw->GetPixelFormat());

    // All Mipmap levels of a cube map must be the same
    NIASSERT(pkPosXRaw->GetNumMipmapLevels()==pkNegXRaw->GetNumMipmapLevels());
    NIASSERT(pkPosXRaw->GetNumMipmapLevels()==pkPosYRaw->GetNumMipmapLevels());
    NIASSERT(pkPosXRaw->GetNumMipmapLevels()==pkNegYRaw->GetNumMipmapLevels());
    NIASSERT(pkPosXRaw->GetNumMipmapLevels()==pkPosZRaw->GetNumMipmapLevels());
    NIASSERT(pkPosXRaw->GetNumMipmapLevels()==pkNegZRaw->GetNumMipmapLevels());

    m_spSrcPixelData = NiNew NiPixelData(pkPosXRaw->GetWidth(),
        pkPosXRaw->GetHeight(), pkPosXRaw->GetPixelFormat(), 
        pkPosXRaw->GetNumMipmapLevels(), 6);

    NiPixelDataPtr aspFaceSrcPixelDatas[FACE_NUM];

    aspFaceSrcPixelDatas[FACE_POS_X] = pkPosXRaw;
    aspFaceSrcPixelDatas[FACE_NEG_X] = pkNegXRaw;
    aspFaceSrcPixelDatas[FACE_POS_Y] = pkPosYRaw;
    aspFaceSrcPixelDatas[FACE_NEG_Y] = pkNegYRaw;
    aspFaceSrcPixelDatas[FACE_POS_Z] = pkPosZRaw;
    aspFaceSrcPixelDatas[FACE_NEG_Z] = pkNegZRaw;

    for (unsigned int uiFace = 0; uiFace < FACE_NUM; uiFace++)
    {
        for (unsigned int uiMip = 0; uiMip <  
            aspFaceSrcPixelDatas[uiFace]->GetNumMipmapLevels(); uiMip++)
        {
            unsigned char* pcDestData = 
                m_spSrcPixelData->GetPixels(uiMip, uiFace);
            size_t stDestSize =
                m_spSrcPixelData->GetSizeInBytes(uiMip, uiFace);
            const unsigned char* pcSrcData = 
                aspFaceSrcPixelDatas[uiFace]->GetPixels(uiMip, 0);

            NiMemcpy(pcDestData, stDestSize, pcSrcData, 
                aspFaceSrcPixelDatas[uiFace]->GetSizeInBytes(uiMip, 0));
        }
    }

    // Note that this function intentionally does not copy the palette from
    // the source pixel data objects. This is because palettized cube maps
    // are not supported.
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSourceCubeMap);
//---------------------------------------------------------------------------
void NiSourceCubeMap::LoadBinary(NiStream& kStream)
{
    if (kStream.GetFileVersion() < kStream.GetVersion(10,3,0,6))
    {
        // We completely bypass NiSourceTexture here b/c we don't stream
        // its data members..

        NiTexture::LoadBinary(kStream);

        // IncRefCount() guarantees that "this" won't be deleted
        // by kStream.ChangeObject if this is a shared texture.
        IncRefCount();

        NiBool bExternalTexture = false;
            
        NiStreamLoadBinary(kStream, bExternalTexture);

        // Read in all the filenames and convert them to platform specific if 
        // possible
        unsigned int ui = 0;
        char* apcNames[FACE_NUM];
        memset(apcNames, 0, FACE_NUM*sizeof(char*)); 
        
        for (ui = 0; ui < FACE_NUM; ui++)
        {
            kStream.LoadCString(apcNames[ui]);
        }
        
        // Handle the array of pixel data pointers
        kStream.SetNumberOfLinkIDs(FACE_NUM);

        for (ui = 0; ui < FACE_NUM; ui++)
        {
            kStream.ReadLinkID();   // m_spSrcPixelData
        }

        // Don't look for shared textures until we know how to do it right.

        if (bExternalTexture)
        {  
            // Grab the search path object and prepare to search for the file
            NiSearchPath& kPath = *kStream.GetSearchPath();
            LoadFromFiles(apcNames[FACE_POS_X], apcNames[FACE_NEG_X],
                apcNames[FACE_POS_Y], apcNames[FACE_NEG_Y], 
                apcNames[FACE_POS_Z], apcNames[FACE_NEG_Z], kPath);
        }

        // Clean up the names because they aren't needed any longer
        for (ui = 0; ui < FACE_NUM; ui++)
        {
            NiFree(apcNames[ui]);
        }

        NiStreamLoadEnum(kStream, m_kFormatPrefs.m_ePixelLayout);
        NiStreamLoadEnum(kStream, m_kFormatPrefs.m_eMipMapped);
        NiStreamLoadEnum(kStream, m_kFormatPrefs.m_eAlphaFmt);

        NiBool bStatic = m_bStatic;
        NiStreamLoadBinary(kStream, bStatic);
        m_bStatic = (bStatic != 0);
        DecRefCount();
    }
    else
    {
        NiSourceTexture::LoadBinary(kStream);
    }
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::LinkObject(NiStream& kStream)
{
    if (kStream.GetFileVersion() < kStream.GetVersion(10,3,0,6))
    {
        NiTexture::LinkObject(kStream);

        NiPixelDataPtr aspFaceSrcPixelDatas[FACE_NUM];
        if (kStream.GetNumberOfLinkIDs() == FACE_NUM)
        {
            for (unsigned int ui = 0; ui < FACE_NUM; ui++)
            {
                aspFaceSrcPixelDatas[ui] = 
                    (NiPixelData*)kStream.GetObjectFromLinkID();
            }
        }

        if (m_spSrcPixelData == NULL)
        {
            LoadFromPixelData(aspFaceSrcPixelDatas[FACE_POS_X], 
                aspFaceSrcPixelDatas[FACE_NEG_X], 
                aspFaceSrcPixelDatas[FACE_POS_Y], 
                aspFaceSrcPixelDatas[FACE_NEG_Y], 
                aspFaceSrcPixelDatas[FACE_POS_Z], 
                aspFaceSrcPixelDatas[FACE_NEG_Z]);
        }
    }
    else
    {
        NiSourceTexture::LinkObject(kStream);
    }
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::PostLinkObject(NiStream&)
{
    // Normally, PostLinkObject is called on the parent class at this point.
    // However, this causes problems streaming NiSourceCubeMap, so this call
    // is excluded.

    // This cannot be done in LinkObject, as we must assume that the 
    // NiPixelData object (if any) has been linked as well, in case the 
    // NiPixelData object contains a palette.
    if (NiSourceTexture::ms_bPreload)
    {
        //Creating the renderer data may delete the pixels on a shared
        //texture, so we must protect the cleanup code in streaming.
        NiStream::LockCleanupSection();
        bool bRet = CreateRendererData();
        NiStream::UnlockCleanupSection();
        if (!bRet)
            NiOutputDebugString("Failed to create Cube Map renderer data!\n");
    }
}
//---------------------------------------------------------------------------
bool NiSourceCubeMap::RegisterStreamables(NiStream& kStream)
{
   if (!NiSourceTexture::RegisterStreamables(kStream))
        return false;

   return true;
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::SaveBinary(NiStream& kStream)
{
    NiSourceTexture::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiSourceCubeMap::IsEqual(NiObject* pkObject)
{
    if (!NiSourceTexture::IsEqual(pkObject))
        return false;

    NiSourceCubeMap* pkTexture = (NiSourceCubeMap*)pkObject;


    if ((m_kFormatPrefs.m_ePixelLayout != 
        pkTexture->m_kFormatPrefs.m_ePixelLayout) ||
        (m_kFormatPrefs.m_eMipMapped != 
        pkTexture->m_kFormatPrefs.m_eMipMapped) ||
        (m_kFormatPrefs.m_eAlphaFmt != 
        pkTexture->m_kFormatPrefs.m_eAlphaFmt) ||
        (m_bStatic != pkTexture->m_bStatic))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSourceCubeMap::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSourceTexture::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(NiSourceCubeMap::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
