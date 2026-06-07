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
#ifndef NISGIREADER_H
#define NISGIREADER_H

#include "NiImageReader.h"
#include "NiPixelFormat.h"

class NIMAIN_ENTRY NiSGIReader : public NiImageReader
{
public:
    NiSGIReader();
    ~NiSGIReader();

    virtual bool CanReadFile(const char* pFileExtension) const;

    virtual NiPixelData* ReadFile(NiFile& ist, NiPixelData* pOptDest);

    virtual bool ReadHeader(NiFile& ist, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap, unsigned int& uiFaces);

protected:
    enum 
    {
        SGI_VERBATIM = 0,
        SGI_RLE = 1
    };
 
    enum
    {
        SGI_MAGIC = 474
    };

    void ProcessRLERowByte (unsigned char* pOutputImageData, 
        unsigned char* pInputImageData, int iImageStep);
    void ProcessRLERowWord (unsigned char* pOutputImageData, 
        unsigned char* pInputImageData, int iImageStep);

    unsigned short m_usWidth;
    unsigned short m_usHeight;
    unsigned short m_usDepth;
    unsigned char m_ucBytesPerComponent;
    bool m_bRLE;
    NiPixelFormat m_kFormat;
};

#endif
