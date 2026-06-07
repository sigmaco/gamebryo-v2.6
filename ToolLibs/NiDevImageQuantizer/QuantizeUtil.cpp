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
#include <time.h> 
#include <process.h>
#include <memory.h> 
#include <NiSystem.h>
#include <NiDebug.h>

#include "AlphaMapUtility.h"

#define MIN(a,b)  ((a) < (b)? (a) : (b))

#define MAXCOLORS  32767
#define FS_SCALE   1024
#define ERR_DIV 16

static int BoxSumCompare (const void *b1, const void *b2);

//---------------------------------------------------------------------------
ImgInfo::~ImgInfo()
{
    delete[] pucIndexedData;  // write
    delete[] ppucRowPointers;// read/write
    delete[] pucSaveIndexData;
}

//---------------------------------------------------------------------------
QuantizeUtil::QuantizeUtil()
{
    m_pkAMP = NULL;
    m_iErrCode = 0;
    m_uiReqColors = 256;
}
//---------------------------------------------------------------------------
QuantizeUtil::~QuantizeUtil()
{
    delete m_pkAMP;
}
//---------------------------------------------------------------------------
void QuantizeUtil::SetNumberOfColorBits( unsigned int uiColorBits )
{
    // Only 4bit and 8bit currently supported.
    switch(uiColorBits)
    {
        case 4:
            m_uiReqColors = 16;
            break;
        case 8:
        default:
            m_uiReqColors = 256;
            break;
    }
}
//---------------------------------------------------------------------------
unsigned int QuantizeUtil::GetNumberOfPaletteColors()
{
    return m_uiReqColors;
}
//---------------------------------------------------------------------------
int QuantizeUtil::Quantize( ImgInfo* pImgInfo )
{
    RGBAPixel **ppkPixels;
    RGBAPixel *pkP;
    int iCol, iLimitCol;
    int iRow = 0;
    int iIndex;
    int iColors;
    int iNewColors = 0;
    unsigned char ucMaxVal = 255;
    unsigned char ucNewMaxVal;
    int iVerbose = 0;
    int iBottomIndex, iTopIndex;
    unsigned long ulRows, ulCols;
    int aiRemap[256];
    unsigned char *pQ, *pucOutRow, **ppucRowPointers=NULL;
    int iUseHash;
    int iFloyd = 1;
    int iFSDirection = 0;

    m_pkAMP = new NiAlphaMapUtility;

    // NOTE:  rgba_data and ppucRowPointers are allocated but not freed in
    //        rwpng_read_image() 
    ppkPixels = (RGBAPixel **)pImgInfo->ppucRowPointers;
    ulCols = pImgInfo->ulWidth;
    ulRows = pImgInfo->ulHeight;
    
    // Step 2: attempt to make a histogram of the colors, unclustered.
    // If at first we don't succeed, lower maxval to increase color
    // coherence and try again.  This will eventually terminate, with
    // maxval at worst 15, since 32^3 is approximately MAXCOLORS.
    //          [GRR POSSIBLE BUG:  what about 32^4 ?]
    for ( ; ; )
    {
        if (iVerbose)
        {
            NiOutputDebugString(" Making Histograph...\n");
        }

        if (m_pkAMP->ComputeColorHistograph( ppkPixels, ulCols, ulRows, 
            MAXCOLORS, &iColors ) != 0)
            break;

        ucNewMaxVal = ucMaxVal / 2;
        if (iVerbose)
        {
            NiOutputDebugString("Too many colors!\n");
            NiOutputDebugString(" Scaling colors to improve clustering\n");
            // from Max=%d to Max=%d\n", ucMaxVal, ucNewMaxVal);
        }

        for ( iRow = 0; (unsigned long)iRow < ulRows; ++iRow )
        {
            for ( iCol = 0, pkP = ppkPixels[iRow]; 
                (unsigned long)iCol < ulCols; ++iCol, ++pkP )
            {
                QPAM_DEPTH( *pkP, *pkP, 
                    ucMaxVal, ucNewMaxVal );
            }
        }

        ucMaxVal = ucNewMaxVal;
    }
    
    if (iVerbose) 
    {
        NiOutputDebugString("x colors found\n"); //%d colors found\n",iColors);
    }
    
    iNewColors = MIN(iColors, (int)m_uiReqColors);

    // Step 3: apply median-cut to histogram, making the new colormap.
    if (iVerbose && iColors > (int)m_uiReqColors) 
    {
        NiOutputDebugString(" Choosing new colors\n");
        // "  choosing %d iColors...\n", iNewColors);
    }

    MedianCut( iColors, ulRows * ulCols, ucMaxVal, iNewColors );
    
    if (m_iErrCode)
        return 17;

    // Step 3.4 [GRR]: set the bit-depth appropriately, given the actual
    // number of colors that will be used in the output image.

    if (iNewColors <= 2)
        pImgInfo->iSampleDepth = 1;
    else if (iNewColors <= 4)
        pImgInfo->iSampleDepth = 2;
    else if (iNewColors <= 16)
        pImgInfo->iSampleDepth = 4;
    else
        pImgInfo->iSampleDepth = 8;
    if (iVerbose) 
    {
        NiOutputDebugString(" Writing X-bit colormapped image\n");
        // "  writing %d-bit colormapped image\n",pImgInfo->iSampleDepth);
    }

    // Step 3.5 [GRR]: remap the palette colors so that all entries with
    // the maximal alpha value (i.e., fully opaque) are at the end and can
    // therefore be omitted from the tRNS chunk.  Note that the ordering of
    // opaque entries is reversed from how Step 3 arranged them--not that
    // this should matter to anyone.

    if (iVerbose) 
    {
        NiOutputDebugString("Remapping colormap to eliminate opaqueness\n");
        // "remapping colormap to eliminate opaque tRNS-chunk entries...");
    }

    int x;
    for (iTopIndex = iNewColors-1, 
        iBottomIndex = x = 0;  x < iNewColors;  ++x) 
    {
        if (m_pkAMP->GetColorMapA(x) == ucMaxVal)
            aiRemap[x] = iTopIndex--;
        else
            aiRemap[x] = iBottomIndex++;
    }
    if (iVerbose) 
    {
        NiOutputDebugString(".\n");
        // "%d entr%s left\n", iBottomIndex,(iBottomIndex == 1)? "y" : "ies");
    }

    // sanity check:  top and bottom indices should have just crossed paths 
    if (iBottomIndex != iTopIndex + 1) 
    {
        NiOutputDebugString("Internal logic error");
        //  "internal logic err: Remap iBottomIndex = %d, iTopIndex = %d\n",
        //  iBottomIndex, iTopIndex);
        if (pImgInfo->ppucRowPointers)
            delete pImgInfo->ppucRowPointers;
        //if (pImgInfo->pucRGBAData)
        //    delete pImgInfo->pucRGBAData;
        return 18;
    }

    pImgInfo->iNumPalette = iNewColors;
    pImgInfo->iNumTrans = iBottomIndex;
    // GRR TO DO:  if iBottomIndex == 0, 
    //               check whether all RGB samples are gray
    //               and if so, whether grayscale sample_depth would be same
    //               => skip following palette section and go grayscale 


    // Step 3.6 [GRR]: rescale the palette colors to a maxval of 255, as
    // required by the PNG spec.  (Technically, the actual remapping happens
    // in here, too.)
    if (ucMaxVal < 255) 
    {
        if (iVerbose) 
        {
            NiOutputDebugString(" Rescaling\n");
            //  "  rescaling colormap colors from ucMaxVal=%d to "
            //  "ucMaxVal=255\n", ucMaxVal);
        }
        for (x = 0; x < iNewColors; ++x) 
        {
            /* the rescaling part of this is really just QPAM_DEPTH() broken 
             * out for the PNG palette; the trans-remapping just puts the
             * values in different slots in the PNG palette */
            int r,g,b,a;
            m_pkAMP->GetColorMapRGBA(x,&r,&g,&b,&a);

            pImgInfo->Palette[aiRemap[x]].ucRed
              = (unsigned char)((r*255 + (ucMaxVal >> 1)) / ucMaxVal);
            pImgInfo->Palette[aiRemap[x]].ucGreen
              = (unsigned char)((g*255 + (ucMaxVal >> 1)) / ucMaxVal);
            pImgInfo->Palette[aiRemap[x]].ucBlue
              = (unsigned char)((b*255 + (ucMaxVal >> 1)) / ucMaxVal);
            pImgInfo->Palette[aiRemap[x]].ucAlpha 
              = (unsigned char)((a*255 + (ucMaxVal >> 1)) / ucMaxVal);
        }
        // GRR TO DO:  set sBIT flag appropriately 
    } 
    else 
    {
        for (x = 0; x < iNewColors; ++x) 
        {
            int r,g,b,a;
            m_pkAMP->GetColorMapRGBA(x,&r,&g,&b,&a);
            pImgInfo->Palette[aiRemap[x]].ucRed = (unsigned char)r;
            pImgInfo->Palette[aiRemap[x]].ucGreen = (unsigned char)g;
            pImgInfo->Palette[aiRemap[x]].ucBlue = (unsigned char)b;
            pImgInfo->Palette[aiRemap[x]].ucAlpha = (unsigned char)a;
        }
    }


    // Step 3.7 [GRR]: allocate memory for either a single row (non-
    // interlaced -> progressive write) or the entire indexed image
    // (interlaced -> all at once); note that pImgInfo.ppucRowPointers
    // is still in use via ppkPixels (INPUT data).
    
    if (pImgInfo->iInterlaced) 
    {
        if ((pImgInfo->pucIndexedData = 
            new unsigned char[iRow*ulCols]) != NULL) 
        {
            if ((ppucRowPointers = new unsigned char*[ulRows]) != NULL) 
            {
                for (iRow = 0;  (unsigned long)iRow < ulRows;  ++iRow)
                {
                    ppucRowPointers[iRow] =
                        pImgInfo->pucIndexedData + iRow*ulCols;
                }
            }
        }
    }
    else
        pImgInfo->pucIndexedData = new unsigned char[ulCols];

    pImgInfo->pucSaveIndexData = new unsigned char[ulRows * ulCols];
    unsigned int uiSaveIndexDataSize = 
        sizeof(unsigned char) * (ulRows * ulCols);

    if (pImgInfo->pucIndexedData == NULL ||
        (pImgInfo->iInterlaced && ppucRowPointers == NULL))
    {
        NiOutputDebugString("Insufficient Memory.\n");
        //  "  insufficient memory for indexed data and/or row pointers\n");
        if (pImgInfo->ppucRowPointers)
            delete pImgInfo->ppucRowPointers;
        //if (pImgInfo->pucRGBAData)
        //    delete pImgInfo->pucRGBAData;
        if (pImgInfo->pucIndexedData)
            delete pImgInfo->pucIndexedData;
        return 17;
    }


    // Step 4: map the colors in the image to their closest match in the
    // new colormap, and write 'em out.
    if (iVerbose) 
    {
        NiOutputDebugString("Mapping image to new colors\n");
    }

    iUseHash = 1;

    long *lThisErrR = NULL;
    long *lNextErrR = NULL;
    long *lThisErrG = NULL;
    long *lNextErrG = NULL;
    long *lThisErrB = NULL;
    long *lNextErrB = NULL;
    long *lThisErrA = NULL;
    long *lNextErrA = NULL;
    long *lTempErr;

    if ( iFloyd ) 
    {
        // Initialize iFloyd-Steinberg error vectors. 
        lThisErrR = new long[ulCols + 2];
        lNextErrR = new long[ulCols + 2];
        lThisErrG = new long[ulCols + 2];
        lNextErrG = new long[ulCols + 2];
        lThisErrB = new long[ulCols + 2];
        lNextErrB = new long[ulCols + 2];
        lThisErrA = new long[ulCols + 2];
        lNextErrA = new long[ulCols + 2];

        srand(0);
        for ( iCol = 0; (unsigned long)iCol < ulCols + 2; ++iCol ) 
        {
            lThisErrR[iCol] = rand( ) % ( FS_SCALE * 2 ) - FS_SCALE;
            lThisErrG[iCol] = rand( ) % ( FS_SCALE * 2 ) - FS_SCALE;
            lThisErrB[iCol] = rand( ) % ( FS_SCALE * 2 ) - FS_SCALE;
            lThisErrA[iCol] = rand( ) % ( FS_SCALE * 2 ) - FS_SCALE;
            // (random errors in [-1 .. 1]) 
        }
        iFSDirection = 1;
    }

    long lScaledR=0, lScaledG=0, lScaledB=0, lScaledA=0;

    for ( iRow = 0; (unsigned long)iRow < ulRows; ++iRow ) 
    {
        pucOutRow = pImgInfo->iInterlaced? ppucRowPointers[iRow] :
                                        pImgInfo->pucIndexedData;
        if ( iFloyd )
            for ( iCol = 0; (unsigned long)iCol < ulCols + 2; ++iCol )
                lNextErrR[iCol] = lNextErrG[iCol] =
                lNextErrB[iCol] = lNextErrA[iCol] = 0;
        if ( ( ! iFloyd ) || iFSDirection ) 
        {
            iCol = 0;
            iLimitCol = ulCols;
            pkP = ppkPixels[iRow];
            pQ = pucOutRow;
        }
        else 
        {
            iCol = ulCols - 1;
            iLimitCol = -1;
            pkP = &(ppkPixels[iRow][iCol]);
            pQ = &(pucOutRow[iCol]);
        }
        do 
        {

            if ( iFloyd ) 
            {
                // Use iFloyd-Steinberg errors to adjust actual color. 
                lScaledR = QPAM_GETR(*pkP) + lThisErrR[iCol + 1] / FS_SCALE;
                lScaledG = QPAM_GETG(*pkP) + lThisErrG[iCol + 1] / FS_SCALE;
                lScaledB = QPAM_GETB(*pkP) + lThisErrB[iCol + 1] / FS_SCALE;
                lScaledA = QPAM_GETA(*pkP) + lThisErrA[iCol + 1] / FS_SCALE;
                if ( lScaledR < 0 ) lScaledR = 0;
                else if ( lScaledR > ucMaxVal ) lScaledR = ucMaxVal;
                if ( lScaledG < 0 ) lScaledG = 0;
                else if ( lScaledG > ucMaxVal ) lScaledG = ucMaxVal;
                if ( lScaledB < 0 ) lScaledB = 0;
                else if ( lScaledB > ucMaxVal ) lScaledB = ucMaxVal;
                if ( lScaledA < 0 ) lScaledA = 0;
                else if ( lScaledA > ucMaxVal ) lScaledA = ucMaxVal;
                // GRR 20001228:  added casts to quiet warnings; 255 DEPENDENCY
                QPAM_ASSIGN( *pkP, (unsigned char)lScaledR,
                    (unsigned char)lScaledG, (unsigned char)lScaledB, 
                    (unsigned char)lScaledA );
            }

            // Check hash table to see if we have already matched this color. 
            iIndex = m_pkAMP->LookUpColor( pkP );

            if ( iIndex == -1 ) 
            {
                // No; search acolormap for closest match.
                int i, r1, g1, b1, a1, r2, g2, b2, a2;
                long dist, newdist;

                r1 = QPAM_GETR( *pkP );
                g1 = QPAM_GETG( *pkP );
                b1 = QPAM_GETB( *pkP );
                a1 = QPAM_GETA( *pkP );
                dist = 2000000000;
                for ( i = 0; i < iNewColors; ++i ) 
                {
                    m_pkAMP->GetColorMapRGBA( i, &r2, &g2, &b2, &a2 );

                    // GRR POSSIBLE BUG 
                    newdist = ( r1 - r2 ) * ( r1 - r2 ) +  // may overflow?
                              ( g1 - g2 ) * ( g1 - g2 ) +
                              ( b1 - b2 ) * ( b1 - b2 ) +
                              ( a1 - a2 ) * ( a1 - a2 );
                    if ( newdist < dist ) 
                    {
                        iIndex = i;
                        dist = newdist;
                    }
                }
                if ( iUseHash ) 
                {
                    if ( m_pkAMP->AddColorToHash( pkP, iIndex ) < 0 )
                    {
                        if (iVerbose) 
                        {
                            NiOutputDebugString("Out of Mem\n");
                            // "  out of memory adding to hash"
                            //  " table, proceeding without it\n");
                        }
                        iUseHash = 0;
                    }
                }
            }

            if ( iFloyd )
            {
                int r,g,b,a;

                // Propagate iFloyd-Steinberg error terms.
                if ( iFSDirection )
                {
                    long lErr;

                    m_pkAMP->GetColorMapRGBA( iIndex, &r, &g, &b, &a );
                    lErr = (lScaledR - (long)r)*FS_SCALE;
                    lThisErrR[iCol + 2] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrR[iCol    ] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrR[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrR[iCol + 2] += ( lErr     ) / ERR_DIV;
                    lErr = (lScaledG - (long)g)*FS_SCALE;
                    lThisErrG[iCol + 2] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrG[iCol    ] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrG[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrG[iCol + 2] += ( lErr     ) / ERR_DIV;
                    lErr = (lScaledB - (long)b)*FS_SCALE;
                    lThisErrB[iCol + 2] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrB[iCol    ] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrB[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrB[iCol + 2] += ( lErr     ) / ERR_DIV;
                    lErr = (lScaledA - (long)a)*FS_SCALE;
                    lThisErrA[iCol + 2] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrA[iCol    ] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrA[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrA[iCol + 2] += ( lErr     ) / ERR_DIV;
                } 
                else 
                {
                    long lErr;
                    m_pkAMP->GetColorMapRGBA( iIndex, &r, &g, &b, &a );
                    lErr = (lScaledR - (long)r)*FS_SCALE;
                    lThisErrR[iCol    ] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrR[iCol + 2] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrR[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrR[iCol    ] += ( lErr     ) / ERR_DIV;
                    lErr = (lScaledG - (long)g)*FS_SCALE;
                    lThisErrG[iCol    ] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrG[iCol + 2] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrG[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrG[iCol    ] += ( lErr     ) / ERR_DIV;
                    lErr = (lScaledB - (long)b)*FS_SCALE;
                    lThisErrB[iCol    ] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrB[iCol + 2] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrB[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrB[iCol    ] += ( lErr     ) / ERR_DIV;
                    lErr = (lScaledA - (long)a)*FS_SCALE;
                    lThisErrA[iCol    ] += ( lErr * 7 ) / ERR_DIV;
                    lNextErrA[iCol + 2] += ( lErr * 3 ) / ERR_DIV;
                    lNextErrA[iCol + 1] += ( lErr * 5 ) / ERR_DIV;
                    lNextErrA[iCol    ] += ( lErr     ) / ERR_DIV;
                }
            }

            *pQ = (unsigned char)aiRemap[iIndex];

            if ( ( ! iFloyd ) || iFSDirection ) 
            {
                ++iCol;
                ++pkP;
                ++pQ;
            } 
            else 
            {
                --iCol;
                --pkP;
                --pQ;
            }
        }
        while ( iCol != iLimitCol );

        if ( iFloyd ) 
        {
            lTempErr = lThisErrR;
            lThisErrR = lNextErrR;
            lNextErrR = lTempErr;
            lTempErr = lThisErrG;
            lThisErrG = lNextErrG;
            lNextErrG = lTempErr;
            lTempErr = lThisErrB;
            lThisErrB = lNextErrB;
            lNextErrB = lTempErr;
            lTempErr = lThisErrA;
            lThisErrA = lNextErrA;
            lNextErrA = lTempErr;
            iFSDirection = ! iFSDirection;
        }

        // if non-interlaced PNG, write row now 
        if (!pImgInfo->iInterlaced)
        {
            unsigned uiDestSize = uiSaveIndexDataSize - 
                ( sizeof(unsigned char) * (iRow * ulCols));
#if _MSC_VER >= 1400
            int iRet = 
                memcpy_s(pImgInfo->pucSaveIndexData + (iRow*ulCols),
                uiDestSize, pImgInfo->pucIndexedData, ulCols); 
            NI_UNUSED_ARG(iRet);
            NIASSERT(iRet == 0);
#else // #if _MSC_VER >= 1400
            memcpy(pImgInfo->pucSaveIndexData + (iRow*ulCols),
                pImgInfo->pucIndexedData, ulCols); 
#endif // #if _MSC_VER >= 1400
        }

        delete [] lThisErrR; 
        delete [] lNextErrR;
        delete [] lThisErrG; 
        delete [] lNextErrG;
        delete [] lThisErrB; 
        delete [] lNextErrB;
        delete [] lThisErrA;
        delete [] lNextErrA;
    }
    delete m_pkAMP;
    m_pkAMP = NULL;
    return 0;
}
//---------------------------------------------------------------------------
// Here is the fun part, the median-cut colormap generator.  This is based
// on Paul Heckbert's paper, "Color Image Quantization for Frame Buffer
// Display," SIGGRAPH 1982 Proceedings, page 297.
//---------------------------------------------------------------------------
void QuantizeUtil::MedianCut( int iColors, int iSum, 
    unsigned char ucMaxVal, int iNewColors )
{
    ColorBox* bv;
    int bi, i;
    int boxes;
    
    bv = new ColorBox[iNewColors];

    m_pkAMP->CreateColorMap( iNewColors );

    if ( bv == (ColorBox*) 0 )
    {
        NiOutputDebugString("Out of Memory Allocating ColorBox");
        m_iErrCode = 6;
        return;
    }

    // Set up the initial ColorBox.
    bv[0].iIndex = 0;
    bv[0].iColors = iColors;
    bv[0].iSum = iSum;
    boxes = 1;

    // Main loop: split boxes until we have enough.
    while ( boxes < iNewColors ) 
    {
        int indx, clrs;
        int sm;
        int halfsum, lowersum;

        // Find the first splittable ColorBox.
        for ( bi = 0; bi < boxes; ++bi )
            if ( bv[bi].iColors >= 2 )
                break;
        if ( bi == boxes )
            break;        // ran out of colors!
        indx = bv[bi].iIndex;
        clrs = bv[bi].iColors;
        sm = bv[bi].iSum;

        m_pkAMP->SortHistograph(indx,clrs);

        // Now find the median based on the counts, so that about half the
        // pixels (not colors, pixels) are in each subdivision.
        lowersum = m_pkAMP->GetHistographValue(indx); 
        halfsum = sm / 2;
        for ( i = 1; i < clrs - 1; ++i )
        {
            if ( lowersum >= halfsum )
                break;
            lowersum += m_pkAMP->GetHistographValue(indx + i); 
        }

        // Split the ColorBox, and sort to bring the biggest boxes to the top.
        bv[bi].iColors = i;
        bv[bi].iSum = lowersum;
        bv[boxes].iIndex = indx + i;
        bv[boxes].iColors = clrs - i;
        bv[boxes].iSum = sm - lowersum;
        ++boxes;
        qsort( (char*) bv, boxes, sizeof(struct ColorBox), BoxSumCompare );
    }

    // Ok, we've got enough boxes.  Now choose a representative color for
    // each box.  There are a number of possible ways to make this choice.
    // One would be to choose the center of the box; this ignores any structure
    // within the boxes.  Another method would be to average all the colors in
    // the box - this is the method specified in Heckbert's paper.  A third
    // method is to average all the pixels in the box.  You can switch which
    // method is used by switching the commenting on the REP_ defines at
    // the beginning of this source file.
    for ( bi = 0; bi < boxes; ++bi ) 
    {
        int indx = bv[bi].iIndex;
        int clrs = bv[bi].iColors;
        long r = 0, g = 0, b = 0, a = 0, sum = 0;

        for ( i = 0; i < clrs; ++i )
        {
            r += m_pkAMP->GetHistographRxVal( indx + i );
            g += m_pkAMP->GetHistographGxVal( indx + i );
            b += m_pkAMP->GetHistographBxVal( indx + i );
            a += m_pkAMP->GetHistographAxVal( indx + i );
            sum += m_pkAMP->GetHistographValue( indx + i );
        }
        r = r / sum;
        if ( r > ucMaxVal ) r = ucMaxVal;        // avoid math errors 
        g = g / sum;
        if ( g > ucMaxVal ) g = ucMaxVal;
        b = b / sum;
        if ( b > ucMaxVal ) b = ucMaxVal;
        a = a / sum;
        if ( a > ucMaxVal ) a = ucMaxVal;
        // GRR 20001228:  added casts to quiet warnings; 255 DEPENDENCY
        m_pkAMP->SetColorMapRGBA( 
            bi, (unsigned char)r, (unsigned char)g,
            (unsigned char)b, (unsigned char)a );
    }

    delete [] bv;
    m_pkAMP->SetHistograph( NULL );
}
//---------------------------------------------------------------------------
static int BoxSumCompare( const void *b1, const void *b2 )
{
    return ((ColorBox*)b2)->iSum -
           ((ColorBox*)b1)->iSum;
}
//---------------------------------------------------------------------------
