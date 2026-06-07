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

#include <NiMath.h>
#include <NiMatrix3.h>
#include "NiOBBRoot.h"
#include "NiOBBox.h"

// flags for separation axis test
enum
{
    intrNone, intrA0, intrA1, intrA2, intrB0, intrB1, intrB2,
    intrA0B0, intrA0B1, intrA0B2, intrA1B0, intrA1B1, intrA1B2,
    intrA2B0, intrA2B1, intrA2B2
};

const float FLOAT_EPSILON = 1.0e-07f;

//---------------------------------------------------------------------------
void NiOBBox::Transform(const NiTransform& xform)
{
    m_wCenter = xform.m_fScale*(xform.m_Rotate*m_center) + xform.m_Translate;
    m_wBasis[0] = xform.m_Rotate*m_basis[0];
    m_wBasis[1] = xform.m_Rotate*m_basis[1];
    m_wBasis[2] = xform.m_Rotate*m_basis[2];
    m_wExtent[0] = xform.m_fScale*m_extent[0];
    m_wExtent[1] = xform.m_fScale*m_extent[1];
    m_wExtent[2] = xform.m_fScale*m_extent[2];
}
//---------------------------------------------------------------------------
int NiOBBox::TestIntersection(const NiOBBox& box)
{
    // name shortcuts
    const NiPoint3* A = m_wBasis;
    const NiPoint3* B = box.m_wBasis;
    const float* extA = m_wExtent;
    const float* extB = box.m_wExtent;

    float r0, r1, rsum, u0, u1;

    // memoized values for use in edge-edge separation tests
    float C[3][3];
    float d0[3], d1[3];

    // compute difference of centers and velocities
    NiPoint3 D = box.m_wCenter - m_wCenter;
    NiPoint3 V = box.m_pRoot->GetVelocity() - m_pRoot->GetVelocity();

    // time step for this frame
    float dt = m_pRoot->GetDeltaTime();

    // L = A0
    C[0][0] = A[0]*B[0];
    C[0][1] = A[0]*B[1];
    C[0][2] = A[0]*B[2];
    d0[0] = A[0]*D;
    d1[0] = d0[0]+dt*(A[0]*V);
    r0 = NiAbs(extA[0]);
    r1 = NiAbs(extB[0]*C[0][0])+NiAbs(extB[1]*C[0][1])+NiAbs(extB[2]*C[0][2]);
    rsum = r0+r1;
    if ( (d0[0] > rsum && d1[0] > rsum) || (d0[0] < -rsum && d1[0] < -rsum) )
        return 0;

    // L = A1
    C[1][0] = A[1]*B[0];
    C[1][1] = A[1]*B[1];
    C[1][2] = A[1]*B[2];
    d0[1] = A[1]*D;
    d1[1] = d0[1]+dt*(A[1]*V);
    r0 = NiAbs(extA[1]);
    r1 = NiAbs(extB[0]*C[1][0])+NiAbs(extB[1]*C[1][1])+NiAbs(extB[2]*C[1][2]);
    rsum = r0+r1;
    if ( (d0[1] > rsum && d1[1] > rsum) || (d0[1] < -rsum && d1[1] < -rsum) )
        return 0;

    // L = A2
    C[2][0] = A[2]*B[0];
    C[2][1] = A[2]*B[1];
    C[2][2] = A[2]*B[2];
    d0[2] = A[2]*D;
    d1[2] = d0[2]+dt*(A[2]*V);
    r0 = NiAbs(extA[2]);
    r1 = NiAbs(extB[0]*C[2][0])+NiAbs(extB[1]*C[2][1])+NiAbs(extB[2]*C[2][2]);
    rsum = r0+r1;
    if ( (d0[2] > rsum && d1[2] > rsum) || (d0[2] < -rsum && d1[2] < -rsum) )
        return 0;

    // L = B0
    r0 = NiAbs(extA[0]*C[0][0])+NiAbs(extA[1]*C[1][0])+NiAbs(extA[2]*C[2][0]);
    r1 = NiAbs(extB[0]);
    rsum = r0+r1;
    u0 = B[0]*D;
    u1 = u0+dt*(B[0]*V);
    if ( (u0 > rsum && u1 > rsum) || (u0 < -rsum && u1 < -rsum) )
        return 0;

    // L = B1
    r0 = NiAbs(extA[0]*C[0][1])+NiAbs(extA[1]*C[1][1])+NiAbs(extA[2]*C[2][1]);
    r1 = NiAbs(extB[1]);
    rsum = r0+r1;
    u0 = B[1]*D;
    u1 = u0+dt*(B[1]*V);
    if ( (u0 > rsum && u1 > rsum) || (u0 < -rsum && u1 < -rsum) )
        return 0;

    // L = B2
    r0 = NiAbs(extA[0]*C[0][2])+NiAbs(extA[1]*C[1][2])+NiAbs(extA[2]*C[2][2]);
    r1 = NiAbs(extB[2]);
    rsum = r0+r1;
    u0 = B[2]*D;
    u1 = u0+dt*(B[2]*V);
    if ( (u0 > rsum && u1 > rsum) || (u0 < -rsum && u1 < -rsum) )
        return 0;

    // L = A0xB0
    r0 = NiAbs(extA[1]*C[2][0])+NiAbs(extA[2]*C[1][0]);
    r1 = NiAbs(extB[1]*C[0][2])+NiAbs(extB[2]*C[0][1]);
    rsum = r0+r1;
    u0 = d0[2]*C[1][0]-d0[1]*C[2][0];
    if ( u0 > rsum )
    {
        u1 = d1[2]*C[1][0]-d1[1]*C[2][0];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[2]*C[1][0]-d1[1]*C[2][0];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A0xB1
    r0 = NiAbs(extA[1]*C[2][1])+NiAbs(extA[2]*C[1][1]);
    r1 = NiAbs(extB[0]*C[0][2])+NiAbs(extB[2]*C[0][0]);
    rsum = r0+r1;
    u0 = d0[2]*C[1][1]-d0[1]*C[2][1];
    if ( u0 > rsum )
    {
        u1 = d1[2]*C[1][1]-d1[1]*C[2][1];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[2]*C[1][1]-d1[1]*C[2][1];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A0xB2
    r0 = NiAbs(extA[1]*C[2][2])+NiAbs(extA[2]*C[1][2]);
    r1 = NiAbs(extB[0]*C[0][1])+NiAbs(extB[1]*C[0][0]);
    rsum = r0+r1;
    u0 = d0[2]*C[1][2]-d0[1]*C[2][2];
    if ( u0 > rsum )
    {
        u1 = d1[2]*C[1][2]-d1[1]*C[2][2];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[2]*C[1][2]-d1[1]*C[2][2];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A1xB0
    r0 = NiAbs(extA[0]*C[2][0])+NiAbs(extA[2]*C[0][0]);
    r1 = NiAbs(extB[1]*C[1][2])+NiAbs(extB[2]*C[1][1]);
    rsum = r0+r1;
    u0 = d0[0]*C[2][0]-d0[2]*C[0][0];
    if ( u0 > rsum )
    {
        u1 = d1[0]*C[2][0]-d1[2]*C[0][0];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[0]*C[2][0]-d1[2]*C[0][0];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A1xB1
    r0 = NiAbs(extA[0]*C[2][1])+NiAbs(extA[2]*C[0][1]);
    r1 = NiAbs(extB[0]*C[1][2])+NiAbs(extB[2]*C[1][0]);
    rsum = r0+r1;
    u0 = d0[0]*C[2][1]-d0[2]*C[0][1];
    if ( u0 > rsum )
    {
        u1 = d1[0]*C[2][1]-d1[2]*C[0][1];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[0]*C[2][1]-d1[2]*C[0][1];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A1xB2
    r0 = NiAbs(extA[0]*C[2][2])+NiAbs(extA[2]*C[0][2]);
    r1 = NiAbs(extB[0]*C[1][1])+NiAbs(extB[1]*C[1][0]);
    rsum = r0+r1;
    u0 = d0[0]*C[2][2]-d0[2]*C[0][2];
    if ( u0 > rsum )
    {
        u1 = d1[0]*C[2][2]-d1[2]*C[0][2];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[0]*C[2][2]-d1[2]*C[0][2];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A2xB0
    r0 = NiAbs(extA[0]*C[1][0])+NiAbs(extA[1]*C[0][0]);
    r1 = NiAbs(extB[1]*C[2][2])+NiAbs(extB[2]*C[2][1]);
    rsum = r0+r1;
    u0 = d0[1]*C[0][0]-d0[0]*C[1][0];
    if ( u0 > rsum )
    {
        u1 = d1[1]*C[0][0]-d1[0]*C[1][0];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[1]*C[0][0]-d1[0]*C[1][0];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A2xB1
    r0 = NiAbs(extA[0]*C[1][1])+NiAbs(extA[1]*C[0][1]);
    r1 = NiAbs(extB[0]*C[2][2])+NiAbs(extB[2]*C[2][0]);
    rsum = r0+r1;
    u0 = d0[1]*C[0][1]-d0[0]*C[1][1];
    if ( u0 > rsum )
    {
        u1 = d1[1]*C[0][1]-d1[0]*C[1][1];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[1]*C[0][1]-d1[0]*C[1][1];
        if ( u1 < -rsum )
            return 0;
    }

    // L = A2xB2
    r0 = NiAbs(extA[0]*C[1][2])+NiAbs(extA[1]*C[0][2]);
    r1 = NiAbs(extB[0]*C[2][1])+NiAbs(extB[1]*C[2][0]);
    rsum = r0+r1;
    u0 = d0[1]*C[0][2]-d0[0]*C[1][2];
    if ( u0 > rsum )
    {
        u1 = d1[1]*C[0][2]-d1[0]*C[1][2];
        if ( u1 > rsum )
            return 0;
    }
    else if ( u0 < -rsum )
    {
        u1 = d1[1]*C[0][2]-d1[0]*C[1][2];
        if ( u1 < -rsum )
            return 0;
    }

    // At this point none of the 15 axes separate the boxes.  It is still
    // possible that they are separated as viewed in any plane orthogonal
    // to the relative direction of motion W.  In the worst case, the two
    // projected boxes are hexagons.  This requires three separating axis
    // tests per box.
    NiPoint3 kVxD = V.Cross(D);
    float afVA[3], afVB[3], R, R0, R1, R01;

    // axis C0 + t*VxA0
    afVA[1] = V.Dot(A[1]);
    afVA[2] = V.Dot(A[2]);
    R = NiAbs(A[0].Dot(kVxD));
    R0 = extA[1]*NiAbs(afVA[2]) + extA[2]*NiAbs(afVA[1]);
    R1 =
        extB[0]*NiAbs(C[1][0]*afVA[2] - C[2][0]*afVA[1]) +
        extB[1]*NiAbs(C[1][1]*afVA[2] - C[2][1]*afVA[1]) +
        extB[2]*NiAbs(C[1][2]*afVA[2] - C[2][2]*afVA[1]);
    R01 = R0 + R1;
    if (R > R01)
        return 0;

    // axis C0 + t*VxA1
    afVA[0] = V.Dot(A[0]);
    R = NiAbs(A[1].Dot(kVxD));
    R0 = extA[2]*NiAbs(afVA[0]) + extA[0]*NiAbs(afVA[2]);
    R1 =
        extB[0]*NiAbs(C[2][0]*afVA[0] - C[0][0]*afVA[2]) +
        extB[1]*NiAbs(C[2][1]*afVA[0] - C[0][1]*afVA[2]) +
        extB[2]*NiAbs(C[2][2]*afVA[0] - C[0][2]*afVA[2]);
    R01 = R0 + R1;
    if (R > R01)
        return 0;

    // axis C0 + t*VxA2
    R = NiAbs(A[2].Dot(kVxD));
    R0 = extA[0]*NiAbs(afVA[1]) + extA[1]*NiAbs(afVA[0]);
    R1 =
        extB[0]*NiAbs(C[0][0]*afVA[1] - C[1][0]*afVA[0]) +
        extB[1]*NiAbs(C[0][1]*afVA[1] - C[1][1]*afVA[0]) +
        extB[2]*NiAbs(C[0][2]*afVA[1] - C[1][2]*afVA[0]);
    R01 = R0 + R1;
    if (R > R01)
        return 0;

    // axis C0 + t*VxB0
    afVB[1] = V.Dot(B[1]);
    afVB[2] = V.Dot(B[2]);
    R = NiAbs(B[0].Dot(kVxD));
    R0 =
        extA[0]*NiAbs(C[0][1]*afVB[2] - C[0][2]*afVB[1]) +
        extA[1]*NiAbs(C[1][1]*afVB[2] - C[1][2]*afVB[1]) +
        extA[2]*NiAbs(C[2][1]*afVB[2] - C[2][2]*afVB[1]);
    R1 = extB[1]*NiAbs(afVB[2]) + extB[2]*NiAbs(afVB[1]);
    R01 = R0 + R1;
    if (R > R01)
        return 0;

    // axis C0 + t*VxB1
    afVB[0] = V.Dot(B[0]);
    R = NiAbs(B[1].Dot(kVxD));
    R0 =
        extA[0]*NiAbs(C[0][2]*afVB[0] - C[0][0]*afVB[2]) +
        extA[1]*NiAbs(C[1][2]*afVB[0] - C[1][0]*afVB[2]) +
        extA[2]*NiAbs(C[2][2]*afVB[0] - C[2][0]*afVB[2]);
    R1 = extB[2]*NiAbs(afVB[0]) + extB[0]*NiAbs(afVB[2]);
    R01 = R0 + R1;
    if (R > R01)
        return 0;

    // axis C0 + t*VxB2
    R = NiAbs(B[2].Dot(kVxD));
    R0 =
        extA[0]*NiAbs(C[0][0]*afVB[1] - C[0][1]*afVB[0]) +
        extA[1]*NiAbs(C[1][0]*afVB[1] - C[1][1]*afVB[0]) +
        extA[2]*NiAbs(C[2][0]*afVB[1] - C[2][1]*afVB[0]);
    R1 = extB[0]*NiAbs(afVB[1]) + extB[1]*NiAbs(afVB[0]);
    R01 = R0 + R1;
    if (R > R01)
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
#define UPDATE_TIME(TYPE) \
{ \
    if ( u0 > rsum ) \
    { \
        if ( u1 > rsum ) \
        { \
            return 0; \
        } \
        tintr = dt*(rsum-u0)/(u1-u0); \
        if ( tintr > tmax ) \
        { \
            tmax = tintr; \
            type = TYPE; \
            side = +1; \
        } \
    } \
    else if ( u0 < -rsum ) \
    { \
        if ( u1 < -rsum ) \
        { \
            return 0; \
        } \
        tintr = dt*(-rsum-u0)/(u1-u0); \
        if ( tintr > tmax ) \
        { \
            tmax = tintr; \
            type = TYPE; \
            side = -1; \
        } \
    } \
}
//---------------------------------------------------------------------------
#define GET_COEFF_PLUS_MINUS(coeff,side,cmat,ext) \
{ \
    if ( cmat > 0.0f ) \
    { \
        coeff = side*ext; \
    } \
    else if ( cmat < 0.0f ) \
    { \
        coeff = -side*ext; \
    } \
    else \
    { \
        coeff = 0.0f; \
    } \
}
//---------------------------------------------------------------------------
#define GET_COEFF_MINUS_PLUS(coeff,side,cmat,ext) \
{ \
    if ( cmat > 0.0f ) \
    { \
        coeff = -side*ext; \
    } \
    else if ( cmat < 0.0f ) \
    { \
        coeff = side*ext; \
    } \
    else \
    { \
        coeff = 0.0f; \
    } \
}
//---------------------------------------------------------------------------
int NiOBBox::FindIntersection(const NiOBBox& box, float& t, NiPoint3& p)
{
    // name shortcuts
    const NiPoint3* A = m_wBasis;
    const NiPoint3* B = box.m_wBasis;
    const float* extA = m_wExtent;
    const float* extB = box.m_wExtent;

    float r0, r1, rsum, u0, u1;

    // memoized values for use in edge-edge separation tests
    float C[3][3];
    float d0[3], d1[3];

    // compute difference of centers and velocities
    NiPoint3 D = box.m_wCenter - m_wCenter;
    NiPoint3 V = box.m_pRoot->GetVelocity() - m_pRoot->GetVelocity();

    // time step for this frame
    float dt = m_pRoot->GetDeltaTime();

    // track minimum time of projection-intersection
    int type = intrNone, side = 0;
    float tintr, tmax = 0.0f;

    // L = A0
    C[0][0] = A[0]*B[0];
    C[0][1] = A[0]*B[1];
    C[0][2] = A[0]*B[2];
    u0 = d0[0] = A[0]*D;
    u1 = d1[0] = d0[0]+dt*(V*A[0]);
    r0 = NiAbs(extA[0]);
    r1 = NiAbs(extB[0]*C[0][0])+NiAbs(extB[1]*C[0][1])+NiAbs(extB[2]*C[0][2]);
    rsum = r0+r1;
    UPDATE_TIME(intrA0);

    // L = A1
    C[1][0] = A[1]*B[0];
    C[1][1] = A[1]*B[1];
    C[1][2] = A[1]*B[2];
    d0[1] = A[1]*D;
    d1[1] = d0[1]+dt*(A[1]*V);
    r0 = NiAbs(extA[1]);
    r1 = NiAbs(extB[0]*C[1][0])+NiAbs(extB[1]*C[1][1])+NiAbs(extB[2]*C[1][2]);
    rsum = r0+r1;
    UPDATE_TIME(intrA1);

    // L = A2
    C[2][0] = A[2]*B[0];
    C[2][1] = A[2]*B[1];
    C[2][2] = A[2]*B[2];
    d0[2] = A[2]*D;
    d1[2] = d0[2]+dt*(A[2]*V);
    r0 = NiAbs(extA[2]);
    r1 = NiAbs(extB[0]*C[2][0])+NiAbs(extB[1]*C[2][1])+NiAbs(extB[2]*C[2][2]);
    rsum = r0+r1;
    UPDATE_TIME(intrA2);

    // L = B0
    r0 = NiAbs(extA[0]*C[0][0])+NiAbs(extA[1]*C[1][0])+NiAbs(extA[2]*C[2][0]);
    r1 = NiAbs(extB[0]);
    rsum = r0+r1;
    u0 = B[0]*D;
    u1 = u0+dt*(B[0]*V);
    UPDATE_TIME(intrB0);

    // L = B1
    r0 = NiAbs(extA[0]*C[0][1])+NiAbs(extA[1]*C[1][1])+NiAbs(extA[2]*C[2][1]);
    r1 = NiAbs(extB[1]);
    rsum = r0+r1;
    u0 = B[1]*D;
    u1 = u0+dt*(B[1]*V);
    UPDATE_TIME(intrB1);

    // L = B2
    r0 = NiAbs(extA[0]*C[0][2])+NiAbs(extA[1]*C[1][2])+NiAbs(extA[2]*C[2][2]);
    r1 = NiAbs(extB[2]);
    rsum = r0+r1;
    u0 = B[2]*D;
    u1 = u0+dt*(B[2]*V);
    UPDATE_TIME(intrB2);

    // L = A0xB0
    r0 = NiAbs(extA[1]*C[2][0])+NiAbs(extA[2]*C[1][0]);
    r1 = NiAbs(extB[1]*C[0][2])+NiAbs(extB[2]*C[0][1]);
    rsum = r0+r1;
    u0 = d0[2]*C[1][0]-d0[1]*C[2][0];
    u1 = d1[2]*C[1][0]-d1[1]*C[2][0];
    UPDATE_TIME(intrA0B0);

    // L = A0xB1
    r0 = NiAbs(extA[1]*C[2][1])+NiAbs(extA[2]*C[1][1]);
    r1 = NiAbs(extB[0]*C[0][2])+NiAbs(extB[2]*C[0][0]);
    rsum = r0+r1;
    u0 = d0[2]*C[1][1]-d0[1]*C[2][1];
    u1 = d1[2]*C[1][1]-d1[1]*C[2][1];
    UPDATE_TIME(intrA0B1);

    // L = A0xB2
    r0 = NiAbs(extA[1]*C[2][2])+NiAbs(extA[2]*C[1][2]);
    r1 = NiAbs(extB[0]*C[0][1])+NiAbs(extB[1]*C[0][0]);
    rsum = r0+r1;
    u0 = d0[2]*C[1][2]-d0[1]*C[2][2];
    u1 = d1[2]*C[1][2]-d1[1]*C[2][2];
    UPDATE_TIME(intrA0B2);

    // L = A1xB0
    r0 = NiAbs(extA[0]*C[2][0])+NiAbs(extA[2]*C[0][0]);
    r1 = NiAbs(extB[1]*C[1][2])+NiAbs(extB[2]*C[1][1]);
    rsum = r0+r1;
    u0 = d0[0]*C[2][0]-d0[2]*C[0][0];
    u1 = d1[0]*C[2][0]-d1[2]*C[0][0];
    UPDATE_TIME(intrA1B0);

    // L = A1xB1
    r0 = NiAbs(extA[0]*C[2][1])+NiAbs(extA[2]*C[0][1]);
    r1 = NiAbs(extB[0]*C[1][2])+NiAbs(extB[2]*C[1][0]);
    rsum = r0+r1;
    u0 = d0[0]*C[2][1]-d0[2]*C[0][1];
    u1 = d1[0]*C[2][1]-d1[2]*C[0][1];
    UPDATE_TIME(intrA1B1);

    // L = A1xB2
    r0 = NiAbs(extA[0]*C[2][2])+NiAbs(extA[2]*C[0][2]);
    r1 = NiAbs(extB[0]*C[1][1])+NiAbs(extB[1]*C[1][0]);
    rsum = r0+r1;
    u0 = d0[0]*C[2][2]-d0[2]*C[0][2];
    u1 = d1[0]*C[2][2]-d1[2]*C[0][2];
    UPDATE_TIME(intrA1B2);

    // L = A2xB0
    r0 = NiAbs(extA[0]*C[1][0])+NiAbs(extA[1]*C[0][0]);
    r1 = NiAbs(extB[1]*C[2][2])+NiAbs(extB[2]*C[2][1]);
    rsum = r0+r1;
    u0 = d0[1]*C[0][0]-d0[0]*C[1][0];
    u1 = d1[1]*C[0][0]-d1[0]*C[1][0];
    UPDATE_TIME(intrA2B0);

    // L = A2xB1
    r0 = NiAbs(extA[0]*C[1][1])+NiAbs(extA[1]*C[0][1]);
    r1 = NiAbs(extB[0]*C[2][2])+NiAbs(extB[2]*C[2][0]);
    rsum = r0+r1;
    u0 = d0[1]*C[0][1]-d0[0]*C[1][1];
    u1 = d1[1]*C[0][1]-d1[0]*C[1][1];
    UPDATE_TIME(intrA2B1);

    // L = A2xB2
    r0 = NiAbs(extA[0]*C[1][2])+NiAbs(extA[1]*C[0][2]);
    r1 = NiAbs(extB[0]*C[2][1])+NiAbs(extB[1]*C[2][0]);
    rsum = r0+r1;
    u0 = d0[1]*C[0][2]-d0[0]*C[1][2];
    u1 = d1[1]*C[0][2]-d1[0]*C[1][2];
    UPDATE_TIME(intrA2B2);

    // time of intersection
    t = tmax;

    // determine the point of intersection
    const float epsilon = 1e-06f;
    int i, j;
    float x[3], y[3], ad, bd, div, tmp;

    NiPoint3 DTW;
    const NiPoint3& V0 = m_pRoot->GetVelocity();
    const NiPoint3& V1 = box.m_pRoot->GetVelocity();

    switch ( type )
    {
        case intrA0:
        case intrA1:
        case intrA2:
        {
            i = type-1;
            for (j = 0; j < 3; j++)
            {
                GET_COEFF_MINUS_PLUS(y[j],side,C[i][j],extB[j]);
            }
            p = box.m_wCenter+t*V1+y[0]*B[0]+y[1]*B[1]+y[2]*B[2];
            break;
        }
        case intrB0:
        case intrB1:
        case intrB2:
        {
            j = type-4;
            for (i = 0; i < 3; i++)
            {
                GET_COEFF_PLUS_MINUS(x[i],side,C[i][j],extA[i]);
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA0B0:
        {
            GET_COEFF_MINUS_PLUS(x[1],side,C[2][0],extA[1]);
            GET_COEFF_PLUS_MINUS(x[2],side,C[1][0],extA[2]);
            GET_COEFF_MINUS_PLUS(y[1],side,C[0][2],extB[1]);
            GET_COEFF_PLUS_MINUS(y[2],side,C[0][1],extB[2]);
            div = 1.0f-C[0][0]*C[0][0];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[0]*DTW;
                bd = B[0]*DTW;
                tmp = C[1][0]*x[1]+C[2][0]*x[2]-bd;
                x[0] = (ad+C[0][0]*tmp+C[0][1]*y[1]+C[0][2]*y[2])/div;
            }
            else
            {
                x[0] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA0B1:
        {
            GET_COEFF_MINUS_PLUS(x[1],side,C[2][1],extA[1]);
            GET_COEFF_PLUS_MINUS(x[2],side,C[1][1],extA[2]);
            GET_COEFF_PLUS_MINUS(y[0],side,C[0][2],extB[0]);
            GET_COEFF_MINUS_PLUS(y[2],side,C[0][0],extB[2]);
            div = 1.0f-C[0][1]*C[0][1];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[0]*DTW;
                bd = B[1]*DTW;
                tmp = C[1][1]*x[1]+C[2][1]*x[2]-bd;
                x[0] = (ad+C[0][1]*tmp+C[0][0]*y[0]+C[0][2]*y[2])/div;
            }
            else
            {
                x[0] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA0B2:
        {
            GET_COEFF_MINUS_PLUS(x[1],side,C[2][2],extA[1]);
            GET_COEFF_PLUS_MINUS(x[2],side,C[1][2],extA[2]);
            GET_COEFF_MINUS_PLUS(y[0],side,C[0][1],extB[0]);
            GET_COEFF_PLUS_MINUS(y[1],side,C[0][0],extB[1]);
            div = 1.0f-C[0][2]*C[0][2];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[0]*DTW;
                bd = B[2]*DTW;
                tmp = C[1][2]*x[1]+C[2][2]*x[2]-bd;
                x[0] = (ad+C[0][2]*tmp+C[0][0]*y[0]+C[0][1]*y[1])/div;
            }
            else
            {
                x[0] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA1B0:
        {
            GET_COEFF_PLUS_MINUS(x[0],side,C[2][0],extA[0]);
            GET_COEFF_MINUS_PLUS(x[2],side,C[0][0],extA[2]);
            GET_COEFF_MINUS_PLUS(y[1],side,C[1][2],extB[1]);
            GET_COEFF_PLUS_MINUS(y[2],side,C[1][1],extB[2]);
            div = 1.0f-C[1][0]*C[1][0];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[1]*DTW;
                bd = B[0]*DTW;
                tmp = C[0][0]*x[0]+C[2][0]*x[2]-bd;
                x[1] = (ad+C[1][0]*tmp+C[1][1]*y[1]+C[1][2]*y[2])/div;
            }
            else
            {
                x[1] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA1B1:
        {
            GET_COEFF_PLUS_MINUS(x[0],side,C[2][1],extA[0]);
            GET_COEFF_MINUS_PLUS(x[2],side,C[0][1],extA[2]);
            GET_COEFF_PLUS_MINUS(y[0],side,C[1][2],extB[0]);
            GET_COEFF_MINUS_PLUS(y[2],side,C[1][0],extB[2]);
            div = 1.0f-C[1][1]*C[1][1];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[1]*DTW;
                bd = B[1]*DTW;
                tmp = C[0][1]*x[0]+C[2][1]*x[2]-bd;
                x[1] = (ad+C[1][1]*tmp+C[1][0]*y[0]+C[1][2]*y[2])/div;
            }
            else
            {
                x[1] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA1B2:
        {
            GET_COEFF_PLUS_MINUS(x[0],side,C[2][2],extA[0]);
            GET_COEFF_MINUS_PLUS(x[2],side,C[0][2],extA[2]);
            GET_COEFF_MINUS_PLUS(y[0],side,C[1][1],extB[0]);
            GET_COEFF_PLUS_MINUS(y[1],side,C[1][0],extB[1]);
            div = 1.0f-C[1][2]*C[1][2];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[1]*DTW;
                bd = B[2]*DTW;
                tmp = C[0][2]*x[0]+C[2][2]*x[2]-bd;
                x[1] = (ad+C[1][2]*tmp+C[1][0]*y[0]+C[1][1]*y[1])/div;
            }
            else
            {
                x[1] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA2B0:
        {
            GET_COEFF_MINUS_PLUS(x[0],side,C[1][0],extA[0]);
            GET_COEFF_PLUS_MINUS(x[1],side,C[0][0],extA[1]);
            GET_COEFF_MINUS_PLUS(y[1],side,C[2][2],extB[1]);
            GET_COEFF_PLUS_MINUS(y[2],side,C[2][1],extB[2]);
            div = 1.0f-C[2][0]*C[2][0];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[2]*DTW;
                bd = B[0]*DTW;
                tmp = C[0][0]*x[0]+C[1][0]*x[1]-bd;
                x[2] = (ad+C[2][0]*tmp+C[2][1]*y[1]+C[2][2]*y[2])/div;
            }
            else
            {
                x[2] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA2B1:
        {
            GET_COEFF_MINUS_PLUS(x[0],side,C[1][1],extA[0]);
            GET_COEFF_PLUS_MINUS(x[1],side,C[0][1],extA[1]);
            GET_COEFF_PLUS_MINUS(y[0],side,C[2][2],extB[0]);
            GET_COEFF_MINUS_PLUS(y[2],side,C[2][0],extB[2]);
            div = 1.0f-C[2][1]*C[2][1];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[2]*DTW;
                bd = B[1]*DTW;
                tmp = C[0][1]*x[0]+C[1][1]*x[1]-bd;
                x[2] = (ad+C[2][1]*tmp+C[2][0]*y[0]+C[2][2]*y[2])/div;
            }
            else
            {
                x[2] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
        case intrA2B2:
        {
            GET_COEFF_MINUS_PLUS(x[0],side,C[1][2],extA[0]);
            GET_COEFF_PLUS_MINUS(x[1],side,C[0][2],extA[1]);
            GET_COEFF_MINUS_PLUS(y[0],side,C[2][1],extB[0]);
            GET_COEFF_PLUS_MINUS(y[1],side,C[2][0],extB[1]);
            div = 1.0f-C[2][2]*C[2][2];
            if ( NiAbs(div) > epsilon )
            {
                DTW = D+t*V;
                ad = A[2]*DTW;
                bd = B[2]*DTW;
                tmp = C[0][2]*x[0]+C[1][2]*x[1]-bd;
                x[2] = (ad+C[2][2]*tmp+C[2][0]*y[0]+C[2][1]*y[1])/div;
            }
            else
            {
                x[2] = 0.0f;
            }
            p = m_wCenter+t*V0+x[0]*A[0]+x[1]*A[1]+x[2]*A[2];
            break;
        }
    };

    return 1;
}
//---------------------------------------------------------------------------
void NiOBBox::ComputeNormal(const NiPoint3& p, NiPoint3& normal) const
{
    // unitize m_wExtent for relative error testing
    NiPoint3 unitExtent(m_wExtent[0],m_wExtent[1],m_wExtent[2]);
    unitExtent.Unitize();

    // use the obvious normals when boxes are flat
    const float fEpsilon = 1e-04f;

    if ( unitExtent.x <= fEpsilon )
    {
        normal = m_wBasis[0];
        return;
    }
    if ( unitExtent.y <= fEpsilon )
    {
        normal = m_wBasis[1];
        return;
    }
    if ( unitExtent.z <= fEpsilon )
    {
        normal = m_wBasis[2];
        return;
    }

    // For non-flat boxes, use circumscribed ellipsoid to produce
    // a smoothed normal.  Avoids having to handle special cases
    // when intersection is on an edge or at a vertex.

    // Point is p = C+x*B0+y*B1+z*B2 where C is the box center and
    // B0, B1, and B2 are the basis vectors.  Let E be the extent.
    // The ellipsoid is 
    //   F(x,y,z) = ((x-C.x)/E.x)^2+((y-C.y)/E.y)^2+((z-C.z)/E.z)^2 = const.
    // A normal vector at p is
    //   DF = (F_x,F_y,F_z)/2 = ((x-C.x)/E.x^2,(y-C.y)/E.y^2,(z-C.z)/E.z^2)

    NiPoint3 diff = p - m_wCenter;
    normal.x = (diff*m_wBasis[0])/(m_wExtent[0]*m_wExtent[0]);
    normal.y = (diff*m_wBasis[1])/(m_wExtent[1]*m_wExtent[1]);
    normal.z = (diff*m_wBasis[2])/(m_wExtent[2]*m_wExtent[2]);
    normal.Unitize();
}
//---------------------------------------------------------------------------
int NiOBBox::TestIntersection(const NiPoint3& origin, const NiPoint3& dir)
{
    float fTMin = -FLT_MAX;
    float fTMax = FLT_MAX;

    const NiPoint3* akAxis = GetWorldBasis();

    float afExtent[3] =
    {
        NiAbs(GetWorldExtent()[0]),
        NiAbs(GetWorldExtent()[1]),
        NiAbs(GetWorldExtent()[2])
    };

    NiPoint3 diff = GetWorldCenter() - origin;

    for (int i = 0; i < 3; i++)
    {

        float fAxisDotDiff = akAxis[i]*diff;
        float fAxisDotDir = akAxis[i]*dir;

        if ( NiAbs(fAxisDotDir) > FLOAT_EPSILON )
        {
            float fInv = 1.0f/fAxisDotDir;
            float fT1 = (fAxisDotDiff + afExtent[i])*fInv;
            float fT2 = (fAxisDotDiff - afExtent[i])*fInv;
            if ( fT1 > fT2 )
            {
                float fTemp = fT1;
                fT1 = fT2;
                fT2 = fTemp;
            }

            if ( fT1 > fTMin )
                fTMin = fT1;

            if ( fT2 < fTMax )
                fTMax = fT2;

            if ( fTMin > fTMax || fTMax < 0.0f )
                return 0;
        }
        else
        {
            if ( NiAbs(fAxisDotDiff) > afExtent[i] )
                return 0;
        }
    }

    return 1;
}
//---------------------------------------------------------------------------
int NiOBBox::FindIntersection(const NiPoint3& origin, const NiPoint3& dir,
    float& t, NiPoint3& normal)
{
    float fTMin = -FLT_MAX;
    float fTMax = FLT_MAX;
    int iMinAxis = -1;
    int iMaxAxis = -1;

    const NiPoint3* akAxis = GetWorldBasis();

    float afExtent[3] =
    {
        NiAbs(GetWorldExtent()[0]),
        NiAbs(GetWorldExtent()[1]),
        NiAbs(GetWorldExtent()[2])
    };

    NiPoint3 diff = GetWorldCenter() - origin;

    for (int i = 0; i < 3; i++)
    {
        float fAxisDotDiff = akAxis[i]*diff;
        float fAxisDotDir = akAxis[i]*dir;

        if ( NiAbs(fAxisDotDir) > FLOAT_EPSILON )
        {
            float fInv = 1.0f/fAxisDotDir;
            float fT1 = (fAxisDotDiff + afExtent[i])*fInv;
            float fT2 = (fAxisDotDiff - afExtent[i])*fInv;
            if ( fT1 > fT2 )
            {
                float fTemp = fT1;
                fT1 = fT2;
                fT2 = fTemp;
            }

            if ( fT1 > fTMin)
            {
                fTMin = fT1;
                iMinAxis = i;
            }

            if ( fT2 < fTMax )
            {
                fTMax = fT2;
                iMaxAxis = i;
            }

            if ( fTMin > fTMax || fTMax < 0.0f )
                return 0;
        }
        else
        {
            if ( NiAbs(fAxisDotDiff) > afExtent[i] )
                return 0;
        }
    }

    if ( fTMin > 0.0f )
    {
        t = fTMin;
        normal = akAxis[iMinAxis];
    }
    else
    {
        t = fTMax;
        if ( iMaxAxis == -1 ) // this will only happen if ray length is zero
            return 0;
        normal = akAxis[iMaxAxis];
    }

    if ( dir.Dot(normal) > 0.0f )
        normal = -normal;

    return 1;
}
//---------------------------------------------------------------------------
