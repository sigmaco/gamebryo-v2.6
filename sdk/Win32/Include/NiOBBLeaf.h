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

#ifndef NIOBBLEAF_H
#define NIOBBLEAF_H

#include "NiOBBNode.h"
#include "NiPick.h"

class NiAVObject;
class NiPoint3;

class NICOLLISION_ENTRY NiOBBLeaf : public NiOBBNode
{
public:
    // construction
    NiOBBLeaf (NiOBBRoot* pRoot, const unsigned short* pTriList,
        const NiPoint3* pVertex, const NiPoint3* pWVertex,
        unsigned short usTriangle);

    // attributes
    virtual bool IsLeaf ();

    // collisions
    virtual bool TestCollision (NiOBBNode* pTestNode, NiAVObject* pkObj0,
        NiAVObject* pkObj1,int iMaxDepth, int iTestMaxDepth, 
        unsigned int uiChangeStamp0, unsigned int uiChangeStamp1);

    virtual int FindCollisions (NiOBBNode* pTestNode, NiAVObject* pRoot0,
        NiAVObject* pRoot1, NiAVObject* pObj0, NiAVObject* pObj1,
        int iMaxDepth, int iTestMaxDepth, unsigned int uiChangeStamp0,
        unsigned int uiChangeStamp1, bool& bCollision);

    virtual int FindIntersections (const NiPoint3& origin,
        const NiPoint3& dir, NiPick& pick, bool& bIntersects,
        NiAVObject* pObj, unsigned int uiChangeStamp);

protected:
    // intersections
    static void ComputeBarycenter (const NiPoint3& v0, const NiPoint3& v1,
        const NiPoint3& v2, const NiPoint3& p, float pCenter[3]);

    static int TentativeCollision (float fDT, const NiPoint3& N, float fC,
        const NiPoint3& V, const NiPoint3& q0, const NiPoint3& q1,
        float& fT0, NiPoint3& p0, float& fT1, NiPoint3& p1);

    static int TestEdgeTri (const NiPoint3& edge0, const NiPoint3& edge1,
        const NiPoint3* pVer[3], const NiPoint3& N, float fC,
        const NiPoint3& V, float fDT, float& fTMin, NiPoint3& pmin);

    static int FindIntersection (const NiPoint3* pVer0[3],
        const NiPoint3* pVer1[3], const NiPoint3& V, float fDT, float& fTMin,
        NiPoint3& pmin);

    static int FindIntersection (NiOBBLeaf* pLeaf0, NiOBBLeaf* pLeaf1,
        float& fT, NiPoint3& p);

    int FaceFaceCollision (NiOBBLeaf* pTestLeaf, NiAVObject* pRoot0,
        NiAVObject* pRoot1, NiAVObject* pObj0, NiAVObject* pObj1,
        bool& bCollision);


    // pointers to triangle's world vertices
    const NiPoint3* m_pWorldVertex[3];
};

#include "NiOBBLeaf.inl"

#endif
