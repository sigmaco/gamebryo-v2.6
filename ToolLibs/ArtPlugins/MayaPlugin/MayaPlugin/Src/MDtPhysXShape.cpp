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


// Declare a global PhysX shape Manager.
kMDtPhysXShapeManager gPhysXShapeManager;


//---------------------------------------------------------------------------
kMDtPhysXShape::kMDtPhysXShape()
{
    m_lShapeIndex = -1;
    m_szName = NULL;

    m_MObjectShape = MObject::kNullObj;
    m_MObjectParent = MObject::kNullObj;

    // All other attributes
    m_eShapeType = kNone;

    m_fShapeScaleX = 1.0f;
    m_fShapeScaleY = 1.0f;
    m_fShapeScaleZ = 1.0f;
    
    m_fPoint1X = 0.0f;
    m_fPoint1Y = 0.0f;
    m_fPoint1Z = 0.0f;

    m_fPoint2X = 0.0f;
    m_fPoint2Y = 0.0f;
    m_fPoint2Z = 0.0f;

    m_fRadius = 0.0f;

    m_bUseDensity = true;
    m_fMass = 0.0f;
    m_fDensity = 1.0f;
}
//---------------------------------------------------------------------------
kMDtPhysXShape::~kMDtPhysXShape()
{
    // Free the allocated name
    if(m_szName)
    {
        NiFree(m_szName);
        m_szName = NULL;
    }
}
//---------------------------------------------------------------------------
bool kMDtPhysXShape::Load(MObject kShape, MObject kParent)
{
    MStatus stat = MS::kSuccess;
    MFnDependencyNode dgNode;
    stat = dgNode.setObject(kShape);

    m_MObjectShape = kShape;
    m_MObjectParent = kParent;

    // Create the Name buffer
    size_t stNameLength = strlen( dgNode.name().asChar() ) + 1;
    m_szName = NiAlloc(char, stNameLength);
    NiStrcpy( m_szName, stNameLength, dgNode.name().asChar() );   

    // ----------------------------------------------------------
    MPlug Plug;

    Plug = dgNode.findPlug("shapeType");
    NIASSERT( !Plug.isNull() );
    int iType;
    if( Plug.getValue( iType ) != MS::kSuccess )
        printf("Error Reading radius\n");
    m_eShapeType = (ShapeType)iType;
    
    MPlug kComponent;
    Plug = dgNode.findPlug("scale");
    NIASSERT( !Plug.isNull() );
    kComponent = Plug.child(0);
    if( kComponent.getValue(m_fShapeScaleX) != MS::kSuccess )
        printf("Error Reading ScaleX\n");
    kComponent = Plug.child(1);
    if( kComponent.getValue(m_fShapeScaleY) != MS::kSuccess )
        printf("Error Reading ScaleY\n");
    kComponent = Plug.child(2);
    if( kComponent.getValue(m_fShapeScaleZ) != MS::kSuccess )
        printf("Error Reading ScaleZ\n");

    Plug = dgNode.findPlug("point1");
    NIASSERT( !Plug.isNull() );
    kComponent = Plug.child(0);
    if( kComponent.getValue(m_fPoint1X) != MS::kSuccess )
        printf("Error Reading Point1X\n");
    kComponent = Plug.child(1);
    if( kComponent.getValue(m_fPoint1Y) != MS::kSuccess )
        printf("Error Reading Point1Y\n");
    kComponent = Plug.child(2);
    if( kComponent.getValue(m_fPoint1Z) != MS::kSuccess )
        printf("Error Reading Point1Z\n");

    Plug = dgNode.findPlug("point2");
    NIASSERT( !Plug.isNull() );
    kComponent = Plug.child(0);
    if( kComponent.getValue(m_fPoint2X) != MS::kSuccess )
        printf("Error Reading Point2X\n");
    kComponent = Plug.child(1);
    if( kComponent.getValue(m_fPoint2Y) != MS::kSuccess )
        printf("Error Reading Point2Y\n");
    kComponent = Plug.child(2);
    if( kComponent.getValue(m_fPoint2Z) != MS::kSuccess )
        printf("Error Reading Point2Z\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("radius");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fRadius ) != MS::kSuccess )
        printf("Error Reading radius\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("useMassOrDensity");  

    NIASSERT( !Plug.isNull() );
    int iUseDensity;
    if( Plug.getValue( iUseDensity ) != MS::kSuccess )
        printf("Error Reading useMassOrDensity\n");
    m_bUseDensity = (iUseDensity > 0);

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("mass");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fMass ) != MS::kSuccess )
        printf("Error Reading mass\n");

    // ----------------------------------------------------------
    Plug = dgNode.findPlug("density");  

    NIASSERT( !Plug.isNull() );
    if( Plug.getValue( m_fDensity ) != MS::kSuccess )
        printf("Error Reading density\n");

    return true;
}

//---------------------------------------------------------------------------
//
//   PHYSX SHAPE MANAGER
//
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
kMDtPhysXShapeManager::kMDtPhysXShapeManager()
{
    m_iNumShapes = 0;
    m_ppShapes = NULL;
}
//---------------------------------------------------------------------------
kMDtPhysXShapeManager::~kMDtPhysXShapeManager()
{
    if( m_ppShapes )
    {
        NiFree(m_ppShapes);
        m_ppShapes = NULL;

        m_iNumShapes = 0;
    }
}
//---------------------------------------------------------------------------
void kMDtPhysXShapeManager::Reset()
{
    if (m_ppShapes)
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < (unsigned int)m_iNumShapes; uiLoop++)
            NiExternalDelete m_ppShapes[uiLoop];

        NiFree(m_ppShapes);
        m_ppShapes = NULL;

        m_iNumShapes = 0;
    }
}
//---------------------------------------------------------------------------
int kMDtPhysXShapeManager::GetNumShapes()
{
    return m_iNumShapes;
}
//---------------------------------------------------------------------------
int kMDtPhysXShapeManager::AddShape( kMDtPhysXShape* pNewShape)
{
    // INCREASE THE SIZE OF THE ARRAY
    kMDtPhysXShape** ppExpandedArray = NiAlloc(kMDtPhysXShape*,
        m_iNumShapes+1);

    // Copy over the old array;
    for(int iLoop = 0; iLoop < m_iNumShapes; iLoop++)
    {
        ppExpandedArray[iLoop] = m_ppShapes[iLoop];
    }

    // Add the new particle System
    ppExpandedArray[m_iNumShapes] = pNewShape;

    // DELETE THE OLD ARRAY
    NiFree(m_ppShapes);

    // Assign the newly created array
    m_ppShapes = ppExpandedArray;

    return m_iNumShapes++;
}
//---------------------------------------------------------------------------
kMDtPhysXShape* kMDtPhysXShapeManager::GetShape( int iIndex )
{
    NIASSERT(iIndex >= 0);
    NIASSERT(iIndex < m_iNumShapes);

    return m_ppShapes[iIndex];
}
//---------------------------------------------------------------------------
int kMDtPhysXShapeManager::GetShape( MObject mobj )
{
    // Scan the Array of Particle Systems looking for the match
    for(int iLoop = 0; iLoop < m_iNumShapes; iLoop++)
    {
        if(m_ppShapes[iLoop]->m_MObjectShape == mobj)
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
int addTransformPhysXShape(MObject transformNode, MObject shapeNode)
{
    kMDtPhysXShape* pNewShape = NiExternalNew kMDtPhysXShape();

    if(!pNewShape->Load(shapeNode, transformNode))
    {
        NiExternalDelete pNewShape;
        return -1;
    }

    return gPhysXShapeManager.AddShape(pNewShape);
}
//---------------------------------------------------------------------------
void gPhysXShapeNew()
{
    // INITIALIZE AT THE START OF THE EXPORTER

    gPhysXShapeManager.Reset();
}
//---------------------------------------------------------------------------
int DtExt_PhysXGetShapeNode(int shapeID, MObject &obj )
{
    kMDtPhysXShape* pkShape = gPhysXShapeManager.GetShape(shapeID);

    if(pkShape == NULL)
        return 0;

    obj = pkShape->m_MObjectShape;
    return 1;
}
//---------------------------------------------------------------------------
int DtExt_PhysXGetTransform(int shapeID, MObject &obj )
{
    kMDtPhysXShape* pkShape = gPhysXShapeManager.GetShape(shapeID);

    if(pkShape == NULL)
        return 0;

    obj = pkShape->m_MObjectParent;
    return 1;
}
//---------------------------------------------------------------------------
