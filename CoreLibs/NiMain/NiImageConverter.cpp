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

#include <NiFile.h>
#include <NiFilename.h>
#include <NiSystem.h>
#include "NiImageConverter.h"
#include "NiPixelData.h"

NiImplementRootRTTI(NiImageConverter);

NiImageConverterPtr NiImageConverter::ms_spConverter = 0;
char* NiImageConverter::ms_pcPlatformSubDir = 0;

//---------------------------------------------------------------------------
void NiImageConverter::_SDMInit()
{
    ms_spConverter = NiNew NiImageConverter;
}
//---------------------------------------------------------------------------
void NiImageConverter::_SDMShutdown()
{
    ms_spConverter = 0;
}
//---------------------------------------------------------------------------
NiImageConverter* NiImageConverter::GetImageConverter()
{
    return ms_spConverter;
}
//---------------------------------------------------------------------------
void NiImageConverter::SetImageConverter(NiImageConverter* pkConverter)
{
    ms_spConverter = pkConverter;
}
//---------------------------------------------------------------------------
NiImageConverter::NiImageConverter()
{
}
//---------------------------------------------------------------------------
NiImageConverter::~NiImageConverter()
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }
    ms_pcPlatformSubDir = NULL;
}
//---------------------------------------------------------------------------
bool NiImageConverter::CanReadImageFile(const char* pcFilename) const
{
    NiFilename kFileName(pcFilename);
    const char* pcExt = kFileName.GetExt();

    if (m_kReader.CanReadFile(pcExt))
    {
        NiFile* kIst = NiFile::GetFile(pcFilename, NiFile::READ_ONLY);
        if ((!kIst) || (!(*kIst)))
        {
            NiDelete kIst;
            return false;
        }
        
        NiDelete kIst;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiImageConverter::ReadImageFileInfo(const char* pcFilename,
    NiPixelFormat& kFmt, bool& bMipmap, unsigned int& uiWidth, 
    unsigned int& uiHeight, unsigned int& uiNumFaces)
{
    NiFilename kFileName(pcFilename);
    const char* pcExt = kFileName.GetExt();

    if (m_kReader.CanReadFile(pcExt))
    {
        NiFile* kIst = NiFile::GetFile(pcFilename, NiFile::READ_ONLY);
        if ((!kIst) || (!(*kIst)))
        {
            NiDelete kIst;
            return false;
        }
        
        if (m_kReader.ReadHeader(*kIst, uiWidth, uiHeight, kFmt, bMipmap, 
            uiNumFaces))
        {
            NiDelete kIst;
            return true;
        }

        NiDelete kIst;
    }

    return false;
}
//---------------------------------------------------------------------------
NiPixelData* NiImageConverter::ReadImageFile(const char* pcFilename,
    NiPixelData* pkOptDest)
{
    NiFilename kFileName(pcFilename);
    const char* pcExt = kFileName.GetExt();

    if (m_kReader.CanReadFile(pcExt))
    {
        NiFile* kIst = NiFile::GetFile(pcFilename, NiFile::READ_ONLY);
        if ((!kIst) || (!(*kIst)))
        {
            NiDelete kIst;
            return false;
        }
        NiPixelData* pkDest = m_kReader.ReadFile(*kIst, pkOptDest);

        if (pkDest)
        {
            NiDelete kIst;
            return pkDest;
        }

        NiDelete kIst;
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiImageConverter::CanConvertPixelData(const NiPixelFormat& kSrcFmt, 
    const NiPixelFormat& kDestFmt) const
{
    return kSrcFmt == kDestFmt;
}
//---------------------------------------------------------------------------
NiPixelData* NiImageConverter::ConvertPixelData(const NiPixelData& kSrc, 
    const NiPixelFormat& kDestFmt, NiPixelData* pkOptDest, bool)
{
    if (kSrc.GetPixelFormat() != kDestFmt)
        return NULL;

    if (pkOptDest)
    {
        if (pkOptDest == &kSrc)
        {
            // Source equals optional destination - return without any copies
            return pkOptDest;
        }
        else if ((pkOptDest->GetPixelFormat() == kDestFmt) &&
            (pkOptDest->GetWidth() == kSrc.GetWidth()) &&
            (pkOptDest->GetHeight() == kSrc.GetHeight()) &&
            (pkOptDest->GetNumMipmapLevels() <= kSrc.GetNumMipmapLevels()))
        {
            // Destination size, format and miplevels match - copy into 
            // supplied destination data objects
            unsigned uiMaxLevel = pkOptDest->GetNumMipmapLevels();
            for (unsigned int uiMip = 0; uiMip < uiMaxLevel; uiMip++)
            {
                NiMemcpy(pkOptDest->GetPixels(uiMip), 
                    pkOptDest->GetSizeInBytes(uiMip),
                    kSrc.GetPixels(uiMip), kSrc.GetSizeInBytes(uiMip));
            }

            if (kSrc.GetPalette())
            {
                // Copy the palette.
                if (!pkOptDest->GetPalette())
                    pkOptDest->SetPalette(NiNew NiPalette(*kSrc.GetPalette()));
                else
                    *pkOptDest->GetPalette() = *kSrc.GetPalette();
            }

            return pkOptDest;
        }
    }
    
    // allocate the required destination data and copy into it
    NiPixelData* pkDest = NiNew NiPixelData(kSrc);

    return pkDest;
}
//---------------------------------------------------------------------------
const char* NiImageConverter::GetPlatformSpecificSubdirectory() 
{
    return ms_pcPlatformSubDir;
}
//---------------------------------------------------------------------------
void NiImageConverter::SetPlatformSpecificSubdirectory(const char* pcSubDir)
{
    if (ms_pcPlatformSubDir)
    {
        NiFree(ms_pcPlatformSubDir);
    }

    if (pcSubDir)
    {
        size_t stLen = strlen(pcSubDir) + 1;
        ms_pcPlatformSubDir = NiAlloc(char, stLen);
        NiStrcpy(ms_pcPlatformSubDir, stLen, pcSubDir);
        NiPath::Standardize(ms_pcPlatformSubDir);
    }
    else
    {
        ms_pcPlatformSubDir = NULL;
    }
}
//---------------------------------------------------------------------------
char* NiImageConverter::ConvertFilenameToPlatformSpecific(const char* pcPath)
{
    char* pcPlatformPath = NiAlloc(char, 260);
    if (ms_pcPlatformSubDir)
    {
        NiFilename kPlatform(pcPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(pcPlatformPath, 260);
    }
    else
    {
        NiStrncpy(pcPlatformPath, 260, pcPath, strlen(pcPath));
    }
    return pcPlatformPath;
}
//---------------------------------------------------------------------------
void NiImageConverter::ConvertFilenameToPlatformSpecific(
    const NiFixedString& kPath, NiFixedString& kPlatformSpecificPath)
{
    if (ms_pcPlatformSubDir)
    {
        char acPlatformPath[260];
        NiFilename kPlatform((const char*)kPath);
        kPlatform.SetPlatformSubDir(ms_pcPlatformSubDir);
        kPlatform.GetFullPath(acPlatformPath, 260);
        kPlatformSpecificPath = acPlatformPath;
    }
    else
    {
        kPlatformSpecificPath = kPath;
    }
}
//---------------------------------------------------------------------------
