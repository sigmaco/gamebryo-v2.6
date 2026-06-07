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

// MDtParticleSystem.h

#ifndef MDTPHYSXJOINT_H
#define MDTPHYSXJOINT_H

#include "MDt.h"
#include "MDtExt.h"



class kMDtPhysXJoint
{
public:
    kMDtPhysXJoint();
    ~kMDtPhysXJoint();

    bool Load(MObject kJoint);

    long m_lJointIndex;
    MObject m_MObjectJoint;

    // We extract all attributes later - it's all in the MObject
};


class kMDtPhysXJointManager
{
public:

    kMDtPhysXJointManager();
    ~kMDtPhysXJointManager();

    void Reset();

    int GetNumJoints();

    int AddJoint(kMDtPhysXJoint* pNewJoint);
    kMDtPhysXJoint* GetJoint(int iIndex);
    int GetJoint(MObject mobj);     

protected:

    int m_iNumJoints;
    kMDtPhysXJoint** m_ppJoints;
};


//---------------------------------------------------------------------------

int addPhysXJoint(MObject jointNode);

//---------------------------------------------------------------------------

void gPhysXJointNew();  // INITIALIZE AT THE START OF THE EXPORTER

//---------------------------------------------------------------------------

// Declare a global Joint Manager.
extern kMDtPhysXJointManager gPhysXJointManager;

#endif