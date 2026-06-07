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
inline NiUInt32 NiPSParticleSystem::GetMaxNumParticles() const
{
    return m_uiMaxNumParticles;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSParticleSystem::GetNumParticles() const
{
    return m_uiNumParticles;
}
//---------------------------------------------------------------------------
inline float NiPSParticleSystem::GetLastTime() const
{
    return m_fLastTime;
}
//---------------------------------------------------------------------------
inline bool NiPSParticleSystem::HasColors() const
{
    return (m_pkColors != NULL);
}
//---------------------------------------------------------------------------
inline bool NiPSParticleSystem::HasRotations() const
{
    return (m_pfRotationAngles != NULL) && (m_pfRotationSpeeds != NULL);
}
//---------------------------------------------------------------------------
inline bool NiPSParticleSystem::HasRotationAxes() const
{
    return (m_pkRotationAxes != NULL);
}
//---------------------------------------------------------------------------
inline bool NiPSParticleSystem::GetWorldSpace() const
{
    return m_bWorldSpace;
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::SetWorldSpace(bool bWorldSpace)
{
    m_bWorldSpace = bWorldSpace;
}
//---------------------------------------------------------------------------
inline NiPSSimulator* NiPSParticleSystem::GetSimulator() const
{
    return m_pkSimulator;
}
//---------------------------------------------------------------------------
inline NiPSBoundUpdater* NiPSParticleSystem::GetBoundUpdater() const
{
    return m_spBoundUpdater;
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::SetBoundUpdater(
    NiPSBoundUpdater* pkBoundUpdater)
{
    m_spBoundUpdater = pkBoundUpdater;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSParticleSystem::GetEmitterCount() const
{
    return m_kEmitters.GetSize();
}
//---------------------------------------------------------------------------
inline NiPSEmitter* NiPSParticleSystem::GetEmitterAt(NiUInt32 uiIndex) const
{
    NIASSERT(uiIndex < m_kEmitters.GetSize());

    return m_kEmitters.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::AddEmitter(NiPSEmitter* pkEmitter)
{
    NIASSERT(pkEmitter);
    m_kEmitters.Add(pkEmitter);
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::RemoveEmitterAt(
    NiUInt32 uiIndex,
    bool bMaintainOrder)
{
    NIASSERT(uiIndex < m_kEmitters.GetSize());

    if (bMaintainOrder)
    {
        m_kEmitters.OrderedRemoveAt(uiIndex);
    }
    else
    {
        m_kEmitters.RemoveAt(uiIndex);
    }
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::RemoveAllEmitters()
{
    m_kEmitters.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSParticleSystem::GetSpawnerCount() const
{
    return m_kSpawners.GetSize();
}
//---------------------------------------------------------------------------
inline NiPSSpawner* NiPSParticleSystem::GetSpawnerAt(NiUInt32 uiIndex) const
{
    NIASSERT(uiIndex < m_kSpawners.GetSize());

    return m_kSpawners.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiPSSpawner* NiPSParticleSystem::GetDeathSpawner() const
{
    return m_pkDeathSpawner;
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::SetDeathSpawner(NiPSSpawner* pkDeathSpawner)
{
    m_pkDeathSpawner = pkDeathSpawner;
}
//---------------------------------------------------------------------------
inline void NiPSParticleSystem::ForceSimulationToComplete()
{
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteModifiers(&kSyncArgs);
}
//---------------------------------------------------------------------------
inline NiPoint3* NiPSParticleSystem::GetPositions()
{
    return m_pkPositions;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiPSParticleSystem::GetPositions() const
{
    return m_pkPositions;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiPSParticleSystem::GetVelocities()
{
    return m_pkVelocities;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiPSParticleSystem::GetVelocities() const
{
    return m_pkVelocities;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetAges()
{
    return m_pfAges;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetAges() const
{
    return m_pfAges;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetLifeSpans()
{
    return m_pfLifeSpans;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetLifeSpans() const
{
    return m_pfLifeSpans;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetLastUpdateTimes()
{
    return m_pfLastUpdateTimes;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetLastUpdateTimes() const
{
    return m_pfLastUpdateTimes;
}
//---------------------------------------------------------------------------
inline NiUInt32* NiPSParticleSystem::GetFlags()
{
    return m_puiFlags;
}
//---------------------------------------------------------------------------
inline const NiUInt32* NiPSParticleSystem::GetFlags() const
{
    return m_puiFlags;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetRadii()
{
    return m_pfRadii;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetRadii() const
{
    return m_pfRadii;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetSizes()
{
    return m_pfSizes;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetSizes() const
{
    return m_pfSizes;
}
//---------------------------------------------------------------------------
inline NiRGBA* NiPSParticleSystem::GetColors()
{
    return m_pkColors;
}
//---------------------------------------------------------------------------
inline const NiRGBA* NiPSParticleSystem::GetColors() const
{
    return m_pkColors;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetRotationAngles()
{
    return m_pfRotationAngles;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetRotationAngles() const
{
    return m_pfRotationAngles;
}
//---------------------------------------------------------------------------
inline float* NiPSParticleSystem::GetRotationSpeeds()
{
    return m_pfRotationSpeeds;
}
//---------------------------------------------------------------------------
inline const float* NiPSParticleSystem::GetRotationSpeeds() const
{
    return m_pfRotationSpeeds;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiPSParticleSystem::GetRotationAxes()
{
    return m_pkRotationAxes;
}
//---------------------------------------------------------------------------
inline const NiPoint3* NiPSParticleSystem::GetRotationAxes() const
{
    return m_pkRotationAxes;
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPSParticleSystem::GetMaxValidSpawnerID() const
{
    return (NiUInt16) m_kActiveSpawners.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPSParticleSystem::GetSpawnerID(NiPSSpawner* pkSpawner)
{
    return (NiUInt16) m_kActiveSpawners.Add(pkSpawner);
}
//---------------------------------------------------------------------------
inline NiPSSpawner* NiPSParticleSystem::GetSpawnerFromID(NiUInt16 uiID) const
{
    return m_kActiveSpawners.GetAt(uiID);
}
//---------------------------------------------------------------------------
