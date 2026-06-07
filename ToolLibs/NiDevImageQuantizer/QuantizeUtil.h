//---------------------------------------------------------------------------
// Herein code has been modified by Emergent. The original src for
// code & algorithms comes from the pngquant program. This modification
// seperates the quantization code into a library and removes it's
// integration with the png format. It also simplifies the code a bit,
// adopts hungarian notation, and has moved to c++.
//---------------------------------------------------------------------------
// Copyright (C) 1989, 1991 by Jef Poskanzer.
// Copyright (C) 1997 by Greg Roelofs.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted, provided
// that the above copyright notice appear in all copies and that both that
// copyright notice and this permission notice appear in supporting
// documentation.  This software is provided "as is" without express or
// implied warranty.
//---------------------------------------------------------------------------

#ifndef _QUANTIZER_H_
#define _QUANTIZER_H_

#include "NiDevImageQuantizerLibType.h"

#define QPAM_GETR(p) ((p).ucRed)
#define QPAM_GETG(p) ((p).ucGreen)
#define QPAM_GETB(p) ((p).ucBlue)
#define QPAM_GETA(p) ((p).ucAlpha)

#define QPAM_ASSIGN(p,red,grn,blu,alf) \
    (p).Set( red, grn, blu, alf )

#define QPAM_EQUAL(p,q) \
   ((p).ucRed == (q).ucRed && \
    (p).ucGreen == (q).ucGreen && \
    (p).ucBlue == (q).ucBlue && \
    (p).ucAlpha == (q).ucAlpha)

#define QPAM_DEPTH(newp,p,oldmaxval,newmaxval) \
   (newp).Set( \
      ( (unsigned char) QPAM_GETR(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
      ( (unsigned char) QPAM_GETG(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
      ( (unsigned char) QPAM_GETB(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval), \
      ( (unsigned char) QPAM_GETA(p) * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval) )


struct NIDEVIMAGEQUANTIZER_ENTRY RGBAPixel
{
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue; 
    unsigned char ucAlpha;

    inline void Set( unsigned char ucR, unsigned char ucG,
        unsigned char ucB, unsigned char ucA );
};


struct NIDEVIMAGEQUANTIZER_ENTRY ImgInfo
{
    ~ImgInfo();
    unsigned long ulWidth;          // read/write
    unsigned long ulHeight;         // read/write
    unsigned long ulRowBytes;       // read

    RGBAPixel Palette[256];

    const unsigned char *pucRGBAData;     // read
    unsigned char *pucIndexedData;  // write
    const unsigned char **ppucRowPointers;// read/write
    
    int iInterlaced;    // read/write
    int iChannels;      // read (currently not used)
    int iSampleDepth;   // write
    int iNumPalette;    // write
    int iNumTrans;      // write
    unsigned char* pucSaveIndexData;
};


class NiAlphaMapUtility;
class NIDEVIMAGEQUANTIZER_ENTRY QuantizeUtil
{
public:
    int m_iErrCode;

    QuantizeUtil();
    ~QuantizeUtil();

    int Quantize( ImgInfo* pImgInfo );
    inline int GetError();
    void SetNumberOfColorBits( unsigned int uiColorBits );
    unsigned int GetNumberOfPaletteColors();

private:
    NiAlphaMapUtility *m_pkAMP;
     unsigned int m_uiReqColors;


    void MedianCut (int iColors, int iSum, unsigned char ucMaxVal,
        int iNewColors);

};

#include "QuantizeUtil.inl"


#endif
