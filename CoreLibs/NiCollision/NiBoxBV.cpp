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


#include "NiCollisionPCH.h" // Precompiled header.

#include <NiTransform.h>
#include "NiBoxBV.h"
#include "NiBoxBVMacros.h"
#include "NiBoxCapsuleIntersector.h"
#include "NiBoxSphereIntersector.h"
#include "NiCapsuleBV.h"
#include "NiSegment.h"
#include "NiSphereBV.h"
#include "NiSqrDistance.h"

//---------------------------------------------------------------------------
NiBoxBV::NiBoxBV(const NiPoint3& kExtent, const NiPoint3& kCenter,  
    const NiPoint3& kXAvis, const NiPoint3& kYAvis, const NiPoint3& kZAxis)
{
    m_kBox.m_kCenter = kCenter;
    m_kBox.m_afExtent[0] = kExtent[0];
    m_kBox.m_afExtent[1] = kExtent[1];
    m_kBox.m_afExtent[2] = kExtent[2];
    m_kBox.m_akAxis[0] = kXAvis;
    m_kBox.m_akAxis[1] = kYAvis;
    m_kBox.m_akAxis[2] = kZAxis;
}
//---------------------------------------------------------------------------
void NiBoxBV::Copy(const NiBoundingVolume& kABV)
{
    NiBoxBV& kBBV = (NiBoxBV&)kABV;
    SetBox(kBBV.GetBox());
}
//---------------------------------------------------------------------------
bool NiBoxBV::operator == (const NiBoundingVolume& kABV) const
{
    return kABV.Type() == BOX_BV && ((NiBoxBV&)kABV).m_kBox == m_kBox;
}
//---------------------------------------------------------------------------
bool NiBoxBV::operator != (const NiBoundingVolume& kABV) const
{
    return !operator == (kABV);
}
//---------------------------------------------------------------------------
void NiBoxBV::UpdateWorldData(const NiBoundingVolume& kModelABV, 
    const NiTransform& kWorld)
{
    const NiBoxBV& kModelBox = (const NiBoxBV&) kModelABV;

    SetCenter(kWorld.m_fScale * (kWorld.m_Rotate * kModelBox.GetCenter()) +
        kWorld.m_Translate);

    SetAxis(0, kWorld.m_Rotate * kModelBox.GetAxis(0));
    SetExtent(0, kWorld.m_fScale * kModelBox.GetExtent(0));

    SetAxis(1, kWorld.m_Rotate * kModelBox.GetAxis(1));
    SetExtent(1, kWorld.m_fScale * kModelBox.GetExtent(1));
    
    SetAxis(2, kWorld.m_Rotate * kModelBox.GetAxis(2));
    SetExtent(2, kWorld.m_fScale * kModelBox.GetExtent(2));
}
//---------------------------------------------------------------------------
// 'Test' intersections.
//---------------------------------------------------------------------------
bool NiBoxBV::BoxSphereTestIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kBV1, const NiPoint3& kV1)
{
    const NiBoxBV& kBBV = (const NiBoxBV&) kBV0;
    const NiSphereBV& kSBV = (const NiSphereBV&) kBV1;

    // Quick check for sphere center already inside box.
    NiPoint3 kDeltaC = kSBV.GetCenter() - kBBV.GetCenter();
    float fCx = kDeltaC.Dot(kBBV.GetAxis(0));
    if (NiAbs(fCx) <= kBBV.GetExtent(0)) // Flip into 1st octant.
    {
        float fCy = kDeltaC.Dot(kBBV.GetAxis(1));
        if (NiAbs(fCy) <= kBBV.GetExtent(1)) // Flip into 1st octant.
        {
            float fCz = kDeltaC.Dot(kBBV.GetAxis(2));
            if (NiAbs(fCz) <= kBBV.GetExtent(2)) // Flip into 1st octant.
            {
                return true;    // Sphere center already inside box.
            }
        }
    }

    // Perform calculations in the coordinate system of the box by subtracting
    // its velocity from the sphere.  Thus, the box is stationary and the
    // sphere is moving.  The problem reduces to measuring distance from line
    // segment to box (if sphere is moving relative to box) or from point to
    // box (if sphere is stationary relative to box).
    NiPoint3 kDeltaV = kV1 - kV0;

    float fSqrDist, fR0, fR1, fR2;

    if (kDeltaV != NiPoint3::ZERO)
    {
        // Compute squared distance from line segment to box.
        NiSegment kSeg;
        kSeg.m_kOrigin = kSBV.GetCenter();
        kSeg.m_kDirection = fTime * kDeltaV;

        float fS;
        fSqrDist = NiSqrDistance::Compute(kSeg, kBBV.GetBox(), fS, fR0, fR1,
            fR2);
    }
    else
    {
        // Compute squared distance from point to box.
        fSqrDist = NiSqrDistance::Compute(kSBV.GetCenter(), kBBV.GetBox(),
            fR0, fR1, fR2);
    }

    // Compare distances.
    return fSqrDist <= kSBV.GetRadius() * kSBV.GetRadius();
}
//---------------------------------------------------------------------------
bool NiBoxBV::BoxBoxTestIntersect(float fTime, const NiBoundingVolume& kBV0, 
    const NiPoint3& kV0, const NiBoundingVolume& kBV1, const NiPoint3& kV1)
{
    const NiBoxBV& kBBV0 = (const NiBoxBV&) kBV0;
    const NiBoxBV& kBBV1 = (const NiBoxBV&) kBV1;

    // convenience variables
    const NiPoint3* A = kBBV0.GetAxes();
    const NiPoint3* B = kBBV1.GetAxes();
    const float* extA = kBBV0.GetExtents();
    const float* extB = kBBV1.GetExtents();

    NiPoint3 W;       // relative velocity between boxes
    NiPoint3 D0;      // difference of box centers at time '0'
    NiPoint3 D1;      // difference of box centers at time 'fTime'
    float AB[3][3];   // matrix C = A^fTime B, c_{ij} = Dot(A_i, B_j)
    float fAB[3][3];  // fabs(c_{ij})
    float AD0[3];     // Dot(A_i, D0)
    float AD1[3];     // Dot(A_i, D1)
    float R0;         // interval radius 0
    float R1;         // interval radius 1
    float R;          // distance between centers
    float R01;        // R0 + R1

    // Compute relative velocity of box1 with respect to box0 so that
    // box0 may as well be stationary.
    W = kV1 - kV0;

    // compute difference of box centers (time 0 and time fTime)
    D0 = kBBV1.GetCenter() - kBBV0.GetCenter();
    D1 = D0 + (fTime * W);
    
    // axis C0 + t*A0
    AB[0][0] = A[0].Dot(B[0]);
    AB[0][1] = A[0].Dot(B[1]);
    AB[0][2] = A[0].Dot(B[2]);
    AD0[0] = A[0].Dot(D0);
    AD1[0] = A[0].Dot(D1);
    fAB[0][0] = NiAbs(AB[0][0]);
    fAB[0][1] = NiAbs(AB[0][1]);
    fAB[0][2] = NiAbs(AB[0][2]);
    R1 = (extB[0] * fAB[0][0]) + (extB[1] * fAB[0][1])
        + (extB[2] * fAB[0][2]);
    R01 = extA[0] + R1;
    if (AD0[0] > R01)
    {
        if (AD1[0] > R01)
            return false;  // AXIS_A0
    }
    else if (AD0[0] < -R01)
    {
        if (AD1[0] < -R01)
            return false;  // AXIS_A0
    }

    // axis C0 + t*A1
    AB[1][0] = A[1].Dot(B[0]);
    AB[1][1] = A[1].Dot(B[1]);
    AB[1][2] = A[1].Dot(B[2]);
    AD0[1] = A[1].Dot(D0);
    AD1[1] = A[1].Dot(D1);
    fAB[1][0] = NiAbs(AB[1][0]);
    fAB[1][1] = NiAbs(AB[1][1]);
    fAB[1][2] = NiAbs(AB[1][2]);
    R1 = (extB[0] * fAB[1][0]) + (extB[1] * fAB[1][1])
        + (extB[2] * fAB[1][2]);
    R01 = extA[1] + R1;
    if (AD0[1] > R01)
    {
        if (AD1[1] > R01)
            return false;  // AXIS_A1
    }
    else if (AD0[1] < -R01)
    {
        if (AD1[1] < -R01)
            return false;  // AXIS_A1
    }

    // axis C0 + t*A2
    AB[2][0] = A[2].Dot(B[0]);
    AB[2][1] = A[2].Dot(B[1]);
    AB[2][2] = A[2].Dot(B[2]);
    AD0[2] = A[2].Dot(D0);
    AD1[2] = A[2].Dot(D1);
    fAB[2][0] = NiAbs(AB[2][0]);
    fAB[2][1] = NiAbs(AB[2][1]);
    fAB[2][2] = NiAbs(AB[2][2]);
    R1 = (extB[0] * fAB[2][0]) + (extB[1] * fAB[2][1])
        + (extB[2] * fAB[2][2]);
    R01 = extA[2] + R1;
    if (AD0[2] > R01)
    {
        if (AD1[2] > R01)
            return false;  // AXIS_A2
    }
    else if (AD0[2] < -R01)
    {
        if (AD1[2] < -R01)
            return false;  // AXIS_A2
    }

    // axis C0 + t*B0
    R0 = (extA[0] * fAB[0][0]) + (extA[1] * fAB[1][0])
        + (extA[2] * fAB[2][0]);
    R = B[0].Dot(D0);
    R01 = R0 + extB[0];
    if (R > R01)
    {
        if (B[0].Dot(D1) > R01)
            return false;  // AXIS_B0
    }
    else if (R < -R01)
    {
        if (B[0].Dot(D1) < -R01)
            return false;  // AXIS_B0
    }

    // axis C0 + t*B1
    R0 = (extA[0] * fAB[0][1]) + (extA[1] * fAB[1][1])
        + (extA[2] * fAB[2][1]);
    R = B[1].Dot(D0);
    R01 = R0 + extB[1];
    if (R > R01)
    {
        if (B[1].Dot(D1) > R01)
            return false;  // AXIS_B1
    }
    else if (R < -R01)
    {
        if (B[1].Dot(D1) < -R01)
            return false;  // AXIS_B1
    }

    // axis C0 + t*B2
    R0 = (extA[0] * fAB[0][2]) + (extA[1] * fAB[1][2])
        + (extA[2] * fAB[2][2]);
    R = B[2].Dot(D0);
    R01 = R0 + extB[2];
    if (R > R01)
    {
        if (B[2].Dot(D1) > R01)
            return false;  // AXIS_B2
    }
    else if (R < -R01)
    {
        if (B[2].Dot(D1) < -R01)
            return false;  // AXIS_B2
    }

    // axis C0 + t*A0xB0
    R0 = (extA[1] * fAB[2][0]) + (extA[2] * fAB[1][0]);
    R1 = (extB[1] * fAB[0][2]) + (extB[2] * fAB[0][1]);
    R = (AD0[2] * AB[1][0]) - (AD0[1] * AB[2][0]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[2] * AB[1][0]) - (AD1[1] * AB[2][0]) > R01)
            return false;  // AXIS_A0xB0
    }
    else if (R < -R01)
    {
        if ((AD1[2] * AB[1][0]) - (AD1[1] * AB[2][0]) < -R01)
            return false;  // AXIS_A0xB0
    }

    // axis C0 + t*A0xB1
    R0 = (extA[1] * fAB[2][1]) + (extA[2] * fAB[1][1]);
    R1 = (extB[0] * fAB[0][2]) + (extB[2] * fAB[0][0]);
    R = (AD0[2] * AB[1][1]) - (AD0[1] * AB[2][1]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[2] * AB[1][1]) - (AD1[1] * AB[2][1]) > R01)
            return false;  // AXIS_A0xB1
    }
    else if (R < -R01)
    {
        if ((AD1[2] * AB[1][1]) - (AD1[1] * AB[2][1]) < -R01)
            return false;  // AXIS_A0xB1
    }

    // axis C0 + t*A0xB2
    R0 = (extA[1] * fAB[2][2]) + (extA[2] * fAB[1][2]);
    R1 = (extB[0] * fAB[0][1]) + (extB[1] * fAB[0][0]);
    R = (AD0[2] * AB[1][2]) - (AD0[1] * AB[2][2]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[2] * AB[1][2]) - (AD1[1] * AB[2][2]) > R01)
            return false;  // AXIS_A0xB2
    }
    else if (R < -R01)
    {
        if ((AD1[2] * AB[1][2]) - (AD1[1] * AB[2][2]) < -R01)
            return false;  // AXIS_A0xB2
    }

    // axis C0 + t*A1xB0
    R0 = (extA[0] * fAB[2][0]) + (extA[2] * fAB[0][0]);
    R1 = (extB[1] * fAB[1][2]) + (extB[2] * fAB[1][1]);
    R = (AD0[0] * AB[2][0]) - (AD0[2] * AB[0][0]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[0] * AB[2][0]) - (AD1[2] * AB[0][0]) > R01)
            return false;  // AXIS_A1xB0
    }
    else if (R < -R01)
    {
        if ((AD1[0] * AB[2][0]) - (AD1[2] * AB[0][0]) < -R01)
            return false;  // AXIS_A1xB0
    }

    // axis C0 + t*A1xB1
    R0 = (extA[0] * fAB[2][1]) + (extA[2] * fAB[0][1]);
    R1 = (extB[0] * fAB[1][2]) + (extB[2] * fAB[1][0]);
    R = (AD0[0] * AB[2][1]) - (AD0[2] * AB[0][1]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[0] * AB[2][1]) - (AD1[2] * AB[0][1]) > R01)
            return false;  // AXIS_A1xB1
    }
    else if (R < -R01)
    {
        if ((AD1[0] * AB[2][1]) - (AD1[2] * AB[0][1]) < -R01)
            return false;  // AXIS_A1xB1
    }

    // axis C0 + t*A1xB2
    R0 = (extA[0] * fAB[2][2]) + (extA[2] * fAB[0][2]);
    R1 = (extB[0] * fAB[1][1]) + (extB[1] * fAB[1][0]);
    R = (AD0[0] * AB[2][2]) - (AD0[2] * AB[0][2]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[0] * AB[2][2]) - (AD1[2] * AB[0][2]) > R01)
            return false;  // AXIS_A1xB2
    }
    else if (R < -R01)
    {
        if ((AD1[0] * AB[2][2]) - (AD1[2] * AB[0][2]) < -R01)
            return false;  // AXIS_A1xB2
    }

    // axis C0 + t*A2xB0
    R0 = (extA[0] * fAB[1][0]) + (extA[1] * fAB[0][0]);
    R1 = (extB[1] * fAB[2][2]) + (extB[2] * fAB[2][1]);
    R = (AD0[1] * AB[0][0]) - (AD0[0] * AB[1][0]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[1] * AB[0][0]) - (AD1[0] * AB[1][0]) > R01)
            return false;  // AXIS_A2xB0
    }
    else if (R < -R01)
    {
        if ((AD1[1] * AB[0][0]) - (AD1[0] * AB[1][0]) < -R01)
            return false;  // AXIS_A2xB0
    }

    // axis C0 + t*A2xB1
    R0 = (extA[0] * fAB[1][1]) + (extA[1] * fAB[0][1]);
    R1 = (extB[0] * fAB[2][2]) + (extB[2] * fAB[2][0]);
    R = (AD0[1] * AB[0][1]) - (AD0[0] * AB[1][1]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[1] * AB[0][1]) - (AD1[0] * AB[1][1]) > R01)
            return false;  // AXIS_A2xB1
    }
    else if (R < -R01)
    {
        if ((AD1[1] * AB[0][1]) - (AD1[0] * AB[1][1]) < -R01)
            return false;  // AXIS_A2xB1
    }

    // axis C0 + t*A2xB2
    R0 = (extA[0] * fAB[1][2]) + (extA[1] * fAB[0][2]);
    R1 = (extB[0] * fAB[2][1]) + (extB[1] * fAB[2][0]);
    R = (AD0[1] * AB[0][2]) - (AD0[0] * AB[1][2]);
    R01 = R0 + R1;
    if (R > R01)
    {
        if ((AD1[1] * AB[0][2]) - (AD1[0] * AB[1][2]) > R01)
            return false;  // AXIS_A2xB2
    }
    else if (R < -R01)
    {
        if ((AD1[1] * AB[0][2]) - (AD1[0] * AB[1][2]) < -R01)
            return false;  // AXIS_A2xB2
    }

    // At this point none of the 15 axes separate the boxes.  It is still
    // possible that they are separated as viewed in any plane orthogonal
    // to the relative direction of motion W.  In the worst case, the two
    // projected boxes are hexagons.  This requires three separating axis
    // tests per box.
    NiPoint3 kWxD0 = W.Cross(D0);
    float afWA[3], afWB[3];

    // axis C0 + t*WxA0
    afWA[1] = W.Dot(A[1]);
    afWA[2] = W.Dot(A[2]);
    R = NiAbs(A[0].Dot(kWxD0));
    R0 = extA[1]*NiAbs(afWA[2]) + extA[2]*NiAbs(afWA[1]);
    R1 =
        extB[0]*NiAbs(AB[1][0]*afWA[2] - AB[2][0]*afWA[1]) +
        extB[1]*NiAbs(AB[1][1]*afWA[2] - AB[2][1]*afWA[1]) +
        extB[2]*NiAbs(AB[1][2]*afWA[2] - AB[2][2]*afWA[1]);
    R01 = R0 + R1;
    if (R > R01)
        return false;

    // axis C0 + t*WxA1
    afWA[0] = W.Dot(A[0]);
    R = NiAbs(A[1].Dot(kWxD0));
    R0 = extA[2]*NiAbs(afWA[0]) + extA[0]*NiAbs(afWA[2]);
    R1 =
        extB[0]*NiAbs(AB[2][0]*afWA[0] - AB[0][0]*afWA[2]) +
        extB[1]*NiAbs(AB[2][1]*afWA[0] - AB[0][1]*afWA[2]) +
        extB[2]*NiAbs(AB[2][2]*afWA[0] - AB[0][2]*afWA[2]);
    R01 = R0 + R1;
    if (R > R01)
        return false;

    // axis C0 + t*WxA2
    R = NiAbs(A[2].Dot(kWxD0));
    R0 = extA[0]*NiAbs(afWA[1]) + extA[1]*NiAbs(afWA[0]);
    R1 =
        extB[0]*NiAbs(AB[0][0]*afWA[1] - AB[1][0]*afWA[0]) +
        extB[1]*NiAbs(AB[0][1]*afWA[1] - AB[1][1]*afWA[0]) +
        extB[2]*NiAbs(AB[0][2]*afWA[1] - AB[1][2]*afWA[0]);
    R01 = R0 + R1;
    if (R > R01)
        return false;

    // axis C0 + t*WxB0
    afWB[1] = W.Dot(B[1]);
    afWB[2] = W.Dot(B[2]);
    R = NiAbs(B[0].Dot(kWxD0));
    R0 =
        extA[0]*NiAbs(AB[0][1]*afWB[2] - AB[0][2]*afWB[1]) +
        extA[1]*NiAbs(AB[1][1]*afWB[2] - AB[1][2]*afWB[1]) +
        extA[2]*NiAbs(AB[2][1]*afWB[2] - AB[2][2]*afWB[1]);
    R1 = extB[1]*NiAbs(afWB[2]) + extB[2]*NiAbs(afWB[1]);
    R01 = R0 + R1;
    if (R > R01)
        return false;

    // axis C0 + t*WxB1
    afWB[0] = W.Dot(B[0]);
    R = NiAbs(B[1].Dot(kWxD0));
    R0 =
        extA[0]*NiAbs(AB[0][2]*afWB[0] - AB[0][0]*afWB[2]) +
        extA[1]*NiAbs(AB[1][2]*afWB[0] - AB[1][0]*afWB[2]) +
        extA[2]*NiAbs(AB[2][2]*afWB[0] - AB[2][0]*afWB[2]);
    R1 = extB[2]*NiAbs(afWB[0]) + extB[0]*NiAbs(afWB[2]);
    R01 = R0 + R1;
    if (R > R01)
        return false;

    // axis C0 + t*WxB2
    R = NiAbs(B[2].Dot(kWxD0));
    R0 =
        extA[0]*NiAbs(AB[0][0]*afWB[1] - AB[0][1]*afWB[0]) +
        extA[1]*NiAbs(AB[1][0]*afWB[1] - AB[1][1]*afWB[0]) +
        extA[2]*NiAbs(AB[2][0]*afWB[1] - AB[2][1]*afWB[0]);
    R1 = extB[0]*NiAbs(afWB[1]) + extB[1]*NiAbs(afWB[0]);
    R01 = R0 + R1;
    if (R > R01)
        return false;

    // intersection occurs
    return true;
}
//---------------------------------------------------------------------------
bool NiBoxBV::BoxCapsuleTestIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kBV1, const NiPoint3& kV1)
{
    // epsilon is unused, but leaving it in, just in case.
    //const float fEpsilon = 1e-05f;

    const NiBoxBV& kBBV = (const NiBoxBV&) kBV0;
    const NiCapsuleBV& kCBV = (const NiCapsuleBV&) kBV1;

    // Quick check for capsule center already inside box.
    NiPoint3 kDeltaC = kCBV.GetCenter() - kBBV.GetCenter();
    float fCx = kDeltaC.Dot(kBBV.GetAxis(0));
    if (NiAbs(fCx) <= kBBV.GetExtent(0)) // Flip into 1st octant.
    {
        float fCy = kDeltaC.Dot(kBBV.GetAxis(1));
        if (NiAbs(fCy) <= kBBV.GetExtent(1)) // Flip into 1st octant.
        {
            float fCz = kDeltaC.Dot(kBBV.GetAxis(2));
            if (NiAbs(fCz) <= kBBV.GetExtent(2)) // Flip into 1st octant.
            {
                return true;    // Sphere center already inside box.
            }
        }
    }

    NiBoxCapsuleIntersector kIntersector(kBBV, kCBV, fTime);
    kIntersector.Test(kV0, kV1);
    return kIntersector.GetIntersectionType() == NiIntersector::IT_OVERLAP
        || kIntersector.GetIntersectionType() == NiIntersector::IT_CONTACT;
}
//---------------------------------------------------------------------------
bool NiBoxBV::BoxTriTestIntersect(float fTime, const NiBoundingVolume& kBV0,
    const NiPoint3& kV0, const NiPoint3& kVert0, const NiPoint3& kVert1,
    const NiPoint3& kVert2, const NiPoint3& kV1)
{
    const NiBoxBV& kBBV = (const NiBoxBV&) kBV0;

    // convenience variables
    const NiPoint3* A = kBBV.GetAxes();
    const float* extA = kBBV.GetExtents();

    // Compute relative velocity of triangle with respect to box so that box
    // may as well be stationary.
    NiPoint3 W = kV1 - kV0;

    // construct triangle normal, difference of kCenter and vertex
    NiPoint3 D;
    NiPoint3 E[2];
    NiPoint3 N;
    E[0] = kVert1 - kVert0;
    E[1] = kVert2 - kVert0;
    N = E[0].Cross(E[1]);
    D = kVert0 - kBBV.GetCenter();

    // axis C + t*N
    float A0dN = A[0].Dot(N);
    float A1dN = A[1].Dot(N);
    float A2dN = A[2].Dot(N);
    float R = NiAbs(extA[0] * A0dN) + NiAbs(extA[1] * A1dN)
        + NiAbs(extA[2] * A2dN);
    float NdD = N.Dot(D);
    BT_TEST0(NdD, R, fTime, N, W);

    // axis C + t*A0
    float A0dD = A[0].Dot(D);
    float A0dE0 = A[0].Dot(E[0]);
    float A0dE1 = A[0].Dot(E[1]);
    float A0dW = A[0].Dot(W);
    BT_TEST1(A0dD, fTime, A0dW, A0dE0, A0dE1, extA[0]);

    // axis C + t*A1
    float A1dD = A[1].Dot(D);
    float A1dE0 = A[1].Dot(E[0]);
    float A1dE1 = A[1].Dot(E[1]);
    float A1dW = A[1].Dot(W);
    BT_TEST1(A1dD, fTime, A1dW, A1dE0, A1dE1, extA[1]);

    // axis C + t*A2
    float A2dD = A[2].Dot(D);
    float A2dE0 = A[2].Dot(E[0]);
    float A2dE1 = A[2].Dot(E[1]);
    float A2dW = A[2].Dot(W);
    BT_TEST1(A2dD, fTime, A2dW, A2dE0, A2dE1, extA[2]);

    // axis C + t*A0xE0
    NiPoint3 A0xE0;
    A0xE0 = A[0].Cross(E[0]);
    float A0xE0dD = A0xE0.Dot(D);
    float A0xE0dW = A0xE0.Dot(W);
    R = NiAbs(extA[1] * A2dE0) + NiAbs(extA[2] * A1dE0);
    BT_TEST2(A0xE0dD, fTime, A0xE0dW, A0dN, R);

    // axis C + t*A0xE1
    NiPoint3 A0xE1;
    A0xE1 = A[0].Cross(E[1]);
    float A0xE1dD = A0xE1.Dot(D);
    float A0xE1dW = A0xE1.Dot(W);
    R = NiAbs(extA[1] * A2dE1) + NiAbs(extA[2] * A1dE1);
    BT_TEST2(A0xE1dD, fTime, A0xE1dW, -A0dN, R);

    // axis C + t*A0xE2
    float A1dE2 = A1dE1 - A1dE0;
    float A2dE2 = A2dE1 - A2dE0;
    float A0xE2dD = A0xE1dD - A0xE0dD;
    float A0xE2dW = A0xE1dW - A0xE0dW;
    R = NiAbs(extA[1] * A2dE2) + NiAbs(extA[2] * A1dE2);
    BT_TEST2(A0xE2dD, fTime, A0xE2dW, -A0dN, R);

    // axis C + t*A1xE0
    NiPoint3 A1xE0;
    A1xE0 = A[1].Cross(E[0]);
    float A1xE0dD = A1xE0.Dot(D);
    float A1xE0dW = A1xE0.Dot(W);
    R = NiAbs(extA[0] * A2dE0) + NiAbs(extA[2] * A0dE0);
    BT_TEST2(A1xE0dD, fTime, A1xE0dW, A1dN, R);

    // axis C + t*A1xE1
    NiPoint3 A1xE1;
    A1xE1 = A[1].Cross(E[1]);
    float A1xE1dD = A1xE1.Dot(D);
    float A1xE1dW = A1xE1.Dot(W);
    R = NiAbs(extA[0] * A2dE1) + NiAbs(extA[2] * A0dE1);
    BT_TEST2(A1xE1dD, fTime, A1xE1dW, -A1dN, R);

    // axis C + t*A1xE2
    float A0dE2 = A0dE1 - A0dE0;
    float A1xE2dD = A1xE1dD - A1xE0dD;
    float A1xE2dW = A1xE1dW - A1xE0dW;
    R = NiAbs(extA[0] * A2dE2) + NiAbs(extA[2] * A0dE2);
    BT_TEST2(A1xE2dD, fTime, A1xE2dW, -A1dN, R);

    // axis C + t*A2xE0
    NiPoint3 A2xE0;
    A2xE0 = A[2].Cross(E[0]);
    float A2xE0dD = A2xE0.Dot(D);
    float A2xE0dW = A2xE0.Dot(W);
    R = NiAbs(extA[0] * A1dE0) + NiAbs(extA[1] * A0dE0);
    BT_TEST2(A2xE0dD, fTime, A2xE0dW, A2dN, R);

    // axis C + t*A2xE1
    NiPoint3 A2xE1;
    A2xE1 = A[2].Cross(E[1]);
    float A2xE1dD = A2xE1.Dot(D);
    float A2xE1dW = A2xE1.Dot(W);
    R = NiAbs(extA[0] * A1dE1) + NiAbs(extA[1] * A0dE1);
    BT_TEST2(A2xE1dD, fTime, A2xE1dW, -A2dN, R);

    // axis C + t*A2xE2
    float A2xE2dD = A2xE1dD - A2xE0dD;
    float A2xE2dW = A2xE1dW - A2xE0dW;
    R = NiAbs(extA[0] * A1dE2) + NiAbs(extA[1] * A0dE2);
    BT_TEST2(A2xE2dD, fTime, A2xE2dW, -A2dN, R);

    return true;    // Intersection occurs.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// 'Find' intersections
//---------------------------------------------------------------------------
bool NiBoxBV::BoxSphereFindIntersect(float fTime,
    const NiBoundingVolume& kBV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kBV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    NiBoxSphereIntersector kIntersector((const NiBoxBV&)kBV0,
        (const NiSphereBV&)kBV1, fTime);

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
bool NiBoxBV::BoxBoxFindIntersect(float fTime, const NiBoundingVolume& kBV0, 
    const NiPoint3& kV0, const NiBoundingVolume& kBV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals, 
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    const float fEpsilon = 1e-06f;
    const NiBoxBV& kBBV0 = (const NiBoxBV&) kBV0;
    const NiBoxBV& kBBV1 = (const NiBoxBV&) kBV1;

    // Convenience variables.
    const NiPoint3* A = kBBV0.GetAxes();
    const NiPoint3* B = kBBV1.GetAxes();
    const float* extA = kBBV0.GetExtents();
    const float* extB = kBBV1.GetExtents();

    NiPoint3 W;       // relative velocity between boxes
    NiPoint3 D0;      // difference of box centers at time '0'
    NiPoint3 D1;      // difference of box centers at time 'fTime'
    float AB[3][3];   // matrix C = A^fTime B, c_{ij} = Dot(A_i, B_j)
    float fAB[3][3];  // fabs(c_{ij})
    float AD0[3];     // Dot(A_i, D0)
    float AD1[3];     // Dot(A_i, D1)
    float BD0[3];     // Dot(B_i, D0)
    float BD1[3];     // Dot(B_i, D1)
    float AR1[3];     // AR1[i] = Dot(extB, AB[i][])
    float BR0[3];     // BR0[i] = Dot(extA, AB[][i])
    float R0;         // interval radius 0
    float R1;         // interval radius 1
    float R;          // distance between centers
    float Rsum;       // R0 + R1

    // Compute relative velocity of kBBV1 with respect to kBBV0 so that kBBV0
    // may as well be stationary.
    W = kV1 - kV0;

    // Compute difference of box centers (time 0 and time fTime)
    D0 = kBBV1.GetCenter() - kBBV0.GetCenter();
    D1 = D0 + (fTime * W);

    // Track maximum time of projection - intersection
    unsigned int uiType = 0;
    int iSide = 0;
    fIntrTime = 0.0f;

    // axis C0 + t*A0
    AB[0][0] = A[0].Dot(B[0]);
    AB[0][1] = A[0].Dot(B[1]);
    AB[0][2] = A[0].Dot(B[2]);
    AD0[0] = A[0].Dot(D0);
    AD1[0] = A[0].Dot(D1);
    fAB[0][0] = NiAbs(AB[0][0]);
    fAB[0][1] = NiAbs(AB[0][1]);
    fAB[0][2] = NiAbs(AB[0][2]);
    AR1[0] = (extB[0] * fAB[0][0]) + (extB[1] * fAB[0][1]) + 
        (extB[2] * fAB[0][2]);
    Rsum = extA[0] + AR1[0];
    BB_FIND0(AD0[0], AD1[0], Rsum, fTime, fIntrTime, uiType, iSide, 1);

    // axis C0 + t*A1
    AB[1][0] = A[1].Dot(B[0]);
    AB[1][1] = A[1].Dot(B[1]);
    AB[1][2] = A[1].Dot(B[2]);
    AD0[1] = A[1].Dot(D0);
    AD1[1] = A[1].Dot(D1);
    fAB[1][0] = NiAbs(AB[1][0]);
    fAB[1][1] = NiAbs(AB[1][1]);
    fAB[1][2] = NiAbs(AB[1][2]);
    AR1[1] = (extB[0] * fAB[1][0]) + (extB[1] * fAB[1][1]) + 
        (extB[2] * fAB[1][2]);
    Rsum = extA[1] + AR1[1];
    BB_FIND0(AD0[1], AD1[1], Rsum, fTime, fIntrTime, uiType, iSide, 2);

    // axis C0 + t*A2
    AB[2][0] = A[2].Dot(B[0]);
    AB[2][1] = A[2].Dot(B[1]);
    AB[2][2] = A[2].Dot(B[2]);
    AD0[2] = A[2].Dot(D0);
    AD1[2] = A[2].Dot(D1);
    fAB[2][0] = NiAbs(AB[2][0]);
    fAB[2][1] = NiAbs(AB[2][1]);
    fAB[2][2] = NiAbs(AB[2][2]);
    AR1[2] = (extB[0] * fAB[2][0]) + (extB[1] * fAB[2][1]) + 
        (extB[2] * fAB[2][2]);
    Rsum = extA[2] + AR1[2];
    BB_FIND0(AD0[2], AD1[2], Rsum, fTime, fIntrTime, uiType, iSide, 3);

    // axis C0 + t*B0
    BD0[0] = B[0].Dot(D0);
    BD1[0] = B[0].Dot(D1);
    BR0[0] = (extA[0] * fAB[0][0]) + (extA[1] * fAB[1][0]) + 
        (extA[2] * fAB[2][0]);
    Rsum = BR0[0] + extB[0];
    BB_FIND0(BD0[0], BD1[0], Rsum, fTime, fIntrTime, uiType, iSide, 4);

    // axis C0 + t*B1
    BD0[1] = B[1].Dot(D0);
    BD1[1] = B[1].Dot(D1);
    BR0[1] = (extA[0] * fAB[0][1]) + (extA[1] * fAB[1][1]) + 
        (extA[2] * fAB[2][1]);
    Rsum = BR0[1] + extB[1];
    BB_FIND0(BD0[1], BD1[1], Rsum, fTime, fIntrTime, uiType, iSide, 5);

    // axis C0 + t*B2
    BD0[2] = B[2].Dot(D0);
    BD1[2] = B[2].Dot(D1);
    BR0[2] = (extA[0] * fAB[0][2]) + ((extA[1] * fAB[1][2])) + 
        (extA[2] * fAB[2][2]);
    Rsum = BR0[2] + extB[2];
    BB_FIND0(BD0[2], BD1[2], Rsum, fTime, fIntrTime, uiType, iSide, 6);

    // axis C0 + t*A0xB0
    R0 = (extA[1] * fAB[2][0]) + (extA[2] * fAB[1][0]);
    R1 = (extB[1] * fAB[0][2]) + (extB[2] * fAB[0][1]);
    R = (AD0[2] * AB[1][0]) - (AD0[1] * AB[2][0]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[2], AB[1][0], AD1[1], AB[2][0], Rsum, fTime, fIntrTime, 
        uiType, iSide, 7);

    // axis C0 + t*A0xB1
    R0 = (extA[1] * fAB[2][1]) + (extA[2] * fAB[1][1]);
    R1 = (extB[0] * fAB[0][2]) + (extB[2] * fAB[0][0]);
    R = (AD0[2] * AB[1][1]) - (AD0[1] * AB[2][1]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[2], AB[1][1], AD1[1], AB[2][1], Rsum, fTime, fIntrTime, 
        uiType, iSide, 8);

    // axis C0 + t*A0xB2
    R0 = (extA[1] * fAB[2][2]) + (extA[2] * fAB[1][2]);
    R1 = (extB[0] * fAB[0][1]) + (extB[1] * fAB[0][0]);
    R = (AD0[2] * AB[1][2]) - (AD0[1] * AB[2][2]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[2], AB[1][2], AD1[1], AB[2][2], Rsum, fTime, fIntrTime,
        uiType, iSide, 9);

    // axis C0 + t*A1xB0
    R0 = (extA[0] * fAB[2][0]) + (extA[2] * fAB[0][0]);
    R1 = (extB[1] * fAB[1][2]) + (extB[2] * fAB[1][1]);
    R = (AD0[0] * AB[2][0]) - (AD0[2] * AB[0][0]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[0], AB[2][0], AD1[2], AB[0][0], Rsum, fTime, fIntrTime, 
        uiType, iSide, 10);

    // axis C0 + t*A1xB1
    R0 = (extA[0] * fAB[2][1]) + (extA[2] * fAB[0][1]);
    R1 = (extB[0] * fAB[1][2]) + (extB[2] * fAB[1][0]);
    R = (AD0[0] * AB[2][1]) - (AD0[2] * AB[0][1]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[0], AB[2][1], AD1[2], AB[0][1], Rsum, fTime, fIntrTime, 
        uiType, iSide, 11);

    // axis C0 + t*A1xB2
    R0 = (extA[0] * fAB[2][2]) + (extA[2] * fAB[0][2]);
    R1 = (extB[0] * fAB[1][1]) + (extB[1] * fAB[1][0]);
    R = (AD0[0] * AB[2][2]) - (AD0[2] * AB[0][2]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[0], AB[2][2], AD1[2], AB[0][2], Rsum, fTime, fIntrTime, 
        uiType, iSide, 12);

    // axis C0 + t*A2xB0
    R0 = (extA[0] * fAB[1][0]) + (extA[1] * fAB[0][0]);
    R1 = (extB[1] * fAB[2][2]) + (extB[2] * fAB[2][1]);
    R = (AD0[1] * AB[0][0]) - (AD0[0] * AB[1][0]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[1], AB[0][0], AD1[0], AB[1][0], Rsum, fTime, fIntrTime, 
        uiType, iSide, 13);

    // axis C0 + t*A2xB1
    R0 = (extA[0] * fAB[1][1]) + (extA[1] * fAB[0][1]);
    R1 = (extB[0] * fAB[2][2]) + (extB[2] * fAB[2][0]);
    R = (AD0[1] * AB[0][1]) - (AD0[0] * AB[1][1]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[1], AB[0][1], AD1[0], AB[1][1], Rsum, fTime, fIntrTime, 
        uiType, iSide, 14);

    // axis C0 + t*A2xB2
    R0 = (extA[0] * fAB[1][2]) + (extA[1] * fAB[0][2]);
    R1 = (extB[0] * fAB[2][1]) + (extB[1] * fAB[2][0]);
    R = (AD0[1] * AB[0][2]) - (AD0[0] * AB[1][2]);
    Rsum = R0 + R1;
    BB_FIND1(R, AD1[1], AB[0][2], AD1[0], AB[1][2], Rsum, fTime, fIntrTime, 
        uiType, iSide, 15);

    // determine the point of intersection
    int i, j;
    float x[3], y[3], ad, bd, div, tmp;
    NiPoint3 D;

    switch (uiType)
    {
        case 0:  // Already intersecting.
        {
            NIASSERT(fIntrTime == 0.0f);

            // Use midpoint of line segment connecting box centers.
            kIntrPt = 0.5f * (kBBV0.GetCenter() + kBBV1.GetCenter());

            if (bCalcNormals)
            {
                // Use the direction vector from the first box center to the
                // intersection point for the normal vector.
                kNormal0 = kIntrPt - kBBV0.GetCenter();
            }

            break;
        }
        case 1:  // AXIS_A0    Face Collision
        case 2:  // AXIS_A1
        case 3:  // AXIS_A2
        {
            i = uiType - 1;

            BB_GET_COEFF_MINUS_PLUS(y[0], iSide, AB[i][0], extB[0]);
            BB_GET_COEFF_MINUS_PLUS(y[1], iSide, AB[i][1], extB[1]);
            BB_GET_COEFF_MINUS_PLUS(y[2], iSide, AB[i][2], extB[2]);

            BB_GET_POINT(kIntrPt, kBBV1, fIntrTime, kV1, y);

            // Found point on kBV1, make sure we find one on kBV0, also
            RefinePoint(fTime, &kBBV1, &kV1, &kBBV0, &kV0, fIntrTime,
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[i];
                else
                    kNormal0 = A[i];
                kNormal1 = -kNormal0;
            }

            return true;

            break;
        }
        case 4:  // AXIS_B0
        case 5:  // AXIS_B1
        case 6:  // AXIS_B2
        {
            j = uiType - 4;

            BB_GET_COEFF_PLUS_MINUS(x[0], iSide, AB[0][j], extA[0]);
            BB_GET_COEFF_PLUS_MINUS(x[1], iSide, AB[1][j], extA[1]);
            BB_GET_COEFF_PLUS_MINUS(x[2], iSide, AB[2][j], extA[2]);

            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -B[j];
                else
                    kNormal0 = B[j];
                kNormal1 = -kNormal0;
            }

            return true;

            break;
        }
        case 7:  // AXIS_A0xB0
        {
            BB_GET_COEFF_MINUS_PLUS(x[1], iSide, AB[2][0], extA[1]);
            BB_GET_COEFF_PLUS_MINUS(x[2], iSide, AB[1][0], extA[2]);
            BB_GET_COEFF_MINUS_PLUS(y[1], iSide, AB[0][2], extB[1]);
            BB_GET_COEFF_PLUS_MINUS(y[2], iSide, AB[0][1], extB[2]);
            div = 1.0f - (AB[0][0] * AB[0][0]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[0].Dot(D);
                bd = B[0].Dot(D);
                tmp = (AB[1][0] * x[1]) + (AB[2][0] * x[2]) - bd;
                x[0] = (ad + (AB[0][0] * tmp) + (AB[0][1] * y[1])
                    + (AB[0][2] * y[2])) / div;
            }
            else
            {
                x[0] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[0].Cross(B[0]);
                else
                    kNormal0 = A[0].Cross(B[0]);
            }

            break;
        }
        case 8:  // AXIS_A0xB1
        {
            BB_GET_COEFF_MINUS_PLUS(x[1], iSide, AB[2][1], extA[1]);
            BB_GET_COEFF_PLUS_MINUS(x[2], iSide, AB[1][1], extA[2]);
            BB_GET_COEFF_PLUS_MINUS(y[0], iSide, AB[0][2], extB[0]);
            BB_GET_COEFF_MINUS_PLUS(y[2], iSide, AB[0][0], extB[2]);
            div = 1.0f - (AB[0][1] * AB[0][1]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[0].Dot(D);
                bd = B[1].Dot(D);
                tmp = (AB[1][1] * x[1]) + (AB[2][1] * x[2]) - bd;
                x[0] = (ad + (AB[0][1] * tmp) + (AB[0][0] * y[0])
                    + (AB[0][2] * y[2])) / div;
            }
            else
            {
                x[0] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[0].Cross(B[1]);
                else
                    kNormal0 = A[0].Cross(B[1]);
            }

            break;
        }
        case 9:  // AXIS_A0xB2
        {
            BB_GET_COEFF_MINUS_PLUS(x[1], iSide, AB[2][2], extA[1]);
            BB_GET_COEFF_PLUS_MINUS(x[2], iSide, AB[1][2], extA[2]);
            BB_GET_COEFF_MINUS_PLUS(y[0], iSide, AB[0][1], extB[0]);
            BB_GET_COEFF_PLUS_MINUS(y[1], iSide, AB[0][0], extB[1]);
            div = 1.0f - (AB[0][2] * AB[0][2]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[0].Dot(D);
                bd = B[2].Dot(D);
                tmp = (AB[1][2] * x[1]) + (AB[2][2] * x[2]) - bd;
                x[0] = (ad + (AB[0][2] * tmp) + (AB[0][0] * y[0])
                    + (AB[0][1] * y[1])) / div;
            }
            else
            {
                x[0] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[0].Cross(B[2]);
                else
                    kNormal0 = A[0].Cross(B[2]);
            }

            break;
        }
        case 10:  // AXIS_A1xB0
        {
            BB_GET_COEFF_PLUS_MINUS(x[0], iSide, AB[2][0], extA[0]);
            BB_GET_COEFF_MINUS_PLUS(x[2], iSide, AB[0][0], extA[2]);
            BB_GET_COEFF_MINUS_PLUS(y[1], iSide, AB[1][2], extB[1]);
            BB_GET_COEFF_PLUS_MINUS(y[2], iSide, AB[1][1], extB[2]);
            div = 1.0f - (AB[1][0] * AB[1][0]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[1].Dot(D);
                bd = B[0].Dot(D);
                tmp = (AB[0][0] * x[0]) + (AB[2][0] * x[2]) - bd;
                x[1] = (ad + (AB[1][0] * tmp) + (AB[1][1] * y[1])
                    + (AB[1][2] * y[2])) / div;
            }
            else
            {
                x[1] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[1].Cross(B[0]);
                else
                    kNormal0 = A[1].Cross(B[0]);
            }

            break;
        }
        case 11:  // AXIS_A1xB1
        {
            BB_GET_COEFF_PLUS_MINUS(x[0], iSide, AB[2][1], extA[0]);
            BB_GET_COEFF_MINUS_PLUS(x[2], iSide, AB[0][1], extA[2]);
            BB_GET_COEFF_PLUS_MINUS(y[0], iSide, AB[1][2], extB[0]);
            BB_GET_COEFF_MINUS_PLUS(y[2], iSide, AB[1][0], extB[2]);
            div = 1.0f - (AB[1][1] * AB[1][1]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[1].Dot(D);
                bd = B[1].Dot(D);
                tmp = (AB[0][1] * x[0]) + (AB[2][1] * x[2]) - bd;
                x[1] = (ad + (AB[1][1] * tmp) + (AB[1][0] * y[0])
                    + (AB[1][2] * y[2])) / div;
            }
            else
            {
                x[1] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[1].Cross(B[1]);
                else
                    kNormal0 = A[1].Cross(B[1]);
            }

            break;
        }
        case 12:  // AXIS_A1xB2
        {
            BB_GET_COEFF_PLUS_MINUS(x[0], iSide, AB[2][2], extA[0]);
            BB_GET_COEFF_MINUS_PLUS(x[2], iSide, AB[0][2], extA[2]);
            BB_GET_COEFF_MINUS_PLUS(y[0], iSide, AB[1][1], extB[0]);
            BB_GET_COEFF_PLUS_MINUS(y[1], iSide, AB[1][0], extB[1]);
            div = 1.0f - (AB[1][2] * AB[1][2]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[1].Dot(D);
                bd = B[2].Dot(D);
                tmp = (AB[0][2] * x[0]) + (AB[2][2] * x[2]) - bd;
                x[1] = (ad + (AB[1][2] * tmp) + (AB[1][0] * y[0])
                    + (AB[1][1] * y[1])) / div;
            }
            else
            {
                x[1] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[1].Cross(B[2]);
                else
                    kNormal0 = A[1].Cross(B[2]);
            }

            break;
        }
        case 13:  // AXIS_A2xB0
        {
            BB_GET_COEFF_MINUS_PLUS(x[0], iSide, AB[1][0], extA[0]);
            BB_GET_COEFF_PLUS_MINUS(x[1], iSide, AB[0][0], extA[1]);
            BB_GET_COEFF_MINUS_PLUS(y[1], iSide, AB[2][2], extB[1]);
            BB_GET_COEFF_PLUS_MINUS(y[2], iSide, AB[2][1], extB[2]);
            div = 1.0f - (AB[2][0] * AB[2][0]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[2].Dot(D);
                bd = B[0].Dot(D);
                tmp = (AB[0][0] * x[0]) + (AB[1][0] * x[1]) - bd;
                x[2] = (ad + (AB[2][0] * tmp) + (AB[2][1] * y[1])
                    + (AB[2][2] * y[2])) / div;
            }
            else
            {
                x[2] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);
            
            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[2].Cross(B[0]);
                else
                    kNormal0 = A[2].Cross(B[0]);
            }

            break;
        }
        case 14:  // AXIS_A2xB1
        {
            BB_GET_COEFF_MINUS_PLUS(x[0], iSide, AB[1][1], extA[0]);
            BB_GET_COEFF_PLUS_MINUS(x[1], iSide, AB[0][1], extA[1]);
            BB_GET_COEFF_PLUS_MINUS(y[0], iSide, AB[2][2], extB[0]);
            BB_GET_COEFF_MINUS_PLUS(y[2], iSide, AB[2][0], extB[2]);
            div = 1.0f - (AB[2][1] * AB[2][1]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[2].Dot(D);
                bd = B[1].Dot(D);
                tmp = (AB[0][1] * x[0]) + (AB[1][1] * x[1]) - bd;
                x[2] = (ad + (AB[2][1] * tmp) + (AB[2][0] * y[0])
                    + (AB[2][2] * y[2])) / div;
            }
            else
            {
                x[2] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[2].Cross(B[1]);
                else
                    kNormal0 = A[2].Cross(B[1]);
            }

            break;
        }
        case 15:  // AXIS_A2xB2
        {
            BB_GET_COEFF_MINUS_PLUS(x[0], iSide, AB[1][2], extA[0]);
            BB_GET_COEFF_PLUS_MINUS(x[1], iSide, AB[0][2], extA[1]);
            BB_GET_COEFF_MINUS_PLUS(y[0], iSide, AB[2][1], extB[0]);
            BB_GET_COEFF_PLUS_MINUS(y[1], iSide, AB[2][0], extB[1]);
            div = 1.0f - (AB[2][2] * AB[2][2]);
            if (NiAbs(div) > fEpsilon)
            {
                D = D0 + (fIntrTime * W);
                ad = A[2].Dot(D);
                bd = B[2].Dot(D);
                tmp = (AB[0][2] * x[0]) + (AB[1][2] * x[1]) - bd;
                x[2] = (ad + (AB[2][2] * tmp) + (AB[2][0] * y[0])
                    + (AB[2][1] * y[1])) / div;
            }
            else
            {
                x[2] = 0.0f;
            }
            BB_GET_POINT(kIntrPt, kBBV0, fIntrTime, kV0, x);

            // Found point on kBV0, make sure we find one on kBV1, also
            RefinePoint(fTime, &kBBV0, &kV0, &kBBV1, &kV1, fIntrTime, 
                kIntrPt);

            if (bCalcNormals)
            {
                if (iSide < 0)
                    kNormal0 = -A[2].Cross(B[2]);
                else
                    kNormal0 = A[2].Cross(B[2]);
            }

            break;
        }
    };

    if (bCalcNormals)
    {
        // Often, there's no need to normalize.  Do so only if required.
        const float fNormalSqrLength = kNormal0.SqrLength();
        if (NiAbs(fNormalSqrLength - 1.0f) > FLT_EPSILON)
        {
            if (fNormalSqrLength > FLT_EPSILON)
                kNormal0.Unitize();
            else
                kNormal0 = NiPoint3::UNIT_X;
        }
        kNormal1 = -kNormal0;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoxBV::RefinePoint(float fTime, const NiBoxBV* pkBBV0, 
    const NiPoint3* pkV0, const NiBoxBV* pkBBV1, const NiPoint3* pkV1, 
    float& fIntrTime, NiPoint3& kIntrPt0)
{
    // kIntrPt0 is on kBV0.  If this is not on kBV1, find the point on kBV1
    // that's closest to this point as the final intersection.

    NiSphereBV kSBV;
    kSBV.SetCenter(kIntrPt0);
    kSBV.SetRadius(1e-02f);
    float fClampPoint = 0.99f;

    // If point is found erroneously from Box/Box test, find a good one
    if (!BoxSphereTestIntersect(fTime, *pkBBV0, *pkV0, kSBV, NiPoint3::ZERO))
    {
        // Use vector from intersect point to center of box to determine
        // which box axes to use for intersect point calculation
        NiPoint3 kNewCenter = pkBBV0->GetCenter() + fIntrTime * *pkV0;
        NiPoint3 kFromCenter = kIntrPt0 - kNewCenter;

        // Find projected lengths of bad intersect point against box normals
        float fNrmlAxisProj[3] = {
            (kFromCenter.Dot(pkBBV0->GetAxis(0))),
            (kFromCenter.Dot(pkBBV0->GetAxis(1))),
            (kFromCenter.Dot(pkBBV0->GetAxis(2))) 
        };

        // Clamp to box extents
        if (fNrmlAxisProj[0] > pkBBV0->GetExtent(0) * fClampPoint)
            fNrmlAxisProj[0] = pkBBV0->GetExtent(0);
        else if (fNrmlAxisProj[0] < -pkBBV0->GetExtent(0) * fClampPoint)
            fNrmlAxisProj[0] = -pkBBV0->GetExtent(0);

        if (fNrmlAxisProj[1] > pkBBV0->GetExtent(1) * fClampPoint)
            fNrmlAxisProj[1] = pkBBV0->GetExtent(1);
        else if (fNrmlAxisProj[1] < -pkBBV0->GetExtent(1) * fClampPoint)
            fNrmlAxisProj[1] = -pkBBV0->GetExtent(1);

        if (fNrmlAxisProj[2] > pkBBV0->GetExtent(2) * fClampPoint)
            fNrmlAxisProj[2] = pkBBV0->GetExtent(2);
        else if (fNrmlAxisProj[2] < -pkBBV0->GetExtent(2) * fClampPoint)
            fNrmlAxisProj[2] = -pkBBV0->GetExtent(2);

        kIntrPt0 = kNewCenter + 
            fNrmlAxisProj[0] * pkBBV0->GetAxis(0) +
            fNrmlAxisProj[1] * pkBBV0->GetAxis(1) + 
            fNrmlAxisProj[2] * pkBBV0->GetAxis(2);
    }

    // Exit if this point already on second surface, too.
    if (BoxSphereTestIntersect(fTime, *pkBBV1, *pkV1, kSBV, NiPoint3::ZERO))
        return;
    
    // Select another point from other surface 
    NiPoint3 kNewCenter = pkBBV1->GetCenter() + fIntrTime * *pkV1;

    // Use vector from intersect point to center of box to determine which box
    // axes to use for intersect point calculation.
    NiPoint3 kFromCenter = kIntrPt0 - kNewCenter;

    // Find projected lengths of bad intersect point against box normals.
    float fNrmlAxisProj[3] = {
        (kFromCenter.Dot(pkBBV1->GetAxis(0))),
        (kFromCenter.Dot(pkBBV1->GetAxis(1))),
        (kFromCenter.Dot(pkBBV1->GetAxis(2))) 
    };

    // Clamp to box extents.
    if (fNrmlAxisProj[0] > pkBBV1->GetExtent(0) * fClampPoint)
        fNrmlAxisProj[0] = pkBBV1->GetExtent(0);
    else if (fNrmlAxisProj[0] < -pkBBV1->GetExtent(0) * fClampPoint)
        fNrmlAxisProj[0] = -pkBBV1->GetExtent(0);

    if (fNrmlAxisProj[1] > pkBBV1->GetExtent(1) * fClampPoint)
        fNrmlAxisProj[1] = pkBBV1->GetExtent(1);
    else if (fNrmlAxisProj[1] < -pkBBV1->GetExtent(1) * fClampPoint)
        fNrmlAxisProj[1] = -pkBBV1->GetExtent(1);

    if (fNrmlAxisProj[2] > pkBBV1->GetExtent(2) * fClampPoint)
        fNrmlAxisProj[2] = pkBBV1->GetExtent(2);
    else if (fNrmlAxisProj[2] < -pkBBV1->GetExtent(2) * fClampPoint)
        fNrmlAxisProj[2] = -pkBBV1->GetExtent(2);

    kIntrPt0 = kNewCenter + 
        fNrmlAxisProj[0] * pkBBV1->GetAxis(0) +
        fNrmlAxisProj[1] * pkBBV1->GetAxis(1) + 
        fNrmlAxisProj[2] * pkBBV1->GetAxis(2);
}
//---------------------------------------------------------------------------
NiPoint3 NiBoxBV::GetNormal(int iSign0, int iSign1, int iSign2,
    const NiPoint3 kVelocity) const
{
    // This function considers the relative velocity of the 2 objects in
    // order to ignore contributions from box face normals that are
    // perpendicular to this relative velocity.  Otherwise, returned normals
    // will be biased toward edges over faces, even when that bias does not
    // make sense.  The values iSign0, iSign1, and iSign2 are in the discrete
    // set {-1, 0, +1}/
    unsigned int uiAxisCount = 0;
    NiPoint3 kNormal = NiPoint3::ZERO;
    bool bZeroVelocity = ((kVelocity == NiPoint3::ZERO) ? (true) : (false));

    // Determine normal.

    if ((bZeroVelocity)
        || (NiAbs(kVelocity.Dot(m_kBox.m_akAxis[0])) > FLT_EPSILON))
    {
        if (iSign0 == 1)
        {
            kNormal += m_kBox.m_akAxis[0];
            uiAxisCount++;
        }
        else if (iSign0 == -1)
        {
            kNormal -= m_kBox.m_akAxis[0];
            uiAxisCount++;
        }
    }

    if ((bZeroVelocity)
        || (NiAbs(kVelocity.Dot(m_kBox.m_akAxis[1])) > FLT_EPSILON))
    {
        if (iSign1 == 1)
        {
            kNormal += m_kBox.m_akAxis[1];
            uiAxisCount++;
        }
        else if (iSign1 == -1)
        {
            kNormal -= m_kBox.m_akAxis[1];
            uiAxisCount++;
        }
    }

    if ((bZeroVelocity)
        || (NiAbs(kVelocity.Dot(m_kBox.m_akAxis[2])) > FLT_EPSILON))
    {
        if (iSign2 == 1)
        {
            kNormal += m_kBox.m_akAxis[2];
            uiAxisCount++;
        }
        else if (iSign2 == -1)
        {
            kNormal -= m_kBox.m_akAxis[2];
            uiAxisCount++;
        }
    }

    // Unitize only if there is more than one face normal being summed.
    if (uiAxisCount != 1)
        kNormal.Unitize();

    return kNormal;
}
//---------------------------------------------------------------------------
bool NiBoxBV::BoxCapsuleFindIntersect(float fTime, 
    const NiBoundingVolume& kBV0, const NiPoint3& kV0, 
    const NiBoundingVolume& kBV1, const NiPoint3& kV1, 
    float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals, 
    NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    NiBoxCapsuleIntersector kIntersector((const NiBoxBV&)kBV0,
        (const NiCapsuleBV&)kBV1, fTime);

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
bool NiBoxBV::BoxTriFindIntersect(float fTime, const NiBoundingVolume& kBV0,
    const NiPoint3& kV0, const NiPoint3& kVert0, const NiPoint3& kVert1,
    const NiPoint3& kVert2, const NiPoint3& kV1, float& fIntrTime,
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0,
    NiPoint3& kNormal1)
{
    const NiBoxBV& kBBV = (const NiBoxBV&) kBV0;

    // Convenience variables.
    const NiPoint3* A = kBBV.GetAxes();
    const float* extA = kBBV.GetExtents();

    // Compute relative velocity of triangle with respect to box so that box
    // may as well be stationary.
    NiPoint3 W = kV1 - kV0;

    // Construct triangle normal, difference of kCenter and vertex.
    NiPoint3 D;
    NiPoint3 E[3];
    NiPoint3 N;
    E[0] = kVert1 - kVert0;
    E[1] = kVert2 - kVert0;
    N = E[0].Cross(E[1]);
    D = kVert0 - kBBV.GetCenter();

    // Track maximum time of projection - intersection.
    unsigned int uiType = 0;
    unsigned int uiExtr = 0;
    int iSide = 0;
    fIntrTime = 0.0f;
    int i;

    // axis C + t*N
    float AN[3];
    AN[0] = A[0].Dot(N);
    AN[1] = A[1].Dot(N);
    AN[2] = A[2].Dot(N);
    float R = (extA[0] * NiAbs(AN[0])) + (extA[1] * NiAbs(AN[1]))
        + (extA[2] * NiAbs(AN[2]));
    float NdD = N.Dot(D);
    BT_FIND0(NdD, R, fTime, N, W, fIntrTime, uiType, iSide, 1);

    float AD[3];
    float AE[3][3];

    for (i = 0; i < 3; i++)
    {
        // axis C + t*Ai
        AD[i] = A[i].Dot(D);
        AE[i][0] = A[i].Dot(E[0]);
        AE[i][1] = A[i].Dot(E[1]);
        float AidW = A[i].Dot(W);
        BT_FIND1(AD[i], fTime, AidW, AE[i][0], AE[i][1], extA[i], fIntrTime,
            uiType, uiExtr, iSide, i+2);
    }

    // axis C + t*A0xE0
    NiPoint3 A0xE0;
    A0xE0 = A[0].Cross(E[0]);
    float A0xE0dD = A0xE0.Dot(D);
    float A0xE0dW = A0xE0.Dot(W);
    R = (extA[1] * NiAbs(AE[2][0])) + (extA[2] * NiAbs(AE[1][0]));
    BT_FIND2(A0xE0dD, fTime, A0xE0dW, AN[0], R, fIntrTime, uiType, uiExtr, 
        iSide, 5);

    // axis C + t*A0xE1
    NiPoint3 A0xE1;
    A0xE1 = A[0].Cross(E[1]);
    float A0xE1dD = A0xE1.Dot(D);
    float A0xE1dW = A0xE1.Dot(W);
    R = (extA[1] * NiAbs(AE[2][1])) + (extA[2] * NiAbs(AE[1][1]));
    BT_FIND2(A0xE1dD, fTime, A0xE1dW, -AN[0], R, fIntrTime, uiType, uiExtr, 
        iSide, 6);

    // axis C + t*A0xE2
    AE[1][2] = AE[1][1] - AE[1][0];
    AE[2][2] = AE[2][1] - AE[2][0];
    float A0xE2dD = A0xE1dD - A0xE0dD;
    float A0xE2dW = A0xE1dW - A0xE0dW;
    R = (extA[1] * NiAbs(AE[2][2])) + (extA[2] * NiAbs(AE[1][2]));
    BT_FIND2(A0xE2dD, fTime, A0xE2dW, -AN[0], R, fIntrTime, uiType, uiExtr, 
        iSide, 7);

    // axis C + t*A1xE0
    NiPoint3 A1xE0;
    A1xE0 = A[1].Cross(E[0]);
    float A1xE0dD = A1xE0.Dot(D);
    float A1xE0dW = A1xE0.Dot(W);
    R = (extA[0] * NiAbs(AE[2][0])) + (extA[2] * NiAbs(AE[0][0]));
    BT_FIND2(A1xE0dD, fTime, A1xE0dW, AN[1], R, fIntrTime, uiType, uiExtr, 
        iSide, 8);

    // axis C + t*A1xE1
    NiPoint3 A1xE1;
    A1xE1 = A[1].Cross(E[1]);
    float A1xE1dD = A1xE1.Dot(D);
    float A1xE1dW = A1xE1.Dot(W);
    R = (extA[0] * NiAbs(AE[2][1])) + (extA[2] * NiAbs(AE[0][1]));
    BT_FIND2(A1xE1dD, fTime, A1xE1dW, -AN[1], R, fIntrTime, uiType, uiExtr, 
        iSide, 9);

    // axis C + t*A1xE2
    AE[0][2] = AE[0][1] - AE[0][0];
    float A1xE2dD = A1xE1dD - A1xE0dD;
    float A1xE2dW = A1xE1dW - A1xE0dW;
    R = (extA[0] * NiAbs(AE[2][2])) + (extA[2] * NiAbs(AE[0][2]));
    BT_FIND2(A1xE2dD, fTime, A1xE2dW, -AN[1], R, fIntrTime, uiType, uiExtr, 
        iSide, 10);

    // axis C + t*A2xE0
    NiPoint3 A2xE0;
    A2xE0 = A[2].Cross(E[0]);
    float A2xE0dD = A2xE0.Dot(D);
    float A2xE0dW = A2xE0.Dot(W);
    R = (extA[0] * NiAbs(AE[1][0])) + (extA[1] * NiAbs(AE[0][0]));
    BT_FIND2(A2xE0dD, fTime, A2xE0dW, AN[2], R, fIntrTime, uiType, uiExtr, 
        iSide, 11);

    // axis C + t*A2xE1
    NiPoint3 A2xE1;
    A2xE1 = A[2].Cross(E[1]);
    float A2xE1dD = A2xE1.Dot(D);
    float A2xE1dW = A2xE1.Dot(W);
    R = (extA[0] * NiAbs(AE[1][1])) + (extA[1] * NiAbs(AE[0][1]));
    BT_FIND2(A2xE1dD, fTime, A2xE1dW, -AN[2], R, fIntrTime, uiType, uiExtr, 
        iSide, 12);

    // axis C + t*A2xE2
    float A2xE2dD = A2xE1dD - A2xE0dD;
    float A2xE2dW = A2xE1dW - A2xE0dW;
    R = (extA[0] * NiAbs(AE[1][2])) + (extA[1] * NiAbs(AE[0][2]));
    BT_FIND2(A2xE2dD, fTime, A2xE2dW, -AN[2], R, fIntrTime, uiType, uiExtr, 
        iSide, 13);

    // determine the point of intersection
    float x[3];
    float fNumer;
    float NDE[3];
    float NAE[3][3];
    NiPoint3 DxE[3];
    NiPoint3 A0xE2;
    NiPoint3 A1xE2;
    NiPoint3 A2xE2;

    switch (uiType)
    {
        case 0:  // Already intersecting.
        {
            // Use midpoint of line segment connecting box kCenter and
            // triangle centroid.
            NIASSERT(fIntrTime == 0.0f);
            static float fOneThird = 1.0f / 3.0f;
            kIntrPt = 0.5f * (fOneThird * (kVert0 + kVert1 + kVert2) + 
                kBBV.GetCenter());
            break;
        }
        case 1:  // AXIS_N
        {
            BT_GET_COEFF_PLUS_MINUS(x[0], iSide, AN[0], extA[0]);
            BT_GET_COEFF_PLUS_MINUS(x[1], iSide, AN[1], extA[1]);
            BT_GET_COEFF_PLUS_MINUS(x[2], iSide, AN[2], extA[2]);

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 2:  // AXIS_A0
        case 3:  // AXIS_A1
        case 4:  // AXIS_A2
        {
            if (uiExtr == 0)    // y0 = 0, y1 = 0
                kIntrPt = kVert0 + (fIntrTime * kV1);
            else if (uiExtr == 1)   // y0 = 1, y1 = 0
                kIntrPt = kVert0 + (fIntrTime * kV1) + E[0];
            else    // uiExtr == 2     y0 = 0, y1 = 1
                kIntrPt = kVert0 + (fIntrTime * kV1) + E[1];
            break;
        }
        case 5:  // AXIS_A0xE0
        {
            BT_GET_COEFF_MINUS_PLUS(x[1], iSide, AE[2][0], extA[1]);
            BT_GET_COEFF_PLUS_MINUS(x[2], iSide, AE[1][0], extA[2]);
            D += (fIntrTime * W);
            DxE[0] = D.Cross(E[0]);
            NDE[0] = A0xE0.Dot(DxE[0]);
            NAE[0][0] = A0xE0.Dot(A0xE0);
            NAE[1][0] = A0xE0.Dot(A1xE0);
            NAE[2][0] = A0xE0.Dot(A2xE0);
            fNumer = NDE[0] - (NAE[1][0] * x[1]) - (NAE[2][0] * x[2]);
            if (uiExtr == 0) // y1 = 0
                x[0] = fNumer / NAE[0][0];
            else  // y1 = 1, uiExtr == 1
                x[0] = (fNumer - A0xE0.Dot(N)) / NAE[0][0];
 
            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 6:  // AXIS_A0xE1
        {
            BT_GET_COEFF_MINUS_PLUS(x[1], iSide, AE[2][1], extA[1]);
            BT_GET_COEFF_PLUS_MINUS(x[2], iSide, AE[1][1], extA[2]);
            D += (fIntrTime * W);
            DxE[1] = D.Cross(E[1]);
            NDE[1] = A0xE1.Dot(DxE[1]);
            NAE[0][1] = A0xE1.Dot(A0xE1);
            NAE[1][1] = A0xE1.Dot(A1xE1);
            NAE[2][1] = A0xE1.Dot(A2xE1);
            fNumer = NDE[1] - (NAE[1][1] * x[1]) - (NAE[2][1] * x[2]);
            if (uiExtr == 0) // y0 = 0                
                x[0] = fNumer / NAE[0][1];
            else  // y0 = 1, uiExtr == 1
                x[0] = (fNumer + A0xE1.Dot(N)) / NAE[0][1];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 7:  // AXIS_A0xE2
        {
            E[2] = E[1] - E[0];
            A0xE2 = A[0].Cross(E[2]);
            A1xE2 = A[1].Cross(E[2]);
            A2xE2 = A[2].Cross(E[2]);

            BT_GET_COEFF_MINUS_PLUS(x[1], iSide, AE[2][2], extA[1]);
            BT_GET_COEFF_PLUS_MINUS(x[2], iSide, AE[1][2], extA[2]);
            D += (fIntrTime * W);
            DxE[2] = D.Cross(E[2]);
            NDE[2] = A0xE2.Dot(DxE[2]);
            NAE[0][2] = A0xE2.Dot(A0xE2);
            NAE[1][2] = A0xE2.Dot(A1xE2);
            NAE[2][2] = A0xE2.Dot(A2xE2);
            fNumer = NDE[2] - (NAE[1][2] * x[1]) - (NAE[2][2] * x[2]);
            if (uiExtr == 0) // y0 + y1 = 0
                x[0] = fNumer / NAE[0][2];
            else  // y0 + y1 = 1, uiExtr == 1
                x[0] = (fNumer + A0xE2.Dot(N)) / NAE[0][2];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 8:  // AXIS_A1xE0
        {
            BT_GET_COEFF_PLUS_MINUS(x[0], iSide, AE[2][0], extA[0]);
            BT_GET_COEFF_MINUS_PLUS(x[2], iSide, AE[0][0], extA[2]);
            D += (fIntrTime * W);
            DxE[0] = D.Cross(E[0]);
            NDE[0] = A1xE0.Dot(DxE[0]);
            NAE[0][0] = A1xE0.Dot(A0xE0);
            NAE[1][0] = A1xE0.Dot(A1xE0);
            NAE[2][0] = A1xE0.Dot(A2xE0);
            fNumer = NDE[0] - (NAE[0][0] * x[0]) - (NAE[2][0] * x[2]);
            if (uiExtr == 0) // y1 = 0
                x[1] = fNumer / NAE[1][0];
            else  // y1 = 1, uiExtr == 1
                x[1] = (fNumer - A1xE0.Dot(N)) / NAE[1][0];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 9:  // AXIS_A1xE1
        {
            BT_GET_COEFF_PLUS_MINUS(x[0], iSide, AE[2][1], extA[0]);
            BT_GET_COEFF_MINUS_PLUS(x[2], iSide, AE[0][1], extA[2]);
            D += (fIntrTime * W);
            DxE[1] = D.Cross(E[1]);
            NDE[1] = A1xE1.Dot(DxE[1]);
            NAE[0][1] = A1xE1.Dot(A0xE1);
            NAE[1][1] = A1xE1.Dot(A1xE1);
            NAE[2][1] = A1xE1.Dot(A2xE1);
            fNumer = NDE[1] - (NAE[0][1] * x[0]) - (NAE[2][1] * x[2]);
            if (uiExtr == 0) // y0 = 0
                x[1] = fNumer / NAE[1][1];
            else  // y0 = 1, uiExtr == 1
                x[1] = (fNumer + A1xE1.Dot(N)) / NAE[1][1];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 10:  // AXIS_A1xE2
        {
            E[2] = E[1] - E[0];
            A0xE2 = A[0].Cross(E[2]);
            A1xE2 = A[1].Cross(E[2]);
            A2xE2 = A[2].Cross(E[2]);

            BT_GET_COEFF_PLUS_MINUS(x[0], iSide, AE[2][2], extA[0]);
            BT_GET_COEFF_MINUS_PLUS(x[2], iSide, AE[0][2], extA[2]);
            D += (fIntrTime * W);
            DxE[2] = D.Cross(E[2]);
            NDE[2] = A1xE2.Dot(DxE[2]);
            NAE[0][2] = A1xE2.Dot(A0xE2);
            NAE[1][2] = A1xE2.Dot(A1xE2);
            NAE[2][2] = A1xE2.Dot(A2xE2);
            fNumer = NDE[2] - (NAE[0][2] * x[0]) - (NAE[2][2] * x[2]);
            if (uiExtr == 0) // y0 + y1 = 0
                x[1] = fNumer / NAE[1][2];
            else  // y0 + y1 = 1, uiExtr == 1
                x[1] = (fNumer + A1xE2.Dot(N)) / NAE[1][2];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 11:  // AXIS_A2xE0
        {
            BT_GET_COEFF_MINUS_PLUS(x[0], iSide, AE[1][0], extA[0]);
            BT_GET_COEFF_PLUS_MINUS(x[1], iSide, AE[0][0], extA[1]);
            D += (fIntrTime * W);
            DxE[0] = D.Cross(E[0]);
            NDE[0] = A2xE0.Dot(DxE[0]);
            NAE[0][0] = A2xE0.Dot(A0xE0);
            NAE[1][0] = A2xE0.Dot(A1xE0);
            NAE[2][0] = A2xE0.Dot(A2xE0);
            fNumer = NDE[0] - (NAE[0][0] * x[0]) - (NAE[1][0] * x[1]);
            if (uiExtr == 0) // y1 = 0
                x[2] = fNumer / NAE[2][0];
            else  // y1 = 1, uiExtr == 1
                x[2] = (fNumer - A2xE0.Dot(N)) / NAE[2][0];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 12:  // AXIS_A2xE1
        {
            BT_GET_COEFF_MINUS_PLUS(x[0], iSide, AE[1][1], extA[0]);
            BT_GET_COEFF_PLUS_MINUS(x[1], iSide, AE[0][1], extA[1]);
            D += (fIntrTime * W);
            DxE[1] = D.Cross(E[1]);
            NDE[1] = A2xE1.Dot(DxE[1]);
            NAE[0][1] = A2xE1.Dot(A0xE1);
            NAE[1][1] = A2xE1.Dot(A1xE1);
            NAE[2][1] = A2xE1.Dot(A2xE1);
            fNumer = NDE[1] - (NAE[0][1] * x[0]) - (NAE[1][1] * x[1]);
            if (uiExtr == 0) // y0 = 0
                x[2] = fNumer / NAE[2][1];
            else  // y0 = 1, uiExtr == 1
                x[2] = (fNumer + A2xE1.Dot(N)) / NAE[2][1];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
        case 13:  // AXIS_A2xE2
        {
            E[2] = E[1] - E[0];
            A0xE2 = A[0].Cross(E[2]);
            A1xE2 = A[1].Cross(E[2]);
            A2xE2 = A[2].Cross(E[2]);

            BT_GET_COEFF_MINUS_PLUS(x[0], iSide, AE[1][2], extA[0]);
            BT_GET_COEFF_PLUS_MINUS(x[1], iSide, AE[0][2], extA[1]);
            D += (fIntrTime * W);
            DxE[2] = D.Cross(E[2]);
            NDE[2] = A2xE2.Dot(DxE[2]);
            NAE[0][2] = A2xE2.Dot(A0xE2);
            NAE[1][2] = A2xE2.Dot(A1xE2);
            NAE[2][2] = A2xE2.Dot(A2xE2);
            fNumer = NDE[2] - (NAE[0][2] * x[0]) - (NAE[1][2] * x[1]);
            if (uiExtr == 0) // y0 + y1 = 0
                x[2] = fNumer / NAE[2][2];
            else  // y0 + y1 = 1, uiExtr == 1
                x[2] = (fNumer + A2xE2.Dot(N)) / NAE[2][2];

            BT_GET_POINT(kIntrPt, kBBV, fIntrTime, kV0, x);
            break;
        }
    }

    // Box & triangle intersect.  Conditionally, calculate normals.
    if (bCalcNormals)       
    {
        // Triangle normal at intersection point is N (edge0 x edge1).
        kNormal1 = N;
        kNormal1.Unitize();
        kNormal0 = -kNormal1;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning.
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoxBV::Clone() const
{
    NiBoxBV* pkClone = NiNew NiBoxBV(*this);
    NIASSERT(pkClone);
    return (NiBoundingVolume*) pkClone;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming.
//---------------------------------------------------------------------------
NiBoundingVolume* NiBoxBV::CreateFromStream(NiStream& kStream)
{
    NiBoxBV* pABV = NiNew NiBoxBV;
    NIASSERT(pABV);
    pABV->LoadBinary(kStream);
    return pABV;
}
//---------------------------------------------------------------------------
void NiBoxBV::LoadBinary(NiStream& kStream)
{
    NiBoundingVolume::LoadBinary(kStream);
    m_kBox.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBoxBV::SaveBinary(NiStream& kStream)
{
    NiBoundingVolume::SaveBinary(kStream);
    m_kBox.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBoxBV::AddViewerStrings(const char* pcPrefix, 
    NiViewerStringsArray* pkStrings) const
{
    size_t stLen = strlen(pcPrefix) + 10;
    char* pString = NiAlloc(char, stLen);

    NiSprintf(pString, stLen, "%s = BOX_BV", pcPrefix);
    pkStrings->Add(pString);
}
//---------------------------------------------------------------------------
