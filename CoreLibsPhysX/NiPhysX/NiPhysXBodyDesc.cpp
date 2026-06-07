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

NiImplementRTTI(NiPhysXBodyDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXBodyDesc::NiPhysXBodyDesc()
{
    m_uiNumVels = 0;
    m_pkLinVels = 0;
    m_pkAngVels = 0;
}
//---------------------------------------------------------------------------
NiPhysXBodyDesc::~NiPhysXBodyDesc()
{
    NiFree(m_pkLinVels);
    NiFree(m_pkAngVels);
}
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::ToBodyDesc(NxBodyDesc& kBodyDesc, const NiUInt32 uiIndex,
    const NxMat33& kXform) const
{
    kBodyDesc = m_kBodyDesc;
    
    if (uiIndex < m_uiNumVels)
    {
        kBodyDesc.angularVelocity = kXform * m_pkAngVels[uiIndex];
        kBodyDesc.linearVelocity = kXform * m_pkLinVels[uiIndex];
    }
    else
    {
        kBodyDesc.angularVelocity.zero();
        kBodyDesc.linearVelocity.zero();
    }
}
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::SetVelocities(const NxVec3& kLinearVelocity,
    const NxVec3& kAngularVelocity, const NiUInt32 uiIndex,
    const NxMat33& kXform)
{
    if (m_uiNumVels <= uiIndex)
    {
        m_uiNumVels = uiIndex + 1;
        
        NiUInt32 uiSize = sizeof(NxVec3) * m_uiNumVels;
        m_pkLinVels = (NxVec3*)NiRealloc((void*)m_pkLinVels, uiSize); 
        m_pkAngVels = (NxVec3*)NiRealloc((void*)m_pkAngVels, uiSize); 
    }

    m_pkLinVels[uiIndex] = kXform * kLinearVelocity;
    m_pkAngVels[uiIndex] = kXform * kAngularVelocity;
}
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::RemoveStatesBeyond(const NiUInt32 uiState)
{
    if (m_uiNumVels > uiState)
    {
        m_uiNumVels = uiState;

        NiUInt32 uiSize = sizeof(NxVec3) * m_uiNumVels;
        m_pkLinVels = (NxVec3*)NiRealloc((void*)m_pkLinVels, uiSize); 
        m_pkAngVels = (NxVec3*)NiRealloc((void*)m_pkAngVels, uiSize); 

        NIASSERT(m_pkLinVels);
        NIASSERT(m_pkAngVels);
    }
}
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::RemoveState(const NiUInt32 uiState)
{
    if (m_uiNumVels > uiState)
    {
        NxVec3* pkNewLinVels = NiAlloc(NxVec3, m_uiNumVels - 1);
        NxVec3* pkNewAngVels = NiAlloc(NxVec3, m_uiNumVels - 1);
        NIASSERT(pkNewLinVels);
        NIASSERT(pkNewAngVels);

        NiUInt32 uiIndex = 0;
        for (NiUInt32 ui = 0; ui < m_uiNumVels; ui++)
        {
            if (ui != uiState)
            {
                pkNewLinVels[uiIndex] = m_pkLinVels[ui];
                pkNewAngVels[uiIndex] = m_pkAngVels[ui];
                uiIndex++;
            }
        }

        NiFree(m_pkLinVels);
        NiFree(m_pkAngVels);

        m_pkLinVels = pkNewLinVels;
        m_pkAngVels = pkNewAngVels;

        m_uiNumVels--;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXBodyDesc);
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::CopyMembers(NiPhysXBodyDesc* pkDest, 
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kBodyDesc = m_kBodyDesc;
    
    pkDest->m_uiNumVels = m_uiNumVels;
    pkDest->m_pkLinVels = NiAlloc(NxVec3, m_uiNumVels);
    pkDest->m_pkAngVels = NiAlloc(NxVec3, m_uiNumVels);
    for (NiUInt32 ui = 0; ui < m_uiNumVels; ui++)
    {
        pkDest->m_pkLinVels[ui] = m_pkLinVels[ui];
        pkDest->m_pkAngVels[ui] = m_pkAngVels[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXBodyDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiPhysXTypes::NxMat34SaveBinary(kStream, m_kBodyDesc.massLocalPose);
    
    NiPhysXTypes::NxVec3SaveBinary(kStream, m_kBodyDesc.massSpaceInertia);
    
    NiStreamSaveBinary(kStream, m_kBodyDesc.mass);

    NiStreamSaveBinary(kStream, m_uiNumVels);
    for (NiUInt32 ui = 0; ui < m_uiNumVels; ui++)
    {
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_pkLinVels[ui]);
        NiPhysXTypes::NxVec3SaveBinary(kStream, m_pkAngVels[ui]);
    }
    
    NiStreamSaveBinary(kStream, m_kBodyDesc.wakeUpCounter);
    NiStreamSaveBinary(kStream, m_kBodyDesc.linearDamping);
    NiStreamSaveBinary(kStream, m_kBodyDesc.angularDamping);
    NiStreamSaveBinary(kStream, m_kBodyDesc.maxAngularVelocity);
    NiStreamSaveBinary(kStream, m_kBodyDesc.CCDMotionThreshold);
    NiStreamSaveBinary(kStream, m_kBodyDesc.flags);
    NiStreamSaveBinary(kStream, m_kBodyDesc.sleepLinearVelocity);
    NiStreamSaveBinary(kStream, m_kBodyDesc.sleepAngularVelocity);
    NiStreamSaveBinary(kStream, m_kBodyDesc.solverIterationCount);
    NiStreamSaveBinary(kStream, m_kBodyDesc.sleepEnergyThreshold);
    NiStreamSaveBinary(kStream, m_kBodyDesc.sleepDamping);
    NiStreamSaveBinary(kStream, m_kBodyDesc.contactReportThreshold);

}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXBodyDesc);
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiPhysXTypes::NxMat34LoadBinary(kStream, m_kBodyDesc.massLocalPose);

    NiPhysXTypes::NxVec3LoadBinary(kStream, m_kBodyDesc.massSpaceInertia);
    
    NiStreamLoadBinary(kStream, m_kBodyDesc.mass);

    NiStreamLoadBinary(kStream, m_uiNumVels);
    m_pkLinVels = NiAlloc(NxVec3, m_uiNumVels);
    m_pkAngVels = NiAlloc(NxVec3, m_uiNumVels);
    for (NiUInt32 ui = 0; ui < m_uiNumVels; ui++)
    {
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_pkLinVels[ui]);
        NiPhysXTypes::NxVec3LoadBinary(kStream, m_pkAngVels[ui]);
    }

    NiStreamLoadBinary(kStream, m_kBodyDesc.wakeUpCounter);
    NiStreamLoadBinary(kStream, m_kBodyDesc.linearDamping);
    NiStreamLoadBinary(kStream, m_kBodyDesc.angularDamping);
    NiStreamLoadBinary(kStream, m_kBodyDesc.maxAngularVelocity);
    NiStreamLoadBinary(kStream, m_kBodyDesc.CCDMotionThreshold);
    NiStreamLoadBinary(kStream, m_kBodyDesc.flags);
    NiStreamLoadBinary(kStream, m_kBodyDesc.sleepLinearVelocity);
    NiStreamLoadBinary(kStream, m_kBodyDesc.sleepAngularVelocity);
    NiStreamLoadBinary(kStream, m_kBodyDesc.solverIterationCount);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kBodyDesc.sleepEnergyThreshold);
        NiStreamLoadBinary(kStream, m_kBodyDesc.sleepDamping);
    }
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_kBodyDesc.contactReportThreshold);
    }

}
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXBodyDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXBodyDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXBodyDesc* pkNewObject = (NiPhysXBodyDesc*)pkObject;

    NiMatrix3 kNewM;
    NiPoint3 kNewP;
    NiMatrix3 kThisM;
    NiPoint3 kThisP;
    
    NiPhysXTypes::NxMat34ToNiTransform(pkNewObject->m_kBodyDesc.massLocalPose,
        kNewM, kNewP);
    NiPhysXTypes::NxMat34ToNiTransform(m_kBodyDesc.massLocalPose,
        kThisM, kThisP);

    if (m_uiNumVels != pkNewObject->m_uiNumVels)
        return false;
    for (NiUInt32 ui = 0; ui < m_uiNumVels; ui++)
    {
        NxVec3 kDiff;
        kDiff = m_pkLinVels[ui] - pkNewObject->m_pkLinVels[ui];
        if (kDiff.magnitudeSquared() > 1.0e-8f)
            return false;
        kDiff = m_pkAngVels[ui] -pkNewObject->m_pkAngVels[ui];
        if (kDiff.magnitudeSquared() > 1.0e-8f)
            return false;
    }

    if (kNewM != kThisM || kNewP != kThisP ||
        pkNewObject->m_kBodyDesc.massSpaceInertia
        != m_kBodyDesc.massSpaceInertia  ||
        pkNewObject->m_kBodyDesc.mass != m_kBodyDesc.mass ||
        pkNewObject->m_kBodyDesc.wakeUpCounter != m_kBodyDesc.wakeUpCounter ||
        pkNewObject->m_kBodyDesc.linearDamping != m_kBodyDesc.linearDamping ||
        pkNewObject->m_kBodyDesc.angularDamping != m_kBodyDesc.angularDamping||
        pkNewObject->m_kBodyDesc.maxAngularVelocity
        != m_kBodyDesc.maxAngularVelocity ||
        pkNewObject->m_kBodyDesc.CCDMotionThreshold
        != m_kBodyDesc.CCDMotionThreshold ||
        pkNewObject->m_kBodyDesc.flags != m_kBodyDesc.flags ||
        pkNewObject->m_kBodyDesc.sleepLinearVelocity
        != m_kBodyDesc.sleepLinearVelocity ||
        pkNewObject->m_kBodyDesc.sleepAngularVelocity
        != m_kBodyDesc.sleepAngularVelocity ||
        pkNewObject->m_kBodyDesc.solverIterationCount
        != m_kBodyDesc.solverIterationCount ||
        pkNewObject->m_kBodyDesc.sleepEnergyThreshold
        != m_kBodyDesc.sleepEnergyThreshold ||
        pkNewObject->m_kBodyDesc.sleepDamping
        != m_kBodyDesc.sleepDamping ||
        pkNewObject->m_kBodyDesc.contactReportThreshold 
        != m_kBodyDesc.contactReportThreshold)
    {
        return false;
    }
   
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXBodyDesc::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(NiPhysXBodyDesc::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("massSpaceInertia",
        &m_kBodyDesc.massSpaceInertia));
    pkStrings->Add(NiGetViewerString("mass", m_kBodyDesc.mass));

    pkStrings->Add(NiGetViewerString("wakeUpCounter",
        m_kBodyDesc.wakeUpCounter));
    pkStrings->Add(NiGetViewerString("linearDamping",
        m_kBodyDesc.linearDamping));
    pkStrings->Add(NiGetViewerString("angularDamping",
        m_kBodyDesc.angularDamping));
    pkStrings->Add(NiGetViewerString("maxAngularVelocity",
        m_kBodyDesc.maxAngularVelocity));
    pkStrings->Add(NiGetViewerString("CCDMotionThreshold",
        m_kBodyDesc.CCDMotionThreshold));
    pkStrings->Add(NiGetViewerString("flags", m_kBodyDesc.flags));
    pkStrings->Add(NiGetViewerString("sleepLinearVelocity",
        m_kBodyDesc.sleepLinearVelocity));
    pkStrings->Add(NiGetViewerString("sleepAngularVelocity",
        m_kBodyDesc.sleepAngularVelocity));
    pkStrings->Add(NiGetViewerString("solverIterationCount",
        m_kBodyDesc.solverIterationCount));
}
//---------------------------------------------------------------------------
