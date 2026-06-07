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
inline NiPSParticleSystem* NiPhysXFluidDest::GetTarget()
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline NiTransform& NiPhysXFluidDest::GetPhysXToFluid()
{
    return m_kPhysXToFluid;
}
//---------------------------------------------------------------------------
inline float NiPhysXFluidDest::GetScalePtoW()
{
    return m_fScalePToW;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidDest::UpdateSceneGraph(const float,
    const NiTransform&, const float,
    const bool)
{
    // This function deliberately left blank
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidDest::SetEmitter(NiPhysXPSFluidEmitter* pkEmitter)
{
    m_spEmitter = pkEmitter;
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

