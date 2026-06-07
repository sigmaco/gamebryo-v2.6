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

//#include "NiPhysXFluid.h"
#include "NiPhysXFluidEmitterDesc.h"

NiImplementRTTI(NiPhysXFluidEmitterDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXFluidEmitterDesc::NiPhysXFluidEmitterDesc() : m_kName(NULL)
{
    m_kDesc.setToDefault();
    m_spFrameShape = 0;
    m_pkEmitter = 0;
    
    m_spDeprecatedFrameActor = 0;
}
//---------------------------------------------------------------------------
NiPhysXFluidEmitterDesc::~NiPhysXFluidEmitterDesc()
{
    m_spFrameShape = 0;

    m_spDeprecatedFrameActor = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::SetNxFluidEmitterDesc(
    const NxFluidEmitterDesc& kEmitterDesc)
{
    // Does not set the actor or modify the NxFluidEmitter pointer
    
    m_kDesc = kEmitterDesc;
    m_kName = kEmitterDesc.name;
    m_kDesc.name = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::SetEmitter(NxFluidEmitter* pkEmitter,
    const NxMat34& kXform, NiTMap<NxShape*, NiPhysXShapeDescPtr>& kShapeMap)
{
    m_pkEmitter = pkEmitter;

    m_pkEmitter->saveToDesc(m_kDesc);

    if (m_pkEmitter->getName())
        m_kName = NiFixedString(m_pkEmitter->getName());
    else
        m_kName = NiFixedString(NULL);

    if (m_pkEmitter->getFrameShape())
    {
        NxShape* pkShape = m_pkEmitter->getFrameShape();
        bool bFoundShape = kShapeMap.GetAt(pkShape, m_spFrameShape);
        if (!bFoundShape)
        {
            m_spFrameShape = 0;
            m_kDesc.relPose = kXform * m_kDesc.relPose;
        }
    }
    else
    {
        m_spFrameShape = 0;
        m_kDesc.relPose = kXform * m_kDesc.relPose;
    }    

}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::ToFluidEmitterDesc(
    NxFluidEmitterDesc& kEmitterDesc, const NxMat34& kXform)
{
    kEmitterDesc = m_kDesc;
    
    kEmitterDesc.name = m_kName;

    if (m_spFrameShape)
    {
        kEmitterDesc.frameShape = m_spFrameShape->GetShape();
        kEmitterDesc.relPose = m_kDesc.relPose;
    }
    else
    {
        kEmitterDesc.frameShape = 0;
        kEmitterDesc.relPose.multiply(kXform, m_kDesc.relPose);
    }
}
//---------------------------------------------------------------------------
NxFluidEmitter* NiPhysXFluidEmitterDesc::CreateEmitter(NxFluid* pkFluid,
    const NxMat34& kXform)
{
    NxFluidEmitterDesc kEmitterDesc;
    ToFluidEmitterDesc(kEmitterDesc, kXform);
            
    m_pkEmitter = pkFluid->createEmitter(kEmitterDesc);
#ifdef NIDEBUG
    if (!m_pkEmitter)
    {
        NiOutputDebugString("NiPhysXFluidEmitterDesc::CreateEmitter: "
            "Emitter creation failed.\n");
        NiOutputDebugString("NiPhysXFluidEmitterDesc::CreateEmitter: "
            "Look for a message from the "
            "PhysX SDK as to why this might have occurred.\n");
        return 0;
    }
#endif
    
    return m_pkEmitter;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::RemoveEmitter(NxFluid* pkFluid)
{
    if (m_pkEmitter)
    {
        pkFluid->releaseEmitter(*m_pkEmitter);
        m_pkEmitter = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXFluidEmitterDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::CopyMembers(NiPhysXFluidEmitterDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kDesc = m_kDesc;

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

    pkDest->m_spFrameShape = m_spFrameShape;
    
    pkDest->m_pkEmitter = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXFluidEmitterDesc* pkClone = (NiPhysXFluidEmitterDesc*) pkCloneObj;

    if (m_spFrameShape)
    {
        NiObject* pkClonedFrameObj = 0;
        bCloned = kCloning.m_pkCloneMap->GetAt(m_spFrameShape, 
            pkClonedFrameObj);        
        NiPhysXShapeDesc* pkClonedFrame = (NiPhysXShapeDesc*) pkClonedFrameObj;
        if (bCloned)
        {
            pkClone->m_spFrameShape = pkClonedFrame;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitterDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    kStream.RegisterFixedString(m_kName);

    if (m_spFrameShape)
        m_spFrameShape->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    NiPhysXTypes::NxMat34SaveBinary(kStream, m_kDesc.relPose);
    NiStreamSaveBinary(kStream, m_kDesc.type);
    NiStreamSaveBinary(kStream, m_kDesc.maxParticles);
    NiStreamSaveBinary(kStream, m_kDesc.shape);
    NiStreamSaveBinary(kStream, m_kDesc.dimensionX);
    NiStreamSaveBinary(kStream, m_kDesc.dimensionY);
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kDesc.randomPos);
    NiStreamSaveBinary(kStream, m_kDesc.randomAngle);
    NiStreamSaveBinary(kStream, m_kDesc.fluidVelocityMagnitude);
    NiStreamSaveBinary(kStream, m_kDesc.rate);
    NiStreamSaveBinary(kStream, m_kDesc.particleLifetime);
    NiStreamSaveBinary(kStream, m_kDesc.flags);
    NiStreamSaveBinary(kStream, m_kDesc.repulsionCoefficient);
    
    kStream.SaveFixedString(m_kName);
        

    kStream.SaveLinkID(m_spFrameShape);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidEmitterDesc);
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiPhysXTypes::NxMat34LoadBinary(kStream, m_kDesc.relPose);
    NiStreamLoadBinary(kStream, m_kDesc.type);
    NiStreamLoadBinary(kStream, m_kDesc.maxParticles);
    NiStreamLoadBinary(kStream, m_kDesc.shape);
    NiStreamLoadBinary(kStream, m_kDesc.dimensionX);
    NiStreamLoadBinary(kStream, m_kDesc.dimensionY);
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kDesc.randomPos);
    NiStreamLoadBinary(kStream, m_kDesc.randomAngle);
    NiStreamLoadBinary(kStream, m_kDesc.fluidVelocityMagnitude);
    NiStreamLoadBinary(kStream, m_kDesc.rate);
    NiStreamLoadBinary(kStream, m_kDesc.particleLifetime);
    NiStreamLoadBinary(kStream, m_kDesc.flags);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 10))
    {
        NiStreamLoadBinary(kStream, m_kDesc.repulsionCoefficient);
    }
    
    kStream.LoadFixedString(m_kName);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 6))
        m_spFrameShape = (NiPhysXShapeDesc*)kStream.ResolveLinkID();
    else
        m_spDeprecatedFrameActor = (NiPhysXActorDesc*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitterDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    if (m_spDeprecatedFrameActor)
    {
        m_spFrameShape = m_spDeprecatedFrameActor->GetActorShapes().GetAt(0);
        NIASSERT(m_spFrameShape);
        
        NxMat34 kShapeXform = m_spFrameShape->GetLocalPose();
        
        m_kDesc.relPose = kShapeXform * m_kDesc.relPose;
    }
}
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitterDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXFluidEmitterDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXFluidEmitterDesc* pkNewObject = (NiPhysXFluidEmitterDesc*)pkObject;
    
    if (m_kName != pkNewObject->m_kName)
        return false;
    
    NiMatrix3 kNewM;
    NiPoint3 kNewP;
    NiMatrix3 kThisM;
    NiPoint3 kThisP;
    
    NiPhysXTypes::NxMat34ToNiTransform(pkNewObject->m_kDesc.relPose,
        kNewM, kNewP);
    NiPhysXTypes::NxMat34ToNiTransform(m_kDesc.relPose, kThisM, kThisP);
    if (kNewM != kThisM || kNewP != kThisP)
        return false;

    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kDesc.randomPos, kNewP);
    NiPhysXTypes::NxVec3ToNiPoint3(m_kDesc.randomPos, kThisP);
    if (kNewP != kThisP)
        return false;
    
    if (pkNewObject->m_kDesc.type != m_kDesc.type ||
        pkNewObject->m_kDesc.maxParticles != m_kDesc.maxParticles ||
        pkNewObject->m_kDesc.shape != m_kDesc.shape ||
        pkNewObject->m_kDesc.dimensionX != m_kDesc.dimensionX ||
        pkNewObject->m_kDesc.dimensionY != m_kDesc.dimensionY ||
        pkNewObject->m_kDesc.randomAngle != m_kDesc.randomAngle ||
        pkNewObject->m_kDesc.fluidVelocityMagnitude !=
            m_kDesc.fluidVelocityMagnitude ||
        pkNewObject->m_kDesc.rate != m_kDesc.rate ||
        pkNewObject->m_kDesc.particleLifetime != m_kDesc.particleLifetime ||
        pkNewObject->m_kDesc.flags != m_kDesc.flags ||
        pkNewObject->m_kDesc.repulsionCoefficient != 
            m_kDesc.repulsionCoefficient)
        return false;

    if ((m_spFrameShape && !pkNewObject->m_spFrameShape) ||
        (!m_spFrameShape && pkNewObject->m_spFrameShape))
        return false;
        
    if (m_spFrameShape &&
        (!m_spFrameShape->IsEqual(pkNewObject->m_spFrameShape)))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
