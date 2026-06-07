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

#include "NiBlendFloatInterpolator.h"
#include <NiStream.h>

NiImplementRTTI(NiBlendFloatInterpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendFloatInterpolator::NiBlendFloatInterpolator(bool bManagerControlled,
    float fWeightThreshold, unsigned char ucArraySize) : 
    NiBlendInterpolator(bManagerControlled, fWeightThreshold, ucArraySize),
    m_fFloatValue(INVALID_FLOAT)
{
}
//---------------------------------------------------------------------------
NiBlendFloatInterpolator::NiBlendFloatInterpolator() :
    m_fFloatValue(INVALID_FLOAT)
{
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::IsFloatValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsFloatValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, float& fValue)
{
    if (!GetSingleUpdateTime(fTime))
        return false;

    NIASSERT(m_pkSingleInterpolator != NULL);
    if (m_pkSingleInterpolator->Update(fTime, pkInterpTarget, m_fFloatValue))
    {
       fValue = m_fFloatValue;
       return true;
    }
    else
    {
       fValue = m_fFloatValue = INVALID_FLOAT;
       return false;
    }
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::BlendValues(float fTime, 
    NiObjectNET* pkInterpTarget, float& fValue)
{
    bool bModifiedValue = false;
    m_fFloatValue = 0.0f;
    float fTotalNormalizedWeight = 1.0f;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[uc].m_spInterpolator;
        if (pkInterpolator && m_pkInterpArray[uc].m_fNormalizedWeight > 0.0f)
        {
            float fVal;
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime, m_pkInterpArray[uc]))
            {
                fTotalNormalizedWeight -= m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                continue;
            }

            bool bSuccess = pkInterpolator->Update(fUpdateTime,
                pkInterpTarget, fVal);

            if (bSuccess)
            {
                m_fFloatValue += fVal * m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                bModifiedValue = true;
            }
            else
            {
                fTotalNormalizedWeight -= m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
            }
        }
    }

    // If we encountered any Update failures, we 
    // need to correct the final value for those
    // failures.
    m_fFloatValue /= fTotalNormalizedWeight;

    if (!bModifiedValue)
    {
        m_fFloatValue = fValue = INVALID_FLOAT;
        return false;
    }

    fValue = m_fFloatValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, float& fValue)
{
    // Do not use the TimeHasChanged check here, because blend interpolators
    // should always update their interpolators.

    bool bReturnValue = false;
    if (m_ucInterpCount == 1)
    {
        bReturnValue = StoreSingleValue(fTime, pkInterpTarget, fValue);
    }
    else if (m_ucInterpCount > 0)
    {
        ComputeNormalizedWeights();
       
        bReturnValue = BlendValues(fTime, pkInterpTarget, fValue);
    }
    
    m_fLastTime = fTime;
    return bReturnValue;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBlendFloatInterpolator);
//---------------------------------------------------------------------------
void NiBlendFloatInterpolator::CopyMembers(NiBlendFloatInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_fFloatValue = m_fFloatValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendFloatInterpolator);
//---------------------------------------------------------------------------
void NiBlendFloatInterpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fFloatValue);
}
//---------------------------------------------------------------------------
void NiBlendFloatInterpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendFloatInterpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fFloatValue);
}
//---------------------------------------------------------------------------
bool NiBlendFloatInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendFloatInterpolator* pkDest = (NiBlendFloatInterpolator*) pkObject;

    if (m_fFloatValue != pkDest->m_fFloatValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendFloatInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendFloatInterpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_fFloatValue", m_fFloatValue));
}
//---------------------------------------------------------------------------
