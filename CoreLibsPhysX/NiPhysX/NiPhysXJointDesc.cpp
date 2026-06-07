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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiImplementRTTI(NiPhysXJointDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXJointDesc::NiPhysXJointDesc() : m_kJointName(NULL)
{
    m_eType = NX_JOINT_COUNT;
    m_aspActors[0] = 0;
    m_aspActors[1] = 0;
    m_akLocalNormal[0].set(1.0f, 0.0f, 0.0f);
    m_akLocalNormal[1].set(1.0f, 0.0f, 0.0f);
    m_akLocalAxis[0].set(0.0f, 0.0f, 1.0f);
    m_akLocalAxis[1].set(0.0f, 0.0f, 1.0f);
    m_akLocalAnchor[0].zero();
    m_akLocalAnchor[1].zero();
    m_fMaxForce = NX_MAX_REAL;
    m_fMaxTorque = NX_MAX_REAL;
    m_fSolverExtrapolationFactor = 1.0f;
    m_uiUseAccelerationSpring = 0;
    m_uiJointFlags = NX_JF_VISUALIZATION;
    
    m_kLimitPoint.zero();
    m_uiNumLimits = 0;
    m_pkLimitPlaneNorms = 0;
    m_pkLimitPlaneDs = 0;
    m_pkLimitPlaneRs = 0;
}
//---------------------------------------------------------------------------
NiPhysXJointDesc::~NiPhysXJointDesc()
{
    m_aspActors[0] = 0;
    m_aspActors[1] = 0;
    
    NiFree(m_pkLimitPlaneNorms);
    NiFree(m_pkLimitPlaneDs);
    NiFree(m_pkLimitPlaneRs);
}
//---------------------------------------------------------------------------
NiPhysXJointDesc* NiPhysXJointDesc::CreateFromJoint(NxJoint* pkJoint,
    NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap, const NxMat34& kXform)
{
    NxD6Joint* pkD6Joint = pkJoint->isD6Joint();
    if (!pkD6Joint)
    {
        NiOutputDebugString(
            "NiPhysXJointDesc::CreateFromJoint only "
            "support D6 type joints.\n");
        return 0;
    }
    
    NiPhysXD6JointDesc* pkD6JointDesc = NiNew NiPhysXD6JointDesc;
    pkD6JointDesc->FromJoint(pkD6Joint, kActorMap, kXform);
    
    return pkD6JointDesc;
}
//---------------------------------------------------------------------------
NiPhysXActorDescPtr NiPhysXJointDesc::GetActor(const NiUInt32 uiIndex)
{
    return m_aspActors[uiIndex];
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetActor(const NiUInt32 uiIndex,
    NiPhysXActorDesc* kActorDesc)
{
    m_aspActors[uiIndex] = kActorDesc;
}
//---------------------------------------------------------------------------
NxVec3 NiPhysXJointDesc::GetLimitPoint() const
{
    return m_kLimitPoint;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetLimitPoint(const NxVec3& kLimitPt)
{
    m_kLimitPoint = kLimitPt;
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXJointDesc::GetNumLimits() const
{
    return m_uiNumLimits;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::GetLimitPlane(const NiUInt32 uiIndex,
    NxVec3& kNorm, NxReal& kD, NxReal& kR)
{
    kNorm = m_pkLimitPlaneNorms[uiIndex];
    kD = m_pkLimitPlaneDs[uiIndex];
    kR = m_pkLimitPlaneRs[uiIndex];
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetLimitPlane(const NiUInt32 uiIndex, 
    const NxVec3& kNorm, const NxReal& kD, const NxReal& kR)
{
    if (m_uiNumLimits <= uiIndex)
    {
        NxVec3* akNewNorms = NiAlloc(NxVec3, uiIndex + 1);
        NxReal* akNewDs = NiAlloc(NxReal, uiIndex + 1);
        NxReal* akNewRs = NiAlloc(NxReal, uiIndex + 1);
        for (NiUInt32 ui = 0; ui < m_uiNumLimits; ui++)
        {
            akNewNorms[ui] = m_pkLimitPlaneNorms[ui];
            akNewDs[ui] = m_pkLimitPlaneDs[ui];
            akNewRs[ui] = m_pkLimitPlaneRs[ui];
        }
        NiFree(m_pkLimitPlaneNorms);
        NiFree(m_pkLimitPlaneDs);
        NiFree(m_pkLimitPlaneRs);
        m_pkLimitPlaneNorms = akNewNorms;
        m_pkLimitPlaneDs = akNewDs;
        m_pkLimitPlaneRs = akNewRs;
        m_uiNumLimits = uiIndex + 1;
    }

    m_pkLimitPlaneNorms[uiIndex] = kNorm;
    m_pkLimitPlaneDs[uiIndex] = kD;
    m_pkLimitPlaneRs[uiIndex] = kR;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SetJointDesc(const NxJointDesc& kJointDesc)
{
    m_eType = kJointDesc.getType();

    if (kJointDesc.name)
        m_kJointName = NiFixedString(kJointDesc.name);
    else
        m_kJointName = NiFixedString(NULL);
        
    m_akLocalNormal[0] = kJointDesc.localNormal[0];
    m_akLocalNormal[1] = kJointDesc.localNormal[1];
    m_akLocalAxis[0] = kJointDesc.localAxis[0];
    m_akLocalAxis[1] = kJointDesc.localAxis[1];
    m_akLocalAnchor[0] = kJointDesc.localAnchor[0];
    m_akLocalAnchor[1] = kJointDesc.localAnchor[1];
    m_fMaxForce = kJointDesc.maxForce;
    m_fMaxTorque = kJointDesc.maxTorque;
    m_fSolverExtrapolationFactor = kJointDesc.solverExtrapolationFactor;
    m_uiUseAccelerationSpring = kJointDesc.useAccelerationSpring;
    m_uiJointFlags = kJointDesc.jointFlags; 
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::GetJointDesc(NxJointDesc& kJointDesc,
    const NxMat34& kXform) const
{
    kJointDesc.name = m_kJointName;
        
    if (m_aspActors[0])
    {
        kJointDesc.actor[0] = m_aspActors[0]->GetActor();
        kJointDesc.localNormal[0] = m_akLocalNormal[0];
        kJointDesc.localAxis[0] = m_akLocalAxis[0];
        kJointDesc.localAnchor[0] = m_akLocalAnchor[0];
    }
    else
    {
        kJointDesc.actor[0] = NULL;
        kJointDesc.localNormal[0] = kXform.M * m_akLocalNormal[0];
        kJointDesc.localAxis[0] = kXform.M * m_akLocalAxis[0];
        kJointDesc.localAnchor[0] = kXform * m_akLocalAnchor[0];
    }

    if (m_aspActors[1])
    {
        kJointDesc.actor[1] = m_aspActors[1]->GetActor();
        kJointDesc.localNormal[1] = m_akLocalNormal[1];
        kJointDesc.localAxis[1] = m_akLocalAxis[1];
        kJointDesc.localAnchor[1] = m_akLocalAnchor[1];
    }
    else
    {
        kJointDesc.actor[1] = NULL;
        kJointDesc.localNormal[1] = kXform.M * m_akLocalNormal[1];
        kJointDesc.localAxis[1] = kXform.M * m_akLocalAxis[1];
        kJointDesc.localAnchor[1] = kXform * m_akLocalAnchor[1];
    }

    kJointDesc.maxForce = m_fMaxForce;
    kJointDesc.maxTorque = m_fMaxTorque;
    kJointDesc.solverExtrapolationFactor = m_fSolverExtrapolationFactor;
    kJointDesc.useAccelerationSpring = m_uiUseAccelerationSpring;
    kJointDesc.jointFlags = m_uiJointFlags; 
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::FromJoint(NxJoint* pkJoint)
{
    // Can't get descriptor from here, so caller must ensure descriptor is
    // extracted.
    
    pkJoint->getLimitPoint(m_kLimitPoint);
    
    NiFree(m_pkLimitPlaneNorms);
    m_pkLimitPlaneNorms = 0;
    NiFree(m_pkLimitPlaneDs);
    m_pkLimitPlaneDs = 0;
    NiFree(m_pkLimitPlaneRs);
    m_pkLimitPlaneRs = 0;
    m_uiNumLimits = 0;
    pkJoint->resetLimitPlaneIterator();
    while (pkJoint->hasMoreLimitPlanes())
    {
        NxVec3 kDummy;
        NxReal fDummy;
        pkJoint->getNextLimitPlane(kDummy, fDummy);
        m_uiNumLimits++;
    }
    m_pkLimitPlaneNorms = NiAlloc(NxVec3, m_uiNumLimits);
    m_pkLimitPlaneDs = NiAlloc(NxReal, m_uiNumLimits);
    m_pkLimitPlaneRs = NiAlloc(NxReal, m_uiNumLimits);
    m_uiNumLimits = 0;
    pkJoint->resetLimitPlaneIterator();
    while (pkJoint->hasMoreLimitPlanes())
    {
        pkJoint->getNextLimitPlane(m_pkLimitPlaneNorms[m_uiNumLimits],
            m_pkLimitPlaneDs[m_uiNumLimits],
            m_pkLimitPlaneRs + m_uiNumLimits);
        m_uiNumLimits++;
    }
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::ToJoint(NxJoint* pkJoint)
{
    // Always assume point is on actor 1. We have no way of finding otherwise.
    pkJoint->setLimitPoint(m_kLimitPoint);
    
    for (NiUInt32 i = 0; i < m_uiNumLimits; i++)
    {
        NxVec3 kPtOnPlane = -m_pkLimitPlaneDs[i] * m_pkLimitPlaneNorms[i];
        pkJoint->addLimitPlane(m_pkLimitPlaneNorms[i], kPtOnPlane,
            m_pkLimitPlaneRs[i]);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXJointDesc);
//---------------------------------------------------------------------------
void NiPhysXJointDesc::CopyMembers(NiPhysXJointDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_eType = m_eType;

    // copy object names
    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kJointName = m_kJointName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kJointName)
        {
            size_t stQuantity = strlen(m_kJointName) + 2;
            char* pcNamePlus = NiAlloc(char, stQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, stQuantity, m_kJointName);
            pcNamePlus[stQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[stQuantity - 1] = 0;
            pkDest->m_kJointName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }
    
    pkDest->m_aspActors[0] = m_aspActors[0];
    pkDest->m_aspActors[1] = m_aspActors[1];
    pkDest->m_akLocalNormal[0] = m_akLocalNormal[0];
    pkDest->m_akLocalNormal[1] = m_akLocalNormal[1];
    pkDest->m_akLocalAxis[0] = m_akLocalAxis[0];
    pkDest->m_akLocalAxis[1] = m_akLocalAxis[1];
    pkDest->m_akLocalAnchor[0] = m_akLocalAnchor[0];
    pkDest->m_akLocalAnchor[1] = m_akLocalAnchor[1];
    
    pkDest->m_fMaxForce = m_fMaxForce;
    pkDest->m_fMaxTorque = m_fMaxTorque;
    pkDest->m_fSolverExtrapolationFactor = m_fSolverExtrapolationFactor;
    pkDest->m_uiUseAccelerationSpring = m_uiUseAccelerationSpring;
    pkDest->m_uiJointFlags = m_uiJointFlags;
    
    pkDest->m_kLimitPoint = m_kLimitPoint;
    pkDest->m_uiNumLimits = m_uiNumLimits;
    
    pkDest->m_pkLimitPlaneNorms = NiAlloc(NxVec3, m_uiNumLimits);
    pkDest->m_pkLimitPlaneDs = NiAlloc(NxReal, m_uiNumLimits);
    pkDest->m_pkLimitPlaneRs = NiAlloc(NxReal, m_uiNumLimits);
    for (NiUInt32 ui = 0; ui < m_uiNumLimits; ui++)
    {
        pkDest->m_pkLimitPlaneNorms[ui] = m_pkLimitPlaneNorms[ui];
        pkDest->m_pkLimitPlaneDs[ui] = m_pkLimitPlaneDs[ui];
        pkDest->m_pkLimitPlaneRs[ui] = m_pkLimitPlaneRs[ui];
    }
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
    
    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXJointDesc* pkClone = (NiPhysXJointDesc*) pkCloneObj;
            
    NiObject* pkClonedActorObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(m_aspActors[0], pkClonedActorObj);
    NiPhysXActorDesc* pkClonedActor = (NiPhysXActorDesc*) pkClonedActorObj;
    if (bCloned)
    {
       pkClone->m_aspActors[0] = pkClonedActor;
    }

    bCloned = kCloning.m_pkCloneMap->
        GetAt(m_aspActors[1], pkClonedActorObj);
    pkClonedActor = (NiPhysXActorDesc*) pkClonedActorObj;
    if (bCloned)
    {
       pkClone->m_aspActors[1] = pkClonedActor;
    }
    
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXJointDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;
    
    kStream.RegisterFixedString(m_kJointName);

    if (m_aspActors[0])
        m_aspActors[0]->RegisterStreamables(kStream);
    if (m_aspActors[1])
        m_aspActors[1]->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXJointDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveEnum(kStream, m_eType);
    kStream.SaveFixedString(m_kJointName);

    for (NiUInt32 i = 0; i < 2; i++)
    {
        kStream.SaveLinkID(m_aspActors[i]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_akLocalNormal[i]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_akLocalAxis[i]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_akLocalAnchor[i]);
    }

    NiStreamSaveBinary(kStream, m_fMaxForce);
    NiStreamSaveBinary(kStream, m_fMaxTorque);
    NiStreamSaveBinary(kStream, m_fSolverExtrapolationFactor);
    NiStreamSaveBinary(kStream, m_uiUseAccelerationSpring);
    NiStreamSaveBinary(kStream, m_uiJointFlags);

    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kLimitPoint);
    NiStreamSaveBinary(kStream, m_uiNumLimits);
    for (NiUInt32 i = 0; i < m_uiNumLimits; i++)
    {
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_pkLimitPlaneNorms[i]);
        NiStreamSaveBinary(kStream, m_pkLimitPlaneDs[i]);
        NiStreamSaveBinary(kStream, m_pkLimitPlaneRs[i]);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXJointDesc);
//---------------------------------------------------------------------------
void NiPhysXJointDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadEnum(kStream, m_eType);
    kStream.LoadFixedString(m_kJointName);

    for (NiUInt32 i = 0; i < 2; i++)
    {
        m_aspActors[i] = (NiPhysXActorDesc*)kStream.ResolveLinkID();
        
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_akLocalNormal[i]);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_akLocalAxis[i]);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_akLocalAnchor[i]);
    }

    NiStreamLoadBinary(kStream, m_fMaxForce);
    NiStreamLoadBinary(kStream, m_fMaxTorque);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 5, 0, 3))
    {
        NiStreamLoadBinary(kStream, m_fSolverExtrapolationFactor);
        NiStreamLoadBinary(kStream, m_uiUseAccelerationSpring);
    }

    NiStreamLoadBinary(kStream, m_uiJointFlags);
    
    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kLimitPoint);
    NiStreamLoadBinary(kStream, m_uiNumLimits);
    for (NiUInt32 i = 0; i < m_uiNumLimits; i++)
    {
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_pkLimitPlaneNorms[i]);
        NiStreamLoadBinary(kStream, m_pkLimitPlaneDs[i]);

        if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
        {
            NiStreamLoadBinary(kStream, m_pkLimitPlaneRs[i]);
        }
        else
        {
            m_pkLimitPlaneRs[i] = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXJointDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXJointDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXJointDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXJointDesc* pkNewObject = (NiPhysXJointDesc*)pkObject;
    
    if (m_eType != pkNewObject->m_eType ||
        m_kJointName != pkNewObject->m_kJointName)
        return false;

    NiPoint3 kNewP;
    NiPoint3 kThisP;
    for (NiUInt32 i = 0; i < 2; i++)
    {
        if ((m_aspActors[i] && !pkNewObject->m_aspActors[i]) ||
            (!m_aspActors[i] && pkNewObject->m_aspActors[i]))
            return false;
            
        if (m_aspActors[i] &&
            (!m_aspActors[i]->IsEqual(pkNewObject->m_aspActors[i])))
        {
            return false;
        }
 
        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_akLocalNormal[i], kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_akLocalNormal[i], kThisP);
        if (kNewP != kThisP)
            return false;

        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_akLocalAxis[i], kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_akLocalAxis[i], kThisP);
        if (kNewP != kThisP)
            return false;

        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_akLocalAnchor[i], kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_akLocalAnchor[i], kThisP);
        if (kNewP != kThisP)
            return false;
    }
       
    if (pkNewObject->m_fMaxForce != m_fMaxForce  ||
        pkNewObject->m_fMaxTorque != m_fMaxTorque ||
        pkNewObject->m_fSolverExtrapolationFactor != m_fSolverExtrapolationFactor ||
        pkNewObject->m_uiUseAccelerationSpring != m_uiUseAccelerationSpring ||
        pkNewObject->m_uiJointFlags != m_uiJointFlags)
        return false;

    NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_kLimitPoint, kNewP);
    NiPhysXTypes::NxVec3ToNiPoint3(m_kLimitPoint, kThisP);
    if (kNewP != kThisP)
        return false;
    
    if (m_uiNumLimits != pkNewObject->m_uiNumLimits)
        return false;

    for (NiUInt32 i = 0; i < m_uiNumLimits; i++)
    {
        NiPhysXTypes::NxVec3ToNiPoint3(pkNewObject->m_pkLimitPlaneNorms[i],
            kNewP);
        NiPhysXTypes::NxVec3ToNiPoint3(m_pkLimitPlaneNorms[i], kThisP);
        if (kNewP != kThisP)
            return false;
        if (m_pkLimitPlaneDs[i] != pkNewObject->m_pkLimitPlaneDs[i])
            return false;
        if (m_pkLimitPlaneRs[i] != pkNewObject->m_pkLimitPlaneRs[i])
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
