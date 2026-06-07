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
inline NiPhysXScenePtr NiPhysXPSysData::GetScene() const
{
    return m_spScene;
}
//---------------------------------------------------------------------------
inline void NiPhysXPSysData::GetActorPoolRegion(NiPoint3& kCenter,
    NiPoint3& kDim)
{
    kCenter = m_kActorPoolCenter;
    kDim = m_kActorPoolDim;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSysData::GetFillActorPoolOnLoad()
{
    return m_bFillActorPoolOnLoad;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSysData::GetKeepsActorMeshes()
{
    return m_bKeepsActorMeshes;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPSysData::GetDefaultActorPoolSize()
{
    return m_uiDefaultActorPoolSize;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDesc* NiPhysXPSysData::GetActorDesc() const
{
    return m_spActorDescriptor;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSysData::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
