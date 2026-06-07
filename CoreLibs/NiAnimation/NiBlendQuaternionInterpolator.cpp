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

#include "NiBlendQuaternionInterpolator.h"

NiImplementRTTI(NiBlendQuaternionInterpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendQuaternionInterpolator::NiBlendQuaternionInterpolator(
    bool bManagerControlled, float fWeightThreshold,
    unsigned char ucArraySize) : NiBlendInterpolator(bManagerControlled,
    fWeightThreshold, ucArraySize), m_kQuaternionValue(INVALID_QUATERNION)
{
}
//---------------------------------------------------------------------------
NiBlendQuaternionInterpolator::NiBlendQuaternionInterpolator() :
    m_kQuaternionValue(INVALID_QUATERNION)
{
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::IsQuaternionValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsQuaternionValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, NiQuaternion& kValue)
{
    if (!GetSingleUpdateTime(fTime))
    {
        m_kQuaternionValue = kValue = INVALID_QUATERNION;
        return false;
    }

    NIASSERT(m_pkSingleInterpolator != NULL);
    if (!m_pkSingleInterpolator->Update(fTime, 
        pkInterpTarget, m_kQuaternionValue))
    {
        m_kQuaternionValue = kValue = INVALID_QUATERNION;
        return false;
    }

    kValue = m_kQuaternionValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::BlendValues(float fTime, NiObjectNET* 
    pkInterpTarget, NiQuaternion& kValue)
{
    // Generally speaking, when you want to blend between quaternions,
    // you use slerp. There are three qualities that any rotation
    // interpolation needs to satisfy.
    // 1) Commutativity
    // 2) Constant Velocity
    // 3) Minimal Torque (ie shortest path on great sphere)
    // Slerp has 2 and 3. For blending like this, we really want 1.
    // Therefore, we will use normalized lerp. This algorithm has the
    // properties of 1 and 3. 
    
    bool bValidValue = false;
    m_kQuaternionValue.SetValues(0.0f, 0.0f, 0.0f,
        0.0f);
    float fTotalValueWeight = 0.0f;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[uc].m_spInterpolator;
        if (pkInterpolator && m_pkInterpArray[uc].m_fNormalizedWeight > 0.0f)
        {
            NiQuaternion kVal;
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime, m_pkInterpArray[uc]))
                continue;

            bool bSuccess = pkInterpolator->Update(fUpdateTime,
                pkInterpTarget, kVal);

            if (bSuccess)
            {
                // Dot only represents the angle between quats when they are
                // unitized. 
                if (fTotalValueWeight > 0.0f)
                {
                    float fCos = NiQuaternion::Dot(m_kQuaternionValue,
                        kVal);

                    // if the angle is negative, we need to invert the quat to 
                    // get the best path
                    if (fCos < 0.0f)
                    {
                        kVal = -kVal;
                    }
                }

                // Multiply in the weights to the quaternions.
                // Note that this makes them non-rotations.
                kVal = kVal * m_pkInterpArray[uc].m_fNormalizedWeight;
                m_kQuaternionValue = m_kQuaternionValue * fTotalValueWeight;

                // Accumulate the total weighted values into the 
                m_kQuaternionValue.SetValues(
                    kVal.GetW() + m_kQuaternionValue.GetW(), 
                    kVal.GetX() + m_kQuaternionValue.GetX(), 
                    kVal.GetY() + m_kQuaternionValue.GetY(),
                    kVal.GetZ() + m_kQuaternionValue.GetZ());

                // Need to re-normalize quaternion.
                m_kQuaternionValue.Normalize();
                fTotalValueWeight += m_pkInterpArray[uc].m_fNormalizedWeight;

                bValidValue = true;
            }
        }
    }

    if (!bValidValue)
    {
        kValue = m_kQuaternionValue = INVALID_QUATERNION;
        return false;
    }

    kValue = m_kQuaternionValue;
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, NiQuaternion& kValue)
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
NiImplementCreateClone(NiBlendQuaternionInterpolator);
//---------------------------------------------------------------------------
void NiBlendQuaternionInterpolator::CopyMembers(
    NiBlendQuaternionInterpolator* pkDest, NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kQuaternionValue = m_kQuaternionValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendQuaternionInterpolator);

//---------------------------------------------------------------------------
void NiBlendQuaternionInterpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);

    m_kQuaternionValue.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBlendQuaternionInterpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendQuaternionInterpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);

    m_kQuaternionValue.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendQuaternionInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendQuaternionInterpolator* pkDest = (NiBlendQuaternionInterpolator*)
        pkObject;

    if (m_kQuaternionValue != pkDest->m_kQuaternionValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendQuaternionInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendQuaternionInterpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(m_kQuaternionValue.GetViewerString("m_kQuaternionValue"));
}
//---------------------------------------------------------------------------
