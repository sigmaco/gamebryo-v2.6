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
inline NiPhysXScenePtr NiPhysXMeshPSysData::GetScene() const
{
    return m_spScene;
}
//---------------------------------------------------------------------------
inline void NiPhysXMeshPSysData::GetActorPoolRegion(NiPoint3& kCenter,
    NiPoint3& kDim)
{
    kCenter = m_kActorPoolCenter;
    kDim = m_kActorPoolDim;
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshPSysData::GetFillActorPoolsOnLoad()
{
    return m_bFillActorPoolsOnLoad;
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshPSysData::GetKeepsActorMeshes()
{
    return m_bKeepsActorMeshes;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXMeshPSysData::GetDefaultActorPoolsSize()
{
    return m_uiDefaultActorPoolSize;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDesc* NiPhysXMeshPSysData::GetActorDescAt(
    NiUInt32 uiIndex) const
{
    if (uiIndex < m_kActorDescriptors.GetSize())
    {
        return m_kActorDescriptors.GetAt(uiIndex);
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXMeshPSysData::GetActorDescCount() const
{
    return m_kActorDescriptors.GetSize();
}
//---------------------------------------------------------------------------
inline bool NiPhysXMeshPSysData::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
