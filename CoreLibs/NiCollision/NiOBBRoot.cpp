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

#include "NiOBBRoot.h"
#include "NiOBBLeaf.h"
#include "NiOBBNode.h"

//---------------------------------------------------------------------------
NiOBBRoot::NiOBBRoot(unsigned short usTriangles,
    const unsigned short* pTriList, const NiPoint3* pVertex,
    const NiPoint3* pWVertex, int iBinSize)
{
    NIASSERT( iBinSize >= 1 );

    if ( usTriangles > 1 )
    {
        m_pTree = NiNew NiOBBNode(this,usTriangles,pTriList,pVertex,pWVertex,
            iBinSize);
    }
    else
    {
        m_pTree = NiNew NiOBBLeaf(this,pTriList,pVertex,pWVertex,0);
    }
    NIASSERT( m_pTree );

    m_fDeltaTime = 0.0f;
    m_velocity = NiPoint3::ZERO;
    m_xform.MakeIdentity();
    m_bTransformChanged = false;
    m_uiChangeStamp = 0;
}
//---------------------------------------------------------------------------
NiOBBRoot::~NiOBBRoot ()
{
    NiDelete m_pTree;
}
//---------------------------------------------------------------------------
void NiOBBRoot::Transform(const NiTransform& xform)
{
    if ( m_xform != xform )
    {       
        m_xform = xform;
        m_bTransformChanged = true;
        m_uiChangeStamp++;
    }
}
//---------------------------------------------------------------------------
bool NiOBBRoot::TestCollision(NiOBBRoot* pTestRoot, NiAVObject* pThisObj, 
    NiAVObject* pOtherObj, int iMaxDepth, int iTestMaxDepth)
{
    if ( this == pTestRoot )
        return false;

    return m_pTree->TestCollision(pTestRoot->m_pTree, pThisObj, pOtherObj,
        iMaxDepth, iTestMaxDepth, m_uiChangeStamp,
        pTestRoot->GetChangeStamp());
}
//---------------------------------------------------------------------------
int NiOBBRoot::FindCollisions(NiOBBRoot* pTestRoot, NiAVObject* pThisRoot,
    NiAVObject* pOtherRoot, NiAVObject* pThisObj, NiAVObject* pOtherObj,
    int iMaxDepth, int iTestMaxDepth, bool& bCollision)
{
    if ( this == pTestRoot )
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    return m_pTree->FindCollisions(pTestRoot->m_pTree, pThisRoot, pOtherRoot,
        pThisObj, pOtherObj, iMaxDepth, iTestMaxDepth, m_uiChangeStamp,
        pTestRoot->GetChangeStamp(), bCollision);
}
//---------------------------------------------------------------------------
int NiOBBRoot::FindIntersections(const NiPoint3& origin, 
    const NiPoint3& dir, NiPick& pick, bool& bIntersects, NiAVObject* pObj,
    unsigned int uiChangeStamp)
{
    return m_pTree->FindIntersections(origin, dir, pick, bIntersects, pObj,
        uiChangeStamp);
}
//---------------------------------------------------------------------------
