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

#ifndef _ALPHAMAPUTILITY_H_
#define _ALPHAMAPUTILITY_H_

#include "QuantizeUtil.h"
#include "NiDevImageQuantizerLibType.h"

//---------------------------------------------------------------------------
struct NIDEVIMAGEQUANTIZER_ENTRY ColorBox 
{
    int iIndex;
    int iColors;
    int iSum;
};
//---------------------------------------------------------------------------
struct NIDEVIMAGEQUANTIZER_ENTRY ColorHistographItem 
{
    RGBAPixel RGBAColor;
    int iValue;
};
//---------------------------------------------------------------------------
struct NIDEVIMAGEQUANTIZER_ENTRY ColorHistographList 
{
    ColorHistographList();
    ~ColorHistographList();
    ColorHistographItem HistoItem;
    ColorHistographList* pNext;

    static unsigned int ms_uiNumItems;
};
//---------------------------------------------------------------------------
class NIDEVIMAGEQUANTIZER_ENTRY NiAlphaMapUtility
{
public:
     NiAlphaMapUtility();
    ~NiAlphaMapUtility();

    int LookUpColor( RGBAPixel* pRGBAColor );
    int AddColorToHash( RGBAPixel* pRGBAColor, int iVal );
    int ComputeColorHistograph( RGBAPixel** ppPixels, int cols, int rows,
        int iMaxColors, int* acolorsP );
    
    inline ColorHistographItem* GetColorMap();
    void CreateColorMap( int iSize );

    inline void SetColorMap( ColorHistographItem* pColorMap );
    inline int GetColorMapA( int iIndex );
    
    int GetColorMapRGBA( int iIndex, int *iR, int *iG, int *iB, int* iA );
    int SetColorMapRGBA( int iIndex, 
        unsigned char ucR,
        unsigned char ucG,
        unsigned char ucB,
        unsigned char ucA );

    inline ColorHistographItem* GetHistograph();
    void SortHistograph(int indx, int clrs);
    inline void SetHistograph( ColorHistographItem* pHistograph );
    int GetHistographRGBA( int iIndex, int *iR, int *iG, int *iB, int* iA );
    int SetHistographRGBA( int iIndex, 
        unsigned char ucR,
        unsigned char ucG,
        unsigned char ucB,
        unsigned char ucA );

    // inline
    inline int GetHistographRxVal( int iIndex );
    inline int GetHistographGxVal( int iIndex );
    inline int GetHistographBxVal( int iIndex );
    inline int GetHistographAxVal( int iIndex );
    inline int GetHistographValue( int iIndex );

private:
    ColorHistographList** m_ppHistographHash;
    ColorHistographItem* m_pHistograph;
    ColorHistographItem* m_pColorMap;

    ColorHistographList** ComputeColorHash( RGBAPixel** ppPixels, int iCols,
        int iRows, int iMaxColors, int* piColors );
    ColorHistographItem* ColorHashToColorHist( ColorHistographList** ppAcht,
        int iMaxColors );
    ColorHistographList** AllocColorHash( );
    int AddToColorHash( ColorHistographList** acht, RGBAPixel* ppColorP,
        int iValue );
    void FreeColorHash( ColorHistographList** acht );

    //ColorHistographList** m_acht;

};

#include "AlphaMapUtility.inl"

#endif
