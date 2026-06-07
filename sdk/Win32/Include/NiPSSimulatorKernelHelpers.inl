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

#include "NiPSFlagsHelpers.h"
#include "NiOS.h"

//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSSimulatorKernelHelpers::UpdateParticlePosition(
    NiPoint3& kOPosition,
    const NiPoint3& kIPosition,
    const NiPoint3& kIVelocity,
    const float fILastUpdate,
    const float fCurrentTime)
{
    kOPosition = kIPosition + (kIVelocity * (fCurrentTime - fILastUpdate));
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSSimulatorKernelHelpers::UpdateParticleGrowShrink(
    float& fOSize,
    const NiUInt16 usIGeneration,
    const float fIAge,
    const float fILifeSpan,
    const float fGrowTime,
    const float fShrinkTime,
    const NiUInt16 usGrowGeneration,
    const NiUInt16 usShrinkGeneration)
{
    float fGrow = 1.0f;
    if (usIGeneration == usGrowGeneration &&
        fIAge < fGrowTime &&
        fGrowTime > 0.0f)
    {
        fGrow = fIAge / fGrowTime;
    }

    float fShrink = 1.0f;
    float fTimeLeft = fILifeSpan - fIAge;
    if (usIGeneration == usShrinkGeneration &&
        fTimeLeft < fShrinkTime &&
        fShrinkTime > 0.0f)
    {
        fShrink = fTimeLeft / fShrinkTime;
    }

    fOSize = (fShrink > fGrow ? fGrow : fShrink);
    if (fOSize < NIPSKERNEL_EPSILON)
    {
        fOSize = NIPSKERNEL_EPSILON;
    }
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSSimulatorKernelHelpers::UpdateParticleColor(
    NiRGBA& kOColor,
    const float fIAge,
    const float fILifeSpan,
    const NiPSKernelColorKey* pkColorKeys,
    const NiUInt8 ucNumColorKeys)
{
    // Compute scaled age (between 0.0 and 1.0).
    const float fScaledAge = fIAge / fILifeSpan;

    // Find surrounding color values in the color keys.
    const NiPSKernelColorKey* pkPrevKey = &pkColorKeys[0];
    const NiPSKernelColorKey* pkNextKey = pkPrevKey;
    for (NiUInt8 uc = 1; uc < ucNumColorKeys; ++uc)
    {
        pkNextKey = &pkColorKeys[uc];
        if (fScaledAge <= pkNextKey->m_fTime)
        {
            break;
        }

        pkPrevKey = pkNextKey;
    }

    // Compute normalized time between the keys.
    float fNormTime = (fScaledAge - pkPrevKey->m_fTime) /
        (pkNextKey->m_fTime - pkPrevKey->m_fTime);

    // Linear interpolation only.
    kOColor = (1.0f - fNormTime) * pkPrevKey->m_kColor +
        fNormTime * pkNextKey->m_kColor;
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSSimulatorKernelHelpers::UpdateParticleRotation(
    float& fORotAngle,
    const float fIRotAngle,
    const float fIRotSpeed,
    const float fTimeDelta)
{
    // Compute updated rotation angle.
    fORotAngle = fIRotAngle + (fIRotSpeed * fTimeDelta);

    const float fTwoPi = NI_TWO_PI;
    const float fTenPi = 10.0f * NI_PI;

    // Ensure that rotation angle is in the range [-2Pi, 2Pi].
    if (fORotAngle > fTenPi || fORotAngle < -fTenPi)
    {
        // Avoid long loops. This should only happen if this single frame's
        // rotation is greater than ten Pi, which should be very rare.
        fORotAngle = 0.0f;
    }
    else
    {
        while (fORotAngle > fTwoPi)
        {
            fORotAngle -= fTwoPi;
        }
        while (fORotAngle < -fTwoPi)
        {
            fORotAngle += fTwoPi;
        }
    }
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSSimulatorKernelHelpers::UpdateParticleAgeDeath(
    float& fOAge,
    float& fOLastUpdateTime,
    NiUInt32& uiOFlags,
    const float fIAge,
    const float fILifeSpan,
    const float fILastUpdateTime,
    const NiUInt32 uiIFlags,
    const float fCurrentTime,
    const bool bSpawnOnDeath,
    const NiUInt16 ucSpawnerID)
{
    // Update age.
    fOAge = fIAge + (fCurrentTime - fILastUpdateTime);

    // Copy last update time.
    fOLastUpdateTime = fILastUpdateTime;

    // Copy particle flags.
    uiOFlags = uiIFlags;

    // Update death.
    if (fOAge > fILifeSpan)
    {
        // Set the particle to be killed.
        NiPSFlagsHelpers::SetShouldDie(uiOFlags, true);

        // Handle spawning.
        if (bSpawnOnDeath)
        {
            // Update fOLastUpdateTime with the time of death. This is used
            // when spawning on death.
            fOLastUpdateTime = fCurrentTime - (fOAge - fILifeSpan);

            // Set the particle to spawn.
            NiPSFlagsHelpers::SetShouldSpawn(uiOFlags, true);
            NiPSFlagsHelpers::SetSpawnerID(uiOFlags, ucSpawnerID);
        }
    }
}
//---------------------------------------------------------------------------
