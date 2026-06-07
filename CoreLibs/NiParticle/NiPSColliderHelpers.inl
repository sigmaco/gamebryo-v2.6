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

#include "NiPSKernelDefinitions.h"
#include "NiPSFlagsHelpers.h"
#include "NiOS.h"

//---------------------------------------------------------------------------
NI_FORCEINLINE bool NiPSColliderHelpers::DetectPlanarCollision(
    float& fCollisionTime,
    NiPoint3& kCollisionPoint,
    const float fInitialTime,
    const NiPoint3& kParticlePosition,
    const NiPoint3& kParticleVelocity,
    const NiPSColliderDefinitions::PlanarColliderData& kInputData)
{
    bool bCollision = false;

    float fDistToPlane = kInputData.m_kPlane.Distance(kParticlePosition);

    static const float sfCollisionEpsilon = NiPow(2.0f, -21.0f);

    // Early out if the particle is heading away from the plane
    if (((kParticleVelocity * kInputData.m_kPlane.GetNormal()) *
        fDistToPlane) > 0.0f)
    {
        return false;
    }

    if (NiAbs(fDistToPlane) < sfCollisionEpsilon)
    {
        // Make sure collision point is inside rectangle
        NiPoint3 kDisplacementFromCenter = kParticlePosition -
            kInputData.m_kColliderPosition;
        NiPoint3 kXComponent = (kInputData.m_kRotatedXAxis *
            kDisplacementFromCenter) * kInputData.m_kRotatedXAxis;
        NiPoint3 kYComponent = (kInputData.m_kRotatedYAxis *
            kDisplacementFromCenter) * kInputData.m_kRotatedYAxis;

        if (kXComponent.SqrLength() < kInputData.m_fHalfSqScaledWidth &&
            kYComponent.SqrLength() < kInputData.m_fHalfSqScaledHeight)
        {
            // Initially colliding with plane.
            fCollisionTime = fInitialTime;
            kCollisionPoint = kParticlePosition;
            bCollision = true;
        }
    }
    else
    {
        float fDeltaTime = fCollisionTime - fInitialTime;
        NiPoint3 kTravelDisplacement = fDeltaTime * kParticleVelocity;
        float fTravelDistAlongNormal = kInputData.m_kPlane.GetNormal() * 
            kTravelDisplacement;
        float fSum = fDistToPlane + fTravelDistAlongNormal;
        if (NiAbs(fSum) < sfCollisionEpsilon)
        {
            fSum = 0.0f;
        }
        if (fDistToPlane * fSum < 0.0f)
        {
            // Collision with plane - test rectangle.
            float fTimeToPlane = -fDistToPlane * fDeltaTime / 
                fTravelDistAlongNormal;

            NiPoint3 kNewCollisionPoint = kParticlePosition + fTimeToPlane *
                kParticleVelocity;

            // Make sure collision point is on same side of plane.  Since we
            // are on the backside of the plane due to floating point 
            // precision issues, we scale by 2 ^ -21.  This gives us
            // significant precision since IEEE 754 has 23 bits of mantissa
            // but also gives a small amount of error tolerance for
            // non-standard implementations.
            if (kInputData.m_kPlane.Distance(kNewCollisionPoint) *
                fDistToPlane < 0.0f)
            {
                float fMaxValue = NiAbs(kNewCollisionPoint.x);
                if (NiAbs(kNewCollisionPoint.y) > fMaxValue)
                {
                    fMaxValue = NiAbs(kNewCollisionPoint.y);
                }
                if (NiAbs(kNewCollisionPoint.z) > fMaxValue)
                {
                    fMaxValue = NiAbs(kNewCollisionPoint.z);
                }

                if (fDistToPlane < 0.0f)
                {
                    kNewCollisionPoint -= fMaxValue * sfCollisionEpsilon *
                        kInputData.m_kPlane.GetNormal();
                }
                else
                {
                    kNewCollisionPoint += fMaxValue * sfCollisionEpsilon *
                        kInputData.m_kPlane.GetNormal();
                }
            }

            // Make sure collision point is inside rectangle
            NiPoint3 kDisplacementFromCenter = kNewCollisionPoint -
                kInputData.m_kColliderPosition;
            NiPoint3 kXComponent = (kInputData.m_kRotatedXAxis *
                kDisplacementFromCenter) * kInputData.m_kRotatedXAxis;
            NiPoint3 kYComponent = (kInputData.m_kRotatedYAxis *
                kDisplacementFromCenter) * kInputData.m_kRotatedYAxis;
            if (kXComponent.SqrLength() < kInputData.m_fHalfSqScaledWidth &&
                kYComponent.SqrLength() < kInputData.m_fHalfSqScaledHeight)
            {
                fCollisionTime = fInitialTime + fTimeToPlane;
                kCollisionPoint = kNewCollisionPoint;
                bCollision = true;
            }
        }
    }

    return bCollision;
}
//---------------------------------------------------------------------------
NI_FORCEINLINE bool NiPSColliderHelpers::DetectSphericalCollision(
    float& fCollisionTime,
    NiPoint3& kCollisionPoint,
    const float fInitialTime,
    const NiPoint3& kParticlePosition,
    const NiPoint3& kParticleVelocity,
    const NiPSColliderDefinitions::SphericalColliderData& kInputData)
{
    bool bCollision = false;
 
    // Check for initial position
    NiPoint3 kCurrentDisplacement = kParticlePosition -
        kInputData.m_kColliderPosition;
    float fCurrentDistSq = kCurrentDisplacement.SqrLength();
    float fDiff = fCurrentDistSq - kInputData.m_fRadiusSq;
    bool bInsideSphere = -fDiff > NIPSKERNEL_EPSILON;
    if (bInsideSphere || fDiff > NIPSKERNEL_EPSILON)
    {
        // Initially inside or outside sphere
        float fTime = -(kParticleVelocity * kCurrentDisplacement);

        if (bInsideSphere || fTime > 0)
        {
            float fSpeedSq = kParticleVelocity.SqrLength();
            fTime /= fSpeedSq;

            NiPoint3 kClosestDisplacement = kParticlePosition + 
                fTime * kParticleVelocity - kInputData.m_kColliderPosition;

            float fClosestDistSq = kClosestDisplacement.SqrLength();
            if (bInsideSphere || fClosestDistSq < kInputData.m_fRadiusSq)
            {
                float fHalfChordDistSq = kInputData.m_fRadiusSq -
                    fClosestDistSq;

                if (bInsideSphere)
                {
                    fTime += NiSqrt(fHalfChordDistSq / fSpeedSq);
                }
                else
                {
                    fTime -= NiSqrt(fHalfChordDistSq / fSpeedSq);
                }

                float fDt = fCollisionTime - fInitialTime;
                if (fTime < fDt)
                {
                    fCollisionTime = fInitialTime + fTime;
                    kCollisionPoint = kParticlePosition + fTime *
                        kParticleVelocity;
                    bCollision = true;
                }
            }
        }
    }

    return bCollision;
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSColliderHelpers::HandleGeneralCollision(
    NiPoint3& kOParticlePosition,
    NiUInt32& uiOParticleFlags,
    float& fOLastUpdateTime,
    const float fCollisionTime,
    const NiPoint3& kCollisionPoint,
    const NiUInt32 uiIParticleFlags,
    const float,
    const NiPSColliderDefinitions::ColliderData& kInputData)
{
    // Update particle position.
    kOParticlePosition = kCollisionPoint;

    // Copy particle flags.
    uiOParticleFlags = uiIParticleFlags;

    // We have updated this particle up to the collision point.
    fOLastUpdateTime = fCollisionTime;

    // Handle spawning.
    if (kInputData.m_bSpawnOnCollide)
    {
        // Set particle flags for spawning.
        NiPSFlagsHelpers::SetShouldSpawn(uiOParticleFlags, true);
        NiPSFlagsHelpers::SetSpawnerID(uiOParticleFlags,
            kInputData.m_ucSpawnerID);
    }

    // Handle dying.
    if (kInputData.m_bDieOnCollide)
    {
        // Set particle flags for death.
        NiPSFlagsHelpers::SetShouldDie(uiOParticleFlags, true);
    }
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSColliderHelpers::HandlePlanarCollision(
    NiPoint3& kOParticlePosition,
    NiPoint3& kOParticleVelocity,
    NiUInt32& uiOParticleFlags,
    float& fOLastUpdateTime,
    const float fCollisionTime,
    const NiPoint3& kCollisionPoint,
    const NiPoint3& kIParticleVelocity,
    const NiUInt32 uiIParticleFlags,
    const float fILastUpdateTime,
    const NiPSColliderDefinitions::PlanarColliderData& kInputData,
    const float fCurrentTime)
{
    // Update velocity.
    float fDot = kIParticleVelocity * kInputData.m_kPlane.GetNormal();
    NiPoint3 kOffset = fDot * kInputData.m_kPlane.GetNormal();

    // If initially colliding, check velocity in direction of normal.
    // If it's too low, eliminate the velocity in that direction.
    if (fCollisionTime == fCurrentTime && fDot < NIPSKERNEL_EPSILON)
    {
        kOParticleVelocity = kIParticleVelocity - kOffset;
    }
    else
    {
        kOParticleVelocity = kIParticleVelocity - (kOffset + kOffset);
        kOParticleVelocity *= kInputData.m_fBounce;
    }

    HandleGeneralCollision(
        kOParticlePosition,
        uiOParticleFlags,
        fOLastUpdateTime,
        fCollisionTime,
        kCollisionPoint,
        uiIParticleFlags,
        fILastUpdateTime,
        kInputData);
}
//---------------------------------------------------------------------------
NI_FORCEINLINE void NiPSColliderHelpers::HandleSphericalCollision(
    NiPoint3& kOParticlePosition,
    NiPoint3& kOParticleVelocity,
    NiUInt32& uiOParticleFlags,
    float& fOLastUpdateTime,
    const float fCollisionTime,
    const NiPoint3& kCollisionPoint,
    const NiPoint3& kIParticleVelocity,
    const NiUInt32 uiIParticleFlags,
    const float fILastUpdateTime,
    const NiPSColliderDefinitions::SphericalColliderData& kInputData,
    const float)
{
    // Update velocity.
    NiPoint3 kNormal = (kCollisionPoint - kInputData.m_kColliderPosition) /
        kInputData.m_fScaledRadius;

    float fDot = kIParticleVelocity * kNormal;
    NiPoint3 kOffset = fDot * kNormal;
    kOParticleVelocity = kIParticleVelocity - (kOffset + kOffset);
    kOParticleVelocity *= kInputData.m_fBounce;

    HandleGeneralCollision(
        kOParticlePosition,
        uiOParticleFlags,
        fOLastUpdateTime,
        fCollisionTime,
        kCollisionPoint,
        uiIParticleFlags,
        fILastUpdateTime,
        kInputData);
}
//---------------------------------------------------------------------------
