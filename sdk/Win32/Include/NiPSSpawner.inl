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
inline NiUInt16 NiPSSpawner::GetNumSpawnGenerations() const
{
    return m_usNumSpawnGenerations;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetNumSpawnGenerations(
    NiUInt16 usNumSpawnGenerations)
{
    m_usNumSpawnGenerations = usNumSpawnGenerations;
}
//---------------------------------------------------------------------------
inline float NiPSSpawner::GetPercentageSpawned() const
{
    return m_fPercentageSpawned;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetPercentageSpawned(
    float fPercentageSpawned)
{
    m_fPercentageSpawned = fPercentageSpawned;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSSpawner::GetMinNumToSpawn() const
{
    return m_uiMinNumToSpawn;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetMinNumToSpawn(
    NiUInt32 uiMinNumToSpawn)
{
    m_uiMinNumToSpawn = uiMinNumToSpawn;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPSSpawner::GetMaxNumToSpawn() const
{
    return m_uiMaxNumToSpawn;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetMaxNumToSpawn(
    NiUInt32 uiMaxNumToSpawn)
{
    m_uiMaxNumToSpawn = uiMaxNumToSpawn;
}
//---------------------------------------------------------------------------
inline float NiPSSpawner::GetSpawnSpeedChaos() const
{
    return m_fSpawnSpeedChaos;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetSpawnSpeedChaos(float fSpawnSpeedChaos)
{
    m_fSpawnSpeedChaos = fSpawnSpeedChaos;
}
//---------------------------------------------------------------------------
inline float NiPSSpawner::GetSpawnDirChaos() const
{
    return m_fSpawnDirChaos;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetSpawnDirChaos(float fSpawnDirChaos)
{
    m_fSpawnDirChaos = fSpawnDirChaos;
}
//---------------------------------------------------------------------------
inline float NiPSSpawner::GetLifeSpan() const
{
    return m_fLifeSpan;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetLifeSpan(float fLifeSpan)
{
    m_fLifeSpan = fLifeSpan;
}
//---------------------------------------------------------------------------
inline float NiPSSpawner::GetLifeSpanVar() const
{
    return m_fLifeSpanVar;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetLifeSpanVar(float fLifeSpanVar)
{
    m_fLifeSpanVar = fLifeSpanVar;
}
//---------------------------------------------------------------------------
inline void NiPSSpawner::SetMasterPSystem(NiPSParticleSystem* pkMasterPSystem)
{
    m_pkMasterPSystem = pkMasterPSystem;
}
//---------------------------------------------------------------------------
