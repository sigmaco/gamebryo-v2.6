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

#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluidConverter.h"
#include "NiParticle.h"
#include <NiSequenceData.h>
#include <NiGeometryConverter.h>
#include <NiPhysXFluid.h>

// Deprecated classes.
#include "NiPhysXFluidEmitter.h"
#include "NiPhysXFluidSystem.h"
#include "NiPhysXFluidData.h"
#include "NiPhysXMeshFluidSystem.h"
#include "NiPhysXMeshFluidData.h"
#include "NiPhysXFluidSceneDesc.h"
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
void NiPhysXFluidConverter::Convert(NiStream& kStream, 
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

    NiPhysXScenePtr m_spPhysScene = 0;
    NiTObjectArray<NiPhysXPropPtr> kSceneProps;
    for (NiUInt32 i = 0; i < kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = kTopObjects.GetAt(i);
        if (NiIsKindOf(NiAVObject, pkObject))
        {
            ConvertSceneGraph(kStream, (NiAVObject*) pkObject,
                kOldToNewSpawnerMap);
        }

#if defined(WIN32)
        if (NiIsExactKindOf(NiPhysXScene, pkObject))
        {
            m_spPhysScene = (NiPhysXScene*)pkObject;

            // Manage back compatable file loading of Fluid scenes
            if (NiIsExactKindOf(NiPhysXFluidSceneDesc, 
                m_spPhysScene->GetSnapshot()))
            {
                NiPhysXFluidSceneDesc* pkFluidSceneDesc =
                    (NiPhysXFluidSceneDesc*)m_spPhysScene->GetSnapshot();
                pkFluidSceneDesc->ConvertToProp(m_spPhysScene);
                
                // Move the prop to the set of top level objects.
                for (NiUInt32 uj = 0; uj < m_spPhysScene->GetPropCount(); uj++)
                {
                    kSceneProps.Add(m_spPhysScene->GetPropAt(uj));
                }
            }
        }
#endif

    }

    // Add any scene props to the top level objects.
    for (NiUInt32 ui = 0; ui < kSceneProps.GetSize(); ui++)
    {
        NiPhysXPropPtr spProp = kSceneProps.GetAt(ui);
        NIASSERT(NiIsExactKindOf(NiPhysXFluidPropDesc, spProp->GetSnapshot()));
        NiObjectPtr spObject = NiDynamicCast(NiObject, spProp);
        kTopObjects.Add(spObject);
        m_spPhysScene->RemoveProp(spProp);
    }
    kSceneProps.RemoveAll();

    for (NiUInt32 ui = 0; ui < kTopObjects.GetSize(); ui++)
    {
        NiObject* pkObject = kTopObjects.GetAt(ui);
        if (NiIsExactKindOf(NiPhysXProp, pkObject))
        {
            NiPhysXProp* pkProp = (NiPhysXProp*)pkObject;
            ConvertNiPhysXProp(pkProp, kStream.GetConversionMap());

            if (NiIsExactKindOf(NiPhysXFluidPropDesc, pkProp->GetSnapshot()))
            {
                ConvertNiPhysXFluidDesc(
                    (NiPhysXFluidPropDesc*)pkProp->GetSnapshot(), 
                    kStream.GetConversionMap());     
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidConverter::ConvertSceneGraph(NiStream& kStream,
    NiAVObject* pkObject, 
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap)
{
    if (NiIsExactKindOf(NiPhysXFluidSystem, pkObject))
    {
        NiPhysXPSFluidSystem* pkNewFluid = ConvertOldFluid(
            (NiPhysXFluidSystem*)pkObject, kOldToNewSpawnerMap, 
            kStream.GetConversionMap(), kStream.GetPrepareMeshModifiers());
        NiNode* pkParent = pkObject->GetParent();
        NIASSERT(pkParent);

        kStream.GetConversionMap().SetAt(pkObject, pkNewFluid);

        pkParent->DetachChild(pkObject);
        pkParent->AttachChild(pkNewFluid);
    }
    else if (NiIsExactKindOf(NiPhysXMeshFluidSystem, pkObject))
    {
        NiPhysXPSMeshFluidSystem* pkNewFluid = ConvertOldMeshFluid(
            (NiPhysXMeshFluidSystem*) pkObject, kOldToNewSpawnerMap,
            kStream.GetConversionMap(), kStream.GetPrepareMeshModifiers());
        NiNode* pkParent = pkObject->GetParent();
        NIASSERT(pkParent);

        kStream.GetConversionMap().SetAt(pkObject, pkNewFluid);

        pkParent->DetachChild(pkObject);
        pkParent->AttachChild(pkNewFluid);
    }
    else if (NiIsKindOf(NiParticles, pkObject))
    {
        NILOG("NiPhysXFluidConverter WARNING! Unsupported NiParticles "
            "object found, skipping conversion.");
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
NiPhysXPSFluidSystem* NiPhysXFluidConverter::ConvertOldFluid(
    NiPhysXFluidSystem* pkOldFluid,
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
    bool)
{
    // Get NiPSysData object.
    NIASSERT(NiIsExactKindOf(NiPhysXFluidData, pkOldFluid->GetModelData()));
    NiPhysXFluidData* pkOldPSysData = 
        (NiPhysXFluidData*) pkOldFluid->GetModelData();

    // Create new particle system object.
    NiPhysXPSFluidSystem* pkNewFluid = NiPhysXPSFluidSystem::Create(0,
        pkOldPSysData->GetMaxNumParticles(),
        pkOldPSysData->GetColors() != NULL, 
        pkOldPSysData->GetRotationAngles() != NULL, 
        pkOldFluid->GetWorldSpace(), true, true, true, 0,
        pkOldFluid->GetPhysXSpace());

    NiGeometryConverter::ConvertCore(pkOldFluid, pkNewFluid);
    NiGeometryConverter::ConvertMaterials(pkOldFluid, pkNewFluid);

    ConvertPSysModifiers(pkOldFluid, pkNewFluid, kOldToNewSpawnerMap,
        kConversionMap);
    ConvertPSysControllers(pkOldFluid, pkNewFluid);

    return pkNewFluid;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem* NiPhysXFluidConverter::ConvertOldMeshFluid(
    NiPhysXMeshFluidSystem* pkOldFluid,
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
    bool)
{
    // Get NiMeshPSysData object.
    NIASSERT(NiIsExactKindOf(NiPhysXMeshFluidData, 
        pkOldFluid->GetModelData()));
    NiPhysXMeshFluidData* pkOldFluidData = (NiPhysXMeshFluidData*)
        pkOldFluid->GetModelData();

    // Find mesh update modifier if one exists.
    NiPSysMeshUpdateModifier* pkOldMeshUpdateModifier = NULL;
    const NiUInt32 uiModifierCount = pkOldFluid->GetModifierCount();
    for (NiUInt32 ui = 0; ui < uiModifierCount; ++ui)
    {
        pkOldMeshUpdateModifier = NiDynamicCast(NiPSysMeshUpdateModifier,
            pkOldFluid->GetModifierAt(ui));
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
        uiNumGenerations = pkOldFluidData->GetNumGenerations();
    }
    if (uiNumGenerations > (NiUInt16) -1)
    {
        NILOG("NiPSConverter: NiMeshParticleSystem has too many "
            "generations. They will be clamped to 65536.\n");
        uiNumGenerations = (NiUInt16) -1;
    }
    NiUInt16 usNumGenerations = (NiUInt16) uiNumGenerations;

    // Create new particle system object.
    NiPhysXPSMeshFluidSystem* pkNewFluid = 
        NiPhysXPSMeshFluidSystem::Create(0,
            pkOldFluid->GetMaxNumParticles(), 
            pkOldFluidData->GetColors() != NULL, 
            pkOldFluidData->GetRotationAngles() != NULL, 
            pkOldFluid->GetWorldSpace(), true, true,
            pkOldFluidData->GetDefaultPoolsSize(),
            usNumGenerations, pkOldFluidData->GetFillPoolsOnLoad(), 0,
            pkOldFluid->GetPhysXSpace());

    NiGeometryConverter::ConvertCore(pkOldFluid, pkNewFluid);
    NiGeometryConverter::ConvertMaterials(pkOldFluid, pkNewFluid);

    ConvertPSysModifiers(pkOldFluid, pkNewFluid, kOldToNewSpawnerMap,
        kConversionMap);
    ConvertPSysControllers(pkOldFluid, pkNewFluid);

    return pkNewFluid;
}
//---------------------------------------------------------------------------
void NiPhysXFluidConverter::ConvertPSysModifiers(NiParticleSystem* pkOldFluid, 
    NiPSParticleSystem* pkNewFluid, 
    NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    // Grab pointers to simulation steps (for use later during conversion).
    NiPSSimulatorGeneralStep* pkGeneralStep = NULL;
    NiPhysXFluidSimulatorStep* pkFluidStep = NULL;
    for (NiUInt32 ui = 0; ui < pkNewFluid->GetSimulator()->GetStepCount(); 
        ++ui)
    {
        NiPSSimulatorStep* pkStep = pkNewFluid->GetSimulator()->GetStepAt(ui);
        if (NiIsExactKindOf(NiPSSimulatorGeneralStep, pkStep))
        {
            pkGeneralStep = (NiPSSimulatorGeneralStep*) pkStep;
        }
        else if (NiIsExactKindOf(NiPhysXFluidSimulatorStep, pkStep))
        {
            pkFluidStep = (NiPhysXFluidSimulatorStep*) pkStep;
        }
    }
    NIASSERT(pkGeneralStep && pkFluidStep);

    // Find rotation modifier first. Its parameters must be pushed into
    // emitters in the new particle system.
    NiPSysRotationModifier* pkOldRotationModifier = NULL;
    for (NiUInt32 ui = 0; ui < pkOldFluid->GetModifierCount(); ++ui)
    {
        NiPSysModifier* pkOldModifier = pkOldFluid->GetModifierAt(ui);

        if (NiIsExactKindOf(NiPSysRotationModifier, pkOldModifier))
        {
            pkOldRotationModifier = (NiPSysRotationModifier*) pkOldModifier;
            break;
        }
    }

    // Iterate over modifiers, converting along the way.
    for (NiUInt32 uiModifier = 0; uiModifier < pkOldFluid->GetModifierCount();
        ++uiModifier)
    {
        NiPSysModifier* pkOldModifier = pkOldFluid->GetModifierAt(uiModifier);

        if (NiIsKindOf(NiPSysEmitter, pkOldModifier))
        {
            NiPSysEmitter* pkOldEmitter = (NiPSysEmitter*) pkOldModifier;

            NiPSEmitter* pkNewEmitter = NULL;

            if (NiIsExactKindOf(NiPhysXFluidEmitter, pkOldEmitter))
            {
                NiPhysXFluidEmitter* pkOldFluidEmitter = (NiPhysXFluidEmitter*)
                    pkOldEmitter;
                NiRGBA kInitialColor;
                pkOldFluidEmitter->GetInitialColor().GetAs(kInitialColor);
                pkNewEmitter = NiNew NiPhysXPSFluidEmitter(
                    pkOldFluidEmitter->GetName(), 
                    kInitialColor, pkOldFluidEmitter->GetInitialRadius(), 
                    pkOldFluidEmitter->GetRadiusVar());

                if (NiIsExactKindOf(NiPhysXPSFluidSystem, pkNewFluid))
                {
                    NiPhysXPSFluidSystem* pkFluidSystem = 
                        (NiPhysXPSFluidSystem*)pkNewFluid;
                    pkFluidSystem->SetFluidEmitter(
                        (NiPhysXPSFluidEmitter*)pkNewEmitter);
                }
                else if (NiIsExactKindOf(NiPhysXPSMeshFluidSystem, pkNewFluid))
                {
                    NiPhysXPSMeshFluidSystem* pkFluidSystem = 
                        (NiPhysXPSMeshFluidSystem*)pkNewFluid;
                    pkFluidSystem->SetFluidEmitter(
                        (NiPhysXPSFluidEmitter*)pkNewEmitter);
                }
            }
            else if (NiIsExactKindOf(NiPSysBoxEmitter, pkOldEmitter))
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
                pkNewFluid->AddEmitter(pkNewEmitter);
            }
        }
        else if (NiIsExactKindOf(NiPSysSpawnModifier, pkOldModifier))
        {
            NiPSysSpawnModifier* pkOldSpawnModifier = (NiPSysSpawnModifier*)
                pkOldModifier;

            NiPSSpawner* pkNewSpawner = ConvertOldSpawnModifier(
                pkOldSpawnModifier, kOldToNewSpawnerMap);
            NIASSERT(pkNewSpawner);

            pkNewFluid->AddSpawner(pkNewSpawner);
        }
        else if (NiIsExactKindOf(NiPSysBoundUpdateModifier, pkOldModifier))
        {
            NiPSysBoundUpdateModifier* pkOldBoundModifier =
                (NiPSysBoundUpdateModifier*) pkOldModifier;
            pkNewFluid->SetBoundUpdater(NiNew NiPSBoundUpdater(
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
                pkNewFluid->SetDeathSpawner(ConvertOldSpawnModifier(
                    pkOldAgeDeathModifier->GetSpawnModifier(),
                    kOldToNewSpawnerMap));
            }
        }
        else if (NiIsExactKindOf(NiPSysMeshUpdateModifier, pkOldModifier))
        {
            NiPSysMeshUpdateModifier* pkOldMeshUpdateModifier =
                (NiPSysMeshUpdateModifier*) pkOldModifier;

            NIASSERT(NiIsKindOf(NiPSMeshParticleSystem, pkNewFluid));
            NiPSMeshParticleSystem* pkNewMeshPSys = (NiPSMeshParticleSystem*)
                pkNewFluid;

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
void NiPhysXFluidConverter::ConvertPSysControllers(
    NiParticleSystem*, NiPSParticleSystem* pkNewFluid)
{
    // Emitter index used for certain controllers below.
    NiUInt32 uiCurEmitterIdx = 0;

    // Controllers have already been cloned to the new particle system. We
    // need to cull out unnecessary ones and replace old controllers with new
    // controllers.
    NiTObjectPtrSet<NiTimeControllerPtr> kNewCtlrs;
    NiTimeController* pkOldCtlr = pkNewFluid->GetControllers();
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
                    if (uiCurEmitterIdx < pkNewFluid->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewFluid->GetEmitterAt(
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
                    if (uiCurEmitterIdx < pkNewFluid->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewFluid->GetEmitterAt(
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
                    if (uiCurEmitterIdx < pkNewFluid->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewFluid->GetEmitterAt(
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
                    if (uiCurEmitterIdx < pkNewFluid->GetEmitterCount())
                    {
                        NiPSEmitter* pkEmitter = pkNewFluid->GetEmitterAt(
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
    pkNewFluid->RemoveAllControllers();

    // Add all new controllers. Iterate the array in reverse order since
    // SetTarget calls PrependController.
    for (NiUInt32 ui = kNewCtlrs.GetSize(); ui > 0; --ui)
    {
        kNewCtlrs.GetAt(ui - 1)->SetTarget(pkNewFluid);
    }
}
//---------------------------------------------------------------------------
NiPSSpawner* NiPhysXFluidConverter::ConvertOldSpawnModifier(
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
void NiPhysXFluidConverter::ConvertNiPhysXProp(NiPhysXProp* pkPhysXProp, 
    const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiUInt32 uiDestCount = pkPhysXProp->GetDestinationsCount();
    for (NiUInt32 ui = 0; ui < uiDestCount; ui++)
    {
        NiPhysXDest* pkDest = pkPhysXProp->GetDestinationAt(ui);
        if (NiIsExactKindOf(NiPhysXFluidDest, pkDest))
        {
            NiPhysXFluidDest* pkFluidDest = (NiPhysXFluidDest*)pkDest;
            NiAVObject* pkOldTarget = pkFluidDest->GetTarget();
            NiAVObject* pkNewTarget = 0;

            // Find the new target for the destination and set it.
            // Also set the fluid emitter on the destination and set the
            // destination on the fluid system.
            if (kConversionMap.GetAt(pkOldTarget, pkNewTarget))
            {
                NIASSERT(NiIsKindOf(NiPSParticleSystem, pkNewTarget));
                pkFluidDest->m_pkTarget = (NiPSParticleSystem*)pkNewTarget;

                NiPhysXPSFluidEmitter* pkFluidEmitter = 0;
                if (NiIsExactKindOf(NiPhysXPSMeshFluidSystem, pkNewTarget))
                {
                    NiPhysXPSMeshFluidSystem* pkFluidSystem = 
                        (NiPhysXPSMeshFluidSystem*)pkNewTarget;
                    pkFluidEmitter = pkFluidSystem->GetFluidEmitter();
                    pkFluidSystem->SetDestination(pkFluidDest);
                }
                else if (NiIsExactKindOf(NiPhysXPSFluidSystem, pkNewTarget))
                {
                    NiPhysXPSFluidSystem* pkFluidSystem = 
                        (NiPhysXPSFluidSystem*)pkNewTarget;
                    pkFluidEmitter = pkFluidSystem->GetFluidEmitter();
                    pkFluidSystem->SetDestination(pkFluidDest);
                }
                pkFluidDest->SetEmitter(pkFluidEmitter);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidConverter::ConvertNiPhysXFluidDesc(
    NiPhysXFluidPropDesc* pkFluidPropDesc,
    const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    for (NiUInt32 ui = 0; ui < pkFluidPropDesc->GetNumFluids(); ui++)
    {
        NiPhysXFluidDesc* pkDesc = pkFluidPropDesc->GetFluidDesc(ui);
        NiAVObject* pkOldTarget = pkDesc->GetParticleSystem();
        NiAVObject* pkNewTarget = 0;

        // Find the new target for the destination and set it.
        // Also set the fluid emitter on the destination and set the
        // destination on the fluid system.
        if (kConversionMap.GetAt(pkOldTarget, pkNewTarget))
        {
            NIASSERT(NiIsKindOf(NiPSParticleSystem, pkNewTarget));
            pkDesc->SetParticleSystem((NiPSParticleSystem*)pkNewTarget);
        }
    }
}
//---------------------------------------------------------------------------
