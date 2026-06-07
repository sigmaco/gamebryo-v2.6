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
#include "NiSystemPCH.h"

#include "NiBinaryStream.h"
#include "NiRTLib.h"
#include "NiBinaryLoadSave.h"

bool NiBinaryStream::ms_bEndianMatchHint = false;

//---------------------------------------------------------------------------
NiBinaryStream::NiBinaryStream()
{
    // Set read and write function pointers as explicitly invalid.
    // Derived classes MUST assign before reading/writing.
    m_pfnRead = 0;
    m_pfnWrite = 0;
    m_uiAbsoluteCurrentPos = 0;
}
//---------------------------------------------------------------------------
NiBinaryStream::~NiBinaryStream()
{
}

//---------------------------------------------------------------------------
unsigned int NiBinaryStream::GetLine(char* pBuffer, unsigned int uiMaxBytes)
{
    unsigned int uiBytesRead = 0;
    unsigned int i = 0;

    NIASSERT(uiMaxBytes > 0);

    while (i + 1 < uiMaxBytes)
    {
        char c;
        unsigned int uiRead = Read(&c, 1);
        uiBytesRead += uiRead;

        if (uiRead != 1 || c == '\n')
            break;

        if (c != '\r')
            pBuffer[i++] = c;
    }

    pBuffer[i] = 0;

    return uiBytesRead;
}

//---------------------------------------------------------------------------
unsigned int NiBinaryStream::PutS(const char* pBuffer)
{
    unsigned int i = 0;

    while (*pBuffer != 0)
    {
        if (Write(pBuffer++, 1) == 1)
        {
            i++;
        }
        else
        {
            break;
        }
    }

    return i;
}
//---------------------------------------------------------------------------
bool NiBinaryStream::GetEndianMatchHint()
{
    return ms_bEndianMatchHint;
}
//---------------------------------------------------------------------------
void NiBinaryStream::SetEndianMatchHint(bool bForceMatch)
{
    ms_bEndianMatchHint = bForceMatch;
}
//---------------------------------------------------------------------------
void NiBinaryStream::DoByteSwap(void* pvData, 
    unsigned int uiTotalBytes, unsigned int* puiComponentSizes, 
    unsigned int uiNumComponents)
{
    // short circuit for byte transfer
    if (uiNumComponents == 1 && *puiComponentSizes == 1)
        return;

    char* pDummy = (char*)(pvData);
    unsigned int uiSize = 0;
    while (uiSize < uiTotalBytes)
    {
        for (unsigned int j = 0; j < uiNumComponents; j++)
        {
            unsigned int uiCompSize = puiComponentSizes[j];
            switch (uiCompSize)
            {
            case 2:
                NiEndian::Swap16((char*)pDummy, 1);
                break;
            case 4:
                NiEndian::Swap32((char*)pDummy, 1);
                break;
            case 8:
                NiEndian::Swap64((char*)pDummy, 1);
                break;
            case 1:
                // Don't need to swap a single byte.
                break;
            default:
                NIASSERT(!"Endian swapping invalid size.");
                break;
            }
            pDummy += uiCompSize;
            uiSize += uiCompSize;
        }   
    }
    NIASSERT(uiSize == uiTotalBytes);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryStream::GetPosition() const
{
    return m_uiAbsoluteCurrentPos;
}
//---------------------------------------------------------------------------
void NiBinaryStream::WriteCString(const char* pcString)
{
    size_t uiLength = (pcString ? strlen(pcString) : 0);
    NiStreamSaveBinary(*this, uiLength);

    if (uiLength)
    {
        Write(pcString, (unsigned int)uiLength);
    }
}
//---------------------------------------------------------------------------
char* NiBinaryStream::ReadCString()
{
    char* pcString = NULL;

    unsigned int uiLength;
    NiStreamLoadBinary(*this, uiLength);
    if (uiLength > 0)
    {
        pcString = NiAlloc(char, uiLength + 1);
        NIASSERT(pcString);
        Read(pcString, uiLength);

        pcString[uiLength] = 0;
    }

    return pcString;
}
//---------------------------------------------------------------------------
