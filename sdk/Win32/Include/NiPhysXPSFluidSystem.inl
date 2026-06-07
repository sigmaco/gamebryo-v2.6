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
inline NxParticleData& NiPhysXPSFluidSystem::GetFluidData()
{
    return m_kFluidData;
}
//---------------------------------------------------------------------------
inline NxParticleIdData& NiPhysXPSFluidSystem::GetParticleCreationData()
{
    return m_kFluidCreationData;
}
//---------------------------------------------------------------------------
inline NxParticleIdData& NiPhysXPSFluidSystem::GetParticleDeletionData()
{
    return m_kFluidDeletionData;
}
//---------------------------------------------------------------------------
inline NiUInt32* NiPhysXPSFluidSystem::GetPhysXToGBMap()
{
    return m_puiPhysXToGBMap;
}
//---------------------------------------------------------------------------
inline NxVec3* NiPhysXPSFluidSystem::GetPositionsBuffer()
{
    return m_pkPositionsBuffer;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSFluidSystem::GetMaxNumPhysXParticles()
{
    return m_uiMaxNumPhysXParticles;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSFluidSystem::SetUsesPhysXSpace(const bool bPhysXSpace)
{
    m_bPhysXSpace = bPhysXSpace;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSFluidSystem::GetUsesPhysXSpace()
{
    return m_bPhysXSpace;
}
//---------------------------------------------------------------------------
inline NiTransform& NiPhysXPSFluidSystem::GetPhysXToFluid()
{
    return m_spDestination->GetPhysXToFluid();
}
//---------------------------------------------------------------------------
inline float NiPhysXPSFluidSystem::GetScalePhysXToFluid()
{
    return m_spDestination->GetScalePtoW();
}
//---------------------------------------------------------------------------
