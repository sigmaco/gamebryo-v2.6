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

// MDtSkinBoneIK.h

#ifndef MDTSKINBONEIK_H
#define MDTSKINBONEIK_H

#include <maya/MStatus.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MFnTransform.h>

#include "MDt.h"    
#include "MDtExt.h"

class kDtIK
{
    public:
        int m_iModels;

        kDtIK(void);
        ~kDtIK(void);
        int IKNew(void);
        int GetRigidSkinningInfo(void);
        int GetSmoothSkinningInfo(void);

};

int addTransformJoint( MDagPath &transformPath,
                       MObject transformNode,
                       MObject parentNode );


int gIKDataNew(void);

extern kDtIK gIKData;
#endif