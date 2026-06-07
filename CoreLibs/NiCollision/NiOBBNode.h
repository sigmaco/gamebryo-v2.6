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

#ifndef NIOBBNODE_H
#define NIOBBNODE_H

#include "NiCollisionGroup.h"
#include "NiOBBox.h"
#include "NiPick.h"

class NiAVObject;
class NiOBBRoot;

class NICOLLISION_ENTRY NiOBBNode : public NiMemObject
{
public:
    // construction and destruction
    NiOBBNode(NiOBBRoot* pRoot, unsigned short usTriangles,
        const unsigned short* pTriList, const NiPoint3* pVertex,
        const NiPoint3* pWVertex, int iBinSize);
    virtual ~NiOBBNode();

    // attributes
    virtual bool IsLeaf();
    inline bool HasChildren();
    inline const NiOBBox& GetBox() const;
    inline NiOBBNode* GetLeft();
    inline NiOBBNode* GetRight();

    // update bounding boxes
    void Transform(const NiTransform& xform);

    // collisions
    virtual bool TestCollision(NiOBBNode* pTestNode, NiAVObject* pkObj0,
        NiAVObject* pkObj1, int iMaxDepth, int iTestMaxDepth,
        unsigned int uiChangeStamp0, unsigned int uiChangeStamp1);

    virtual int FindCollisions(NiOBBNode* pTestNode, NiAVObject* pRoot0,
        NiAVObject* pRoot1, NiAVObject* pObj0, NiAVObject* pObj1,
        int iMaxDepth, int iTestMaxDepth, unsigned int uiChangeStamp0,
        unsigned int uiChangeStamp1, bool& bCollision);

    inline void TransformBox(const NiTransform& xform);

    inline void SetChangeStamp(unsigned int uiChangeStamp);
    inline unsigned int GetChangeStamp() const;

    virtual int FindIntersections(const NiPoint3& origin, 
        const NiPoint3& dir, NiPick& pick, bool& bIntersects, 
        NiAVObject* pObj, unsigned int uiChangeStamp);

protected:
    // construction
    NiOBBNode();

    // bounding box construction
    void CreateRecursive(NiOBBRoot* pRoot, const unsigned short* pTriList,
        const NiPoint3* pVertex, const NiPoint3* pWVertex, float* pArea,
        NiPoint3* pCentroid, int i0, int i1, int* pISplit, int* pOSplit,
        int iBinSize);

    void ComputeOBB(NiOBBRoot* pRoot, const unsigned short* pTriList,
        const NiPoint3* pVertex, float* pArea, NiPoint3* pCentroid,
        int i0, int i1, int* pISplit);

    NiMatrix3 ComputeCovarianceAndMean(const unsigned short* pTriList,
        const NiPoint3* pVertex, float* pArea, NiPoint3* pCentroid,
        int i0, int i1, int* pISplit);

    void DivideTriangles(NiPoint3* pCentroid, int i0, int i1, int* pISplit,
        int& j0, int& j1, int* pOSplit);

    // collisions
    int CollisionCallback(NiCollisionGroup::Intersect& intr);

    int BoxBoxCollision(NiOBBNode* pTestNode, NiAVObject* pRoot0,
        NiAVObject* pRoot1, NiAVObject* pObj0, NiAVObject* pObj1,
        bool& bCollision);

    NiOBBox m_box;
    NiOBBNode* m_pLeft;
    NiOBBNode* m_pRight;
    unsigned int m_uiChangeStamp;
};

#include "NiOBBNode.inl"

#endif

