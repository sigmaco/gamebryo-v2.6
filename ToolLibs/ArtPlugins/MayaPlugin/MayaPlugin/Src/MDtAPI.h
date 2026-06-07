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

#ifndef MDTAPI_H
#define MDTAPI_H


#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//#include <sys/param.h>
#include <malloc.h>
#include <stdarg.h>
#include <math.h>
#include <process.h>

typedef unsigned int uint;
#include <maya/MTypes.h>

#ifndef __uint32_t
typedef __int32 __uint32_t;
#endif

// End of the NT specific modifications (Maya NT 1.0)

#include <maya/ilib.h>
#include <maya/MAnimControl.h>
#include <maya/MColor.h>
#include <maya/MCommandResult.h>
#include <maya/MComputation.h>
#include "maya/MDoubleArray.h"
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnAnimCurve.h>
#include "maya/MFnAttribute.h"
#include <maya/MFnBlinnShader.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnLight.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNonAmbientLight.h>
#include <maya/MFnNonExtendedLight.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnPhongShader.h>
#include <maya/MPointArray.h>
#include <maya/MFnReflectShader.h>
#include <maya/MFnSet.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFnTransform.h>
#include <maya/MFnWeightGeometryFilter.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MITDependencyNodes.h>
#include <maya/MITGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItSelectionList.h>
#include <maya/MITSurfaceCV.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPoint.h>
#include <maya/MQuaternion.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include "maya/MStringArray.h"
#include <maya/MTime.h>
#include "maya/MTransformationMatrix.h"
#include <maya/MTesselationParams.h>
#include <maya/MVector.h>
#include <maya/MFloatMatrix.h>
#include <maya/MRenderUtil.h>
#include <maya/MFnIkHandle.h>
#include <maya/MProgressWindow.h>

#include <iffreader.h>
#include <iffwriter.h>

#include "MDt.h"
//#include "MDtAnim.h"
#include <MDtCamera.h>
#include "MDtCollider.h"
#include "MDtExt.h"
#include <MDtField.h>
#include <MDtJoint.h>
#include <MDtLevelOfDetail.h>
#include <MDtLight.h>
#include "MDtMultParents.h"
#include <MDtParticleSystem.h>
#include "MDtPhysXClothConstr.h"
#include "MDtPhysXJoint.h"
#include "MDtPhysXShape.h"
#include "MDtSkinBoneIK.h"
#include "kMDtObject.h"

#include <NiMatrix3.h>
#include <NiOptimize.h>

#if defined(MAYA65) || defined(MAYA70) || defined(MAYA80)
#include <iostream>
using namespace std;
#endif





// The Maya API header files.
#ifndef MAYA40
#include <maya/MItMeshFaceVertex.h>
#endif


#endif // #ifndef MDTAPI_H
