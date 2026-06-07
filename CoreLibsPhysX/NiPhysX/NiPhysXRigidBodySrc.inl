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

//---------------------------------------------------------------------------
inline NiAVObject* NiPhysXRigidBodySrc::GetSource() const
{
    return m_pkSource;
}
//---------------------------------------------------------------------------
inline void NiPhysXRigidBodySrc::SetTarget(NxActor* pkActor)
{
    m_pkTarget = pkActor;
}
//---------------------------------------------------------------------------
inline NxActor* NiPhysXRigidBodySrc::GetTarget() const
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline float NiPhysXRigidBodySrc::GetTime(NiUInt32 uiIndex) const
{
    return m_afTimes[m_aucIndices[uiIndex]];
}
//---------------------------------------------------------------------------
inline NxMat34 NiPhysXRigidBodySrc::GetPose(NiUInt32 uiIndex) const
{
    return m_akPoses[m_aucIndices[uiIndex]];
}
//---------------------------------------------------------------------------
inline void NiPhysXRigidBodySrc::ResetVelocity()
{
    m_ucResetVelocity = 2;
    m_bForcedUpdate = false;
}
//---------------------------------------------------------------------------
inline bool NiPhysXRigidBodySrc::CanComputeVelocities() const
{
    return (m_ucResetVelocity == 0 &&
        NiAbs(m_afTimes[1] - m_afTimes[0]) > 1.0e-4);
}
//---------------------------------------------------------------------------
