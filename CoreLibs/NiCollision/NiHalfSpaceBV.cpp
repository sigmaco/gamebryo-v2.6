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

#include "NiBoxBV.h"
#include "NiCapsuleBV.h"
#include "NiHalfSpaceBV.h"
#include "NiSphereBV.h"
#include "NiSqrDistance.h"
#include "NiUnionBV.h"
#include <NiMath.h>
#include <NiMatrix3.h>
#include <NiTransform.h>

//---------------------------------------------------------------------------
void NiHalfSpaceBV::SetPlane(const NiPlane& kPlane)
{
    NiPoint3 kNormal = kPlane.GetNormal();
    float fConstant = kPlane.GetConstant();
    float fLength = kNormal.Unitize();
    NIASSERT(fLength > 0.0f);
    fConstant /= fLength;
    m_kPlane.SetNormal(kNormal);
    m_kPlane.SetConstant(fConstant);
}
//---------------------------------------------------------------------------
void NiHalfSpaceBV::Copy(const NiBoundingVolume& kABV)
{
    NiHalfSpaceBV& kHBV = (NiHalfSpaceBV&)kABV;

    SetPlane(kHBV.GetPlane());
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::operator==(const NiBoundingVolume& kABV) const
{
    return kABV.Type() == HALFSPACE_BV
        && ((NiHalfSpaceBV&)kABV).m_kPlane == m_kPlane;
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::operator!=(const NiBoundingVolume& kABV) const
{
    return !operator==(kABV);
}
//---------------------------------------------------------------------------
void NiHalfSpaceBV::UpdateWorldData(const NiBoundingVolume& kModelABV, 
    const NiTransform &kWorld)
{
    const NiHalfSpaceBV& kModelHlfABV = (const NiHalfSpaceBV&) kModelABV;

    // The model kPlane has kNormal N0, constant C0 and is given by
    // Dot(N0, X) = C0.  If Y = s*R*X+T where s is kWorld scale, R is kWorld
    // rotation, and T is kWorld translation for current node, then
    // X = (1/s)*R^t*(Y-T) and
    //     C0 = Dot(N0, X) = Dot(N0, (1/s)*R^t*(Y-T)) = (1/s)*Dot(R*N0, Y-T)
    // so
    //     Dot(R*N0, Y) = s*c0+Dot(R*N0, T)
    // The kWorld kPlane has
    //     kNormal N1 = R*N0
    //     constant C1 = s*C0+Dot(R*N0, T) = s*C0+Dot(N1, T)

    NiPoint3 kNormal = kWorld.m_Rotate * kModelHlfABV.GetPlane().GetNormal();
    float fConstant = kWorld.m_fScale * kModelHlfABV.GetPlane().GetConstant()
        + kNormal * kWorld.m_Translate;
    m_kPlane.SetNormal(kNormal);
    m_kPlane.SetConstant(fConstant);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'test' intersections
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceSphereTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiSphereBV& kSphABV = (const NiSphereBV&) kABV1;
    const NiPlane& kPlane = kHalfABV.GetPlane();

    float fSignedDistance = kPlane.Distance(kSphABV.GetCenter());
    if (fSignedDistance >= -kSphABV.GetRadius())
    {
        // sphere initially intersecting half space
        return true;
    }

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the sphere.  Thus, the kPlane is stationary and the
    // sphere is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is sphere moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    // Sphere moving towards half space, test if it crosses the kPlane during
    // the specified time interval.
    return fSignedDistance + fTime * fNdV >= -kSphABV.GetRadius();
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceBoxTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiBoxBV& kBoxABV = (const NiBoxBV&) kABV1;
    const NiPlane& kPlane = kHalfABV.GetPlane();
    const NiBox& kBox = kBoxABV.GetBox();

    float afAxisNormalProjLength[3] =
    {
        kBox.m_afExtent[0] * (kPlane.GetNormal().Dot(kBox.m_akAxis[0])), 
        kBox.m_afExtent[1] * (kPlane.GetNormal().Dot(kBox.m_akAxis[1])), 
        kBox.m_afExtent[2] * (kPlane.GetNormal().Dot(kBox.m_akAxis[2]))
    };

    float fRadius = NiAbs(afAxisNormalProjLength[0]) + 
        NiAbs(afAxisNormalProjLength[1]) + NiAbs(afAxisNormalProjLength[2]);

    float fSignedDistance = kPlane.Distance(kBoxABV.GetCenter());

    // is kBox initially intersecting half space?
    if (fSignedDistance >= -fRadius)
        return true;

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the kBox.  Thus, the kPlane is stationary and the kBox 
    // is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is kBox moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    // Box moving towards half space, test if it crosses the kPlane during
    // the specified time interval.
    return fSignedDistance + fTime * fNdV >= -fRadius;
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceCapsuleTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiCapsuleBV& kCapABV = (const NiCapsuleBV&) kABV1;
    const NiPlane& kPlane = kHalfABV.GetPlane();
    const NiCapsule& kCapsule = kCapABV.GetCapsule();

    float fSignedDistance0 = kPlane.Distance(kCapsule.m_kSegment.m_kOrigin);

    // is kCapsule end initially intersecting half space?
    if (fSignedDistance0 >= -kCapsule.m_fRadius)
        return true;

    float fSignedDistance1 = fSignedDistance0 +
        kPlane.GetNormal().Dot(kCapsule.m_kSegment.m_kDirection);

    // is kCapsule end initially intersecting half space?
    if (fSignedDistance1 >= -kCapsule.m_fRadius)
        return true;

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the kCapsule.  Thus, the kPlane is stationary and the
    // kCapsule is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is kCapsule moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    // Capsule moving towards half space, test if it crosses the kPlane 
    // during the specified time interval.
    float fRHS = -(kCapsule.m_fRadius + fTime * fNdV);
    return fSignedDistance0 >= fRHS || fSignedDistance1 >= fRHS;
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceHalfSpaceTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1)
{
    const NiHalfSpaceBV& kHalfABV0 = (const NiHalfSpaceBV&) kABV0;
    const NiPlane& kPlane0 = kHalfABV0.GetPlane();
    const NiHalfSpaceBV& kHalfABV1 = (const NiHalfSpaceBV&) kABV1;
    const NiPlane& kPlane1 = kHalfABV1.GetPlane();

    // Half spaces do not intersect whenever their kNormal vectors have
    // opposite directions and there is a separation between the planes.
    // Allow a small amount of floating point error in making this decision.
    float fDot = kPlane0.GetNormal().Dot(kPlane1.GetNormal());

    // are planes not parallel? -- then they must intersect
    if (fDot > -0.999999f)
        return true;

    // if parallel, are half spaces initially overlapping?
    if (kPlane0.GetConstant() <= -kPlane1.GetConstant())
        return true;

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the other.  Thus, the first kPlane is stationary
    // and the second kPlane is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane0.GetNormal().Dot(kDeltaV);

    // is second half space moving away from first half space?
    if (fNdV <= 0.0f)
        return false;

    return kPlane0.GetConstant() <= -kPlane1.GetConstant() + fTime * fNdV;
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceTriTestIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, const NiPoint3& kVert2,
    const NiPoint3& kV1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiPlane& kPlane = kHalfABV.GetPlane();

    float fSignedDistance0 = kPlane.Distance(kVert0);
    // is triangle vertex in half space?
    if (fSignedDistance0 >= 0.0f)
        return true;

    float fSignedDistance1 = kPlane.Distance(kVert1);
    // is triangle vertex in half space
    if (fSignedDistance1 >= 0.0f)
        return true;

    float fSignedDistance2 = kPlane.Distance(kVert2);
    // is triangle vertex in half space
    if (fSignedDistance2 >= 0.0f)
        return true;

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the triangle.  Thus, the kPlane is stationary and the
    // triangle is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is triangle moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    float fRHS = -fTime * fNdV;
    return fSignedDistance0 >= fRHS
        || fSignedDistance1 >= fRHS
        || fSignedDistance2 >= fRHS;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'find' intersections
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceSphereFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiSphereBV& kSphABV = (const NiSphereBV&) kABV1;
    const NiPlane& kPlane = kHalfABV.GetPlane();

    float fSignedDistance = kPlane.Distance(kSphABV.GetCenter());
    if (fSignedDistance >= -kSphABV.GetRadius())
    {
        // Sphere initially intersecting half space.  Use the closest point
        // on the kPlane as the intersection point and use the kPlane kNormal
        // for kNormal vectors.
        fIntrTime = 0.0f;
        kIntrPt = kSphABV.GetCenter() - fSignedDistance * kPlane.GetNormal();

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the sphere.  Thus, the kPlane is stationary and the
    // sphere is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is sphere moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    // Sphere moving towards half space, test if it crosses the kPlane during
    // the specified time interval.
    if (fSignedDistance + fTime * fNdV >= -kSphABV.GetRadius())
    {
        fIntrTime = -(kSphABV.GetRadius() + fSignedDistance) / fNdV;
        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        kIntrPt = kSphABV.GetCenter() + fIntrTime * kV1 + kNormal1 * 
            kSphABV.GetRadius();
        return true;
    }
    else
    {
        // sphere remains outside half space
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceBoxFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiBoxBV& kBoxABV = (const NiBoxBV&) kABV1;
    const NiPlane& kPlane = kHalfABV.GetPlane();
    const NiBox& kBox = kBoxABV.GetBox();

    float afAxisNormalProjLength[3] =
    {
        kBox.m_afExtent[0] * kPlane.GetNormal().Dot(kBox.m_akAxis[0]), 
        kBox.m_afExtent[1] * kPlane.GetNormal().Dot(kBox.m_akAxis[1]), 
        kBox.m_afExtent[2] * kPlane.GetNormal().Dot(kBox.m_akAxis[2])
    };

    float fRadius = NiAbs(afAxisNormalProjLength[0]) + 
        NiAbs(afAxisNormalProjLength[1]) + NiAbs(afAxisNormalProjLength[2]);

    float fSignedDistance = kPlane.Distance(kBoxABV.GetCenter());
    if (fSignedDistance >= -fRadius)
    {
        // Box initially intersecting half space.  Use the projection
        // of center onto the plane as the collision point, the useful 
        // approximation for static collision testing.  Dynamic testing
        // should never reach this if it is set up correctly
        fIntrTime = 0.0f;
        kIntrPt = kBox.m_kCenter - fSignedDistance * kPlane.GetNormal();

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the kBox.  Thus, the kPlane is stationary and the kBox 
    // is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is kBox moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    // Box moving towards half space, test if it crosses the kPlane during
    // the specified time interval.
    if (fSignedDistance + fTime * fNdV >= -fRadius)
    {
        fIntrTime = -(fRadius + fSignedDistance) / fNdV;

        // Use as intersection point the kBox vertex that comes in contact
        // first with the kPlane.
        kIntrPt = kBox.m_kCenter + fIntrTime * kV1;
        for (int i = 0; i < 3; i++)
        {
            // Only add axis length if it's not _|_ to halfspace normal.  
            // This will cause intersection point to be centered on face and 
            // edge collisions properly.
            if (NiAbs(afAxisNormalProjLength[i]) > 1e-03f)
            {
                if (afAxisNormalProjLength[i] > 0.0f)
                    kIntrPt += kBox.m_afExtent[i] * kBox.m_akAxis[i];
                else
                    kIntrPt -= kBox.m_afExtent[i] * kBox.m_akAxis[i]; 
            }
        } 

        if (bCalcNormals)
        {
            kNormal0 = -kPlane.GetNormal();
            kNormal1 = kIntrPt - kBox.m_kCenter;
        }
        return true;
    }
    else
    {
        // kBox remains outside half space
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceCapsuleFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiCapsuleBV& kCapABV = (const NiCapsuleBV&) kABV1;
    const NiPlane& kPlane = kHalfABV.GetPlane();
    const NiCapsule& kCapsule = kCapABV.GetCapsule();

    float fSignedDistance0 = kPlane.Distance(kCapsule.m_kSegment.m_kOrigin);
    float fSignedDistance1 = kPlane.Distance(kCapsule.m_kSegment.m_kOrigin +
        kCapsule.m_kSegment.m_kDirection);

    if (((fSignedDistance0 < 0.0f) && (fSignedDistance1 > 0.0f)) 
        || ((fSignedDistance0 > 0.0f) && (fSignedDistance1 < 0.0f)))
    {
        // Capsule segment initially intersecting half space.  Use intersect
        // of segment and plane to calculate return point and normal of plane
        // to estimate both normals
        fIntrTime = 0.0f;

        kIntrPt = kCapsule.m_kSegment.m_kOrigin + (fSignedDistance0 / 
            (fSignedDistance0 - fSignedDistance1)) *
            kCapsule.m_kSegment.m_kDirection;

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }

        return true;
    }

    if (fSignedDistance0 >= -kCapsule.m_fRadius)
    {
        fIntrTime = 0.0f;

        NiSphereBV kSphABV;
        kSphABV.SetRadius(kCapABV.GetRadius());

        if (fSignedDistance1 >= -kCapsule.m_fRadius)
        {
            // Both ends are intersecting.  Use center of segment for kIntr
            kSphABV.SetCenter(kCapABV.GetCenter());

            return HalfSpaceSphereFindIntersect(fTime, kABV0, kV0, kSphABV, 
                kV1, fIntrTime, kIntrPt, bCalcNormals, kNormal0, kNormal1);
        }

        // Capsule end at segment origin initially intersecting half space.  
        kSphABV.SetCenter(kCapABV.GetSegment().m_kOrigin);

        return HalfSpaceSphereFindIntersect(fTime, kABV0, kV0, kSphABV, 
            kV1, fIntrTime, kIntrPt, bCalcNormals, kNormal0, kNormal1);
    }
    
    if (fSignedDistance1 >= -kCapsule.m_fRadius)
    {
        fIntrTime = 0.0f;

        NiSphereBV kSphABV;
        kSphABV.SetRadius(kCapABV.GetRadius());

        // Capsule end at segment end initially intersecting half space.  
        kSphABV.SetCenter(kCapABV.GetSegment().m_kOrigin + 
            kCapABV.GetSegment().m_kDirection);

        return HalfSpaceSphereFindIntersect(fTime, kABV0, kV0, kSphABV, 
            kV1, fIntrTime, kIntrPt, bCalcNormals, kNormal0, kNormal1);
    }

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the kCapsule.  Thus, the kPlane is stationary and the
    // kCapsule is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is kCapsule moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    // Capsule moving towards half space, test if it crosses the kPlane 
    // during the specified time interval.
    float fRHS = -(kCapsule.m_fRadius + fTime * fNdV);
    if (fSignedDistance0 >= fRHS || fSignedDistance1 >= fRHS)
    {
        if (fSignedDistance0 > fSignedDistance1 + 1e-06f)
        {
            fIntrTime = -(kCapsule.m_fRadius + fSignedDistance0) / fNdV;
            kIntrPt = kCapsule.m_kSegment.m_kOrigin + fIntrTime * kV1 + 
                kPlane.GetNormal() * kCapsule.m_fRadius;
        }
        else if (fSignedDistance1 > fSignedDistance0 + 1e-06f)
        {
            fIntrTime = -(kCapsule.m_fRadius + fSignedDistance1) / fNdV;
            kIntrPt = kCapsule.m_kSegment.m_kOrigin +
                kCapsule.m_kSegment.m_kDirection + fIntrTime * kV1 + 
                kPlane.GetNormal() * kCapsule.m_fRadius;
        }
        else
        {
            fIntrTime = -(kCapsule.m_fRadius + fSignedDistance1) / fNdV;
            kIntrPt = kCapsule.m_kSegment.m_kOrigin +
                kCapsule.m_kSegment.m_kDirection * 0.5f + fIntrTime * kV1 + 
                kPlane.GetNormal() * kCapsule.m_fRadius;
        }

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }
    else
    {
        // kCapsule remains outside half space
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceHalfSpaceFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kABV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals, 
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    const NiHalfSpaceBV& kHalfABV0 = (const NiHalfSpaceBV&) kABV0;
    const NiPlane& kPlane0 = kHalfABV0.GetPlane();
    const NiHalfSpaceBV& kHalfABV1 = (const NiHalfSpaceBV&) kABV1;
    const NiPlane& kPlane1 = kHalfABV1.GetPlane();

    // Half spaces do not intersect whenever their kNormal vectors have
    // opposite directions and there is a separation between the planes.
    // Allow a small amount of floating point error in making this decision.
    float fDot = kPlane0.GetNormal().Dot(kPlane1.GetNormal());
    if (fDot > -0.999999f)
    {
        // Planes are not parallel, they must intersect.  Line of
        // intersection is X(t) = t*Cross(N0, N1) + u0*N0 + u1*N1.  The
        // point of intersection is chosen to be X(0).
        float fFactor = 1.0f / (1.0f - fDot * fDot);
        float fU0 =
            (kPlane0.GetConstant() - fDot * kPlane1.GetConstant()) * fFactor;
        float fU1 =
            (kPlane1.GetConstant() - fDot * kPlane0.GetConstant()) * fFactor;
        
        fIntrTime = 0.0f;
        kIntrPt = fU0 * kPlane0.GetNormal() + fU1 * kPlane1.GetNormal();
        
        if (bCalcNormals)
        {
            kNormal0 = -kPlane0.GetNormal();
            kNormal1 = -kPlane1.GetNormal();
        }

        return true;
    }

    // planes are parallel
    if (kPlane0.GetConstant() <= -kPlane1.GetConstant())
    {
        // Half spaces are initially overlapping.  Use halfway point of
        // line segment X(t) = t*N0 in the region of overlap as the
        // intersection point.
        fIntrTime = 0.0f;
        float fDiff = kPlane0.GetConstant() - kPlane1.GetConstant();
        kIntrPt = fDiff * kPlane0.GetNormal();

        if (bCalcNormals)
        {
            kNormal1 = kPlane0.GetNormal();
            kNormal0 = kPlane1.GetNormal();
        }
        return true;
    }

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the other.  Thus, the first kPlane is stationary
    // and the second kPlane is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane0.GetNormal().Dot(kDeltaV);

    // is second half space moving away from first half space?
    if (fNdV <= 0.0f)
        return false;

    if (kPlane0.GetConstant() <= -kPlane1.GetConstant() + fTime * fNdV)
    {
        fIntrTime = (kPlane0.GetConstant() + kPlane1.GetConstant()) / fNdV;
        kIntrPt = kPlane0.GetConstant() * kPlane0.GetNormal() + 
            fIntrTime * kV0;

        if (bCalcNormals)
        {
            kNormal1 = kPlane0.GetNormal();
            kNormal0 = kPlane1.GetNormal();
        }
        return true;
    }
    else
    {
        // planes remain separated during time interval
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiHalfSpaceBV::HalfSpaceTriFindIntersect(float fTime, 
    const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
    const NiPoint3& kVert0, const NiPoint3& kVert1, const NiPoint3& kVert2, 
    const NiPoint3& kV1, float& fIntrTime, NiPoint3& kIntrPt, 
    bool bCalcNormals, NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    const NiHalfSpaceBV& kHalfABV = (const NiHalfSpaceBV&) kABV0;
    const NiPlane& kPlane = kHalfABV.GetPlane();

    float fSignedDistance0 = kPlane.Distance(kVert0);
    if (fSignedDistance0 >= 0.0f)
    {
        // Triangle vertex is in half space.  Use it as the point of
        // intersection.
        fIntrTime = 0.0f;
        kIntrPt = kVert0;

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }

    float fSignedDistance1 = kPlane.Distance(kVert1);
    if (fSignedDistance1 >= 0.0f)
    {
        // Triangle vertex is in half space.  Use it as the point of
        // intersection.
        fIntrTime = 0.0f;
        kIntrPt = kVert1;

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }

    float fSignedDistance2 = kPlane.Distance(kVert2);
    if (fSignedDistance2 >= 0.0f)
    {
        // Triangle vertex is in half space.  Use it as the point of
        // intersection.
        fIntrTime = 0.0f;
        kIntrPt = kVert2;

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }

    // Perform calculations in the frame of the kPlane by subtracting its
    // velocity from the triangle.  Thus, the kPlane is stationary and the
    // triangle is moving.
    NiPoint3 kDeltaV = kV1 - kV0;
    float fNdV = kPlane.GetNormal().Dot(kDeltaV);

    // is triangle moving away from half space?
    if (fNdV <= 0.0f)
        return false;

    float fRHS = -fTime * fNdV;
    if (fSignedDistance0 >= fRHS || fSignedDistance1 >= fRHS
        || fSignedDistance2 >= fRHS)
    {
        // Use as intersection point the triangle vertex that comes in
        // contact first with the kPlane.
        if (fSignedDistance0 >= fSignedDistance1)
        {
            if (fSignedDistance0 >= fSignedDistance2)
            {
                fIntrTime = -fSignedDistance0 / fNdV;
                kIntrPt = kVert0 + fTime * kV1;
            }
            else
            {
                fIntrTime = -fSignedDistance2 / fNdV;
                kIntrPt = kVert2 + fTime * kV1;
            }
        }
        else
        {
            if (fSignedDistance1 >= fSignedDistance2)
            {
                fIntrTime = -fSignedDistance1 / fNdV;
                kIntrPt = kVert1 + fTime * kV1;
            }
            else
            {
                fIntrTime = -fSignedDistance2 / fNdV;
                kIntrPt = kVert2 + fTime * kV1;
            }
        }

        if (bCalcNormals)
        {
            kNormal1 = kPlane.GetNormal();
            kNormal0 = -kNormal1;
        }
        return true;
    }
    else
    {
        // triangle remains outside half space
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiBoundingVolume* NiHalfSpaceBV::Clone() const
{
    NiHalfSpaceBV* pkClone = NiNew NiHalfSpaceBV(*this);
    NIASSERT(pkClone);
    return (NiBoundingVolume*) pkClone;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiBoundingVolume* NiHalfSpaceBV::CreateFromStream(NiStream& kStream)
{
    NiHalfSpaceBV* pkABV = NiNew NiHalfSpaceBV;
    NIASSERT(pkABV);
    pkABV->LoadBinary(kStream);
    return pkABV;
}
//---------------------------------------------------------------------------
void NiHalfSpaceBV::LoadBinary(NiStream& kStream)
{
    NiBoundingVolume::LoadBinary(kStream);
    m_kPlane.LoadBinary(kStream);
    m_kCenter.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiHalfSpaceBV::SaveBinary(NiStream& kStream)
{
    NiBoundingVolume::SaveBinary(kStream);
    m_kPlane.SaveBinary(kStream);
    m_kCenter.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
void NiHalfSpaceBV::AddViewerStrings(const char* pcPrefix, 
    NiViewerStringsArray* pkStrings) const
{
    size_t stLen = strlen(pcPrefix) + 16;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "%s = HALFSPACE_BV", pcPrefix);
    pkStrings->Add(pString);
}
//---------------------------------------------------------------------------
