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
inline NiUInt32 NiSkinningMeshModifier::GetBoneCount() const
{
    return m_uiBoneCount;
}
//---------------------------------------------------------------------------
inline void NiSkinningMeshModifier::SetRootBoneParent(NiAVObject* pkRBP)
{
    m_pkRootBoneParent = pkRBP;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiSkinningMeshModifier::GetRootBoneParent() const
{
    return m_pkRootBoneParent;
}
//---------------------------------------------------------------------------
inline void NiSkinningMeshModifier::SetRootBoneParentToSkinTransform(
   const NiTransform& kRootBoneParentToSkinTransform)
{
    m_kRootBoneParentToSkinTransform = kRootBoneParentToSkinTransform;
}
//---------------------------------------------------------------------------
inline 
NiTransform& NiSkinningMeshModifier::GetRootBoneParentToSkinTransform()
{
    return m_kRootBoneParentToSkinTransform;
}
//---------------------------------------------------------------------------
inline NiAVObject** NiSkinningMeshModifier::GetBones() const
{
    return m_apkBones;
}
//---------------------------------------------------------------------------
inline NiTransform* NiSkinningMeshModifier::GetSkinToBoneTransforms() const
{
    return m_pkSkinToBoneTransforms;
}
//---------------------------------------------------------------------------
inline void NiSkinningMeshModifier::GetWorldToSkinTransform(
    NiTransform& kWorldToSkin) const
{
    NiTransform kWorldToRootParent;

    m_pkRootBoneParent->GetWorldTransform().Invert(kWorldToRootParent);
    kWorldToSkin = m_kRootBoneParentToSkinTransform * kWorldToRootParent;
}
//---------------------------------------------------------------------------
inline NiMatrix3x4* NiSkinningMeshModifier::GetBoneMatrices() const
{
    return m_pkBoneMatrices;
}
//---------------------------------------------------------------------------
inline const NiTransform& NiSkinningMeshModifier::GetWorldBoneMatrix(
    NiUInt32 uiBone) const
{
    NIASSERT(uiBone < GetBoneCount());
    return *(m_ppkBoneWorlds[uiBone]);
}
//---------------------------------------------------------------------------
inline NiBound* NiSkinningMeshModifier::GetBoneBounds() const
{
    return m_pkBoneBounds;
}
//---------------------------------------------------------------------------
inline void NiSkinningMeshModifier::SetSelectiveUpdateFlags(
    bool& bSelectiveUpdate, bool& bSelectiveUpdateTransforms, bool& bRigid)
{
    bSelectiveUpdate = true;
    bSelectiveUpdateTransforms = true;
    bRigid = false;
}
//---------------------------------------------------------------------------
inline bool NiSkinningMeshModifier::GetSoftwareSkinned() const
{
    return GetBit(USE_SOFTWARE_SKINNING);
}
//---------------------------------------------------------------------------
inline
void NiSkinningMeshModifier::SetSoftwareSkinned(bool bUseSoftwareSkinning)
{
    SetBit(bUseSoftwareSkinning, USE_SOFTWARE_SKINNING);
}
//---------------------------------------------------------------------------
