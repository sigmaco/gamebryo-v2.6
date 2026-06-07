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

#include "NiBlendBoolInterpolator.h"
#include <NiStream.h>

NiImplementRTTI(NiBlendBoolInterpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendBoolInterpolator::NiBlendBoolInterpolator(bool bManagerControlled,
    float fWeightThreshold, unsigned char ucArraySize) : 
    NiBlendInterpolator(bManagerControlled, fWeightThreshold, ucArraySize),
    m_bBoolValue(INVALID_BOOL)
{
}
//---------------------------------------------------------------------------
NiBlendBoolInterpolator::NiBlendBoolInterpolator() :
    m_bBoolValue(INVALID_BOOL)
{
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::IsBoolValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsBoolValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, bool& bValue)
{
    if (!GetSingleUpdateTime(fTime))
        return false;

    NIASSERT(m_pkSingleInterpolator != NULL);
    bool bSuccess = m_pkSingleInterpolator->Update(fTime, pkInterpTarget, 
        bValue);
    if (bSuccess)
    {
       m_bBoolValue = NiBool(bValue);
       return true;
    }
    else
    {
       bValue = false;
       m_bBoolValue = INVALID_BOOL;
       return false;
    }
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::BlendValues(float fTime, 
    NiObjectNET* pkInterpTarget, bool& bValue)
{
    bool bModifiedValue = false;
    float fBlendValue = 0.0f;
    float fTotalNormalizedWeights = 1.0f;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[uc].m_spInterpolator;
        if (pkInterpolator && m_pkInterpArray[uc].m_fNormalizedWeight > 0.0f)
        {
            bool bVal;
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime, m_pkInterpArray[uc]))
            {
                fTotalNormalizedWeights -= m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                continue;
            }

            bool bSuccess = pkInterpolator->Update(fUpdateTime,
                pkInterpTarget, bVal);

            if (bSuccess)
            {
                float fValue = (float) bVal;
                fBlendValue += fValue * m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
                bModifiedValue = true;
            }
            else
            {
                fTotalNormalizedWeights -= m_pkInterpArray[uc]
                    .m_fNormalizedWeight;
            }
        }
    }

    // If we encountered any Update failures, we 
    // need to correct the final value for those
    // failures.
    fBlendValue /= fTotalNormalizedWeights;

    if (!bModifiedValue)
    {
        bValue = false;
        m_bBoolValue = INVALID_BOOL;
        return false;
    }
    else if (fBlendValue < 0.5f)
    {
        m_bBoolValue = NiBool(false);
        bValue = false;
        return true;
    }
    else
    {
        m_bBoolValue = NiBool(true);
        bValue = true;
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, bool& bValue)
{
    // Do not use the TimeHasChanged check here, because blend interpolators
    // should always update their interpolators.

    bool bReturnValue = false;
    if (m_ucInterpCount == 1)
    {
        bReturnValue = StoreSingleValue(fTime, pkInterpTarget, bValue);
    }
    else if (m_ucInterpCount > 0)
    {
        ComputeNormalizedWeights();
       
        bReturnValue = BlendValues(fTime, pkInterpTarget, bValue);
    }
    
    m_fLastTime = fTime;
    return bReturnValue;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBlendBoolInterpolator);
//---------------------------------------------------------------------------
void NiBlendBoolInterpolator::CopyMembers(NiBlendBoolInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_bBoolValue = m_bBoolValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendBoolInterpolator);
//---------------------------------------------------------------------------
void NiBlendBoolInterpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_bBoolValue);
}
//---------------------------------------------------------------------------
void NiBlendBoolInterpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendBoolInterpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_bBoolValue);
}
//---------------------------------------------------------------------------
bool NiBlendBoolInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendBoolInterpolator* pkDest = (NiBlendBoolInterpolator*) pkObject;

    if (m_bBoolValue != pkDest->m_bBoolValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendBoolInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendBoolInterpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_bBoolValue", m_bBoolValue));
}
//---------------------------------------------------------------------------
