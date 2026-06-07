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
#ifndef NIOPENEXRREADER_H
#define NIOPENEXRREADER_H

#include "NiOpenEXRReaderLibType.h"
#include "NiImageReader.h"
#include "NiPalette.h"
#include "NiPixelFormat.h"

class NIOPENEXRREADER_ENTRY NiOpenEXRReader : public NiImageReader
{
public:
    NiOpenEXRReader();
    ~NiOpenEXRReader();

    virtual bool CanReadFile(const char* pcFileExtension) const;

    virtual NiPixelData* ReadFile(NiFile& kIst, NiPixelData* pkOptDest);

    virtual bool ReadHeader(NiFile& kIst, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap,
        unsigned int& uiNumFaces);

    NiPixelData* ReadBody(NiFile &kIst, NiPixelData* pkOptDest);
    NiPixelData* ReadImage(NiFile &kIst, NiPixelData* pkOptDest);
    NiPixelData* ReadMipMapImage(NiFile &kIst, NiPixelData* pkOptDest);

protected:
    typedef void 
        (NiOpenEXRReader::*Unpacker)(unsigned char*, unsigned char*);

    NiPixelFormat m_kFormat;
    Unpacker m_pfnUnpacker;
    bool m_bMipMap;
    
    unsigned int m_uiHeight;
    unsigned int m_uiWidth;

    unsigned int m_uiNumFaces;
};

#endif
