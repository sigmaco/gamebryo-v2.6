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

#include "NiPhysXManager.h"
#include "NiPhysXFluidDesc.h"
#include "NiPhysXPSFluidSystem.h"
#include "NiPhysXPSMeshFluidSystem.h"

NiImplementRTTI(NiPhysXFluidDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXFluidDesc::NiPhysXFluidDesc() : m_kName(NULL), m_kEmitters(0,1), 
    m_pkFluid(0), m_pkPSys(0)
{
    m_kDesc.setToDefault();
}
//---------------------------------------------------------------------------
NiPhysXFluidDesc::~NiPhysXFluidDesc()
{
    m_kEmitters.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::SetNxFluidDesc(const NxFluidDesc& kFluidDesc)
{
    // Does not set the emitters or modify the NxFluid pointer
    
    m_kDesc = kFluidDesc;
    m_kName = kFluidDesc.name;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::SetFluid(NxFluid* pkFluid, const NxMat34& kXform,
    NiTMap<NxShape*, NiPhysXShapeDescPtr>& kShapeMap)
{
    m_pkFluid = pkFluid;

    m_pkFluid->saveToDesc(m_kDesc);
    m_kDesc.externalAcceleration = kXform.M * m_kDesc.externalAcceleration;

    if (m_pkFluid->getName())
        m_kName = NiFixedString(m_pkFluid->getName());
    else
        m_kName = NiFixedString(NULL);
    
    m_kEmitters.RemoveAll();
    NiUInt32 uiNumEmitters = pkFluid->getNbEmitters();
    NxFluidEmitter** ppkEmitters = pkFluid->getEmitters();
    for (NiUInt32 ui = 0; ui < uiNumEmitters; ui++)
    {
        NiPhysXFluidEmitterDesc* pkEmitter = NiNew NiPhysXFluidEmitterDesc;
        pkEmitter->SetEmitter(ppkEmitters[ui], kXform, kShapeMap);
        m_kEmitters.Add(pkEmitter);
    } 
}
//---------------------------------------------------------------------------
NxFluid* NiPhysXFluidDesc::CreateFluid(NxScene* pkScene,
    NxCompartment* pkCompartment, const NxMat34& kXform)
{
    NxFluidDesc kFluidDesc = m_kDesc;
    kFluidDesc.compartment = pkCompartment;

    if (kFluidDesc.flags & NX_FF_HARDWARE)
    {
        bool bHaveHardware = NiPhysXManager::GetPhysXManager()->
            m_pkPhysXSDK->getHWVersion() > 0;
        if (!bHaveHardware)
        {
            kFluidDesc.flags = kFluidDesc.flags & (~NX_FF_HARDWARE);
            NiOutputDebugString("NiPhysXFluidDesc::CreateFluid: "
                "No hardware. Fluids in software.\n");
        }
    }

    kFluidDesc.externalAcceleration =
        kXform.M * kFluidDesc.externalAcceleration;
    kFluidDesc.emitters.clear();
    kFluidDesc.name = m_kName;

    if (m_pkPSys)
    {
        if (NiIsKindOf(NiPhysXPSFluidSystem, m_pkPSys))
        {
            NiPhysXPSFluidSystem* pkFluidSys = (NiPhysXPSFluidSystem*)m_pkPSys;
            
            kFluidDesc.initialParticleData = pkFluidSys->GetFluidData();
            kFluidDesc.particlesWriteData = pkFluidSys->GetFluidData();
            kFluidDesc.particleCreationIdWriteData = 
                pkFluidSys->GetParticleCreationData();
            kFluidDesc.particleDeletionIdWriteData = 
                pkFluidSys->GetParticleDeletionData();
            kFluidDesc.maxParticles =
                pkFluidSys->GetMaxNumPhysXParticles();
        }
        else if (NiIsKindOf(NiPhysXPSMeshFluidSystem, m_pkPSys))
        {
            NiPhysXPSMeshFluidSystem* pkFluidSys = 
                (NiPhysXPSMeshFluidSystem*)m_pkPSys;
            
            kFluidDesc.initialParticleData = pkFluidSys->GetFluidData();
            kFluidDesc.particlesWriteData = pkFluidSys->GetFluidData();
            kFluidDesc.particleCreationIdWriteData = 
                pkFluidSys->GetParticleCreationData();
            kFluidDesc.particleDeletionIdWriteData = 
                pkFluidSys->GetParticleDeletionData();
            kFluidDesc.maxParticles =
                pkFluidSys->GetMaxNumPhysXParticles();
        }
        else
        {
            NIASSERT(!"NiPhysXFluidDesc::CreateFluid PSys is not fluid");
        }
    }
    else
    {
        kFluidDesc.initialParticleData.numParticlesPtr = 0;
        kFluidDesc.particlesWriteData.numParticlesPtr = 0;
    }

    NIASSERT(kFluidDesc.isValid());
    m_pkFluid = pkScene->createFluid(kFluidDesc);
#ifdef NIDEBUG
    if (!m_pkFluid)
    {
        NiOutputDebugString(
            "NiPhysXFluidDesc::CreateFluid: Fluid creation failed.\n");
        NiOutputDebugString(
            "NiPhysXFluidDesc::CreateFluid: Look for a message from the "
            "PhysX SDK as to why this might have occurred.\n");
        return 0;
    }
#endif

    for (NiUInt32 ui = 0; ui < m_kEmitters.GetSize(); ui++)
    {
        if (!m_kEmitters.GetAt(ui)->CreateEmitter(m_pkFluid, kXform))
        {
            pkScene->releaseFluid(*m_pkFluid);
            return 0;
        }
    }

    return m_pkFluid;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::RemoveFluid(NxScene* pkScene)
{
    if (m_pkFluid)
    {
        pkScene->releaseFluid(*m_pkFluid);
        m_pkFluid = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXFluidDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::CopyMembers(NiPhysXFluidDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // copy object names
    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kName = m_kName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kName)
        {
            size_t stQuantity = strlen(m_kName) + 2;
            char* pcNamePlus = NiAlloc(char, stQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, stQuantity, m_kName);
            pcNamePlus[stQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[stQuantity - 1] = 0;
            pkDest->m_kName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }

    pkDest->m_kDesc = m_kDesc;
    
    pkDest->m_kEmitters.SetSize(m_kEmitters.GetSize());
    for (NiUInt32 ui = 0; ui < m_kEmitters.GetSize(); ui++)
    {
        NiPhysXFluidEmitterDescPtr spEmitter = m_kEmitters.GetAt(ui);
        pkDest->m_kEmitters.SetAt(ui, 
            (NiPhysXFluidEmitterDesc*)spEmitter->CreateClone(kCloning));
    }

    pkDest->m_pkFluid = 0;
    
    pkDest->m_pkPSys = m_pkPSys;
    
}
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXFluidDesc* pkClone = (NiPhysXFluidDesc*)pkCloneObj;

    if (m_pkPSys)
    {
        NiObject* pkClonedPSysObj = 0;
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkPSys, pkClonedPSysObj);
        NiPSParticleSystem* pkClonedPSys = 
            (NiPSParticleSystem*) pkClonedPSysObj;
        if (bCloned)
        {
            pkClone->m_pkPSys = pkClonedPSys;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXFluidDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    for (NiUInt32 ui = 0; ui < m_kEmitters.GetSize(); ui++)
    {
        m_kEmitters.GetAt(ui)->RegisterStreamables(kStream);
    }
    
    kStream.RegisterFixedString(m_kName);

    if (m_pkPSys)
        m_pkPSys->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    kStream.SaveFixedString(m_kName);

    NiStreamSaveBinary(kStream, m_kDesc.maxParticles);
    NiStreamSaveBinary(kStream, m_kDesc.restParticlesPerMeter);
    NiStreamSaveBinary(kStream, m_kDesc.restDensity);
    NiStreamSaveBinary(kStream, m_kDesc.kernelRadiusMultiplier);
    NiStreamSaveBinary(kStream, m_kDesc.motionLimitMultiplier);
    NiStreamSaveBinary(kStream, m_kDesc.packetSizeMultiplier);
    NiStreamSaveBinary(kStream, m_kDesc.stiffness);
    NiStreamSaveBinary(kStream, m_kDesc.viscosity);
    NiStreamSaveBinary(kStream, m_kDesc.surfaceTension);
    NiStreamSaveBinary(kStream, m_kDesc.damping);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kDesc.externalAcceleration);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kDesc.projectionPlane.normal);
    NiStreamSaveBinary(kStream, m_kDesc.projectionPlane.d);
    NiStreamSaveBinary(kStream, m_kDesc.restitutionForStaticShapes);
    NiStreamSaveBinary(kStream, m_kDesc.dynamicFrictionForStaticShapes);
    NiStreamSaveBinary(kStream, m_kDesc.staticFrictionForStaticShapes);
    NiStreamSaveBinary(kStream, m_kDesc.restitutionForDynamicShapes);
    NiStreamSaveBinary(kStream, m_kDesc.dynamicFrictionForDynamicShapes);
    NiStreamSaveBinary(kStream, m_kDesc.staticFrictionForDynamicShapes);
    NiStreamSaveBinary(kStream, m_kDesc.simulationMethod);
    NiStreamSaveBinary(kStream, m_kDesc.collisionMethod);
    NiStreamSaveBinary(kStream, m_kDesc.flags);
    
    // Added in PhysX 2.5.0
    NiStreamSaveBinary(kStream, m_kDesc.collisionResponseCoefficient);
    NiStreamSaveBinary(kStream, m_kDesc.collisionGroup);
    NiStreamSaveBinary(kStream, m_kDesc.groupsMask.bits0);
    NiStreamSaveBinary(kStream, m_kDesc.groupsMask.bits1);
    NiStreamSaveBinary(kStream, m_kDesc.groupsMask.bits2);
    NiStreamSaveBinary(kStream, m_kDesc.groupsMask.bits3);

    NiStreamSaveBinary(kStream, m_kDesc.forceFieldMaterial);
    
    // Added in PhysX 2.6.2
    NiStreamSaveBinary(kStream, m_kDesc.collisionDistanceMultiplier);

    NiStreamSaveBinary(kStream, m_kEmitters.GetSize());
    for (NiUInt32 ui = 0; ui < m_kEmitters.GetSize(); ui++)
    {
        kStream.SaveLinkID(m_kEmitters.GetAt(ui));
    }

    kStream.SaveLinkID(m_pkPSys);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kName);

    NiStreamLoadBinary(kStream, m_kDesc.maxParticles);
    // Apparently this limit changed between versions.
    if (m_kDesc.maxParticles > 32767)
        m_kDesc.maxParticles = 32767;
    NiStreamLoadBinary(kStream, m_kDesc.restParticlesPerMeter);
    NiStreamLoadBinary(kStream, m_kDesc.restDensity);
    NiStreamLoadBinary(kStream, m_kDesc.kernelRadiusMultiplier);
    NiStreamLoadBinary(kStream, m_kDesc.motionLimitMultiplier);
    NiStreamLoadBinary(kStream, m_kDesc.packetSizeMultiplier);
    NiStreamLoadBinary(kStream, m_kDesc.stiffness);
    NiStreamLoadBinary(kStream, m_kDesc.viscosity);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kDesc.surfaceTension);
    }
    NiStreamLoadBinary(kStream, m_kDesc.damping);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
    {
        // Stream in the m_kDesc.externalForce from PhysX 2.3.2 which was
        // streamed in Gamebryo version 2.2 and earlier. Convert it
        // and put it in externalAccel.
        NxVec3 kNxVec;
        NiPhysXTypes::NxVec3LoadBinary(kStream, kNxVec);
        m_kDesc.externalAcceleration = kNxVec * ( 1.0f / m_kDesc.restDensity);
    }
    else
    {
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_kDesc.externalAcceleration);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiPhysXTypes::NxVec3LoadBinary(kStream,
            m_kDesc.projectionPlane.normal);
        NiStreamLoadBinary(kStream, m_kDesc.projectionPlane.d);
    }

    NiStreamLoadBinary(kStream, m_kDesc.restitutionForStaticShapes);
    NiStreamLoadBinary(kStream, m_kDesc.dynamicFrictionForStaticShapes);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kDesc.staticFrictionForStaticShapes);
    }
    NiStreamLoadBinary(kStream, m_kDesc.restitutionForDynamicShapes);
    NiStreamLoadBinary(kStream, m_kDesc.dynamicFrictionForDynamicShapes);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kDesc.staticFrictionForDynamicShapes);
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
    {
        // Stream in the m_kDesc.dynamicActorReactionSmoothing and
        // m_kDesc.dynamicActorReactionScaling from PhysX 2.3.2 which were
        // streamed in Gamebryo version 2.2 and earlier.
        NxReal kReal;
        NiStreamLoadBinary(kStream, kReal);
        NiStreamLoadBinary(kStream, kReal);
    }

    NiStreamLoadBinary(kStream, m_kDesc.simulationMethod);
    NiStreamLoadBinary(kStream, m_kDesc.collisionMethod);
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
    {
        // Stream in the m_kDesc.onSeparation, m_kDesc.onCollision and
        // m_kDesc.onLifetimeExpired from PhysX 2.3.2 which were streamed
        // in Gamebryo version 2.2 and earlier.
        NxU32 kU32;
        NiStreamLoadBinary(kStream, kU32);
        NiStreamLoadBinary(kStream, kU32);
        NiStreamLoadBinary(kStream, kU32);
    }
    
    NiStreamLoadBinary(kStream, m_kDesc.flags);
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 0))
    {
        // The enabled and hardware flags were added for PhysX 2.5.0
        m_kDesc.flags |= NX_FF_ENABLED|NX_FF_HARDWARE;
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 2, 0, 8))
    {
        // Stream in the m_kDesc.collisionGroup from PhysX 2.3.2 which
        // were streamed in Gamebryo version 2.2 and earlier.
        NxCollisionGroup kU16;
        NiStreamLoadBinary(kStream, kU16);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 0))
    {
        // Stream in PhysX 2.5.0 and later data
        NiStreamLoadBinary(kStream, m_kDesc.collisionResponseCoefficient);
        NiStreamLoadBinary(kStream, m_kDesc.collisionGroup);
        NiStreamLoadBinary(kStream, m_kDesc.groupsMask.bits0);
        NiStreamLoadBinary(kStream, m_kDesc.groupsMask.bits1);
        NiStreamLoadBinary(kStream, m_kDesc.groupsMask.bits2);
        NiStreamLoadBinary(kStream, m_kDesc.groupsMask.bits3);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kDesc.forceFieldMaterial);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 6))
    {
        NiStreamLoadBinary(kStream, m_kDesc.collisionDistanceMultiplier);
    }

    NiUInt32 uiEmittersSize;
    NiStreamLoadBinary(kStream, uiEmittersSize);
    m_kEmitters.SetSize(uiEmittersSize);
    for (NiUInt32 ui = 0; ui < uiEmittersSize; ui++)
    {
        m_kEmitters.SetAt(ui,
            (NiPhysXFluidEmitterDesc*)kStream.ResolveLinkID());
    }

    m_pkPSys = (NiPSParticleSystem*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXFluidDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXFluidDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXFluidDesc* pkNewObject = (NiPhysXFluidDesc*)pkObject;
    
    if (m_kName != pkNewObject->m_kName)
        return false;
    
    if (m_kDesc.maxParticles != pkNewObject->m_kDesc.maxParticles ||
        m_kDesc.restParticlesPerMeter !=
        pkNewObject->m_kDesc.restParticlesPerMeter ||
        m_kDesc.restDensity != pkNewObject->m_kDesc.restDensity ||
        m_kDesc.kernelRadiusMultiplier !=
        pkNewObject->m_kDesc.kernelRadiusMultiplier ||
        m_kDesc.motionLimitMultiplier !=
        pkNewObject->m_kDesc.motionLimitMultiplier ||
        m_kDesc.restParticlesPerMeter !=
        pkNewObject->m_kDesc.restParticlesPerMeter ||
        m_kDesc.packetSizeMultiplier !=
        pkNewObject->m_kDesc.packetSizeMultiplier ||
        m_kDesc.stiffness != pkNewObject->m_kDesc.stiffness ||
        m_kDesc.viscosity != pkNewObject->m_kDesc.viscosity ||
        m_kDesc.surfaceTension != pkNewObject->m_kDesc.surfaceTension ||
        m_kDesc.damping != pkNewObject->m_kDesc.damping ||
        m_kDesc.restitutionForStaticShapes !=
        pkNewObject->m_kDesc.restitutionForStaticShapes ||
        m_kDesc.dynamicFrictionForStaticShapes !=
        pkNewObject->m_kDesc.dynamicFrictionForStaticShapes ||
        m_kDesc.staticFrictionForStaticShapes !=
        pkNewObject->m_kDesc.staticFrictionForStaticShapes ||
        m_kDesc.restitutionForDynamicShapes !=
        pkNewObject->m_kDesc.restitutionForDynamicShapes ||
        m_kDesc.dynamicFrictionForDynamicShapes !=
        pkNewObject->m_kDesc.dynamicFrictionForDynamicShapes ||
        m_kDesc.staticFrictionForDynamicShapes !=
        pkNewObject->m_kDesc.staticFrictionForDynamicShapes ||        
        m_kDesc.simulationMethod !=
        pkNewObject->m_kDesc.simulationMethod ||
        m_kDesc.collisionMethod !=
        pkNewObject->m_kDesc.collisionMethod ||
        m_kDesc.collisionResponseCoefficient !=
        pkNewObject->m_kDesc.collisionResponseCoefficient ||
        m_kDesc.collisionGroup !=
        pkNewObject->m_kDesc.collisionGroup ||
        m_kDesc.groupsMask.bits0 !=
        pkNewObject->m_kDesc.groupsMask.bits0 ||
        m_kDesc.groupsMask.bits1 !=
        pkNewObject->m_kDesc.groupsMask.bits1 ||
        m_kDesc.groupsMask.bits2 !=
        pkNewObject->m_kDesc.groupsMask.bits2 ||
        m_kDesc.groupsMask.bits3 !=
        pkNewObject->m_kDesc.groupsMask.bits3 ||
        m_kDesc.forceFieldMaterial != 
        pkNewObject->m_kDesc.forceFieldMaterial ||
        m_kDesc.flags != pkNewObject->m_kDesc.flags)
    {
        return false;
    }
    
    NiPoint3 kNewP;
    NiPoint3 kThisP;

    NiPhysXTypes::NxVec3ToNiPoint3(m_kDesc.externalAcceleration, kThisP);
    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kDesc.externalAcceleration,
        kNewP);
    if (kNewP != kThisP)
        return false;

    NiPhysXTypes::NxVec3ToNiPoint3(m_kDesc.projectionPlane.normal, kThisP);
    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kDesc.projectionPlane.normal,
        kNewP);
    if (kNewP != kThisP ||
        m_kDesc.projectionPlane.d != pkNewObject->m_kDesc.projectionPlane.d)
        return false;

    if (m_kEmitters.GetSize() != pkNewObject->m_kEmitters.GetSize())
        return false;
        
    for (NiUInt32 ui = 0; ui < m_kEmitters.GetSize(); ui++)
    {
        NiPhysXFluidEmitterDescPtr spEmitter = m_kEmitters.GetAt(ui);
        NiPhysXFluidEmitterDescPtr spNewEmitter =
            pkNewObject->m_kEmitters.GetAt(ui);
        if ((spEmitter && !spNewEmitter) || (!spEmitter && spNewEmitter))
            return false;
        
        if (spEmitter && (!spEmitter->IsEqual(spNewEmitter)))
            return false;
    }

    if (m_pkPSys)
    {
        if (!pkNewObject->m_pkPSys ||
            !m_pkPSys->IsEqual(pkNewObject->m_pkPSys))
        return false;
    }
    else
    {
        if (pkNewObject->m_pkPSys)
            return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------

