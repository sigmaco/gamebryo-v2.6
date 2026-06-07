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
#ifndef NIPNGREADER_H
#define NIPNGREADER_H

#include "NiPNGReaderLibType.h"
#include <NiMemManager.h>
#include <NiImageReader.h>
#include <NiPalette.h>
#include <NiPixelFormat.h>

class NIPNGREADER_ENTRY NiPNGReader : public NiImageReader
{
public:
    NiPNGReader();
    ~NiPNGReader();

    virtual bool CanReadFile(const char* pcFileExtension) const;

    virtual NiPixelData* ReadFile(NiFile& kIst, NiPixelData* pkOptDest);

    virtual bool ReadHeader(NiFile& kIst, 
        unsigned int& uiWidth, unsigned int& uiHeight, 
        NiPixelFormat& kFormat, bool& bMipmap,
        unsigned int& uiNumFaces);

    NiPixelData* ReadBody(NiFile &kIst, NiPixelData* pkOptDest);

protected:
    typedef void 
        (NiPNGReader::*Unpacker)(unsigned char*, unsigned char*);

    enum 
    {
        PNG_Mono = 0,
        PNG_RGB = 2,
        PNG_Map = 3,
        PNG_MonoA = 4,
        PNG_RGBA = 6
    };

    enum 
    {
        MAXCOLORS = 16384
    };

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
    void Unpack32BitFrom24BitSourceRow(unsigned char* pucSrc, 
        unsigned char* pucDest);
    void Unpack32BitFrom32BitSourceRow(unsigned char* pucSrc, 
        unsigned char* pucDest);

    void GetColormap(NiFile& kIst);


    unsigned char m_aucHeader[8]; // Header Array
    unsigned int m_uiLength;
    unsigned int m_uiType;
    char m_acType[8];
    unsigned int m_uiCRC;

    unsigned char m_ucIDLength; // length of Identifier String
    //unsigned char m_ucCoMapType; // 0 = no map
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

    NiPixelFormat m_kFormat;

    unsigned int m_uiRawDataMax;
    unsigned char* m_pucRawData;
    unsigned int m_uiColorMapMax;
    NiPalette::PaletteEntry* m_pkColorMap;

    Unpacker m_pfnUnpacker;
};

#endif
