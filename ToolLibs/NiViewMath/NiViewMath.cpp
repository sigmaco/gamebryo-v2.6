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

#include "NiViewMath.h"

const float NiViewMath::PARALLEL_THRESHOLD = 0.99f;
const float NiViewMath::INV_PARALLEL_THRESHOLD = 1.0f - PARALLEL_THRESHOLD;

float NiViewMath::m_fOrthoZoomSpeed = 0.99f;
//---------------------------------------------------------------------------
NiPoint3 NiViewMath::Pan(const float fDX, const float fDY,
    const NiPoint3& kInputPoint, const NiMatrix3& kInputRotation)
{
    NiPoint3 kNewTranslation = NiPoint3(0.0f, fDY, -fDX);
    kNewTranslation = kInputRotation * kNewTranslation;
    return (kInputPoint + kNewTranslation);
}
//---------------------------------------------------------------------------
NiMatrix3 NiViewMath::Look(const float fDX, const float fDY,
    const NiMatrix3& kInputRotation, const NiPoint3& kUp)
{
    NiPoint3 kLook;
    kInputRotation.GetCol(0, kLook);

    // prevent from looking straight up/down; causes rapid orientation changes
    float fDeltaY = fDY;
    if (((kUp.Dot(kLook) > PARALLEL_THRESHOLD) && (fDY < 0.0f)) ||
        ((kUp.Dot(kLook) < -PARALLEL_THRESHOLD) && (fDY > 0.0f)))
    {
        fDeltaY = 0.0f;
    }

    NiPoint3 kLookOffset = NiPoint3(0.0f, -fDeltaY, fDX);
    kLookOffset = kInputRotation * kLookOffset;
    kLook += kLookOffset;
    kLook.Unitize();
    NiPoint3 kLookTangent = kLook.Cross(kUp);
    kLookTangent.Unitize();
    NiPoint3 kLookBiTangent = kLookTangent.Cross(kLook);

    return NiMatrix3(kLook, kLookBiTangent, kLookTangent);
}
//---------------------------------------------------------------------------
void NiViewMath::Orbit(const float fDX, const float fDY,
    const NiPoint3& kInputPoint, const NiMatrix3& kInputRotation,
    const NiPoint3& kCenter, const NiPoint3& kUp, NiPoint3& kReturnPoint,
    NiMatrix3& kReturnRotation)
{
    // figure out how far our look direction is from the orbit center
    NiPoint3 kCurrentLook;
    kInputRotation.GetCol(0, kCurrentLook);
    NiPoint3 kCenterOffset = ((kCenter - kInputPoint).Dot(kCurrentLook) * 
        kCurrentLook) - (kCenter - kInputPoint);

    // prevent from looking straight up/down
    float fDeltaY = fDY;
    if (((kUp.Dot(kCurrentLook) > PARALLEL_THRESHOLD) && (fDY < 0.0f)) ||
        ((kUp.Dot(kCurrentLook) < -PARALLEL_THRESHOLD) && (fDY > 0.0f)))
    {
        fDeltaY = 0.0f;
    }

    float fDistance = (kInputPoint - kCenter - kCenterOffset).Length();
    NiPoint3 kTranslationOffset = NiPoint3(fDistance, 0.0f, 0.0f);
    kCenterOffset = kInputRotation.Transpose() * kCenterOffset;
    NiMatrix3 kYRotation;
    kYRotation.MakeZRotation(fDeltaY);
    kTranslationOffset = kInputRotation * (kYRotation * kTranslationOffset);
    kCenterOffset = kInputRotation * (kYRotation * kCenterOffset);
    NiMatrix3 kXRotation;
    kXRotation.MakeZRotation(fDX);
    kTranslationOffset = kXRotation * kTranslationOffset;
    kCenterOffset = kXRotation * kCenterOffset;
    
    kReturnPoint = (kCenter - kTranslationOffset + kCenterOffset);

    NiPoint3 kLook = kTranslationOffset;
    kLook.Unitize();
    NiPoint3 kLookTangent = kLook.Cross(kUp);
    kLookTangent.Unitize();
    NiPoint3 kLookBiTangent = kLookTangent.Cross(kLook);
    kReturnRotation.SetCol(0, kLook);
    kReturnRotation.SetCol(1, kLookBiTangent);
    kReturnRotation.SetCol(2, kLookTangent);
}
//---------------------------------------------------------------------------
NiPoint3 NiViewMath::Dolly(const float fDZ, const NiPoint3& kInputPoint, 
    const NiMatrix3& kInputRotation)
{
    NiPoint3 kDirection;
    kInputRotation.GetCol(0, kDirection);
    return (kInputPoint + kDirection * fDZ);
}
//---------------------------------------------------------------------------
NiFrustum NiViewMath::OrthoZoom(const float fDZ,
    const NiFrustum& kInputFrustum)
{
    NiFrustum kReturnFrustum = kInputFrustum;
    float fScaleFactor = pow(m_fOrthoZoomSpeed, fDZ);
    kReturnFrustum.m_fLeft *= fScaleFactor;
    kReturnFrustum.m_fRight *= fScaleFactor;
    kReturnFrustum.m_fTop *= fScaleFactor;
    kReturnFrustum.m_fBottom *= fScaleFactor;
    return kReturnFrustum;
}
//---------------------------------------------------------------------------
NiMatrix3 NiViewMath::LookAt(const NiPoint3& kFocus, const NiPoint3& kSource, 
    const NiPoint3& kUp)
{
    NiPoint3 kLook = kFocus - kSource;
    kLook.Unitize();
    NiPoint3 kLookTangent = kLook.Cross(kUp);
    kLookTangent.Unitize();
    NiPoint3 kLookBiTangent = kLookTangent.Cross(kLook);

    return NiMatrix3(kLook, kLookBiTangent, kLookTangent);
}
//---------------------------------------------------------------------------
NiPoint3 NiViewMath::PanTo(const NiBound& kFocus,
    const NiMatrix3& kCurrentRotation, const NiFrustum& kFrustum)
{
    NiPoint3 kLook;
    kCurrentRotation.GetCol(0, kLook);
    float fFrustumEdge = (kFrustum.m_fRight > kFrustum.m_fTop) ? 
        kFrustum.m_fTop : kFrustum.m_fRight;
    float fDistanceToCenter = 2.0f * kFocus.GetRadius() / fFrustumEdge;

    return (kFocus.GetCenter() - fDistanceToCenter * kLook);
}
//---------------------------------------------------------------------------
void NiViewMath::MouseToRay(const float fX, const float fY, 
    const unsigned int uiAppWidth, const unsigned int uiAppHeight,
    const NiCamera* pkCamera, NiPoint3& kOrigin, NiPoint3& kDirection)
{
    float fUnitizedX = (fX / uiAppWidth) * 2.0f - 1.0f;
    float fUnitizedY = ((uiAppHeight - fY) / uiAppHeight) * 2.0f - 1.0f;
    fUnitizedX *= pkCamera->GetViewFrustum().m_fRight;
    fUnitizedY *= pkCamera->GetViewFrustum().m_fTop;

    NiMatrix3 kRotation = pkCamera->GetRotate();
    NiPoint3 kLook, kLookUp, kLookRight;
    kRotation.GetCol(0, kLook);
    kRotation.GetCol(1, kLookUp);
    kRotation.GetCol(2, kLookRight);

    if (pkCamera->GetViewFrustum().m_bOrtho)
    {
        kOrigin = pkCamera->GetWorldTranslate() + kLookRight * fUnitizedX + 
            kLookUp * fUnitizedY;
        kDirection = kLook;
    }
    else
    {
        kOrigin = pkCamera->GetWorldTranslate();
        kDirection = kLook + kLookUp * fUnitizedY + kLookRight * fUnitizedX;
        kDirection.Unitize();
    }
}
//---------------------------------------------------------------------------
NiPoint3 NiViewMath::TranslateOnAxis(const NiPoint3& kStartingPoint,
    const NiPoint3& kAxis, const NiPoint3& kInputOrigin,
    const NiPoint3& kInputDirection)
{
    // figure out the closest point between two 3d lines
    // one line is (entitylocation + m_eAxis * x)
    // the other is (kOrigin + kDir * y)
    // the closest pt on line 1 should be the new location of the entity
    NiPoint3 kP1, kP2;  // line origins
    NiPoint3 kD1, kD2;  // line directions
    NiPoint3 kDelta;    // line between origins

    kP1 = kStartingPoint;
    kP2 = kInputOrigin;
    kD1 = kAxis;
    kD2 = kInputDirection;
    kDelta = kP1 - kP2;

    float fDenominator = kD1.SqrLength() * kD2.SqrLength() - 
        kD2.Dot(kD1) * kD2.Dot(kD1);
    float fNumerator = kDelta.Dot(kD2) * kD2.Dot(kD1) - kDelta.Dot(kD1) * 
        kD2.Dot(kD2);
    // fSolution is the number of D1 to get to the closest point from P1
    float fSolution = fNumerator / fDenominator;

    return (kD1 * fSolution);
}
//---------------------------------------------------------------------------
NiPoint3 NiViewMath::TranslateOnPlane(const NiPoint3& kStartingPoint,
    const NiPoint3& kNormal, const NiPoint3& kInputOrigin,
    const NiPoint3& kInputDirection)
{
    // project the ray on to the plain, use the resulting point
    // find the component of delta in the direction of the plane
    float fDistance = (kStartingPoint - kInputOrigin).Dot(kNormal);
    // component of the direction vector with respect to normal
    float fCompDirN = kInputDirection.Dot(kNormal);
    NiPoint3 kProjectedPt = kInputOrigin + kInputDirection * 
        (fDistance / fCompDirN);
    // find the difference between starting location and projected pt
    return (kProjectedPt - kStartingPoint);
}
//---------------------------------------------------------------------------
float NiViewMath::RotateAboutAxis(const NiPoint3& kStartingPoint,
    const NiPoint3& kAxis, const NiPoint3& kTangent,
    const NiPoint3& kBiTangent, const NiPoint3& kInputOrigin,
    const NiPoint3& kInputDirection)
{
    // return the number of radians from the tangent in the direction of
    // the bi-tangent

    // determine if the input is above the horizon by making sure input 
    // origin and direction are in different directions relative to the axis
    float fAdotO = kAxis.Dot(kInputOrigin);
    float fAdotD = kAxis.Dot(kInputDirection);
    float fMultiplier = (fAdotO * fAdotD >= 0.0f) ? -1.0f : 1.0f;
    // project input onto a plane
    // make a vector from the projected pt - plane origin
    NiPoint3 kOffSet = TranslateOnPlane(kStartingPoint, kAxis, kInputOrigin, 
        kInputDirection);
    // if input is looking away from the horizon, invert offset
    kOffSet *= fMultiplier;
    // unitize this vector
    kOffSet.Unitize();
    // use its components to figure the radians
    float fTangentComponent = kOffSet.Dot(kTangent);
    float fBiTangentComponent = kOffSet.Dot(kBiTangent);
    // fReturn value is the radian rotation about the given axis
    // that the input is different from the tangent vector
    float fReturnValue = NiACos(fTangentComponent);
    if (fBiTangentComponent <= 0.0f)
    {
        fReturnValue = NI_TWO_PI - fReturnValue;
    }
    return fReturnValue;
}
//---------------------------------------------------------------------------
