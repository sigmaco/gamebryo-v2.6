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

#ifndef NINIFIMAGEREADER_H
#define NINIFIMAGEREADER_H

#include "NiImageReader.h"
#include "NiStream.h"

class NIMAIN_ENTRY NiNIFImageReader : public NiImageReader
{
public:
    NiNIFImageReader();
    ~NiNIFImageReader();

    virtual bool CanReadFile(const char* pcFileExtension) const;

    virtual NiPixelData* ReadFile(NiFile& kIst, NiPixelData* pkOptDest);

    virtual bool ReadHeader(NiFile& kIst, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap, 
        unsigned int& uiFaces);

protected:
    NiStream m_kStream;
};

#endif
