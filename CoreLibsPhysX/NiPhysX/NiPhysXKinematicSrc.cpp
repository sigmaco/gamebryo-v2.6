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

NiImplementRTTI(NiPhysXKinematicSrc, NiPhysXRigidBodySrc);

//---------------------------------------------------------------------------
NiPhysXKinematicSrc::NiPhysXKinematicSrc()
{
}
//---------------------------------------------------------------------------
NiPhysXKinematicSrc::NiPhysXKinematicSrc(NiAVObject* pkSource,
    NxActor* pkTarget) : NiPhysXRigidBodySrc(pkSource, pkTarget)
{
}
//---------------------------------------------------------------------------
NiPhysXKinematicSrc::~NiPhysXKinematicSrc()
{
}
//---------------------------------------------------------------------------
void NiPhysXKinematicSrc::UpdateToActors(const float, 
    const float fTEnd)
{
    if (!m_bActive || !m_pkTarget)
        return;
        
    // All the hard work is done in NiPhysXRigidBodySrc::Interpolate
    if (!m_bInterp)
    {
        m_pkTarget->moveGlobalPose(m_akPoses[m_aucIndices[1]]);
    }
    else if ( fTEnd <= m_afTimes[m_aucIndices[0]] )
    {
        m_pkTarget->moveGlobalPose(m_akPoses[m_aucIndices[0]]);
    }
    else
    {
        NxMat34 kPose;
        Interpolate(fTEnd, kPose);
        m_pkTarget->moveGlobalPose(kPose);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXKinematicSrc);
//---------------------------------------------------------------------------
void NiPhysXKinematicSrc::CopyMembers(NiPhysXKinematicSrc* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXRigidBodySrc::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiPhysXKinematicSrc::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXRigidBodySrc::ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXKinematicSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXRigidBodySrc::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXKinematicSrc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXKinematicSrc::SaveBinary(NiStream& kStream)
{
    NiPhysXRigidBodySrc::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXKinematicSrc);
//---------------------------------------------------------------------------
void NiPhysXKinematicSrc::LoadBinary(NiStream& kStream)
{
    NiPhysXRigidBodySrc::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXKinematicSrc::LinkObject(NiStream& kStream)
{
    NiPhysXRigidBodySrc::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXKinematicSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXKinematicSrc, pkObject));
    if(!NiPhysXRigidBodySrc::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
