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

//---------------------------------------------------------------------------
inline float NiAbs (float fValue)
{
    return float(fabs(fValue));
}
//---------------------------------------------------------------------------
inline float NiACos (float fValue)
{
    if ( -1.0f < fValue )
    {
        if ( fValue < 1.0f )
            return float(acos(fValue));
        else
            return 0.0f;
    }
    else
    {
        return NI_PI;
    }
}
//---------------------------------------------------------------------------
inline float NiASin (float fValue)
{
    if ( -1.0f < fValue )
    {
        if ( fValue < 1.0f )
            return float(asin(fValue));
        else
            return NI_HALF_PI;
    }
    else
    {
        return -NI_HALF_PI;
    }
}
//---------------------------------------------------------------------------
inline float NiATan (float fValue)
{
    return float(atan(fValue));
}
//---------------------------------------------------------------------------
inline float NiATan2 (float fNumerator, float fDenominator)
{
    return float(atan2(fNumerator, fDenominator));
}
//---------------------------------------------------------------------------
inline float NiCos (float fRadians)
{
    return float(cos(fRadians));
}
//---------------------------------------------------------------------------
inline float NiExp (float fValue)
{
    return float(exp(fValue));
}
//---------------------------------------------------------------------------
inline float NiFloor (float fValue)
{
    return float(floor(fValue));
}
//---------------------------------------------------------------------------
inline float NiFmod (float fNumerator, float fDenominator)
{
    return float(fmod(fNumerator, fDenominator));
}
//---------------------------------------------------------------------------
inline float NiLog (float fValue)
{
    return float(log(fValue));
}
//---------------------------------------------------------------------------
inline float NiPow (float fBase, float fExponent)
{
    return float(pow(fBase, fExponent));
}
//---------------------------------------------------------------------------
inline float NiSign (float fValue)
{
    return ( fValue > 0.0f ? +1.0f : ( fValue < 0.0f ? -1.0f : 0.0f ) );
}
//---------------------------------------------------------------------------
inline float NiSin (float fRadians)
{
    return float(sin(fRadians));
}
//---------------------------------------------------------------------------
inline void NiSinCos (float fRadians, float& fSin, float& fCos)
{
    float s, c;
    __asm fld fRadians
    __asm fsincos
    __asm fstp c
    __asm fstp s

    fSin = s;
    fCos = c;
}
//---------------------------------------------------------------------------
inline float NiSqr (float fValue)
{
    return fValue*fValue;
}
//---------------------------------------------------------------------------
inline float NiSqrt (float fValue)
{
    return float(sqrt(fValue));
}
//---------------------------------------------------------------------------
inline float NiMin(float fA, float fB)
{
    return (fA < fB) ? fA : fB;
}
//---------------------------------------------------------------------------
inline float NiMax(float fA, float fB)
{
    return (fA > fB) ? fA : fB;
}
//---------------------------------------------------------------------------
inline float NiClamp(float fValue, float fMin, float fMax)
{
    NIASSERT(fMin <= fMax);
    fValue = NiMax(fValue, fMin);
    fValue = NiMin(fValue, fMax);
    return fValue;
}
//---------------------------------------------------------------------------
inline int NiMin(int iA, int iB)
{
    return (iA < iB) ? iA : iB;
}
//---------------------------------------------------------------------------
inline int NiMax(int iA, int iB)
{
    return (iA > iB) ? iA : iB;
}
//---------------------------------------------------------------------------
inline int NiClamp(int iValue, int iMin, int iMax)
{
    NIASSERT(iMin <= iMax);
    iValue = NiMax(iValue, iMin);
    iValue = NiMin(iValue, iMax);
    return iValue;
}
//---------------------------------------------------------------------------
inline bool NiIsPowerOf2(unsigned int uiI)
{
    if (!uiI)
        return false;

    return (uiI & (uiI - 1)) == 0;
}
//---------------------------------------------------------------------------
inline void NiSrand (unsigned int uiSeed)
{
    srand(uiSeed);
}
//---------------------------------------------------------------------------
inline int NiRand ()
{
    return rand();
}
//---------------------------------------------------------------------------
inline float NiUnitRandom ()
{
    return float(NiRand())/float(RAND_MAX);
}
//---------------------------------------------------------------------------
inline float NiSymmetricRandom ()
{
    return -1.0f + 2.0f*float(NiRand())/float(RAND_MAX);
}
//---------------------------------------------------------------------------
inline unsigned int NiFastLog (unsigned int uiNum)
{
    unsigned int uiLog = 0;
    unsigned int uiTemp = uiNum >> 1;
    while ( uiTemp & 0xffffffff )
    {
        uiTemp >>= 1;
        uiLog++;
    }

    return uiLog;
}
//---------------------------------------------------------------------------
// Algorithm for fast inverse square root from Dave Eberly's WildMagic code.
// See http://www.wild-magic.com/Web/Documents/FastInverseSqrt.pdf for
// implementation discussion.
inline float NiFastInvSqrt(float fValue)
{
    float fHalf = 0.5f*fValue;
    int i  = *(int*)&fValue;
    i = 0x5f3759df - (i >> 1);
    fValue = *(float*)&i;
    fValue = fValue*(1.5f - fHalf*fValue*fValue);
    return fValue;
}
//---------------------------------------------------------------------------
inline float NiFastSqrt(float fValue)
{
    return NiFastInvSqrt(fValue) * fValue;
}
//---------------------------------------------------------------------------
