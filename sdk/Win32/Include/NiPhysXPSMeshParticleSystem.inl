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
inline 
NiPhysXPSMeshParticleSystemProp* NiPhysXPSMeshParticleSystem::GetProp() const
{
    return m_pkProp;
}
//---------------------------------------------------------------------------
inline NiPhysXScenePtr NiPhysXPSMeshParticleSystem::GetScene() const
{
    return m_pkScene;
}
//---------------------------------------------------------------------------
inline 
const NiTransform& NiPhysXPSMeshParticleSystem::GetPhysXToPSysTransform()
{
    return m_kPhysXToPSys;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetPhysXToPSysTransform(
    const NiTransform& kPhysXToPSys)
{
    m_kPhysXToPSys = kPhysXToPSys;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetScalePhysXToWorld(float fScalePToW)
{
    m_fScalePToW = fScalePToW;
}
//---------------------------------------------------------------------------
inline float NiPhysXPSMeshParticleSystem::GetScalePhysXToWorld()
{
    return m_fScalePToW;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetNeedsUpdate(
    const bool bNeedsUpdate)
{
    SetBit(bNeedsUpdate, FLAG_NEEDS_UPDATE);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshParticleSystem::GetNeedsUpdate() const
{
    return GetBit(FLAG_NEEDS_UPDATE);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshParticleSystem::GetUsesPhysXRotations() const
{
    return GetBit(FLAG_PHYSX_ROTATIONS);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetUsesPhysXRotations(
    const bool bPhysXRotations)
{
    SetBit(bPhysXRotations, FLAG_PHYSX_ROTATIONS);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshParticleSystem::GetUsesPhysXSpace() const
{
    return GetBit(FLAG_PHYSX_SPACE);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetUsesPhysXSpace(
    const bool bPhysXSpace)
{
    SetBit(bPhysXSpace, FLAG_PHYSX_SPACE);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSMeshParticleSystem::GetCompartmentID() const
{
    return m_uiCompartmentID;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetCompartmentID(
    const NiUInt32 uiCompartmentID)
{
    m_uiCompartmentID = uiCompartmentID;
}
//---------------------------------------------------------------------------
inline NxActor** NiPhysXPSMeshParticleSystem::GetPhysXParticleActors()
{
    return m_ppkActors;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiPhysXPSMeshParticleSystem::GetPhysXParticleLastVelocities()
{
    return m_pkLastVelocities;
}
//---------------------------------------------------------------------------
inline const 
NiPoint3* NiPhysXPSMeshParticleSystem::GetPhysXParticleLastVelocities() const
{
    return m_pkLastVelocities;
}
//---------------------------------------------------------------------------
inline NxMat34* NiPhysXPSMeshParticleSystem::GetPhysXParticleActorPose()
{
    return m_pkActorPose;
}
//---------------------------------------------------------------------------
inline 
const NxMat34* NiPhysXPSMeshParticleSystem::GetPhysXParticleActorPose() const
{
    return m_pkActorPose;
}
//---------------------------------------------------------------------------
inline NxVec3* NiPhysXPSMeshParticleSystem::GetPhysXParticleActorVelocity()
{
    return m_pkActorVelocity;
}
//---------------------------------------------------------------------------
inline const 
NxVec3* NiPhysXPSMeshParticleSystem::GetPhysXParticleActorVelocity() const
{
    return m_pkActorVelocity;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetActorAt(NiUInt16 usIndex,
    NxActor* pkActor)
{
    m_ppkActors[usIndex] = pkActor;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSMeshParticleSystem::GetNumGenerations()
{
    return m_kActorPools.GetSize();
}
//---------------------------------------------------------------------------
inline int NiPhysXPSMeshParticleSystem::GetActorPoolSize(
    NiUInt32 uiGeneration)
{
    return m_kActorPools.GetAt(uiGeneration)->GetAllocatedSize();
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetActorPoolSize(
    NiUInt32 uiGeneration, 
    NiUInt32 uiSize)
{
    NIASSERT(uiGeneration < m_kActorPools.GetSize());

    m_kActorPools.GetAt(uiGeneration)->SetSize(uiSize);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::GetActorPoolRegion(NiPoint3& kCenter,
    NiPoint3& kDim)
{
    kCenter = m_kActorPoolCenter;
    kDim = m_kActorPoolDim;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetActorPoolRegion(
    const NiPoint3 kCenter,
    const NiPoint3 kDim)
{
    m_kActorPoolDim = kDim;
    m_kActorPoolCenter = kCenter;
}
//---------------------------------------------------------------------------
inline int NiPhysXPSMeshParticleSystem::GetActorPoolCount(
    NiUInt32 uiGeneration)
{
    NIASSERT(uiGeneration < m_kActorPools.GetSize());

    return m_kActorPools.GetAt(uiGeneration)->GetSize();
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshParticleSystem::IsActorPoolEmpty(
    NiUInt32 uiGeneration)
{
    NIASSERT(uiGeneration < m_kActorPools.GetSize());

    return (m_kActorPools.GetAt(uiGeneration)->GetSize() == 0);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshParticleSystem::GetFillActorPoolsOnLoad()
{
    return GetBit(FLAG_FILL_POOLS_ON_LOAD);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetKeepsActorMeshes(bool bKeep)
{
    SetBit(bKeep, FLAG_KEEP_ACTOR_MESHES);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshParticleSystem::GetKeepsActorMeshes()
{
    return GetBit(FLAG_KEEP_ACTOR_MESHES);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetFillActorPoolsOnLoad(bool bFill)
{
    SetBit(bFill, FLAG_FILL_POOLS_ON_LOAD);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSMeshParticleSystem::GetDefaultActorPoolsSize()
{
    return m_uiDefaultActorPoolSize;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshParticleSystem::SetDefaultActorPoolsSize(
    NiUInt32 uiSize)
{
    m_uiDefaultActorPoolSize = uiSize;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDesc* NiPhysXPSMeshParticleSystem::GetActorDescAt(
    NiUInt32 uiIndex) const
{
    if (uiIndex < m_kActorDescriptors.GetSize())
    {
        return m_kActorDescriptors.GetAt(uiIndex);
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXPSMeshParticleSystem::SetActorDescAt(
    NiUInt32 uiIndex, NiPhysXActorDesc* pkActor)
{
    if (uiIndex < m_kActorDescriptors.GetSize())
    {
        NiPhysXActorDescPtr spFormerActor = m_kActorDescriptors.GetAt(uiIndex);
        m_kActorDescriptors.SetAt(uiIndex, pkActor);
        return spFormerActor;
    }

    m_kActorDescriptors.SetAtGrow(uiIndex, pkActor);
    return NULL;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPhysXPSMeshParticleSystem::GetActorDescCount() const
{
    NIASSERT(m_kActorDescriptors.GetSize() < (NiUInt16)0xFFFF);
    return (NiUInt16)m_kActorDescriptors.GetSize();
}
//---------------------------------------------------------------------------
