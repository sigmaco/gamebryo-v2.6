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
#ifndef NITGAREADER_H
#define NITGAREADER_H

#include "NiImageReader.h"
#include "NiPalette.h"
#include "NiPixelFormat.h"

class NIMAIN_ENTRY NiTGAReader : public NiImageReader
{
public:
    NiTGAReader();
    ~NiTGAReader();

    virtual bool CanReadFile(const char* pcFileExtension) const;

    virtual NiPixelData* ReadFile(NiFile& kIst, NiPixelData* pkOptDest);

    virtual bool ReadHeader(NiFile& kIst, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap, unsigned int& uiFaces);

protected:
    typedef void 
        (NiTGAReader::*Unpacker)(unsigned char*, unsigned char*);

    enum 
    {
        TGA_Map = 1,
        TGA_RGB = 2,
        TGA_Mono = 3,
        TGA_RLEMap = 9,
        TGA_RLERGB = 10,
        TGA_RLEMono = 11
    };

    enum 
    {
        MAXCOLORS = 16384
    };

    void LoadRLERow(NiFile& kIst, unsigned char *pucDest,
        unsigned int uiDestSize);
    void Unpack4BitSourceRowPal(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack8BitSourceRowPal(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack8BitSourceRowGray(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack8BitSourceRowIndexed(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack8BitSourceRowIndexedAlpha(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack16BitSourceRowRGB(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack16BitSourceRowIndexed(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack16BitSourceRowIndexedAlpha(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack24BitSourceRow(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack32BitSourceRow(unsigned char* pucSrc, 
        unsigned char* pucDest);

    void GetColormap(NiFile& kIst);


    unsigned char m_ucIDLength; // length of Identifier String
    unsigned char m_ucCoMapType; // 0 = no map
    unsigned char m_ucImgType; // image type
    unsigned short m_usMinPalIndex; // index of first color map entry
    unsigned short m_usPalLength; // number of entries in color map
    unsigned char m_ucCoSize; // size of color map entry (15,16,24,32)
    unsigned short m_usXOrigin; // x origin of image
    unsigned short m_usYOrigin; // y origin of image
    unsigned short m_usWidth; // width of image
    unsigned short m_usHeight; // height of image
    unsigned char m_ucPixelSize; // pixel size (8,15,16,24,32)
    unsigned char m_ucAttBits; // 4 bits, number of attribute bits per pixel

    unsigned char m_uiBytesPerPixel; // pixel size (8,15,16,24,32)

    bool m_bAlpha;
    bool m_bColormapped;
    bool m_bFlipVert;
    bool m_bRLE;
    NiPixelFormat m_kFormat;

    unsigned int m_uiRawDataMax;
    unsigned char* m_pucRawData;
    unsigned int m_uiColorMapMax;
    NiPalette::PaletteEntry* m_pkColorMap;

    Unpacker m_pfnUnpacker;

    unsigned int m_uiRLECount;
    bool m_bRLEncodedRun;
    unsigned char m_aucRLEBits[4];
};

#endif
