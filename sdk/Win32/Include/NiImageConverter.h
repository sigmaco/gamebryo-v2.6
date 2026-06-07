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
#ifndef NIIMAGECONVERTER_H
#define NIIMAGECONVERTER_H

#include "NiNIFImageReader.h"
#include "NiObjectNET.h"
#include "NiPixelFormat.h"

class NiPixelData;
class NiImageConverter;

typedef NiPointer<NiImageConverter> NiImageConverterPtr;

class NIMAIN_ENTRY NiImageConverter : public NiRefObject
{
public:
    NiDeclareRootRTTI(NiImageConverter);

    NiImageConverter();
    virtual ~NiImageConverter();

    virtual bool CanReadImageFile(const char* pcFilename) const;

    virtual bool ReadImageFileInfo(const char* pcFilename,
        NiPixelFormat& kFmt, bool& bMipmap, unsigned int& uiWidth,
        unsigned int& uiHeight, unsigned int& uiNumFaces);

    virtual NiPixelData* ReadImageFile(const char* pcFilename,
        NiPixelData* pkOptDest);

    virtual bool CanConvertPixelData(const NiPixelFormat& kSrcFmt, 
        const NiPixelFormat& kDestFmt) const;

    virtual NiPixelData* ConvertPixelData(const NiPixelData& kSrc, 
        const NiPixelFormat& kDestFmt, NiPixelData* pkOptDest, bool bMipmap);

    static NiImageConverter* GetImageConverter();
    static void SetImageConverter(NiImageConverter* pkConverter);

    // Set the platform specific image directory string.  Image loaders 
    // will first look in "base-directory/platform-specific-directory" for 
    // the image filename
    static void SetPlatformSpecificSubdirectory(const char* pcSubDir);
    static const char* GetPlatformSpecificSubdirectory();

    static char* ConvertFilenameToPlatformSpecific(const char* pcPath);
    static void ConvertFilenameToPlatformSpecific(const NiFixedString& kPath,
        NiFixedString& kPlatformSpecificPath);

    static void _SDMInit();
    static void _SDMShutdown();
protected:
    NiNIFImageReader m_kReader;

    // Active image converter
    static NiImageConverterPtr ms_spConverter;

    static char* ms_pcPlatformSubDir;
};

#endif
