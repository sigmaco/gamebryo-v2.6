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
inline void NiPhysXPropDesc::SetKeepAllStates(const bool bKeepAllStates)
{
    SetBit(bKeepAllStates, KEEP_ALL_STATES_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::GetKeepAllStates() const
{
    return GetBit(KEEP_ALL_STATES_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::SetKeepDefaultState(const bool bKeepDefaultState)
{
    SetBit(bKeepDefaultState, KEEP_DEFAULT_STATE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::GetKeepDefaultState() const
{
    return GetBit(KEEP_DEFAULT_STATE_MASK);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::GetUseHardware() const
{
    return GetBit(TARGET_HARDWARE_MASK);
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::SetUseHardware(const bool bHardware)
{
    SetBit(bHardware, TARGET_HARDWARE_MASK);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetRBCompartmentID() const
{
    return m_uiRBCompartmentID;
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::SetRBCompartmentID(const NiUInt32 uiID)
{
    m_uiRBCompartmentID = uiID;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetClothCompartmentID() const
{
    return m_uiClothCompartmentID;
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::SetClothCompartmentID(const NiUInt32 uiID)
{
    m_uiClothCompartmentID = uiID;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetFluidCompartmentID() const
{
    return m_uiFluidCompartmentID;
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::SetFluidCompartmentID(const NiUInt32 uiID)
{
    m_uiFluidCompartmentID = uiID;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetActorCount() const
{
    return m_kActors.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXPropDesc::GetActorAt(NiUInt32 uiIndex)
{
    return m_kActors.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::AddActor(NiPhysXActorDesc* pkActorDesc)
{
    return m_kActors.AddFirstEmpty(pkActorDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXPropDesc::RemoveActorAt(
    NiUInt32 uiIndex)
{
    return m_kActors.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetJointCount() const
{
    return m_kJoints.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXJointDescPtr NiPhysXPropDesc::GetJointAt(NiUInt32 uiIndex)
{
    return m_kJoints.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::AddJoint(NiPhysXJointDesc* pkJointDesc)
{
    return m_kJoints.AddFirstEmpty(pkJointDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXJointDescPtr NiPhysXPropDesc::RemoveJointAt(NiUInt32 uiIndex)
{
    return m_kJoints.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetClothCount() const
{
    return m_kClothes.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXClothDescPtr NiPhysXPropDesc::GetClothAt(NiUInt32 uiIndex)
{
    return m_kClothes.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::AddCloth(NiPhysXClothDesc* pkClothDesc)
{
    return m_kClothes.AddFirstEmpty(pkClothDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXClothDescPtr NiPhysXPropDesc::RemoveClothAt(NiUInt32 uiIndex)
{
    return m_kClothes.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline NiPhysXMaterialDescPtr NiPhysXPropDesc::GetMaterial(
    const NxMaterialIndex usIndex)
{
    NiPhysXMaterialDescPtr pkMaterial;
    
    if (m_kMaterials.GetAt(usIndex, pkMaterial))
        return pkMaterial;

    return 0;
}
//---------------------------------------------------------------------------
inline NiTMapIterator NiPhysXPropDesc::GetFirstMaterial()
{
    return m_kMaterials.GetFirstPos();
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::GetNextMaterial(NiTMapIterator& kPos,
    NxMaterialIndex& usIndex, NiPhysXMaterialDescPtr& spMaterial)
{
    m_kMaterials.GetNext(kPos, usIndex, spMaterial);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::RemoveMaterial(const NxMaterialIndex usIndex)
{
    return m_kMaterials.RemoveAt(usIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetNumStates() const
{
    return m_uiNumStates;
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::SetNumStates(const NiUInt32 uiNumStates)
{
    m_uiNumStates = uiNumStates;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXPropDesc::GetStateIndex(const NiFixedString& kName)
{
    NiUInt32 uiIndex;
    if (m_kStateNameMap.GetAt(kName, uiIndex))
        return uiIndex;
        
    return 0;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::SetStateName(NiUInt32 uiIndex,
    NiFixedString& kName)
{
    NiUInt32 uiTestIndex;
    if (m_kStateNameMap.GetAt(kName, uiTestIndex))
        return (uiIndex == uiTestIndex);

    m_kStateNameMap.SetAt(kName, uiIndex);

    return true;
}
//---------------------------------------------------------------------------
inline NiTMapIterator NiPhysXPropDesc::GetFirstStateName()
{
    return m_kStateNameMap.GetFirstPos();
}
//---------------------------------------------------------------------------
inline void NiPhysXPropDesc::GetNextStateName(NiTMapIterator& kPos,
    NiFixedString& kName, NiUInt32& uiIndex)
{
    m_kStateNameMap.GetNext(kPos, kName, uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::RemoveStateName(NiFixedString& kName)
{
    return m_kStateNameMap.RemoveAt(kName);
}
//---------------------------------------------------------------------------
inline bool NiPhysXPropDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
