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

#ifndef MAYAPLUGIN_H
#define MAYAPLUGIN_H

//#define REQUIRE_IOSTREAM

#include "iffwriter.h"
#include "maya/MAnimControl.h"
#include "maya/MCommandResult.h"
#include "maya/MFileObject.h"
#include "maya/MGlobal.h"
#include "maya/MIntArray.h"
#include "maya/MItDependencyGraph.h"
#include "maya/MItMeshPolygon.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MFn.h"
#include "maya/MFnAnimCurve.h"
#include "maya/MFnAttribute.h" 
#include "maya/MFnData.h"
#include "maya/MFnDoubleArrayData.h"
#include "maya/MFnEnumAttribute.h"
#include "maya/MFnIntArrayData.h"
#include "maya/MFnMatrixData.h"
#include "maya/MFnNonAmbientLight.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnNumericData.h" 
#include "maya/MFnPlugin.h"
#include "maya/MPointArray.h"
#include "maya/MFnPointArrayData.h"
#include "maya/MFnSkinCluster.h"
#include "maya/MFnStringArrayData.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MMatrix.h"
#include "maya/MObjectArray.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MPxFileTranslator.h"
#include "maya/MStatus.h"
#include "maya/MSceneMessage.h"
#include "maya/MSelectionList.h"
#include "maya/MString.h"
#include "maya/MStringArray.h"
#include "maya/MTime.h"
#include "maya/MFnAnimCurve.h"
#include "maya/MProgressWindow.h"

#include <strstream>
#include <vector>
#include <algorithm>

#include <windows.h>

#include <NiFrustum.h>
#include <NiMath.h>
#include <NiAnimation.h>
#include <NiShaderDesc.h>
#include <NiExporterOptionsSharedData.h>
#include <NiFramework.h>
#include <NiMain.h>
#include <NiOptimization.h>
#include <NiPluginManager.h>
#include <NiPluginToolkit.h>
#include <NiRendererSharedData.h>
#include <NiSceneGraphSharedData.h>
#include <NiScriptInfoDialogs.h>
#include <NiScriptTemplateManager.h>
#include <NiMaterialToolkit.h>
#include <NiStripifyInterface.h>
#include <NiTimerSharedData.h>
#include <NiVersion.h>
#include <NiDevImageQuantizer.h>
#include <NiTextureTransform.h>
#include <NiTextureTransformController.h>
#include <NiParticle.h>
#include <NiOpenEXRReader.h>
#include <NiHDRReader.h>
#include <NiPNGReader.h>
#include <NiMesh.h>
#include <NiRenderObjectMaterialOption.h>
#include <NiNBTGenerator.h>






#include "MDt.h"
#include "MDtCollider.h"
#include "MDtExt.h"
#include "MDtField.h"
#include "MDtLevelOfDetail.h"
#include "MDtLight.h"
#include "MDtParticleSystem.h"
#include "MDtPhysXClothConstr.h"
#include "MDtPhysXJoint.h"
#include "MDtPhysXShape.h"


#include "MyiCamera.h"
#include "MyiExport.h"
#include "MyiFloatData.h"
#include "MyiLight.h"
#include "MyiMaterial.h"
#include "MyiMaterialManager.h"
#include "MyImmerse.h"
#include "MyiNode.h"
#include "MyiObject.h"
#include "MyiPlugin.h"
#include "MyiParticleSystem.h"
#include "MyiPhysX.h"
#include "MyiPosData.h"
#include "MyiRotData.h"
#include "MyiScaleData.h"
#include "MyiTexture.h"
#include "MyiTextureEffect.h"
#include "MyiTextureManager.h"
#include "MyiTriMesh.h"
#include "MyiUserData.h"
#include "MyiVisibleData.h"
#include "StatusChecking.h"



#endif // #ifndef MAYAPLUGIN_H

