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
#include "NiCollisionPCH.h"

#include <NiTransform.h>
#include "NiCapsuleBV.h"
#include "NiSphereBV.h"
#include "NiSqrDistance.h"
#include "NiTrigon.h"
#include "NiCapsuleCapsuleIntersector.h"
#include "NiCapsuleSphereIntersector.h"
#include "NiCapsuleTriIntersector.h"
#include "NiParallelogram.h"


class _CapsuleTriangleData  // Support for intersection of capsule & triangle.
{
public:
    NiSegment m_kSegment;
    NiPoint3 m_kVelocity;
    NiTrigon m_kTri;
    float m_fRSqr;
    float m_fS0;
    float m_fT0;
    float m_fT1;
};

//---------------------------------------------------------------------------
NiCapsuleBV::NiCapsuleBV(float fRadius, float fExtent, NiPoint3 kCenter, 
    NiPoint3 kAxis)
{
    m_kCenter = kCenter;
    m_kAxis = kAxis;
    m_fExtent = fExtent;
    m_kCapsule.m_fRadius = fRadius;

    CalculateCapsule();
}
//---------------------------------------------------------------------------
void NiCapsuleBV::Copy(const NiBoundingVolume& kABV)
{
    NiCapsuleBV& kCBV = (NiCapsuleBV&)kABV;

    m_kCenter = kCBV.GetCenter();
    m_kAxis = kCBV.GetAxis();
    m_fExtent = kCBV.GetExtent();
    m_kCapsule.m_fRadius = kCBV.GetRadius();

    CalculateCapsule();
}
//---------------------------------------------------------------------------
bool NiCapsuleBV::operator==(const NiBoundingVolume& kABV) const
{
    return kABV.Type() == CAPSULE_BV
        && ((NiCapsuleBV&)kABV).m_kCapsule == m_kCapsule;
}
//---------------------------------------------------------------------------
bool NiCapsuleBV::operator!=(const NiBoundingVolume& kABV) const
{
    return !operator==(kABV);
}
//---------------------------------------------------------------------------
void NiCapsuleBV::UpdateWorldData(const NiBoundingVolume& kModelABV, 
    const NiTransform &kWorld)
{
    const NiCapsuleBV& kModelCapABV = (const NiCapsuleBV&) kModelABV;

    // set the capsuleBV data
    m_kCenter = kWorld.m_fScale*(kWorld.m_Rotate*kModelCapABV.GetCenter()) 
        + kWorld.m_Translate;
    m_kAxis = kWorld.m_Rotate * kModelCapABV.GetAxis();
    m_fExtent = kWorld.m_fScale * kModelCapABV.GetExtent();
    m_kCapsule.m_fRadius = kWorld.m_fScale * kModelCapABV.GetRadius();

    // now set the capsule data
    CalculateCapsule();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'test' intersections
//---------------------------------------------------------------------------
bool NiCapsuleBV::CapsuleSphereTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiCapsuleBV& kCapABV = (const NiCapsuleBV&) kABV0;
    const NiSphereBV& kSphABV = (const NiSphereBV&) kABV1;

    // Perform calculations in the frame of the capsule by subtracting its
    // velocity from the sphere.  Thus, the capsule is stationary and the
    // sphere is moving.  The problem reduces to measuring distance from line
    // segment to line segment.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fSqrDist;
    float fS0;

    if (kDeltaV != NiPoint3::ZERO)
    {
        // Build line kSegment corresponding to region spanned through time
        // by the sphere.
        NiSegment kSegment;
        kSegment.m_kOrigin = kSphABV.GetCenter();
        kSegment.m_kDirection = fTime * kDeltaV;
        
        // compute squared distance from line kSegment to line kSegment
        float fS1;
        fSqrDist = NiSqrDistance::Compute(kSegment, kCapABV.GetSegment(), 
            fS0, fS1);
    }
    else
    {
        // compute squared distance from point to line kSegment
        fSqrDist = NiSqrDistance::Compute(kSphABV.GetCenter(), 
            kCapABV.GetSegment(), fS0);
    }

    // compare distances
    float fRadiusSum = kCapABV.GetRadius() + kSphABV.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;
    return fSqrDist <= fRadiusSumSqr;
}
//---------------------------------------------------------------------------
bool NiCapsuleBV::CapsuleCapsuleTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiCapsuleBV& CapABV0 = (const NiCapsuleBV&) kABV0;
    const NiCapsuleBV& CapABV1 = (const NiCapsuleBV&) kABV1;

    // Perform calculations in the frame of capsule0 by subtracting its
    // velocity from capsule1.  Thus, capsule0 is stationary and capsule1 is
    // moving.  The problem reduces to measuring distance from line kSegment
    // to parallelogram.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fSqrDist;
    float fS0;

    if (kDeltaV != NiPoint3::ZERO)
    {
        // Build parallelogram corresponding to region spanned through time
        // by capsule1.
        NiParallelogram kPgm;
        kPgm.m_kOrigin = CapABV1.GetOrigin();
        kPgm.m_kEdge0 = CapABV1.GetDirection();
        kPgm.m_kEdge1 = fTime * kDeltaV;
        
        // compute squared distance from line kSegment to parallelogram
        float fP0;
        float fP1;
        fSqrDist = NiSqrDistance::Compute(CapABV0.GetSegment(), kPgm, fS0, 
            fP0, fP1);
    }
    else
    {
        // compute squared distance from line kSegment to line kSegment
        float fS1;
        fSqrDist = NiSqrDistance::Compute(CapABV0.GetSegment(), 
            CapABV1.GetSegment(), fS0, fS1);
    }

    // compare distances
    float fRadiusSum = CapABV0.GetRadius() + CapABV1.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;
    return fSqrDist <= fRadiusSumSqr;
}
//---------------------------------------------------------------------------
bool NiCapsuleBV::CapsuleTriTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, 
    const NiPoint3& kVert2, const NiPoint3& kV1)
{
    const NiCapsuleBV& kCapABV = (const NiCapsuleBV&) kABV0;

    NiTrigon kTri;
    kTri.m_kOrigin = kVert0;
    kTri.m_kEdge0 = kVert1 - kVert0;
    kTri.m_kEdge1 = kVert2 - kVert0;

    // Perform calculations in the frame of the triangle by subtracting its
    // velocity from the capsule.  Thus, the triangle is stationary and the
    // capsule is moving.  The problem reduces to measuring distance from
    // parallelogram to triangle (if capsule moving relative to triangle) or
    // from line kSegment to triangle (if capsule stationary relative to
    // triangle).
    NiPoint3 kDeltaV = kV0 - kV1;


    // Quick out:  Test if capsule is moving away from plane of triangle.
    NiPoint3 kTriNormal = - kTri.m_kEdge0.Cross(kTri.m_kEdge1);

    // Project capsule center on triangle normal.
    float fNdotC = kTriNormal.Dot(kCapABV.GetCenter());

    // Project capsule "radius" in triangle normal direction.
    float PseudoRadius = ((kCapABV.GetExtent() > kCapABV.GetRadius()) ?
        (kCapABV.GetExtent()) : (kCapABV.GetRadius()));
    float fNormRadiusSqr = kTriNormal.SqrLength() * PseudoRadius
        * PseudoRadius;
    float fNdotT = kTriNormal.Dot(kVert0);

    // Calculate distance from capsule to triangle along the normal.
    float fDistCapTri = fNdotC - fNdotT;
    if (fDistCapTri * fDistCapTri > fNormRadiusSqr)
    {   // Capsule does not currently intersect the plane of the triangle.
        NiPoint3 kSurfacePt;
        kTriNormal.Unitize();
        if (fNdotC > fNdotT)    // Capsule on positive side of triangle.
        {
            if (kDeltaV.Dot(kTriNormal) >= 0.0f)
                return false;   // Quick out - moving away
            kSurfacePt = kCapABV.GetCenter() - PseudoRadius * kTriNormal;
        }
        else    // Capsule on negative side of triangle.
        {
            if (kDeltaV.Dot(kTriNormal) <= 0.0f)
                return false;   // Quick out - moving away
            kSurfacePt = kCapABV.GetCenter() + PseudoRadius * kTriNormal;
        }

        // Quick out - Beginning and ending "nearest point" are both "outside"
        // triangle plane.
        fNdotT = kTriNormal.Dot(kVert0); // Calculate with unitized tri normal.
        if (kTriNormal.Dot(kSurfacePt) > fNdotT + 1e-03f)
        {
            if (kTriNormal.Dot(kSurfacePt + fTime * kDeltaV)
                > fNdotT + 1e-03f)
            {
                return false;
            }
        }
    }


    float fSqrDist, fT0, fT1;

    if (kDeltaV != NiPoint3::ZERO)
    {
        NiParallelogram kPgm;
        kPgm.m_kOrigin = kCapABV.GetOrigin();
        kPgm.m_kEdge0 = kCapABV.GetDirection();
        kPgm.m_kEdge1 = fTime * kDeltaV;

        // Compute distance from parallelogram to triangle.
        float fP0, fP1;
        fSqrDist = NiSqrDistance::Compute(kTri, kPgm, fT0, fT1, fP0, fP1);
    }
    else
    {
        // Compute distance from line kSegment to triangle.
        float fS0;
        fSqrDist = NiSqrDistance::Compute(kCapABV.GetSegment(), kTri, fS0, 
            fT0, fT1);
    }

    return fSqrDist <= kCapABV.GetRadius() * kCapABV.GetRadius();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'find' intersections
//---------------------------------------------------------------------------
bool NiCapsuleBV::CapsuleSphereFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals, 
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    NiCapsuleSphereIntersector kIntersector((const NiCapsuleBV&)kABV0,
        (const NiSphereBV&)kABV1, fTime);

    kIntersector.Find(kV0, kV1);

    fIntrTime = kIntersector.GetContactTime();

    if (kIntersector.GetIntersectionType() == NiIntersector::IT_OVERLAP ||
        kIntersector.GetIntersectionType() == NiIntersector::IT_CONTACT)
    {
        kIntrPt = kIntersector.GetContactPoint();
        if (bCalcNormals)
        {
            kNormal0 = kIntersector.GetContactNormal();
            kNormal1 = -kNormal0;
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiCapsuleBV::CapsuleCapsuleFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals, 
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    NiCapsuleCapsuleIntersector kIntersector((const NiCapsuleBV&)kABV0,
        (const NiCapsuleBV&)kABV1, fTime);

    kIntersector.Find(kV0, kV1);

    fIntrTime = kIntersector.GetContactTime();

    if (kIntersector.GetIntersectionType() == NiIntersector::IT_OVERLAP ||
        kIntersector.GetIntersectionType() == NiIntersector::IT_CONTACT)
    {
        kIntrPt = kIntersector.GetContactPoint();
        if (bCalcNormals)
        {
            kNormal0 = kIntersector.GetContactNormal();
            kNormal1 = -kNormal0;
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiCapsuleBV::CapsuleTriFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, const NiPoint3& kVert2, 
    const NiPoint3& kV1, float& fIntrTime, NiPoint3& kIntrPt, 
    bool bCalcNormals, NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    // -----------------------------------------------------------------------
    // Use several plane-oriented "quick outs" to avoid the expensive
    // intersection calculations.  These quick outs are all the more important
    // since ABV-triangle intersections are really a part of ABV-TriangleMesh
    // calculations, and most triangles in the TriangleMesh will not intersect
    // the ABV (assuming no initial interpenetration).

    const NiCapsuleBV& kCapABV = (const NiCapsuleBV&) kABV0;

    _CapsuleTriangleData kData;
    kData.m_kSegment = kCapABV.GetSegment();
    kData.m_fRSqr = kCapABV.GetRadius() * kCapABV.GetRadius();
    kData.m_kTri.m_kOrigin = kVert0;
    kData.m_kTri.m_kEdge0 = kVert1 - kVert0;
    kData.m_kTri.m_kEdge1 = kVert2 - kVert0;

    float fF0 = NiSqrDistance::Compute(kData.m_kSegment, kData.m_kTri, 
        kData.m_fS0, kData.m_fT0, kData.m_fT1) - kData.m_fRSqr;
    if (fF0 <= 0.0f)
    {
        // Capsule is already intersecting triangle.  The closest point to
        // the capsule kSegment is used as the intersection point.
        fIntrTime = 0.0f;
        kIntrPt = kData.m_kTri.m_kOrigin + kData.m_fT0 
            * kData.m_kTri.m_kEdge0 + kData.m_fT1 * kData.m_kTri.m_kEdge1;

        if (bCalcNormals)
        {
            kNormal1 = kData.m_kTri.GetNormal();
            kNormal0 = -kNormal1;
        }

        return true;
    }

    // Perform calculations in the frame of the triangle by subtracting its
    // velocity from the capsule.  Thus, the triangle is stationary and the
    // capsule is moving.
    const float fEpsilon = 1e-05f;
    kData.m_kVelocity = kV0 - kV1;
 

    // Quick out:  If capsule is not initially intersecting triangle and is
    // not moving,  no intersection occurs.
    if ((kData.m_kVelocity.SqrLength() * fTime) <= fEpsilon)
        return false;

    // Quick out:  Test if capsule is moving away from plane of triangle.
    NiPoint3 kTriNormal
        = - kData.m_kTri.m_kEdge0.Cross(kData.m_kTri.m_kEdge1);

    // Project capsule center on triangle normal.
    float fNdotC = kTriNormal.Dot(kCapABV.GetCenter());

    // Project capsule "radius" in triangle normal direction.
    float PseudoRadius = ((kCapABV.GetExtent() > kCapABV.GetRadius()) ?
        (kCapABV.GetExtent()) : (kCapABV.GetRadius()));
    float fNormRadiusSqr = kTriNormal.SqrLength() * PseudoRadius
        * PseudoRadius;
    float fNdotT = kTriNormal.Dot(kVert0);

    // Calculate distance from capsule to triangle along the normal.
    float fDistCapTri = fNdotC - fNdotT;
    if (fDistCapTri * fDistCapTri > fNormRadiusSqr)
    {   // Capsule does not currently intersect the plane of the triangle.
        NiPoint3 kSurfacePt;
        kTriNormal.Unitize();
        if (fNdotC > fNdotT)    // Capsule on positive side of triangle.
        {
            if (kData.m_kVelocity.Dot(kTriNormal) >= 0.0f)
                return false;   // Quick out - moving away
            kSurfacePt = kCapABV.GetCenter() - PseudoRadius * kTriNormal;
        }
        else    // Capsule on negative side of triangle.
        {
            if (kData.m_kVelocity.Dot(kTriNormal) <= 0.0f)
                return false;   // Quick out - moving away
            kSurfacePt = kCapABV.GetCenter() + PseudoRadius * kTriNormal;
        }

        // Quick out - Beginning and ending "nearest point" are both "outside"
        // triangle plane.
        fNdotT = kTriNormal.Dot(kVert0); // Calculate with unitized tri normal.
        if (kTriNormal.Dot(kSurfacePt) > fNdotT + 1e-03f)
        {
            if (kTriNormal.Dot(kSurfacePt + fTime * kData.m_kVelocity)
                > fNdotT + 1e-03f)
            {
                return false;
            }
        }
    }

    // End of "quick outs".
    // -----------------------------------------------------------------------

    NiCapsuleTriIntersector kIntersector((const NiCapsuleBV&)kABV0,
        kVert0, kVert1, kVert2, fTime);

    kIntersector.Find(kV0, kV1);

    fIntrTime = kIntersector.GetContactTime();

    if (kIntersector.GetIntersectionType() == NiIntersector::IT_OVERLAP ||
        kIntersector.GetIntersectionType() == NiIntersector::IT_CONTACT)
    {
        kIntrPt = kIntersector.GetContactPoint();
        if (bCalcNormals)
        {
            kNormal0 = kIntersector.GetContactNormal();
            kNormal1 = -kNormal0;
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiBoundingVolume* NiCapsuleBV::Clone() const
{
    NiCapsuleBV* pClone = NiNew NiCapsuleBV(*this);
    NIASSERT(pClone);
    return (NiBoundingVolume*) pClone;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiBoundingVolume* NiCapsuleBV::CreateFromStream(NiStream& stream)
{
    NiCapsuleBV* pBV = NiNew NiCapsuleBV;
    NIASSERT(pBV);
    pBV->LoadBinary(stream);
    return pBV;
}
//---------------------------------------------------------------------------
void NiCapsuleBV::LoadBinary(NiStream& stream)
{
    NiBoundingVolume::LoadBinary(stream);
    m_kCenter.LoadBinary(stream);
    m_kAxis.LoadBinary(stream);
    NiStreamLoadBinary(stream, m_fExtent);
    NiStreamLoadBinary(stream, m_kCapsule.m_fRadius);

    CalculateCapsule();
}
//---------------------------------------------------------------------------
void NiCapsuleBV::SaveBinary(NiStream& stream)
{
    NiBoundingVolume::SaveBinary(stream);
    m_kCenter.SaveBinary(stream);
    m_kAxis.SaveBinary(stream);
    NiStreamSaveBinary(stream, m_fExtent);
    NiStreamSaveBinary(stream, m_kCapsule.m_fRadius);
}
//---------------------------------------------------------------------------
void NiCapsuleBV::AddViewerStrings(const char* pcPrefix, 
    NiViewerStringsArray* pkStrings) const
{
    size_t stLen = strlen(pcPrefix) + 14;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "%s = CAPSULE_BV", pcPrefix);
    pkStrings->Add(pString);
}
//---------------------------------------------------------------------------
