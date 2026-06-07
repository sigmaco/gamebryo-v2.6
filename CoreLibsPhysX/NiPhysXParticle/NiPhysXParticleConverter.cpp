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

#include "NiPhysXParticlePCH.h"

#include "NiPhysXParticleConverter.h"
#include "NiParticle.h"
#include <NiSequenceData.h>
#include <NiGeometryConverter.h>

#include "NiPhysXPSParticleSystem.h"
#include "NiPhysXPSMeshParticleSystem.h"
#include "NiPhysXPSParticleSystemProp.h"
#include "NiPhysXPSMeshParticleSystemProp.h"
#include "NiPhysXPSSimulatorFinalStep.h"
#include "NiPhysXPSSimulatorInitialStep.h"

// Deprecated classes.
#include "NiPhysXMeshParticleSystem.h"
#include "NiPhysXMeshPSysProp.h"
#include "NiPhysXParticleSystem.h"
#include "NiPhysXPSysData.h"
#include "NiPhysXPSysProp.h"
#include <NiMeshPSysData.h>
#include <NiParticleInfo.h>
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
#include <NiPSysModifierBoolCtlr.h>
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

//---------------------------------------------------------------------------
void NiPhysXParticleConverter::Convert(NiStream& kStream, 
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    // Only convert if the NIF file version is prior to 20.4.0.0.
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        return;
    }

    // Create map to link old spawn modifiers to new spawners. This 
    // map must be shared by all particle systems in the scene graph.
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*> kOldToNewSpawnerMap;
    
    // Store pointers to the old particle and meshparticle props.
    // After all old particle and meshparticle systems are converted the old
    // props will be converted.
    NiTPointerList<NiPhysXPSysProp*> kOldPhysXParticlesProps;
    NiTPointerList<NiPhysXMeshPSysProp*> kOldPhysXMeshParticlesProps;

    for (NiUInt32 i = 0; i < kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = kTopObjects.GetAt(i);
        if (NiIsKindOf(NiAVObject, pkObject))
        {
            ConvertSceneGraph(kStream, (NiAVObject*) pkObject,
                kOldToNewSpawnerMap);
        }
        else if (NiIsExactKindOf(NiPhysXPSysProp, pkObject))
        {
            kOldPhysXParticlesProps.AddTail(
                (NiPhysXPSysProp*)pkObject);
        }
        else if (NiIsExactKindOf(NiPhysXMeshPSysProp, pkObject))
        {
            kOldPhysXMeshParticlesProps.AddTail(
                (NiPhysXMeshPSysProp*)pkObject);
        }
    }

    // First iterate over the old NiPhysXPSysProps and convert to the new
    // NiPhysXPSParticleSystemProp.
    NiTListIterator kIterator = kOldPhysXParticlesProps.GetHeadPos();
    NiPhysXPSysProp* pkPhysXPSysProp = 0;
    while (kIterator)
    {
        pkPhysXPSysProp = kOldPhysXParticlesProps.GetNext(kIterator);

        NiPhysXPSParticleSystemProp* pkNewPsysProp = 
            ConvertPhysXParticlesProp(pkPhysXPSysProp, 
                kStream.GetConversionMap());

        NiUInt32 uiIndex = kTopObjects.Remove(pkPhysXPSysProp);
        kTopObjects.SetAt(uiIndex, pkNewPsysProp);
    }

    // Now iterate over the old NiPhysXMeshPSysProps and convert to the new
    // NiPhysXPSMeshParticleSystemProp.
    kIterator = kOldPhysXMeshParticlesProps.GetHeadPos();
    NiPhysXMeshPSysProp* pkPhysXMeshPSysProp = 0;
    while (kIterator)
    {
        pkPhysXMeshPSysProp = kOldPhysXMeshParticlesProps.GetNext(kIterator);

        NiPhysXPSMeshParticleSystemProp* pkNewMeshPsysProp = 
            ConvertPhysXMeshParticlesProp(pkPhysXMeshPSysProp, 
                kStream.GetConversionMap());

        NiUInt32 uiIndex = kTopObjects.Remove(pkPhysXMeshPSysProp);
        kTopObjects.SetAt(uiIndex, pkNewMeshPsysProp);
    }
}
//---------------------------------------------------------------------------
void NiPhysXParticleConverter::ConvertSceneGraph(
    NiStream& kStream, NiAVObject* pkObject, 
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap)
{
    if (NiIsExactKindOf(NiPhysXParticleSystem, pkObject))
    {
        NiPhysXPSParticleSystem* pkNewPSys = 
            NiPhysXParticleConverter::ConvertOldPSys(
            (NiPhysXParticleSystem*)pkObject, kOldToNewSpawnerMap, 
            kStream.GetConversionMap(), kStream.GetPrepareMeshModifiers());
        NiNode* pkParent = pkObject->GetParent();
        NIASSERT(pkParent);

        pkParent->DetachChild(pkObject);
        pkParent->AttachChild(pkNewPSys);
    }
    else if (NiIsExactKindOf(NiPhysXMeshParticleSystem, pkObject))
    {
        NiPhysXPSMeshParticleSystem* pkNewPSys = ConvertOldMeshPSys(
            (NiPhysXMeshParticleSystem*) pkObject, kOldToNewSpawnerMap,
            kStream.GetConversionMap(), kStream.GetPrepareMeshModifiers());
        NiNode* pkParent = pkObject->GetParent();
        NIASSERT(pkParent);

        pkParent->DetachChild(pkObject);
        pkParent->AttachChild(pkNewPSys);
    }
    else if (NiIsKindOf(NiParticles, pkObject))
    {
        NILOG("NiPhysXParticleConverter WARNING! Unsupported NiParticles "
            " object found, skipping conversion.");
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        // Recurse over children.
        NiNode* pkNode = (NiNode*) pkObject;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ++ui)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                ConvertSceneGraph(kStream, pkChild, kOldToNewSpawnerMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem* NiPhysXParticleConverter::ConvertOldPSys(
    NiPhysXParticleSystem* pkOldPSys,
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
    bool bAttachModifiers)
{
    // Get NiPSysData object.
    NIASSERT(NiIsExactKindOf(NiPhysXPSysData, pkOldPSys->GetModelData()));
    NiPhysXPSysData* pkOldPSysData = 
        (NiPhysXPSysData*) pkOldPSys->GetModelData();

    // Create new particle system object.
    NiPhysXPSParticleSystem* pkNewPSys = NiPhysXPSParticleSystem::Create(
        pkOldPSysData->GetMaxNumParticles(),
        pkOldPSysData->GetColors() != NULL,
        pkOldPSysData->GetRotationAngles() != NULL,
        pkOldPSys->GetWorldSpace(),
        false,
        true,
        bAttachModifiers,
        0,
        0,
        pkOldPSys->GetPhysXSpace(),
        pkOldPSysData->GetRotationAngles() != NULL,
        pkOldPSysData->GetDefaultActorPoolSize(),
        pkOldPSysData->GetFillActorPoolOnLoad(),
        pkOldPSysData->GetKeepsActorMeshes());

    kConversionMap.SetAt(pkOldPSys, pkNewPSys);

    NiGeometryConverter::ConvertCore(pkOldPSys, pkNewPSys);
    NiGeometryConverter::ConvertMaterials(pkOldPSys, pkNewPSys);

    ConvertPSysModifiers(pkOldPSys, pkNewPSys, kOldToNewSpawnerMap,
        kConversionMap);
    ConvertPSysControllers(pkOldPSys, pkNewPSys);

    pkNewPSys->SetActorDesc(pkOldPSysData->GetActorDesc());

    return pkNewPSys;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystem* NiPhysXParticleConverter::ConvertOldMeshPSys(
    NiPhysXMeshParticleSystem* pkOldPSys,
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
    bool bAttachModifiers)
{
    // Get NiMeshPSysData object.
    NIASSERT(NiIsExactKindOf(NiPhysXMeshPSysData, pkOldPSys->GetModelData()));
    NiPhysXMeshPSysData* pkOldPSysData = (NiPhysXMeshPSysData*)
        pkOldPSys->GetModelData();

    // Find mesh update modifier if one exists.
    NiPSysMeshUpdateModifier* pkOldMeshUpdateModifier = NULL;
    const NiUInt32 uiModifierCount = pkOldPSys->GetModifierCount();
    for (NiUInt32 ui = 0; ui < uiModifierCount; ++ui)
    {
        pkOldMeshUpdateModifier = NiDynamicCast(NiPSysMeshUpdateModifier,
            pkOldPSys->GetModifierAt(ui));
        if (pkOldMeshUpdateModifier)
        {
            break;
        }
    }

    // Get number of generations.
    NiUInt32 uiNumGenerations;
    if (pkOldMeshUpdateModifier)
    {
        uiNumGenerations = pkOldMeshUpdateModifier->GetMeshCount();
    }
    else
    {
        uiNumGenerations = pkOldPSysData->GetNumGenerations();
    }
    if (uiNumGenerations > (NiUInt16) -1)
    {
        NILOG("NiPSConverter: NiMeshParticleSystem has too many "
            "generations. They will be clamped to 65536.\n");
        uiNumGenerations = (NiUInt16) -1;
    }
    NiUInt16 usNumGenerations = (NiUInt16) uiNumGenerations;

    // Create new particle system object.
    NiPhysXPSMeshParticleSystem* pkNewPSys = 
        NiPhysXPSMeshParticleSystem::Create(pkOldPSys->GetMaxNumParticles(),
            pkOldPSysData->GetColors() != NULL,
            pkOldPSysData->GetRotationAngles() != NULL,
            pkOldPSys->GetWorldSpace(),
            false,
            bAttachModifiers,
            pkOldPSysData->GetDefaultPoolsSize(),
            usNumGenerations,
            pkOldPSysData->GetFillPoolsOnLoad(),
            0,
            0,
            pkOldPSys->GetPhysXSpace(),
            pkOldPSys->GetUsesPhysXRotations(),
            pkOldPSysData->GetDefaultActorPoolsSize(),
            pkOldPSysData->GetFillActorPoolsOnLoad(),
            pkOldPSysData->GetKeepsActorMeshes());

    kConversionMap.SetAt(pkOldPSys, pkNewPSys);

    NiGeometryConverter::ConvertCore(pkOldPSys, pkNewPSys);
    NiGeometryConverter::ConvertMaterials(pkOldPSys, pkNewPSys);

    ConvertPSysModifiers(pkOldPSys, pkNewPSys, kOldToNewSpawnerMap,
        kConversionMap);
    ConvertPSysControllers(pkOldPSys, pkNewPSys);

    for (NiUInt32 ui = 0; ui < pkOldPSysData->GetActorDescCount(); ui++)
    {
        pkNewPSys->SetActorDescAt(ui, pkOldPSysData->GetActorDescAt(ui));
    }

    return pkNewPSys;
}
//---------------------------------------------------------------------------
void NiPhysXParticleConverter::ConvertPSysModifiers(
    NiParticleSystem* pkOldPSys, NiPSParticleSystem* pkNewPSys,
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    // Grab pointers to simulation steps (for use later during conversion).
    NiPhysXPSSimulatorInitialStep* pkPhysXInitialStep = NULL;
    NiPhysXPSSimulatorFinalStep* pkPhysXFinalStep = NULL;
    NiPSSimulatorGeneralStep* pkGeneralStep = NULL;
    NiPSSimulatorForcesStep* pkForcesStep = NULL;
    NiPSSimulatorCollidersStep* pkCollidersStep = NULL;
    for (NiUInt32 ui = 0; ui < pkNewPSys->GetSimulator()->GetStepCount(); ++ui)
    {
        NiPSSimulatorStep* pkStep = pkNewPSys->GetSimulator()->GetStepAt(ui);
        if (NiIsExactKindOf(NiPhysXPSSimulatorInitialStep, pkStep))
        {
            pkPhysXInitialStep = (NiPhysXPSSimulatorInitialStep*) pkStep;
        }
        else if (NiIsExactKindOf(NiPhysXPSSimulatorFinalStep, pkStep))
        {
            pkPhysXFinalStep = (NiPhysXPSSimulatorFinalStep*) pkStep;
        }
        else if (NiIsExactKindOf(NiPSSimulatorGeneralStep, pkStep))
        {
            pkGeneralStep = (NiPSSimulatorGeneralStep*) pkStep;
        }
        else if (NiIsExactKindOf(NiPSSimulatorForcesStep, pkStep))
        {
            pkForcesStep = (NiPSSimulatorForcesStep*) pkStep;
        }
        else if (NiIsExactKindOf(NiPSSimulatorCollidersStep, pkStep))
        {
            pkCollidersStep = (NiPSSimulatorCollidersStep*) pkStep;
        }
    }
    NIASSERT(pkPhysXInitialStep && pkPhysXFinalStep && pkGeneralStep && 
        pkForcesStep && pkCollidersStep);

    // Find rotation modifier first. Its parameters must be pushed into
    // emitters in the new particle system.
    NiPSysRotationModifier* pkOldRotationModifier = NULL;
    for (NiUInt32 ui = 0; ui < pkOldPSys->GetModifierCount(); ++ui)
    {
        NiPSysModifier* pkOldModifier = pkOldPSys->GetModifierAt(ui);

        if (NiIsExactKindOf(NiPSysRotationModifier, pkOldModifier))
        {
            pkOldRotationModifier = (NiPSysRotationModifier*) pkOldModifier;
            break;
        }
    }

    // Iterate over modifiers, converting along the way.
    for (NiUInt32 uiModifier = 0; uiModifier < pkOldPSys->GetModifierCount();
        ++uiModifier)
    {
        NiPSysModifier* pkOldModifier = pkOldPSys->GetModifierAt(uiModifier);

        if (NiIsKindOf(NiPSysEmitter, pkOldModifier))
        {
            NiPSysEmitter* pkOldEmitter = (NiPSysEmitter*) pkOldModifier;

            NiPSEmitter* pkNewEmitter = NULL;
            if (NiIsExactKindOf(NiPSysBoxEmitter, pkOldEmitter))
            {
                NiPSysBoxEmitter* pkOldBoxEmitter = (NiPSysBoxEmitter*)
                    pkOldEmitter;
                NiRGBA kInitialColor;
                pkOldBoxEmitter->GetInitialColor().GetAs(kInitialColor);
                pkNewEmitter = NiNew NiPSBoxEmitter(
                    pkOldBoxEmitter->GetName(),
                    pkOldBoxEmitter->GetEmitterWidth(),
                    pkOldBoxEmitter->GetEmitterHeight(),
                    pkOldBoxEmitter->GetEmitterDepth(),
                    LookupObject(pkOldBoxEmitter->GetEmitterObj(),
                        kConversionMap),
                    pkOldBoxEmitter->GetSpeed(),
                    pkOldBoxEmitter->GetSpeedVar(),
                    pkOldBoxEmitter->GetDeclination(),
                    pkOldBoxEmitter->GetDeclinationVar(),
                    pkOldBoxEmitter->GetPlanarAngle(),
                    pkOldBoxEmitter->GetPlanarAngleVar(),
                    kInitialColor,
                    pkOldBoxEmitter->GetInitialRadius(),
                    pkOldBoxEmitter->GetRadiusVar(),
                    pkOldBoxEmitter->GetLifeSpan(),
                    pkOldBoxEmitter->GetLifeSpanVar());
            }
            else if (NiIsExactKindOf(NiPSysCylinderEmitter, pkOldEmitter))
            {
                NiPSysCylinderEmitter* pkOldCylinderEmitter =
                    (NiPSysCylinderEmitter*) pkOldEmitter;
                NiRGBA kInitialColor;
                pkOldCylinderEmitter->GetInitialColor().GetAs(kInitialColor);
                pkNewEmitter = NiNew NiPSCylinderEmitter(
                    pkOldCylinderEmitter->GetName(),
                    pkOldCylinderEmitter->GetEmitterRadius(),
                    pkOldCylinderEmitter->GetEmitterHeight(),
                    LookupObject(pkOldCylinderEmitter->GetEmitterObj(),
                        kConversionMap),
                    pkOldCylinderEmitter->GetSpeed(),
                    pkOldCylinderEmitter->GetSpeedVar(),
                    pkOldCylinderEmitter->GetDeclination(),
                    pkOldCylinderEmitter->GetDeclinationVar(),
                    pkOldCylinderEmitter->GetPlanarAngle(),
                    pkOldCylinderEmitter->GetPlanarAngleVar(),
                    kInitialColor,
                    pkOldCylinderEmitter->GetInitialRadius(),
                    pkOldCylinderEmitter->GetRadiusVar(),
                    pkOldCylinderEmitter->GetLifeSpan(),
                    pkOldCylinderEmitter->GetLifeSpanVar());
            }
            else if (NiIsExactKindOf(NiPSysSphereEmitter, pkOldEmitter))
            {
                NiPSysSphereEmitter* pkOldSphereEmitter =
                    (NiPSysSphereEmitter*) pkOldEmitter;
                NiRGBA kInitialColor;
                pkOldSphereEmitter->GetInitialColor().GetAs(kInitialColor);
                pkNewEmitter = NiNew NiPSSphereEmitter(
                    pkOldSphereEmitter->GetName(),
                    pkOldSphereEmitter->GetEmitterRadius(),
                    LookupObject(pkOldSphereEmitter->GetEmitterObj(),
                        kConversionMap),
                    pkOldSphereEmitter->GetSpeed(),
                    pkOldSphereEmitter->GetSpeedVar(),
                    pkOldSphereEmitter->GetDeclination(),
                    pkOldSphereEmitter->GetDeclinationVar(),
                    pkOldSphereEmitter->GetPlanarAngle(),
                    pkOldSphereEmitter->GetPlanarAngleVar(),
                    kInitialColor,
                    pkOldSphereEmitter->GetInitialRadius(),
                    pkOldSphereEmitter->GetRadiusVar(),
                    pkOldSphereEmitter->GetLifeSpan(),
                    pkOldSphereEmitter->GetLifeSpanVar());
            }
            else if (NiIsExactKindOf(NiPSysMeshEmitter, pkOldEmitter))
            {
                NiPSysMeshEmitter* pkOldMeshEmitter = (NiPSysMeshEmitter*)
                    pkOldEmitter;

                NiPSMeshEmitter::MeshEmissionType eNewMeshEmissionType;
                switch (pkOldMeshEmitter->GetMeshEmissionType())
                {
                    case NiPSysMeshEmitter::NI_EMIT_FROM_VERTICES:
                        eNewMeshEmissionType =
                            NiPSMeshEmitter::EMIT_FROM_VERTICES;
                        break;
                    case NiPSysMeshEmitter::NI_EMIT_FROM_FACE_CENTER:
                        eNewMeshEmissionType =
                            NiPSMeshEmitter::EMIT_FROM_FACE_CENTER;
                        break;
                    case NiPSysMeshEmitter::NI_EMIT_FROM_EDGE_CENTER:
                        eNewMeshEmissionType =
                            NiPSMeshEmitter::EMIT_FROM_EDGE_CENTER;
                        break;
                    case NiPSysMeshEmitter::NI_EMIT_FROM_FACE_SURFACE:
                        eNewMeshEmissionType =
                            NiPSMeshEmitter::EMIT_FROM_FACE_SURFACE;
                        break;
                    case NiPSysMeshEmitter::NI_EMIT_FROM_EDGE_SURFACE:
                        eNewMeshEmissionType =
                            NiPSMeshEmitter::EMIT_FROM_EDGE_SURFACE;
                        break;
                    default:
                        NIASSERT(!"Unknown MeshEmissionType!");
                        eNewMeshEmissionType =
                            NiPSMeshEmitter::EMIT_FROM_VERTICES;
                        break;
                }

                NiPSMeshEmitter::InitialVelocityType eNewInitialVelocityType;
                switch (pkOldMeshEmitter->GetInitialVelocityType())
                {
                    case NiPSysMeshEmitter::NI_VELOCITY_USE_NORMALS:
                        eNewInitialVelocityType =
                            NiPSMeshEmitter::VELOCITY_USE_NORMALS;
                        break;
                    case NiPSysMeshEmitter::NI_VELOCITY_USE_RANDOM:
                        eNewInitialVelocityType =
                            NiPSMeshEmitter::VELOCITY_USE_RANDOM;
                        break;
                    case NiPSysMeshEmitter::NI_VELOCITY_USE_DIRECTION:
                        eNewInitialVelocityType =
                            NiPSMeshEmitter::VELOCITY_USE_DIRECTION;
                        break;
                    default:
                        NIASSERT(!"Unknown InitialVelocityType!");
                        eNewInitialVelocityType =
                            NiPSMeshEmitter::VELOCITY_USE_NORMALS;
                        break;
                }

                NiRGBA kInitialColor;
                pkOldMeshEmitter->GetInitialColor().GetAs(kInitialColor);

                NiPSMeshEmitter* pkNewMeshEmitter = NiNew NiPSMeshEmitter(
                    pkOldMeshEmitter->GetName(),
                    NULL,
                    eNewMeshEmissionType,
                    eNewInitialVelocityType,
                    pkOldMeshEmitter->GetEmitAxis(),
                    pkOldMeshEmitter->GetSpeed(),
                    pkOldMeshEmitter->GetSpeedVar(),
                    pkOldMeshEmitter->GetDeclination(),
                    pkOldMeshEmitter->GetDeclinationVar(),
                    pkOldMeshEmitter->GetPlanarAngle(),
                    pkOldMeshEmitter->GetPlanarAngleVar(),
                    kInitialColor,
                    pkOldMeshEmitter->GetInitialRadius(),
                    pkOldMeshEmitter->GetRadiusVar(),
                    pkOldMeshEmitter->GetLifeSpan(),
                    pkOldMeshEmitter->GetLifeSpanVar());

                const NiUInt32 uiGeomEmitterCount =
                    pkOldMeshEmitter->GetGeometryEmitterCount();
                for (NiUInt32 uiGeomEmitter = 0;
                    uiGeomEmitter < uiGeomEmitterCount; ++uiGeomEmitter)
                {
                    NiGeometry* pkOldGeometryEmitter =
                        pkOldMeshEmitter->GetGeometryEmitter(uiGeomEmitter);
                    NiAVObject* pkNewGeometryEmitter =
                        LookupObject(pkOldGeometryEmitter, kConversionMap);
                    NIASSERT(pkNewGeometryEmitter != pkOldGeometryEmitter);
                    NIASSERT(NiIsKindOf(NiMesh, pkNewGeometryEmitter));
                    NIVERIFY(pkNewMeshEmitter->AddMeshEmitter((NiMesh*)
                        pkNewGeometryEmitter));
                }

                pkNewEmitter = pkNewMeshEmitter;
            }

            if (pkNewEmitter)
            {
                // Copy old rotation modifier parameters.
                if (pkOldRotationModifier)
                {
                    pkNewEmitter->SetRotAngle(
                        pkOldRotationModifier->GetInitialRotAngle());
                    pkNewEmitter->SetRotAngleVar(
                        pkOldRotationModifier->GetInitialRotAngleVar());
                    pkNewEmitter->SetRotSpeed(
                        pkOldRotationModifier->GetInitialRotSpeed());
                    pkNewEmitter->SetRotSpeedVar(
                        pkOldRotationModifier->GetInitialRotSpeedVar());
                    pkNewEmitter->SetRandomRotSpeedSign(
                        pkOldRotationModifier->GetRandomRotSpeedSign());
                    pkNewEmitter->SetRotAxis(
                        pkOldRotationModifier->GetInitialAxis());
                    pkNewEmitter->SetRandomRotAxis(
                        pkOldRotationModifier->GetRandomInitialAxis());
                }

                // Add new emitter to particle system.
                pkNewPSys->AddEmitter(pkNewEmitter);
            }
        }
        else if (NiIsExactKindOf(NiPSysSpawnModifier, pkOldModifier))
        {
            NiPSysSpawnModifier* pkOldSpawnModifier = (NiPSysSpawnModifier*)
                pkOldModifier;

            NiPSSpawner* pkNewSpawner = ConvertOldSpawnModifier(
                pkOldSpawnModifier, kOldToNewSpawnerMap);
            NIASSERT(pkNewSpawner);

            pkNewPSys->AddSpawner(pkNewSpawner);
        }
        else if (NiIsExactKindOf(NiPSysBoundUpdateModifier, pkOldModifier))
        {
            NiPSysBoundUpdateModifier* pkOldBoundModifier =
                (NiPSysBoundUpdateModifier*) pkOldModifier;
            pkNewPSys->SetBoundUpdater(NiNew NiPSBoundUpdater(
                pkOldBoundModifier->GetUpdateSkip()));
        }
        else if (NiIsExactKindOf(NiPSysGrowFadeModifier, pkOldModifier))
        {
            NiPSysGrowFadeModifier* pkOldGrowFadeModifier =
                (NiPSysGrowFadeModifier*) pkOldModifier;

            pkGeneralStep->SetGrowTime(pkOldGrowFadeModifier->GetGrowTime());
            pkGeneralStep->SetShrinkTime(pkOldGrowFadeModifier->GetFadeTime());
            pkGeneralStep->SetGrowGeneration(
                pkOldGrowFadeModifier->GetGrowGeneration());
            pkGeneralStep->SetShrinkGeneration(
                pkOldGrowFadeModifier->GetFadeGeneration());
        }
        else if (NiIsExactKindOf(NiPSysColorModifier, pkOldModifier))
        {
            NiPSysColorModifier* pkOldColorModifier = (NiPSysColorModifier*)
                pkOldModifier;

            NiColorData* pkOldColorData = pkOldColorModifier->GetColorData();
            if (pkOldColorData)
            {
                NiUInt32 uiOldNumColorKeys;
                NiAnimationKey::KeyType eOldKeyType;
                NiUInt8 ucOldKeySize;
                NiColorKey* pkOldColorKeys = pkOldColorData->GetAnim(
                    uiOldNumColorKeys, eOldKeyType, ucOldKeySize);
                if (uiOldNumColorKeys > 0)
                {
                    // Old key array must have less than 253 keys. This leaves
                    // room to add proper begin and end keys, if necessary.
                    NIASSERT(uiOldNumColorKeys <= (NiUInt8) -3);

                    NiUInt8 ucNewNumColorKeys = (NiUInt8) uiOldNumColorKeys;

                    float fOldBeginKeyTime = pkOldColorKeys->GetKeyAt(0,
                        ucOldKeySize)->GetTime();
                    float fOldEndKeyTime = pkOldColorKeys->GetKeyAt(
                        ucNewNumColorKeys - 1, ucOldKeySize)->GetTime();

                    // Old key times must be within the range of 0.0 to 1.0.
                    NIASSERT(fOldBeginKeyTime >= 0.0f &&
                        fOldEndKeyTime <= 1.0f);

                    // The start and end keys must have times of 0.0 and 1.0,
                    // respectively. If the old color keys do not, additional
                    // keys must be added to provide clamping behavior.
                    if (fOldBeginKeyTime > 0.0f)
                    {
                        ++ucNewNumColorKeys;
                    }
                    if (fOldEndKeyTime < 1.0f)
                    {
                        ++ucNewNumColorKeys;
                    }

                    NiPSKernelColorKey* pkNewColorKeys = NiAlloc(
                        NiPSKernelColorKey, ucNewNumColorKeys);

                    NiUInt8 ucNewIndex = 0;
                    if (fOldBeginKeyTime > 0.0f)
                    {
                        pkNewColorKeys[ucNewIndex].m_fTime = 0.0f;
                        pkOldColorKeys->GetKeyAt(0, ucOldKeySize)->GetColor()
                            .GetAs(pkNewColorKeys[ucNewIndex].m_kColor);
                        ++ucNewIndex;
                    }

                    for (NiUInt8 ucOldIndex = 0;
                        ucOldIndex < uiOldNumColorKeys; ++ucOldIndex)
                    {
                        NiColorKey* pkOldKey = pkOldColorKeys->GetKeyAt(
                            ucOldIndex, ucOldKeySize);

                        pkNewColorKeys[ucNewIndex].m_fTime =
                            pkOldKey->GetTime();
                        pkOldKey->GetColor().GetAs(
                            pkNewColorKeys[ucNewIndex].m_kColor);
                        ++ucNewIndex;
                    }

                    if (fOldEndKeyTime < 1.0f)
                    {
                        pkNewColorKeys[ucNewIndex].m_fTime = 1.0f;
                        pkNewColorKeys[ucNewIndex].m_kColor =
                            pkNewColorKeys[ucNewIndex - 1].m_kColor;
                    }

                    pkGeneralStep->CopyColorKeys(pkNewColorKeys,
                        ucNewNumColorKeys);

                    NiFree(pkNewColorKeys);
                }
            }
        }
        else if (NiIsExactKindOf(NiPSysAgeDeathModifier, pkOldModifier))
        {
            NiPSysAgeDeathModifier* pkOldAgeDeathModifier =
                (NiPSysAgeDeathModifier*) pkOldModifier;

            if (pkOldAgeDeathModifier->GetSpawnOnDeath() &&
                pkOldAgeDeathModifier->GetSpawnModifier())
            {
                pkNewPSys->SetDeathSpawner(ConvertOldSpawnModifier(
                    pkOldAgeDeathModifier->GetSpawnModifier(),
                    kOldToNewSpawnerMap));
            }
        }
        else if (NiIsExactKindOf(NiPSysBombModifier, pkOldModifier))
        {
            NiPSysBombModifier* pkOldBombModifier = (NiPSysBombModifier*)
                pkOldModifier;

            NiPSForceDefinitions::BombData::DecayType eNewDecayType;
            switch (pkOldBombModifier->GetDecayType())
            {
                case NiPSysBombModifier::NONE:
                    eNewDecayType = NiPSForceDefinitions::BombData::DECAY_NONE;
                    break;
                case NiPSysBombModifier::LINEAR:
                    eNewDecayType =
                        NiPSForceDefinitions::BombData::DECAY_LINEAR;
                    break;
                case NiPSysBombModifier::EXPONENTIAL:
                    eNewDecayType =
                        NiPSForceDefinitions::BombData::DECAY_EXPONENTIAL;
                    break;
                default:
                    NIASSERT(!"Unknown DecayType!");
                    eNewDecayType = NiPSForceDefinitions::BombData::DECAY_NONE;
                    break;
            }

            NiPSForceDefinitions::BombData::SymmType eNewSymmType;
            switch (pkOldBombModifier->GetSymmType())
            {
                case NiPSysBombModifier::SPHERICAL:
                    eNewSymmType =
                        NiPSForceDefinitions::BombData::SYMM_SPHERICAL;
                    break;
                case NiPSysBombModifier::CYLINDRICAL:
                    eNewSymmType =
                        NiPSForceDefinitions::BombData::SYMM_CYLINDRICAL;
                    break;
                case NiPSysBombModifier::PLANAR:
                    eNewSymmType =
                        NiPSForceDefinitions::BombData::SYMM_PLANAR;
                    break;
                default:
                    NIASSERT(!"Unknown SymmType!");
                    eNewSymmType =
                        NiPSForceDefinitions::BombData::SYMM_SPHERICAL;
                    break;
            }

            NiPSBombForce* pkNewBombForce = NiNew NiPSBombForce(
                pkOldBombModifier->GetName(),
                LookupObject(pkOldBombModifier->GetBombObj(), kConversionMap),
                pkOldBombModifier->GetBombAxis(),
                pkOldBombModifier->GetDecay(),
                pkOldBombModifier->GetDeltaV(),
                eNewDecayType,
                eNewSymmType);

            pkForcesStep->AddForce(pkNewBombForce);
        }
        else if (NiIsExactKindOf(NiPSysGravityModifier, pkOldModifier))
        {
            NiPSysGravityModifier* pkOldGravityModifier =
                (NiPSysGravityModifier*) pkOldModifier;

            NiPSForceDefinitions::GravityData::ForceType eNewForceType;
            switch (pkOldGravityModifier->GetType())
            {
                case NiPSysGravityModifier::FORCE_PLANAR:
                    eNewForceType =
                        NiPSForceDefinitions::GravityData::FORCE_PLANAR;
                    break;
                case NiPSysGravityModifier::FORCE_SPHERICAL:
                    eNewForceType =
                        NiPSForceDefinitions::GravityData::FORCE_SPHERICAL;
                    break;
                default:
                    NIASSERT(!"Unknown ForceType!");
                    eNewForceType =
                        NiPSForceDefinitions::GravityData::FORCE_PLANAR;
                    break;
            }

            NiPSGravityForce* pkNewGravityForce = NiNew NiPSGravityForce(
                pkOldGravityModifier->GetName(),
                LookupObject(pkOldGravityModifier->GetGravityObj(),
                    kConversionMap),
                pkOldGravityModifier->GetGravityAxis(),
                pkOldGravityModifier->GetDecay(),
                pkOldGravityModifier->GetStrength(),
                eNewForceType,
                pkOldGravityModifier->GetTurbulence(),
                pkOldGravityModifier->GetTurbulenceScale());

            pkForcesStep->AddForce(pkNewGravityForce);
        }
        else if (NiIsExactKindOf(NiPSysDragModifier, pkOldModifier))
        {
            NiPSysDragModifier* pkOldDragModifier = (NiPSysDragModifier*)
                pkOldModifier;

            NiPSDragForce* pkNewDragForce = NiNew NiPSDragForce(
                pkOldDragModifier->GetName(),
                LookupObject(pkOldDragModifier->GetDragObj(), kConversionMap),
                pkOldDragModifier->GetDragAxis(),
                pkOldDragModifier->GetPercentage(),
                pkOldDragModifier->GetRange(),
                pkOldDragModifier->GetRangeFalloff());

            pkForcesStep->AddForce(pkNewDragForce);
        }
        else if (NiIsExactKindOf(NiPSysDragFieldModifier, pkOldModifier))
        {
            NiPSysDragFieldModifier* pkOldDragFieldModifier =
                (NiPSysDragFieldModifier*) pkOldModifier;

            NiPSDragFieldForce* pkNewDragFieldForce = NiNew NiPSDragFieldForce(
                pkOldDragFieldModifier->GetName(),
                LookupObject(pkOldDragFieldModifier->GetFieldObj(),
                    kConversionMap),
                pkOldDragFieldModifier->GetMagnitude(),
                pkOldDragFieldModifier->GetAttenuation(),
                pkOldDragFieldModifier->GetUseMaxDistance(),
                pkOldDragFieldModifier->GetMaxDistance(),
                pkOldDragFieldModifier->GetUseDirection(),
                pkOldDragFieldModifier->GetDirection());

            pkForcesStep->AddForce(pkNewDragFieldForce);
        }
        else if (NiIsExactKindOf(NiPSysGravityFieldModifier, pkOldModifier))
        {
            NiPSysGravityFieldModifier* pkOldGravityFieldModifier =
                (NiPSysGravityFieldModifier*) pkOldModifier;

            NiPSGravityFieldForce* pkNewGravityFieldForce = NiNew
                NiPSGravityFieldForce(
                    pkOldGravityFieldModifier->GetName(),
                    LookupObject(pkOldGravityFieldModifier->GetFieldObj(),
                        kConversionMap),
                    pkOldGravityFieldModifier->GetMagnitude(),
                    pkOldGravityFieldModifier->GetAttenuation(),
                    pkOldGravityFieldModifier->GetUseMaxDistance(),
                    pkOldGravityFieldModifier->GetMaxDistance(),
                    pkOldGravityFieldModifier->GetDirection());

            pkForcesStep->AddForce(pkNewGravityFieldForce);
        }
        else if (NiIsExactKindOf(NiPSysRadialFieldModifier, pkOldModifier))
        {
            NiPSysRadialFieldModifier* pkOldRadialFieldModifier =
                (NiPSysRadialFieldModifier*) pkOldModifier;

            NiPSRadialFieldForce* pkNewRadialFieldForce = NiNew
                NiPSRadialFieldForce(
                    pkOldRadialFieldModifier->GetName(),
                    LookupObject(pkOldRadialFieldModifier->GetFieldObj(),
                        kConversionMap),
                    pkOldRadialFieldModifier->GetMagnitude(),
                    pkOldRadialFieldModifier->GetAttenuation(),
                    pkOldRadialFieldModifier->GetUseMaxDistance(),
                    pkOldRadialFieldModifier->GetMaxDistance(),
                    pkOldRadialFieldModifier->GetRadialType());

            pkForcesStep->AddForce(pkNewRadialFieldForce);
        }
        else if (NiIsExactKindOf(NiPSysVortexFieldModifier, pkOldModifier))
        {
            NiPSysVortexFieldModifier* pkOldVortexFieldModifier =
                (NiPSysVortexFieldModifier*) pkOldModifier;

            NiPSVortexFieldForce* pkNewVortexFieldForce = NiNew
                NiPSVortexFieldForce(
                    pkOldVortexFieldModifier->GetName(),
                    LookupObject(pkOldVortexFieldModifier->GetFieldObj(),
                        kConversionMap),
                    pkOldVortexFieldModifier->GetMagnitude(),
                    pkOldVortexFieldModifier->GetAttenuation(),
                    pkOldVortexFieldModifier->GetUseMaxDistance(),
                    pkOldVortexFieldModifier->GetMaxDistance(),
                    pkOldVortexFieldModifier->GetDirection());

            pkForcesStep->AddForce(pkNewVortexFieldForce);
        }
        else if (NiIsExactKindOf(NiPSysAirFieldModifier, pkOldModifier))
        {
            NiPSysAirFieldModifier* pkOldAirFieldModifier =
                (NiPSysAirFieldModifier*) pkOldModifier;

            NiPSAirFieldForce* pkNewAirFieldForce = NiNew NiPSAirFieldForce(
                pkOldAirFieldModifier->GetName(),
                LookupObject(pkOldAirFieldModifier->GetFieldObj(),
                    kConversionMap),
                pkOldAirFieldModifier->GetMagnitude(),
                pkOldAirFieldModifier->GetAttenuation(),
                pkOldAirFieldModifier->GetUseMaxDistance(),
                pkOldAirFieldModifier->GetMaxDistance(),
                pkOldAirFieldModifier->GetDirection(),
                pkOldAirFieldModifier->GetAirFriction(),
                pkOldAirFieldModifier->GetInheritVelocity(),
                pkOldAirFieldModifier->GetInheritRotation(),
                pkOldAirFieldModifier->GetEnableSpread(),
                pkOldAirFieldModifier->GetSpread());

            pkForcesStep->AddForce(pkNewAirFieldForce);
        }
        else if (NiIsExactKindOf(NiPSysTurbulenceFieldModifier, pkOldModifier))
        {
            NiPSysTurbulenceFieldModifier* pkOldTurbulenceFieldModifier =
                (NiPSysTurbulenceFieldModifier*) pkOldModifier;

            NiPSTurbulenceFieldForce* pkNewTurbulenceFieldForce = NiNew
                NiPSTurbulenceFieldForce(
                    pkOldTurbulenceFieldModifier->GetName(),
                    LookupObject(pkOldTurbulenceFieldModifier->GetFieldObj(),
                        kConversionMap),
                    pkOldTurbulenceFieldModifier->GetMagnitude(),
                    pkOldTurbulenceFieldModifier->GetAttenuation(),
                    pkOldTurbulenceFieldModifier->GetUseMaxDistance(),
                    pkOldTurbulenceFieldModifier->GetMaxDistance(),
                    pkOldTurbulenceFieldModifier->GetFrequency());

            pkForcesStep->AddForce(pkNewTurbulenceFieldForce);
        }
        else if (NiIsExactKindOf(NiPSysColliderManager, pkOldModifier))
        {
            NiPSysColliderManager* pkOldColliderManager =
                (NiPSysColliderManager*) pkOldModifier;
            
            NiPSysCollider* pkCollider = pkOldColliderManager->GetColliders();
            while (pkCollider)
            {
                if (NiIsExactKindOf(NiPSysPlanarCollider, pkCollider))
                {
                    NiPSysPlanarCollider* pkOldPlanarCollider =
                        (NiPSysPlanarCollider*) pkCollider;

                    NiPSPlanarCollider* pkNewPlanarCollider = NiNew
                        NiPSPlanarCollider(
                            pkOldPlanarCollider->GetBounce(),
                            pkOldPlanarCollider->GetSpawnOnCollide(),
                            pkOldPlanarCollider->GetDieOnCollide(),
                            ConvertOldSpawnModifier(
                                pkOldPlanarCollider->GetSpawnModifier(),
                                kOldToNewSpawnerMap),
                            LookupObject(pkOldPlanarCollider->GetColliderObj(),
                                kConversionMap),
                            pkOldPlanarCollider->GetWidth(),
                            pkOldPlanarCollider->GetHeight(),
                            pkOldPlanarCollider->GetXAxis(),
                            pkOldPlanarCollider->GetYAxis());

                    pkCollidersStep->AddCollider(pkNewPlanarCollider);
                }
                else if (NiIsExactKindOf(NiPSysSphericalCollider, pkCollider))
                {
                    NiPSysSphericalCollider* pkOldSphericalCollider =
                        (NiPSysSphericalCollider*) pkCollider;

                    NiPSSphericalCollider* pkNewSphericalCollider = NiNew
                        NiPSSphericalCollider(
                            pkOldSphericalCollider->GetBounce(),
                            pkOldSphericalCollider->GetSpawnOnCollide(),
                            pkOldSphericalCollider->GetDieOnCollide(),
                            ConvertOldSpawnModifier(
                                pkOldSphericalCollider->GetSpawnModifier(),
                                kOldToNewSpawnerMap),
                            LookupObject(
                                pkOldSphericalCollider->GetColliderObj(),
                                kConversionMap),
                            pkOldSphericalCollider->GetRadius());

                    pkCollidersStep->AddCollider(pkNewSphericalCollider);
                }

                pkCollider = pkCollider->GetNext();
            }
        }
        else if (NiIsExactKindOf(NiPSysMeshUpdateModifier, pkOldModifier))
        {
            NiPSysMeshUpdateModifier* pkOldMeshUpdateModifier =
                (NiPSysMeshUpdateModifier*) pkOldModifier;

            NIASSERT(NiIsKindOf(NiPSMeshParticleSystem, pkNewPSys));
            NiPSMeshParticleSystem* pkNewMeshPSys = (NiPSMeshParticleSystem*)
                pkNewPSys;

            // Retrieve old master particles.
            NiUInt32 uiMasterParticlesCount =
                pkOldMeshUpdateModifier->GetMeshCount();
            NiNodePtr spDummyParent = NiNew NiNode(uiMasterParticlesCount);
            for (NiUInt32 ui = 0; ui < uiMasterParticlesCount; ++ui)
            {
                NiAVObject* pkOldMasterParticle =
                    pkOldMeshUpdateModifier->GetMeshAt(ui);
                NIASSERT(pkOldMasterParticle);
                spDummyParent->AttachChild(pkOldMasterParticle);
            }

            // Create geometry converter.
            const bool bForce32BitIndices = false;
            const bool bIsRuntime = true;
            NiGeometryConverter kConverter(bForce32BitIndices, bIsRuntime);

            // Convert old master particles.
            NiTLargeObjectArray<NiObjectPtr> kTopLevelObjects;
            kConverter.RecursiveConvert(spDummyParent, kTopLevelObjects, 0,
                kConversionMap);

            // Add new master particles.
            for (NiUInt16 us = 0; us < uiMasterParticlesCount; ++us)
            {
                NiAVObjectPtr spNewMasterParticle =
                    spDummyParent->DetachChildAt(us);
                NIASSERT(spNewMasterParticle);
                pkNewMeshPSys->SetMasterParticle(us, spNewMasterParticle);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXParticleConverter::ConvertPSysControllers(
    NiParticleSystem*, NiPSParticleSystem* pkNewPSys)
{
    // Emitter index used for certain controllers below.
    NiUInt32 uiCurEmitterIdx = 0;

    // Controllers have already been cloned to the new particle system. We
    // need to cull out unnecessary ones and replace old controllers with new
    // controllers.
    NiTObjectPtrSet<NiTimeControllerPtr> kNewCtlrs;
    NiTimeController* pkOldCtlr = pkNewPSys->GetControllers();
    while (pkOldCtlr)
    {
        NiTimeController* pkNewCtlr = NULL;
        bool bMoveToNextCtlr = true;

        if (NiIsKindOf(NiInterpController, pkOldCtlr))
        {
            NiInterpController* pkOldInterpCtlr = (NiInterpController*)
                pkOldCtlr;

            NiInterpController* pkNewInterpCtlr = NULL;
            if (NiIsExactKindOf(NiPSysEmitterCtlr, pkOldInterpCtlr))
            {
                pkNewInterpCtlr = NiNew NiPSEmitParticlesCtlr(
                    ((NiPSysEmitterCtlr*) pkOldInterpCtlr)->GetModifierName());
            }
            else if (NiIsKindOf(NiPSysModifierCtlr, pkOldInterpCtlr))
            {
                if (NiIsExactKindOf(NiPSysModifierActiveCtlr, pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSForceActiveCtlr(
                        ((NiPSysModifierActiveCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysAirFieldAirFrictionCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSAirFieldAirFrictionCtlr(
                        ((NiPSysAirFieldAirFrictionCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysAirFieldInheritVelocityCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSAirFieldInheritedVelocityCtlr(
                        ((NiPSysAirFieldInheritVelocityCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysAirFieldSpreadCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSAirFieldSpreadCtlr(
                        ((NiPSysAirFieldSpreadCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterDeclinationCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterDeclinationCtlr(
                        ((NiPSysEmitterDeclinationCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterDeclinationVarCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterDeclinationVarCtlr(
                        ((NiPSysEmitterDeclinationVarCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterInitialRadiusCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterRadiusCtlr(
                        ((NiPSysEmitterInitialRadiusCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterLifeSpanCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterLifeSpanCtlr(
                        ((NiPSysEmitterLifeSpanCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterPlanarAngleCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterPlanarAngleCtlr(
                        ((NiPSysEmitterPlanarAngleCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterPlanarAngleVarCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterPlanarAngleVarCtlr(
                        ((NiPSysEmitterPlanarAngleVarCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysEmitterSpeedCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSEmitterSpeedCtlr(
                        ((NiPSysEmitterSpeedCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysFieldAttenuationCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSFieldAttenuationCtlr(
                        ((NiPSysFieldAttenuationCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysFieldMagnitudeCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSFieldMagnitudeCtlr(
                        ((NiPSysFieldMagnitudeCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysFieldMaxDistanceCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSFieldMaxDistanceCtlr(
                        ((NiPSysFieldMaxDistanceCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysGravityStrengthCtlr,
                    pkOldInterpCtlr))
                {
                    pkNewInterpCtlr = NiNew NiPSGravityStrengthCtlr(
                        ((NiPSysGravityStrengthCtlr*) pkOldInterpCtlr)
                        ->GetModifierName());
                }
                else if (NiIsExactKindOf(NiPSysInitialRotAngleCtlr,
                    pkOldInterpCtlr))
                {
                    if (uiCurEmitterIdx < pkNewPSys->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewPSys->GetEmitterAt(
                            uiCurEmitterIdx++);
                        NIASSERT(pkEmitter);
                        pkNewInterpCtlr = NiNew NiPSEmitterRotAngleCtlr(
                            pkEmitter->GetName());
                        bMoveToNextCtlr = false;
                    }
                }
                else if (NiIsExactKindOf(NiPSysInitialRotAngleVarCtlr,
                    pkOldInterpCtlr))
                {
                    if (uiCurEmitterIdx < pkNewPSys->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewPSys->GetEmitterAt(
                            uiCurEmitterIdx++);
                        NIASSERT(pkEmitter);
                        pkNewInterpCtlr = NiNew NiPSEmitterRotAngleVarCtlr(
                            pkEmitter->GetName());
                        bMoveToNextCtlr = false;
                    }
                }
                else if (NiIsExactKindOf(NiPSysInitialRotSpeedCtlr,
                    pkOldInterpCtlr))
                {
                    if (uiCurEmitterIdx < pkNewPSys->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewPSys->GetEmitterAt(
                            uiCurEmitterIdx++);
                        NIASSERT(pkEmitter);
                        pkNewInterpCtlr = NiNew NiPSEmitterRotSpeedCtlr(
                            pkEmitter->GetName());
                        bMoveToNextCtlr = false;
                    }
                }
                else if (NiIsExactKindOf(NiPSysInitialRotSpeedVarCtlr,
                    pkOldInterpCtlr))
                {
                    if (uiCurEmitterIdx < pkNewPSys->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewPSys->GetEmitterAt(
                            uiCurEmitterIdx++);
                        NIASSERT(pkEmitter);
                        pkNewInterpCtlr = NiNew NiPSEmitterRotSpeedVarCtlr(
                            pkEmitter->GetName());
                        bMoveToNextCtlr = false;
                    }
                }
                else
                {
                    NIASSERT(!"Unknown NiPSysModifierCtlr class!");
                }
            }
            else
            {
                // Not a particle system controller, so keep it on the new
                // particle system.
                pkNewInterpCtlr = pkOldInterpCtlr;
            }

            if (pkNewInterpCtlr)
            {
                if (pkNewInterpCtlr != pkOldInterpCtlr)
                {
                    const NiUInt16 usInterpCount =
                        pkOldInterpCtlr->GetInterpolatorCount();
                    for (NiUInt16 us = 0; us < usInterpCount; ++us)
                    {
                        pkNewInterpCtlr->SetInterpolator(
                            pkOldInterpCtlr->GetInterpolator(us), us);
                    }

                    pkNewInterpCtlr->SetManagerControlled(
                        pkOldInterpCtlr->GetManagerControlled());
                }

                pkNewCtlr = pkNewInterpCtlr;
            }
        }
        else if (NiIsExactKindOf(NiPSysResetOnLoopCtlr, pkOldCtlr))
        {
            pkNewCtlr = NiNew NiPSResetOnLoopCtlr();
        }
        else if (NiIsExactKindOf(NiPSysUpdateCtlr, pkOldCtlr))
        {
            // Do not add this controller to the new particle system.
        }
        else
        {
            // Not a particle system controller, so keep it on the new
            // particle system.
            pkNewCtlr = pkOldCtlr;
        }

        if (pkNewCtlr)
        {
            if (pkNewCtlr != pkOldCtlr)
            {
                pkNewCtlr->SetAnimType(pkOldCtlr->GetAnimType());
                pkNewCtlr->SetCycleType(pkOldCtlr->GetCycleType());
                pkNewCtlr->SetPlayBackwards(pkOldCtlr->GetPlayBackwards());
                pkNewCtlr->SetFrequency(pkOldCtlr->GetFrequency());
                pkNewCtlr->SetPhase(pkOldCtlr->GetPhase());
                pkNewCtlr->SetBeginKeyTime(pkOldCtlr->GetBeginKeyTime());
                pkNewCtlr->SetEndKeyTime(pkOldCtlr->GetEndKeyTime());
                pkNewCtlr->SetActive(pkOldCtlr->GetActive());
            }

            kNewCtlrs.Add(pkNewCtlr);
        }

        if (bMoveToNextCtlr)
        {
            pkOldCtlr = pkOldCtlr->GetNext();
            uiCurEmitterIdx = 0;
        }
    }

    // Remove all old controllers.
    pkNewPSys->RemoveAllControllers();

    // Add all new controllers. Iterate the array in reverse order since
    // SetTarget calls PrependController.
    for (NiUInt32 ui = kNewCtlrs.GetSize(); ui > 0; --ui)
    {
        kNewCtlrs.GetAt(ui - 1)->SetTarget(pkNewPSys);
    }
}
//---------------------------------------------------------------------------
NiPSSpawner* NiPhysXParticleConverter::ConvertOldSpawnModifier(
    NiPSysSpawnModifier* pkOldSpawnModifier,
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap)
{
    if (!pkOldSpawnModifier)
    {
        return NULL;
    }

    NiPSSpawner* pkNewSpawner;
    if (!kOldToNewSpawnerMap.GetAt(pkOldSpawnModifier, pkNewSpawner))
    {
        pkNewSpawner = NiNew NiPSSpawner(
            pkOldSpawnModifier->GetNumSpawnGenerations(),
            pkOldSpawnModifier->GetPercentageSpawned(),
            pkOldSpawnModifier->GetMinNumToSpawn(),
            pkOldSpawnModifier->GetMaxNumToSpawn(),
            pkOldSpawnModifier->GetSpawnSpeedChaos(),
            pkOldSpawnModifier->GetSpawnDirChaos(),
            pkOldSpawnModifier->GetLifeSpan(),
            pkOldSpawnModifier->GetLifeSpanVar());
        kOldToNewSpawnerMap.SetAt(pkOldSpawnModifier, pkNewSpawner);
    }
    NIASSERT(pkNewSpawner);

    return pkNewSpawner;
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystemProp* 
NiPhysXParticleConverter::ConvertPhysXParticlesProp(
    NiPhysXPSysProp* pkPhysXPSysProp, 
    const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiPhysXPSParticleSystemProp* pkNewPhysXPSysProp = 
        NiNew NiPhysXPSParticleSystemProp();

    NiPhysXParticleSystem* pkOldSystem = 0;
    NiAVObject* pkNewSystem = 0;
    for (NiUInt32 ui = 0; ui < pkPhysXPSysProp->GetSystemCount(); ui++)
    {
        pkOldSystem = pkPhysXPSysProp->GetSystemAt(ui);
        NIASSERT(pkOldSystem);
        NIVERIFY(kConversionMap.GetAt(pkOldSystem, pkNewSystem));
        NIASSERT(NiIsKindOf(NiPhysXPSParticleSystem, pkNewSystem));
        pkNewPhysXPSysProp->AddSystem((NiPhysXPSParticleSystem*)pkNewSystem);

        // Relink and copy the old props datamembers to the new prop.

        // Copy scale factor PToW.
        pkNewPhysXPSysProp->SetScaleFactor(pkPhysXPSysProp->GetScaleFactor());

        // Copy over the sources, since the old prop keeps pointer to old
        // NiPhysXParticleSystems, find the newly created one and add that one
        // to the new NiPhysXPSParticleSystemProp.
        NiAVObject* pkNewParticleSystem = 0;
        for (NiUInt32 uiIdx = 0; uiIdx < pkPhysXPSysProp->GetSourcesCount();
            uiIdx++)
        {
            NiPhysXSrc* pkSrc = pkPhysXPSysProp->GetSourceAt(uiIdx);
            if (NiIsKindOf(NiPhysXPSysSrc, pkSrc))
            {
                NiPhysXPSysSrc* pkOldSrc = (NiPhysXPSysSrc*)pkSrc;

                NIVERIFY(kConversionMap.GetAt(pkOldSrc->GetSource(), 
                    pkNewParticleSystem));
                NIASSERT(NiIsKindOf(NiPhysXPSParticleSystem, 
                    pkNewParticleSystem));

                NiPhysXPSParticleSystemDest* pkNewDest = 
                    NiNew NiPhysXPSParticleSystemDest(
                        (NiPhysXPSParticleSystem*)pkNewParticleSystem);
                pkNewPhysXPSysProp->AddDestination(pkNewDest);
            }
        }

        // No need to copy over the old sources since those aren't used
        // any more in the new NiPhysXPSParticleSystem.

        // Copy over the keep meshes bool.
        pkNewPhysXPSysProp->SetKeepMeshes(pkPhysXPSysProp->GetKeepMeshes());

        // Copy over the snapshot.
        pkNewPhysXPSysProp->SetSnapshot(pkPhysXPSysProp->GetSnapshot());
    }

    return pkNewPhysXPSysProp;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshParticleSystemProp* 
NiPhysXParticleConverter::ConvertPhysXMeshParticlesProp(
    NiPhysXMeshPSysProp* pkPhysXMeshPSysProp, 
    const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiPhysXPSMeshParticleSystemProp* pkNewPhysXMeshPSysProp =
        NiNew NiPhysXPSMeshParticleSystemProp();

    NiPhysXMeshParticleSystem* pkOldSystem = 0;
    NiAVObject* pkNewSystem = 0;
    for (NiUInt32 ui = 0; ui < pkPhysXMeshPSysProp->GetSystemCount(); ui++)
    {
        pkOldSystem = pkPhysXMeshPSysProp->GetSystemAt(ui);
        NIASSERT(pkOldSystem);
        NIVERIFY(kConversionMap.GetAt(pkOldSystem, pkNewSystem));
        NIASSERT(NiIsKindOf(NiPhysXPSMeshParticleSystem, pkNewSystem));
        pkNewPhysXMeshPSysProp->AddSystem(
            (NiPhysXPSMeshParticleSystem*)pkNewSystem);


        // Relink and copy the old props datamembers to the new prop.

        // Copy scale factor PToW.
        pkNewPhysXMeshPSysProp->SetScaleFactor(
            pkPhysXMeshPSysProp->GetScaleFactor());

        // Copy over the sources, since the old prop keeps pointer to old
        // NiPhysXMeshParticleSystems, find the newly created one and add that 
        // one to the new NiPhysXPSMeshParticleSystemProp.
        NiAVObject* pkNewParticleSystem = 0;
        for (NiUInt32 uiIdx = 0; uiIdx <
            pkPhysXMeshPSysProp->GetSourcesCount(); uiIdx++)
        {
            NiPhysXSrc* pkSrc = pkPhysXMeshPSysProp->GetSourceAt(uiIdx);
            if (NiIsKindOf(NiPhysXMeshPSysSrc, pkSrc))
            {
                NiPhysXMeshPSysSrc* pkOldSrc = (NiPhysXMeshPSysSrc*)pkSrc;

                NIVERIFY(kConversionMap.GetAt(pkOldSrc->GetSource(), 
                    pkNewParticleSystem));
                NIASSERT(NiIsKindOf(NiPhysXPSParticleSystem, 
                    pkNewParticleSystem));

                NiPhysXPSParticleSystemDest* pkNewDest = 
                    NiNew NiPhysXPSParticleSystemDest(
                        (NiPhysXPSParticleSystem*)pkNewParticleSystem);
                pkNewPhysXMeshPSysProp->AddDestination(pkNewDest);
            }
        }

        // No need to copy over the old sources since those aren't used
        // any more in the new NiPhysXPSMeshParticleSystem.

        // Copy over the keep meshes bool.
        pkNewPhysXMeshPSysProp->SetKeepMeshes(
            pkPhysXMeshPSysProp->GetKeepMeshes());

        // Copy over the snapshot.
        pkNewPhysXMeshPSysProp->SetSnapshot(
            pkPhysXMeshPSysProp->GetSnapshot());
    }

    return pkNewPhysXMeshPSysProp;
}
//---------------------------------------------------------------------------
