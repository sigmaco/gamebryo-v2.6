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

#include <maya/MFnIntArrayData.h>
#include <maya/MFnPointArrayData.h>


// Declare a global Cloth Constraint Manager.
MDtPhysXClothConstrManager gPhysXClothConstrManager;


//---------------------------------------------------------------------------
MDtPhysXClothConstr::MDtPhysXClothConstr()
{
    m_lConstrIndex = -1;

    m_MObjectConstr = MObject::kNullObj;

    MObject m_kClothNode = MObject::kNullObj;
    MObject m_kShapeNode = MObject::kNullObj;

    m_uiVertexCount = 0;
    m_puiVertexIDs = 0;
    m_pkPositions = 0;
}
//---------------------------------------------------------------------------
MDtPhysXClothConstr::~MDtPhysXClothConstr()
{
    NiFree(m_puiVertexIDs);
    NiFree(m_pkPositions);
}
//---------------------------------------------------------------------------
bool MDtPhysXClothConstr::Load(MObject kConstr)
{
    m_MObjectConstr = kConstr;

    MPlug kPlug;
    MPlug kComponent;
    MPlugArray kPlugArr;
    MStatus kStatus;

    MFnDependencyNode dgNode(m_MObjectConstr, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "Cloth Constraint object is not a dependency node\n");
        return false;
    }
    
    kPlug = dgNode.findPlug("physicsShape", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "Could not find physicsShape plug\n");
        return false;
    }
    kPlug.connectedTo(kPlugArr, true, false, &kStatus);
    if (kPlugArr.length() == 1)
    {
        m_kShapeNode = kPlugArr[0].node();
    }
    
    bool bWorldAttachment = false;
    kPlug = dgNode.findPlug("worldSpace", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find worldSpace plug\n");
    }
    kPlug.getValue(bWorldAttachment);
    if (bWorldAttachment)
        m_kShapeNode = MObject::kNullObj;

    MObject kIntArrayObj;
    kPlug = dgNode.findPlug("vertexId", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "Could not find vertexId plug\n");
        return false;
    }
    kPlug.getValue(kIntArrayObj);
    MFnIntArrayData kFnIntArray(kIntArrayObj, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "kIntArrayObj is not an int array\n");
        return false;
    }
    m_uiVertexCount = kFnIntArray.length();
    m_puiVertexIDs = NiAlloc(unsigned int, m_uiVertexCount);
    for (unsigned int ui = 0; ui < m_uiVertexCount; ui++)
    {
        m_puiVertexIDs[ui] = kFnIntArray[ui];
    }  

    // Always stash world coordinates
    MObject kPointArrayObj;
    kPlug = dgNode.findPlug("vertexPos", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "Could not find vertexPos plug\n");
        return false;
    }
    kPlug.getValue(kPointArrayObj);
    MFnPointArrayData kFnPointArray(kPointArrayObj, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "kPointArrayObj is not an point array\n");
        return false;
    }
    if (m_uiVertexCount != kFnPointArray.length())
    {
        DtExt_Err("Error:: kMDtPhysXClothConstr::Load:"
            "kPointArrayObj is of wrong length\n");
        return false;
    }
    m_pkPositions = NiAlloc(float, m_uiVertexCount * 3);;
    for (unsigned int ui = 0; ui < m_uiVertexCount; ui++)
    {
        m_pkPositions[ui * 3] = (float)kFnPointArray[ui].x;
        m_pkPositions[ui * 3 + 1] = (float)kFnPointArray[ui].y;
        m_pkPositions[ui * 3 + 2] = (float)kFnPointArray[ui].z;
    }  

    return true;
}

//---------------------------------------------------------------------------
//
//   PHYSX CLOTH CONSTRAINT MANAGER
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
MDtPhysXClothConstrManager::MDtPhysXClothConstrManager()
{
    m_iNumConstr = 0;
    m_ppConstrs = NULL;
}
//---------------------------------------------------------------------------
MDtPhysXClothConstrManager::~MDtPhysXClothConstrManager()
{
    if( m_ppConstrs )
    {
        NiFree(m_ppConstrs);
        m_ppConstrs = NULL;

        m_iNumConstr = 0;
    }
}
//---------------------------------------------------------------------------
void MDtPhysXClothConstrManager::Reset()
{
    if (m_ppConstrs)
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < (unsigned int)m_iNumConstr; uiLoop++)
            NiExternalDelete m_ppConstrs[uiLoop];

        NiFree(m_ppConstrs);
        m_ppConstrs = NULL;

        m_iNumConstr = 0;
    }
}
//---------------------------------------------------------------------------
unsigned int MDtPhysXClothConstrManager::GetNumConstraints()
{
    return m_iNumConstr;
}
//---------------------------------------------------------------------------
int MDtPhysXClothConstrManager::AddClothConstr(
    MDtPhysXClothConstr* pNewConstr)
{
    // INCREASE THE SIZE OF THE ARRAY
    MDtPhysXClothConstr** ppExpandedArray = NiAlloc(MDtPhysXClothConstr*,
        m_iNumConstr+1);

    // Copy over the old array;
    for(int iLoop = 0; iLoop < m_iNumConstr; iLoop++)
    {
        ppExpandedArray[iLoop] = m_ppConstrs[iLoop];
    }

    // Add the new particle System
    ppExpandedArray[m_iNumConstr] = pNewConstr;

    // DELETE THE OLD ARRAY
    NiFree(m_ppConstrs);

    // Assign the newly created array
    m_ppConstrs = ppExpandedArray;

    return m_iNumConstr++;
}
//---------------------------------------------------------------------------
MDtPhysXClothConstr* MDtPhysXClothConstrManager::GetClothConstr(int iIndex)
{
    NIASSERT(iIndex >= 0);
    NIASSERT(iIndex < m_iNumConstr);

    return m_ppConstrs[iIndex];
}
//---------------------------------------------------------------------------
int MDtPhysXClothConstrManager::GetClothConstr( MObject mobj )
{
    // Scan the Array of Particle Systems looking for the match
    for(int iLoop = 0; iLoop < m_iNumConstr; iLoop++)
    {
        if(m_ppConstrs[iLoop]->m_MObjectConstr == mobj)
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
int addPhysXClothConstr(MObject constrNode)
{
    MDtPhysXClothConstr* pNewConstr = NiExternalNew MDtPhysXClothConstr();

    if(!pNewConstr->Load(constrNode))
    {
        NiExternalDelete pNewConstr;
        return -1;
    }

    return gPhysXClothConstrManager.AddClothConstr(pNewConstr);
}
//---------------------------------------------------------------------------
void gPhysXClothConstrNew()
{
    // INITIALIZE AT THE START OF THE EXPORTER

    gPhysXClothConstrManager.Reset();
}
//---------------------------------------------------------------------------

