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

#ifndef NIMESHLIB_H
#define NIMESHLIB_H

// sets up for DLL import (if desired)
#include "NiMeshLibType.h"
#include "NiBatchedUpdateProcess.h"
#include "NiDataStream.h"
#include "NiDataStreamElement.h"
#include "NiDataStreamElementLock.h"
#include "NiDataStreamFactory.h"
#include "NiDataStreamRef.h"
#include "NiDataStreamLock.h"
#include "NiDataStreamLockPolicy.h"
#include "NiDataStreamPrimitiveLock.h"
#include "NiGeometryConverter.h"
#include "NiImmediateModeAdapter.h"
#include "NiImmediateModeMacro.h"
#include "NiInstancingMeshModifier.h"
#include "NiInstancingUtilities.h"
#include "NiMatrix3x4.h"
#include "NiMesh2DRenderView.h"
#include "NiMeshAlgorithms.h"
#include "NiMeshCullingProcess.h"
#include "NiMesh.h"
#include "NiMeshModifier.h"
#include "NiMeshRequirements.h"
#include "NiMeshScreenElements.h"
#include "NiMeshSDM.h"
#include "NiMeshUpdateProcess.h"
#include "NiMeshUtilities.h"
#include "NiMeshVertexOperators.h"
#include "NiMorphMeshModifier.h"
#include "NiScreenFillingRenderViewImpl.h"
#include "NiSkinningMeshModifier.h"
#include "NiTIndexGenerationIterator.h"
#include "NiToolDataStream.h"
#include "NiTPrimitiveRandomAccessIterator.h"
#include "NiTSimpleArray.h"
#include "NiTStridedRandomAccessIterator.h"

static NiMeshSDM NiMeshSDMObject;

#endif //#ifndef NIMESH_H
