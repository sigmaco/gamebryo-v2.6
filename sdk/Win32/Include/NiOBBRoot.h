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

#ifndef NIOBBROOT_H
#define NIOBBROOT_H

#include "NiCollisionGroup.h"
#include <NiMatrix3.h>
#include "NiPick.h"
#include <NiPoint3.h>

class NiNode;
class NiOBBNode;

class NICOLLISION_ENTRY NiOBBRoot : public NiMemObject
{
public:
    // construction and destruction
    NiOBBRoot (unsigned short usTriangles, const unsigned short* pTriList,
        const NiPoint3* pVertex, const NiPoint3* pWVertex, int iBinSize);
    virtual ~NiOBBRoot ();

    // attributes
    inline NiOBBNode* GetOBBTree ();
    inline void SetDeltaTime (float fDeltaTime);
    inline float GetDeltaTime () const;
    inline void SetVelocity (const NiPoint3& velocity);
    inline const NiPoint3& GetVelocity () const;

    // update bounding boxes of obb tree
    void Transform (const NiTransform& xform);

    // collisions
    virtual bool TestCollision (NiOBBRoot* pTestRoot, NiAVObject* pThisObj,
        NiAVObject* pOtherObj, int iMaxDepth, int iTestMaxDepth);

    virtual int FindCollisions (NiOBBRoot* pTestRoot, NiAVObject* pThisRoot,
        NiAVObject* pOtherRoot, NiAVObject* pThisObj, NiAVObject* pOtherObj,
        int iMaxDepth, int iTestMaxDepth, bool& bCollision);

    inline void SetChangeStamp (unsigned int uiChangeStamp);
    inline unsigned int GetChangeStamp () const;

    virtual int FindIntersections (const NiPoint3& origin, 
        const NiPoint3& dir, NiPick& pick, bool& bIntersects,
        NiAVObject* pObj, unsigned int uiChangeStamp);

protected:
    float m_fDeltaTime;
    NiOBBNode* m_pTree;
    bool m_bTransformChanged;

    // world quantities
    NiPoint3 m_velocity;
    NiTransform m_xform;

    unsigned int m_uiChangeStamp;
};

#include "NiOBBRoot.inl"

#endif

