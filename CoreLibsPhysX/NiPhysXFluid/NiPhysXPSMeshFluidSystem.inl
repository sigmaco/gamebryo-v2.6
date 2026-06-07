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
inline NxParticleData& NiPhysXPSMeshFluidSystem::GetFluidData()
{
    return m_kFluidData;
}
//---------------------------------------------------------------------------
inline NxParticleIdData& NiPhysXPSMeshFluidSystem::GetParticleCreationData()
{
    return m_kFluidCreationData;
}
//---------------------------------------------------------------------------
inline NxParticleIdData& NiPhysXPSMeshFluidSystem::GetParticleDeletionData()
{
    return m_kFluidDeletionData;
}
//---------------------------------------------------------------------------
inline NiUInt32* NiPhysXPSMeshFluidSystem::GetPhysXToGBMap()
{
    return m_puiPhysXToGBMap;
}
//---------------------------------------------------------------------------
inline NxVec3* NiPhysXPSMeshFluidSystem::GetPositionsBuffer()
{
    return m_pkPositionsBuffer;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSMeshFluidSystem::GetMaxNumPhysXParticles()
{
    return m_uiMaxNumPhysXParticles;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSMeshFluidSystem::SetUsesPhysXSpace(const bool bPhysXSpace)
{
    m_bPhysXSpace = bPhysXSpace;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSMeshFluidSystem::GetUsesPhysXSpace()
{
    return m_bPhysXSpace;
}
//---------------------------------------------------------------------------
inline NiTransform& NiPhysXPSMeshFluidSystem::GetPhysXToFluid()
{
    return m_spDestination->GetPhysXToFluid();
}
//---------------------------------------------------------------------------
inline float NiPhysXPSMeshFluidSystem::GetScalePhysXToFluid()
{
    return m_spDestination->GetScalePtoW();
}
//---------------------------------------------------------------------------

