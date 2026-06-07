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
#include <Windows.h>
#include <stdlib.h>
#include <NiSystem.h>
#include <NiMemoryDefines.h>

#include "AlphaMapUtility.h"

static int redcompare (const void *ch1, const void *ch2);
static int greencompare (const void *ch1, const void *ch2);
static int bluecompare (const void *ch1, const void *ch2);
static int alphacompare (const void *ch1, const void *ch2);

#define HASH_SIZE 20023

#define HASH_A_PIXEL(p) ( ( ( (long) QPAM_GETR(p) * 33023 + \
                                (long) QPAM_GETG(p) * 30013 + \
                                (long) QPAM_GETB(p) * 27011 + \
                                (long) QPAM_GETA(p) * 24007 ) \
                              & 0x7fffffff ) % HASH_SIZE )


//---------------------------------------------------------------------------
NiAlphaMapUtility::NiAlphaMapUtility()
{
    m_ppHistographHash =  AllocColorHash( );
    m_pColorMap = NULL;
}
//---------------------------------------------------------------------------
NiAlphaMapUtility::~NiAlphaMapUtility()
{
    FreeColorHash(m_ppHistographHash);

    delete [] m_pColorMap;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::LookUpColor( RGBAPixel* pRGBAColor )
{
    int hash;
    ColorHistographList* pAchl; 

    hash = HASH_A_PIXEL( *pRGBAColor );
    for ( pAchl = m_ppHistographHash[hash]; 
        pAchl != (ColorHistographList*) 0; pAchl = pAchl->pNext ) 
    {
        if ( QPAM_EQUAL( pAchl->HistoItem.RGBAColor, *pRGBAColor ) )
            return pAchl->HistoItem.iValue;
    }

    return -1;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::AddColorToHash( RGBAPixel* pRGBAColor, int iVal )
{
    int hash;
    ColorHistographList* pAchl; 

    pAchl = NiExternalNew ColorHistographList;


    if ( pAchl == 0 )
        return -1;
    
    hash = HASH_A_PIXEL( *pRGBAColor );
    
    pAchl->HistoItem.RGBAColor = *pRGBAColor;
    pAchl->HistoItem.iValue = iVal;
    pAchl->pNext = m_ppHistographHash[hash];
    m_ppHistographHash[hash] = pAchl;
    
    return 0;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::ComputeColorHistograph( RGBAPixel** ppPixels, int cols, 
    int rows, int iMaxColors, int* acolorsP )
{
    ColorHistographList** acht;

    acht = ComputeColorHash( ppPixels, cols, rows, iMaxColors, acolorsP );
    if ( acht == (ColorHistographList**) 0 )
        m_pHistograph = NULL;
    else
    {
        m_pHistograph = ColorHashToColorHist( acht, iMaxColors );
        FreeColorHash( acht );
    }

     if (m_pHistograph)
        return 1;
    else 
        return 0;
}
//---------------------------------------------------------------------------
void NiAlphaMapUtility::CreateColorMap( int iSize )
{
    NiExternalDelete m_pColorMap;
    m_pColorMap = NiExternalNew ColorHistographItem[iSize];
    if (m_pColorMap==NULL)
        return;
    
    for (int i=0; i<iSize; i++)
    {
        m_pColorMap->RGBAColor.ucRed = 0;
        m_pColorMap->RGBAColor.ucGreen = 0;
        m_pColorMap->RGBAColor.ucBlue = 0;
        m_pColorMap->RGBAColor.ucAlpha = 0;
    }
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::SetColorMapRGBA( 
    int iIndex, unsigned char ucR, unsigned char ucG, 
    unsigned char ucB, unsigned char ucA )
{
    if (m_pColorMap == NULL)
        return 0;

    m_pColorMap[iIndex].RGBAColor.ucRed = ucR;
    m_pColorMap[iIndex].RGBAColor.ucGreen = ucG;
    m_pColorMap[iIndex].RGBAColor.ucBlue = ucB;
    m_pColorMap[iIndex].RGBAColor.ucAlpha = ucA;

    return 1;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::GetColorMapRGBA( int iIndex, int *iR, int *iG,
    int *iB, int* iA )
{
    if (m_pColorMap == NULL)
        return 0;

    *iR = m_pColorMap[iIndex].RGBAColor.ucRed;
    *iG = m_pColorMap[iIndex].RGBAColor.ucGreen;
    *iB = m_pColorMap[iIndex].RGBAColor.ucBlue;
    *iA = m_pColorMap[iIndex].RGBAColor.ucAlpha;

    return 1;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::GetHistographRGBA( int iIndex, int *iR, int *iG, 
    int *iB, int* iA )
{
    if (m_pHistograph == NULL)
        return 0;


    *iR = m_pHistograph[iIndex].RGBAColor.ucRed;
    *iG = m_pHistograph[iIndex].RGBAColor.ucGreen;
    *iB = m_pHistograph[iIndex].RGBAColor.ucBlue;
    *iA = m_pHistograph[iIndex].RGBAColor.ucAlpha;

    return 1;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::SetHistographRGBA( 
    int iIndex, unsigned char ucR, unsigned char ucG, 
    unsigned char ucB, unsigned char ucA )
{
    if (m_pHistograph == NULL)
        return 0;

    m_pHistograph[iIndex].RGBAColor.ucRed = ucR;
    m_pHistograph[iIndex].RGBAColor.ucGreen = ucG;
    m_pHistograph[iIndex].RGBAColor.ucBlue = ucB;
    m_pHistograph[iIndex].RGBAColor.ucAlpha = ucA;

    return 1;
}
//---------------------------------------------------------------------------
void NiAlphaMapUtility::SortHistograph(int indx, int clrs)
{
    int minr, maxr, ming, mina, maxg, minb, maxb, maxa;
    int r,g,b,a;
    int i;

    // Go through the box finding the minimum and maximum of each
    // component - the boundaries of the box.
    
    GetHistographRGBA( indx, &r, &g, &b, &a );
    minr = maxr = r;
    ming = maxg = g;
    minb = maxb = b;
    mina = maxa = a;
         
    for ( i = 1; i < clrs; ++i )
    {
        GetHistographRGBA( indx + i, &r, &g, &b, &a );
        if ( r < minr ) minr = r;
        if ( r > maxr ) maxr = r;
        if ( g < ming ) ming = g;
        if ( g > maxg ) maxg = g;
        if ( b < minb ) minb = b;
        if ( b > maxb ) maxb = b;
        if ( a < mina ) mina = a;
        if ( a > maxa ) maxa = a;
    }

    // Find the largest dimension, and sort by that component.  I have
    // included two methods for determining the "largest" dimension;
    // first by simply comparing the range in RGB space, and second
    // by transforming into luminosities before the comparison.  You
    // can switch which method is used by switching the commenting on
    // the LARGE_ defines at the beginning of this source file.

    if ( maxa - mina >= maxr - minr && maxa - mina >= 
        maxg - ming && maxa - mina >= maxb - minb )
    {
        qsort((char*) &(m_pHistograph[indx]), clrs, 
            sizeof(struct ColorHistographItem), alphacompare );
    }
    else if ( maxr - minr >= maxg - ming && maxr - minr >= maxb - minb )
    {
        qsort((char*) &(m_pHistograph[indx]), clrs,
            sizeof(struct ColorHistographItem), redcompare );
    }
    else if ( maxg - ming >= maxb - minb )
    {
        qsort((char*) &(m_pHistograph[indx]), clrs,
            sizeof(struct ColorHistographItem), greencompare );
    }
    else
    {
        qsort((char*) &(m_pHistograph[indx]), clrs,
            sizeof(struct ColorHistographItem),bluecompare );
    }
}
//---------------------------------------------------------------------------
ColorHistographList** NiAlphaMapUtility::ComputeColorHash(
    RGBAPixel** ppPixels, int iCols, int iRows, int iMaxColors,
    int* piColors )
{
    ColorHistographList** acht;
    ColorHistographList *pAchl; 
    RGBAPixel* pP;
    int iCol, iRow, iHash;

    acht = AllocColorHash( );
    *piColors = 0;

    // Go through the entire image, building a hash table of colors. 
    for ( iRow = 0; iRow < iRows; ++iRow )
    {
        for ( iCol = 0, pP = ppPixels[iRow]; iCol < iCols; ++iCol, ++pP )
        {
            iHash = HASH_A_PIXEL( *pP );
            for ( pAchl = acht[iHash]; 
                pAchl != (ColorHistographList*) 0; pAchl = pAchl->pNext ) 
            {
                if ( QPAM_EQUAL( pAchl->HistoItem.RGBAColor, *pP ) )
                    break;
            }
            
            if ( pAchl != (ColorHistographList*) 0 ) 
            {
                ++(pAchl->HistoItem.iValue);
            }
            else
            {
                if ( ++(*piColors) > iMaxColors )
                {
                    FreeColorHash( acht );
                    return (ColorHistographList**) 0;
                }
            
                pAchl = NiExternalNew ColorHistographList;

                if ( pAchl == 0 ) 
                {
                    NiOutputDebugString("Out of Memory\n");
                    return NULL;
                    // fprintf( stderr,
                    // "  out of memory computing hash table\n" );
                    //exit(7);
                }
            
                pAchl->HistoItem.RGBAColor = *pP;
                pAchl->HistoItem.iValue = 1;
                pAchl->pNext = acht[iHash];
                acht[iHash] = pAchl;
            }
        }
    }
    
    return acht;
}
//---------------------------------------------------------------------------
ColorHistographList** NiAlphaMapUtility::AllocColorHash( )
{
    ColorHistographList** acht;

    int i;

    acht = NiExternalNew ColorHistographList*[ HASH_SIZE ];


    if ( acht == 0 ) 
    {
        NiOutputDebugString(" Out of Memory\n");
        return NULL;
        // fprintf( stderr, "  out of memory allocating hash table\n" );
        //exit(8);
    }

    for ( i = 0; i < HASH_SIZE; ++i )
        acht[i] = (ColorHistographList*) 0;

    return acht;
}
//---------------------------------------------------------------------------
int NiAlphaMapUtility::AddToColorHash( ColorHistographList** acht, 
    RGBAPixel* ppColorP, int iValue )
{
    int hash;
    ColorHistographList* pAchl; 

    pAchl = NiExternalNew ColorHistographList;


    if ( pAchl == 0 )
        return -1;
    
    hash = HASH_A_PIXEL( *ppColorP );
    
    pAchl->HistoItem.RGBAColor = *ppColorP;
    pAchl->HistoItem.iValue = iValue;
    pAchl->pNext = acht[hash];
    acht[hash] = pAchl;
    
    return 0;
}
//---------------------------------------------------------------------------
ColorHistographItem* NiAlphaMapUtility::ColorHashToColorHist( 
    ColorHistographList** ppAcht, int iMaxColors )
{
    ColorHistographItem* pAchv;
    ColorHistographList* pAchl; 
    int i, j;

    // Now collate the hash table into a simple acolorhist array. 
    pAchv = NiExternalNew ColorHistographItem[ iMaxColors ];

    // (Leave room for expansion by caller.) 
    if ( pAchv == (ColorHistographItem*) 0 ) 
    {
        NiOutputDebugString("Out of Mem\n");
        return NULL;
        // fprintf( stderr, "  out of memory generating histograph\n" );
        //exit(9);
    }

    // Loop through the hash table. 
    j = 0;
    for ( i = 0; i < HASH_SIZE; ++i )
    {
        for ( pAchl = ppAcht[i]; 
            pAchl != (ColorHistographList*) 0; pAchl = pAchl->pNext ) 
        {
            // Add the new entry. 
            pAchv[j] = pAchl->HistoItem;
            ++j;
        }
    }

    // All done.
    return pAchv;
}
//---------------------------------------------------------------------------
void NiAlphaMapUtility::FreeColorHash( ColorHistographList** acht )
{
    int i;
    ColorHistographList* pAchl; 
    ColorHistographList* pAchlnext; 

    for ( i = 0; i < HASH_SIZE; ++i )
    {
        for ( pAchl = acht[i]; 
            pAchl != (ColorHistographList*) 0; pAchl = pAchlnext ) 
        {
            pAchlnext = pAchl->pNext;
            delete pAchl;
        }
        acht[i] = NULL;
    }
    delete [] acht;
}
//---------------------------------------------------------------------------
static int redcompare( const void *ch1, const void *ch2 )
{
    return (int) QPAM_GETR( ((ColorHistographItem*)ch1)->RGBAColor ) -
           (int) QPAM_GETR( ((ColorHistographItem*)ch2)->RGBAColor );
}
//---------------------------------------------------------------------------
static int greencompare( const void *ch1, const void *ch2 )
{
    return (int) QPAM_GETG( ((ColorHistographItem*)ch1)->RGBAColor ) -
           (int) QPAM_GETG( ((ColorHistographItem*)ch2)->RGBAColor );
}
//---------------------------------------------------------------------------
static int bluecompare( const void *ch1, const void *ch2 )
{
    return (int) QPAM_GETB( ((ColorHistographItem*)ch1)->RGBAColor ) -
           (int) QPAM_GETB( ((ColorHistographItem*)ch2)->RGBAColor );
}
//---------------------------------------------------------------------------
static int alphacompare( const void *ch1, const void *ch2 )
{
    return (int) QPAM_GETA( ((ColorHistographItem*)ch1)->RGBAColor ) -
           (int) QPAM_GETA( ((ColorHistographItem*)ch2)->RGBAColor );
}
//---------------------------------------------------------------------------
ColorHistographList::ColorHistographList()
{
    ms_uiNumItems++;
}
//---------------------------------------------------------------------------
ColorHistographList::~ColorHistographList()
{
    ms_uiNumItems--;
}
//---------------------------------------------------------------------------
unsigned int ColorHistographList::ms_uiNumItems = 0;
