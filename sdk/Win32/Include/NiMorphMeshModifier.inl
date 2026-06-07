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
inline NiUInt16 NiMorphMeshModifier::GetNumTargets() const
{
    return m_uiNumTargets;
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetNumTargets(const NiUInt16 uiNumTargets)
{
    m_uiNumTargets = uiNumTargets;
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetRelativeTargets() const
{
    return GetBit(FLAG_RELATIVETARGETS);
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetUpdateNormals() const
{
    return GetBit(FLAG_UPDATENORMALS);
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetNeedsUpdate() const
{
    return GetBit(FLAG_NEEDSUPDATE);
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetAlwaysUpdate() const
{
    return GetBit(FLAG_ALWAYSUPDATE);
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetNeedsCompletion() const
{
    return GetBit(FLAG_NEEDSCOMPLETION);
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetSkinned() const
{
    return GetBit(FLAG_SKINNED);
}
//---------------------------------------------------------------------------
inline bool NiMorphMeshModifier::GetSWSkinned() const
{
    return GetBit(FLAG_SWSKINNED);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetRelativeTargets(const bool bValue)
{
    return SetBit(bValue, FLAG_RELATIVETARGETS);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetUpdateNormals(const bool bValue)
{
    return SetBit(bValue, FLAG_UPDATENORMALS);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetNeedsUpdate()
{
    return SetBit(true, FLAG_NEEDSUPDATE);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetAlwaysUpdate(const bool bValue)
{
    return SetBit(bValue, FLAG_ALWAYSUPDATE);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetNeedsUpdate(const bool bValue)
{
    return SetBit(bValue, FLAG_NEEDSUPDATE);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetNeedsCompletion(const bool bValue)
{
    return SetBit(bValue, FLAG_NEEDSCOMPLETION);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetSkinned(const bool bValue)
{
    return SetBit(bValue, FLAG_SKINNED);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetSWSkinned(const bool bValue)
{
    return SetBit(bValue, FLAG_SWSKINNED);
}
//---------------------------------------------------------------------------
inline void NiMorphMeshModifier::SetSelectiveUpdateFlags(
    bool& bSelectiveUpdate, bool& bSelectiveUpdateTransforms, bool& bRigid)
{
    bSelectiveUpdate = true;
    bSelectiveUpdateTransforms = false;
    bRigid = true;
}
//---------------------------------------------------------------------------
inline NiFixedString NiMorphMeshModifier::MorphElementSemantic(
    const NiFixedString& kSemantic)
{
    char acTemp[128] = "MORPH_";
    NiStrcat(acTemp, 128, (const char*)kSemantic);
    return NiFixedString(acTemp);
}
//---------------------------------------------------------------------------
