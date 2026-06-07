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

#ifndef NIMATH_H
#define NIMATH_H

#include <NiRTLib.h>
#include "NiMainLibType.h"

#ifndef __SPU__
const float NI_INFINITY = FLT_MAX;
const float NI_PI = 3.1415926535897932f;
const float NI_HALF_PI = 0.5f * NI_PI;
const float NI_TWO_PI = 2.0f * NI_PI;
const float NI_E = 2.7182818284590452f;
#else
// SPU compiler does not like global static constant variables
#define NI_INFINITY FLT_MAX
#define NI_PI       3.1415926535897932f
#define NI_HALF_PI  1.5707963267948966f;
#define NI_TWO_PI   6.2831853071795865f;
#define NI_E        2.7182818284590452f;
#endif

// 'float' wrappers for standard math functions
float NiAbs(float fValue);
float NiACos(float fValue);
float NiASin(float fValue);
float NiATan(float fValue);
float NiATan2(float fNumerator, float fDenominator);
float NiCos(float fRadians);
float NiExp(float fValue);
float NiFloor(float fValue);
float NiFmod(float fNumerator, float fDenominator);
float NiLog(float fValue);
float NiPow(float fBase, float fExponent);
float NiSign(float fValue);
float NiSin(float fRadians);
void NiSinCos(float fRadians, float& fSin, float& fCos);
float NiSqr(float fValue);
float NiSqrt(float fValue);

// min max clamp
float NiMin(float fA, float fB);
float NiMax(float fA, float fB);
float NiClamp(float fValue, float fMin, float fMax);
int NiMin(int iA, int iB);
int NiMax(int iA, int iB);
int NiClamp(int iValue, int iMin, int iMax);

// utility functions
bool NiIsPowerOf2(unsigned int uiI);
#define NiLerp(T, START, FINISH) ((START) + ((T) * ((FINISH) - (START))))

// random number generation
void NiSrand(unsigned int uiSeed);
int NiRand();  // r in [0,RAND_MAX]
float NiSymmetricRandom();  // r in [-1,1]
float NiUnitRandom();  // r in [0,1]

// fast functions
unsigned int NiFastLog(unsigned int uiNum);
float NiFastInvSqrt(float fValue);
float NiFastSqrt(float fValue);
NIMAIN_ENTRY float NiFastATan2(float fY, float fX);


#include "NiMath.inl"

#endif // NIMATH_H
