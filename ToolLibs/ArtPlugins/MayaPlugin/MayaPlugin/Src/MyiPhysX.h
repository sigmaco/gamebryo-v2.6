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

#ifndef MYIPHYSX_H
#define MYIPHYSX_H

#include "MyiNode.h"

#include "MDt.h"
#include "MDtExt.h"
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>

#include <NiNode.h>

class MyiPhysX : public MyiObject
{
public:
    static void Initialize();
    static void Shutdown();

    static void ProcessActor(NiNode* pkNewNode, MFnDagNode& dgNode);
    static void ProcessMesh(NiAVObject* pkMesh, int iShapeNum,
        int iGroupID);
    static void ProcessPhysXShape(int iShapeNum);
    static void ProcessPhysXJoint(int iComponentNum);

protected:
    static void ProcessCloth(NiNode* pkNode, MObject& kMeshInObj);

    static void ExtractAxisAngle(double* adAngles, float* afAxis,
        float* afNormal);
    static void AppendIntExtraData(NiNode* pkNode, const char* pcTag,
        int iVal);
    static void AppendVectorExtraData(NiNode* pkNode, const char* pcTag,
        float* afAddVals);
    static void AppendFloatExtraData(NiNode* pkNode, const char* pcTag,
        float fVal);
    static void AppendStringExtraData(NiNode* pkNode, const char* pcTag,
        const char* pcVal);

    static unsigned int ms_uiNextIdentifier;
    
    // Map of objects, which are sure to be meshes, to PhysX compound IDs
    static NiTMap<int, unsigned int>* ms_pkShapeMap;
    static NiTMap<unsigned int, bool>* ms_pkConvexMap;
    static NiTMap<int, unsigned int>* ms_pkPhysXShapeMap;
    static NiTMap<int, unsigned int>* ms_pkPhysXClothConstrMap;
};

#endif // #ifndef MYIPHYSX_H
