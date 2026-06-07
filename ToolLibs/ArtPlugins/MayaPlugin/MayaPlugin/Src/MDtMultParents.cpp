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

// MDtMultParents.cpp
// The purpose of this module is to extend the functionality of MDt
// to cope with objects that have more than one parent.

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "MDtMultParents.h"


// Declare a global Joint Manager.
kMDtMultParentManager gMultParentMan;

//---------------------------------------------------------------------------
kMDtMultParentManager::kMDtMultParentManager(void)
{
    m_ppMultParentInfo = NULL;
    m_iUsed = 0;
    m_iAllocated = 0;
}
//---------------------------------------------------------------------------
kMDtMultParentManager::~kMDtMultParentManager(void)
{
    Reset();
}
//---------------------------------------------------------------------------
void kMDtMultParentManager::Reset(void)
{
    for(int i=0; i<m_iUsed;i++)
    {
        NiExternalDelete m_ppMultParentInfo[i];
        m_ppMultParentInfo[i] = NULL;
    }

    m_iUsed = 0;
    m_iAllocated = 0;

    NiFree(m_ppMultParentInfo);

    m_ppMultParentInfo = NULL;
}
//---------------------------------------------------------------------------
void kMDtMultParentManager::DoubleSpace(void)
{
    // Double the existing Space.

    // if no space allocated - go ahead and allocate some...
    if (m_iAllocated < 1)
    {
        m_ppMultParentInfo = NiAlloc(kMDtMultParentInfo*, 32);
        m_iAllocated = 32;
        memset( m_ppMultParentInfo, 0, sizeof(kMDtMultParentInfo*)*32);
        return;
    }

    m_iAllocated *= 2;
    
    // Allocate the new space...
    kMDtMultParentInfo **ppNewSpace = NiAlloc(kMDtMultParentInfo* , 
        m_iAllocated);

    // Copy the old into the newspace.
    memset(ppNewSpace,0,sizeof(kMDtMultParentInfo*)*m_iAllocated);
    for (int i=0; i<m_iUsed;i++)
        ppNewSpace[i] = m_ppMultParentInfo[i];

    NiFree(m_ppMultParentInfo);

    m_ppMultParentInfo = ppNewSpace;
}
//---------------------------------------------------------------------------
int kMDtMultParentManager::Add( MObject &mObj, int iParentCnt )
{
    // First, check to see if this object is already in the list
    for (int i=0; i<m_iUsed; i++)
    {
        if (m_ppMultParentInfo[i]->m_Node == mObj)
        {
            // It is.  So just bump up the encounter count
            m_ppMultParentInfo[i]->iCurrent++;
            return m_ppMultParentInfo[i]->iCurrent;
        }
    }

    if (m_iUsed >= m_iAllocated)
        DoubleSpace();

    kMDtMultParentInfo* pJ = NiExternalNew kMDtMultParentInfo;
    pJ->m_Node = mObj;
    pJ->iCurrent = 0;
    pJ->iNumParents = iParentCnt;
    m_ppMultParentInfo[m_iUsed++] = pJ; 

    return (0); // return the encounter (which is always zero) on this branch
}
//---------------------------------------------------------------------------
kMDtMultParentInfo* kMDtMultParentManager::Get( int iIndex )
{
    return m_ppMultParentInfo[iIndex];
}
//---------------------------------------------------------------------------


