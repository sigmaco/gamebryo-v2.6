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

#include "NiMath.h"
#include "NiPoint3.h"
 
//---------------------------------------------------------------------------
float NiFastATan2(float fY, float fX)
{
    // Poly approximation valid for |z| <= 1.  To compute ATAN(z)
    // for z > 1, use ATAN(z) = PI/2 - ATAN(1/z).  For z < -1, use
    // ATAN(z) = -PI/2 - ATAN(1/z).

    if (fX == 0.0f && fY == 0.0f)
        return 0.0f;

    float fOffset = 0.0f;
    float fZ;
    if (NiAbs(fY) > NiAbs(fX))
    {
        //  |fY/fX| > 1 so use 1/z identities.
        fZ = fX / fY;
        if (fZ > 0.0f)
        {
            fOffset = NI_HALF_PI;
        }
        else if (fZ < 0.0f)
        {
            fOffset = -NI_HALF_PI;
        }
        else // fZ == 0.0f
        {
            // special case for 90deg and -90deg
            return (fY > 0.0f) ? NI_HALF_PI : -NI_HALF_PI;
        }
    }
    else
    {
        fZ = fY / fX;

        if (fZ == 0.0f)
        {
            // special case for 0deg and 180deg
            return (fX > 0.0f) ? 0.0f : NI_PI;
        }
    }

    float fZ2 = fZ * fZ;

    // Polynomial approximation of degree 9, P(z).
    // |ATAN(z)-P(z)| <= 1e-05

    float fResult;
    fResult = 0.0208351f;
    fResult *= fZ2;
    fResult -= 0.0851330f;
    fResult *= fZ2;
    fResult += 0.1801410f;
    fResult *= fZ2;
    fResult -= 0.3302995f;
    fResult *= fZ2;
    fResult += 0.9998660f;
    fResult *= fZ;

    if (fOffset)
        fResult = fOffset - fResult;

    // find proper solution to two arg arctan
    if (fY < 0.0f && fX < 0.0f)  // quadrant IV
        fResult -= NI_PI;
    if (fY > 0.0f && fX < 0.0f)  // quadrant II
        fResult += NI_PI;

    // NIASSERT(NiAbs(fResult - NiATan2(fY, fX)) <= 1e-05);

    return fResult;
}
//---------------------------------------------------------------------------
