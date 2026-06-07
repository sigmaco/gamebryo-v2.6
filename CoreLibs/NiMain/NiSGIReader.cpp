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

#include <NiFile.h>
#include <NiSystem.h>
#include "NiSGIReader.h"
#include "NiPixelData.h"

//---------------------------------------------------------------------------
NiSGIReader::NiSGIReader()
{
    m_usWidth = 0;
    m_usHeight = 0;
    m_usDepth = 0;
    m_ucBytesPerComponent = 0;
    m_bRLE = false;
    m_kFormat = NiPixelFormat::RGB24;
}
//---------------------------------------------------------------------------
NiSGIReader::~NiSGIReader()
{
    /* */
}
//---------------------------------------------------------------------------
void NiSGIReader::ProcessRLERowByte(unsigned char* pucOutputImageData, 
    unsigned char* pucInputImageData, int iImageStep)
{
    unsigned short usCol = 0;

    for( ; ; )
    {
        unsigned char ucKey = *pucInputImageData;
        if ((ucKey == 0) || (usCol == m_usWidth))
            break;

        pucInputImageData++;
        
        unsigned char ucCount = ucKey & 0x7f;
        
        if (ucKey & 0x80) // normal case - block of different pixels
        {
            while (ucCount--) 
            {
                *pucOutputImageData = *(pucInputImageData++);
                pucOutputImageData += iImageStep;
            }

            usCol = (unsigned short)(usCol + ucCount);
        } 
        else // "run" - block of equal pixels
        {
            unsigned char ucPixel = *(pucInputImageData++);

            while (ucCount--) 
            {
                *pucOutputImageData = ucPixel;
                pucOutputImageData += iImageStep;
            }

            usCol = (unsigned short)(usCol + ucCount);
        }
    }
}
//---------------------------------------------------------------------------
void NiSGIReader::ProcessRLERowWord(unsigned char* pucOutputImageData, 
    unsigned char* pucInputImageData, int iImageStep)
{
    unsigned short usCol = 0;

    for( ; ; )
    {
        unsigned char ucKey = *pucInputImageData;
        if ((ucKey == 0) || (usCol == m_usWidth))
            break;

        pucInputImageData++;
        
        unsigned char ucCount = ucKey & 0x7f;
        
        if (ucKey & 0x80) // normal case - block of different pixels
        {
            while (ucCount--) 
            {
                *pucOutputImageData = *pucInputImageData;
                pucInputImageData += 2;
                pucOutputImageData += iImageStep;
            }

            usCol = (unsigned short)(usCol + ucCount);
        } 
        else // "run" - block of equal pixels
        {
            unsigned char ucPixel = *pucInputImageData;
            pucInputImageData += 2;

            while (ucCount--) 
            {
                *pucOutputImageData = ucPixel;
                pucOutputImageData += iImageStep;
            }

            usCol = (unsigned short)(usCol + ucCount);
        }
    }
}
//---------------------------------------------------------------------------
bool NiSGIReader::ReadHeader(NiFile& kIst, unsigned int& uiWidth, 
    unsigned int& uiHeight, NiPixelFormat& kFormat, bool& bMipmap, 
    unsigned int& uiFaces)
{
    // Set SGI file as big endian
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kIst.SetEndianSwap(bPlatformLittle);

    uiFaces = 1;

    unsigned short usTemp;
    unsigned int uiTemp;

    NiStreamLoadBinary(kIst, usTemp);
    if (SGI_MAGIC != usTemp) 
        return false;

    unsigned char ucStorage; // Storage format
    NiStreamLoadBinary(kIst, ucStorage);

    // Since the reader is usually owned by a static image converter class,
    // we lock for safety.  We wait as long as possible since locals will
    // be in thread context.
    m_kReadCriticalSection.Lock();
    NiStreamLoadBinary(kIst, m_ucBytesPerComponent);
    
    NiStreamLoadBinary(kIst, usTemp); // Dimension (UNUSED)

    NiStreamLoadBinary(kIst, m_usWidth);
    NiStreamLoadBinary(kIst, m_usHeight);
    NiStreamLoadBinary(kIst, m_usDepth);
    
    NiStreamLoadBinary(kIst, uiTemp); // Min pixel (UNUSED)
    NiStreamLoadBinary(kIst, uiTemp); // Max pixel (UNUSED)

    // skip file name and dummy field   
    kIst.Seek(4 + 80, NiFile::ms_iSeekCur); 
    
    NiStreamLoadBinary(kIst, uiTemp); // ColorMap (UNUSED)

    // Move to end of header (512 bytes from start of file)
    kIst.Seek(404, NiFile::ms_iSeekCur); 

    if (ucStorage == SGI_RLE)
    {
        m_bRLE = true;        
    }
    else if (ucStorage == SGI_VERBATIM)
    {
        m_bRLE = false;        
    }
    else
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    if(m_usDepth > 4)
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    if((m_ucBytesPerComponent > 2) || !m_ucBytesPerComponent)
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    m_kFormat = (m_usDepth == 4 || m_usDepth == 2) 
        ? NiPixelFormat::RGBA32 : NiPixelFormat::RGB24;

    uiWidth = m_usWidth;
    uiHeight = m_usHeight;
    kFormat = m_kFormat;
    bMipmap = false;

    m_kReadCriticalSection.Unlock();
    return true;
}
//---------------------------------------------------------------------------
bool NiSGIReader::CanReadFile(const char* pcFileExtension) const
{
    if(!NiStricmp(pcFileExtension, ".sgi") 
        || !NiStricmp(pcFileExtension, ".rgb")
        || !NiStricmp(pcFileExtension, ".rgba")
        || !NiStricmp(pcFileExtension, ".int")
        || !NiStricmp(pcFileExtension, ".inta"))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiSGIReader::ReadFile(NiFile &kIst, NiPixelData* pkOptDest)
{
    bool bMipmap;
    unsigned int uiW, uiH;
    NiPixelFormat kFmt;
    unsigned int uiNumFaces;

    // Since the reader is usually owned by a static image converter class,
    // we lock for safety.  We wait as long as possible since locals will
    // be in thread context.
    m_kReadCriticalSection.Lock();
    if(!ReadHeader(kIst, uiW, uiH, kFmt, bMipmap, uiNumFaces))
    {
        m_kReadCriticalSection.Unlock();
        return NULL;
    }

    NiPixelData* pkDest;

    if(pkOptDest && (pkOptDest->GetWidth() == m_usWidth) && 
        (pkOptDest->GetHeight() == m_usHeight) &&
        (pkOptDest->GetPixelFormat() == m_kFormat))
    {
        pkDest = pkOptDest;
    }
    else
    {
        pkDest = NiNew NiPixelData(m_usWidth, m_usHeight, m_kFormat);
    }

    unsigned int uiDataSize;
    unsigned long* pulSGIStartTable = NULL;

    if (m_bRLE) 
    {
        unsigned int i;
        
        unsigned int uiTableSize = m_usHeight * m_usDepth;

        pulSGIStartTable = NiAlloc(unsigned long, uiTableSize);
        
        if (pulSGIStartTable == NULL)
        {
            m_kReadCriticalSection.Unlock();
            return false;
        }

        NiStreamLoadBinary(kIst, pulSGIStartTable, uiTableSize);

        // compute the remaining file length using the table
        uiDataSize = 0;
        for (i = 0; i < uiTableSize; i++) 
        {
            unsigned int uiTemp;
            NiStreamLoadBinary(kIst, uiTemp);
            uiDataSize += uiTemp;
        }
    }
    else 
    {
        uiDataSize = m_usWidth * m_usHeight 
            * m_usDepth * m_ucBytesPerComponent;
    }
    
    unsigned char* pucSGIImageData = NiAlloc(unsigned char, uiDataSize);
    if (pucSGIImageData == NULL)
    {
        m_kReadCriticalSection.Unlock();
        return NULL;
    }

    // read the entire pixel data into a block
    NiStreamLoadBinary(kIst, pucSGIImageData, uiDataSize);

    unsigned int uiDestPixelStride = pkDest->GetPixelStride();

    unsigned int uiDestRowStride = pkDest->GetWidth() * uiDestPixelStride;
    
    unsigned char* pucDestImageData = pkDest->GetPixels(0);

    if (m_bRLE)
    {
        int iDataOffest 
            = 512 + m_usHeight * m_usDepth * sizeof(unsigned int) * 2;

        if (m_ucBytesPerComponent == 1)
        {
            for (unsigned int i = 0; i < m_usDepth; i++) 
            {
                unsigned char *pucOut = pucDestImageData 
                    + uiDestRowStride*(m_usHeight - 1) + i;

                for (unsigned int j = 0; j < m_usHeight; j++) 
                {
                    int iIndex = pulSGIStartTable[j + i * m_usHeight] 
                        - iDataOffest;

                    ProcessRLERowByte(pucOut, pucSGIImageData + iIndex, 
                        uiDestPixelStride);

                    pucOut -= uiDestRowStride;
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < m_usDepth; i++) 
            {
                unsigned char *pucOut = pucDestImageData 
                    + uiDestRowStride*(m_usHeight - 1) + i;

                for (unsigned int j = 0; j < m_usHeight; j++) 
                {
                    int iIndex = pulSGIStartTable[j + i * m_usHeight] 
                        - iDataOffest;

                    ProcessRLERowWord(pucOut, pucSGIImageData + iIndex, 
                        uiDestPixelStride);

                    pucOut -= uiDestRowStride;
                }
            }
        }
    }
    else
    {
        unsigned char* pucSGIImagePtr = pucSGIImageData;

        if(m_ucBytesPerComponent == 1)
        {
            for (unsigned int i = 0; i < m_usDepth; i++) 
            {
                unsigned char *pucOut = pucDestImageData 
                    + uiDestRowStride*(m_usHeight - 1) + i;

                for (unsigned int j = 0; j < m_usHeight; j++) 
                {
                    for (unsigned short k = 0; k < m_usWidth; k++) 
                    {
                        *pucOut = *(pucSGIImagePtr++);
                        pucOut += uiDestPixelStride;
                    }

                    // skip back over scanline just written and the one before
                    pucOut -= 2*uiDestRowStride;
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < m_usDepth; i++) 
            {
                unsigned char *pucOut = pucDestImageData 
                    + uiDestRowStride*(m_usHeight - 1) + i;

                for (unsigned int j = 0; j < m_usHeight; j++) 
                {
                    for (unsigned short k = 0; k < m_usWidth; k++) 
                    {
                        *pucOut = *pucSGIImagePtr;
                        pucSGIImagePtr += 2;
                        pucOut += uiDestPixelStride;
                    }

                    // skip back over scanline just written and the one before
                    pucOut -= 2*uiDestRowStride;
                }
            }
        }
    }

    NiFree(pucSGIImageData);
    NiFree(pulSGIStartTable);

    // If the image is gray or gray+alpha, we must reformat the initially-
    // read pixels into the correct format
    if (m_usDepth == 1)
    {
        unsigned int uiPixels = m_usWidth*m_usHeight;
        unsigned char* pucData = pkDest->GetPixels(0);

        for(unsigned int i = 0; i < uiPixels; i++)
        {
            // replicate gray
            *(pucData+1) = *(pucData+2) = *pucData;
            pucData += 3;
        }
    }
    else if (m_usDepth == 2)
    {
        unsigned int uiPixels = m_usWidth*m_usHeight;
        unsigned char* pucData = pkDest->GetPixels(0);

        for(unsigned int i = 0; i < uiPixels; i++)
        {
            // move alpha
            *(pucData+3) = *(pucData+1);

            // replicate gray
            *(pucData+1) = *(pucData+2) = *pucData;
            pucData += 4;
        }
    }

    m_kReadCriticalSection.Unlock();
    return pkDest;
}
//---------------------------------------------------------------------------
