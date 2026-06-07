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

#ifndef NIPARTICLE_H
#define NIPARTICLE_H

// DLL support
#include "NiParticleLibType.h"

// General classes.
#include "NiPSBoundUpdater.h"
#include "NiPSCommonSemantics.h"
#include "NiPSConverter.h"
#include "NiPSFacingQuadGenerator.h"
#include "NiPSFlagsHelpers.h"
#include "NiPSKernelDefinitions.h"
#include "NiPSMeshParticleSystem.h"
#include "NiPSMetrics.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulator.h"

// Kernels.
#include <NiPSFacingQuadGeneratorKernel.h>
#include <NiPSSimulatorCollidersKernel.h>
#include <NiPSSimulatorFinalKernel.h>
#include <NiPSSimulatorForcesKernel.h>
#include <NiPSSimulatorGeneralKernel.h>

// Colliders.
#include "NiPSCollider.h"
#include "NiPSColliderDefinitions.h"
#include "NiPSColliderHelpers.h"
#include "NiPSPlanarCollider.h"
#include "NiPSSphericalCollider.h"

// Controllers.
#include "NiPSAirFieldAirFrictionCtlr.h"
#include "NiPSAirFieldInheritedVelocityCtlr.h"
#include "NiPSAirFieldSpreadCtlr.h"
#include "NiPSEmitParticlesCtlr.h"
#include "NiPSEmitterCtlr.h"
#include "NiPSEmitterDeclinationCtlr.h"
#include "NiPSEmitterDeclinationVarCtlr.h"
#include "NiPSEmitterFloatCtlr.h"
#include "NiPSEmitterLifeSpanCtlr.h"
#include "NiPSEmitterPlanarAngleCtlr.h"
#include "NiPSEmitterPlanarAngleVarCtlr.h"
#include "NiPSEmitterRadiusCtlr.h"
#include "NiPSEmitterRotAngleCtlr.h"
#include "NiPSEmitterRotAngleVarCtlr.h"
#include "NiPSEmitterRotSpeedCtlr.h"
#include "NiPSEmitterRotSpeedVarCtlr.h"
#include "NiPSEmitterSpeedCtlr.h"
#include "NiPSFieldAttenuationCtlr.h"
#include "NiPSFieldMagnitudeCtlr.h"
#include "NiPSFieldMaxDistanceCtlr.h"
#include "NiPSForceActiveCtlr.h"
#include "NiPSForceBoolCtlr.h"
#include "NiPSForceCtlr.h"
#include "NiPSForceFloatCtlr.h"
#include "NiPSGravityStrengthCtlr.h"
#include "NiPSResetOnLoopCtlr.h"

// Emitters.
#include "NiPSBoxEmitter.h"
#include "NiPSCylinderEmitter.h"
#include "NiPSEmitter.h"
#include "NiPSMeshEmitter.h"
#include "NiPSSpawner.h"
#include "NiPSSphereEmitter.h"
#include "NiPSVolumeEmitter.h"

// Forces.
#include "NiPSAirFieldForce.h"
#include "NiPSBombForce.h"
#include "NiPSDragFieldForce.h"
#include "NiPSDragForce.h"
#include "NiPSFieldForce.h"
#include "NiPSForce.h"
#include "NiPSForceDefinitions.h"
#include "NiPSForceHelpers.h"
#include "NiPSGravityFieldForce.h"
#include "NiPSGravityForce.h"
#include "NiPSRadialFieldForce.h"
#include "NiPSTurbulenceFieldForce.h"
#include "NiPSVortexFieldForce.h"

// Simulator steps.
#include "NiPSSimulatorCollidersStep.h"
#include "NiPSSimulatorFinalStep.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSSimulatorKernelHelpers.h"
#include "NiPSSimulatorGeneralStep.h"
#include "NiPSSimulatorStep.h"

// Static data manager.
#include "NiParticleSDM.h"
static NiParticleSDM NiParticleSDMObject;

#endif  // #ifndef NIPARTICLE_H
