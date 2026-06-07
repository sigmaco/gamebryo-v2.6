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
inline NiControllerManager* NiPhysXSequenceManager::GetControllerManager()
{
    return m_spControllerManager;
}
//---------------------------------------------------------------------------
inline NiPhysXProp* NiPhysXSequenceManager::GetProp()
{
    return m_spProp;
}
//---------------------------------------------------------------------------
inline NiPhysXRigidBodyDest* 
    NiPhysXSequenceManager::SequenceActorData::GetDestination() const
{
    return m_spDestination;
}
//---------------------------------------------------------------------------
inline void NiPhysXSequenceManager::SequenceActorData::SetDestination(
    NiPhysXRigidBodyDest* pkDestination)
{
    m_spDestination = pkDestination;
}
//---------------------------------------------------------------------------
inline NiPhysXDynamicSrc* 
    NiPhysXSequenceManager::SequenceActorData::GetSource() const
{
    return m_spSource;
}
//---------------------------------------------------------------------------
inline void NiPhysXSequenceManager::SequenceActorData::SetSource(
    NiPhysXDynamicSrc* pkSource)
{
    m_spSource = pkSource;
}
//---------------------------------------------------------------------------
inline bool NiPhysXSequenceManager::SequenceData::IsActive() const
{
    return m_pkActiveAnimSeq != 0;
}
//---------------------------------------------------------------------------
inline NiPhysXSequenceManager::ActorDataArray& 
    NiPhysXSequenceManager::SequenceData::GetActorDataArray()
{
    return m_kActorData;
}
//---------------------------------------------------------------------------
inline NiPhysXRigidBodyDest* 
    NiPhysXSequenceManager::SequenceData::GetAccumDestination() const
{
    return m_pkAccumDest;
}
//---------------------------------------------------------------------------
inline void NiPhysXSequenceManager::SequenceData::SetAccumDestination(
    NiPhysXRigidBodyDest* pkAccumDest)
{
    m_pkAccumDest = pkAccumDest;
}
//---------------------------------------------------------------------------
inline NiControllerSequence* 
    NiPhysXSequenceManager::SequenceData::GetActiveAnimSequence() const
{
    return m_pkActiveAnimSeq;
}
//---------------------------------------------------------------------------
inline void NiPhysXSequenceManager::SequenceData::SetActiveAnimSequence(
    NiControllerSequence* pkActiveAnimSeq)
{
    m_pkActiveAnimSeq = pkActiveAnimSeq;
    m_uiTotalPoseBufferItems = 0;
}
//---------------------------------------------------------------------------
