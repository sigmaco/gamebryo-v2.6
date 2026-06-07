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

#include <NiAVObject.h>
#include "NiCollisionData.h"
#include <NiMatrix3.h>
#include "NiOBBRoot.h"
#include "NiOBBLeaf.h"
#include "NiOBBNode.h"

//---------------------------------------------------------------------------
NiOBBNode::NiOBBNode(NiOBBRoot* pRoot, unsigned short usTriangles, 
    const unsigned short* pTriList, const NiPoint3* pVertex, 
    const NiPoint3* pWVertex, int iBinSize)
{
    // compute triangle areas and centroids
    float* pArea = NiAlloc(float,usTriangles);
    NIASSERT(pArea);
    NiPoint3* pCentroid = NiNew NiPoint3[usTriangles];
    NIASSERT(pCentroid);
    unsigned short i, j;
    for (i = 0, j = 0; i < usTriangles; i++)
    {
        unsigned short i0 = pTriList[j++];
        unsigned short i1 = pTriList[j++];
        unsigned short i2 = pTriList[j++];
        NiPoint3 edge1 = pVertex[i1]-pVertex[i0];
        NiPoint3 edge2 = pVertex[i2]-pVertex[i0];
        NiPoint3 cross = edge1.Cross(edge2);
        pArea[i] = 0.5f*cross.Length();
        pCentroid[i] = (pVertex[i0]+pVertex[i1]+pVertex[i2])/3.0f;
    }
    
    // initialize binary-tree arrays for storing triangle indices
    int* pISplit = NiAlloc(int, usTriangles);
    NIASSERT(pISplit);
    for (int k = 0; k < usTriangles; k++)
    {
        pISplit[k] = k;
    }
    int* pOSplit = NiAlloc(int, usTriangles);
    NIASSERT(pOSplit);
    
    // create OBB tree(recursive)
    CreateRecursive(pRoot, pTriList, pVertex, pWVertex, pArea, pCentroid, 0, 
        usTriangles-1, pISplit, pOSplit, iBinSize);

    NiFree(pOSplit);
    NiFree(pISplit);
    NiDelete[] pCentroid;
    NiFree(pArea);

    m_uiChangeStamp = 0;
}
//---------------------------------------------------------------------------
NiOBBNode::~NiOBBNode()
{
    if (m_pLeft)
    {
        NIASSERT(m_pRight);
        NiDelete m_pLeft;
        NiDelete m_pRight;
    }
}
//---------------------------------------------------------------------------
void NiOBBNode::CreateRecursive(NiOBBRoot* pRoot, 
    const unsigned short* pTriList, const NiPoint3* pVertex, 
    const NiPoint3* pWVertex, float* pArea, NiPoint3* pCentroid, int i0, 
    int i1, int* pISplit, int* pOSplit, int iBinSize)
{
    ComputeOBB(pRoot, pTriList, pVertex, pArea, pCentroid, i0, i1, pISplit);

    if (i1-i0 < iBinSize)
    {
        m_pLeft = NULL;
        m_pRight = NULL;
        return;
    }

    int j0, j1;
    DivideTriangles(pCentroid, i0, i1, pISplit, j0, j1, pOSplit);

    if (i0 < j0)
    {
        m_pLeft = NiNew NiOBBNode;
        NIASSERT(m_pLeft);
        m_pLeft->CreateRecursive(pRoot, pTriList, pVertex, pWVertex, pArea, 
            pCentroid, i0, j0, pOSplit, pISplit, iBinSize);
    }
    else if (i0 == j0)
    {
        m_pLeft = NiNew NiOBBLeaf(pRoot, pTriList, pVertex, pWVertex, 
            (unsigned short)pOSplit[i0]);
        NIASSERT(m_pLeft);
    }
    else
    {
        // bad split of triangles, should not get here
        NIASSERT(i0 <= j0);
    }

    if (j1 < i1)
    {
        m_pRight = NiNew NiOBBNode;
        NIASSERT(m_pRight);
        m_pRight->CreateRecursive(pRoot, pTriList, pVertex, pWVertex, pArea, 
            pCentroid, j1, i1, pOSplit, pISplit, iBinSize);
    }
    else if (j1 == i1)
    {
        m_pRight = NiNew NiOBBLeaf(pRoot, pTriList, pVertex, pWVertex, 
            (unsigned short)pOSplit[i1]);
        NIASSERT(m_pRight);
    }
    else
    {
        // bad split of triangles, should not get here
        NIASSERT(j1 <= i1);
    }
}
//---------------------------------------------------------------------------
void NiOBBNode::ComputeOBB(NiOBBRoot* pRoot, const unsigned short* pTriList, 
    const NiPoint3* pVertex, float* pArea, NiPoint3* pCentroid, int i0, 
    int i1, int* pISplit)
{
    // access to box attributes
    NiPoint3& center = m_box.GetCenter();
    NiPoint3* basis = m_box.GetBasis();
    float* extent = m_box.GetExtent();

    // compute box basis
    NiMatrix3 C = ComputeCovarianceAndMean(pTriList, pVertex, pArea, 
        pCentroid, i0, i1, pISplit);
    float eigenvalue[3];
    C.EigenSolveSymmetric(eigenvalue, basis);

    // find the bounding box extents
    NiPoint3 lower(0.0f, 0.0f, 0.0f);
    NiPoint3 upper(0.0f, 0.0f, 0.0f);
    for (unsigned short i = (unsigned short)i0; i <= i1; i++)
    {
        // get triangle
        int usTriangle = pISplit[i];
        int j = 3*usTriangle;
        const NiPoint3& p = pVertex[pTriList[j++]];
        const NiPoint3& q = pVertex[pTriList[j++]];
        const NiPoint3& r = pVertex[pTriList[j++]];

        // update bounding box extents
        NiPoint3 b;
        float x, y, z;
        
        b = p - center;
        x = basis[0]*b;
        y = basis[1]*b;
        z = basis[2]*b;
        if (x < lower.x) lower.x = x; else if (x > upper.x) upper.x = x;
        if (y < lower.y) lower.y = y; else if (y > upper.y) upper.y = y;
        if (z < lower.z) lower.z = z; else if (z > upper.z) upper.z = z;
        
        b = q - center;
        x = basis[0]*b;
        y = basis[1]*b;
        z = basis[2]*b;
        if (x < lower.x) lower.x = x; else if (x > upper.x) upper.x = x;
        if (y < lower.y) lower.y = y; else if (y > upper.y) upper.y = y;
        if (z < lower.z) lower.z = z; else if (z > upper.z) upper.z = z;
        
        b = r - center;
        x = basis[0]*b;
        y = basis[1]*b;
        z = basis[2]*b;
        if (x < lower.x) lower.x = x; else if (x > upper.x) upper.x = x;
        if (y < lower.y) lower.y = y; else if (y > upper.y) upper.y = y;
        if (z < lower.z) lower.z = z; else if (z > upper.z) upper.z = z;
    }

    NiPoint3 radius = 0.5f*(upper-lower);
    extent[0] = radius.x;
    extent[1] = radius.y;
    extent[2] = radius.z;
    
    NiPoint3 delta = 0.5f*(upper+lower);
    center += basis[0]*delta.x + basis[1]*delta.y + basis[2]*delta.z;

    // initialize world center and world basis
    NiTransform I;
    I.MakeIdentity();
    m_box.Transform(I);

    // need to have a pointer to velocity and delta time
    m_box.SetRoot(pRoot);
}
//---------------------------------------------------------------------------
NiMatrix3 NiOBBNode::ComputeCovarianceAndMean(
    const unsigned short* pTriList, const NiPoint3* pVertex, float* pArea, 
    NiPoint3* pCentroid, int i0, int i1, int* pISplit)
{
    // Center of triangles, area a^i, kPoint p^i, q^i, r^i =
    //     1/(3*Sum(a^i))Sum((p^i+q^i+r^i)*a^i)
    //
    // Covariance of triangles' element j with k =
    //     1/(24*Sum(a^i))*Sum(((p^i_j + q^i_j + r^i_j) *
    //    (p^i_k + q^i_k + r^i_k) + p^i_j * p^i_k + q^i_j * q^i_k +
    //     r^i_j * r^i_k) * a^i) - 1/2 * Center_j * Center_k

    NiPoint3 sum(0.0f, 0.0f, 0.0f);;
    float fTotArea = 0.0f;
    float pSumCofactors[6] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

    for (unsigned short i = (unsigned short)i0; i <= i1; i++) 
    {
        // get triangle and its attributes
        int usTriangle = pISplit[i];
        int j = 3*usTriangle;
        const NiPoint3& p = pVertex[pTriList[j++]];
        const NiPoint3& q = pVertex[pTriList[j++]];
        const NiPoint3& r = pVertex[pTriList[j++]];
        const NiPoint3& avr = pCentroid[usTriangle];
        float area = pArea[usTriangle];

        // update covariance entries
        fTotArea += area;
        sum += area*avr;
        pSumCofactors[0] += area*(avr.x*avr.x+p.x*p.x+q.x*q.x+r.x*r.x);
        pSumCofactors[1] += area*(avr.x*avr.y+p.x*p.y+q.x*q.y+r.x*r.y);
        pSumCofactors[2] += area*(avr.x*avr.z+p.x*p.z+q.x*q.z+r.x*r.z);
        pSumCofactors[3] += area*(avr.y*avr.y+p.y*p.y+q.y*q.y+r.y*r.y);
        pSumCofactors[4] += area*(avr.y*avr.z+p.y*p.z+q.y*q.z+r.y*r.z);
        pSumCofactors[5] += area*(avr.z*avr.z+p.z*p.z+q.z*q.z+r.z*r.z);
    }
    
    float fInverseTotalArea = 1.0f/fTotArea;
    float fScale = 0.25f*fInverseTotalArea;

    NiPoint3& center = m_box.GetCenter();
    center = fInverseTotalArea*sum;
    
    NiMatrix3 C;
    C.SetEntry(0, 0, pSumCofactors[0]*fScale-center.x*center.x);
    C.SetEntry(0, 1, pSumCofactors[1]*fScale-center.x*center.y);
    C.SetEntry(0, 2, pSumCofactors[2]*fScale-center.x*center.z);
    C.SetEntry(1, 1, pSumCofactors[3]*fScale-center.y*center.y);
    C.SetEntry(1, 2, pSumCofactors[4]*fScale-center.y*center.z);
    C.SetEntry(2, 2, pSumCofactors[5]*fScale-center.z*center.z);
    C.SetEntry(1, 0, C.GetEntry(0, 1));
    C.SetEntry(2, 0, C.GetEntry(0, 2));
    C.SetEntry(2, 1, C.GetEntry(1, 2));

    return C;
}
//---------------------------------------------------------------------------
void NiOBBNode::DivideTriangles(NiPoint3* pCentroid, int i0, int i1, 
    int* pISplit, int& j0, int& j1, int* pOSplit)
{
    int i;
    int usHalf = (i0+i1)/2;

    j0 = i0-1;
    j1 = i1+1;

    const NiPoint3& center = m_box.GetCenter();
    NiPoint3* basis = m_box.GetBasis();
    int iAxis;
    for (iAxis = 0; iAxis < 3 && (j0 < i0 || j1 > i1); iAxis++)
    {
        // try splitting along an axis
        j0 = i0-1;
        j1 = i1+1;

        for (i = i0; i <= i1; i++)
        {
            int k = pISplit[i];

            float d = basis[iAxis]*(pCentroid[k]-center);
            if (d > 0.0f)
            {
                pOSplit[++j0] = k;
            }
            else if (d < 0.0f)
            {
                pOSplit[--j1] = k;
            }
            else
            {
                // exactly on splitting plane, choose 50% one way, 
                // 50% other way
                if (i <= usHalf)
                {
                    pOSplit[++j0] = k;
                }
                else
                {
                    pOSplit[--j1] = k;
                }
            }
        }
    }

    if (iAxis == 3 && (j0 < i0 || j1 > i1))
    {
        // All 3 attempts to split along axes failed, so just split the
        // triangles manually.
        j0 = usHalf;
        j1 = usHalf+1;
        for (i = i0; i <= i1; i++)
        {
            pOSplit[i] = pISplit[i];
        }
    }
}
//---------------------------------------------------------------------------
void NiOBBNode::Transform(const NiTransform& xform)
{
    m_box.Transform(xform);
}
//---------------------------------------------------------------------------
int NiOBBNode::CollisionCallback(NiCollisionGroup::Intersect& intr)
{
    int ret0 = NiCollisionGroup::CONTINUE_COLLISIONS;
    int ret1 = NiCollisionGroup::CONTINUE_COLLISIONS;

    if (intr.pkRoot0)
    {
        NiCollisionData* pkData = NiGetCollisionData(intr.pkRoot0);

        // If no collision data on the root, let's try the object itself.
        if (!pkData)
            pkData = NiGetCollisionData(intr.pkObj0);

        if (pkData)
        {
            NiCollisionGroup::Callback cb0 = pkData->GetCollideCallback();
            
            if (cb0)
            {
                ret0 = cb0(intr);
            }
        }
    }

    if (intr.pkRoot1)
    {
        NiCollisionData* pkData = NiGetCollisionData(intr.pkRoot1);

        // If no collision data on the root, let's try the object itself.
        if (!pkData)
            pkData = NiGetCollisionData(intr.pkObj1);

        if (pkData)
        {
            NiCollisionGroup::Callback cb1 = pkData->GetCollideCallback();

            if (cb1)
            {
                // Reorder roots and normals so that callback can assume that
                // the caller of the callback is pkRoot0.
                NiAVObject* pSaveRoot = intr.pkRoot0;
                intr.pkRoot0 = intr.pkRoot1;
                intr.pkRoot1 = pSaveRoot;
                NiPoint3 saveNormal = intr.kNormal0;
                intr.kNormal0 = intr.kNormal1;
                intr.kNormal1 = saveNormal;

                ret1 = cb1(intr);
            }
        }
    }

    if (ret0 == NiCollisionGroup::CONTINUE_COLLISIONS
        && ret1 == NiCollisionGroup::CONTINUE_COLLISIONS)
    {
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }
    else if (ret0 == NiCollisionGroup::BREAKOUT_COLLISIONS ||
        ret1 == NiCollisionGroup::BREAKOUT_COLLISIONS)
    {
        return NiCollisionGroup::BREAKOUT_COLLISIONS;
    }
    else
    {
        return NiCollisionGroup::TERMINATE_COLLISIONS;
    }
}
//---------------------------------------------------------------------------
int NiOBBNode::BoxBoxCollision(NiOBBNode* pTestNode, NiAVObject* pkRoot0, 
    NiAVObject* pkRoot1, NiAVObject* pkObj0, NiAVObject* pkObj1,
    bool& bCollision)
{
    NiCollisionGroup::Intersect intr;
    intr.pkRoot0 = pkRoot0;
    intr.pkRoot1 = pkRoot1;
    intr.pkObj0 = pkObj0;
    intr.pkObj1 = pkObj1;

    NiPoint3 velocity0 = m_box.GetRoot()->GetVelocity();
    NiPoint3 velocity1 = pTestNode->m_box.GetRoot()->GetVelocity();

    int iIntersects = 0;

    if (velocity0 == NiPoint3::ZERO && velocity1 == NiPoint3::ZERO)
    {
        // static collision
        const NiPoint3& center0 = m_box.GetWorldCenter();
        const NiPoint3& center1 = pTestNode->m_box.GetWorldCenter();
        iIntersects = 1;
        intr.fTime = 0.0f;
        intr.kPoint = 0.5f*(center0+center1);
        velocity0 = intr.kPoint - center0;
        velocity1 = intr.kPoint - center1;
    }
    else
    {
        // dynamic collision
        iIntersects = m_box.FindIntersection(pTestNode->m_box, intr.fTime, 
            intr.kPoint);
    }

    if (iIntersects)
    {
        bCollision = true;
        m_box.ComputeNormal(intr.kPoint, intr.kNormal0);
        pTestNode->m_box.ComputeNormal(intr.kPoint, intr.kNormal1);
        return CollisionCallback(intr);
    }
    else
        return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
bool NiOBBNode::TestCollision(NiOBBNode* pTestNode, NiAVObject* pkObj0,
    NiAVObject* pkObj1, int iMaxDepth, int iTestMaxDepth,
    unsigned int uiChangeStamp0, unsigned int uiChangeStamp1)
{
    // limited construction of OBBs means some OBBNodes may be childless
    if (!pTestNode)
    {
        return false;
    }

    if (uiChangeStamp0 != m_uiChangeStamp)
    {
        m_box.Transform(pkObj0->GetWorldTransform());
        m_uiChangeStamp = uiChangeStamp0;
    }

    if (uiChangeStamp1 != pTestNode->GetChangeStamp())
    {
        pTestNode->TransformBox(pkObj1->GetWorldTransform());
        pTestNode->SetChangeStamp(uiChangeStamp1);
    }

    if (!m_box.TestIntersection(pTestNode->m_box))
    {
        return false;
    }

    if (iMaxDepth == 0 || iTestMaxDepth == 0)
    {
        return true;
    }

    if (IsLeaf())
    {
        if (pTestNode->IsLeaf() || !pTestNode->HasChildren())
        {
            return true;
        }
        else
        {
            int iTMDm1 = iTestMaxDepth-1;
            return
                TestCollision(pTestNode->m_pLeft, pkObj0, pkObj1, iMaxDepth,
                iTMDm1, uiChangeStamp0, uiChangeStamp1) ||
                TestCollision(pTestNode->m_pRight, pkObj0, pkObj1, iMaxDepth,
                iTMDm1, uiChangeStamp0, uiChangeStamp1);
        }
    }

    if (pTestNode->IsLeaf())
    {
        if (m_pLeft && m_pRight)
        {
            int iMDm1 = iMaxDepth-1;
            return
                m_pLeft->TestCollision(pTestNode, pkObj0,pkObj1, iMDm1,
                iTestMaxDepth, uiChangeStamp0, uiChangeStamp1) ||
                m_pRight->TestCollision(pTestNode, pkObj0,pkObj1, iMDm1, 
                iTestMaxDepth, uiChangeStamp0, uiChangeStamp1);
        }
    }
    else
    {
        if (m_pLeft && m_pRight)
        {
            int iMDm1 = iMaxDepth-1;
            int iTMDm1 = iTestMaxDepth-1;
            return m_pLeft->TestCollision(pTestNode->m_pLeft, pkObj0, pkObj1,
                iMDm1, iTMDm1, uiChangeStamp0, uiChangeStamp1)  ||
                m_pLeft->TestCollision(pTestNode->m_pRight, pkObj0, pkObj1,
                iMDm1, iTMDm1, uiChangeStamp0, uiChangeStamp1) ||
                m_pRight->TestCollision(pTestNode->m_pLeft, pkObj0, pkObj1, 
                iMDm1, iTMDm1,uiChangeStamp0, uiChangeStamp1) ||
                m_pRight->TestCollision(pTestNode->m_pRight, pkObj0, pkObj1,
                iMDm1, iTMDm1, uiChangeStamp0, uiChangeStamp1);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
int NiOBBNode::FindCollisions(NiOBBNode* pTestNode, NiAVObject* pkRoot0, 
    NiAVObject* pkRoot1, NiAVObject* pkObj0, NiAVObject* pkObj1, 
    int iMaxDepth, int iTestMaxDepth, unsigned int uiChangeStamp0, 
    unsigned int uiChangeStamp1, bool& bCollision)
{
    // limited construction of OBBs means some OBBNodes may be childless
    if (!pTestNode)
    {
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    if (uiChangeStamp0 != m_uiChangeStamp)
    {
        m_box.Transform(pkObj0->GetWorldTransform());
        m_uiChangeStamp = uiChangeStamp0;
    }

    if (uiChangeStamp1 != pTestNode->GetChangeStamp())
    {
        pTestNode->TransformBox(pkObj1->GetWorldTransform());
        pTestNode->SetChangeStamp(uiChangeStamp1);
    }

    if (!m_box.TestIntersection(pTestNode->m_box))
    {
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    if (iMaxDepth == 0 || iTestMaxDepth == 0)
    {
        return BoxBoxCollision(pTestNode, pkRoot0, pkRoot1, pkObj0, pkObj1,
            bCollision);
    }

    int ret;

    if (IsLeaf())
    {
        if (pTestNode->IsLeaf() || !pTestNode->HasChildren())
        {
            ret = BoxBoxCollision(pTestNode, pkRoot0, pkRoot1, pkObj0,
                pkObj1, bCollision);
            if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return ret;
            }
        }
        else
        {
            int iTMDm1 = iTestMaxDepth-1;
            ret = FindCollisions(pTestNode->m_pLeft, pkRoot0, pkRoot1,
                pkObj0, pkObj1, iMaxDepth, iTMDm1, uiChangeStamp0, 
                uiChangeStamp1, bCollision);

            if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return ret;
            }

            ret = FindCollisions(pTestNode->m_pRight, pkRoot0, pkRoot1, 
                pkObj0, pkObj1, iMaxDepth, iTMDm1, uiChangeStamp0,
                uiChangeStamp1, bCollision);
            if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return ret;
            }
        }
    }
    else
    {
        if (pTestNode->IsLeaf())
        {
            if (m_pLeft && m_pRight)
            {
                int iMDm1 = iMaxDepth-1;
                ret = m_pLeft->FindCollisions(pTestNode, pkRoot0, pkRoot1, 
                    pkObj0, pkObj1, iMDm1, iTestMaxDepth, 
                    uiChangeStamp0, uiChangeStamp1, bCollision);
                if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
                {
                    return ret;
                }

                ret = m_pRight->FindCollisions(pTestNode, pkRoot0, pkRoot1, 
                    pkObj0, pkObj1, iMDm1, iTestMaxDepth, uiChangeStamp0,
                    uiChangeStamp1, bCollision);
                if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
                {
                    return ret;
                }
            }
            else
            {
                return BoxBoxCollision(pTestNode, pkRoot0, pkRoot1, pkObj0, 
                    pkObj1, bCollision);
            }
        }
        else
        {
            // Descend all the way to leaf of collidee(first guy)
            // before starting down the collider(second guy).
            int iMDm1 = iMaxDepth-1;
            if (m_pLeft && m_pRight) 
            {
                ret = m_pLeft->FindCollisions(pTestNode, pkRoot0, pkRoot1, 
                    pkObj0, pkObj1, iMDm1, iTestMaxDepth, uiChangeStamp0, 
                    uiChangeStamp1, bCollision);
                if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
                {
                    return ret;
                }

                ret = m_pRight->FindCollisions(pTestNode, pkRoot0, pkRoot1, 
                    pkObj0, pkObj1, iMDm1, iTestMaxDepth, uiChangeStamp0,
                    uiChangeStamp1, bCollision);
                if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
                {
                    return ret;
                }
            }
            else
            {
                return BoxBoxCollision(pTestNode, pkRoot0, pkRoot1, pkObj0,
                    pkObj1, bCollision);
            }
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int NiOBBNode::FindIntersections(const NiPoint3& origin, 
    const NiPoint3& dir, NiPick& pick, bool& bIntersects, NiAVObject* pObj, 
    unsigned int uiChangeStamp)
{
    if (uiChangeStamp != m_uiChangeStamp)
    {
        m_box.Transform(pObj->GetWorldTransform());
        m_uiChangeStamp = uiChangeStamp;
    }

    if (!m_box.TestIntersection(origin, dir))
    {
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    int ret;

    if (m_pLeft)
    {
        ret = m_pLeft->FindIntersections(origin, dir, 
            pick, bIntersects, pObj, uiChangeStamp);
        if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return ret;
        }
    }
    if (m_pRight)
    {
        ret = m_pRight->FindIntersections(origin, dir, 
            pick, bIntersects, pObj, uiChangeStamp);
        if (ret >= NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return ret;
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
