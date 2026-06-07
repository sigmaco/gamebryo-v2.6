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

#ifndef NIBOX_H
#define NIBOX_H

#include "NiCollisionLibType.h"
#include "NiPoint3.h"
#include <NiDataStreamElementLock.h>
#include <NiDataStreamRef.h>

class NiMesh;

class NICOLLISION_ENTRY NiBox : public NiMemObject
{
public:
    // Box is B(s0,s1,s2) = C + s0*e0*A0 + s1*e0*A1 + s2*e2*A2 for
    // |s_i| <= 1 for i = 0,1,2.  Also, matrix [A0|A1|A2] is orthonormal
    // with determinant 1.
    NiPoint3 m_kCenter;    // C
    NiPoint3 m_akAxis[3];  // A0, A1, A2
    float m_afExtent[3];   // e0, e1, e2


    // generates point C + p0*A0 + p1*A1 + p2*A2
    NiPoint3 GetPoint(float fP0, float fP1, float fP2) const;

    // comparison
    bool operator==(const NiBox& kBox) const;
    bool operator!=(const NiBox& kBox) const;

    void CreateFromData(unsigned short usQuantity, const NiPoint3* pkVertex);
    void CreateFromData(NiMesh* pkMesh);

    // streaming
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);

protected:
    void InitializeMinMaxAndCenter(NiDataStreamRef* pkStreamRef,
        NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount, 
        float& fMinX, float& fMaxX, float& fMinY, float& fMaxY, float& fMinZ,
        float& fMaxZ);
    void CalculateMinMaxAndCenter(NiDataStreamRef* pkStreamRef, 
        NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount,
        float& fMinX, float& fMaxX, float& fMinY, float& fMaxY, float& fMinZ,
        float& fMaxZ);
    void ComputeCovarianceAndSolve(NiDataStreamRef* pkStreamRef, 
        NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount);
    void ComputeCenterAndExtent(NiDataStreamRef* pkStreamRef, 
        NiDataStreamElementLock& kLockPosition, NiUInt32 uiSubmeshCount);
    bool IsDataDegenerate(float fMinX, float fMaxX, float fMinY,
        float fMaxY, float fMinZ, float fMaxZ);
};

#endif

