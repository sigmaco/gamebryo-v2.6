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
#include "NiTriIntersect.h"

float NiTriIntersect::ms_fEpsilon = 1e-05f;

//---------------------------------------------------------------------------
NiTriIntersect::NiTriIntersect(const NiPoint3* apkP[3], const NiPoint3& rkV0,
    const NiPoint3* apkQ[3], const NiPoint3& rkV1)
    :
    m_rkV0(rkV0),
    m_rkV1(rkV1)
{
    unsigned int uiDestSize = 3 * sizeof(NiPoint3*);
    NiMemcpy(m_apkP, uiDestSize, apkP, uiDestSize);
    NiMemcpy(m_apkQ, uiDestSize, apkQ, uiDestSize);

    // Compute relative velocity of second triangle with respect to first
    // triangle so that first triangle may as well be stationary.
    m_kW = rkV1 - rkV0;

    // compute triangle edges, normals, difference of zero-th vertices
    m_kE0 = (*m_apkP[1]) - (*m_apkP[0]);
    m_kE1 = (*m_apkP[2]) - (*m_apkP[0]);
    m_kF0 = (*m_apkQ[1]) - (*m_apkQ[0]);
    m_kF1 = (*m_apkQ[2]) - (*m_apkQ[0]);
    m_kD  = (*m_apkQ[0]) - (*m_apkP[0]);

    m_uiAxis = AXIS_NONE;
}
//---------------------------------------------------------------------------
bool NiTriIntersect::Test(float fTime)
{
    m_fTime = fTime;

    if ( !TestN() ) return false;
    if ( !TestM() ) return false;

    m_kNxM = m_kN.Cross(m_kM);
    m_fNxMdNxM = m_kNxM.Dot(m_kNxM);
    if ( m_fNxMdNxM > ms_fEpsilon )
    {
        // triangles are not parallel
        if ( !TestE0xF0() ) return false;
        if ( !TestE0xF1() ) return false;
        if ( !TestE0xF2() ) return false;
        if ( !TestE1xF0() ) return false;
        if ( !TestE1xF1() ) return false;
        if ( !TestE1xF2() ) return false;
        if ( !TestE2xF0() ) return false;
        if ( !TestE2xF1() ) return false;
        if ( !TestE2xF2() ) return false;
    }
    else
    {
        // The triangles are parallel and their planes must have passed
        // through each other.  For if the planes had not passed through
        // each other, the separating axis test for N would have indicated
        // no intersection.
    
        if ( NiAbs(m_fNdW) > ms_fEpsilon )
        {
            // time at which triangle planes meet
            float fPlaneMeet = -m_fNdD/m_fNdW;

            // At this instant we need to do separating axis tests for two
            // stationary triangles in the plane.  In this case, W = 0 and
            // the time duration is irrelevant.  Need to move D to D+T*W.
            m_kD += fPlaneMeet*m_kW;
            m_kW = NiPoint3::ZERO;
        }
        // else: triangles are coplanar during the entire interval [0,fTime]

        if ( !TestMxF0() ) return false;
        if ( !TestMxF1() ) return false;
        if ( !TestMxF2() ) return false;
        if ( !TestNxE0() ) return false;
        if ( !TestNxE1() ) return false;
        if ( !TestNxE2() ) return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTriIntersect::Find(float fTime, float& fIntrTime, NiPoint3& kIntrPt,
    bool bCalcNormals, NiPoint3* pkNormal0, NiPoint3* pkNormal1)
{
    m_fTime = fTime;
    m_fIntrTime = 0.0f;

    if ( !FindN() ) return false;
    if ( !FindM() ) return false;

    m_kNxM = m_kN.Cross(m_kM);
    m_fNxMdNxM = m_kNxM.Dot(m_kNxM);
    if ( m_fNxMdNxM > ms_fEpsilon )
    {
        // triangles are not parallel
        if ( !FindE0xF0() ) return false;
        if ( !FindE0xF1() ) return false;
        if ( !FindE0xF2() ) return false;
        if ( !FindE1xF0() ) return false;
        if ( !FindE1xF1() ) return false;
        if ( !FindE1xF2() ) return false;
        if ( !FindE2xF0() ) return false;
        if ( !FindE2xF1() ) return false;
        if ( !FindE2xF2() ) return false;
    }
    else
    {
        // The triangles are parallel and their planes must have passed
        // through each other.  For if the planes had not passed through
        // each other, the separating axis test for N would have indicated
        // no intersection.

        if ( NiAbs(m_fNdW) > ms_fEpsilon )
        {
            // time at which triangle planes meet
            float fPlaneMeet = -m_fNdD/m_fNdW;

            // At this instant we need to do separating axis tests for two
            // stationary triangles in the plane.  In this case, W = 0 and
            // the time duration is irrelevant.  Need to move D to D+T*W.
            m_kD += fPlaneMeet*m_kW;
            m_kW = NiPoint3::ZERO;
        }
        // else: triangles are coplanar during the entire interval [0,fTime]

        if ( !FindMxF0() ) return false;
        if ( !FindMxF1() ) return false;
        if ( !FindMxF2() ) return false;
        if ( !FindNxE0() ) return false;
        if ( !FindNxE1() ) return false;
        if ( !FindNxE2() ) return false;
    }

    // triangles intersect
    LocateIntersection();
    fIntrTime = m_fIntrTime;
    kIntrPt = m_kIntrPt;

    if ( bCalcNormals )
    {
        *pkNormal0 = m_kN;
        pkNormal0->Unitize();
        *pkNormal1 = m_kM;
        pkNormal1->Unitize();
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// sorting of projected triangle vertices
//---------------------------------------------------------------------------
void NiTriIntersect::Sort(float fR0, float fR1, float& rfMin, float& rfMax)
{
    if ( fR1 >= fR0 )
    {
        rfMin = fR0;
        rfMax = fR1;
    }
    else
    {
        rfMin = fR1;
        rfMax = fR0;
    }
}
//---------------------------------------------------------------------------
void NiTriIntersect::Sort(float fR0, float fR1, float fR2, float& rfMin,
    float& rfMax)
{
    if ( fR2 >= fR1 )
    {
        if ( fR1 >= fR0 )  // [r0,r1,r2]
        {
            rfMin = fR0;
            rfMax = fR2;
        }
        else if ( fR2 >= fR0 )  // [r1,r0,r2]
        {
            rfMin = fR1;
            rfMax = fR2;
        }
        else  // [r1,r2,r0]
        {
            rfMin = fR1;
            rfMax = fR0;
        }
    }
    else
    {
        if ( fR2 >= fR0 )  // [r0,r2,r1]
        {
            rfMin = fR0;
            rfMax = fR1;
        }
        else if ( fR1 >= fR0 )  // [r2,r0,r1]
        {
            rfMin = fR2;
            rfMax = fR1;
        }
        else  // [r2,r1,r0]
        {
            rfMin = fR2;
            rfMax = fR0;
        }
    }
}
//---------------------------------------------------------------------------
void NiTriIntersect::Sort(float fR0, float fR1, float fR2, float afSort[3],
    int aiIndex[3])
{
    if ( fR2 >= fR1 )
    {
        if ( fR1 >= fR0 )  // [r0,r1,r2]
        {
            afSort[0] = fR0;
            afSort[1] = fR1;
            afSort[2] = fR2;
            aiIndex[0] = 0;
            aiIndex[1] = 1;
            aiIndex[2] = 2;
        }
        else if ( fR2 >= fR0 )  // [r1,r0,r2]
        {
            afSort[0] = fR1;
            afSort[1] = fR0;
            afSort[2] = fR2;
            aiIndex[0] = 1;
            aiIndex[1] = 0;
            aiIndex[2] = 2;
        }
        else  // [r1,r2,r0]
        {
            afSort[0] = fR1;
            afSort[1] = fR2;
            afSort[2] = fR0;
            aiIndex[0] = 1;
            aiIndex[1] = 2;
            aiIndex[2] = 0;
        }
    }
    else
    {
        if ( fR2 >= fR0 )  // [r0,r2,r1]
        {
            afSort[0] = fR0;
            afSort[1] = fR2;
            afSort[2] = fR1;
            aiIndex[0] = 0;
            aiIndex[1] = 2;
            aiIndex[2] = 1;
        }
        else if ( fR1 >= fR0 )  // [r2,r0,r1]
        {
            afSort[0] = fR2;
            afSort[1] = fR0;
            afSort[2] = fR1;
            aiIndex[0] = 2;
            aiIndex[1] = 0;
            aiIndex[2] = 1;
        }
        else  // [r2,r1,r0]
        {
            afSort[0] = fR2;
            afSort[1] = fR1;
            afSort[2] = fR0;
            aiIndex[0] = 2;
            aiIndex[1] = 1;
            aiIndex[2] = 0;
        }
    }
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestIntervals(float fPMin, float fPMax, float fQMin,
    float fQMax, float fW)
{
    if ( fQMin > fPMax )
    {
        if ( fQMin + m_fTime*fW > fPMax )
            return false;
    }
    else if ( fQMax < fPMin )
    {
        if ( fQMax + m_fTime*fW < fPMin )
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindIntervals(unsigned int uiAxis, float afPSort[3],
    int aiPIndex[3], float afQSort[3], int aiQIndex[3], float fW)
{
    float fTmp;
    unsigned int uiDestSize3Floats = 3*sizeof(float);
    unsigned int uiDestSize3Ints = 3*sizeof(int);
    if ( afQSort[0] > afPSort[2] )
    {
        if ( afQSort[0] + m_fTime*fW > afPSort[2] )
            return false;

        fTmp = (afPSort[2]-afQSort[0])/fW;
        if ( fTmp > m_fIntrTime )
        {
            m_fIntrTime = fTmp;
            m_uiAxis = uiAxis;
            NiMemcpy(m_afPSort, uiDestSize3Floats, afPSort,
                uiDestSize3Floats);
            NiMemcpy(m_aiPIndex, uiDestSize3Ints, aiPIndex, uiDestSize3Ints);
            NiMemcpy(m_afQSort, uiDestSize3Floats, afQSort,
                uiDestSize3Floats);
            NiMemcpy(m_aiQIndex, uiDestSize3Ints, aiQIndex, uiDestSize3Ints);
        }
    }
    else if ( afQSort[2] < afPSort[0] )
    {
        if ( afQSort[2] + m_fTime*fW < afPSort[0] )
            return false;

        fTmp = (afPSort[0]-afQSort[2])/fW;
        if ( fTmp > m_fIntrTime )
        {
            m_fIntrTime = fTmp;
            m_uiAxis = uiAxis;
            NiMemcpy(m_afPSort,uiDestSize3Floats, afPSort,uiDestSize3Floats);
            NiMemcpy(m_aiPIndex,uiDestSize3Ints, aiPIndex, uiDestSize3Ints);
            NiMemcpy(m_afQSort, uiDestSize3Floats,afQSort,uiDestSize3Floats);
            NiMemcpy(m_aiQIndex, uiDestSize3Ints, aiQIndex, uiDestSize3Ints);
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// test intersection
//---------------------------------------------------------------------------
bool NiTriIntersect::TestN()
{
    // Input:
    //   q0 = N*D
    //   q1 = q0 + N*F0
    //   q2 = q0 + N*F1
    //   pmin = pmax = 0
    //   w = N*W

    m_kN = m_kE0.Cross(m_kE1);
    m_fNdF0 = m_kN.Dot(m_kF0);
    m_fNdF1 = m_kN.Dot(m_kF1);
    m_fNdD = m_kN.Dot(m_kD);
    m_fNdW = m_kN.Dot(m_kW);

    // sort values
    float fQMin, fQMax;
    Sort(m_fNdD,m_fNdD+m_fNdF0,m_fNdD+m_fNdF1,fQMin,fQMax);

    return TestIntervals(0.0f,0.0f,fQMin,fQMax,m_fNdW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestM()
{
    // Input:
    //   p0 = 0
    //   p1 = M*E0
    //   p2 = M*E1
    //   qmin = qmax = M*D
    //   w = M*W

    m_kM = m_kF0.Cross(m_kF1);
    m_fMdE0 = m_kM.Dot(m_kE0);
    m_fMdE1 = m_kM.Dot(m_kE1);
    m_fMdD = m_kM.Dot(m_kD);
    m_fMdW = m_kM.Dot(m_kW);

    // sort values
    float fPMin, fPMax;
    Sort(0.0f,m_fMdE0,m_fMdE1,fPMin,fPMax);

    return TestIntervals(fPMin,fPMax,m_fMdD,m_fMdD,m_fMdW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE0xF0()
{
    // Input:
    //   p0 = 0
    //   p1 = 0
    //   p2 = -N*F0
    //   q0 = [E0,F0,D]
    //   q1 = q0
    //   q2 = q0 + M*E0
    //   w = [E0,F0,W]

    m_kE0xF0 = m_kE0.Cross(m_kF0);
    m_fE0xF0dD = m_kE0xF0.Dot(m_kD);
    m_fE0xF0dW = m_kE0xF0.Dot(m_kW);

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,-m_fNdF0,fPMin,fPMax);
    Sort(m_fE0xF0dD,m_fE0xF0dD,m_fE0xF0dD+m_fMdE0,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE0xF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE0xF1()
{
    // Input:
    //   p0 = 0
    //   p1 = 0
    //   p2 = -N*F1
    //   q0 = [E0,F1,D]
    //   q1 = q0 - M*E0
    //   q2 = q0
    //   w = [E0,F1,W]

    m_kE0xF1 = m_kE0.Cross(m_kF1);
    m_fE0xF1dD = m_kE0xF1.Dot(m_kD);
    m_fE0xF1dW = m_kE0xF1.Dot(m_kW);

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,-m_fNdF1,fPMin,fPMax);
    Sort(m_fE0xF1dD,m_fE0xF1dD-m_fMdE0,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE0xF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE0xF2()
{
    // Input:
    //   p0 = 0
    //   p1 = 0
    //   p2 = -N*F2
    //   q0 = [E0,F2,D]
    //   q1 = q0 - M*E0
    //   q2 = q0
    //   w = [E0,F2,W]

    m_fE0xF2dD = m_fE0xF1dD - m_fE0xF0dD;
    m_fE0xF2dW = m_fE0xF1dW - m_fE0xF0dW;
    m_fNdF2 = m_fNdF1 - m_fNdF0;

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,-m_fNdF2,fPMin,fPMax);
    Sort(m_fE0xF2dD,m_fE0xF2dD-m_fMdE0,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE0xF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE1xF0()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F0
    //   p2 = 0
    //   q0 = [E1,F0,D]
    //   q1 = q0
    //   q2 = q0 + M*E1
    //   w = [E1,F0,W]

    m_kE1xF0 = m_kE1.Cross(m_kF0);
    m_fE1xF0dD = m_kE1xF0.Dot(m_kD);
    m_fE1xF0dW = m_kE1xF0.Dot(m_kW);

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,m_fNdF0,fPMin,fPMax);
    Sort(m_fE1xF0dD,m_fE0xF0dD+m_fMdE1,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE1xF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE1xF1()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F1
    //   p2 = 0
    //   q0 = [E1,F1,D]
    //   q1 = q0 - M*E1
    //   q2 = q0
    //   w = [E1,F1,W]

    m_kE1xF1 = m_kE1.Cross(m_kF1);
    m_fE1xF1dD = m_kE1xF1.Dot(m_kD);
    m_fE1xF1dW = m_kE1xF1.Dot(m_kW);

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,m_fNdF1,fPMin,fPMax);
    Sort(m_fE1xF1dD,m_fE1xF1dD-m_fMdE1,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE1xF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE1xF2()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F2
    //   p2 = 0
    //   q0 = [E1,F2,D]
    //   q1 = q0 - M*E1
    //   q2 = q0
    //   w = [E1,F2,W]

    m_fE1xF2dD = m_fE1xF1dD - m_fE1xF0dD;
    m_fE1xF2dW = m_fE1xF1dW - m_fE1xF0dW;

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,m_fNdF2,fPMin,fPMax);
    Sort(m_fE1xF2dD,m_fE1xF2dD-m_fMdE1,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE1xF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE2xF0()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F0
    //   p2 = p1
    //   q0 = [E2,F0,D]
    //   q1 = q0
    //   q2 = q0 + M*E2
    //   w = [E2,F0,W]

    m_fE2xF0dD = m_fE1xF0dD - m_fE0xF0dD;
    m_fE2xF0dW = m_fE1xF0dW - m_fE0xF0dW;
    m_fMdE2 = m_fMdE1 - m_fMdE0;

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,m_fNdF0,fPMin,fPMax);
    Sort(m_fE2xF0dD,m_fE2xF0dD+m_fMdE2,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE2xF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE2xF1()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F1
    //   p2 = p1
    //   q0 = [E2,F1,D]
    //   q1 = q0 - M*E2
    //   q2 = q0
    //   w = [E2,F1,W]

    m_fE2xF1dD = m_fE1xF1dD - m_fE0xF1dD;
    m_fE2xF1dW = m_fE1xF1dW - m_fE0xF1dW;

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,m_fNdF1,fPMin,fPMax);
    Sort(m_fE2xF1dD,m_fE2xF1dD-m_fMdE2,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE2xF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestE2xF2()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F2
    //   p2 = p1
    //   q0 = [E2,F2,D]
    //   q1 = q0 - M*E2
    //   q2 = q0
    //   w = [E2,F2,W]

    m_fE2xF2dD = m_fE2xF1dD - m_fE2xF0dD;
    m_fE2xF2dW = m_fE2xF1dW - m_fE2xF0dW;

    // sort values
    float fPMin, fPMax, fQMin, fQMax;
    Sort(0.0f,m_fNdF2,fPMin,fPMax);
    Sort(m_fE2xF2dD,m_fE2xF2dD-m_fMdE2,fQMin,fQMax);

    return TestIntervals(fPMin,fPMax,fQMin,fQMax,m_fE2xF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestMxF0()
{
    // Input:
    //   p0 = 0
    //   p1 = [M,F0,E0]
    //   p2 = [M,F0,E1]
    //   qmin = [M,F0,D]
    //   qmax = qmin + M*M
    //   w = [M,F0,W]

    m_fMdM = m_kM.Dot(m_kM);
    m_kMxF0 = m_kM.Cross(m_kF0);
    m_fMxF0dE0 = m_kMxF0.Dot(m_kE0);
    m_fMxF0dE1 = m_kMxF0.Dot(m_kE1);
    m_fMxF0dD = m_kMxF0.Dot(m_kD);
    m_fMxF0dW = m_kMxF0.Dot(m_kW);

    // sort values
    float fPMin, fPMax;
    Sort(0.0f,m_fMxF0dE0,m_fMxF0dE1,fPMin,fPMax);

    return TestIntervals(fPMin,fPMax,m_fMxF0dD,m_fMxF0dD+m_fMdM,m_fMxF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestMxF1()
{
    // Input:
    //   p0 = 0
    //   p1 = [M,F1,E0]
    //   p2 = [M,F1,E1]
    //   qmax = [M,F1,D]
    //   qmin = qmax - M*M
    //   w = [M,F1,W]

    m_kMxF1 = m_kM.Cross(m_kF1);
    m_fMxF1dE0 = m_kMxF1.Dot(m_kE0);
    m_fMxF1dE1 = m_kMxF1.Dot(m_kE1);
    m_fMxF1dD = m_kMxF1.Dot(m_kD);
    m_fMxF1dW = m_kMxF1.Dot(m_kW);

    // sort values
    float fPMin, fPMax;
    Sort(0.0f,m_fMxF1dE0,m_fMxF1dE1,fPMin,fPMax);

    return TestIntervals(fPMin,fPMax,m_fMxF1dD-m_fMdM,m_fMxF1dD,m_fMxF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestMxF2()
{
    // Input:
    //   p0 = 0
    //   p1 = [M,F2,E0]
    //   p2 = [M,F2,E1]
    //   qmax = [M,F2,D]
    //   qmin = qmax - [M,F2,F0] = qmax - M*M
    //   w = [M,F2,W]

    m_fMxF2dE0 = m_fMxF1dE0 - m_fMxF0dE0;
    m_fMxF2dE1 = m_fMxF1dE1 - m_fMxF0dE1;
    m_fMxF2dD = m_fMxF1dD - m_fMxF0dD;
    m_fMxF2dW = m_fMxF1dW - m_fMxF0dW;

    // sort values
    float fPMin, fPMax;
    Sort(0.0f,m_fMxF2dE0,m_fMxF2dE1,fPMin,fPMax);

    return TestIntervals(fPMin,fPMax,m_fMxF2dD-m_fMdM,m_fMxF2dD,m_fMxF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestNxE0()
{
    // Input:
    //   q0 = [N,E0,D]
    //   q1 = q0 + [N,E0,F0]
    //   q2 = q0 + [N,E0,F1]
    //   pmin = 0
    //   pmax = N*N
    //   w = [N,E0,W]

    m_fNdN = m_kN.Dot(m_kN);
    m_kNxE0 = m_kN.Cross(m_kE0);
    m_fNxE0dF0 = m_kNxE0.Dot(m_kF0);
    m_fNxE0dF1 = m_kNxE0.Dot(m_kF1);
    m_fNxE0dD = m_kNxE0.Dot(m_kD);
    m_fNxE0dW = m_kNxE0.Dot(m_kW);

    // sort values
    float fQMin, fQMax;
    Sort(m_fNxE0dD,m_fNxE0dD+m_fNxE0dF0,m_fNxE0dD+m_fNxE0dF1,fQMin,fQMax);

    return TestIntervals(0.0f,m_fNdN,fQMin,fQMax,m_fNxE0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestNxE1()
{
    // Input:
    //   q0 = [N,E1,D]
    //   q1 = q0 + [N,E1,F0]
    //   q2 = q0 + [N,E1,F1]
    //   pmin = -N*N
    //   pmax = 0
    //   w = [N,E1,W]

    m_kNxE1 = m_kN.Cross(m_kE1);
    m_fNxE1dF0 = m_kNxE1.Dot(m_kF0);
    m_fNxE1dF1 = m_kNxE1.Dot(m_kF1);
    m_fNxE1dD = m_kNxE1.Dot(m_kD);
    m_fNxE1dW = m_kNxE1.Dot(m_kW);

    // sort q-values
    float fQMin, fQMax;
    Sort(m_fNxE1dD,m_fNxE1dD+m_fNxE1dF0,m_fNxE1dD+m_fNxE1dF1,fQMin,fQMax);

    return TestIntervals(-m_fNdN,0.0f,fQMin,fQMax,m_fNxE1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::TestNxE2()
{
    // Input:
    //   q0 = [N,E2,D]
    //   q1 = q0 + [N,E2,F0]
    //   q2 = q0 + [N,E2,F1]
    //   pmin = -N*N
    //   pmax = 0
    //   w = [N,E2,W]

    m_fNxE2dF0 = m_fNxE1dF0 - m_fNxE0dF0;
    m_fNxE2dF1 = m_fNxE1dF1 - m_fNxE0dF1;
    m_fNxE2dD = m_fNxE1dD - m_fNxE0dD;
    m_fNxE2dW = m_fNxE1dW - m_fNxE0dW;

    // sort q-values
    float fQMin, fQMax;
    Sort(m_fNxE2dD,m_fNxE2dD+m_fNxE2dF0,m_fNxE2dD+m_fNxE2dF1,fQMin,fQMax);

    return TestIntervals(-m_fNdN,0.0f,fQMin,fQMax,m_fNxE2dW);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// find intersection
//---------------------------------------------------------------------------
bool NiTriIntersect::FindN()
{
    // Input:
    //   q0 = N*D
    //   q1 = q0 + N*F0
    //   q2 = q0 + N*F1
    //   pmin = pmax = 0
    //   w = N*W

    m_kN = m_kE0.Cross(m_kE1);
    m_fNdF0 = m_kN.Dot(m_kF0);
    m_fNdF1 = m_kN.Dot(m_kF1);
    m_fNdD = m_kN.Dot(m_kD);
    m_fNdW = m_kN.Dot(m_kW);

    float afPSort[3] = { 0.0f, 0.0f, 0.0f };
    int aiPIndex[3] = { 0, 1, 2 };
    float afQSort[3];
    int aiQIndex[3];
    Sort(m_fNdD,m_fNdD+m_fNdF0,m_fNdD+m_fNdF1,afQSort,aiQIndex);

    return FindIntervals(AXIS_N,afPSort,aiPIndex,afQSort,aiQIndex,m_fNdW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindM()
{
    // Input:
    //   p0 = 0
    //   p1 = M*E0
    //   p2 = M*E1
    //   qmin = qmax = M*D
    //   w = M*W

    m_kM = m_kF0.Cross(m_kF1);
    m_fMdE0 = m_kM.Dot(m_kE0);
    m_fMdE1 = m_kM.Dot(m_kE1);
    m_fMdD = m_kM.Dot(m_kD);
    m_fMdW = m_kM.Dot(m_kW);

    float afQSort[3] = { m_fMdD, m_fMdD, m_fMdD };
    int aiQIndex[3] = { 0, 1, 2 };
    float afPSort[3];
    int aiPIndex[3];
    Sort(0.0f,m_fMdE0,m_fMdE1,afPSort,aiPIndex);

    return FindIntervals(AXIS_M,afPSort,aiPIndex,afQSort,aiQIndex,m_fMdW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE0xF0()
{
    // Input:
    //   p0 = 0
    //   p1 = 0
    //   p2 = -N*F0
    //   q0 = [E0,F0,D]
    //   q1 = q0
    //   q2 = q0 + M*E0
    //   w = [E0,F0,W]

    m_kE0xF0 = m_kE0.Cross(m_kF0);
    m_fE0xF0dD = m_kE0xF0.Dot(m_kD);
    m_fE0xF0dW = m_kE0xF0.Dot(m_kW);

    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,0.0f,-m_fNdF0,afPSort,aiPIndex);
    Sort(m_fE0xF0dD,m_fE0xF0dD,m_fE0xF0dD+m_fMdE0,afQSort,aiQIndex);

    return FindIntervals(AXIS_E0xF0,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE0xF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE0xF1()
{
    // Input:
    //   p0 = 0
    //   p1 = 0
    //   p2 = -N*F1
    //   q0 = [E0,F1,D]
    //   q1 = q0 - M*E0
    //   q2 = q0
    //   w = [E0,F1,W]

    m_kE0xF1 = m_kE0.Cross(m_kF1);
    m_fE0xF1dD = m_kE0xF1.Dot(m_kD);
    m_fE0xF1dW = m_kE0xF1.Dot(m_kW);

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,0.0f,-m_fNdF1,afPSort,aiPIndex);
    Sort(m_fE0xF1dD,m_fE0xF1dD-m_fMdE0,m_fE0xF1dD,afQSort,aiQIndex);

    return FindIntervals(AXIS_E0xF1,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE0xF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE0xF2()
{
    // Input:
    //   p0 = 0
    //   p1 = 0
    //   p2 = -N*F2
    //   q0 = [E0,F2,D]
    //   q1 = q0 - M*E0
    //   q2 = q0
    //   w = [E0,F2,W]

    m_fE0xF2dD = m_fE0xF1dD - m_fE0xF0dD;
    m_fE0xF2dW = m_fE0xF1dW - m_fE0xF0dW;
    m_fNdF2 = m_fNdF1 - m_fNdF0;

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,0.0f,-m_fNdF2,afPSort,aiPIndex);
    float fQ1 = m_fE0xF2dD-m_fMdE0;
    Sort(m_fE0xF2dD,fQ1,fQ1,afQSort,aiQIndex);

    return FindIntervals(AXIS_E0xF2,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE0xF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE1xF0()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F0
    //   p2 = 0
    //   q0 = [E1,F0,D]
    //   q1 = q0
    //   q2 = q0 + M*E1
    //   w = [E1,F0,W]

    m_kE1xF0 = m_kE1.Cross(m_kF0);
    m_fE1xF0dD = m_kE1xF0.Dot(m_kD);
    m_fE1xF0dW = m_kE1xF0.Dot(m_kW);

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,m_fNdF0,0.0f,afPSort,aiPIndex);
    Sort(m_fE1xF0dD,m_fE1xF0dD,m_fE1xF0dD+m_fMdE1,afQSort,aiQIndex);

    return FindIntervals(AXIS_E1xF0,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE1xF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE1xF1()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F1
    //   p2 = 0
    //   q0 = [E1,F1,D]
    //   q1 = q0 - M*E1
    //   q2 = q0
    //   w = [E1,F1,W]

    m_kE1xF1 = m_kE1.Cross(m_kF1);
    m_fE1xF1dD = m_kE1xF1.Dot(m_kD);
    m_fE1xF1dW = m_kE1xF1.Dot(m_kW);

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,m_fNdF1,0.0f,afPSort,aiPIndex);
    Sort(m_fE1xF1dD,m_fE1xF1dD-m_fMdE1,m_fE1xF1dD,afQSort,aiQIndex);

    return FindIntervals(AXIS_E1xF1,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE1xF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE1xF2()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F2
    //   p2 = 0
    //   q0 = [E1,F2,D]
    //   q1 = q0 - M*E1
    //   q2 = q0
    //   w = [E1,F2,W]

    m_fE1xF2dD = m_fE1xF1dD - m_fE1xF0dD;
    m_fE1xF2dW = m_fE1xF1dW - m_fE1xF0dW;

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,m_fNdF2,0.0f,afPSort,aiPIndex);
    float fQ1 = m_fE1xF2dD-m_fMdE1;
    Sort(m_fE1xF2dD,fQ1,fQ1,afQSort,aiQIndex);

    return FindIntervals(AXIS_E1xF2,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE1xF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE2xF0()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F0
    //   p2 = p1
    //   q0 = [E2,F0,D]
    //   q1 = q0
    //   q2 = q0 + M*E2
    //   w = [E2,F0,W]

    m_fE2xF0dD = m_fE1xF0dD - m_fE0xF0dD;
    m_fE2xF0dW = m_fE1xF0dW - m_fE0xF0dW;
    m_fMdE2 = m_fMdE1 - m_fMdE0;

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,m_fNdF0,m_fNdF0,afPSort,aiPIndex);
    Sort(m_fE2xF0dD,m_fE2xF0dD,m_fE2xF0dD+m_fMdE2,afQSort,aiQIndex);

    return FindIntervals(AXIS_E2xF0,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE2xF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE2xF1()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F1
    //   p2 = p1
    //   q0 = [E2,F1,D]
    //   q1 = q0 - M*E2
    //   q2 = q0
    //   w = [E2,F1,W]

    m_fE2xF1dD = m_fE1xF1dD - m_fE0xF1dD;
    m_fE2xF1dW = m_fE1xF1dW - m_fE0xF1dW;

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,m_fNdF1,m_fNdF1,afPSort,aiPIndex);
    Sort(m_fE2xF1dD,m_fE2xF1dD-m_fMdE2,m_fE2xF1dD,afQSort,aiQIndex);

    return FindIntervals(AXIS_E2xF1,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE2xF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindE2xF2()
{
    // Input:
    //   p0 = 0
    //   p1 = N*F2
    //   p2 = p1
    //   q0 = [E2,F2,D]
    //   q1 = q0 - M*E2
    //   q2 = q0
    //   w = [E2,F2,W]

    m_fE2xF2dD = m_fE2xF1dD - m_fE2xF0dD;
    m_fE2xF2dW = m_fE2xF1dW - m_fE2xF0dW;

    // sort values
    float afPSort[3], afQSort[3];
    int aiPIndex[3], aiQIndex[3];
    Sort(0.0f,m_fNdF2,m_fNdF2,afPSort,aiPIndex);
    float fQ1 = m_fE2xF2dD-m_fMdE2;
    Sort(m_fE2xF2dD,fQ1,fQ1,afQSort,aiQIndex);

    return FindIntervals(AXIS_E2xF2,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fE2xF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindMxF0()
{
    // Input:
    //   p0 = 0
    //   p1 = [M,F0,E0]
    //   p2 = [M,F0,E1]
    //   qmin = [M,F0,D]
    //   qmax = qmin + M*M
    //   w = [M,F0,W]

    m_fMdM = m_kM.Dot(m_kM);
    m_kMxF0 = m_kM.Cross(m_kF0);
    m_fMxF0dE0 = m_kMxF0.Dot(m_kE0);
    m_fMxF0dE1 = m_kMxF0.Dot(m_kE1);
    m_fMxF0dD = m_kMxF0.Dot(m_kD);
    m_fMxF0dW = m_kMxF0.Dot(m_kW);

    // sort values
    float afQSort[3] = { m_fMxF0dD, m_fMxF0dD, m_fMxF0dD + m_fMdM };
    int aiQIndex[3] = { 0, 1, 2 };
    float afPSort[3];
    int aiPIndex[3];
    Sort(0.0f,m_fMxF0dE0,m_fMxF0dE1,afPSort,aiPIndex);

    return FindIntervals(AXIS_MxF0,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fMxF0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindMxF1()
{
    // Input:
    //   p0 = 0
    //   p1 = [M,F1,E0]
    //   p2 = [M,F1,E1]
    //   qmax = [M,F1,D]
    //   qmin = qmax - M*M
    //   w = [M,F1,W]

    m_kMxF1 = m_kM.Cross(m_kF1);
    m_fMxF1dE0 = m_kMxF1.Dot(m_kE0);
    m_fMxF1dE1 = m_kMxF1.Dot(m_kE1);
    m_fMxF1dD = m_kMxF1.Dot(m_kD);
    m_fMxF1dW = m_kMxF1.Dot(m_kW);

    // sort values
    float afQSort[3] = { m_fMxF1dD - m_fMdM, m_fMxF1dD, m_fMxF1dD };
    int aiQIndex[3] = { 1, 0, 2 };
    float afPSort[3];
    int aiPIndex[3];
    Sort(0.0f,m_fMxF1dE0,m_fMxF1dE1,afPSort,aiPIndex);

    return FindIntervals(AXIS_MxF1,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fMxF1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindMxF2()
{
    // Input:
    //   p0 = 0
    //   p1 = [M,F2,E0]
    //   p2 = [M,F2,E1]
    //   qmax = [M,F2,D]
    //   qmin = qmax - [M,F2,F0] = qmax - M*M
    //   w = [M,F2,W]

    m_fMxF2dE0 = m_fMxF1dE0 - m_fMxF0dE0;
    m_fMxF2dE1 = m_fMxF1dE1 - m_fMxF0dE1;
    m_fMxF2dD = m_fMxF1dD - m_fMxF0dD;
    m_fMxF2dW = m_fMxF1dW - m_fMxF0dW;

    // sort values
    float fQ1 = m_fMxF2dD - m_fMdM;
    float afQSort[3] = { fQ1, fQ1, m_fMxF2dD };
    int aiQIndex[3] = { 1, 2, 0 };
    float afPSort[3];
    int aiPIndex[3];
    Sort(0.0f,m_fMxF2dE0,m_fMxF2dE1,afPSort,aiPIndex);

    return FindIntervals(AXIS_MxF2,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fMxF2dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindNxE0()
{
    // Input:
    //   q0 = [N,E0,D]
    //   q1 = q0 + [N,E0,F0]
    //   q2 = q0 + [N,E0,F1]
    //   pmin = 0
    //   pmax = N*N
    //   w = [N,E0,W]

    m_fNdN = m_kN.Dot(m_kN);
    m_kNxE0 = m_kN.Cross(m_kE0);
    m_fNxE0dF0 = m_kNxE0.Dot(m_kF0);
    m_fNxE0dF1 = m_kNxE0.Dot(m_kF1);
    m_fNxE0dD = m_kNxE0.Dot(m_kD);
    m_fNxE0dW = m_kNxE0.Dot(m_kW);

    // sort values
    float afPSort[3] = { 0.0f, 0.0f, m_fNdN };
    int aiPIndex[3] = { 0, 1, 2 };
    float afQSort[3];
    int aiQIndex[3];
    Sort(m_fNxE0dD,m_fNxE0dD+m_fNxE0dF0,m_fNxE0dD+m_fNxE0dF1,afQSort,
        aiQIndex);

    return FindIntervals(AXIS_NxE0,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fNxE0dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindNxE1()
{
    // Input:
    //   q0 = [N,E1,D]
    //   q1 = q0 + [N,E1,F0]
    //   q2 = q0 + [N,E1,F1]
    //   pmin = -N*N
    //   pmax = 0
    //   w = [N,E1,W]

    m_kNxE1 = m_kN.Cross(m_kE1);
    m_fNxE1dF0 = m_kNxE1.Dot(m_kF0);
    m_fNxE1dF1 = m_kNxE1.Dot(m_kF1);
    m_fNxE1dD = m_kNxE1.Dot(m_kD);
    m_fNxE1dW = m_kNxE1.Dot(m_kW);

    // sort q-values
    float afPSort[3] = { -m_fNdN, 0.0f, 0.0f };
    int aiPIndex[3] = { 1, 0, 2 };
    float afQSort[3];
    int aiQIndex[3];
    Sort(m_fNxE1dD,m_fNxE1dD+m_fNxE1dF0,m_fNxE1dD+m_fNxE1dF1,afQSort,
        aiQIndex);

    return FindIntervals(AXIS_NxE1,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fNxE1dW);
}
//---------------------------------------------------------------------------
bool NiTriIntersect::FindNxE2()
{
    // Input:
    //   q0 = [N,E2,D]
    //   q1 = q0 + [N,E2,F0]
    //   q2 = q0 + [N,E2,F1]
    //   pmin = -N*N
    //   pmax = 0
    //   w = [N,E2,W]

    m_fNxE2dF0 = m_fNxE1dF0 - m_fNxE0dF0;
    m_fNxE2dF1 = m_fNxE1dF1 - m_fNxE0dF1;
    m_fNxE2dD = m_fNxE1dD - m_fNxE0dD;
    m_fNxE2dW = m_fNxE1dW - m_fNxE0dW;

    // sort q-values
    float afPSort[3] = { -m_fNdN, -m_fNdN, 0.0f };
    int aiPIndex[3] = { 1, 2, 0 };
    float afQSort[3];
    int aiQIndex[3];
    Sort(m_fNxE2dD,m_fNxE2dD+m_fNxE2dF0,m_fNxE2dD+m_fNxE2dF1,afQSort,
        aiQIndex);

    return FindIntervals(AXIS_NxE2,afPSort,aiPIndex,afQSort,aiQIndex,
        m_fNxE2dW);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// locate a first point of contact
//---------------------------------------------------------------------------
void NiTriIntersect::PEdgeQEdgeIntersect(const NiPoint3& rkP0,
    const NiPoint3& rkP1, const NiPoint3& rkQ0, const NiPoint3& rkQ1)
{
    // (1-u)*P0 + u*P1 = Q0 + T*W + v*(Q1-Q0)
    // u*(P1-P0) = Q0+T*W-P0 + v*(Q1-Q0)
    // u*(M*(P1-P0)) = M*(Q0+T*W-P0)
    //
    // If M*(P1-P0) is not zero, then the line segments intersect in a
    // single point.  Otherwise the line segments are collinear and overlap.

    NiPoint3 diffp = rkP1 - rkP0;
    NiPoint3 delta = m_fIntrTime*m_kW;
    NiPoint3 diff0 = rkQ0 + delta - rkP0;
    float fDot = m_kM.Dot(diffp);
    if ( NiAbs(fDot) > ms_fEpsilon )
    {
        // edges intersect at a unique point
        fDot = m_kM.Dot(diff0)/fDot;  // u
        m_kIntrPt = rkP0 + m_fIntrTime*m_rkV0 + fDot*diffp;
    }
    else
    {
        // edges are collinear and overlap
        NiPoint3 diff1 = rkQ1 + delta - rkP1;
        float fDot0 = diffp.Dot(diff0);
        float fDot1 = diffp.Dot(diff1);
        if ( fDot0 <= fDot1 )
        {
            if ( fDot0 >= 0.0f )
            {
                // Q0 is in the intersection
                m_kIntrPt = rkQ0 + m_fIntrTime*m_rkV1;
            }
            else
            {
                // P0 is in the intersection
                m_kIntrPt = rkP0 + m_fIntrTime*m_rkV0;
            }
        }
        else
        {
            if ( fDot1 >= 0.0f )
            {
                // Q1 is in the intersection
                m_kIntrPt = rkQ1 + m_fIntrTime*m_rkV1;
            }
            else
            {
                // P0 is in the intersection
                m_kIntrPt = rkP0 + m_fIntrTime*m_rkV0;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiTriIntersect::PEdgeQFaceIntersect(const NiPoint3& rkP0,
    const NiPoint3& rkP1)
{
    // P = Q0+T*W+r*F0+s*F1 = Q+r*F0+s*F1
    //   P-Q = r*F0+s*F1
    //   F1x(P-Q) = r*(F1xF0) = -r*M
    //   F0x(P-Q) = s*(F0xF1) = s*M
    //   |M|^2 r = -[M,F1,P-Q]
    //   |M|^2 s = +[M,F0,P-Q]

    NiPoint3 cross;
    float fR, fS;
    NiPoint3 delta = (*m_apkQ[0]) + m_fIntrTime*m_kW;

    // test if P0 is inside Q-triangle
    NiPoint3 diff0 = rkP0 - delta;
    cross = m_kF1.Cross(diff0);
    fR = -m_kM.Dot(cross);
    if ( 0.0f <= fR )
    {
        cross = m_kF0.Cross(diff0);
        fS = m_kM.Dot(cross);
        if ( 0.0f <= fS && fR + fS <= m_fMdM )
        {
            m_kIntrPt = rkP0 + m_fIntrTime*m_rkV0;
            return;
        }
    }

    // test if P1 is inside Q-triangle
    NiPoint3 diff1 = rkP1 - delta;
    cross = m_kF1.Cross(diff1);
    fR = -m_kM.Dot(cross);
    if ( 0.0f <= fR )
    {
        cross = m_kF0.Cross(diff1);
        fS = m_kM.Dot(cross);
        if ( 0.0f <= fS && fR + fS <= m_fMdM )
        {
            m_kIntrPt = rkP1 + m_fIntrTime*m_rkV0;
            return;
        }
    }

    // test if intersection lies on edge <Q0,Q1>
    if ( m_fNdF0 > ms_fEpsilon )
    {
        fR = m_kN.Dot(diff0);
        if ( 0.0f <= fR && fR <= m_fNdF0 )
        {
            fR /= m_fNdF0;
            m_kIntrPt = (*m_apkQ[0]) + m_fIntrTime*m_rkV1 + fR*m_kF0;
            return;
        }
    }
    else if ( m_fNdF0 < -ms_fEpsilon )
    {
        fR = m_kN.Dot(diff0);
        if ( m_fNdF0 <= fR && fR <= 0.0f )
        {
            fR /= m_fNdF0;
            m_kIntrPt = (*m_apkQ[0]) + m_fIntrTime*m_rkV1 + fR*m_kF0;
            return;
        }
    }

    // test if intersection lies on edge <Q0,Q2>
    if ( m_fNdF1 > ms_fEpsilon )
    {
        fS = m_kN.Dot(diff0);
        if ( 0.0f <= fS && fS <= m_fNdF1 )
        {
            fS /= m_fNdF1;
            m_kIntrPt = (*m_apkQ[0]) + m_fIntrTime*m_rkV1 + fS*m_kF1;
            return;
        }
    }
    else if ( m_fNdF1 < -ms_fEpsilon )
    {
        fS = m_kN.Dot(diff0);
        if ( m_fNdF1 <= fS && fS <= 0.0f )
        {
            fS /= m_fNdF1;
            m_kIntrPt = (*m_apkQ[0]) + m_fIntrTime*m_rkV1 + fS*m_kF1;
            return;
        }
    }
    
    //NIASSERT( NiAbs(m_kM.Dot(diff0)) <= ms_fEpsilon );
    // Parallel edges, determine if they intersect.  The code
    // below is a measure to at least provide an approximate
    // point of intersection.  (Return vertex Q0)
    m_kIntrPt = (*m_apkQ[0]) + m_fIntrTime*m_rkV1;
    return;
}
//---------------------------------------------------------------------------
void NiTriIntersect::PFaceQEdgeIntersect(const NiPoint3& rkQ0,
    const NiPoint3& rkQ1)
{
    // Q = Qi+T*W = P0+r*E0+s*E1, i one of 0,1,2
    //   Q-P0 = r*E0+s*E1
    //   E1x(Q-P0) = r*(E1xE0) = -r*N
    //   E0x(Q-P0) = s*(E0xE1) = s*N
    //   |N|^2 r = -[N,E1,Q-P0]
    //   |N|^2 s = +[N,E0,Q-P0]

    NiPoint3 cross;
    float fR, fS;
    NiPoint3 delta = m_fIntrTime*m_kW - (*m_apkP[0]);

    // test if Q0 is inside P-triangle
    NiPoint3 diff0 = rkQ0 + delta;
    cross = m_kE1.Cross(diff0);
    fR = -m_kN.Dot(cross);
    if ( 0.0f <= fR )
    {
        cross = m_kE0.Cross(diff0);
        fS = m_kN.Dot(cross);
        if ( 0.0f <= fS && fR + fS <= m_fNdN )
        {
            m_kIntrPt = rkQ0 + m_fIntrTime*m_rkV1;
            return;
        }
    }

    // test if Q1 is inside P-triangle
    NiPoint3 diff1 = rkQ1 + delta;
    cross = m_kE1.Cross(diff1);
    fR = -m_kN.Dot(cross);
    if ( 0.0f <= fR )
    {
        cross = m_kE0.Cross(diff1);
        fS = m_kN.Dot(cross);
        if ( 0.0f <= fS && fR + fS <= m_fNdN )
        {
            m_kIntrPt = rkQ1 + m_fIntrTime*m_rkV1;
            return;
        }
    }

    // test if intersection lies on edge <P0,P1>
    if ( m_fMdE0 > ms_fEpsilon )
    {
        fR = m_kM.Dot(diff0);
        if ( 0.0f <= fR && fR <= m_fMdE0 )
        {
            fR /= m_fMdE0;
            m_kIntrPt = (*m_apkP[0]) + m_fIntrTime*m_rkV0 + fR*m_kE0;
            return;
        }
    }
    else if ( m_fMdE0 < -ms_fEpsilon )
    {
        fR = m_kM.Dot(diff0);
        if ( m_fMdE0 <= fR && fR <= 0.0f )
        {
            fR /= m_fMdE0;
            m_kIntrPt = (*m_apkP[0]) + m_fIntrTime*m_rkV0 + fR*m_kE0;
            return;
        }
    }

    // test if intersection lies on edge <P0,P2>
    if ( m_fMdE1 > ms_fEpsilon )
    {
        fS = m_kM.Dot(diff0);
        if ( 0.0f <= fS && fS <= m_fMdE1 )
        {
            fS /= m_fMdE1;
            m_kIntrPt = (*m_apkP[0]) + m_fIntrTime*m_rkV0 + fS*m_kE1;
            return;
        }
    }
    else if ( m_fMdE1 < -ms_fEpsilon )
    {
        fS = m_kM.Dot(diff0);
        if ( m_fMdE1 <= fS && fS <= 0.0f )
        {
            fS /= m_fMdE1;
            m_kIntrPt = (*m_apkP[0]) + m_fIntrTime*m_rkV0 + fS*m_kE1;
            return;
        }
    }
    
    // NIASSERT( NiAbs(m_kN.Dot(diff0)) <= ms_fEpsilon );
    // Parallel edges, determine if they intersect.  The code
    // below is a measure to at least provide an approximate
    // point of intersection.  (Return vertex P0)
    m_kIntrPt = (*m_apkP[0]) + m_fIntrTime*m_rkV0;
}
//---------------------------------------------------------------------------
void NiTriIntersect::PFaceQFaceIntersect()
{
    // Q+T*W = Q0+T*W+u*F0+v*F1 = P0+r*E0+s*E1
    // |N|^2 r = -([N,E1,D]+T*[N,E1,W]+u*[N,E1,F0]+v*[N,E1,F1])
    //         = a0 + b00*u + b01*v
    // |N|^2 s = +([N,E0,D]+T*[N,E0,W]+u*[N,E0,F0]+v*[N,E0,F1])
    //         = a1 + b10*u + b11*v

    float fR, fS;

    // test (u,v) = (0,0);  Q0 inside P-triangle
    float fA0 = -(m_fNxE1dD + m_fIntrTime*m_fNxE1dW);
    float fA1 = m_fNxE0dD + m_fIntrTime*m_fNxE0dW;
    fR = fA0;
    fS = fA1;
    if ( fR >= 0.0f )
    {
        fS = fA1;
        if ( fS >= 0.0f && fR + fS <= m_fNdN )
        {
            m_kIntrPt = (*m_apkQ[0]) + m_fIntrTime*m_rkV1;
            return;
        }
    }

    // test (u,v) = (1,0); Q1 inside P-triangle
    float fB00 = -m_fNxE1dF0;
    float fB10 = +m_fNxE0dF0;
    fR = fA0 + fB00;
    if ( fR >= 0.0f )
    {
        fS = fA1 + fB10;
        if ( fS >= 0.0f && fR + fS <= m_fNdN )
        {
            m_kIntrPt = (*m_apkQ[1]) + m_fIntrTime*m_rkV1;
            return;
        }
    }

    // test (u,v) = (0,1); Q2 inside P-triangle
    float fB01 = -m_fNxE1dF1;
    float fB11 = +m_fNxE0dF1;
    fR = fA0 + fB01;
    if ( fR >= 0.0f )
    {
        fS = fA1 + fB11;
        if ( fS >= 0.0f && fR + fS <= m_fNdN )
        {
            m_kIntrPt = (*m_apkQ[2]) + m_fIntrTime*m_rkV1;
            return;
        }
    }

    float fDet, fBound;

    // test v=0, s=0; <Q0,Q1> against <P0,P1>
    if ( fB10 > 0.0f )
    {
        fDet = fB10*fA0 - fB00*fA1;
        if ( fDet >= 0.0f )
        {
            fBound = fB10*m_fNdN;
            if ( fDet <= fBound )
            {
                fR = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( fB10 < 0.0f )
    {
        fDet = fB10*fA0 - fB00*fA1;
        if ( fDet <= 0.0f )
        {
            fBound = fB10*m_fNdN;
            if ( fBound <= fDet )
            {
                fR = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( NiAbs(fA1) <= ms_fEpsilon )
    {
        if ( fB00 > 0.0f )
        {
            fR = ( fA0 >= 0.0f ? fA0 : fA0 + fB00 );
        }
        else if ( fB00 < 0.0f )
        {
            fR = ( fA0 <= m_fNdN ? fA0: fA0 + fB00 );
        }
        else
        {
            fR = fA0;
        }

        fR /= m_fNdN;
        m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
        return;
    }

    // test v=0, r=0; <Q0,Q1> against <P0,P2>
    if ( fB00 > 0.0f )
    {
        fDet = fB00*fA1 - fB10*fA0;
        if ( fDet >= 0.0f )
        {
            fBound = fB00*m_fNdN;
            if ( fDet <= fBound )
            {
                fS = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fS*m_kE1 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( fB00 < 0.0f )
    {
        fDet = fB00*fA1 - fB10*fA0;
        if ( fDet <= 0.0f )
        {
            fBound = fB00*m_fNdN;
            if ( fBound <= fDet )
            {
                fS = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fS*m_kE1 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( NiAbs(fA0) <= ms_fEpsilon )
    {
        if ( fB10 > 0.0f )
        {
            fS = ( fA1 >= 0.0f ? fA1 : fA1 + fB10 );
        }
        else if ( fB10 < 0.0f )
        {
            fS = ( fA1 <= m_fNdN ? fA1 : fA1 + fB10 );
        }
        else
        {
            fS = fA1;
        }

        fS /= m_fNdN;
        m_kIntrPt = (*m_apkP[0]) + fS*m_kE0 + m_fIntrTime*m_rkV0;
        return;
    }

    // test u=0, s=0; <Q0,Q2> against <P0,P1>
    if ( fB11 > 0.0f )
    {
        fDet = fB11*fA0 - fB01*fA1;
        if ( fDet >= 0.0f )
        {
            fBound = fB11*m_fNdN;
            if ( fDet <= fBound )
            {
                fR = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( fB11 < 0.0f )
    {
        fDet = fB11*fA0 - fB01*fA1;
        if ( fDet <= 0.0f )
        {
            fBound = fB11*m_fNdN;
            if ( fBound <= fDet )
            {
                fR = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( NiAbs(fA1) <= ms_fEpsilon )
    {
        if ( fB01 > 0.0f )
        {
            fR = ( fA0 >= 0.0f ? fA0 : fA0 + fB01 );
        }
        else if ( fB01 < 0.0f )
        {
            fR = ( fA0 <= m_fNdN ? fA0 : fA0 + fB01 );
        }
        else
        {
            fR = fA0;
        }

        fR /= m_fNdN;
        m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
        return;
    }

    // test u=0, r=0; <Q0,Q2> against <P0,P2>
    if ( fB01 > 0.0f )
    {
        fDet = fB01*fA1 - fB11*fA0;
        if ( fDet >= 0.0f )
        {
            fBound = fB01*m_fNdN;
            if ( fDet <= fBound )
            {
                fS = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fS*m_kE1 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( fB01 < 0.0f )
    {
        fDet = fB01*fA1 - fB11*fA0;
        if ( fDet <= 0.0f )
        {
            fBound = fB01*m_fNdN;
            if ( fBound <= fDet )
            {
                fS = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fS*m_kE1 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( NiAbs(fA0) <= ms_fEpsilon )
    {
        if ( fB11 > 0.0f )
        {
            fS = ( fA1 >= 0.0f ? fA1 : fA1 + fB11 );
        }
        else if ( fB11 < 0.0f )
        {
            fS = ( fA1 <= m_fNdN ? fA1 : fA1 + fB11 );
        }
        else
        {
            fS = fA1;
        }

        fS /= m_fNdN;
        m_kIntrPt = (*m_apkP[0]) + fS*m_kE0 + m_fIntrTime*m_rkV0;
        return;
    }

    float fB00mB01 = fB00 - fB01;
    float fB10mB11 = fB10 - fB11;
    float fA0pB01 = fA0 + fB01;
    float fA1pB11 = fA1 + fB11;

    // test u+v=1, s=0; <Q1,Q2> against <P0,P1>
    if ( fB10mB11 > 0.0f )
    {
        fDet = fB10mB11*fA0pB01 - fB00mB01*fA1pB11;
        if ( fDet >= 0.0f )
        {
            fBound = fB10mB11*m_fNdN;
            if ( fDet <= fBound )
            {
                fR = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( fB10mB11 < 0.0f )
    {
        fDet = fB10mB11*fA0pB01 - fB00mB01*fA1pB11;
        if ( fDet <= 0.0f )
        {
            fBound = fB10mB11*m_fNdN;
            if ( fBound <= fDet )
            {
                fR = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( NiAbs(fA1pB11) <= ms_fEpsilon )
    {
        if ( fB00mB01 > 0.0f )
        {
            fR = ( fA0pB01 >= 0.0f ? fA0pB01 : fA0pB01 + fB00mB01 );
        }
        else if ( fB00mB01 < 0.0f )
        {
            fR = ( fA0pB01 <= m_fNdN ? fA0pB01 : fA0pB01 + fB00mB01 );
        }
        else
        {
            fR = fA0pB01;
        }

        fR /= m_fNdN;
        m_kIntrPt = (*m_apkP[0]) + fR*m_kE0 + m_fIntrTime*m_rkV0;
    }

    // test u+v=1, r=0; <Q1,Q2> against <P0,P2>
    if ( fB00mB01 > 0.0f )
    {
        fDet = fB00mB01*fA1pB11 - fB10mB11*fA0pB01;
        if ( fDet >= 0.0f )
        {
            fBound = fB00mB01*m_fNdN;
            if ( fDet <= fBound )
            {
                fS = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fS*m_kE1 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( fB00mB01 < 0.0f )
    {
        fDet = fB00mB01*fA1pB11 - fB10mB11*fA0pB01;
        if ( fDet <= 0.0f )
        {
            fBound = fB00mB01*m_fNdN;
            if ( fBound <= fDet )
            {
                fS = fDet/fBound;
                m_kIntrPt = (*m_apkP[0]) + fS*m_kE1 + m_fIntrTime*m_rkV0;
                return;
            }
        }
    }
    else if ( NiAbs(fA0pB01) <= ms_fEpsilon )
    {
        if ( fB10mB11 > 0.0f )
        {
            fS = ( fA1pB11 >= 0.0f ? fA1pB11 : fA1pB11 + fB10mB11 );
        }
        else if ( fB10mB11 < 0.0f )
        {
            fS = ( fA1pB11 <= m_fNdN ? fA1pB11 : fA1pB11 + fB10mB11 );
        }
        else
        {
            fS = fA1pB11;
        }

        fS /= m_fNdN;
        m_kIntrPt = (*m_apkP[0]) + fS*m_kE0 + m_fIntrTime*m_rkV0;
        return;
    }

    // p-triangle contained in q-triangle
    m_kIntrPt = (*m_apkP[0]) + m_fIntrTime*m_rkV0;
}
//---------------------------------------------------------------------------
void NiTriIntersect::LocateIntersection()
{
    if ( m_uiAxis == AXIS_NONE )
    {
        // Triangles already intersecting.
        //
        // FOR NOW.  Choose an approximate point of intersection.  Use the
        // centroid of the smaller area triangle.  The application should
        // check to see if the intersection time is zero and then decide
        // whether or not the intersection point should really be used.
        static const float s_fOneThird = 1.0f/3.0f;
        m_fNdN = m_kN.Dot(m_kN);
        m_fMdM = m_kM.Dot(m_kM);
        if ( m_fNdN <= m_fMdM )
            m_kIntrPt = ((*m_apkP[0])+(*m_apkP[1])+(*m_apkP[2]))*s_fOneThird;
        else
            m_kIntrPt = ((*m_apkQ[0])+(*m_apkQ[1])+(*m_apkQ[2]))*s_fOneThird;
        return;
    }

    if ( m_afQSort[0] > m_afPSort[2] )
    {
        // ordering is pmin <= pmid <= pmax <= qmin <= qmid <= qmax
        if ( m_afPSort[1] < m_afPSort[2] )
        {
            // P point, Q any
            m_kIntrPt = (*m_apkP[m_aiPIndex[2]]) + m_fIntrTime*m_rkV0;
        }
        else if ( m_afPSort[0] < m_afPSort[1] )
        {
            if ( m_afQSort[0] < m_afQSort[1] )
            {
                // P edge, Q point
                m_kIntrPt = (*m_apkQ[m_aiQIndex[0]]) + m_fIntrTime*m_rkV1;
            }
            else if ( m_afQSort[1] < m_afQSort[2] )
            {
                PEdgeQEdgeIntersect(*m_apkP[m_aiPIndex[1]],
                    *m_apkP[m_aiPIndex[2]],*m_apkQ[m_aiQIndex[0]],
                    *m_apkQ[m_aiQIndex[1]]);
            }
            else
            {
                PEdgeQFaceIntersect(*m_apkP[m_aiPIndex[1]],
                    *m_apkP[m_aiPIndex[2]]);
            }
        }
        else
        {
            if ( m_afQSort[0] < m_afQSort[1] )
            {
                // P face, Q point
                m_kIntrPt = (*m_apkQ[m_aiQIndex[0]]) + m_fIntrTime*m_rkV1;
            }
            else if ( m_afQSort[1] < m_afQSort[2] )
            {
                PFaceQEdgeIntersect(*m_apkQ[m_aiQIndex[0]],
                    *m_apkQ[m_aiQIndex[1]]);
            }
            else
            {
                PFaceQFaceIntersect();
            }
        }
    }
    else if ( m_afQSort[2] < m_afPSort[0] )
    {
        // ordering is qmin <= qmid <= qmax <= pmin <= pmid <= pmax
        if ( m_afPSort[0] < m_afPSort[1] )
        {
            // P point, Q any
            m_kIntrPt = (*m_apkP[m_aiPIndex[0]]) + m_fIntrTime*m_rkV0;
        }
        else if ( m_afPSort[1] < m_afPSort[2] )
        {
            if ( m_afQSort[1] < m_afQSort[2] )
            {
                // P edge, Q point
                m_kIntrPt = (*m_apkQ[m_aiQIndex[2]]) + m_fIntrTime*m_rkV1;
            }
            else if ( m_afQSort[0] < m_afQSort[1] )
            {
                PEdgeQEdgeIntersect(*m_apkP[m_aiPIndex[0]],
                    *m_apkP[m_aiPIndex[1]],*m_apkQ[m_aiQIndex[1]],
                    *m_apkQ[m_aiQIndex[2]]);
            }
            else
            {
                PEdgeQFaceIntersect(*m_apkP[m_aiPIndex[0]],
                    *m_apkP[m_aiPIndex[1]]);
            }
        }
        else
        {
            if ( m_afQSort[1] < m_afQSort[2] )
            {
                // P face, Q point
                m_kIntrPt = (*m_apkQ[m_aiQIndex[2]]) + m_fIntrTime*m_rkV1;
            }
            else if ( m_afQSort[0] < m_afQSort[1] )
            {
                PFaceQEdgeIntersect(*m_apkQ[m_aiQIndex[1]],
                    *m_apkQ[m_aiQIndex[2]]);
            }
            else
            {
                PFaceQFaceIntersect();
            }
        }
    }
    else
    {
        // This assert is not a temporary thing.  If the FindIntersections
        // call works properly, then we should never get here.
        NIASSERT(0);
    }
}
//---------------------------------------------------------------------------
