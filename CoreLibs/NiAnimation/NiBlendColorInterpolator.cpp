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

#include "NiBlendColorInterpolator.h"
#include <NiStream.h>

NiImplementRTTI(NiBlendColorInterpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendColorInterpolator::NiBlendColorInterpolator(bool bManagerControlled,
    float fWeightThreshold, unsigned char ucArraySize) : 
    NiBlendInterpolator(bManagerControlled, fWeightThreshold, ucArraySize),
    m_kColorValue(INVALID_COLORA), m_bValidValue(false)
{
}
//---------------------------------------------------------------------------
NiBlendColorInterpolator::NiBlendColorInterpolator() :
    m_kColorValue(INVALID_COLORA), m_bValidValue(false)
{
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::IsColorAValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsColorAValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, NiColorA& kValue)
{
    if (!GetSingleUpdateTime(fTime))
        return false;

    NIASSERT(m_pkSingleInterpolator != NULL);
    if (m_pkSingleInterpolator->Update(fTime, pkInterpTarget, m_kColorValue))
    {
       kValue = m_kColorValue;
       return true;
    }
    else
    {
       m_kColorValue = kValue = INVALID_COLORA;
       return false;
    }
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::BlendValues(float fTime, 
    NiObjectNET* pkInterpTarget, NiColorA& kValue)
{
    m_bValidValue = false;
    m_kColorValue = NiColorA(0.0f, 0.0f, 0.0f, 0.0f);
    float fTotalNormalizedWeight = 1.0f;

    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[uc].m_spInterpolator;
        if (pkInterpolator && m_pkInterpArray[uc].m_fNormalizedWeight > 0.0f)
        {
            NiColorA kVal;
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime, m_pkInterpArray[uc]))
            {
                fTotalNormalizedWeight -= m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                continue;
            }

            bool bSuccess = pkInterpolator->Update(fUpdateTime,
                pkInterpTarget, kVal);

            if (bSuccess)
            {
                m_kColorValue += kVal * m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                m_bValidValue = true;
            }
            else
            {
                fTotalNormalizedWeight -= m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
            }
        }
    }

    if (!m_bValidValue)
    {
        kValue = m_kColorValue = INVALID_COLORA;
        return false;
    }

    // If we encountered any Update failures, we 
    // need to correct the final value for those
    // failures.
    m_kColorValue /= fTotalNormalizedWeight;

    kValue = m_kColorValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, NiColorA& kValue)
{
    // Do not use the TimeHasChanged check here, because blend interpolators
    // should always update their interpolators.

    bool bReturnValue = false;
    if (m_ucInterpCount == 1)
    {
        bReturnValue = StoreSingleValue(fTime, pkInterpTarget, kValue);
    }
    else if (m_ucInterpCount > 0)
    {
        ComputeNormalizedWeights();
       
        bReturnValue = BlendValues(fTime, pkInterpTarget, kValue);
    }
    
    m_fLastTime = fTime;
    return bReturnValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBlendColorInterpolator);
//---------------------------------------------------------------------------
void NiBlendColorInterpolator::CopyMembers(NiBlendColorInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kColorValue = m_kColorValue;
    pkDest->m_bValidValue = m_bValidValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendColorInterpolator);

//---------------------------------------------------------------------------
void NiBlendColorInterpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);

    m_kColorValue.LoadBinary(kStream);
    if (m_kColorValue != INVALID_COLORA)
    {
        m_bValidValue = true;
    }
}
//---------------------------------------------------------------------------
void NiBlendColorInterpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendColorInterpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);

    m_kColorValue.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendColorInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendColorInterpolator* pkDest = (NiBlendColorInterpolator*) pkObject;

    if (m_kColorValue != pkDest->m_kColorValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendColorInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendColorInterpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(m_kColorValue.GetViewerString("m_kColorValue"));
}
//---------------------------------------------------------------------------
