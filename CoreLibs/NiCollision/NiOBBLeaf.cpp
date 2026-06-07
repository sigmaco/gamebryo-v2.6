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
#include "NiCollisionUtils.h"
#include "NiOBBRoot.h"
#include "NiOBBLeaf.h"
#include <NiMatrix3.h>
#include <NiPoint3.h>

//---------------------------------------------------------------------------
NiOBBLeaf::NiOBBLeaf(NiOBBRoot* pRoot, const unsigned short* pTriList,
    const NiPoint3* pVertex, const NiPoint3* pWVertex,
    unsigned short usTriangle)
{
    m_pLeft = 0;
    m_pRight = 0;

    // Save vertex pointers for use in FindIntersection.  Use model
    // vertices to build bounding box.
    NIASSERT( pWVertex );
    unsigned short usStart = 3*usTriangle;

    unsigned short usIndex = pTriList[usStart];
    m_pWorldVertex[0] = &pWVertex[usIndex];
    const NiPoint3& v0 = pVertex[usIndex];
    usStart++;

    usIndex = pTriList[usStart];
    m_pWorldVertex[1] = &pWVertex[usIndex];
    const NiPoint3& v1 = pVertex[usIndex];
    usStart++;

    usIndex = pTriList[usStart];
    m_pWorldVertex[2] = &pWVertex[usIndex];
    const NiPoint3& v2 = pVertex[usIndex];

    // access to box attributes
    NiPoint3& center = m_box.GetCenter();
    NiPoint3* basis = m_box.GetBasis();
    float* extent = m_box.GetExtent();

    // box center is centroid of triangle
    center = (v0+v1+v2)/3.0f;

    // first box axis is unitized edge v1-v0
    basis[0] = v1-v0;
    basis[0].Unitize();

    // second box axis is unitized projection of v2-v0 onto basis[0]
    basis[1] = v2-v0;
    float proj = basis[0]*basis[1];
    basis[1] -= proj*basis[0];
    basis[1].Unitize();

    // third box axis is the cross product of basis[0] and basis[1]
    basis[2] = basis[0].Cross(basis[1]);

    // find the bounding box extents
    NiPoint3 lower(0.0f,0.0f,0.0f);
    NiPoint3 upper(0.0f,0.0f,0.0f);

    NiPoint3 b;
    float x, y, z;
    
    b = v0 - center;
    x = basis[0]*b;
    y = basis[1]*b;
    z = basis[2]*b;
    if ( x < lower.x ) lower.x = x; else if ( x > upper.x ) upper.x = x;
    if ( y < lower.y ) lower.y = y; else if ( y > upper.y ) upper.y = y;
    if ( z < lower.z ) lower.z = z; else if ( z > upper.z ) upper.z = z;
    
    b = v1 - center;
    x = basis[0]*b;
    y = basis[1]*b;
    z = basis[2]*b;
    if ( x < lower.x ) lower.x = x; else if ( x > upper.x ) upper.x = x;
    if ( y < lower.y ) lower.y = y; else if ( y > upper.y ) upper.y = y;
    if ( z < lower.z ) lower.z = z; else if ( z > upper.z ) upper.z = z;
    
    b = v2 - center;
    x = basis[0]*b;
    y = basis[1]*b;
    z = basis[2]*b;
    if ( x < lower.x ) lower.x = x; else if ( x > upper.x ) upper.x = x;
    if ( y < lower.y ) lower.y = y; else if ( y > upper.y ) upper.y = y;
    if ( z < lower.z ) lower.z = z; else if ( z > upper.z ) upper.z = z;

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
void NiOBBLeaf::ComputeBarycenter(const NiPoint3& v0, const NiPoint3& v1,
    const NiPoint3& v2, const NiPoint3& p, float pCenter[3])
{
    NiPoint3 e0 = v0-v2, e1 = v1-v2, e2 = p-v2;
    // assert:  det[e0,e1,e2] = 0

    float fM00 = e0*e0, fM01 = e0*e1, fM11 = e1*e1;
    float fR0 = e2*e0, fR1 = e2*e1;
    float fInvdet = 1.0f/(fM00*fM11-fM01*fM01);

    pCenter[0] = fInvdet*(fM11*fR0-fM01*fR1);
    pCenter[1] = fInvdet*(fM00*fR1-fM01*fR0);
    pCenter[2] = 1-pCenter[0]-pCenter[1];
}
//---------------------------------------------------------------------------
int NiOBBLeaf::TentativeCollision(float fDT, const NiPoint3& N, float fC,
    const NiPoint3& V, const NiPoint3& q0, const NiPoint3& q1, float& fT0,
    NiPoint3& p0, float& fT1, NiPoint3& p1)
{
    const float fMinTime = 0.0f;
    const float fMaxTime = fDT;
    const float fDelTime = fMaxTime-fMinTime;

    float fC00 = N*(q0+fMinTime*V)-fC;
    float fC10 = N*(q1+fMinTime*V)-fC;
    float fC01 = N*(q0+fMaxTime*V)-fC;
    float fC11 = N*(q1+fMaxTime*V)-fC;

    float fS, fT;
    NiPoint3 p;

    int iFirstFound = 0;

    if ( fC00*fC10 <= 0.0f )
    {
        if ( fC00 == 0.0f && fC10 == 0.0f )
        {
            fT0 = fMinTime;
            fT1 = fMinTime;
            p0 = q0+fT0*V;
            p1 = q1+fT1*V;
            return 1;
        }
        fT0 = fMinTime;
        fS  = fC00/(fC00-fC10);
        p0 = (1.0f-fS)*q0+fS*q1+fT0*V;
        iFirstFound = 1;
    }

    if ( fC00*fC01 <= 0.0f )
    {
        if ( fC00 == 0.0f && fC01 == 0.0f )
        {
            fT0 = fMinTime;
            fT1 = fMaxTime;
            p0 = q0+fT0*V;
            p1 = q0+fT1*V;
            return 1;
        }
        fT = fMinTime+fDelTime*fC00/(fC00-fC01);
        p = q0+fT*V;
        if ( iFirstFound )
        {
            fT1 = fT;
            p1 = p;
            if ( fT0 > fT1 )
            {
                fT = fT0;
                fT0 = fT1;
                fT1 = fT;
                p = p0;
                p0 = p1;
                p1 = p;
            }
            return 1;
        }
        fT0 = fT;
        p0 = p;
        iFirstFound = 1;
    }

    if ( fC10*fC11 <= 0.0f )
    {
        if ( fC10 == 0.0f && fC11 == 0.0f )
        {
            fT0 = fMinTime;
            fT1 = fMaxTime;
            p0 = q1+fT0*V;
            p1 = q1+fT1*V;
            return 1;
        }
        fT = fMinTime+fDelTime*fC10/(fC10-fC11);
        p = q1+fT*V;
        if ( iFirstFound )
        {
            fT1 = fT;
            p1 = p;
            if ( fT0 > fT1 )
            {
                fT = fT0;
                fT0 = fT1;
                fT1 = fT;
                p = p0;
                p0 = p1;
                p1 = p;
            }
            return 1;
        }
        fT0 = fT;
        p0 = p;
        iFirstFound = 1;
    }

    if ( fC01*fC11 <= 0.0f )
    {
        if ( fC01 == 0.0f && fC11 == 0.0f )
        {
            fT0 = fMaxTime;
            fT1 = fMaxTime;
            p0 = q0+fT0*V;
            p1 = q1+fT1*V;
            return 1;
        }
        fT = fMaxTime;
        fS = fC01/(fC01-fC11);
        p = (1.0f-fS)*q0+fS*q1+fMaxTime*V;
        if ( iFirstFound )
        {
            fT1 = fT;
            p1 = p;
            if ( fT0 > fT1 )
            {
                fT = fT0;
                fT0 = fT1;
                fT1 = fT;
                p = p0;
                p0 = p1;
                p1 = p;
            }
            return 1;
        }
        fT0 = fT;
        p0 = p;
        iFirstFound = 1;
    }

    if ( iFirstFound )
    {
        fT1 = fT0;
        p1 = p0;
        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int NiOBBLeaf::TestEdgeTri(const NiPoint3& edge0, const NiPoint3& edge1,
    const NiPoint3* pVer[3], const NiPoint3& N, float fC, const NiPoint3& V,
    float fDT, float& fTMin, NiPoint3& pmin)
{
    float fT0, fT1;
    NiPoint3 p0, p1;

    if ( TentativeCollision(fDT,N,fC,V,edge0,edge1,fT0,p0,fT1,p1))
    {
        // clip time-projected edge against triangle

        float pBary0[3], pBary1[3];
        ComputeBarycenter(*pVer[0],*pVer[1],*pVer[2],p0,pBary0);
        ComputeBarycenter(*pVer[0],*pVer[1],*pVer[2],p1,pBary1);

        float fS, fT, fA0, fA1, fA2;

        if ( pBary0[0] < 0.0f )
        {
            if ( pBary1[0] < 0.0f )
                return 0;

            fS = pBary0[0]/(pBary0[0]-pBary1[0]);
            fA1 = (1-fS)*pBary0[1]+fS*pBary1[1];
            fA2 = (1-fS)*pBary0[2]+fS*pBary1[2];
            if ( fA1 >= 0.0f && fA2 >= 0.0f )
            {
                fT = fT0*(1-fS)+fT1*fS;
                if ( fT < fTMin )
                {
                    fTMin = fT;
                    pmin = p0*(1-fS)+p1*fS;
                }
                return 1;
            }
            else
                return 0;
        }

        if ( pBary0[1] < 0.0f )
        {
            if ( pBary1[1] < 0.0f )
                return 0;

            fS = pBary0[1]/(pBary0[1]-pBary1[1]);
            fA0 = (1-fS)*pBary0[0]+fS*pBary1[0];
            fA2 = (1-fS)*pBary0[2]+fS*pBary1[2];
            if ( fA0 >= 0.0f && fA2 >= 0.0f )
            {
                fT = fT0*(1-fS)+fT1*fS;
                if ( fT < fTMin )
                {
                    fTMin = fT;
                    pmin = p0*(1-fS)+p1*fS;
                }
                return 1;
            }
            else
                return 0;
        }

        if ( pBary0[2] < 0.0f )
        {
            if ( pBary1[2] < 0.0f )
                return 0;

            fS = pBary0[2]/(pBary0[2]-pBary1[2]);
            fA0 = (1-fS)*pBary0[0]+fS*pBary1[0];
            fA1 = (1-fS)*pBary0[1]+fS*pBary1[1];
            if ( fA0 >= 0.0f && fA1 >= 0.0f )
            {
                fT = fT0*(1-fS)+fT1*fS;
                if ( fT < fTMin )
                {
                    fTMin = fT;
                    pmin = p0*(1-fS)+p1*fS;
                }
                return 1;
            }
            else
                return 0;
        }

        if ( fT0 < fTMin )
        {
            fTMin = fT0;
            pmin = p0;
        }

        return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int NiOBBLeaf::FindIntersection(const NiPoint3* pVer0[3],
    const NiPoint3* pVer1[3], const NiPoint3& V, float fDT, float& fTMin,
    NiPoint3& pmin)
{
    NiPoint3 edge0 = (*pVer1[1])-(*pVer1[0]);
    NiPoint3 edge1 = (*pVer1[2])-(*pVer1[0]);
    NiPoint3 N = edge0.Cross(edge1);
    float fC = N*(*pVer1[0]);

    int iResult = 0;

    iResult |= TestEdgeTri(*pVer0[0],*pVer0[1],pVer1,N,fC,V,fDT,fTMin,pmin);
    iResult |= TestEdgeTri(*pVer0[0],*pVer0[2],pVer1,N,fC,V,fDT,fTMin,pmin);
    iResult |= TestEdgeTri(*pVer0[1],*pVer0[2],pVer1,N,fC,V,fDT,fTMin,pmin);

    return iResult;
}
//---------------------------------------------------------------------------
int NiOBBLeaf::FindIntersection(NiOBBLeaf* pLeaf0, NiOBBLeaf* pLeaf1,
    float& fT, NiPoint3& p)
{
    float fDT = pLeaf0->m_box.GetRoot()->GetDeltaTime();
    const NiPoint3& V0 = pLeaf0->m_box.GetRoot()->GetVelocity();
    const NiPoint3& V1 = pLeaf1->m_box.GetRoot()->GetVelocity();
    NiPoint3 V;

    float fT0 = FLT_MAX;
    NiPoint3 p0;
    V = V0-V1;
    int iRes0 = FindIntersection(pLeaf0->m_pWorldVertex,
        pLeaf1->m_pWorldVertex,V,fDT,fT0,p0);

    float fT1 = FLT_MAX;
    NiPoint3 p1;
    V = V1-V0;
    int iRes1 = FindIntersection(pLeaf1->m_pWorldVertex,
        pLeaf0->m_pWorldVertex,V,fDT,fT1,p1);

    if ( iRes0 )
    {
        if ( iRes1 )
        {
            if ( fT0 <= fT1 )
            {
                fT = fT0;
                p = p0+fT0*V1;
            }
            else
            {
                fT = fT1;
                p = p1+fT1*V0;
            }
        }
        else
        {
            fT = fT0;
            p = p0+fT0*V1;
        }
    }
    else
    {
        if ( iRes1 )
        {
            fT = fT1;
            p = p1+fT1*V0;
        }
    }

    return iRes0 || iRes1;
}
//---------------------------------------------------------------------------
int NiOBBLeaf::FaceFaceCollision(NiOBBLeaf* pTestLeaf, NiAVObject* pkRoot0,
    NiAVObject* pkRoot1, NiAVObject* pkObj0, NiAVObject* pkObj1, 
    bool& bCollision)
{
    NiCollisionGroup::Intersect intr;
    intr.pkRoot0 = pkRoot0;
    intr.pkRoot1 = pkRoot1;
    intr.pkObj0 = pkObj0;
    intr.pkObj1 = pkObj1;

    // all collisions tested as dynamic collision
    int iIntersects = FindIntersection(this,pTestLeaf,intr.fTime,
        intr.kPoint);

    if ( iIntersects )
    {
        bCollision = true;

        intr.appkTri1 = (const NiPoint3**)&(m_pWorldVertex[0]);
        intr.appkTri2 = (const NiPoint3**)&(pTestLeaf->m_pWorldVertex[0]);

        m_box.ComputeNormal(intr.kPoint,intr.kNormal0);
        pTestLeaf->m_box.ComputeNormal(intr.kPoint,intr.kNormal1);
        return CollisionCallback(intr);
    }
    else
        return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
bool NiOBBLeaf::TestCollision(NiOBBNode* pTestNode, NiAVObject* pkObj0,
    NiAVObject* pkObj1, int iMaxDepth, int iTestMaxDepth, 
    unsigned int uiChangeStamp0, unsigned int uiChangeStamp1)
{
    if ( uiChangeStamp0 != m_uiChangeStamp )
    {
        m_box.Transform(pkObj0->GetWorldTransform());
        m_uiChangeStamp = uiChangeStamp0;
    }

    if ( uiChangeStamp1 != pTestNode->GetChangeStamp())
    {
        pTestNode->TransformBox(pkObj1->GetWorldTransform());
        pTestNode->SetChangeStamp(uiChangeStamp1);
    }

    if (!m_box.TestIntersection(pTestNode->GetBox()))
        return false;
  
    if (!pTestNode->IsLeaf())
    {
        if ( iTestMaxDepth == 0 || !pTestNode->HasChildren())
            return true;
        else
        {
            int iTMDm1 = iTestMaxDepth-1;

            return TestCollision(pTestNode->GetLeft(), pkObj0, pkObj1,
                    iMaxDepth,iTMDm1, uiChangeStamp0, uiChangeStamp1)
                || TestCollision(pTestNode->GetRight(), pkObj0, pkObj1,
                    iMaxDepth,iTMDm1, uiChangeStamp0, uiChangeStamp1);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
int NiOBBLeaf::FindCollisions(NiOBBNode* pTestNode, NiAVObject* pkRoot0,
    NiAVObject* pkRoot1, NiAVObject* pkObj0, NiAVObject* pkObj1,
    int iMaxDepth, int iTestMaxDepth, unsigned int uiChangeStamp0,
    unsigned int uiChangeStamp1, bool& bCollision)
{
    if ( uiChangeStamp0 != m_uiChangeStamp )
    {
        m_box.Transform(pkObj0->GetWorldTransform());
        m_uiChangeStamp = uiChangeStamp0;
    }

    if ( uiChangeStamp1 != pTestNode->GetChangeStamp())
    {
        pTestNode->TransformBox(pkObj1->GetWorldTransform());
        pTestNode->SetChangeStamp(uiChangeStamp1);
    }

    if (!m_box.TestIntersection(pTestNode->GetBox()))
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    if (!pTestNode->IsLeaf())
    {
        if ( iTestMaxDepth == 0 || !pTestNode->HasChildren())
        {
            return BoxBoxCollision(pTestNode,pkRoot0,pkRoot1,pkObj0,pkObj1,
                bCollision);
        }
        else
        {
            int ret = FindCollisions(pTestNode->GetLeft(),pkRoot0,pkRoot1,
                pkObj0,pkObj1,iMaxDepth,iTestMaxDepth-1,
                uiChangeStamp0, uiChangeStamp1, bCollision);
            if ( ret >= NiCollisionGroup::TERMINATE_COLLISIONS )
                return ret;

            ret = FindCollisions(pTestNode->GetRight(),pkRoot0,pkRoot1,
                pkObj0,pkObj1,iMaxDepth,iTestMaxDepth-1,
                uiChangeStamp0, uiChangeStamp1, bCollision);
            if ( ret >= NiCollisionGroup::TERMINATE_COLLISIONS )
                return ret;
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    if ( iMaxDepth >= 0 || iTestMaxDepth >= 0 )
        return BoxBoxCollision(pTestNode,pkRoot0,pkRoot1,pkObj0,pkObj1,
            bCollision);

    return FaceFaceCollision((NiOBBLeaf*)pTestNode,pkRoot0,pkRoot1,pkObj0,
        pkObj1, bCollision);
}
//---------------------------------------------------------------------------
int NiOBBLeaf::FindIntersections(const NiPoint3& origin,
    const NiPoint3& dir, NiPick& pick, bool& bIntersects, NiAVObject* pObj,
    unsigned int uiChangeStamp)
{

    if ( uiChangeStamp != m_uiChangeStamp )
    {
        m_box.Transform(pObj->GetWorldTransform());
        m_uiChangeStamp = uiChangeStamp;
    }

    NiPoint3 intersect;
    float fLineParam = 0.0f;
    float fTriParam1, fTriParam2;
    
    bIntersects = NiCollisionUtils::IntersectTriangle(origin, dir,
        *m_pWorldVertex[0], *m_pWorldVertex[1], *m_pWorldVertex[2],
        pick.GetFrontOnly(), intersect, fLineParam, fTriParam1, fTriParam2);

    if ( bIntersects )
    {
        NiPick::Record* pRecord = pick.Add(pObj);
        pRecord->SetIntersection(intersect);
        pRecord->SetDistance(fLineParam);

        if ( pick.GetReturnNormal())
        {
            NiPoint3 v0 = *m_pWorldVertex[1] - *m_pWorldVertex[0];
            NiPoint3 v1 = *m_pWorldVertex[2] - *m_pWorldVertex[0];
            NiPoint3 normal = v0.Cross(v1);
            normal.Unitize();
            pRecord->SetNormal(normal);
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------

