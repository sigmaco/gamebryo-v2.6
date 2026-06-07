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

#include "NiFileIStream.h"
#include "NiOpenEXRReader.h"
#include "NiBinaryStream.h"
#include "NiPalette.h"
#include "NiPixelData.h"

#include <NiFile.h>
#include <NiSystem.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4996)
#endif

#include <ImfRgba.h>
#include <ImfArray.h>
#include <ImathBox.h>

#include <ImfRgbaFile.h>
#include <ImfTiledRgbaFile.h>
#include <ImfInputFile.h>
#include <ImfTiledInputFile.h>
#include <ImfPreviewImage.h>
#include <ImfChannelList.h>

#include <Objidl.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

using namespace Imath;
using namespace Imf;
using namespace std;

//---------------------------------------------------------------------------
NiOpenEXRReader::NiOpenEXRReader()
{
    m_pfnUnpacker = NULL;
}
//---------------------------------------------------------------------------
NiOpenEXRReader::~NiOpenEXRReader()
{
}
//---------------------------------------------------------------------------
bool NiOpenEXRReader::ReadHeader(NiFile& kIst, unsigned int& uiWidth, 
    unsigned int& uiHeight, NiPixelFormat& kFormat, bool& bMipmap,
    unsigned int& uiNumFaces)
{
    uiWidth = 0;
    uiHeight = 0;
    kFormat = NiPixelFormat::RGBA64;
    bMipmap = false;
    uiNumFaces = 0;  

    // Grab generic interface
    NiFileIStream kStrm(&kIst);
    InputFile kFile(kStrm);

    // Ensure that RGBA channels exist
    Imf::ChannelList kCList = kFile.header().channels();
    NIASSERT(kCList.findChannel("R") && kCList.findChannel("G") &&
        kCList.findChannel("B") && kCList.findChannel("A"));
    if (!kCList.findChannel("R") || !kCList.findChannel("G") ||
        !kCList.findChannel("B") || !kCList.findChannel("A"))
    {
        // One of the RGBA channels could not be found and
        // we have an error.
        return false;
    }

    // Store base level width and height
    Box2i kDataWindow = kFile.header().dataWindow();
    int iMinX = kDataWindow.min.x;
    int iMinY = kDataWindow.min.y;
    int iMaxX = kDataWindow.max.x;
    int iMaxY = kDataWindow.max.y;

    NIASSERT(iMinX == 0);
    NIASSERT(iMinY == 0);

    uiWidth = iMaxX - iMinX + 1;
    uiHeight = iMaxY - iMinY + 1;

    // Determine if there are mipmap levels
    if (kFile.header().hasTileDescription())
    {
        const TileDescription kTD = kFile.header().tileDescription();

        if (kTD.mode == Imf::MIPMAP_LEVELS)
        {
            // will be necessary to use the tile interface
            bMipmap = true;
        }
    }
    kStrm.seekg(0);

    return true;
}
//---------------------------------------------------------------------------
bool NiOpenEXRReader::CanReadFile(const char* pcFileExtension) const
{
    if (!NiStricmp(pcFileExtension, ".exr")) 
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiOpenEXRReader::ReadFile(NiFile &kIst, NiPixelData* pkOptDest)
{
    bool bResult = ReadHeader(kIst, m_uiWidth, m_uiHeight, m_kFormat,
        m_bMipMap, m_uiNumFaces);
    NIASSERT(bResult);
    
    if (bResult == false)
        return NULL;

    return ReadBody(kIst, pkOptDest);
}
//---------------------------------------------------------------------------
NiPixelData*  NiOpenEXRReader::ReadBody(NiFile &kIst, NiPixelData* pkOptDest)
{
    // At this time, mipmapping is not supported.
    NIASSERT(m_bMipMap == false);

    // if (m_bMipMap)
    //    return ReadMipMapImage(kIst, pkOptDest);
    // else 
        return ReadImage(kIst, pkOptDest);
}
//---------------------------------------------------------------------------
NiPixelData*  NiOpenEXRReader::ReadImage(NiFile &kIst,
    NiPixelData* pkOptDest)
{
    NiFileIStream kStrm(&kIst);

    int iMinX = 0;
    int iMinY = 0;
    int iMaxY = m_uiHeight - 1;

    RgbaInputFile kRGBAFile(kStrm);

    Array2D<Rgba> akPixels;
    akPixels.resizeErase(m_uiHeight, m_uiWidth);

    kRGBAFile.setFrameBuffer(
        &akPixels[0][0] - iMinX - iMinY * m_uiWidth, 1, m_uiWidth);
    kRGBAFile.readPixels(iMinY, iMaxY);
       
    NiPixelData* pkDest = NULL;
    if (pkOptDest && (pkOptDest->GetWidth() == m_uiWidth) && 
        (pkOptDest->GetHeight() == m_uiHeight) &&
        (pkOptDest->GetPixelFormat() == m_kFormat))
    {
         pkDest = pkOptDest;
    }
    else
    {
        pkDest = NiNew NiPixelData(m_uiWidth, m_uiHeight, m_kFormat, 1, 1);
    }

    unsigned short* pusDest = 
        (unsigned short*)pkDest->GetPixels(0,0);
    
    for (unsigned int y=0; y < m_uiHeight; y++)
    {
        for (unsigned int x=0; x < m_uiWidth; x++)
        {
            half R = akPixels[y][x].r;
            half G = akPixels[y][x].g;
            half B = akPixels[y][x].b;
            half A = akPixels[y][x].a;
    
            *(pusDest++) = R.bits();
            *(pusDest++) = G.bits();
            *(pusDest++) = B.bits();
            *(pusDest++) = A.bits();
        }
    }

    kStrm.seekg(0);
    return pkDest;
}
//---------------------------------------------------------------------------
NiPixelData*  NiOpenEXRReader::ReadMipMapImage(NiFile &kIst,
    NiPixelData* pkOptDest)
{
    NiFileIStream kStrm(&kIst);

    // Mipmap levels can only occur if dealing with Tiled format
    TiledRgbaInputFile kTiledFile(kStrm);

    // Allocate pixel data to accomodate all mipmap levels
    int iNumLevels = kTiledFile.numLevels();
    
    NiPixelData* pkDest = NULL;
    if (pkOptDest && (pkOptDest->GetWidth() == m_uiWidth) && 
        (pkOptDest->GetHeight() == m_uiHeight) &&
        (pkOptDest->GetPixelFormat() == m_kFormat))
    {
         pkDest = pkOptDest;
    }
    else
    {
        pkDest = 
            NiNew NiPixelData(m_uiWidth, m_uiHeight, m_kFormat, iNumLevels, 1);
    }

    Array2D<Rgba> akPixels(m_uiHeight, m_uiWidth);

    unsigned int uiTileWidth = kTiledFile.tileXSize();
    // TEST unsigned int uiTileHeight = kTiledFile.tileYSize();
    // TEST Array2D<Rgba> akTilePixels(uiTileHeight, uiTileWidth);
    
    for (int iLevel = 0; iLevel < iNumLevels; ++iLevel)
    {
        int iLevelWidth = kTiledFile.levelWidth(iLevel);
        int iLevelHeight = kTiledFile.levelHeight(iLevel);

        int iMaxYTiles = kTiledFile.numYTiles(iLevel);
        int iMaxXTiles = kTiledFile.numXTiles(iLevel);

        for (int iTileY = 0; iTileY < iMaxYTiles; ++iTileY)
        {
            for (int iTileX = 0; iTileX < iMaxXTiles; ++iTileX)
            {
                Box2i kRange = kTiledFile.dataWindowForTile(iTileX,
                    iTileY, iLevel);

                Rgba* pkBase = &akPixels[-kRange.min.y][-kRange.min.x];
                kTiledFile.setFrameBuffer(pkBase, 1, uiTileWidth);
                kTiledFile.readTile(iTileX, iTileY, iLevel);                  
                
                // TEST Rgba* pkBase = &akTilePixels[0][0];
                // TEST kTiledFile.setFrameBuffer(pkBase, 1, uiTileWidth);
                // TEST kTiledFile.readTile(1, 0, 0);                  
            }
        }

        // The complete level has been loaded and stored in akPixels
        // Now we assign it to our pixel data for that level.      
        unsigned short* pusDest = 
            (unsigned short*)pkDest->GetPixels(iLevel,0);

        unsigned int uiSize = 
            (NiUInt32)pkDest->GetSizeInBytes(iLevel, 0);
        NI_UNUSED_ARG(uiSize);
        NIASSERT(uiSize == (4 * sizeof(short) * iLevelHeight * iLevelWidth));

        for (int y=0; y < iLevelHeight; y++)
        {
            for (int x=0; x < iLevelWidth; x++)
            {
                half R = akPixels[y][x].r;
                half G = akPixels[y][x].g;
                half B = akPixels[y][x].b;
                half A = akPixels[y][x].a;
        
                *(pusDest++) = R.bits();
                *(pusDest++) = G.bits();
                *(pusDest++) = B.bits();
                *(pusDest++) = A.bits();
            }
        }
    }

    kStrm.seekg(0);
    return pkDest;
}
//---------------------------------------------------------------------------
