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
inline NiUInt16 NiPSMeshParticleSystem::GetNumGenerations() const
{
    return (NiUInt16) m_kPools.GetSize();
}
//---------------------------------------------------------------------------
inline NiAVObjectPtr NiPSMeshParticleSystem::SetMasterParticle(
    NiUInt16 usGeneration,
    NiAVObject* pkMasterParticle)
{
    NIASSERT(usGeneration < GetNumGenerations());

    NiAVObjectPtr spOldMasterParticle = m_kMasterParticles.GetAt(usGeneration);
    m_kMasterParticles.SetAt(usGeneration, pkMasterParticle);

    return spOldMasterParticle;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiPSMeshParticleSystem::GetMasterParticle(
    NiUInt16 usGeneration) const
{
    NIASSERT(usGeneration < GetNumGenerations());
    return m_kMasterParticles.GetAt(usGeneration);
}
//---------------------------------------------------------------------------
inline bool NiPSMeshParticleSystem::GetAutoFillPools() const
{
    return m_bAutoFillPools;
}
//---------------------------------------------------------------------------
inline void NiPSMeshParticleSystem::SetAutoFillPools(bool bAutoFillPools)
{
    m_bAutoFillPools = bAutoFillPools;
}
//---------------------------------------------------------------------------
inline NiNode* NiPSMeshParticleSystem::GetParticleContainer() const
{
    return m_spParticleContainer;
}
//---------------------------------------------------------------------------
