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
inline bool NiPhysXClothModifier::GetActive() const
{
    return GetBit(ACTIVE_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetActive(const bool bActive)
{
    SetBit(bActive, ACTIVE_MASK);
}
//---------------------------------------------------------------------------
inline NxCloth* NiPhysXClothModifier::GetCloth()
{
    return m_pkCloth;
}
//---------------------------------------------------------------------------
inline NxMeshData& NiPhysXClothModifier::GetClothData()
{
    return m_kClothData;
}
//---------------------------------------------------------------------------
inline NxActor* NiPhysXClothModifier::GetParentActor()
{
    return m_pkParentActor;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetParentActor(NxActor* pkActor)
{
    m_pkParentActor = pkActor;
}
//---------------------------------------------------------------------------
inline bool NiPhysXClothModifier::GetUpdateNormals() const
{
    return GetBit(UPDATE_NORMALS_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetUpdateNormals(const bool bUpdate)
{
    SetBit(bUpdate, UPDATE_NORMALS_MASK);
}
//---------------------------------------------------------------------------
inline float NiPhysXClothModifier::GetMeshGrowFactor() const
{
    return m_fMeshGrowFactor;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetMeshGrowFactor(const float fGrowFactor)
{
    m_fMeshGrowFactor = fGrowFactor;
}
//---------------------------------------------------------------------------
inline bool NiPhysXClothModifier::GetTearableHint() const
{
    return GetBit(TEARABLE_HINT_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetTearableHint(const bool bTearable)
{
    SetBit(bTearable, TEARABLE_HINT_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXClothModifier::GetDoSleepXforms() const
{
    return GetBit(DO_SLEEP_XFORMS_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetDoSleepXforms(const bool bDoSleepXforms)
{
    SetBit(bDoSleepXforms, DO_SLEEP_XFORMS_MASK);
}
//---------------------------------------------------------------------------
inline NiUInt16 NiPhysXClothModifier::GetNBTSet() const
{
    return m_uiNBTSet;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetNBTSet(const NiUInt16 uiSet)
{
    m_uiNBTSet = uiSet;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothModifier::SetSleeping(bool bSleeping)
{
    SetBit(bSleeping, SLEEPING_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXClothModifier::GetSleeping() const
{
    return GetBit(SLEEPING_MASK);
}
//---------------------------------------------------------------------------
