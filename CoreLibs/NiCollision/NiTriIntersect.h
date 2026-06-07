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

#ifndef NITRIINTERSECT_H
#define NITRIINTERSECT_H

#include "NiCollisionLibType.h"
#include <NiPoint3.h>

class NICOLLISION_ENTRY NiTriIntersect : public NiMemObject
{
public:

    // *** begin Emergent internal use only ***

    NiTriIntersect (const NiPoint3* apkP[3], const NiPoint3& rkV0,
        const NiPoint3* apkQ[3], const NiPoint3& rkV1);

    bool Test (float fTime);

    bool Find (float fTime, float& fIntrTime, NiPoint3& kIntrPt,
        bool bCalcNormals = false, NiPoint3* pkNormal0 = 0,
        NiPoint3* pkNormal1 = 0);

    // *** end Emergent internal use only ***

protected:
    void Sort (float fR0, float fR1, float& rfMin, float& rfMax);
    void Sort (float fR0, float fR1, float fR2, float& rfMin, float& rfMax);
    void Sort (float fR0, float fR1, float fR2, float afSort[3],
        int aiIndex[3]);

    bool TestIntervals (float fPMin, float fPMax, float fQMin, float fQMax,
        float fW);

    bool FindIntervals (unsigned int uiAxis, float afPSort[3],
        int aiPIndex[3], float afQSort[3], int aiQIndex[3], float fW);

    // test any triangles
    bool TestN ();
    bool TestM ();

    // test nonparallel triangles only
    bool TestE0xF0 ();
    bool TestE0xF1 ();
    bool TestE0xF2 ();
    bool TestE1xF0 ();
    bool TestE1xF1 ();
    bool TestE1xF2 ();
    bool TestE2xF0 ();
    bool TestE2xF1 ();
    bool TestE2xF2 ();

    // test parallel triangles only
    bool TestMxF0 ();
    bool TestMxF1 ();
    bool TestMxF2 ();
    bool TestNxE0 ();
    bool TestNxE1 ();
    bool TestNxE2 ();

    // find any triangles
    bool FindN ();
    bool FindM ();

    // find nonparallel triangles only
    bool FindE0xF0 ();
    bool FindE0xF1 ();
    bool FindE0xF2 ();
    bool FindE1xF0 ();
    bool FindE1xF1 ();
    bool FindE1xF2 ();
    bool FindE2xF0 ();
    bool FindE2xF1 ();
    bool FindE2xF2 ();

    // find parallel triangles only
    bool FindMxF0 ();
    bool FindMxF1 ();
    bool FindMxF2 ();
    bool FindNxE0 ();
    bool FindNxE1 ();
    bool FindNxE2 ();

    // locate any triangles
    void PEdgeQEdgeIntersect (const NiPoint3& rkP0, const NiPoint3& rkP1,
        const NiPoint3& rkQ0, const NiPoint3& rkQ1);
    void PEdgeQFaceIntersect (const NiPoint3& rkP0, const NiPoint3& rkP1);
    void PFaceQEdgeIntersect (const NiPoint3& rkQ0, const NiPoint3& rkQ1);
    void PFaceQFaceIntersect ();
    void LocateIntersection ();


    // input triangles and velocities
    const NiPoint3* m_apkP[3];
    const NiPoint3& m_rkV0;
    const NiPoint3* m_apkQ[3];
    const NiPoint3& m_rkV1;

    NiPoint3 m_kD, m_kW, m_kNxM;
    float m_fNxMdNxM;

    NiPoint3 m_kE0xF0, m_kE0xF1, m_kE1xF0, m_kE1xF1;
    float m_fE0xF0dD, m_fE0xF1dD, m_fE0xF2dD;
    float m_fE1xF0dD, m_fE1xF1dD, m_fE1xF2dD;
    float m_fE2xF0dD, m_fE2xF1dD, m_fE2xF2dD;
    float m_fE0xF0dW, m_fE0xF1dW, m_fE0xF2dW;
    float m_fE1xF0dW, m_fE1xF1dW, m_fE1xF2dW;
    float m_fE2xF0dW, m_fE2xF1dW, m_fE2xF2dW;

    NiPoint3 m_kE0, m_kE1, m_kN, m_kNxE0, m_kNxE1, m_kNxE2;
    float m_fNdN, m_fNdF0, m_fNdF1, m_fNdF2, m_fNdD, m_fNdW;
    float m_fNxE0dD, m_fNxE1dD, m_fNxE2dD;
    float m_fNxE0dW, m_fNxE1dW, m_fNxE2dW;
    float m_fNxE0dF0, m_fNxE0dF1, m_fNxE0dF2;
    float m_fNxE1dF0, m_fNxE1dF1, m_fNxE1dF2;
    float m_fNxE2dF0, m_fNxE2dF1, m_fNxE2dF2;

    NiPoint3 m_kF0, m_kF1, m_kM, m_kMxF0, m_kMxF1, m_kMxF2;
    float m_fMdM, m_fMdE0, m_fMdE1, m_fMdE2, m_fMdD, m_fMdW;
    float m_fMxF0dD, m_fMxF1dD, m_fMxF2dD;
    float m_fMxF0dW, m_fMxF1dW, m_fMxF2dW;
    float m_fMxF0dE0, m_fMxF0dE1, m_fMxF0dE2;
    float m_fMxF1dE0, m_fMxF1dE1, m_fMxF1dE2;
    float m_fMxF2dE0, m_fMxF2dE1, m_fMxF2dE2;

    // potential separating axes
    enum
    {
        AXIS_NONE  = 0x00000000,
        AXIS_N     = 0x00000001,
        AXIS_M     = 0x00000002,
        AXIS_E0xF0 = 0x00000004,
        AXIS_E0xF1 = 0x00000008,
        AXIS_E0xF2 = 0x00000010,
        AXIS_E1xF0 = 0x00000020,
        AXIS_E1xF1 = 0x00000040,
        AXIS_E1xF2 = 0x00000080,
        AXIS_E2xF0 = 0x00000100,
        AXIS_E2xF1 = 0x00000200,
        AXIS_E2xF2 = 0x00000400,
        AXIS_NxE0  = 0x00000800,
        AXIS_NxE1  = 0x00001000,
        AXIS_NxE2  = 0x00002000,
        AXIS_MxF0  = 0x00004000,
        AXIS_MxF1  = 0x00008000,
        AXIS_MxF2  = 0x00010000
    };

    // for finding intersection points
    unsigned int m_uiAxis;
    float m_afPSort[3], m_afQSort[3];
    int m_aiPIndex[3], m_aiQIndex[3];

    float m_fTime, m_fIntrTime;
    NiPoint3 m_kIntrPt;

    static float ms_fEpsilon;

private:
    NiTriIntersect & operator=( const NiTriIntersect & );
};

#endif
