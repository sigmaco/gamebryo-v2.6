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

#include "NiPhysXPSFluidSystem.h"
#include "NiPhysXFluidDest.h"
#include "NiPhysXFluidPropDesc.h"
#include "NiPhysXFluidSimulatorStep.h"
#include "NiPhysXFluidEmitter.h"
#include <NiFloatInterpolator.h>

NiImplementRTTI(NiPhysXPSFluidSystem, NiPSParticleSystem);

//---------------------------------------------------------------------------
NiPhysXPSFluidSystem::~NiPhysXPSFluidSystem()
{
    // Must do this here so that the object is still alive to
    // complete simulation.
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteModifiers(&kSyncArgs);

    // We need to remove the simulator modifier now, while the particle
    // system simulator still exists to detach.
    NiMeshModifier* pkSimulator =
        GetModifierByType(&NiPhysXPSSimulator::ms_RTTI);
    RemoveModifier(pkSimulator);

    m_spFluidEmitter = 0;
    m_spDestination = 0;

    OnDeletion();
}
//---------------------------------------------------------------------------
NiPhysXPSFluidSystem* NiPhysXPSFluidSystem::Create(
    NiPhysXPSFluidEmitter* pkEmitter,
    NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations,
    bool bWorldSpace, bool bDynamicBounds, bool bCreateDefaultGenerator,
    bool bAttachMeshModifiers, NiPhysXProp* pkProp, bool bPhysXSpace, 
    float fExpansionFactor)
{
    // Create simulator.
    NiPhysXPSSimulator* pkSimulator = NiNew NiPhysXPSSimulator();

    // Add simulation steps.
    pkSimulator->AddStep(NiNew NiPSSimulatorGeneralStep());
    pkSimulator->AddStep(NiNew NiPhysXFluidSimulatorStep());

    // Create bound updater, if requested.
    NiPSBoundUpdater* pkBoundUpdater = NULL;
    if (bDynamicBounds)
    {
        pkBoundUpdater = NiNew NiPSBoundUpdater();
    }

    NiPhysXPSFluidSystem* pkPhysXFluidSystem = NiNew NiPhysXPSFluidSystem(
        pkEmitter, pkSimulator, 
        NiUInt32(uiMaxNumParticles * fExpansionFactor), bHasColors, 
        bHasRotations, bWorldSpace, pkBoundUpdater, pkProp, bPhysXSpace,
        fExpansionFactor);

    // Add simulator.
    pkPhysXFluidSystem->AddModifier(pkSimulator, bAttachMeshModifiers);
    
    // Create and add default generator, if requested.
    if (bCreateDefaultGenerator)
    {
        pkPhysXFluidSystem->AddModifier(NiNew NiPSFacingQuadGenerator(),
            bAttachMeshModifiers);
    }

    return pkPhysXFluidSystem;
}
//---------------------------------------------------------------------------
NiPhysXPSFluidSystem* NiPhysXPSFluidSystem::Create(NiPSParticleSystem* pkPSys,
    NiPhysXProp* pkProp, bool bPhysXSpace, 
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap, 
    float fExpansionFactor)
{
    NiPhysXPSFluidSystem* pkFluidSystem = NiNew NiPhysXPSFluidSystem(pkPSys,
        pkProp, bPhysXSpace, kActorMap, fExpansionFactor);

    return pkFluidSystem;
}
//---------------------------------------------------------------------------
NiPhysXPSFluidSystem::NiPhysXPSFluidSystem(NiPhysXPSFluidEmitter* pkEmitter,
    NiPhysXPSSimulator* pkSimulator,
    NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations,
    bool bWorldSpace, NiPSBoundUpdater* pkBoundUpdater, NiPhysXProp* pkProp,
    bool bPhysXSpace, float fExpansionFactor) : NiPSParticleSystem(
    pkSimulator,
    NiUInt32(uiMaxNumParticles * fExpansionFactor), bHasColors, bHasRotations,
    bHasRotations, bWorldSpace, pkBoundUpdater)
{
    m_uiMaxNumPhysXParticles = uiMaxNumParticles;
    m_bPhysXSpace = bPhysXSpace;

    m_spFluidEmitter = pkEmitter;

    // Set up sources and destinations to get data into and out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
    else
    {
        m_spDestination = 0;
    }
    OnCreation();
}
//---------------------------------------------------------------------------
NiPhysXPSFluidSystem::NiPhysXPSFluidSystem(NiPSParticleSystem* pkPSys,
    NiPhysXProp* pkProp, bool bPhysXSpace, 
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap,
    float fExpansionFactor) : NiPSParticleSystem()
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    ((NiPhysXPSFluidSystem*)pkPSys)->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);

    // Detach all modifiers and then delete all of the datastreamsrefs.
    // The datastreamsrefs will be recreated as soon as the modifiers are
    // reattached. Another purpose with the detachment of all the modifiers is
    // to be able to manipulate add and remove steps from the NiPSSimulator.
    NIVERIFY(DetachAllModifiers());
    NiMesh::RemoveAllStreamRefs();

    // Reallocate the data buffers and set the PhysX particles max size
    // to m_uiMaxNumParticles.
    m_uiMaxNumPhysXParticles = m_uiMaxNumParticles;
    m_uiMaxNumParticles = NiUInt32(m_uiMaxNumParticles * fExpansionFactor);
    AllocateDataBuffers(m_uiMaxNumParticles, 
        HasColors(), HasRotations(), HasRotationAxes());

    // Set the stuff the clone didn't catch
    m_bPhysXSpace = bPhysXSpace;
    
    OnCreation();   
    
    // Create a fluid descriptor and add it to the scene
    
    // First get the scene descriptor.
    NIASSERT(pkProp->GetSnapshot() &&
        NiIsKindOf(NiPhysXFluidPropDesc, pkProp->GetSnapshot()));
    NiPhysXFluidPropDesc* pkPropDesc =
        (NiPhysXFluidPropDesc*)pkProp->GetSnapshot();

    // Create the Fluid descriptor
    NiPhysXFluidDescPtr spFluidDesc = NiNew NiPhysXFluidDesc();
    spFluidDesc->SetName(GetName());
    spFluidDesc->SetParticleSystem(this);

    // Reset the simulator
    NiPSSimulatorPtr spInputSimulator = m_pkSimulator;
    RemoveModifier(m_pkSimulator);
    m_pkSimulator = NiNew NiPhysXPSSimulator();
    AddModifier(m_pkSimulator, false);

    m_spFluidEmitter = ConvertModifiers(this, spInputSimulator, pkProp,
        spFluidDesc, kActorMap);
    AddEmitter(m_spFluidEmitter);

    // Reattach all modifiers.
    NIVERIFY(AttachAllModifiers());

    pkPropDesc->AddFluidDesc(spFluidDesc);

    // Set up the destinations to get data out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
    else
    {
        m_spDestination = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXPSFluidSystem::NiPhysXPSFluidSystem()
{
    m_spDestination = 0;
    m_spFluidEmitter = 0;
    m_uiMaxNumPhysXParticles = 0;
    m_pkPositionsBuffer = 0;
    m_puiPhysXToGBMap = 0;
    m_puiGBToPhysXMap = 0;
    m_bPhysXSpace = false;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::InitializeParticle(NiUInt32 uiNewParticle)
{
    NiPSParticleSystem::InitializeParticle(uiNewParticle);

    NiUInt32 uiIndexFromPhysXToGB = 
        m_kFluidCreationData.bufferId[m_uiNumAddedParticles - 1];

    m_puiPhysXToGBMap[uiIndexFromPhysXToGB] = uiNewParticle;
    m_puiGBToPhysXMap[uiNewParticle] = uiIndexFromPhysXToGB;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::RemoveParticle(NiUInt32 uiIndexToRemove)
{
    NiUInt32 uiLastIndex = m_uiNumParticles - 1;
    NiPSParticleSystem::RemoveParticle(uiIndexToRemove);

    NiUInt32 uiIndexFromPhysXToGB = m_puiGBToPhysXMap[uiLastIndex];
    m_puiPhysXToGBMap[uiIndexFromPhysXToGB] = uiIndexToRemove;
    m_puiGBToPhysXMap[uiIndexToRemove] = uiIndexFromPhysXToGB;
    m_pkPositionsBuffer[uiIndexToRemove] = m_pkPositionsBuffer[uiLastIndex];
}
//---------------------------------------------------------------------------
NiPhysXPSFluidEmitter* NiPhysXPSFluidSystem::GetFluidEmitter()
{
    return m_spFluidEmitter;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::SetFluidEmitter(
    NiPhysXPSFluidEmitter* pkFluidEmitter)
{
    if (m_spDestination)
    {
        m_spDestination->SetEmitter(pkFluidEmitter);
    }
    m_spFluidEmitter = pkFluidEmitter;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::SetDestination(NiPhysXFluidDest* pkFluidDest)
{
    m_spDestination = pkFluidDest;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::AttachToProp(NiPhysXProp* pkProp)
{
    NIASSERT(m_spFluidEmitter);
    
    m_spDestination = NiNew NiPhysXFluidDest(this, m_spFluidEmitter);
    pkProp->AddDestination(m_spDestination);
    pkProp->AddModifiedMesh(this);
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::DetachFromProp(NiPhysXProp* pkProp)
{
    pkProp->DeleteModifiedMesh(this);
    pkProp->DeleteDestination(m_spDestination);
    m_spDestination = 0;
}
//---------------------------------------------------------------------------
NiPhysXPSFluidEmitter* NiPhysXPSFluidSystem::ConvertModifiers(
    NiPSParticleSystem* pkTarget, NiPSSimulator* pkOrigSimulator,
    NiPhysXProp* pkProp, NiPhysXFluidDescPtr spFluidDesc,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap)
{
    // Cloning misses setting the target on the controllers attached to the
    // particle system object. Go through and set them now.
    NiTimeController* pkController = pkTarget->GetControllers();
    while (pkController)
    {
        pkController->SetTarget(pkTarget);
        pkController = pkController->GetNext();
    }
    
    // We get these from the first emitter we find
    NiRGBA kEmitColor(255, 255, 255, 255);
    float fEmitRad = 1.0f;
    float fEmitRadVar = 0.0f;
    
    NiTObjectArray<NiPhysXFluidEmitterDescPtr> kEmitters;

    NiTPointerList<NiPSEmitterPtr> kVictimEmitters;
    NiUInt32 uiEmitterCount = pkTarget->GetEmitterCount();
    for (NiUInt32 ui = 0; ui < uiEmitterCount; ui++)
    {
        NiPSEmitter* pkEmitter = pkTarget->GetEmitterAt(ui);

        kEmitColor = pkEmitter->GetColor();
        fEmitRad = pkEmitter->GetRadius();
        fEmitRadVar = pkEmitter->GetRadiusVar();

        // Create a fluid emitter for each volume emitter found
        float fInvScale = 1.0f;
        if (pkProp)
            fInvScale = 1.0f / pkProp->GetScaleFactor();
        ConvertEmitter(
            pkTarget, pkEmitter, fInvScale, kActorMap, kEmitters);
        kVictimEmitters.AddTail(pkEmitter);
    }

    NiTPointerList<NiPSSimulatorStepPtr> kVictimSimulatorSteps;
    NiPSSimulator* pkSimulator = pkTarget->GetSimulator();
    
    NiUInt32 uiSimulatorStepCount = pkOrigSimulator->GetStepCount();
    for (NiUInt32 ui = 0; ui < uiSimulatorStepCount; ui++)
    {
        NiPSSimulatorStep* pkSimulatorStep = pkOrigSimulator->GetStepAt(ui);
        if (NiIsKindOf(NiPSSimulatorForcesStep, pkSimulatorStep) || 
            NiIsKindOf(NiPSSimulatorFinalStep, pkSimulatorStep) ||
            NiIsKindOf(NiPSSimulatorCollidersStep, pkSimulatorStep))
        {
            kVictimSimulatorSteps.AddTail(pkSimulatorStep);
        }
        else
        {
            pkSimulator->AddStep(pkSimulatorStep);
        }
    }

    NiTPointerList<NiPSEmitterCtlrPtr> kVictimEmitterControllers;
    NiTPointerList<NiPSForceCtlrPtr> kVictimForcesControllers;
    pkController = pkTarget->GetControllers();
    while (pkController)
    {
        if (NiIsKindOf(NiPSEmitterCtlr, pkController))
        {
            NiPSEmitterCtlr* pkEmitterCtrl = (NiPSEmitterCtlr*)pkController;
            
            const NiFixedString kEmitterName = pkEmitterCtrl->GetEmitterName();
            NiTListIterator kIter = kVictimEmitters.GetHeadPos();
            while (kIter)
            {
                NiPSEmitter* pkEmitter = kVictimEmitters.GetNext(kIter);
                if (kEmitterName == pkEmitter->GetName())
                {
                    kVictimEmitterControllers.AddTail(pkEmitterCtrl);
                    break;
                }
            }
        }
        else if (NiIsKindOf(NiPSForceCtlr, pkController))
        {
            NiPSForceCtlr* pkForceCtrl = (NiPSForceCtlr*)pkController;
            
            const NiFixedString kForceStepName = pkForceCtrl->GetForceName();
            NiTListIterator kIter = kVictimEmitters.GetHeadPos();
            while (kIter)
            {
                bool found = false;
                NiPSSimulatorStep* pkSimulatorStep = 
                    kVictimSimulatorSteps.GetNext(kIter);
                if (NiIsKindOf(NiPSSimulatorForcesStep, pkSimulatorStep))
                {
                    NiPSSimulatorForcesStep* pkSimForceStep = 
                        (NiPSSimulatorForcesStep*)pkSimulatorStep;
                    for (NiUInt32 ui = 0; ui < 
                        pkSimForceStep->GetForcesCount(); ui++)
                    {
                        NiPSForce* pkForce = pkSimForceStep->GetForceAt(ui);
                        if (kForceStepName == pkForce->GetName())
                        {
                            kVictimForcesControllers.AddTail(pkForceCtrl);
                            found = true;
                            break;
                        }
                    }
                } 
                if (found)
                {
                    break;
                }
            }
        }
        pkController = pkController->GetNext();            
    }
    
    NiTListIterator kEmitterCtrlIter = kVictimEmitterControllers.GetHeadPos();
    while (kEmitterCtrlIter)
    {
        NiPSEmitterCtlr* pkEmitterCtrl = 
            kVictimEmitterControllers.GetNext(kEmitterCtrlIter);
        
        // Setting the target deletes the controller from the target's
        // list, and then removing all from the list below deletes it.
        pkEmitterCtrl->SetTarget(0);
    }
    kVictimEmitterControllers.RemoveAll();

    NiTListIterator kForcesCtrlIter = kVictimForcesControllers.GetHeadPos();
    while (kForcesCtrlIter)
    {
        NiPSForceCtlr* pkForceCtrl = 
            kVictimForcesControllers.GetNext(kForcesCtrlIter);
        
        // Setting the target deletes the controller from the target's
        // list, and then removing all from the list below deletes it.
        pkForceCtrl->SetTarget(0);
    }
    kVictimForcesControllers.RemoveAll();
    
    kVictimEmitters.RemoveAll();
    pkTarget->RemoveAllEmitters();
   
    // Add a step specific for physx fluids.
    pkSimulator->AddStep(NiNew NiPhysXFluidSimulatorStep());
    
    spFluidDesc->SetEmitters(kEmitters);

    // Return a NiPhysXFluidEmtter.
    return NiNew NiPhysXPSFluidEmitter("FluidEmitter", kEmitColor, fEmitRad, 
        fEmitRadVar);
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::ConvertEmitter(NiPSParticleSystem* pkTarget,
    NiPSEmitter* pkEmitter, float fInvScale,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap,
    NiTObjectArray<NiPhysXFluidEmitterDescPtr>& kEmitters)
{
    if (NiIsKindOf(NiPSVolumeEmitter, pkEmitter))
    {
        NiPSVolumeEmitter* pkVolEmitter = (NiPSVolumeEmitter*)pkEmitter;
        
        NiPhysXActorDescPtr spActorDesc = 0;
        NiPhysXShapeDescPtr spShapeDesc = 0;
        NxMat34 kPose = NiPhysXTypes::NXMAT34_ID;
        NiAVObject* pkEmitObj = pkVolEmitter->GetEmitterObj();
        if (pkEmitObj)
        {
            NiAVObject* pkEmitParent = pkEmitObj->GetParent();
            if (pkEmitParent && kActorMap.GetAt(pkEmitParent, spActorDesc))
            {
                // Need relative Xform
                NiMatrix3 kRotation = pkEmitObj->GetRotate();
                NiPoint3 kTranslation = pkEmitObj->GetTranslate();
                NxMat34 kActorXform;
                NiPhysXTypes::NiTransformToNxMat34(kRotation, kTranslation,
                    kActorXform);
                kActorXform.t *= fInvScale;
                
                spShapeDesc = spActorDesc->GetActorShapes().GetAt(0);
                NIASSERT(spShapeDesc);
                
                NxMat34 kShapeXform = spShapeDesc->GetLocalPose();
                kShapeXform.getInverse(kShapeXform);
                
                kPose = kShapeXform * kActorXform;
            }
            else
            {
                NiMatrix3 kRotation = pkEmitObj->GetWorldRotate();
                NiPoint3 kTranslation = pkEmitObj->GetWorldTranslate();
                NiPhysXTypes::NiTransformToNxMat34(kRotation, kTranslation,
                    kPose);
                kPose.t *= fInvScale;
            }
        }

        float fEmitSpeed = pkEmitter->GetSpeed() * fInvScale;
        float fEmitLife = pkEmitter->GetLifeSpan();
        
        bool bEllipse;
        float fDimX;
        float fDimY;
        if (NiIsKindOf(NiPSBoxEmitter, pkVolEmitter))
        {
            fDimX = ((NiPSBoxEmitter*)pkVolEmitter)->
                GetEmitterWidth() * fInvScale;
            fDimY = ((NiPSBoxEmitter*)pkVolEmitter)->
                GetEmitterHeight() * fInvScale;
            bEllipse = false;
        }
        else if (NiIsKindOf(NiPSSphereEmitter, pkVolEmitter))
        {
            fDimX = fDimY = ((NiPSSphereEmitter*)pkVolEmitter)->
                GetEmitterRadius() * fInvScale;
            bEllipse = true;
        }
        else if (NiIsKindOf(NiPSCylinderEmitter, pkVolEmitter))
        {
            fDimX = fDimY = ((NiPSCylinderEmitter*)pkVolEmitter)->
                GetEmitterRadius() * fInvScale;
            bEllipse = true;
        }
        else
        {
            NIASSERT(false && "NiPhysXPSFluidSystem::ConvertModifiers"
                    && "Unknown type of emitter");
            fDimX = fDimY = 0.0f;
            bEllipse = false;
        }
        
        float fBirthRate = 0.0f;
        NiTimeController* pkController = pkTarget->GetControllers();
        while (pkController)
        {
            if (NiIsKindOf(NiPSEmitParticlesCtlr, pkController))
            {
                NiPSEmitParticlesCtlr* pkEmitterCtrl =
                    (NiPSEmitParticlesCtlr*)pkController;
                if (pkEmitterCtrl->GetEmitterPointer() == pkEmitter)
                {
                    NiFloatInterpolator* pkInterp =
                        pkEmitterCtrl->GetBirthRateInterpolator();
                    pkInterp->Update(0.0f, pkTarget, fBirthRate);
                }
                break;
            }
            pkController = pkController->GetNext();
        }
        
        NxFluidEmitterDesc kNxEmitDesc;
        kNxEmitDesc.setToDefault();
        kNxEmitDesc.rate = fBirthRate;
        kNxEmitDesc.fluidVelocityMagnitude = fEmitSpeed;
        kNxEmitDesc.particleLifetime = fEmitLife;
        kNxEmitDesc.type = NX_FE_CONSTANT_FLOW_RATE;
        kNxEmitDesc.dimensionX = fDimX * 0.5f;
        kNxEmitDesc.dimensionY = fDimY * 0.5f;
        if (bEllipse)
            kNxEmitDesc.shape = NX_FE_ELLIPSE;
        else
            kNxEmitDesc.shape = NX_FE_RECTANGULAR;
        kNxEmitDesc.relPose = kPose;
        
        NiPhysXFluidEmitterDesc* pkEmitterDesc =
            NiNew NiPhysXFluidEmitterDesc();
        pkEmitterDesc->SetNxFluidEmitterDesc(kNxEmitDesc);
        pkEmitterDesc->SetFrameShape(spShapeDesc);
        spActorDesc = 0;
        spShapeDesc = 0;

        kEmitters.AddFirstEmpty(pkEmitterDesc);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::OnCreation()
{
    // Fluid data, position, velocity and buffer ID.
    m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
    *(m_kFluidData.numParticlesPtr) = 0;
    m_kFluidData.bufferPosByteStride = 3 * sizeof(NxF32);
    m_kFluidData.bufferPos = NiAlloc(NxF32, m_uiMaxNumPhysXParticles * 3);
    m_kFluidData.bufferId = NiAlloc(NxU32, m_uiMaxNumPhysXParticles);
    m_kFluidData.bufferIdByteStride = sizeof(NxU32);

    // Arrays with indices to map particles from PhysX to GB.
    m_puiPhysXToGBMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);
    memset(m_puiPhysXToGBMap, 0xFF, m_uiMaxNumParticles * sizeof(NiUInt32));
    m_puiGBToPhysXMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);

    // Creation of PhysX particles.
    m_kFluidCreationData.numIdsPtr = NiAlloc(NxU32, 1);
    *(m_kFluidCreationData.numIdsPtr) = 0;
    m_kFluidCreationData.bufferId = NiAlloc(NxU32, m_uiMaxNumPhysXParticles);
    m_kFluidCreationData.bufferIdByteStride = sizeof(NxU32);
   
    // Deletion  of PhysX particles.
    m_kFluidDeletionData.numIdsPtr = NiAlloc(NxU32, 1);
    *(m_kFluidDeletionData.numIdsPtr) = 0;
    m_kFluidDeletionData.bufferId = NiAlloc(NxU32, m_uiMaxNumPhysXParticles);
    m_kFluidDeletionData.bufferIdByteStride = sizeof(NxU32);
    
    // Buffer for PhysX positions in Gamebryo ordering
    m_pkPositionsBuffer = NiAlloc(NxVec3, m_uiMaxNumParticles);
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::OnDeletion()
{
    NiFree(m_kFluidData.numParticlesPtr);
    NiFree(m_kFluidData.bufferPos);    
    NiFree(m_kFluidData.bufferId);
    
    NiFree(m_puiPhysXToGBMap);
    NiFree(m_puiGBToPhysXMap);

    NiFree(m_kFluidCreationData.numIdsPtr);
    NiFree(m_kFluidCreationData.bufferId);

    NiFree(m_kFluidDeletionData.numIdsPtr);
    NiFree(m_kFluidDeletionData.bufferId);
    
    NiFree(m_pkPositionsBuffer);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSFluidSystem);
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::CopyMembers(NiPhysXPSFluidSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSParticleSystem::CopyMembers(pkDest, kCloning);

    if (!NiIsKindOf(NiPhysXPSFluidSystem, this))
        return;

    if (m_spDestination)
    {
        // We expect the prop that this system is associated with to also
        // clone the destination, so use shared clone function.
        pkDest->m_spDestination =
            (NiPhysXFluidDest*)m_spDestination->CreateSharedClone(kCloning);
    }
    else
    {
        m_spDestination = 0;
    }
    
    pkDest->m_bPhysXSpace = m_bPhysXSpace;

    // The fluid emitter is cloned as if sharing, but since nothing else
    // shares the original emitter, nothing will share this either. We just
    // want anything else holding a pointer to the emitter to get the same
    // emitter clone.
    pkDest->m_spFluidEmitter =
        (NiPhysXPSFluidEmitter*)m_spFluidEmitter->CreateSharedClone(kCloning);

    pkDest->m_uiMaxNumPhysXParticles = m_uiMaxNumPhysXParticles;

    // Sets all the fluid data structures.
    pkDest->OnCreation();
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSParticleSystem::ProcessClone(kCloning);

    bool bCloned = false;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    
    if (m_spDestination)
    {
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(m_spDestination, bJunk))
            m_spDestination->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSFluidSystem);
//---------------------------------------------------------------------------
bool NiPhysXPSFluidSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::LoadBinary(NiStream& kStream)
{
    NiPSParticleSystem::LoadBinary(kStream);

    m_spDestination = (NiPhysXFluidDest*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );

    NiStreamLoadBinary(kStream, m_uiMaxNumPhysXParticles);

    m_puiPhysXToGBMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);
    memset(m_puiPhysXToGBMap, 0xFF, m_uiMaxNumParticles * sizeof(NiUInt32));
    m_puiGBToPhysXMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);
    m_pkPositionsBuffer = NiAlloc(NxVec3, m_uiMaxNumParticles);

    m_spFluidEmitter = (NiPhysXPSFluidEmitter*)kStream.ResolveLinkID();

    m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidData.numParticlesPtr));
    
    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidData.bufferPosByteStride);
        NIASSERT(uiSize == m_uiMaxNumPhysXParticles *
            m_kFluidData.bufferPosByteStride / sizeof(NxF32));
        m_kFluidData.bufferPos = NiAlloc(NxF32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidData.bufferPos, uiSize);
        m_kFluidData.bufferId = NiAlloc(NxU32, m_uiMaxNumParticles);
        m_kFluidData.bufferIdByteStride = sizeof(NxU32);
    }

    m_kFluidCreationData.numIdsPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidCreationData.numIdsPtr));
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidCreationData.bufferIdByteStride);
        NIASSERT(uiSize == m_uiMaxNumPhysXParticles *
            m_kFluidCreationData.bufferIdByteStride / sizeof(NxU32));
        m_kFluidCreationData.bufferId = NiAlloc(NxU32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidCreationData.bufferId, uiSize);
    } 

    m_kFluidDeletionData.numIdsPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidDeletionData.numIdsPtr));
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidDeletionData.bufferIdByteStride);
        NIASSERT(uiSize == m_uiMaxNumPhysXParticles *
            m_kFluidDeletionData.bufferIdByteStride / sizeof(NxU32));
        m_kFluidDeletionData.bufferId = NiAlloc(NxU32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidDeletionData.bufferId, uiSize);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::LinkObject(NiStream& kStream)
{
    NiPSParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSFluidSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidSystem::SaveBinary(NiStream& kStream)
{
    NiPSParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spDestination);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));

    NiStreamSaveBinary(kStream, m_uiMaxNumPhysXParticles);

    kStream.SaveLinkID(m_spFluidEmitter);

    NiStreamSaveBinary(kStream, *(m_kFluidData.numParticlesPtr));
        
    NiUInt32 uiSize;
    
    if (m_kFluidData.bufferPos)
    {
        uiSize = m_uiMaxNumPhysXParticles* m_kFluidData.bufferPosByteStride
            / sizeof(NxF32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferPosByteStride);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferPos, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
    
    NiStreamSaveBinary(kStream, *(m_kFluidCreationData.numIdsPtr));
        
    if (m_kFluidCreationData.bufferId)
    {
        uiSize = m_uiMaxNumPhysXParticles * 
            m_kFluidCreationData.bufferIdByteStride / sizeof(NxU32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidCreationData.bufferIdByteStride);
        NiStreamSaveBinary(kStream, m_kFluidCreationData.bufferId, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }

    NiStreamSaveBinary(kStream, *(m_kFluidDeletionData.numIdsPtr));
        
    if (m_kFluidDeletionData.bufferId)
    {
        uiSize = m_uiMaxNumPhysXParticles * 
            m_kFluidDeletionData.bufferIdByteStride / sizeof(NxU32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidDeletionData.bufferIdByteStride);
        NiStreamSaveBinary(kStream, m_kFluidDeletionData.bufferId, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXPSFluidSystem::IsEqual(NiObject* pkObject)
{
    if (!NiPSParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXPSFluidSystem* pkNewObject = (NiPhysXPSFluidSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    if (*(m_kFluidData.numParticlesPtr) != 
        *(pkNewObject->m_kFluidData.numParticlesPtr))
        return false;

    if (m_kFluidData.bufferPos)
    {
        if (!pkNewObject->m_kFluidData.bufferPos)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidData.numParticlesPtr) * 3;
            ui++)
        {
            if (m_kFluidData.bufferPos[ui] !=
                pkNewObject->m_kFluidData.bufferPos[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidData.bufferPos)
            return false;
    }

    if (m_kFluidData.bufferVel)
    {
        if (!pkNewObject->m_kFluidData.bufferVel)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidData.numParticlesPtr) * 3;
            ui++)
        {
            if (m_kFluidData.bufferVel[ui] !=
                pkNewObject->m_kFluidData.bufferVel[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidData.bufferVel)
            return false;
    }    

    if (m_kFluidCreationData.bufferId)
    {
        if (!pkNewObject->m_kFluidCreationData.bufferId)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidCreationData.numIdsPtr);
            ui++)
        {
            if (m_kFluidCreationData.bufferId[ui] !=
                pkNewObject->m_kFluidCreationData.bufferId[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidCreationData.bufferId)
            return false;
    }

    if (m_kFluidDeletionData.bufferId)
    {
        if (!pkNewObject->m_kFluidDeletionData.bufferId)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidDeletionData.numIdsPtr);
            ui++)
        {
            if (m_kFluidDeletionData.bufferId[ui] !=
                pkNewObject->m_kFluidDeletionData.bufferId[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidDeletionData.bufferId)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
