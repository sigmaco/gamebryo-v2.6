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
#include "NiPoint3.h"

const NiPoint3 NiPoint3::ZERO(0.0f,0.0f,0.0f);
const NiPoint3 NiPoint3::UNIT_X(1.0f,0.0f,0.0f);
const NiPoint3 NiPoint3::UNIT_Y(0.0f,1.0f,0.0f);
const NiPoint3 NiPoint3::UNIT_Z(0.0f,0.0f,1.0f);
const NiPoint3 NiPoint3::UNIT_ALL(1.0f,1.0f,1.0f);

#ifndef __SPU__

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiPoint3::LoadBinary(NiStream& stream)
{
    NiStreamLoadBinary(stream,x);
    NiStreamLoadBinary(stream,y);
    NiStreamLoadBinary(stream,z);
}
//---------------------------------------------------------------------------
void NiPoint3::SaveBinary(NiStream& stream) const
{
    NiStreamSaveBinary(stream,x);
    NiStreamSaveBinary(stream,y);
    NiStreamSaveBinary(stream,z);
}
//---------------------------------------------------------------------------
char* NiPoint3::GetViewerString(const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 64;
    char* pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "%s = (%g,%g,%g)", pPrefix, x, y, z);
    return pString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// This algorithm for fast square roots was published as "A High Speed, Low
// Precision Square Root", by Paul Lalonde and Robert Dawon, Dalhousie
// University, Halifax, Nova Scotia, Canada, on pp. 424-6 of "Graphics Gems",
// edited by Andrew Glassner, Academic Press, 1990.

// These results are generally faster than their full-precision counterparts
// (except on modern PC hardware), but are only worth 7 bits of binary
// precision (1 in 128).
unsigned int* NiPoint3::InitSqrtTable()
{
    union FloatIntUnion
    {
        unsigned int ui;
        float f;
    };

    FloatIntUnion kRep;

    // A table of square roots with 7-bit precision requires 256 entries.
    unsigned int* pSqrtTable = NiAlloc(unsigned int, 256);

    for (unsigned int i = 0; i < 128; i++)
    {
        // Build a float with the bit pattern i as mantissa and 0 as exponent.
        kRep.ui = (i<<16) | (127<<23);
        kRep.f = NiSqrt(kRep.f);

        // Store the first 7 bits of the mantissa in the table.
        pSqrtTable[i] = (kRep.ui & 0x7fffff);

        // Build a float with the bit pattern i as mantissa and 1 as exponent.
        kRep.ui = (i << 16) | (128 << 23);
        kRep.f = NiSqrt(kRep.f);

        // Store the first 7 bits of the mantissa in the table.
        pSqrtTable[i+128] = (kRep.ui & 0x7fffff);
    }

    return pSqrtTable;
}
//---------------------------------------------------------------------------
#endif
