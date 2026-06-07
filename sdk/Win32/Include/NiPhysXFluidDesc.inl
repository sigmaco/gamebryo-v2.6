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
inline NxFluidDesc NiPhysXFluidDesc::GetNxFluidDesc() const
{
    return m_kDesc;
}
//---------------------------------------------------------------------------
inline NiTObjectArray<NiPhysXFluidEmitterDescPtr>&
    NiPhysXFluidDesc::GetEmitters()
{
    return m_kEmitters;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidDesc::SetEmitters(
        const NiTObjectArray<NiPhysXFluidEmitterDescPtr>& kEmitters)
{
    m_kEmitters.RemoveAll();
    for (NiUInt32 ui = 0; ui < kEmitters.GetSize(); ui++)
        m_kEmitters.Add(kEmitters.GetAt(ui));
}
//---------------------------------------------------------------------------
inline NiFixedString NiPhysXFluidDesc::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidDesc::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NxFluid* NiPhysXFluidDesc::GetFluid() const
{
    return m_pkFluid;
}
//---------------------------------------------------------------------------
inline NiPSParticleSystemPtr NiPhysXFluidDesc::GetParticleSystem() const
{
    return m_pkPSys;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidDesc::SetParticleSystem(NiPSParticleSystem* pkPSys)
{
    m_pkPSys = pkPSys;
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
