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

#ifndef NIIMAGEREADER_H
#define NIIMAGEREADER_H

#include "NiCriticalSection.h"
#include "NiSmartPointer.h"
#include "NiMainLibType.h"
#include <NiMemObject.h>

class NiFile;
class NiPixelData;
class NiPixelFormat;

class NIMAIN_ENTRY NiImageReader : public NiMemObject
{
public:
    inline virtual ~NiImageReader() { /* */ }

    virtual bool CanReadFile(const char* pcFileExtension) const = 0;

    virtual NiPixelData* ReadFile(NiFile& kIst, NiPixelData* pkOptDest) = 0;

    virtual bool ReadHeader(NiFile& kIst, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap, unsigned int& uiFaces) = 0;
protected:
    NiCriticalSection m_kReadCriticalSection;
};

#endif
