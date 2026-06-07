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

#ifndef MDTPHYSXSHAPE_H
#define MDTPHYSXSHAPE_H

#include "MDt.h"
#include "MDtExt.h"



class kMDtPhysXShape
{
public:
    kMDtPhysXShape();
    ~kMDtPhysXShape();

    typedef enum {
        kNone,
        kBox,
        kSphere,
        kConvexHull,
        kCapsule,
        kCylinder,
        kEllipsoid,
    } ShapeType;

    bool Load(MObject kShape, MObject kParent);

    long        m_lShapeIndex;
    char*       m_szName;
    MObject     m_MObjectShape;
    MObject     m_MObjectParent;

    // Attributes from PhysicsShape
    ShapeType m_eShapeType;

    float m_fShapeScaleX;
    float m_fShapeScaleY;
    float m_fShapeScaleZ;
    
    float m_fPoint1X;
    float m_fPoint1Y;
    float m_fPoint1Z;

    float m_fPoint2X;
    float m_fPoint2Y;
    float m_fPoint2Z;

    float m_fRadius;

    bool m_bUseDensity;
    float m_fMass;
    float m_fDensity;
};


class kMDtPhysXShapeManager
{
public:

    kMDtPhysXShapeManager();
    ~kMDtPhysXShapeManager();

    void Reset();

    int GetNumShapes();

    int AddShape(kMDtPhysXShape* pNewShape);
    kMDtPhysXShape* GetShape(int iIndex);
    int GetShape(MObject mobj);     

protected:

    int m_iNumShapes;
    kMDtPhysXShape** m_ppShapes;
};


//---------------------------------------------------------------------------

int addTransformPhysXShape(MObject transformNode,  MObject shapeNode);

//---------------------------------------------------------------------------

void gPhysXShapeNew();  // INITIALIZE AT THE START OF THE EXPORTER

//---------------------------------------------------------------------------

// Declare a global PhysX shape Manager.
extern kMDtPhysXShapeManager gPhysXShapeManager;

#endif