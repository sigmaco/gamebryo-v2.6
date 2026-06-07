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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"

NiImplementRTTI(NiPhysXDynamicSrc, NiPhysXRigidBodySrc);

//---------------------------------------------------------------------------
NiPhysXDynamicSrc::NiPhysXDynamicSrc()
{
}
//---------------------------------------------------------------------------
NiPhysXDynamicSrc::NiPhysXDynamicSrc(NiAVObject* pkSource, NxActor* pkTarget)
    : NiPhysXRigidBodySrc(pkSource, pkTarget)
{
}
//---------------------------------------------------------------------------
NiPhysXDynamicSrc::~NiPhysXDynamicSrc()
{
}
//---------------------------------------------------------------------------
void NiPhysXDynamicSrc::UpdateToActors(const float fTBegin, 
    const float fTEnd)
{
    if (!m_bActive || !m_pkTarget)
        return;
        
    if (m_pkTarget->readBodyFlag(NX_BF_KINEMATIC))
    {
        if (!m_bInterp)
        {
            if (m_bForcedUpdate)
                m_pkTarget->setGlobalPose(m_akPoses[m_aucIndices[1]]);
            else
                m_pkTarget->moveGlobalPose(m_akPoses[m_aucIndices[1]]);
        }
        else if ( fTEnd <= m_afTimes[m_aucIndices[0]] )
        {
            if (m_bForcedUpdate)
                m_pkTarget->setGlobalPose(m_akPoses[m_aucIndices[0]]);
            else
                m_pkTarget->moveGlobalPose(m_akPoses[m_aucIndices[0]]);
        }
        else
        {
            NxMat34 kPose;
            Interpolate(fTEnd, kPose);
            if (m_bForcedUpdate)
                m_pkTarget->setGlobalPose(kPose);
            else
                m_pkTarget->moveGlobalPose(kPose);
        }
    }
    else
    {
        bool bHaveVelocities = CanComputeVelocities();
        if (!m_bInterp || !bHaveVelocities ||
            fTBegin == m_afTimes[m_aucIndices[1]])
        {
            // In this case, just push the latest Gamebryo state into the
            // target and use the Gamebryo velocity estimate.
            m_pkTarget->setGlobalPose(m_akPoses[m_aucIndices[1]]);

            if (bHaveVelocities)
            {
                NxMat34 kBodyLocalPose = m_pkTarget->getCMassLocalPose();
                NxVec3 kAngVel;
                NxVec3 kLinVel;
                ComputeVelocities(kBodyLocalPose, kLinVel, kAngVel);
                m_pkTarget->setLinearVelocity(kLinVel);
                m_pkTarget->setAngularVelocity(kAngVel);
            }
        }
        else
        {
            // Compute COM velocities
            NxMat34 kBodyLocalPose = m_pkTarget->getCMassLocalPose();
            NxMat34 kBodyWorldPose;
            NxVec3 kAngVel;
            NxVec3 kLinVel;
            ComputeDynamicData(fTBegin, kBodyLocalPose, kBodyWorldPose,
                kLinVel, kAngVel);

            m_pkTarget->setCMassGlobalPose(kBodyWorldPose);

            m_pkTarget->setLinearVelocity(kLinVel);
            m_pkTarget->setAngularVelocity(kAngVel);
        }
        
    }
    m_bForcedUpdate = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXDynamicSrc);
//---------------------------------------------------------------------------
void NiPhysXDynamicSrc::CopyMembers(NiPhysXDynamicSrc* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXRigidBodySrc::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiPhysXDynamicSrc::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXRigidBodySrc::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXDynamicSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXRigidBodySrc::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXDynamicSrc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXDynamicSrc::SaveBinary(NiStream& kStream)
{
    NiPhysXRigidBodySrc::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXDynamicSrc);
//---------------------------------------------------------------------------
void NiPhysXDynamicSrc::LoadBinary(NiStream& kStream)
{
    NiPhysXRigidBodySrc::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20,3,0,3))
    {
        kStream.ResolveLinkID();
        NiUInt32 uiSeqIndex;
        NiStreamLoadBinary(kStream, uiSeqIndex);
    }
}
//---------------------------------------------------------------------------
void NiPhysXDynamicSrc::LinkObject(NiStream& kStream)
{
    NiPhysXRigidBodySrc::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXDynamicSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXDynamicSrc, pkObject));
    if(!NiPhysXRigidBodySrc::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
