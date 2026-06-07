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

#ifndef NIBMPREADER_H
#define NIBMPREADER_H

#include "NiPixelFormat.h"
#include "NiImageReader.h"

class NIMAIN_ENTRY NiBMPReader : public NiImageReader
{
public:
    NiBMPReader();
    ~NiBMPReader();

    virtual bool CanReadFile(const char* pcFileExtension) const;

    virtual NiPixelData* ReadFile(NiFile& kIst, NiPixelData* pkOptDest);

    virtual bool ReadHeader(NiFile& kIst, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap, unsigned int& uiFaces);

protected:
    class NIMAIN_ENTRY WINDOWS_RGBQUAD: public NiMemObject
    { // rgbquad 
        // DO NOT reorder this structure's elements
    public:
        unsigned char m_ucBlue; 
        unsigned char m_ucGreen; 
        unsigned char m_ucRed; 
        unsigned char m_ucReserved; 
    }; 

    enum 
    {
        WINDOWS_BMPCOREHEADER_SIZE = 12
    };

    enum
    {
        WINDOWS_BMPTOTALHEADER_SIZE = 54
    };

    enum
    {
        WINDOWS_BI_RGB = 0,
        WINDOWS_BI_RLE8 = 1,
        WINDOWS_BI_RLE4 = 2,
        WINDOWS_BI_BITFIELDS = 3
    };

    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    NiPixelFormat m_kFormat;
    unsigned short m_usBPP;
    unsigned int m_uiClrUsed;
    unsigned int m_uiOffBits;
    bool m_bFlipVert;
    bool m_bRLE;
};

#endif
