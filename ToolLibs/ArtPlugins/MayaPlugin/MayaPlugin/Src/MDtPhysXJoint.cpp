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

// Precompiled Headers
#include "MayaPluginPCH.h"


// Declare a global Joint Manager.
kMDtPhysXJointManager gPhysXJointManager;


//---------------------------------------------------------------------------
kMDtPhysXJoint::kMDtPhysXJoint()
{
    m_lJointIndex = -1;

    m_MObjectJoint = MObject::kNullObj;
}
//---------------------------------------------------------------------------
kMDtPhysXJoint::~kMDtPhysXJoint()
{
}
//---------------------------------------------------------------------------
bool kMDtPhysXJoint::Load(MObject kJoint)
{
    m_MObjectJoint = kJoint;

    return true;
}

//---------------------------------------------------------------------------
//
//   PHYSX SHAPE MANAGER
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
kMDtPhysXJointManager::kMDtPhysXJointManager()
{
    m_iNumJoints = 0;
    m_ppJoints = NULL;
}
//---------------------------------------------------------------------------
kMDtPhysXJointManager::~kMDtPhysXJointManager()
{
    if( m_ppJoints )
    {
        NiExternalFree(m_ppJoints);
        m_ppJoints = NULL;

        m_iNumJoints = 0;
    }
}
//---------------------------------------------------------------------------
void kMDtPhysXJointManager::Reset()
{
    if (m_ppJoints)
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < (unsigned int)m_iNumJoints; uiLoop++)
            NiExternalDelete m_ppJoints[uiLoop];

        NiExternalFree(m_ppJoints);
        m_ppJoints = NULL;

        m_iNumJoints = 0;
    }
}
//---------------------------------------------------------------------------
int kMDtPhysXJointManager::GetNumJoints()
{
    return m_iNumJoints;
}
//---------------------------------------------------------------------------
int kMDtPhysXJointManager::AddJoint( kMDtPhysXJoint* pNewJoint)
{
    // INCREASE THE SIZE OF THE ARRAY
    kMDtPhysXJoint** ppExpandedArray = NiExternalAlloc(kMDtPhysXJoint*,
        m_iNumJoints+1);

    // Copy over the old array;
    for(int iLoop = 0; iLoop < m_iNumJoints; iLoop++)
    {
        ppExpandedArray[iLoop] = m_ppJoints[iLoop];
    }

    // Add the new particle System
    ppExpandedArray[m_iNumJoints] = pNewJoint;

    // DELETE THE OLD ARRAY
    NiExternalFree(m_ppJoints);

    // Assign the newly created array
    m_ppJoints = ppExpandedArray;

    return m_iNumJoints++;
}
//---------------------------------------------------------------------------
kMDtPhysXJoint* kMDtPhysXJointManager::GetJoint( int iIndex )
{
    NIASSERT(iIndex >= 0);
    NIASSERT(iIndex < m_iNumJoints);

    return m_ppJoints[iIndex];
}
//---------------------------------------------------------------------------
int kMDtPhysXJointManager::GetJoint( MObject mobj )
{
    // Scan the Array of Particle Systems looking for the match
    for(int iLoop = 0; iLoop < m_iNumJoints; iLoop++)
    {
        if(m_ppJoints[iLoop]->m_MObjectJoint == mobj)
            return iLoop;
    }

    return -1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  BASIC ADDING FUNCTIONS
//
//---------------------------------------------------------------------------
int addPhysXJoint(MObject jointNode)
{
    kMDtPhysXJoint* pNewJoint = NiExternalNew kMDtPhysXJoint();

    if(!pNewJoint->Load(jointNode))
    {
        NiExternalDelete pNewJoint;
        return -1;
    }

    return gPhysXJointManager.AddJoint(pNewJoint);
}
//---------------------------------------------------------------------------
void gPhysXJointNew()
{
    // INITIALIZE AT THE START OF THE EXPORTER

    gPhysXJointManager.Reset();
}
//---------------------------------------------------------------------------
