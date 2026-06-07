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

// initialization for fast square roots
unsigned int* NiPoint3::ms_pSqrtTable = 0;
//---------------------------------------------------------------------------
void NiPoint3::_SDMInit()
{
    ms_pSqrtTable = InitSqrtTable();
}
//---------------------------------------------------------------------------
void NiPoint3::_SDMShutdown()
{
    NiFree(ms_pSqrtTable); 
    ms_pSqrtTable = 0;
}
//---------------------------------------------------------------------------
// This algorithm was published as "A High Speed, Low Precision Square Root",
// by Paul Lalonde and Robert Dawon, Dalhousie University, Halifax, Nova
// Scotia, Canada, on pp. 424-6 of "Graphics Gems", edited by Andrew Glassner,
// Academic Press, 1990.

// These results are generally faster than their full-precision counterparts
// (except on modern PC hardware), but are only worth 7 bits of binary
// precision (1 in 128).
// [A table for 7-bit precision requires 256 entries.]
void NiPoint3::UnitizeVectors(NiPoint3* pkV, unsigned int uiNum,
    unsigned int uiStride)
{
    float fLeng;
    short sExp;

    // This pointer allows us to treat the float as its integer bit 
    // representation.
    unsigned int *puiRep = (unsigned int*) &fLeng;

    NIASSERT(uiStride % sizeof(float) == 0);

    // WARNING:  SERIOUS ALIASING going on here.  Be very careful with
    // optimization flags.
    for (unsigned int i = 0; i < uiNum; i++)
    {
        // Compute the squared length normally.
        fLeng = pkV->x * pkV->x + pkV->y * pkV->y + pkV->z * pkV->z;
   
        if (!(*puiRep)) // If the squared length is zero, exit.
        {
            fLeng = 0.0f;
        }
        else
        {
            // Shift and mask the exponent from the float.
            sExp = static_cast<short>(((*puiRep) >> 23) - 127);

            // Mask the exponent away.
            *puiRep &= 0x7fffff;

            // If the exponent is odd, use the upper half of the square root
            // table.
            if (sExp & 0x1)
                *puiRep |= 0x800000;

            // Compute the sqrt'ed exponent (divide by 2).
            sExp >>= 1;

            // Build the new floating point representation by ORing the
            // looked-up mantissa with the computed exponent.
            *puiRep = ms_pSqrtTable[(*puiRep) >> 16] | ((sExp + 127) << 23);

            fLeng = 1.0f / fLeng;   // Invert the length.
        }

        pkV->x *= fLeng;
        pkV->y *= fLeng; 
        pkV->z *= fLeng;

        pkV = reinterpret_cast<NiPoint3*>(
            reinterpret_cast<NiUInt8*>(pkV) + uiStride);

    }
}
//---------------------------------------------------------------------------
void NiPoint3::PointsPlusEqualFloatTimesPoints(NiPoint3* pkDst, float f,
    const NiPoint3* pkSrc, unsigned int uiVerts)
{
    for (unsigned int i = 0; i < uiVerts; i++)
    {
        pkDst[i] += f * pkSrc[i];
    }
}
//---------------------------------------------------------------------------
void NiPoint3::WeightedPointsPlusWeightedPoints(NiPoint3* pkDst,
    float fWeight, const NiPoint3* pkSrc, unsigned int uiVerts)
{
    float fOneMinusWeight = 1.0f - fWeight;
    for (unsigned int ui = 0; ui < uiVerts; ui++)
    {
        pkDst[ui] = fOneMinusWeight * pkSrc[ui] + fWeight * pkDst[ui];
    }
}
//---------------------------------------------------------------------------
