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

#include "NiBlendPoint3Interpolator.h"
#include <NiStream.h>

NiImplementRTTI(NiBlendPoint3Interpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendPoint3Interpolator::NiBlendPoint3Interpolator(bool bManagerControlled,
    float fWeightThreshold, unsigned char ucArraySize) : 
    NiBlendInterpolator(bManagerControlled, fWeightThreshold, ucArraySize),
    m_kPoint3Value(INVALID_POINT3), m_bValidValue(false)
{
}
//---------------------------------------------------------------------------
NiBlendPoint3Interpolator::NiBlendPoint3Interpolator() :
    m_kPoint3Value(INVALID_POINT3), m_bValidValue(false)
{
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::IsPoint3ValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsPoint3ValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, NiPoint3& kValue)
{
    if (!GetSingleUpdateTime(fTime))
        return false;

    NIASSERT(m_pkSingleInterpolator != NULL);
    if (m_pkSingleInterpolator->Update(fTime, pkInterpTarget, m_kPoint3Value))
    {
       kValue = m_kPoint3Value;
       return true;
    }
    else
    {
       kValue = m_kPoint3Value = INVALID_POINT3;
       return false;
    }
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::BlendValues(float fTime, 
    NiObjectNET* pkInterpTarget, NiPoint3& kValue)
{
    m_bValidValue = false;
    m_kPoint3Value = NiPoint3::ZERO;
    float fTotalNormalizedWeight = 1.0f;

    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[uc].m_spInterpolator;
        if (pkInterpolator && m_pkInterpArray[uc].m_fNormalizedWeight > 0.0f)
        {
            NiPoint3 kVal;
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime, m_pkInterpArray[uc]))
            {
                fTotalNormalizedWeight -=  m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                continue;
            }

            bool bSuccess = pkInterpolator->Update(fUpdateTime,
                pkInterpTarget, kVal);
            if (bSuccess)
            {
                m_kPoint3Value += kVal * m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                m_bValidValue = true;
            }
            else
            {
                fTotalNormalizedWeight -=  m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
            }
        }
    }

    if (!m_bValidValue)
    {
        kValue = m_kPoint3Value = INVALID_POINT3;
        return false;
    }

    // If we encountered any Update failures, we 
    // need to correct the final value for those
    // failures.
    m_kPoint3Value /= fTotalNormalizedWeight;
    

    kValue = m_kPoint3Value;
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, NiPoint3& kValue)
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
NiImplementCreateClone(NiBlendPoint3Interpolator);
//---------------------------------------------------------------------------
void NiBlendPoint3Interpolator::CopyMembers(NiBlendPoint3Interpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kPoint3Value = m_kPoint3Value;
    pkDest->m_bValidValue = m_bValidValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendPoint3Interpolator);
//---------------------------------------------------------------------------
void NiBlendPoint3Interpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);

    m_kPoint3Value.LoadBinary(kStream);
    if (m_kPoint3Value != INVALID_POINT3)
    {
        m_bValidValue = true;
    }
}
//---------------------------------------------------------------------------
void NiBlendPoint3Interpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendPoint3Interpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);

    m_kPoint3Value.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendPoint3Interpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendPoint3Interpolator* pkDest = (NiBlendPoint3Interpolator*) pkObject;

    if (m_kPoint3Value != pkDest->m_kPoint3Value)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendPoint3Interpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendPoint3Interpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(m_kPoint3Value.GetViewerString("m_kPoint3Value"));
}
//---------------------------------------------------------------------------
