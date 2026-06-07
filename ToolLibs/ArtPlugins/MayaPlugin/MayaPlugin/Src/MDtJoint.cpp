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

// MDtJoint.cpp

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "MDtJoint.h"



// Declare a global Joint Manager.
kMDtJointXformManager gJointXformMan;

//---------------------------------------------------------------------------
kMDtJointInfo::kMDtJointInfo(void)
{
    m_iJointIndex           = -1;
}
//---------------------------------------------------------------------------
kMDtJointInfo::~kMDtJointInfo(void)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
kMDtJointInfoManager::kMDtJointInfoManager(void)
{
    m_iJoints = 0;
    m_iAllocated = 0;
    m_ppJoints = NULL;
}
//---------------------------------------------------------------------------
kMDtJointInfoManager::~kMDtJointInfoManager(void)
{
    // Delete the joint info
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < (unsigned int)m_iJoints; uiLoop++)
        NiExternalDelete m_ppJoints[uiLoop];

    NiFree(m_ppJoints);
}
//---------------------------------------------------------------------------
int kMDtJointInfoManager::Add( kMDtJointInfo *pJ )
{
    if (m_iJoints >= m_iAllocated)
        DoubleSpace();

    int joint = m_iJoints;
    m_ppJoints[m_iJoints++] = pJ; 

    return (joint); // return the slot where added..
}
//---------------------------------------------------------------------------
kMDtJointInfo* kMDtJointInfoManager::Get( int iIndex )
{
    return m_ppJoints[iIndex];
}
//---------------------------------------------------------------------------
void kMDtJointInfoManager::DoubleSpace(void)
{
    // Double the existing Space.

    // if no space allocated - go ahead and allocate some...
    if (m_iAllocated < 1)
    {
        m_ppJoints = NiAlloc(kMDtJointInfo*, 128);
        m_iAllocated = 128;
        memset( m_ppJoints, 0, sizeof(kMDtJointInfo*)*128);
        return;
    }

    m_iAllocated *= 2;
    
    // Allocate the new space...
    kMDtJointInfo **ppNewSpace = NiAlloc(kMDtJointInfo*, m_iAllocated);

    // Copy the old into the newspace.
    memset(ppNewSpace,0,sizeof(kMDtJointInfo*)*m_iAllocated);
    for (int i=0; i<m_iJoints;i++)
        ppNewSpace[i] = m_ppJoints[i];

    NiFree(m_ppJoints);

    m_ppJoints = ppNewSpace;
}
//---------------------------------------------------------------------------
void kMDtJointInfoManager::GetAllShapeJoints( int iShapeNum, int *iNumJoints,
    int **piJoints )
{
    NI_UNUSED_ARG(iShapeNum);
    *iNumJoints = 0;

    if (m_iJoints <= 0)
        return;

    *iNumJoints = m_iJoints;
    *piJoints = NiAlloc(int, m_iJoints);

    int ifound=0;
    for (int i=0;i<m_iJoints;i++)
    {
        kMDtJointInfo *pCurr = Get(i);
        NIASSERT(pCurr);
        if (pCurr)
            (*piJoints)[ifound++] = pCurr->m_iJointIndex; //i; 
    }
}
//---------------------------------------------------------------------------
void kMDtJointInfoManager::GetSkinToBoneMat( int iJoint, float mat[4][4])
{
    kMDtJointInfo *pCurr = NULL;

    int i;
    for(i=0; i< m_iJoints;i++)
    {
        pCurr = Get(i);

        if (pCurr->m_iJointIndex == iJoint)
            break;
    }

    NIASSERT(pCurr);

    for (i=0;i<4;i++)
        for (int j=0;j<4;j++)
            mat[i][j] = pCurr->m_fSkinToBoneMat[i][j];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
kMDtJointXformManager::kMDtJointXformManager(void)
{
    m_ppJointXforms     = NULL;
    m_iJointXforms      = 0;
    m_iAllocated    = 0;
}
//---------------------------------------------------------------------------
kMDtJointXformManager::~kMDtJointXformManager(void)
{
    Reset();
}
//---------------------------------------------------------------------------
void kMDtJointXformManager::Reset(void)
{
    for(int i=0; i<m_iJointXforms;i++)
    {
        NiExternalDelete m_ppJointXforms[i];
        m_ppJointXforms[i] = NULL;
    }

    m_iJointXforms   = 0;
    m_iAllocated = 0;

    if (m_ppJointXforms)
        NiFree(m_ppJointXforms);

    m_ppJointXforms = NULL;
}
//---------------------------------------------------------------------------
void kMDtJointXformManager::DoubleSpace(void)
{
    // Double the existing Space.

    // if no space allocated - go ahead and allocate some...
    if (m_iAllocated < 1)
    {
        m_ppJointXforms = NiAlloc(kMDtJointXform*, 128);
        m_iAllocated = 128;
        memset( m_ppJointXforms, 0, sizeof(kMDtJointXform*)*128);
        return;
    }

    m_iAllocated *= 2;
    
    // Allocate the new space...
    kMDtJointXform **ppNewSpace = NiAlloc(kMDtJointXform*, m_iAllocated);

    // Copy the old into the newspace.
    memset(ppNewSpace,0,sizeof(kMDtJointXform*)*m_iAllocated);
    for (int i=0; i<m_iJointXforms;i++)
        ppNewSpace[i] = m_ppJointXforms[i];

    NiFree(m_ppJointXforms);

    m_ppJointXforms = ppNewSpace;
}
//---------------------------------------------------------------------------
int kMDtJointXformManager::Add( kMDtJointXform *pJ )
{
    if (m_iJointXforms >= m_iAllocated)
        DoubleSpace();

    int jointxform = m_iJointXforms;
    m_ppJointXforms[m_iJointXforms++] = pJ; 

    return (jointxform); // return the slot where added..
}
//---------------------------------------------------------------------------
kMDtJointXform* kMDtJointXformManager::Get( int iIndex )
{
    return m_ppJointXforms[iIndex];
}
//---------------------------------------------------------------------------
MObject& kMDtJointXformManager::GetJointXformNode(int iJoint)
{
    return m_ppJointXforms[iJoint]->m_MObjectJoint;
}
//---------------------------------------------------------------------------
int kMDtJointXformManager::GetJoint( MObject mobj )
{
    if (m_ppJointXforms == NULL)
        return -1;

    for (int i=0; i<m_iJointXforms; i++)
    {
        if (m_ppJointXforms[i]->m_MObjectJoint == mobj)
            return i;
    }
    
    return -1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// C WRAPPERS
//---------------------------------------------------------------------------
MObject& gJointGetJointNode( int iJoint )
{
    return gJointXformMan.GetJointXformNode( iJoint );
}
