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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiPixelData.h"
#include "NiTextureCodec.h"

//---------------------------------------------------------------------------
NiTextureCodec::NiTextureCodec()
{
    m_pfnAlphaBlockDecoder = NULL;
    m_pfnColorBlockDecoder = &NiTextureCodec::DecodeColorBlock;

    m_pucSrcPtr = NULL;

    m_uiDestRowStride = 0;

    for (unsigned int i = 0; i < 256; i++)
    {
        m_aaucExplicitAlphaLookup[i][0] 
            = (unsigned char)(((i & 0xf) / 15.0f) * 255.0f);
        m_aaucExplicitAlphaLookup[i][1] 
            = (unsigned char)((((i & 0xf0) >> 4) / 15.0f) * 255.0f);
    }
}
//---------------------------------------------------------------------------
NiTextureCodec::~NiTextureCodec()
{
    /* */
}
//---------------------------------------------------------------------------
void NiTextureCodec::DecodeT3AlphaBlock(unsigned char* pucAlphas)
{
    for (unsigned int i = 0; i < 8; i++)
    {
        *pucAlphas = m_aaucExplicitAlphaLookup[*m_pucSrcPtr][0];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = m_aaucExplicitAlphaLookup[*m_pucSrcPtr][1];
        pucAlphas += DEST_PIXEL_STRIDE;
        m_pucSrcPtr++;
    }
}
//---------------------------------------------------------------------------
void NiTextureCodec::DecodeT5AlphaBlock(unsigned char* pucAlphas)
{
    // Pull the two base alpha values from the block data
    unsigned char aucAlphaTable[8];
    
    unsigned int uiA0 = aucAlphaTable[0] = *(m_pucSrcPtr++);
    unsigned int uiA1 = aucAlphaTable[1] = *(m_pucSrcPtr++);

    // Determine the interpolated alphas by looking at the sorted order of 
    // the two base alphas - these select between the two sub-modes
    if (uiA0 > uiA1)
    {
        // 8-alpha block
        aucAlphaTable[2] = (unsigned char)((uiA0 * 6 + uiA1) / 7);
        aucAlphaTable[3] = (unsigned char)((uiA0 * 5 + uiA1 * 2) / 7);
        aucAlphaTable[4] = (unsigned char)((uiA0 * 4 + uiA1 * 3) / 7);
        aucAlphaTable[5] = (unsigned char)((uiA0 * 3 + uiA1 * 4) / 7);
        aucAlphaTable[6] = (unsigned char)((uiA0 * 2 + uiA1 * 5) / 7);
        aucAlphaTable[7] = (unsigned char)((uiA0 + uiA1 * 6) / 7);
    }
    else
    {
        // 6-alpha block
        aucAlphaTable[2] = (unsigned char)((uiA0 * 4 + uiA1) / 5);
        aucAlphaTable[3] = (unsigned char)((uiA0 * 3 + uiA1 * 2) / 5);
        aucAlphaTable[4] = (unsigned char)((uiA0 * 2 + uiA1 * 3) / 5);
        aucAlphaTable[5] = (unsigned char)((uiA0 + uiA1 * 4) / 5);

        aucAlphaTable[6] = 0;
        aucAlphaTable[7] = 255;
    }

    // since the 3-bits per pixel situations cross byte boundaries, we must
    // take care to avoid endian-unfriendly situations.  This only happens
    // four times in a 16-pixel block, however.
    for (unsigned int i = 0; i < 2; i++)
    {
        *pucAlphas = aucAlphaTable[m_pucSrcPtr[0] & 0x7];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[(m_pucSrcPtr[0] >> 3) & 0x7];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[((m_pucSrcPtr[0] >> 6) & 0x3) 
            + ((m_pucSrcPtr[1] << 2) & 0x4)];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[(m_pucSrcPtr[1] >> 1) & 0x7];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[(m_pucSrcPtr[1] >> 4) & 0x7];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[((m_pucSrcPtr[1] >> 7) & 0x1) 
            + ((m_pucSrcPtr[2] << 1) & 0x6)];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[(m_pucSrcPtr[2] >> 2) & 0x7];
        pucAlphas += DEST_PIXEL_STRIDE;
        *pucAlphas = aucAlphaTable[(m_pucSrcPtr[2] >> 5) & 0x7];
        pucAlphas += DEST_PIXEL_STRIDE;

        m_pucSrcPtr += 3;
    }
}
//---------------------------------------------------------------------------
void NiTextureCodec::DecodeColorBlock(unsigned char* pucDest)
{
    // Read the two block colors and compute the others (endian-friendly)
    unsigned short usColor0 
        = (((unsigned short)m_pucSrcPtr[1]) << 8) + m_pucSrcPtr[0];
    m_pucSrcPtr += 2;
    unsigned short usColor1 
        = (((unsigned short)m_pucSrcPtr[1]) << 8) + m_pucSrcPtr[0];
    m_pucSrcPtr += 2;

    unsigned char aaucColors[4][3];
    aaucColors[0][0] = (unsigned char)((usColor0 & 0xf800) >> 8);
    aaucColors[0][1] = (unsigned char)((usColor0 & 0x07f0) >> 3);
    aaucColors[0][2] = (unsigned char)((usColor0 & 0x001f) << 3);

    aaucColors[1][0] = (unsigned char)((usColor1 & 0xf800) >> 8);
    aaucColors[1][1] = (unsigned char)((usColor1 & 0x07f0) >> 3);
    aaucColors[1][2] = (unsigned char)((usColor1 & 0x001f) << 3);

    NIASSERT(usColor0 >= usColor1);

    // Opaque, four color block
    // compute the two intermediate colors
    aaucColors[2][0] = (unsigned char)(((
        ((unsigned int)aaucColors[0][0]) << 1) 
        + ((unsigned int)aaucColors[1][0])) / 3);
    aaucColors[2][1] = (unsigned char)(((
        ((unsigned int)aaucColors[0][1]) << 1) 
        + ((unsigned int)aaucColors[1][1])) / 3);
    aaucColors[2][2] = (unsigned char)(((
        ((unsigned int)aaucColors[0][2]) << 1) 
        + ((unsigned int)aaucColors[1][2])) / 3);

    aaucColors[3][0] = (unsigned char)(((
        ((unsigned int)aaucColors[1][0]) << 1) 
        + ((unsigned int)aaucColors[0][0])) / 3);
    aaucColors[3][1] = (unsigned char)(((
        ((unsigned int)aaucColors[1][1]) << 1) 
        + ((unsigned int)aaucColors[0][1])) / 3);
    aaucColors[3][2] = (unsigned char)(((
        ((unsigned int)aaucColors[1][2]) << 1) 
        + ((unsigned int)aaucColors[0][2])) / 3);

    unsigned int uiBlockRow;
    for (uiBlockRow = 0; uiBlockRow < 4; uiBlockRow++)
    {
        unsigned char ucPixel = *m_pucSrcPtr;

        for (unsigned int uiBlockCol = 0; uiBlockCol < 4; uiBlockCol++)
        {
            unsigned char* pucFinalPixel = aaucColors[(ucPixel & 0x03)];
            *pucDest = *(pucFinalPixel++);
            *(pucDest+1) = *(pucFinalPixel++);
            *(pucDest+2) = *(pucFinalPixel++);

            pucDest += DEST_PIXEL_STRIDE;

            ucPixel >>= 2;
        }

        m_pucSrcPtr++;
    }
}
//---------------------------------------------------------------------------
void NiTextureCodec::DecodeColorAlphaBlock(unsigned char* pucDest)
{
    // Read the two block colors and compute the others (endian-friendly)
    unsigned short usColor0 
        = (((unsigned short)m_pucSrcPtr[1])<<8) + m_pucSrcPtr[0];
    m_pucSrcPtr += 2;
    unsigned short usColor1 
        = (((unsigned short)m_pucSrcPtr[1])<<8) + m_pucSrcPtr[0];
    m_pucSrcPtr += 2;

    unsigned char aaucColors[4][4];
    aaucColors[0][0] = (unsigned char)((usColor0 & 0xf800) >> 8);
    aaucColors[0][1] = (unsigned char)((usColor0 & 0x07f0) >> 3);
    aaucColors[0][2] = (unsigned char)((usColor0 & 0x001f) << 3);
    aaucColors[0][3] = 255;

    aaucColors[1][0] = (unsigned char)((usColor1 & 0xf800) >> 8);
    aaucColors[1][1] = (unsigned char)((usColor1 & 0x07f0) >> 3);
    aaucColors[1][2] = (unsigned char)((usColor1 & 0x001f) << 3);
    aaucColors[1][3] = 255;

    if (usColor0 > usColor1)
    {
        // Opaque, four color block
        // compute the two intermediate colors
        aaucColors[2][0] = (unsigned char)(((
            ((unsigned int)aaucColors[0][0]) << 1) 
            + ((unsigned int)aaucColors[1][0])) / 3);
        aaucColors[2][1] = (unsigned char)(((
            ((unsigned int)aaucColors[0][1]) << 1) 
            + ((unsigned int)aaucColors[1][1])) / 3);
        aaucColors[2][2] = (unsigned char)(((
            ((unsigned int)aaucColors[0][2]) << 1) 
            + ((unsigned int)aaucColors[1][2])) / 3);
        aaucColors[2][3] = 255;

        aaucColors[3][0] = (unsigned char)(((
            ((unsigned int)aaucColors[1][0]) << 1) 
            + ((unsigned int)aaucColors[0][0])) / 3);
        aaucColors[3][1] = (unsigned char)(((
            ((unsigned int)aaucColors[1][1]) << 1) 
            + ((unsigned int)aaucColors[0][1])) / 3);
        aaucColors[3][2] = (unsigned char)(((
            ((unsigned int)aaucColors[1][2]) << 1) 
            + ((unsigned int)aaucColors[0][2])) / 3);
        aaucColors[3][3] = 255;
    }
    else
    {
        // Translucent, three color block
        // compute the intermediate and translucent colors
        aaucColors[2][0] = (unsigned char)((((unsigned int)aaucColors[0][0]) 
            + ((unsigned int)aaucColors[1][0])) >> 1);
        aaucColors[2][1] = (unsigned char)((((unsigned int)aaucColors[0][1]) 
            + ((unsigned int)aaucColors[1][1])) >> 1);
        aaucColors[2][2] = (unsigned char)((((unsigned int)aaucColors[0][2]) 
            + ((unsigned int)aaucColors[1][2])) >> 1);
        aaucColors[2][3] = 255;

        // use the intermediate color for the translucent pixels to avoid
        // potentially ugly interpolation to black or white at edges
        aaucColors[3][0] = aaucColors[2][0];
        aaucColors[3][1] = aaucColors[2][1];
        aaucColors[3][2] = aaucColors[2][2];
        aaucColors[3][3] = 0;
    }

    unsigned int uiBlockRow;
    for (uiBlockRow = 0; uiBlockRow < 4; uiBlockRow++)
    {
        unsigned char ucPixel = *m_pucSrcPtr;

        unsigned int uiBlockCol;
        for (uiBlockCol = 0; uiBlockCol < 4; uiBlockCol++)
        {
            unsigned char* pucFinalPixel = aaucColors[(ucPixel & 0x03)];
            *pucDest = *(pucFinalPixel++);
            *(pucDest + 1) = *(pucFinalPixel++);
            *(pucDest + 2) = *(pucFinalPixel++);
            *(pucDest + 3) = *(pucFinalPixel++);

            pucDest += DEST_PIXEL_STRIDE;

            ucPixel >>= 2;
        }

        m_pucSrcPtr++;
    }
}
//---------------------------------------------------------------------------
void NiTextureCodec::DecodeBlock(unsigned char* pucDest, unsigned int uiCols, 
    unsigned int uiRows)
{
    unsigned char aucColorBlock[DEST_PIXEL_STRIDE * 16];
    unsigned char* pucColor = aucColorBlock;

    // pucColor + 3 is the pointer to the alpha byte of the first pixel
    if (m_pfnAlphaBlockDecoder)
        (this->*m_pfnAlphaBlockDecoder)(pucColor + 3);
    
    (this->*m_pfnColorBlockDecoder)(pucColor);

    // copy the block (or sub-block) to the destination
    unsigned int uiDestRowLength = uiCols * DEST_PIXEL_STRIDE;
    for (unsigned int uiBlockRow = 0; uiBlockRow < uiRows; uiBlockRow++)
    {
        NiMemcpy(pucDest, pucColor, uiDestRowLength);
        pucDest += m_uiDestRowStride;
        pucColor += (DEST_PIXEL_STRIDE << 2);
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiTextureCodec::ExpandCompressedData(const NiPixelData& kSrc)
{
    // return NULL if the source is not compressed
    switch (kSrc.GetPixelFormat().GetFormat())
    {
    case NiPixelFormat::FORMAT_DXT1:
        m_pfnAlphaBlockDecoder = NULL;
        m_pfnColorBlockDecoder = &NiTextureCodec::DecodeColorAlphaBlock;
        break;
    case NiPixelFormat::FORMAT_DXT3:
        m_pfnAlphaBlockDecoder = &NiTextureCodec::DecodeT3AlphaBlock;
        m_pfnColorBlockDecoder = &NiTextureCodec::DecodeColorBlock;
        break;
    case NiPixelFormat::FORMAT_DXT5:
        m_pfnAlphaBlockDecoder = &NiTextureCodec::DecodeT5AlphaBlock;
        m_pfnColorBlockDecoder = &NiTextureCodec::DecodeColorBlock;
        break;
    default:
        return NULL;
    };

    // Create the destination data - we must use RGBA32 because any of the
    // compressed formats can support alpha (in some cases, one-bit alpha)
    NiPixelData* pkDest = NiNew NiPixelData(kSrc.GetWidth(), kSrc.GetHeight(), 
        NiPixelFormat::RGBA32, kSrc.GetNumMipmapLevels(), kSrc.GetNumFaces());

    unsigned int uiMipmapLevels = kSrc.GetNumMipmapLevels();
    unsigned int uiNumFaces = kSrc.GetNumFaces();

    // Loop over all mipmap levels
    unsigned int uiFace;
    for (uiFace = 0; uiFace < uiNumFaces; uiFace++)
    {
        unsigned int i;
        for (i = 0; i < uiMipmapLevels; i++)
        {
            m_pucSrcPtr = kSrc.GetPixels(i, uiFace);
            unsigned char* pucDestPixels = pkDest->GetPixels(i, uiFace);

            unsigned int uiWidth = kSrc.GetWidth(i, uiFace);
            unsigned int uiHeight = kSrc.GetHeight(i, uiFace);

            // Compute block-based info (a block is 4x4 pixels)
            unsigned int uiBlockCols = uiWidth >> 2;
            unsigned int uiBlockRows = uiHeight >> 2;
            unsigned int uiExtraCols = uiWidth & 0x3;
            unsigned int uiExtraRows = uiHeight & 0x3;

            m_uiDestRowStride = uiWidth * DEST_PIXEL_STRIDE;

            unsigned int uiDestCol, uiDestRow;
            for (uiDestRow = 0; uiDestRow < uiBlockRows; uiDestRow++)
            {
                // decode the current block
                for (uiDestCol = 0; uiDestCol < uiBlockCols; uiDestCol++)
                {
                    DecodeBlock(pucDestPixels, 4, 4);

                    pucDestPixels += DEST_PIXEL_STRIDE << 2; // 4 RGBA pixels
                }

                // If the width was not divisible by 4, then there will be one
                // partial block at the end of each row. The input block will 
                // be complete, but the output only covers a subset of the 
                // block pixels
                if (uiExtraCols)
                {
                    DecodeBlock(pucDestPixels, uiExtraCols, 4);

                    pucDestPixels += uiExtraCols * DEST_PIXEL_STRIDE;
                }

                // 3 rows (jump to next block)
                pucDestPixels += 3 * uiWidth * DEST_PIXEL_STRIDE; 
            }

            // If the height was not divisible by 4, then there will be one
            // partial row of blocks at the end of each row.  The input blocks
            // will be complete, but the output only covers a subset of the 
            // block pixels
            if (uiExtraRows)
            {
                // decode the current block
                for (uiDestCol = 0; uiDestCol < uiBlockCols; uiDestCol++)
                {
                    DecodeBlock(pucDestPixels, 4, uiExtraRows);

                    pucDestPixels += DEST_PIXEL_STRIDE << 2; // 4 RGBA pixels
                }

                // If the width was not divisible by 4, then there will be one 
                // partial block at the end of each row.  The input block will
                // be complete, but the output only covers a subset of the 
                // block pixels
                if (uiExtraCols)
                {
                    DecodeBlock(pucDestPixels, uiExtraCols, uiExtraRows);

                    pucDestPixels += uiExtraCols * DEST_PIXEL_STRIDE;
                }
            }
        }
    }

    return pkDest;
}
//---------------------------------------------------------------------------
