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
#if defined(_PS3)
// Prevent inlining of the outermost functions to allow inner function calls
// to be properly inlined
#define NI_MAY_INLINE inline NI_NOINLINE
#else
// No need except on GCC
#define NI_MAY_INLINE inline
#endif
//---------------------------------------------------------------------------

NI_MAY_INLINE void NiPSForceHelpers::ApplyBombForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::BombData& kInputData,
    const float fCurrentTime)
{
    for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
    {
        // Copy velocity.
        pkOVelocities[ui] = pkIVelocities[ui];

        NiPoint3 kDir = pkIPositions[ui] - kInputData.m_kPosition;
        float fDist = kDir.Length();
        if (kInputData.m_eDecayType ==
                NiPSForceDefinitions::BombData::DECAY_NONE ||
            fDist <= kInputData.m_fDecay)
        {
            NiPoint3 kForce;
            switch (kInputData.m_eSymmType)
            {
                case NiPSForceDefinitions::BombData::SYMM_SPHERICAL:
                    kForce = kDir / fDist;
                    break;
                case NiPSForceDefinitions::BombData::SYMM_PLANAR:
                    fDist = kInputData.m_kDirection * kDir;
                    if (fDist < 0.0f)
                    {
                        fDist = -fDist;
                        kForce = -kInputData.m_kDirection;
                    }
                    else
                    {
                        kForce = kInputData.m_kDirection;
                    }
                    break;
                case NiPSForceDefinitions::BombData::SYMM_CYLINDRICAL:
                    // Subtract off the vector component along the
                    // cylinder axis
                    fDist = kInputData.m_kDirection * kDir;

                    kForce = kDir - fDist * kInputData.m_kDirection;

                    fDist = kForce.Length();
                    if (fDist != 0.0f)
                    {
                        kForce /= fDist;
                    }
                    break;
                default:
                    // Should never hit this case.
                    return;
            }

            float fDecay = 1.0f;
            switch (kInputData.m_eDecayType)
            {
                case NiPSForceDefinitions::BombData::DECAY_LINEAR:
                    fDecay = (kInputData.m_fDecay - fDist) /
                        kInputData.m_fDecay;
                    break;
                case NiPSForceDefinitions::BombData::DECAY_EXPONENTIAL:
                    fDecay = NiExp(-fDist / kInputData.m_fDecay);
                    break;
                default:
                    break;
            }

            // we modify the velocity by multiplying the following;
            // 1) fDecay - a multiplier between 0 and 1 based off of
            //  how far the particle is from the bomb
            // 2) kForce - a vector representing the direction of force
            //  this is a unit vector.
            // 3) m_fDeltaV - the strength value exported from the modeling
            //  package.  This value is in units of mass / time^2
            // 4) fDeltaTime - the change in time from the last update
            pkOVelocities[ui] = pkIVelocities[ui] + fDecay *
                kInputData.m_fDeltaV * (fCurrentTime - pfILastUpdateTimes[ui])
                * kForce;
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyDragForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::DragData& kInputData,
    const float fCurrentTime)
{
    float fLenSquared = kInputData.m_kDirection.SqrLength();
    float fRangeDifference = kInputData.m_fRangeFalloff -
        kInputData.m_fRange;
            
    for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
    {
        // Copy velocity.
        pkOVelocities[ui] = pkIVelocities[ui];

        float fDistance = (pkIPositions[ui] - kInputData.m_kPosition)
            .Length();
        float fPercentage = kInputData.m_fPercentage;

        if (fDistance > kInputData.m_fRange)
        {
            if (fDistance >= kInputData.m_fRangeFalloff)
            {
                continue;
            }

            float fDifference = fDistance - kInputData.m_fRange;
            fPercentage = (1.0f - fDifference / fRangeDifference) * 
                fPercentage;
        }
    
    
        // Project the drag axis onto the velocity vector
        float fProj = pkIVelocities[ui].Dot(kInputData.m_kDirection);

        // Ensure you never decelerate so much we go backwards
        float fDeltaT = (fCurrentTime - pfILastUpdateTimes[ui]) / 0.0333333f;
        if (fPercentage * fDeltaT > 1.0f)
        {
            pkOVelocities[ui] = pkIVelocities[ui] +
                ((-fProj / fLenSquared) * kInputData.m_kDirection);
        }
        else
        {
            pkOVelocities[ui] = pkIVelocities[ui] +
                (-fPercentage * fDeltaT * (fProj / fLenSquared) *
                kInputData.m_kDirection);
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyAirFieldForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::AirFieldData& kInputData,
    const float fCurrentTime)
{
    // Check for optimized update path.
    if (kInputData.m_fAttenuation == 0.0f &&
        kInputData.m_fInheritedVelocity == 0.0f &&
        !kInputData.m_bEnableSpread)
    {
        if (kInputData.m_bUseMaxDistance)
        {
            // Update no attenuation no inherit no spread.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Copy velocity.
                pkOVelocities[ui] = pkIVelocities[ui];

                NiPoint3 kParticleOffset = pkIPositions[ui] -
                    kInputData.m_kTransformedPosition;

                // Check for max distance.
                if (kParticleOffset.SqrLength() <=
                    kInputData.m_fMaxDistanceSqr)
                {
                    // Don't go faster then the air.
                    if (kInputData.m_kAirFieldVelocity.Dot(
                        pkIVelocities[ui] - kInputData.m_kAirFieldVelocity)
                        < 0.0f)
                    {
                        // Apply air to the particle velocity.
                        pkOVelocities[ui] = pkIVelocities[ui] +
                            (kInputData.m_kAirFieldVelocity *
                            kInputData.m_fAirFriction *
                            (fCurrentTime - pfILastUpdateTimes[ui]));
                    }
                }
            }
        }
        else
        {
            // Update no attenuation no inherit no spread no max distance.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Copy velocity.
                pkOVelocities[ui] = pkIVelocities[ui];

                // Don't go faster then the air
                if (kInputData.m_kAirFieldVelocity.Dot(pkIVelocities[ui] -
                        kInputData.m_kAirFieldVelocity) < 0.0f)
                {
                    // Apply air to the particle velocity.
                    pkOVelocities[ui] = pkIVelocities[ui] +
                        (kInputData.m_kAirFieldVelocity *
                        kInputData.m_fAirFriction *
                        (fCurrentTime - pfILastUpdateTimes[ui]));
                }
            }
        }
    }
    else
    {
        // Standard update.
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Copy velocity.
            pkOVelocities[ui] = pkIVelocities[ui];

            NiPoint3 kParticleOffset = pkIPositions[ui] -
                kInputData.m_kTransformedPosition;
            float fDistanceSqr = kParticleOffset.SqrLength();

            if (!kInputData.m_bUseMaxDistance ||
                fDistanceSqr <= kInputData.m_fMaxDistanceSqr)
            {
                NiPoint3 kAirVelocity = kInputData.m_kAirFieldVelocity;

                // Apply spread.
                if (kInputData.m_bEnableSpread)
                {
                    kAirVelocity = kParticleOffset;
                    kAirVelocity.Unitize();

                    // If outside the cone, they don't affect this
                    // particle.
                    if (kInputData.m_kUnitAirFieldVelocity.Dot(
                        kAirVelocity) < kInputData.m_fCosSpread)
                    {
                        continue;
                    }

                    kAirVelocity *= kInputData.m_fAirFieldVelocityLength;
                }

                NiPoint3 kPartVel = pkIVelocities[ui];

                // Don't go faster then the air.
                if (kAirVelocity.Dot(kPartVel - kAirVelocity) < 0.0f)
                {
                    float fAttenuationWithTime = fCurrentTime -
                        pfILastUpdateTimes[ui];

                    // Apply attenuation.
                    if (kInputData.m_bUseMaxDistance &&
                        kInputData.m_fAttenuation != 0.0f)
                    {
                        fAttenuationWithTime *= NiPow(1.0f -
                            (NiSqrt(fDistanceSqr) /
                                kInputData.m_fMaxDistance),
                            kInputData.m_fAttenuation);
                    }

                    // Apply air to the particle velocity.
                    kPartVel += kAirVelocity * kInputData.m_fAirFriction *
                        fAttenuationWithTime;

                    // Apply the computed velocity.
                    pkOVelocities[ui] = kPartVel;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyDragFieldForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::DragFieldData& kInputData,
    const float fCurrentTime)
{
    // Check for optimized update path.
    if (!kInputData.m_bUseDirection && kInputData.m_fAttenuation == 0.0f)
    {
        if (kInputData.m_bUseMaxDistance)
        {
            // Update no attenuation no dir.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Copy velocity.
                pkOVelocities[ui] = pkIVelocities[ui];

                // Compute the reduction.
                float fReduction = kInputData.m_fMagnitude * (fCurrentTime -
                    pfILastUpdateTimes[ui]);
                if (fReduction == 0.0f)
                {
                    continue;
                }

                if ((pkIPositions[ui] - kInputData.m_kPosition).SqrLength() <= 
                    kInputData.m_fMaxDistanceSqr)
                {
                    // Ensure we never decelerate so much we go backwards.
                    if (fReduction < 1.0f)
                    {
                        // Compute the change in particle velocity
                        pkOVelocities[ui] = pkIVelocities[ui] *
                            (1.0f - fReduction);
                    }
                    else
                    {
                        pkOVelocities[ui] = NiPoint3(0.0f, 0.0f, 0.0f);
                    }
                }
            }
        }
        else
        {
            // Update no attenuation no dir no dist.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Copy velocity.
                pkOVelocities[ui] = pkIVelocities[ui];

                float fReduction = kInputData.m_fMagnitude * (fCurrentTime - 
                    pfILastUpdateTimes[ui]);

                // Ensure we never decelerate so much we go backwards.
                if (fReduction < 1.0f)
                {
                    // Compute the change in particle velocity
                    pkOVelocities[ui] = pkIVelocities[ui] *
                        (1.0f - fReduction);
                }
                else
                {
                    pkOVelocities[ui] = NiPoint3(0.0f, 0.0f, 0.0f);
                }
            }
        }
    }
    else
    {
        // Standard update.
        // (Magnitude * DeltaTime) / (1.0 + (Distance * Attenuation))
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Copy velocity.
            pkOVelocities[ui] = pkIVelocities[ui];

            float fDistance = (pkIPositions[ui] - kInputData.m_kPosition)
                .Length();
        
            if (!kInputData.m_bUseMaxDistance ||
                fDistance <= kInputData.m_fMaxDistance)
            {
                // Project the drag axis onto the velocity vector
                float fProj = 1.0f;
                if (kInputData.m_bUseDirection)
                {
                    NiPoint3 kParticleDirection = pkIVelocities[ui];
                    kParticleDirection.Unitize();
                    fProj = kParticleDirection.Dot(kInputData.m_kDragAxis);
                }

                float fReduction = kInputData.m_fMagnitude *
                    (fCurrentTime - pfILastUpdateTimes[ui]) * fProj /
                    (1.0f +  fDistance * kInputData.m_fAttenuation);

                // Ensure we never decelerate so much we go backwards
                if (fReduction < 1.0f)
                {
                    pkOVelocities[ui] = pkIVelocities[ui] -
                        (fReduction * pkIVelocities[ui]);
                }
                else
                {
                    pkOVelocities[ui] = NiPoint3(0.0f, 0.0f, 0.0f);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyGravityFieldForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::GravityFieldData& kInputData,
    const float fCurrentTime)
{
    // Check for optimized update path.
    if (!kInputData.m_bUseMaxDistance && (kInputData.m_fAttenuation == 0.0f))
    {
        // Update no attenuation no dist.
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            pkOVelocities[ui] = pkIVelocities[ui] +
                (kInputData.m_kTransformedDirection *
                (fCurrentTime - pfILastUpdateTimes[ui]));
        }
    }
    else
    {
        // Standard update.
        // Direction * (Magnitude * DeltaTime / NiPow(Distance, Attenuation))
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Copy velocity.
            pkOVelocities[ui] = pkIVelocities[ui];

            float fDistanceSqr = (pkIPositions[ui] -
                kInputData.m_kTransformedPosition).SqrLength();

            if (!kInputData.m_bUseMaxDistance ||
                (fDistanceSqr <= kInputData.m_fMaxDistanceSqr))
            {
                float fDeltaT = fCurrentTime - pfILastUpdateTimes[ui];

                NiPoint3 kDeltaV;
                if (kInputData.m_fAttenuation == 0.0f ||
                    fDistanceSqr == 0.0f)
                {
                    kDeltaV = kInputData.m_kTransformedDirection *
                        (kInputData.m_fMagnitude * fDeltaT);
                }
                else
                {
                    kDeltaV = kInputData.m_kTransformedDirection *
                        (kInputData.m_fMagnitude * fDeltaT / NiMax(1.0f,
                        NiPow(fDistanceSqr,
                            kInputData.m_fHalfAttenuation)));
                }

                pkOVelocities[ui] = pkIVelocities[ui] + kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyRadialFieldForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::RadialFieldData& kInputData,
    const float fCurrentTime)
{
    // Check for optimized update path.
    if (!kInputData.m_bUseMaxDistance && (kInputData.m_fAttenuation == 0.0f ||
        kInputData.m_fRadialType == 0.0f))
    {
        // Update no attenuation no dir no dist.
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Find the direction from the particle to the Radial
            NiPoint3 kDirection = pkIPositions[ui] -
                kInputData.m_kTransformedPosition;

            // Since kDirection already has the distance we don't need to
            // determine and multiply by the distance.
            pkOVelocities[ui] = pkIVelocities[ui] + (kDirection *
                (kInputData.m_fMagnitude *
                (fCurrentTime - pfILastUpdateTimes[ui])));
        }
    }
    else
    {
        // Standard update.
        // Direction * (Magnitude * DeltaTime / NiPow(Distance, Attenuation))
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Copy velocity.
            pkOVelocities[ui] = pkIVelocities[ui];

            NiPoint3 kDirection = pkIPositions[ui] -
                kInputData.m_kTransformedPosition;
            float fDistanceSqr = kDirection.SqrLength();

            if (!kInputData.m_bUseMaxDistance ||
                fDistanceSqr <= kInputData.m_fMaxDistanceSqr)
            {
                float fDeltaT = fCurrentTime - pfILastUpdateTimes[ui];

                NiPoint3 kDeltaV;
                if (kInputData.m_fAttenuation == 0.0f ||
                    fDistanceSqr == 0.0f)
                {
                    kDeltaV = kDirection * (kInputData.m_fMagnitude *
                        fDeltaT);
                }
                else
                {
                    // Since kDirection already has the distance we don't
                    // need to determine and multiply by the distance.
                    kDeltaV = kDirection * (kInputData.m_fMagnitude *
                        fDeltaT / NiMax(1.0f, NiPow(fDistanceSqr,
                        kInputData.m_fHalfAttenuation) *
                        kInputData.m_fRadialType));
                }

                pkOVelocities[ui] = pkIVelocities[ui] + kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyTurbulenceFieldForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float*,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::TurbulenceFieldData& kInputData,
    const float)
{
    // Check for optimized update path.
    if (kInputData.m_fAttenuation == 0.0f)
    {
        if (kInputData.m_bUseMaxDistance)
        {
            // Update no attenuation.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Copy velocity.
                pkOVelocities[ui] = pkIVelocities[ui];

                if ((pkIPositions[ui] - kInputData.m_kTransformedPosition)
                    .SqrLength() <= kInputData.m_fMaxDistanceSqr)
                {
                    // Create a random turbulence.
                    NiPoint3 kDeltaV(
                        NiSymmetricRandom(),
                        NiSymmetricRandom(),
                        NiSymmetricRandom());
                    
                    // Apply the random turbulence by the magnitude.
                    pkOVelocities[ui] = pkIVelocities[ui] +
                        (kDeltaV * kInputData.m_fMagnitude);
                }
            }
        }
        else
        {
            // Update no attenuation no distance.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Create a random turbulence.
                NiPoint3 kDeltaV(
                    NiSymmetricRandom(),
                    NiSymmetricRandom(),
                    NiSymmetricRandom());
                
                // Apply the random turbulence by the magnitude.
                pkOVelocities[ui] = pkIVelocities[ui] + (kDeltaV *
                    kInputData.m_fMagnitude);
            }
        }
    }
    else
    {
        // Standard update.
        // Magnitude / (1.0 + (Distance * Attenuation))
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Copy velocity.
            pkOVelocities[ui] = pkIVelocities[ui];

            float fDistance = (pkIPositions[ui] -
                kInputData.m_kTransformedPosition).Length();
            
            if (!kInputData.m_bUseMaxDistance ||
                fDistance <= kInputData.m_fMaxDistance)
            {
                NiPoint3 kDeltaV(
                    NiSymmetricRandom(),
                    NiSymmetricRandom(),
                    NiSymmetricRandom());

                kDeltaV *= kInputData.m_fMagnitude / (1.0f + fDistance *
                    kInputData.m_fAttenuation);
                
                pkOVelocities[ui] = pkIVelocities[ui] + kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyVortexFieldForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::VortexFieldData& kInputData,
    const float fCurrentTime)
{
    // Check for optimized update path.
    if (kInputData.m_fAttenuation == 0.0f)
    {
        if (kInputData.m_bUseMaxDistance)
        {
            // Update no attenuation.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                // Copy velocity.
                pkOVelocities[ui] = pkIVelocities[ui];

                float fDistanceSqr = (pkIPositions[ui] -
                    kInputData.m_kTransformedPosition).SqrLength();

                if (fDistanceSqr != 0.0f &&
                    fDistanceSqr <= kInputData.m_fMaxDistanceSqr)
                {
                    NiPoint3 kDiff = pkIPositions[ui] -
                        kInputData.m_kTransformedPosition;
                    float fSegParam = kInputData.m_kTransformedDirection
                        .Dot(kDiff);
                    kDiff -= fSegParam *
                        kInputData.m_kTransformedDirection;

                    NiPoint3 kDeltaV;
                    if (kDiff == NiPoint3(0.0f, 0.0f, 0.0f))
                    {
                        kDeltaV = kDiff;
                    }
                    else
                    {
                        kDeltaV = kInputData.m_kTransformedDirection
                            .UnitCross(kDiff);
                    }

                    kDeltaV *= kInputData.m_fMagnitude *
                        (fCurrentTime - pfILastUpdateTimes[ui]);

                    pkOVelocities[ui] = pkIVelocities[ui] + kDeltaV;
                }
            }
        }
        else
        {
            // Update no attenuation no dist.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                NiPoint3 kDiff = pkIPositions[ui] -
                    kInputData.m_kTransformedPosition;
                float fSegParam = kInputData.m_kTransformedDirection.Dot(
                    kDiff);
                kDiff -= fSegParam * kInputData.m_kTransformedDirection;

                NiPoint3 kDeltaV;
                if (kDiff == NiPoint3(0.0f, 0.0f, 0.0f))
                {
                    kDeltaV = kDiff;
                }
                else
                {
                    kDeltaV = kInputData.m_kTransformedDirection.UnitCross(
                        kDiff);
                }

                kDeltaV *= kInputData.m_fMagnitude *
                    (fCurrentTime - pfILastUpdateTimes[ui]);

                pkOVelocities[ui] = pkIVelocities[ui] + kDeltaV;
            }
        }
    }
    else
    {
        // Standard update.
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Copy velocity.
            pkOVelocities[ui] = pkIVelocities[ui];

            float fDistance = (pkIPositions[ui] -
                kInputData.m_kTransformedPosition).Length();

            if (fDistance != 0.0f && (!kInputData.m_bUseMaxDistance ||
                fDistance <= kInputData.m_fMaxDistance))
            {
                NiPoint3 kDiff = pkIPositions[ui] -
                    kInputData.m_kTransformedPosition;
                float fSegParam = kInputData.m_kTransformedDirection.Dot(
                    kDiff);
                kDiff -= fSegParam * kInputData.m_kTransformedDirection;

                NiPoint3 kDeltaV;
                if (kDiff == NiPoint3(0.0f, 0.0f, 0.0f))
                {
                    kDeltaV = kDiff;
                }
                else
                {
                    kDeltaV = kInputData.m_kTransformedDirection.UnitCross(
                        kDiff);
                }

                float fDeltaT = fCurrentTime - pfILastUpdateTimes[ui];
                if (kInputData.m_fAttenuation == 0.0f)
                {
                    kDeltaV *= kInputData.m_fMagnitude * fDeltaT;
                }
                else
                {
                    kDeltaV *= kInputData.m_fMagnitude * fDeltaT / 
                        NiPow(fDistance, kInputData.m_fAttenuation);
                }

                pkOVelocities[ui] = pkIVelocities[ui] + kDeltaV;
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplyPlanarGravityForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::GravityData& kInputData,
    const float fCurrentTime)
{
    float fStrength = kInputData.m_fStrength * 1.6f;

    NIASSERT(kInputData.m_eForceType ==
        NiPSForceDefinitions::GravityData::FORCE_PLANAR);
    if (kInputData.m_fDecay == 0.0f)
    {
        if (kInputData.m_fTurbulence == 0.0f)
        {
            // No decay, no turbulence.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                pkOVelocities[ui] = pkIVelocities[ui] +
                    (kInputData.m_kDirection * (fStrength *
                    (fCurrentTime - pfILastUpdateTimes[ui])));
            }
        }
        else
        {
            // No decay, turbulence.
            float fTurbulenceScale = kInputData.m_fTurbulence *
                kInputData.m_fTurbulenceScale * 500.0f;

            NiPoint3 kTurbulence;
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;

                pkOVelocities[ui] = pkIVelocities[ui] + ((kTurbulence +
                    (kInputData.m_kDirection * fStrength)) *
                    (fCurrentTime - pfILastUpdateTimes[ui]));
            }
        }
    }
    else
    {
        if (kInputData.m_fTurbulence == 0.0f)
        {
            // Decay, no turbulence.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                NiPoint3 kDir = kInputData.m_kPosition -
                    pkIPositions[ui];

                float fDist = kInputData.m_kDirection * kDir;
                float fDecay;
                if (fDist < 0.0f)
                {
                    fDecay = NiExp(kInputData.m_fDecay * fDist);
                }
                else
                {
                    fDecay = NiExp(-kInputData.m_fDecay * fDist);
                }

                pkOVelocities[ui] = pkIVelocities[ui] +
                    (kInputData.m_kDirection * (fStrength * fDecay *
                    (fCurrentTime - pfILastUpdateTimes[ui])));
            }
        }
        else
        {
            // Decay, turbulence.
            float fTurbulenceScale = kInputData.m_fTurbulence *
                kInputData.m_fTurbulenceScale * 500.0f;

            NiPoint3 kTurbulence;
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;

                NiPoint3 kDir = kInputData.m_kPosition -
                    pkIPositions[ui];

                float fDist = kInputData.m_kDirection * kDir;
                float fDecay;
                if (fDist < 0.0f)
                {
                    fDecay = NiExp(kInputData.m_fDecay * fDist);
                }
                else
                {
                    fDecay = NiExp(-kInputData.m_fDecay * fDist);
                }

                pkOVelocities[ui] = pkIVelocities[ui] + ((kTurbulence +
                    (kInputData.m_kDirection * fStrength * fDecay)) *
                    (fCurrentTime - pfILastUpdateTimes[ui]));
            }
        }
    }
}
//---------------------------------------------------------------------------
NI_MAY_INLINE void NiPSForceHelpers::ApplySphericalGravityForce(
    const NiUInt32 uiBlockCount,
    NiPoint3* pkOVelocities,
    const float* pfILastUpdateTimes,
    const NiPoint3* pkIPositions,
    const NiPoint3* pkIVelocities,
    const NiPSForceDefinitions::GravityData& kInputData,
    const float fCurrentTime)
{
    float fStrength = kInputData.m_fStrength * 1.6f;

    NIASSERT(kInputData.m_eForceType ==
        NiPSForceDefinitions::GravityData::FORCE_SPHERICAL);
    if (kInputData.m_fDecay == 0.0f)
    {
        if (kInputData.m_fTurbulence == 0.0f)
        {
            // No decay, no turbulence.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                NiPoint3 kDir = kInputData.m_kPosition -
                    pkIPositions[ui];
                kDir.Unitize();

                pkOVelocities[ui] = pkIVelocities[ui] + (kDir *
                    (fStrength * (fCurrentTime -
                    pfILastUpdateTimes[ui])));
            }
        }
        else
        {
            // No decay, turbulence.
            float fTurbulenceScale = kInputData.m_fTurbulence *
                kInputData.m_fTurbulenceScale * 500.0f;

            NiPoint3 kTurbulence;
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                NiPoint3 kDir = kInputData.m_kPosition -
                    pkIPositions[ui];
                kDir.Unitize();

                kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;

                pkOVelocities[ui] = pkIVelocities[ui] +
                    ((kTurbulence + (kDir * fStrength)) *
                    (fCurrentTime - pfILastUpdateTimes[ui]));
            }
        }
    }
    else
    {
        if (kInputData.m_fTurbulence == 0.0f)
        {
            // Decay, no turbulence.
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                NiPoint3 kDir = kInputData.m_kPosition -
                    pkIPositions[ui];
                float fDist = kDir.Unitize();

                float fDecay = NiExp(-kInputData.m_fDecay * fDist);

                pkOVelocities[ui] = pkIVelocities[ui] + (kDir *
                    (fStrength * fDecay * (fCurrentTime -
                    pfILastUpdateTimes[ui])));
            }
        }
        else
        {
            // Decay, turbulence.
            float fTurbulenceScale = kInputData.m_fTurbulence *
                kInputData.m_fTurbulenceScale * 500.0f;

            NiPoint3 kTurbulence;
            for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
            {
                kTurbulence.x = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.y = NiSymmetricRandom() * fTurbulenceScale;
                kTurbulence.z = NiSymmetricRandom() * fTurbulenceScale;

                NiPoint3 kDir = kInputData.m_kPosition -
                    pkIPositions[ui];
                float fDist = kDir.Unitize();

                float fDecay = NiExp(-kInputData.m_fDecay * fDist);

                pkOVelocities[ui] = pkIVelocities[ui] +
                    ((kTurbulence + (kInputData.m_kDirection *
                    fStrength * fDecay)) * (fCurrentTime -
                    pfILastUpdateTimes[ui]));
            }
        }
    }
}
//---------------------------------------------------------------------------
#undef NI_MAY_INLINE
//---------------------------------------------------------------------------
