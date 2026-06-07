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

// Precompiled Header
//---------------------------------------------------------------------------
#include "NiMeshPCH.h"
#include "NiMeshSDM.h"
#include "NiMeshLib.h"
#include "NiMeshModifier.h"
#include "NiToolDataStream.h"
#include "NiGeometryConverter.h"
#include "NiMeshVertexOperators.h"
#include "NiSkinningMeshModifier.h"
#include "NiMorphMeshModifier.h"
#include "NiInstancingMeshModifier.h"
#include "NiMeshHWInstance.h"
#include "NiMeshScreenElements.h"
#include "NiScreenFillingRenderViewImpl.h"
#include "NiMeshUpdateProcess.h"
#include <NiStream.h>
#if defined(_WII)
#include "NiWiiReflectionMeshModifier.h"
#endif

//---------------------------------------------------------------------------
NiImplementSDMConstructor(NiMesh, "NiFloodgate NiMain");
//---------------------------------------------------------------------------
#ifdef NIMESH_EXPORT
NiImplementDllMain(NiMesh);
#endif

//---------------------------------------------------------------------------
void NiMeshSDM::Init()
{
    NiImplementSDMInitCheck();
    NiMeshVertexOperators::_SDMInit();
#if defined(_WII)
    NiWiiReflectionMeshModifier::_SDMInit();
#endif

    // Register implementation classes
    NiRegisterStream(NiMesh);
    NiRegisterStream(NiMeshScreenElements);
    NiRegisterStream(NiMorphMeshModifier);
    NiRegisterStream(NiSkinningMeshModifier);
    NiRegisterStream(NiInstancingMeshModifier);
    NiRegisterStream(NiMeshHWInstance);
    NiRegisterStream(NiDataStream);

    NiStream::RegisterPostProcessFunction(
        NiGeometryConverter::ConvertToNiMesh);
    NiScreenFillingRenderViewImpl::RegisterFactoryMethod();

    // Give NiAVObject a default update process to use for the legacy
    // Update(float) calls
    NiAVObject::RegisterDefaultUpdateProcess(NiNew NiMeshUpdateProcess());
}
//---------------------------------------------------------------------------
void NiMeshSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiAVObject::RegisterDefaultUpdateProcess(NULL);

    NiUnregisterStream(NiMesh);
    NiUnregisterStream(NiMeshScreenElements);
    NiUnregisterStream(NiMorphMeshModifier);
    NiUnregisterStream(NiSkinningMeshModifier);
    NiUnregisterStream(NiInstancingMeshModifier);
    NiUnregisterStream(NiMeshHWInstance);
    NiUnregisterStream(NiDataStream);

    NiStream::UnregisterPostProcessFunction(
        NiGeometryConverter::ConvertToNiMesh);

#if defined(_WII)
    NiWiiReflectionMeshModifier::_SDMShutdown();
#endif
    NiMeshVertexOperators::_SDMShutdown();
}
//---------------------------------------------------------------------------
