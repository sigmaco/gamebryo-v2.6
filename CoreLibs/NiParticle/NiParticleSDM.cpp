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
#include "NiParticlePCH.h"

#include "NiParticleSDM.h"
#include "NiParticle.h"

// Deprecated classes.
#include <NiMeshParticleSystem.h>
#include <NiMeshPSysData.h>
#include <NiParticleInfo.h>
#include <NiParticleSystem.h>
#include <NiPSysAgeDeathModifier.h>
#include <NiPSysAirFieldAirFrictionCtlr.h>
#include <NiPSysAirFieldInheritVelocityCtlr.h>
#include <NiPSysAirFieldModifier.h>
#include <NiPSysAirFieldSpreadCtlr.h>
#include <NiPSysBombModifier.h>
#include <NiPSysBoundUpdateModifier.h>
#include <NiPSysBoxEmitter.h>
#include <NiPSysCollider.h>
#include <NiPSysColliderManager.h>
#include <NiPSysColorModifier.h>
#include <NiPSysCylinderEmitter.h>
#include <NiPSysData.h>
#include <NiPSysDragFieldModifier.h>
#include <NiPSysDragModifier.h>
#include <NiPSysEmitter.h>
#include <NiPSysEmitterCtlr.h>
#include <NiPSysEmitterCtlrData.h>
#include <NiPSysEmitterDeclinationCtlr.h>
#include <NiPSysEmitterDeclinationVarCtlr.h>
#include <NiPSysEmitterInitialRadiusCtlr.h>
#include <NiPSysEmitterLifeSpanCtlr.h>
#include <NiPSysEmitterPlanarAngleCtlr.h>
#include <NiPSysEmitterPlanarAngleVarCtlr.h>
#include <NiPSysEmitterSpeedCtlr.h>
#include <NiPSysFieldAttenuationCtlr.h>
#include <NiPSysFieldMagnitudeCtlr.h>
#include <NiPSysFieldMaxDistanceCtlr.h>
#include <NiPSysFieldModifier.h>
#include <NiPSysGravityFieldModifier.h>
#include <NiPSysGravityModifier.h>
#include <NiPSysGravityStrengthCtlr.h>
#include <NiPSysGrowFadeModifier.h>
#include <NiPSysInitialRotAngleCtlr.h>
#include <NiPSysInitialRotAngleVarCtlr.h>
#include <NiPSysInitialRotSpeedCtlr.h>
#include <NiPSysInitialRotSpeedVarCtlr.h>
#include <NiPSysMeshEmitter.h>
#include <NiPSysMeshUpdateModifier.h>
#include <NiPSysModifier.h>
#include <NiPSysModifierActiveCtlr.h>
#include <NiPSysModifierCtlr.h>
#include <NiPSysModifierFloatCtlr.h>
#include <NiPSysPlanarCollider.h>
#include <NiPSysPositionModifier.h>
#include <NiPSysRadialFieldModifier.h>
#include <NiPSysResetOnLoopCtlr.h>
#include <NiPSysRotationModifier.h>
#include <NiPSysSpawnModifier.h>
#include <NiPSysSphereEmitter.h>
#include <NiPSysSphericalCollider.h>
#include <NiPSysTurbulenceFieldModifier.h>
#include <NiPSysUpdateCtlr.h>
#include <NiPSysVolumeEmitter.h>
#include <NiPSysVortexFieldModifier.h>

NiImplementSDMConstructor(NiParticle, "NiAnimation NiMesh NiFloodgate NiMain");

#ifdef NIPARTICLE_EXPORT
NiImplementDllMain(NiParticle);
#endif

//---------------------------------------------------------------------------
void NiParticleSDM::Init()
{
    NiImplementSDMInitCheck();

    // General classes.
    NiRegisterStream(NiPSBoundUpdater);
    NiRegisterStream(NiPSFacingQuadGenerator);
    NiRegisterStream(NiPSMeshParticleSystem);
    NiRegisterStream(NiPSParticleSystem);
    NiRegisterStream(NiPSSimulator);

    // Colliders.
    NiRegisterStream(NiPSPlanarCollider);
    NiRegisterStream(NiPSSphericalCollider);

    // Controllers.
    NiRegisterStream(NiPSAirFieldAirFrictionCtlr);
    NiRegisterStream(NiPSAirFieldInheritedVelocityCtlr);
    NiRegisterStream(NiPSAirFieldSpreadCtlr);
    NiRegisterStream(NiPSEmitParticlesCtlr);
    NiRegisterStream(NiPSEmitterDeclinationCtlr);
    NiRegisterStream(NiPSEmitterDeclinationVarCtlr);
    NiRegisterStream(NiPSEmitterLifeSpanCtlr);
    NiRegisterStream(NiPSEmitterPlanarAngleCtlr);
    NiRegisterStream(NiPSEmitterPlanarAngleVarCtlr);
    NiRegisterStream(NiPSEmitterRadiusCtlr);
    NiRegisterStream(NiPSEmitterRotAngleCtlr);
    NiRegisterStream(NiPSEmitterRotAngleVarCtlr);
    NiRegisterStream(NiPSEmitterRotSpeedCtlr);
    NiRegisterStream(NiPSEmitterRotSpeedVarCtlr);
    NiRegisterStream(NiPSEmitterSpeedCtlr);
    NiRegisterStream(NiPSFieldAttenuationCtlr);
    NiRegisterStream(NiPSFieldMagnitudeCtlr);
    NiRegisterStream(NiPSFieldMaxDistanceCtlr);
    NiRegisterStream(NiPSForceActiveCtlr);
    NiRegisterStream(NiPSGravityStrengthCtlr);
    NiRegisterStream(NiPSResetOnLoopCtlr);

    // Emitters.
    NiRegisterStream(NiPSBoxEmitter);
    NiRegisterStream(NiPSCylinderEmitter);
    NiRegisterStream(NiPSMeshEmitter);
    NiRegisterStream(NiPSSpawner);
    NiRegisterStream(NiPSSphereEmitter);

    // Forces.
    NiRegisterStream(NiPSAirFieldForce);
    NiRegisterStream(NiPSBombForce);
    NiRegisterStream(NiPSDragFieldForce);
    NiRegisterStream(NiPSDragForce);
    NiRegisterStream(NiPSGravityFieldForce);
    NiRegisterStream(NiPSGravityForce);
    NiRegisterStream(NiPSRadialFieldForce);
    NiRegisterStream(NiPSTurbulenceFieldForce);
    NiRegisterStream(NiPSVortexFieldForce);

    // Simulator steps.
    NiRegisterStream(NiPSSimulatorCollidersStep);
    NiRegisterStream(NiPSSimulatorFinalStep);
    NiRegisterStream(NiPSSimulatorForcesStep);
    NiRegisterStream(NiPSSimulatorGeneralStep);

    // Deprecated classes.
    NiRegisterStream(NiMeshParticleSystem);
    NiRegisterStream(NiMeshPSysData);
    NiRegisterStream(NiParticleSystem);
    NiRegisterStream(NiPSysAirFieldAirFrictionCtlr);
    NiRegisterStream(NiPSysAirFieldInheritVelocityCtlr);
    NiRegisterStream(NiPSysAirFieldModifier);
    NiRegisterStream(NiPSysAirFieldSpreadCtlr);
    NiRegisterStream(NiPSysAgeDeathModifier);
    NiRegisterStream(NiPSysBombModifier);
    NiRegisterStream(NiPSysBoundUpdateModifier);
    NiRegisterStream(NiPSysBoxEmitter);
    NiRegisterStream(NiPSysColliderManager);
    NiRegisterStream(NiPSysColorModifier);
    NiRegisterStream(NiPSysCylinderEmitter);
    NiRegisterStream(NiPSysData);
    NiRegisterStream(NiPSysDragFieldModifier);
    NiRegisterStream(NiPSysDragModifier);
    NiRegisterStream(NiPSysEmitterCtlr);
    NiRegisterStream(NiPSysEmitterCtlrData);
    NiRegisterStream(NiPSysEmitterDeclinationCtlr);
    NiRegisterStream(NiPSysEmitterDeclinationVarCtlr);
    NiRegisterStream(NiPSysEmitterInitialRadiusCtlr);
    NiRegisterStream(NiPSysEmitterLifeSpanCtlr);
    NiRegisterStream(NiPSysEmitterPlanarAngleCtlr);
    NiRegisterStream(NiPSysEmitterPlanarAngleVarCtlr);
    NiRegisterStream(NiPSysEmitterSpeedCtlr);
    NiRegisterStream(NiPSysFieldAttenuationCtlr);
    NiRegisterStream(NiPSysFieldMagnitudeCtlr);
    NiRegisterStream(NiPSysFieldMaxDistanceCtlr);
    NiRegisterStream(NiPSysGravityModifier);
    NiRegisterStream(NiPSysGravityFieldModifier);
    NiRegisterStream(NiPSysGravityStrengthCtlr);
    NiRegisterStream(NiPSysGrowFadeModifier);
    NiRegisterStream(NiPSysInitialRotAngleCtlr);
    NiRegisterStream(NiPSysInitialRotAngleVarCtlr);
    NiRegisterStream(NiPSysInitialRotSpeedCtlr);
    NiRegisterStream(NiPSysInitialRotSpeedVarCtlr);
    NiRegisterStream(NiPSysMeshEmitter);
    NiRegisterStream(NiPSysMeshUpdateModifier);
    NiRegisterStream(NiPSysModifierActiveCtlr);
    NiRegisterStream(NiPSysPlanarCollider);
    NiRegisterStream(NiPSysPositionModifier);
    NiRegisterStream(NiPSysRadialFieldModifier);
    NiRegisterStream(NiPSysResetOnLoopCtlr);
    NiRegisterStream(NiPSysRotationModifier);
    NiRegisterStream(NiPSysSpawnModifier);
    NiRegisterStream(NiPSysSphereEmitter);
    NiRegisterStream(NiPSysSphericalCollider);
    NiRegisterStream(NiPSysTurbulenceFieldModifier);
    NiRegisterStream(NiPSysUpdateCtlr);
    NiRegisterStream(NiPSysVortexFieldModifier);

    // Initialization functions.
    NiPSCommonSemantics::_SDMInit();

    NiStream::RegisterPostProcessFunction(NiPSConverter::Convert);
}
//---------------------------------------------------------------------------
void NiParticleSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiStream::UnregisterPostProcessFunction(NiPSConverter::Convert);

    // General classes.
    NiUnregisterStream(NiPSBoundUpdater);
    NiUnregisterStream(NiPSFacingQuadGenerator);
    NiUnregisterStream(NiPSMeshParticleSystem);
    NiUnregisterStream(NiPSParticleSystem);
    NiUnregisterStream(NiPSSimulator);

    // Colliders.
    NiUnregisterStream(NiPSPlanarCollider);
    NiUnregisterStream(NiPSSphericalCollider);

    // Controllers.
    NiUnregisterStream(NiPSAirFieldAirFrictionCtlr);
    NiUnregisterStream(NiPSAirFieldInheritedVelocityCtlr);
    NiUnregisterStream(NiPSAirFieldSpreadCtlr);
    NiUnregisterStream(NiPSEmitParticlesCtlr);
    NiUnregisterStream(NiPSEmitterDeclinationCtlr);
    NiUnregisterStream(NiPSEmitterDeclinationVarCtlr);
    NiUnregisterStream(NiPSEmitterLifeSpanCtlr);
    NiUnregisterStream(NiPSEmitterPlanarAngleCtlr);
    NiUnregisterStream(NiPSEmitterPlanarAngleVarCtlr);
    NiUnregisterStream(NiPSEmitterRadiusCtlr);
    NiUnregisterStream(NiPSEmitterRotAngleCtlr);
    NiUnregisterStream(NiPSEmitterRotAngleVarCtlr);
    NiUnregisterStream(NiPSEmitterRotSpeedCtlr);
    NiUnregisterStream(NiPSEmitterRotSpeedVarCtlr);
    NiUnregisterStream(NiPSEmitterSpeedCtlr);
    NiUnregisterStream(NiPSFieldAttenuationCtlr);
    NiUnregisterStream(NiPSFieldMagnitudeCtlr);
    NiUnregisterStream(NiPSFieldMaxDistanceCtlr);
    NiUnregisterStream(NiPSForceActiveCtlr);
    NiUnregisterStream(NiPSGravityStrengthCtlr);
    NiUnregisterStream(NiPSResetOnLoopCtlr);

    // Emitters.
    NiUnregisterStream(NiPSBoxEmitter);
    NiUnregisterStream(NiPSCylinderEmitter);
    NiUnregisterStream(NiPSMeshEmitter);
    NiUnregisterStream(NiPSSpawner);
    NiUnregisterStream(NiPSSphereEmitter);

    // Forces.
    NiUnregisterStream(NiPSAirFieldForce);
    NiUnregisterStream(NiPSBombForce);
    NiUnregisterStream(NiPSDragFieldForce);
    NiUnregisterStream(NiPSDragForce);
    NiUnregisterStream(NiPSGravityFieldForce);
    NiUnregisterStream(NiPSGravityForce);
    NiUnregisterStream(NiPSRadialFieldForce);
    NiUnregisterStream(NiPSTurbulenceFieldForce);
    NiUnregisterStream(NiPSVortexFieldForce);

    // Simulator steps.
    NiUnregisterStream(NiPSSimulatorCollidersStep);
    NiUnregisterStream(NiPSSimulatorFinalStep);
    NiUnregisterStream(NiPSSimulatorForcesStep);
    NiUnregisterStream(NiPSSimulatorGeneralStep);

    // Deprecated classes.
    NiUnregisterStream(NiMeshParticleSystem);
    NiUnregisterStream(NiMeshPSysData);
    NiUnregisterStream(NiParticleSystem);
    NiUnregisterStream(NiPSysAirFieldAirFrictionCtlr);
    NiUnregisterStream(NiPSysAirFieldInheritVelocityCtlr);
    NiUnregisterStream(NiPSysAirFieldModifier);
    NiUnregisterStream(NiPSysAirFieldSpreadCtlr);
    NiUnregisterStream(NiPSysAgeDeathModifier);
    NiUnregisterStream(NiPSysBombModifier);
    NiUnregisterStream(NiPSysBoundUpdateModifier);
    NiUnregisterStream(NiPSysBoxEmitter);
    NiUnregisterStream(NiPSysColliderManager);
    NiUnregisterStream(NiPSysColorModifier);
    NiUnregisterStream(NiPSysCylinderEmitter);
    NiUnregisterStream(NiPSysData);
    NiUnregisterStream(NiPSysDragFieldModifier);
    NiUnregisterStream(NiPSysDragModifier);
    NiUnregisterStream(NiPSysEmitterCtlr);
    NiUnregisterStream(NiPSysEmitterCtlrData);
    NiUnregisterStream(NiPSysEmitterDeclinationCtlr);
    NiUnregisterStream(NiPSysEmitterDeclinationVarCtlr);
    NiUnregisterStream(NiPSysEmitterInitialRadiusCtlr);
    NiUnregisterStream(NiPSysEmitterLifeSpanCtlr);
    NiUnregisterStream(NiPSysEmitterPlanarAngleCtlr);
    NiUnregisterStream(NiPSysEmitterPlanarAngleVarCtlr);
    NiUnregisterStream(NiPSysEmitterSpeedCtlr);
    NiUnregisterStream(NiPSysFieldAttenuationCtlr);
    NiUnregisterStream(NiPSysFieldMagnitudeCtlr);
    NiUnregisterStream(NiPSysFieldMaxDistanceCtlr);
    NiUnregisterStream(NiPSysGravityModifier);
    NiUnregisterStream(NiPSysGravityFieldModifier);
    NiUnregisterStream(NiPSysGravityStrengthCtlr);
    NiUnregisterStream(NiPSysGrowFadeModifier);
    NiUnregisterStream(NiPSysInitialRotAngleCtlr);
    NiUnregisterStream(NiPSysInitialRotAngleVarCtlr);
    NiUnregisterStream(NiPSysInitialRotSpeedCtlr);
    NiUnregisterStream(NiPSysInitialRotSpeedVarCtlr);
    NiUnregisterStream(NiPSysMeshEmitter);
    NiUnregisterStream(NiPSysMeshUpdateModifier);
    NiUnregisterStream(NiPSysModifierActiveCtlr);
    NiUnregisterStream(NiPSysPlanarCollider);
    NiUnregisterStream(NiPSysPositionModifier);
    NiUnregisterStream(NiPSysRadialFieldModifier);
    NiUnregisterStream(NiPSysResetOnLoopCtlr);
    NiUnregisterStream(NiPSysRotationModifier);
    NiUnregisterStream(NiPSysSpawnModifier);
    NiUnregisterStream(NiPSysSphereEmitter);
    NiUnregisterStream(NiPSysSphericalCollider);
    NiUnregisterStream(NiPSysTurbulenceFieldModifier);
    NiUnregisterStream(NiPSysUpdateCtlr);
    NiUnregisterStream(NiPSysVortexFieldModifier);

    // Shutdown functions.
    NiPSCommonSemantics::_SDMShutdown();
}
//---------------------------------------------------------------------------
