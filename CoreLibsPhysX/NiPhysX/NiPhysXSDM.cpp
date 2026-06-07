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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"
#include "NiPhysXClothDest.h"
#include "NiPhysXConverter.h"

NiImplementSDMConstructor(NiPhysX, "NiMain NiMesh NiAnimation");

#ifdef NIPHYSX_EXPORT
NiImplementDllMain(NiPhysX);
#endif

//---------------------------------------------------------------------------
void NiPhysXSDM::Init()
{
    NiImplementSDMInitCheck();

    // In Gamebryo 2.6, NiPhysXAccumInterpolatorDest was deprecated.
    // NiPhysXAccumPoseBufferDest took its place.
    NiStream::RegisterLoader("NiPhysXAccumInterpolatorDest",
        NiPhysXAccumPoseBufferDest::CreateObject);

    NiRegisterStream(NiPhysXAccumPoseBufferDest);
    NiRegisterStream(NiPhysXAccumTransformDest);
    NiRegisterStream(NiPhysXActorDesc);
    NiRegisterStream(NiPhysXBodyDesc);
    NiRegisterStream(NiPhysXClothDesc);
    NiRegisterStream(NiPhysXClothModifier);
    NiRegisterStream(NiPhysXDynamicSrc);

    // In Gamebryo 2.6, NiPhysXInterpolatorDest was deprecated.
    // NiPhysXPoseBufferDest took its place.
    NiStream::RegisterLoader("NiPhysXInterpolatorDest",
        NiPhysXPoseBufferDest::CreateObject);

    NiRegisterStream(NiPhysXD6JointDesc);
    NiRegisterStream(NiPhysXJointDesc);
    NiRegisterStream(NiPhysXKinematicSrc);
    NiRegisterStream(NiPhysXMaterialDesc);
    NiRegisterStream(NiPhysXMeshDesc);
    NiRegisterStream(NiPhysXPoseBufferDest);
    NiRegisterStream(NiPhysXProp);
    NiRegisterStream(NiPhysXPropDesc);
    NiRegisterStream(NiPhysXScene);
    NiRegisterStream(NiPhysXSceneDesc);
    NiRegisterStream(NiPhysXSDKDesc);
    NiRegisterStream(NiPhysXShapeDesc);
    NiRegisterStream(NiPhysXTransformDest);

    // Deprecated
    NiRegisterStream(NiPhysXClothDest);

    NiPhysXManager::ms_pPhysXManager = NiNew NiPhysXManager();
    NIASSERT(NiPhysXManager::ms_pPhysXManager);

    NiStream::RegisterPostProcessFunction(NiPhysXConverter::ConvertToNiMesh);
}
//---------------------------------------------------------------------------
void NiPhysXSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    // In Gamebryo 2.6, NiPhysXAccumInterpolatorDest was deprecated.
    // NiPhysXAccumPoseBufferDest took its place.
    NiUnregisterStream(NiPhysXAccumInterpolatorDest);

    NiUnregisterStream(NiPhysXAccumPoseBufferDest);
    NiUnregisterStream(NiPhysXAccumTransformDest);
    NiUnregisterStream(NiPhysXActorDesc);
    NiUnregisterStream(NiPhysXBodyDesc);
    NiUnregisterStream(NiPhysXClothDesc);
    NiUnregisterStream(NiPhysXClothModifier);
    NiUnregisterStream(NiPhysXDynamicSrc);

    // In Gamebryo 2.6, NiPhysXInterpolatorDest was deprecated.
    // NiPhysXPoseBufferDest took its place.
    NiUnregisterStream(NiPhysXInterpolatorDest);

    NiUnregisterStream(NiPhysXD6JointDesc);
    NiUnregisterStream(NiPhysXJointDesc);
    NiUnregisterStream(NiPhysXKinematicSrc);
    NiUnregisterStream(NiPhysXMaterialDesc);
    NiUnregisterStream(NiPhysXMeshDesc);
    NiUnregisterStream(NiPhysXPoseBufferDest);
    NiUnregisterStream(NiPhysXProp);
    NiUnregisterStream(NiPhysXPropDesc);
    NiUnregisterStream(NiPhysXScene);
    NiUnregisterStream(NiPhysXSceneDesc);
    NiUnregisterStream(NiPhysXSDKDesc);
    NiUnregisterStream(NiPhysXShapeDesc);
    NiUnregisterStream(NiPhysXTransformDest);

    // Deprecated
    NiUnregisterStream(NiPhysXClothDest);

    NiStream::UnregisterPostProcessFunction(NiPhysXConverter::ConvertToNiMesh);

    NiDelete NiPhysXManager::ms_pPhysXManager;
}
//---------------------------------------------------------------------------
