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
#include "NiSegment.h"
#include "NiSphereBV.h"
#include "NiSqrDistance.h"
#include "NiTrigon.h"
#include "NiSphereTriIntersector.h"


class _SphereTriangleData   // Support for intersection of sphere & triangle.
{
public:
    NiPoint3 m_kCenter;
    NiPoint3 m_kVelocity;
    NiTrigon m_kTri;
    float m_fRSqr;
    float m_fT0;
    float m_fT1;
};

//---------------------------------------------------------------------------
NiSphereBV::NiSphereBV(const float fRadius, const NiPoint3& kCenter)
{
    m_kSphere.m_kCenter = kCenter;
    m_kSphere.m_fRadius = fRadius;
}
//---------------------------------------------------------------------------
void NiSphereBV::Copy(const NiBoundingVolume& kABV)
{
    NiSphereBV& kSBV = (NiSphereBV&)kABV;

    SetSphere(kSBV.GetSphere());
}
//---------------------------------------------------------------------------
bool NiSphereBV::operator==(const NiBoundingVolume& kABV) const
{
    return kABV.Type() == SPHERE_BV && 
        ((NiSphereBV&)kABV).m_kSphere == m_kSphere;
}
//---------------------------------------------------------------------------
bool NiSphereBV::operator!=(const NiBoundingVolume& kABV) const
{
    return !operator==(kABV);
}
//---------------------------------------------------------------------------
void NiSphereBV::UpdateWorldData(const NiBoundingVolume& kModelABV, 
    const NiTransform &kWorld)
{
    const NiSphereBV& kModelSphABV = (const NiSphereBV&) kModelABV;

    SetCenter(kWorld.m_fScale*(kWorld.m_Rotate*kModelSphABV.GetCenter()) 
        + kWorld.m_Translate);
    SetRadius(kWorld.m_fScale*kModelSphABV.GetRadius());   
}
//---------------------------------------------------------------------------
// 'test' intersections
//---------------------------------------------------------------------------
bool NiSphereBV::SphereSphereTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiSphereBV& kSphABV0 = (const NiSphereBV&) kABV0;
    const NiSphereBV& kSphABV1 = (const NiSphereBV&) kABV1;

    NiPoint3 kDeltaV = kV1 - kV0;
    float fA = kDeltaV.SqrLength();
    NiPoint3 kDeltaC = kSphABV1.GetCenter() - kSphABV0.GetCenter();
    float fC = kDeltaC.SqrLength();
    float fRadiusSum = kSphABV0.GetRadius() + kSphABV1.GetRadius();
    float fRadiusSumSqr = fRadiusSum*fRadiusSum;

    if (fA > 0.0f)
    {
        float fB = kDeltaC.Dot(kDeltaV);
        if (fB <= 0.0f)
        {
            if (-fTime * fA <= fB)
                return fA * fC - fB * fB <= fA * fRadiusSumSqr;
            else
                return (fTime * (fTime * fA + 2.0f * fB) + fC 
                    <= fRadiusSumSqr);
        }
    }

    return fC <= fRadiusSumSqr;
}
//---------------------------------------------------------------------------
bool NiSphereBV::SphereTriTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, 
    const NiPoint3& kVert2, const NiPoint3& kV1)
{
    const NiSphereBV& kSphABV = (const NiSphereBV&) kABV0;

    float fT0, fT1, fSqrDist;
    NiTrigon kTri;
    kTri.m_kOrigin = kVert0;
    kTri.m_kEdge0 = kVert1 - kVert0;
    kTri.m_kEdge1 = kVert2 - kVert0;

    // Perform calculations in the frame of the triangle by subtracting its
    // velocity from the sphere.  Thus, the triangle is stationary and the
    // sphere is moving.  The problem reduces to measuring distance from line
    // segment to triangle (if sphere moving relative to triangle) or from
    // point to triangle (if sphere stationary relative to triangle).
    NiPoint3 kDeltaV = kV0 - kV1;

    if (kDeltaV != NiPoint3::ZERO)
    {
        float fS0;
        NiSegment kSeg;
        kSeg.m_kOrigin = kSphABV.GetCenter();
        kSeg.m_kDirection = fTime * kDeltaV;

        // Compute distance from line segment to triangle.
        fSqrDist = NiSqrDistance::Compute(kSeg, kTri, fS0, fT0, fT1);
    }
    else
    {
        // Compute distance from point to triangle.
        fSqrDist = NiSqrDistance::Compute(kSphABV.GetCenter(), 
            kTri, fT0, fT1);
    }

    return fSqrDist <= kSphABV.GetRadius() * kSphABV.GetRadius();
}
//---------------------------------------------------------------------------
// 'find' intersections
//---------------------------------------------------------------------------
bool NiSphereBV::SphereSphereFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals, 
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    const NiSphereBV& kSphABV0 = (const NiSphereBV&) kABV0;
    const NiSphereBV& kSphABV1 = (const NiSphereBV&) kABV1;

    // compute the theoretical first time and point of contact
    NiPoint3 kDeltaV = kV1 - kV0;
    float fA = kDeltaV.SqrLength();
    NiPoint3 kDeltaC = kSphABV1.GetCenter() - kSphABV0.GetCenter();
    float fC = kDeltaC.SqrLength();
    float fRadiusSum = kSphABV0.GetRadius() + kSphABV1.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;
    const float fEpsilon = 1e-05f;

    if (fA > 0.0f)
    {
        float fB = kDeltaC.Dot(kDeltaV);
        if (fB <= 0.0f)
        {
            if (-fTime * fA <= fB
                || fTime * (fTime * fA + 2.0f * fB) + fC <= 
                (fRadiusSumSqr + fEpsilon))
            {
                float fCDiff = fC - fRadiusSumSqr;
                float fDiscr = fB * fB - fA * fCDiff;
                if (fDiscr >= 0.0f)
                {
                    if (fCDiff <= 0.0f)
                    {
                        // Spheres are already intersecting.  Use midpoint of
                        // segment connecting centers of ABVs.
                        fIntrTime = 0.0f;
                        kIntrPt = 0.5f *
                            (kSphABV0.GetCenter() + kSphABV1.GetCenter());

                        if ( bCalcNormals )
                        {
                            if (fC > FLT_EPSILON)
                            {
                              kNormal0 = kDeltaC;
                              kNormal0.Unitize();
                            }
                            else
                            {
                              kNormal0 = NiPoint3::UNIT_X;
                            }

                            kNormal1 = -kNormal0;
                        }

                        return true;
                    }
                    else
                    {
                        // first time of contact is in [0, tmin]
                        fIntrTime = -(fB+NiSqrt(fDiscr))/fA;

                        // clamp to handle floating point round-off errors
                        if (fIntrTime < 0.0f)
                            fIntrTime = 0.0f;
                        else if (fIntrTime > fTime)
                            fIntrTime = fTime;

                        NiPoint3 kCenDiff = kDeltaC + fIntrTime * kDeltaV;

                        kIntrPt = kSphABV0.GetCenter() + fIntrTime * kV0 +
                            (kSphABV0.GetRadius() / fRadiusSum) * kCenDiff;

                        if (bCalcNormals)
                        {
                            kNormal0 = kCenDiff;
                            kNormal0.Unitize();
                            kNormal1 = -kNormal0;
                        }
                    }
                    return true;
                }
            }
            return false;
        }
    }

    if (fC <= fRadiusSumSqr)
    {
        // The spheres are initially intersecting.  Choose the contact point
        // to be a point on the line segment connecting the sphere centers.
        // The point is on the fastest moving sphere.
        fIntrTime = 0.0f;

        if (kV1.SqrLength() > kV0.SqrLength())
        {
            kNormal0 = kSphABV1.GetCenter() - kSphABV0.GetCenter();
            kNormal0.Unitize();
            kIntrPt = kSphABV0.GetCenter() + kNormal0 * kSphABV0.GetRadius();
            kNormal1 = -kNormal0;
        }
        else
        {
            kNormal1 = kSphABV0.GetCenter() - kSphABV1.GetCenter();
            kNormal1.Unitize();
            kIntrPt = kSphABV1.GetCenter() + kNormal1 * kSphABV1.GetRadius();
            kNormal0 = -kNormal1;
        }

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiSphereBV::SphereTriFindIntersect(float fTime, 
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

    const NiSphereBV& kSphABV = (const NiSphereBV&) kABV0;

    _SphereTriangleData kData;
    kData.m_kCenter = kSphABV.GetCenter();
    kData.m_fRSqr = kSphABV.GetRadius() * kSphABV.GetRadius();
    kData.m_kTri.m_kOrigin = kVert0;
    kData.m_kTri.m_kEdge0 = kVert1 - kVert0;
    kData.m_kTri.m_kEdge1 = kVert2 - kVert0;

    float fF0 = NiSqrDistance::Compute(kData.m_kCenter, kData.m_kTri, 
        kData.m_fT0, kData.m_fT1) - kData.m_fRSqr;
    if (fF0 <= 0.0f)
    {
        // Sphere is already intersecting triangle.  The closest point to
        // the sphere kCenter is used as the intersection point.
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
    // velocity from the sphere.  Thus, the triangle is stationary and the
    // sphere is moving.
    const float fEpsilon = 1e-05f;
    kData.m_kVelocity = kV0 - kV1;

    
    // If Sphere is not initially intersecting triangle and is not moving,
    // no intersection occurs.
    if ((kData.m_kVelocity.SqrLength() * fTime) <= fEpsilon)
        return false;

    // Test if sphere is moving away from plane of triangle.
    NiPoint3 kTriNormal
        = - kData.m_kTri.m_kEdge0.Cross(kData.m_kTri.m_kEdge1);

    // Project sphere center on triangle normal.
    float fNdotC = kTriNormal.Dot(kData.m_kCenter);

    // Project sphere radius in triangle normal direction.
    float fNormRadiusSqr = kTriNormal.SqrLength() * kData.m_fRSqr;
    float fNdotT = kTriNormal.Dot(kVert0);

    // Calculate distance from sphere to triangle along the normal.
    float fDistSphTri = fNdotC - fNdotT;
    if (fDistSphTri * fDistSphTri > fNormRadiusSqr)
    {   // Sphere currently does not intersect the plane of the triangle.
        NiPoint3 kSurfacePt;
        kTriNormal.Unitize();
        if (fNdotC > fNdotT)    // Sphere on positive side of triangle.
        {
            if (kData.m_kVelocity.Dot(kTriNormal) >= 0.0f)
                return false;   // Quick out - moving away
            kSurfacePt = kData.m_kCenter - kSphABV.GetRadius() * kTriNormal;
        }
        else    // Sphere on negative side of triangle.
        {
            if (kData.m_kVelocity.Dot(kTriNormal) <= 0.0f)
                return false;   // Quick out - moving away
            kSurfacePt = kData.m_kCenter + kSphABV.GetRadius() * kTriNormal;
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

    NiSphereTriIntersector kIntersector((const NiSphereBV&)kABV0,
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
NiBoundingVolume* NiSphereBV::Clone() const
{
    NiSphereBV* pkClone = NiNew NiSphereBV(*this);
    NIASSERT(pkClone);
    return (NiBoundingVolume*) pkClone;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiBoundingVolume* NiSphereBV::ConvertLozengeToSphereBoundingVolume(
    NiStream& kStream)
{
    NiPoint3 kCenter, kAxis0, kAxis1;
    float fExtent0, fExtent1, fRadius;

    // Read in defunct data
    kCenter.LoadBinary(kStream); 
    kAxis0.LoadBinary(kStream); 
    kAxis1.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, fExtent0); 
    NiStreamLoadBinary(kStream, fExtent1); 
    NiStreamLoadBinary(kStream, fRadius); 

    return (NiBoundingVolume*) NiNew NiSphereBV(fRadius, kCenter);
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiSphereBV::CreateFromStream(NiStream& kStream)
{
    NiSphereBV* pkSphABV = NiNew NiSphereBV;
    NIASSERT(pkSphABV);
    pkSphABV->LoadBinary(kStream);
    return pkSphABV;
}
//---------------------------------------------------------------------------
void NiSphereBV::LoadBinary(NiStream& kStream)
{
    NiBoundingVolume::LoadBinary(kStream);
    m_kSphere.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiSphereBV::SaveBinary(NiStream& kStream)
{
    NiBoundingVolume::SaveBinary(kStream);
    m_kSphere.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
void NiSphereBV::AddViewerStrings(const char* pcPrefix, 
    NiViewerStringsArray* pkStrings) const
{
    size_t stLen = strlen(pcPrefix) + 13;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "%s = SPHERE_BV", pcPrefix);
    pkStrings->Add(pString);

    pkStrings->Add(GetCenter().GetViewerString("     center"));

    stLen = strlen(pcPrefix) + 30;
    pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "     radius = %g", 
        (int)(GetRadius()*100) / 100.0f);
    pkStrings->Add(pString);
}
//---------------------------------------------------------------------------
