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

#ifndef MDTPHYSXCLOTHCONSTR_H
#define MDTPHYSXCLOTHCONSTR_H

#include "MDt.h"
#include "MDtExt.h"



class MDtPhysXClothConstr
{
public:
    MDtPhysXClothConstr();
    ~MDtPhysXClothConstr();

    bool Load(MObject kConstr);

    long m_lConstrIndex;
    MObject m_MObjectConstr;

    MObject m_kClothNode;
    MObject m_kShapeNode;
        
    // The number of vertices that are attached or 0 if
    // it is to be a “collision” type attachment.
    unsigned int m_uiVertexCount;
    
    // The vertices that are attached.
    unsigned int* m_puiVertexIDs;
    
    // The locations of the attachment on the shape or in the world.
    float* m_pkPositions;
};


class MDtPhysXClothConstrManager
{
public:

    MDtPhysXClothConstrManager();
    ~MDtPhysXClothConstrManager();

    void Reset();

    unsigned int GetNumConstraints();

    int AddClothConstr(MDtPhysXClothConstr* pNewConstr);
    MDtPhysXClothConstr* GetClothConstr(int iIndex);
    int GetClothConstr(MObject mobj);     

protected:

    int m_iNumConstr;
    MDtPhysXClothConstr** m_ppConstrs;
};


//---------------------------------------------------------------------------

int addPhysXClothConstr(MObject constrNode);

//---------------------------------------------------------------------------

void gPhysXClothConstrNew();  // INITIALIZE AT THE START OF THE EXPORTER

//---------------------------------------------------------------------------

// Declare a global Joint Manager.
extern MDtPhysXClothConstrManager gPhysXClothConstrManager;

#endif // MDTPHYSXCLOTHCONSTR_H