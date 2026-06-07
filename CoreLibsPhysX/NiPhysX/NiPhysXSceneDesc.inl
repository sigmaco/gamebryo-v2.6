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
inline NxSceneDesc& NiPhysXSceneDesc::GetNxSceneDesc()
{
    return m_kNxSceneDesc;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetGroupCollisionFlag(
    const unsigned char ucGroup1, const unsigned char ucGroup2, 
    const bool bVal)
{
    m_abGroupCollisionFlags[ucGroup1][ucGroup2] = bVal;
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::GetGroupCollisionFlag(
    const unsigned char ucGroup1, const unsigned char ucGroup2) const
{
    return m_abGroupCollisionFlags[ucGroup1][ucGroup2];
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterOps(NxFilterOp bOp0, NxFilterOp bOp1,
    NxFilterOp bOp2)
{
    m_aeFilterOps[0] = bOp0;
    m_aeFilterOps[1] = bOp1;
    m_aeFilterOps[2] = bOp2;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetFilterOps(NxFilterOp& bOp0, NxFilterOp& bOp1,
    NxFilterOp& bOp2) const
{
    bOp0 = m_aeFilterOps[0];
    bOp1 = m_aeFilterOps[1];
    bOp2 = m_aeFilterOps[2];
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterBool(bool bFlag)
{
    m_bFilterBool = bFlag;
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::GetFilterBool() const
{
    return m_bFilterBool;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterConstant0(const NxGroupsMask &kMask)
{
    m_auiFilterConstants[0] = kMask;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetFilterConstant0(NxGroupsMask &kMask) const
{
    kMask = m_auiFilterConstants[0];
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterConstant1(const NxGroupsMask &kMask)
{
    m_auiFilterConstants[1] = kMask;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetFilterConstant1(NxGroupsMask &kMask) const
{
    kMask = m_auiFilterConstants[1];
}
//---------------------------------------------------------------------------
inline NiTMap<NiUInt32, NxCompartmentDesc*>&
    NiPhysXSceneDesc::GetCompartmentMap()
{
    return m_kCompartmentMap;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXSceneDesc::GetActorCountBackCompatable() const
{
    return m_kActorsBackCompatable.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXSceneDesc::GetActorAtBackCompatable(
    NiUInt32 uiIndex)
{
    return m_kActorsBackCompatable.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXSceneDesc::GetJointCountBackCompatable() const
{
    return m_kJointsBackCompatable.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXJointDescPtr NiPhysXSceneDesc::GetJointAtBackCompatable(
    NiUInt32 uiIndex)
{
    return m_kJointsBackCompatable.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiTMapIterator NiPhysXSceneDesc::GetFirstMaterialBackCompatable()
{
    return m_kMaterialsBackCompatable.GetFirstPos();
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetNextMaterialBackCompatable(
    NiTMapIterator& kPos, NxMaterialIndex& usIndex,
    NiPhysXMaterialDescPtr& spMaterial)
{
    m_kMaterialsBackCompatable.GetNext(kPos, usIndex, spMaterial);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXSceneDesc::GetNumStatesBackCompatable() const
{
    return m_uiNumStatesBackCompatable;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::ClearBackCompatable()
{
    m_kActorsBackCompatable.RemoveAll();
    m_kJointsBackCompatable.RemoveAll();
    m_kMaterialsBackCompatable.RemoveAll();
    
    // DO NOT set NumStatesBackCompatable to 0
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
