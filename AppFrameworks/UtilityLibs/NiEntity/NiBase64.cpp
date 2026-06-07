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
#include "NiEntityPCH.h"

#include "NiBase64.h"

#define FIRST_6BITS 0x3F      // 11 1111
#define SECOND_6BITS 0xFC0    // 0x3F << 6
#define THIRD_6BITS 0x3F000   // 0x3F << 12
#define FOURTH_6BITS 0xFC0000 // 0x3F << 18
#define THREE_BYTE_READS 3

// In the following table, 6 bits of binary data are used as an index,
// giving a valid range of 0-63 (or 0x00 - 0x3F) for the indices. Each
// 6 bit value will map to an ASCII character in the ranges of 
// [A-Z], [a-z], [0-9], [ + ], [ / ]
const unsigned char NiBase64::ms_aucBase64Char[65] = 
{
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
};

// In the following table, the unsigned characters of the Base64
// ASCII representation are used to index in the table. Ie,
// ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
// are valid keys. This table maps the Base64 character to the 6 bit 
// binary value that it is associated with.
const unsigned char NiBase64::ms_aucBase64ToBit6[128] = 
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 
    0x3E,                                                       // [ + ]
    0xFF, 0xFF, 0xFF,
    0x3F,                                                       // [ / ]
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, // [0-9]
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // [A-J]
    0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, // [K-T] 
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19,                         // [U-Z]
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, // [a-j]
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, // [k-t]
    0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,                         // [u-z] 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

//---------------------------------------------------------------------------
char* NiBase64::EncodeToBase64(const void* pvSrcBinaryBuffer,
    int iSrcSize, int iMaxSizeToRead)
{
    // pucSrcBinaryBuffer points to the raw binary
    // iSrcSize is the size of the data to read
    // iMaxSizeToRead is the maximum amount that will be read...
    // (iSrcSize can be greater than MaxSizeToRead, but won't be read
    const unsigned char* pucSrcBinaryBuffer = 
        (const unsigned char*)pvSrcBinaryBuffer;

    if (iMaxSizeToRead == 0)
    {
        // The default is to just read in the entire buffer in a single call
        // So we make iMaxSizeToRead equal iSrcSize
        iMaxSizeToRead = iSrcSize;
    }

    if (iSrcSize < iMaxSizeToRead)
        iMaxSizeToRead = iSrcSize;

    NIASSERT(iSrcSize > 0);

    // For every 3 bytes, we'll need 4 bytes for the destination
    int iAllocationNeeded = ((iMaxSizeToRead * 4) / 3) + 3;
    unsigned char* pucDestBase64Ascii = new unsigned char[iAllocationNeeded];

    unsigned char* pucCurrPosInDest = pucDestBase64Ascii + 3;
    NiUInt32 ui32Bits = 0;
    
    // Every 3 bytes will convert to 4 Base64 characters
    int iSetsOf3Bytes = iMaxSizeToRead / THREE_BYTE_READS; 

    for (int i=0; i<iSetsOf3Bytes; i++)
    {
        // Load our double word with the first 24 bits or 4 characters
        // in our 6 bit system.
        ui32Bits = *pucSrcBinaryBuffer++; // Load 1st byte
        ui32Bits = (ui32Bits<<8) | (*pucSrcBinaryBuffer++); // Load 2nd byte
        ui32Bits = (ui32Bits<<8) | (*pucSrcBinaryBuffer++); // Load 3rd byte

        *pucCurrPosInDest-- = ms_aucBase64Char[ui32Bits & FIRST_6BITS];
        *pucCurrPosInDest-- = ms_aucBase64Char[(ui32Bits & SECOND_6BITS)>>6];
        *pucCurrPosInDest-- = ms_aucBase64Char[(ui32Bits & THIRD_6BITS)>>12];
        *pucCurrPosInDest-- = ms_aucBase64Char[(ui32Bits & FOURTH_6BITS)>>18];

        // Move up 5 to go to next empty spot and add another 3 for the
        // next round.
        pucCurrPosInDest += 8;
    }

    switch (iMaxSizeToRead % THREE_BYTE_READS)
    {
        case 0:
            pucCurrPosInDest -= 3;
            break;
        case 1: // 1 byte left to read => 2 base 64 characters
            pucCurrPosInDest -= 2;
            ui32Bits = *pucSrcBinaryBuffer; // Load 1st byte
            *pucCurrPosInDest-- = ms_aucBase64Char[ui32Bits & FIRST_6BITS];
            *pucCurrPosInDest-- = 
                ms_aucBase64Char[(ui32Bits & SECOND_6BITS)>>6];
            pucCurrPosInDest += 3;
            break;
        case 2: // 2 bytes left to read => 3 base 64 characters
            pucCurrPosInDest -= 1;
            ui32Bits = *pucSrcBinaryBuffer++; // Load 1st byte
            ui32Bits = (ui32Bits<<8) | (*pucSrcBinaryBuffer++); // Load 2nd

            *pucCurrPosInDest-- = ms_aucBase64Char[ui32Bits & FIRST_6BITS];
            *pucCurrPosInDest-- = 
                ms_aucBase64Char[(ui32Bits & SECOND_6BITS)>>6];
            *pucCurrPosInDest-- = 
                ms_aucBase64Char[(ui32Bits & THIRD_6BITS)>>12];
            pucCurrPosInDest += 4;
            break;
    }

    *(pucCurrPosInDest) = 0;

    return reinterpret_cast<char*>(pucDestBase64Ascii);
}
//---------------------------------------------------------------------------
NiBool NiBase64::DecodeFromBase64(void* pvDestBinaryBuffer, 
    const char* pcSrcBase64Ascii, size_t& stBytesWritten)
{
    unsigned char* pucDestBinaryBuffer = (unsigned char*)pvDestBinaryBuffer;

    stBytesWritten = 0;
    // Start on 3rd byte
    unsigned char* pucCurrPosInDest = pucDestBinaryBuffer + 2; 
    NiUInt32 ui32Bits = 0;

    // Reading 4 bytes at a time (which will convert to 3 bytes of binary)
    size_t szStrLen = strlen(pcSrcBase64Ascii);

    size_t stSetsOf4Base64Bytes = szStrLen >> 2; 
    
    for(int i=0; i<(int)stSetsOf4Base64Bytes; i++)
    {   
        // Load 4 bytes of data into ui32Bits
        if (LoadFromBase64(ui32Bits, pcSrcBase64Ascii, 4) == false)
        {
            *pucDestBinaryBuffer = 0;
            return false;
        }
        pcSrcBase64Ascii += 4;

        *pucCurrPosInDest-- = (char)ui32Bits;
        *pucCurrPosInDest-- = (char)(ui32Bits >> 8);
        *pucCurrPosInDest-- = (char)(ui32Bits >> 16);

        stBytesWritten +=3;

        // Move up 4 to go to next empty spot and add another 2 for the
        // next round.
        pucCurrPosInDest += 6;
    }

    int iLeftOverBytes = (int)szStrLen % 4;

    if (!iLeftOverBytes)
    {
        return true;
    }

    // if iBytes is one, then at most it can represent 6 bits
    // of data... this value is therefor impossible.
    NIASSERT(iLeftOverBytes != 1);

    pucCurrPosInDest -= (4 - iLeftOverBytes); 

    if (LoadFromBase64(ui32Bits, pcSrcBase64Ascii, iLeftOverBytes) == false)
    {
        *pucDestBinaryBuffer = 0;
        return false;
    }

    if (iLeftOverBytes == 2)
    {
        *pucCurrPosInDest-- = (char)ui32Bits;
        stBytesWritten += 1;
    }
    else
    {
        NIASSERT(iLeftOverBytes == 3);
        *pucCurrPosInDest-- = (char)ui32Bits;
        *pucCurrPosInDest-- = (char)(ui32Bits >> 8);
        stBytesWritten += 2;
    }

    return true;
}
//---------------------------------------------------------------------------
