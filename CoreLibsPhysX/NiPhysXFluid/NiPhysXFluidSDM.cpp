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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluid.h"
#include "NiPhysXFluidConverter.h"

// Deprecated. Remove when pre-2.4.0.0 NIF file support is dropped.
#include "NiPhysXFluidAgeDeathModifier.h"
#include "NiPhysXFluidData.h"
#include "NiPhysXFluidEmitter.h"
#include "NiPhysXFluidSceneDesc.h"
#include "NiPhysXFluidSystem.h"
#include "NiPhysXFluidUpdateModifier.h"
#include "NiPhysXMeshFluidData.h"
#include "NiPhysXMeshFluidSystem.h"

NiImplementSDMConstructor(NiPhysXFluid, \
    "NiPhysX NiMain NiMesh NiParticle NiAnimation NiPhysXParticle");

#ifdef NIPHYSXFLUID_EXPORT
NiImplementDllMain(NiPhysXFluid);
#endif

//---------------------------------------------------------------------------
void NiPhysXFluidSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiPhysXFluidDesc);
    NiRegisterStream(NiPhysXFluidDest);    
    NiRegisterStream(NiPhysXFluidEmitterDesc);
    NiRegisterStream(NiPhysXFluidPropDesc);
    NiRegisterStream(NiPhysXFluidSimulatorStep);
    NiRegisterStream(NiPhysXPSFluidSystem);
    NiRegisterStream(NiPhysXPSMeshFluidSystem);
    NiRegisterStream(NiPhysXPSFluidEmitter);

    // Deprecated
    NiRegisterStream(NiPhysXFluidAgeDeathModifier);
    NiRegisterStream(NiPhysXFluidData);
    NiRegisterStream(NiPhysXFluidEmitter);
    NiRegisterStream(NiPhysXFluidSceneDesc);
    NiRegisterStream(NiPhysXFluidSystem);
    NiRegisterStream(NiPhysXFluidUpdateModifier);
    NiRegisterStream(NiPhysXMeshFluidData);
    NiRegisterStream(NiPhysXMeshFluidSystem);
    
    // Initialization functions.
    NiPhysXFluidCommonSemantics::_SDMInit();

    NiStream::RegisterPostProcessFunction(NiPhysXFluidConverter::Convert);
}
//---------------------------------------------------------------------------
void NiPhysXFluidSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiPhysXFluidDesc);
    NiUnregisterStream(NiPhysXFluidDest);    
    NiUnregisterStream(NiPhysXFluidEmitterDesc);
    NiUnregisterStream(NiPhysXFluidPropDesc);
    NiUnregisterStream(NiPhysXFluidSimulatorStep);
    NiUnregisterStream(NiPhysXPSFluidSystem);
    NiUnregisterStream(NiPhysXPSMeshFluidSystem);
    NiUnregisterStream(NiPhysXPSFluidEmitter);

    // Deprecated
    NiUnregisterStream(NiPhysXFluidAgeDeathModifier);
    NiUnregisterStream(NiPhysXFluidData);
    NiUnregisterStream(NiPhysXFluidEmitter);
    NiUnregisterStream(NiPhysXFluidSceneDesc);
    NiUnregisterStream(NiPhysXFluidSystem);
    NiUnregisterStream(NiPhysXFluidUpdateModifier);
    NiUnregisterStream(NiPhysXMeshFluidData);
    NiUnregisterStream(NiPhysXMeshFluidSystem);
    
    // Shutdown functions.
    NiPhysXFluidCommonSemantics::_SDMShutdown();

    NiStream::UnregisterPostProcessFunction(NiPhysXFluidConverter::Convert);
}
//---------------------------------------------------------------------------
