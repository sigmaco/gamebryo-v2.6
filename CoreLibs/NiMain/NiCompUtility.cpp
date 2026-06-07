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

#include <NiRTLib.h>
#include "NiCompUtility.h"
#include "NiMath.h"

//---------------------------------------------------------------------------
NiCompUtility::NiCompArray NiCompUtility::CompressFloatArray(
    const float* pfSrcArray, unsigned int uiSrcCount)
{
    float fMin, fMax;
    GetMinMax(pfSrcArray, uiSrcCount, fMin, fMax);
    float fOffset = (fMin + fMax) * 0.5f;
    float fHalfRange = (fMax - fMin) * 0.5f;
    NIASSERT(fMax > fMin);

    NiCompArray kArray;
    kArray.m_asArray = NiAlloc(short, uiSrcCount);
    kArray.m_fOffset = fOffset;
    kArray.m_fHalfRange = fHalfRange;
    kArray.m_uiNumValues = uiSrcCount;

    float fShortMax = (float)SHRT_MAX;
    for (unsigned int ui = 0; ui < uiSrcCount; ui++)
    {
        float fValue = ((pfSrcArray[ui] - fOffset) / 
            (fHalfRange) * (fShortMax));
        if (fValue < -fShortMax)
            fValue = -fShortMax;
        if (fValue > fShortMax)
            fValue = fShortMax;
        kArray.m_asArray[ui] = (short) fValue;
    }

    return kArray;
}
//---------------------------------------------------------------------------
void NiCompUtility::DecompressFloatArray(const short* psSrcArray, 
    unsigned int uiSrcCount,
    float fOffset, float fHalfRange, 
    float* pfDestArray, unsigned int uiDestCount)
{
    NI_UNUSED_ARG(uiSrcCount);
    NIASSERT(uiSrcCount >= uiDestCount);
    for (unsigned int ui = 0; ui < uiDestCount; ui++)
    {
        pfDestArray[ui] = ((((float)psSrcArray[ui]) / ((float)SHRT_MAX)) *
            fHalfRange) + fOffset;
    }
}
//---------------------------------------------------------------------------
void NiCompUtility::GetMinMax(const float* pfArray, unsigned int uiCount, 
    float& fMin, float& fMax)
{
    fMin = NI_INFINITY;
    fMax = -NI_INFINITY;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        if (pfArray[ui] < fMin)
            fMin = pfArray[ui];
        if (pfArray[ui] > fMax)
            fMax = pfArray[ui];
    }

    NIASSERT(fMin < fMax);
}
//---------------------------------------------------------------------------
