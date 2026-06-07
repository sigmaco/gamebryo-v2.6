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

#include "NiMainPCH.h"
#include "NiFrustumPlanes.h"

//---------------------------------------------------------------------------
void NiFrustumPlanes::Set(const NiCamera& kCam)
{
    Set(kCam.GetViewFrustum(), kCam.GetWorldTransform());
}
//---------------------------------------------------------------------------
void NiFrustumPlanes::Set(const NiFrustum& kFrust, 
    const NiTransform& kXform)
{
    NiPoint3 kLoc = kXform.m_Translate;
    NiPoint3 kDVector, kUVector, kRVector;
    kXform.m_Rotate.GetCol(0, kDVector);
    kXform.m_Rotate.GetCol(1, kUVector);
    kXform.m_Rotate.GetCol(2, kRVector);

    NiPoint3 kPoint = kLoc + kFrust.m_fNear * kDVector;
    m_akCullingPlanes[NEAR_PLANE] = NiPlane(kDVector, kPoint);

    kPoint = kLoc + kFrust.m_fFar * kDVector;
    m_akCullingPlanes[FAR_PLANE] = NiPlane(-kDVector, kPoint);

    if (kFrust.m_bOrtho)
    {
        kPoint = kLoc + kFrust.m_fLeft * kRVector;
        m_akCullingPlanes[LEFT_PLANE] = NiPlane(kRVector, kPoint);

        kPoint = kLoc + kFrust.m_fRight * kRVector;
        m_akCullingPlanes[RIGHT_PLANE] = NiPlane(-kRVector, kPoint);

        kPoint = kLoc + kFrust.m_fTop * kUVector;
        m_akCullingPlanes[TOP_PLANE] = NiPlane(-kUVector, kPoint);

        kPoint = kLoc + kFrust.m_fBottom * kUVector;
        m_akCullingPlanes[BOTTOM_PLANE] = NiPlane(kUVector, kPoint);
    }
    else
    {
        float fTmp = kFrust.m_fLeft * kFrust.m_fLeft;
        float fInv = 1.0f / NiSqrt(1.0f + fTmp);
        float fC0 = -kFrust.m_fLeft * fInv;
        float fC1 = fInv;
        NiPoint3 kNormal = fC0 * kDVector + fC1 * kRVector;
        m_akCullingPlanes[LEFT_PLANE] = NiPlane(kNormal, kLoc);

        fTmp = kFrust.m_fRight * kFrust.m_fRight;
        fInv = 1.0f / NiSqrt(1.0f + fTmp);
        fC0 = kFrust.m_fRight * fInv;
        fC1 = -fInv;
        kNormal = fC0 * kDVector + fC1 * kRVector;
        m_akCullingPlanes[RIGHT_PLANE] = NiPlane(kNormal, kLoc);

        fTmp = kFrust.m_fTop * kFrust.m_fTop;
        fInv = 1.0f / NiSqrt(1.0f + fTmp);
        fC0 = kFrust.m_fTop * fInv;
        fC1 = -fInv;
        kNormal = fC0 * kDVector + fC1 * kUVector;
        m_akCullingPlanes[TOP_PLANE] = NiPlane(kNormal, kLoc);

        fTmp = kFrust.m_fBottom * kFrust.m_fBottom;
        fInv = 1.0f / NiSqrt(1.0f + fTmp);
        fC0 = -kFrust.m_fBottom * fInv;
        fC1 = fInv;
        kNormal = fC0 * kDVector + fC1 * kUVector;
        m_akCullingPlanes[BOTTOM_PLANE] = NiPlane(kNormal, kLoc);
    }
}
//---------------------------------------------------------------------------
