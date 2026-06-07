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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXParticle.h"
#include "NiPhysXParticleConverter.h"

// Deprecated classes.
#include "NiPhysXMeshParticleSystem.h"
#include "NiPhysXMeshPSysData.h"
#include "NiPhysXMeshPSysProp.h"
#include "NiPhysXMeshPSysSrc.h"
#include "NiPhysXParticleSystem.h"
#include "NiPhysXPSysData.h"
#include "NiPhysXPSysDest.h"
#include "NiPhysXPSysProp.h"
#include "NiPhysXPSysSrc.h"
#include "NiPhysXPSysUpdateModifier.h"

NiImplementSDMConstructor(NiPhysXParticle, \
    "NiPhysX NiMain NiMesh NiParticle NiAnimation");

#ifdef NIPHYSXPARTICLE_EXPORT
NiImplementDllMain(NiPhysXParticle);
#endif

//---------------------------------------------------------------------------
void NiPhysXParticleSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiPhysXPSMeshParticleSystem);
    NiRegisterStream(NiPhysXPSMeshParticleSystemProp);
    NiRegisterStream(NiPhysXPSParticleSystem);
    NiRegisterStream(NiPhysXPSParticleSystemDest);
    NiRegisterStream(NiPhysXPSParticleSystemProp);
    NiRegisterStream(NiPhysXPSSimulator);
    NiRegisterStream(NiPhysXPSSimulatorFinalStep);
    NiRegisterStream(NiPhysXPSSimulatorInitialStep);

    // Deprecated classes.
    NiRegisterStream(NiPhysXMeshParticleSystem);
    NiRegisterStream(NiPhysXMeshPSysData);
    NiRegisterStream(NiPhysXMeshPSysProp);
    NiRegisterStream(NiPhysXMeshPSysSrc);
    NiRegisterStream(NiPhysXParticleSystem);
    NiRegisterStream(NiPhysXPSysData);
    NiRegisterStream(NiPhysXPSysDest);
    NiRegisterStream(NiPhysXPSysProp);
    NiRegisterStream(NiPhysXPSysSrc);
    NiRegisterStream(NiPhysXPSysUpdateModifier);
    
    // Initialization functions.
    NiPhysXPSCommonSemantics::_SDMInit();

    NiStream::RegisterPostProcessFunction(NiPhysXParticleConverter::Convert);
}
//---------------------------------------------------------------------------
void NiPhysXParticleSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiPhysXPSMeshParticleSystem);
    NiUnregisterStream(NiPhysXPSMeshParticleSystemProp);
    NiUnregisterStream(NiPhysXPSParticleSystem);
    NiUnregisterStream(NiPhysXPSParticleSystemDest);
    NiUnregisterStream(NiPhysXPSParticleSystemProp);
    NiUnregisterStream(NiPhysXPSSimulator);
    NiUnregisterStream(NiPhysXPSSimulatorFinalStep);
    NiUnregisterStream(NiPhysXPSSimulatorInitialStep);

    // Deprecated classes.
    NiUnregisterStream(NiPhysXMeshParticleSystem);
    NiUnregisterStream(NiPhysXMeshPSysData);
    NiUnregisterStream(NiPhysXMeshPSysProp);
    NiUnregisterStream(NiPhysXMeshPSysSrc);
    NiUnregisterStream(NiPhysXParticleSystem);
    NiUnregisterStream(NiPhysXPSysData);
    NiUnregisterStream(NiPhysXPSysDest);
    NiUnregisterStream(NiPhysXPSysProp);
    NiUnregisterStream(NiPhysXPSysSrc);
    NiUnregisterStream(NiPhysXPSysUpdateModifier);

    // Shutdown functions.
    NiPhysXPSCommonSemantics::_SDMShutdown();

    NiStream::UnregisterPostProcessFunction(NiPhysXParticleConverter::Convert);
}
//---------------------------------------------------------------------------
