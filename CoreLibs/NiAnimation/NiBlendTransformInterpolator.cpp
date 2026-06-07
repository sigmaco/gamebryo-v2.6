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

#include "NiBlendTransformInterpolator.h"

NiImplementRTTI(NiBlendTransformInterpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendTransformInterpolator::NiBlendTransformInterpolator(
    bool bManagerControlled, float fWeightThreshold,
    unsigned char ucArraySize) : NiBlendInterpolator(bManagerControlled,
    fWeightThreshold, ucArraySize)
{
}
//---------------------------------------------------------------------------
NiBlendTransformInterpolator::NiBlendTransformInterpolator()
{
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::IsTransformValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsTransformValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
{
    if (!GetSingleUpdateTime(fTime))
    {
        kValue.MakeInvalid();
        return false;
    }

    NIASSERT(m_pkSingleInterpolator != NULL);
    if (m_pkSingleInterpolator->Update(fTime, pkInterpTarget, kValue))
    {
        return true;
    }
    else
    {
        kValue.MakeInvalid();
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::BlendValues(float fTime, 
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
{
    float fTotalTransWeight = 1.0f;
    float fTotalScaleWeight = 1.0f;

    NiPoint3 kFinalTranslate = NiPoint3::ZERO;
    NiQuaternion kFinalRotate = NiQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    float fFinalScale = 0.0f;

    bool bTransChanged = false;
    bool bRotChanged = false;
    bool bScaleChanged = false;

    bool bFirstRotation = true;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[uc];
        if (kItem.m_spInterpolator && kItem.m_fNormalizedWeight > 0.0f)
        {
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime, kItem))
            {
                fTotalTransWeight -= kItem.m_fNormalizedWeight;
                fTotalScaleWeight -= kItem.m_fNormalizedWeight;
                continue;
            }

            NiQuatTransform kTransform;
            bool bSuccess = kItem.m_spInterpolator->Update(fUpdateTime, 
                pkInterpTarget, kTransform);

            if (bSuccess)
            {
                // Add in the current interpolator's weighted
                // translation to the accumulated translation thus far.
                if (kTransform.IsTranslateValid())
                {
                    kFinalTranslate += kTransform.GetTranslate() *
                        kItem.m_fNormalizedWeight;
                    bTransChanged = true;
                }
                else
                {
                    // This translate is invalid, so we need to
                    // remove it's overall weight from the result
                    // at the end
                    fTotalTransWeight -= kItem.m_fNormalizedWeight;
                }

                // Add in the current interpolator's weighted
                // rotation to the accumulated rotation thus far.
                // Since quaternion SLERP is not commutative, we can
                // get away with accumulating weighted sums of the quaternions
                // as long as we re-normalize at the end.
                if (kTransform.IsRotateValid())
                {
                    NiQuaternion kRotValue = kTransform.GetRotate();

                    // Dot only represents the angle between quats when they
                    // are unitized. However, we don't care about the 
                    // specific angle. We only care about the sign of the
                    // angle between the two quats. This is preserved when
                    // quaternions are non-unit.
                    if (!bFirstRotation)
                    {
                        float fCos = NiQuaternion::Dot(kFinalRotate,
                            kRotValue);

                        // If the angle is negative, we need to invert the
                        // quat to get the best path.
                        if (fCos < 0.0f)
                        {
                            kRotValue = -kRotValue;
                        }
                    }
                    else
                    {
                        bFirstRotation = false;
                    }

                    // Multiply in the weights to the quaternions.
                    // Note that this makes them non-rotations.
                    kRotValue = kRotValue * kItem.m_fNormalizedWeight;

                    // Accumulate the total weighted values into the 
                    // rotation
                    kFinalRotate.SetValues(
                        kRotValue.GetW() + kFinalRotate.GetW(), 
                        kRotValue.GetX() + kFinalRotate.GetX(), 
                        kRotValue.GetY() + kFinalRotate.GetY(),
                        kRotValue.GetZ() + kFinalRotate.GetZ());

                    // Need to re-normalize quaternion.
                    bRotChanged = true;
                }
                // we don't need to remove the weight of invalid rotations
                // since we are re-normalizing at the end. It's just extra
                // work.

                // Add in the current interpolator's weighted
                // scale to the accumulated scale thus far.
                if (kTransform.IsScaleValid())
                {
                    fFinalScale += kTransform.GetScale() *
                        kItem.m_fNormalizedWeight;
                    bScaleChanged = true;
                }
                else 
                {
                    // This scale is invalid, so we need to
                    // remove it's overall weight from the result
                    // at the end
                    fTotalScaleWeight -= kItem.m_fNormalizedWeight;
                }
            }
            else
            {
                // If the update failed, we should 
                // remove the weights of the interpolator
                fTotalTransWeight -= kItem.m_fNormalizedWeight;
                fTotalScaleWeight -= kItem.m_fNormalizedWeight;
            }
        }
    }

    // If any of the channels were animated, the final
    // transform needs to be updated
    kValue.MakeInvalid();
    if (bTransChanged || bRotChanged || bScaleChanged)
    {
        // Since channels may or may not actually have been
        // active during the blend, we can remove the weights for
        // channels that weren't active.

        if (bTransChanged)
        {
            // Remove the effect of invalid translations from the
            // weighted sum
            NIASSERT(fTotalTransWeight != 0.0f);
            kFinalTranslate /= fTotalTransWeight;
            kValue.SetTranslate(kFinalTranslate);
        }
        if (bRotChanged)
        {
            // Since we summed quaternions earlier, we have
            // non-unit quaternions, which are not rotations.
            // To make the accumulated quaternion a rotation, we 
            // need to normalize.
            kFinalRotate.Normalize();
            kValue.SetRotate(kFinalRotate);
        }
        if (bScaleChanged)
        {
            // Remove the effect of invalid scales from the
            // weighted sum
            NIASSERT(fTotalScaleWeight != 0.0f);
            fFinalScale /= fTotalScaleWeight;
            kValue.SetScale(fFinalScale);
        }
    }

    if (kValue.IsTransformInvalid())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
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
NiImplementCreateClone(NiBlendTransformInterpolator);
//---------------------------------------------------------------------------
void NiBlendTransformInterpolator::CreateCloneInPlace(
    NiBlendTransformInterpolator* pkClone, NiCloningProcess& kCloning)
{
    NIASSERT(pkClone != NULL);
    CopyMembers(pkClone, kCloning);
}
//---------------------------------------------------------------------------
void NiBlendTransformInterpolator::CopyMembers(
    NiBlendTransformInterpolator* pkDest, NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendTransformInterpolator);
//---------------------------------------------------------------------------
void NiBlendTransformInterpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBlendTransformInterpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendTransformInterpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendTransformInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendTransformInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendTransformInterpolator::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
