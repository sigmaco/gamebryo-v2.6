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
inline NiSkinningLODController::NiSkinningLODController() :
    m_uiCurrentLOD(0), m_uiNextLOD(0)
{
}
//---------------------------------------------------------------------------
inline bool NiSkinningLODController::TargetIsRequiredType() const
{
    return NiIsKindOf(NiNode, m_pkTarget);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSkinningLODController::GetBoneLOD() const
{
    return m_uiCurrentLOD;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSkinningLODController::GetNextBoneLOD() const
{
    return m_uiNextLOD;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSkinningLODController::GetNumberOfBoneLODs() const
{
    return m_kLODs.GetSize();
}
//---------------------------------------------------------------------------
inline NiSkinningLODController::LODInfo::LODInfo() :
    m_uiNumBones(0), m_kActiveSkins(0)
{
}
//---------------------------------------------------------------------------
inline NiSkinningLODController::LODInfo::LODInfo(
    NiUInt32 uiNumBones) : m_uiNumBones(uiNumBones), m_kActiveSkins(0)
{
}
//---------------------------------------------------------------------------
inline bool NiSkinningLODController::LODInfo::operator ==(
    const LODInfo& kRHS) const
{
    if (m_uiNumBones != kRHS.m_uiNumBones ||
        m_kActiveSkins.GetSize() != kRHS.m_kActiveSkins.GetSize())
    {
        return false;
    } 
    for (NiUInt32 i = 0; i < m_kActiveSkins.GetSize(); i++)
    {
        if (m_kActiveSkins.GetAt(i) != kRHS.m_kActiveSkins.GetAt(i))
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
inline bool NiSkinningLODController::LODInfo::operator !=(
    const LODInfo& kRHS) const
{
    return !(*this == kRHS);
}
//---------------------------------------------------------------------------
inline const NiSkinningLODController::LODInfo& 
    NiSkinningLODController::LODInfo::operator =(const LODInfo& kRHS)
{
    m_uiNumBones = kRHS.m_uiNumBones;
    if (kRHS.m_kActiveSkins.GetSize() > 0)
    {
        m_kActiveSkins.SetSize(kRHS.m_kActiveSkins.GetSize());
        for (NiUInt32 i = 0; i < kRHS.m_kActiveSkins.GetSize(); i++)
        {
            m_kActiveSkins.SetAt(i, kRHS.m_kActiveSkins.GetAt(i));
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSkinningLODController::GetNumBonesUsed(
    NiUInt32 uiLODLevel) const
{
    NIASSERT(uiLODLevel < m_kLODs.GetSize());

    return m_kLODs.GetAt(uiLODLevel)->m_uiNumBones;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSkinningLODController::GetNumberOfSkins(
    NiUInt32 uiBoneLOD) const
{
    const LODInfo* pkLOD = m_kLODs.GetAt(uiBoneLOD);
    if (pkLOD)
    {
        return pkLOD->m_kActiveSkins.GetSize();
    }

    return 0;
}
//---------------------------------------------------------------------------
inline NiMesh* NiSkinningLODController::GetSkin(NiUInt32 uiBoneLOD,
    NiUInt32 uiSkin)
{
    const LODInfo* pkLOD = m_kLODs.GetAt(uiBoneLOD);
    if (pkLOD)
    {
        return m_kSkins.GetAt((unsigned int)pkLOD->m_kActiveSkins.GetAt(uiSkin));
    }

    return NULL;
}
//---------------------------------------------------------------------------

