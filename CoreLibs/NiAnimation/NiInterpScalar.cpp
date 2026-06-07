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
#include "NiAnimationPCH.h"

#include "NiInterpScalar.h"

//---------------------------------------------------------------------------
float NiInterpScalar::Linear(float fTime, float fP, float fQ)
{
    return fP*(1-fTime) + fQ*fTime;
}
//---------------------------------------------------------------------------
float NiInterpScalar::Bezier(float fTime, float fP, float fDP, float fQ,
                              float fDQ)
{
    // A = 3(X1 - X0) - (T1 + 2T0)
    float A = 3*(fQ-fP)-(fDQ+2*fDP);

    // B = (T0 + T1) - 2(X1 - X0)
    float B = fDP+fDQ-2*(fQ-fP);

    return fP+(fDP+(A+B*fTime)*fTime)*fTime;
}
//---------------------------------------------------------------------------
float NiInterpScalar::TCB(float fTime, float fP, float fPDD, float fQ,
                           float fQDS)
{
    // A = 3(P1 - P0) - (D1 + 2D0)
    float A = 3*(fQ-fP)-(fQDS+2*fPDD);

    // B = (D0 + D1) - 2(P1 - P0)
    float B = fPDD+fQDS-2*(fQ-fP);

    return fP+(fPDD+(A+B*fTime)*fTime)*fTime;
}
//---------------------------------------------------------------------------
void NiInterpScalar::AdjustBezier(float fLastValue, float fLastTime, 
    float& fLastOut, float fNextValue, float fNextTime, float& fNextIn, 
    float fNewTime, float, float& fNewIn, float& fNewOut)
{
    // Find the coefficients of a cubic polynomial that fits the given 
    // values and slopes.

    float fOldDeltaX = fNextValue - fLastValue;
    float fOldDeltaT = fNextTime - fLastTime;
    float fNewDeltaTA = fNewTime - fLastTime;
    float fNewDeltaTB = fNextTime - fNewTime;

    // calculate normalized time
    float t = fNewDeltaTA / fOldDeltaT;
    
    float a = -2.0f * fOldDeltaX + fLastOut + fNextIn;
    float b = 3.0f * fOldDeltaX - 2.0f * fLastOut - fNextIn;

    // calculate tangent
    fNewIn = (((3.0f * a * t + 2.0f * b) * t + fLastOut) / fOldDeltaT);

    // normalize in and out
    fNewOut = fNewIn * fNewDeltaTB;
    fNewIn *= fNewDeltaTA;

    // renormalize last and next tangents

    fLastOut *= fNewDeltaTA / fOldDeltaT;
    fNextIn *= fNewDeltaTB / fOldDeltaT;
}
//---------------------------------------------------------------------------

