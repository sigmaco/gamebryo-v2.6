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
inline NiPhysXPSParticleSystemProp* NiPhysXPSParticleSystem::GetProp() const
{
    return m_pkProp;
}
//---------------------------------------------------------------------------
inline NiPhysXScene* NiPhysXPSParticleSystem::GetScene() const
{
    return m_pkScene;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiPhysXPSParticleSystem::GetPhysXToPSysTransform()
{
    return m_kPhysXToPSys;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetPhysXToPSysTransform(
    const NiTransform& kPhysXToPSys)
{
    m_kPhysXToPSys = kPhysXToPSys;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetScalePhysXToWorld(float fScalePToW)
{
    m_fScalePToW = fScalePToW;
}
//---------------------------------------------------------------------------
inline float NiPhysXPSParticleSystem::GetScalePhysXToWorld()
{
    return m_fScalePToW;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetNeedsUpdate(
    const bool bNeedsUpdate)
{
    SetBit(bNeedsUpdate, FLAG_NEEDS_UPDATE);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSParticleSystem::GetNeedsUpdate() const
{
    return GetBit(FLAG_NEEDS_UPDATE);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSParticleSystem::GetUsesPhysXRotations() const
{
    return GetBit(FLAG_PHYSX_ROTATIONS);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetUsesPhysXRotations(
    const bool bPhysXRotations)
{
    SetBit(bPhysXRotations, FLAG_PHYSX_ROTATIONS);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSParticleSystem::GetUsesPhysXSpace() const
{
    return GetBit(FLAG_PHYSX_SPACE);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetUsesPhysXSpace(const bool bPhysXSpace)
{
    SetBit(bPhysXSpace, FLAG_PHYSX_SPACE);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSParticleSystem::GetCompartmentID() const
{
    return m_uiCompartmentID;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetCompartmentID(
    const NiUInt32 uiCompartmentID)
{
    m_uiCompartmentID = uiCompartmentID;
}
//---------------------------------------------------------------------------
inline NxActor** NiPhysXPSParticleSystem::GetPhysXParticleActors()
{
    return m_ppkActors;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiPhysXPSParticleSystem::GetPhysXParticleLastVelocities()
{
    return m_pkLastVelocities;
}
//---------------------------------------------------------------------------
inline const
NiPoint3* NiPhysXPSParticleSystem::GetPhysXParticleLastVelocities() const
{
    return m_pkLastVelocities;
}
//---------------------------------------------------------------------------
inline NxMat34* NiPhysXPSParticleSystem::GetPhysXParticleActorPose()
{
    return m_pkActorPose;
}
//---------------------------------------------------------------------------
inline 
const NxMat34* NiPhysXPSParticleSystem::GetPhysXParticleActorPose() const
{
    return m_pkActorPose;
}
//---------------------------------------------------------------------------
inline NxVec3* NiPhysXPSParticleSystem::GetPhysXParticleActorVelocity()
{
    return m_pkActorVelocity;
}
//---------------------------------------------------------------------------
inline 
const NxVec3* NiPhysXPSParticleSystem::GetPhysXParticleActorVelocity() const
{
    return m_pkActorVelocity;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetActorAt(NiUInt16 usIndex,
    NxActor* pkActor)
{
    m_ppkActors[usIndex] = pkActor;
}
//---------------------------------------------------------------------------
inline int NiPhysXPSParticleSystem::GetActorPoolSize()
{
    return m_kActorPool.GetAllocatedSize();
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetActorPoolSize(NiUInt32 uiSize)
{
    m_kActorPool.SetSize(uiSize);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::GetActorPoolRegion(NiPoint3& kCenter,
    NiPoint3& kDim)
{
    kCenter = m_kActorPoolCenter;
    kDim = m_kActorPoolDim;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetActorPoolRegion(const NiPoint3 kCenter,
    const NiPoint3 kDim)
{
    m_kActorPoolDim = kDim;
    m_kActorPoolCenter = kCenter;
}
//---------------------------------------------------------------------------
inline int NiPhysXPSParticleSystem::GetActorPoolCount()
{
    return m_kActorPool.GetSize();
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSParticleSystem::IsActorPoolEmpty()
{
    return (m_kActorPool.GetSize() == 0);
}
//---------------------------------------------------------------------------
inline NxActor* NiPhysXPSParticleSystem::RemoveActorFromPool()
{
    return m_kActorPool.RemoveEnd();
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSParticleSystem::GetFillActorPoolOnLoad()
{
    return GetBit(FLAG_FILL_POOLS_ON_LOAD);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetKeepsActorMeshes(bool bKeep)
{
    SetBit(bKeep, FLAG_KEEP_ACTOR_MESHES);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSParticleSystem::GetKeepsActorMeshes()
{
    return GetBit(FLAG_KEEP_ACTOR_MESHES);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetFillActorPoolOnLoad(bool bFill)
{
    SetBit(bFill, FLAG_FILL_POOLS_ON_LOAD);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSParticleSystem::GetDefaultActorPoolSize()
{
    return m_uiDefaultActorPoolSize;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSParticleSystem::SetDefaultActorPoolSize(NiUInt32 uiSize)
{
    m_uiDefaultActorPoolSize = uiSize;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDesc* NiPhysXPSParticleSystem::GetActorDesc() const
{
    return m_spActorDescriptor;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXPSParticleSystem::SetActorDesc(
    NiPhysXActorDesc* pkActor)
{
    NiPhysXActorDescPtr spOldPtr = m_spActorDescriptor;
    
    m_spActorDescriptor = pkActor;

    return spOldPtr;
}
//---------------------------------------------------------------------------
