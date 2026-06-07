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
inline NiPhysXScene* NiPhysXProp::GetScene()
{
    return m_pkScene;
}
//---------------------------------------------------------------------------
inline NxMat34 NiPhysXProp::GetXform() const
{
    return m_kXform;
}
//---------------------------------------------------------------------------
inline void NiPhysXProp::SetXform(const NxMat34& kXform)
{
    m_kXform = kXform;
}
//---------------------------------------------------------------------------
inline float NiPhysXProp::GetScaleFactor() const
{
    return m_fScalePToW;
}
//---------------------------------------------------------------------------
inline void NiPhysXProp::SetScaleFactor(const float fScale)
{
    m_fScalePToW = fScale;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXProp::GetSourcesCount() const
{
    return m_kSources.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXSrc* NiPhysXProp::GetSourceAt(const NiUInt32 uiIndex)
{
    return m_kSources.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXProp::GetDestinationsCount() const
{
    return m_kDestinations.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXDest* NiPhysXProp::GetDestinationAt(const NiUInt32 uiIndex)
{
    return m_kDestinations.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiPhysXProp::GetModifiedMeshCount() const
{
    return m_kModifiedMeshes.GetSize();
}
//---------------------------------------------------------------------------
inline NiMesh* NiPhysXProp::GetModifiedMeshAt(const NiUInt32 uiIndex)
{
    return m_kModifiedMeshes.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiPhysXPropDesc* NiPhysXProp::GetSnapshot()
{
    return m_spSnapshot;
}
//---------------------------------------------------------------------------
inline void NiPhysXProp::SetSnapshot(NiPhysXPropDesc* pkSnapshot)
{
    m_spSnapshot = pkSnapshot;
}
//---------------------------------------------------------------------------
inline bool NiPhysXProp::GetKeepMeshes() const
{
    return m_bKeepMeshes;
}
//---------------------------------------------------------------------------
inline void NiPhysXProp::SetKeepMeshes(const bool bVal)
{
    m_bKeepMeshes = bVal;
}
//---------------------------------------------------------------------------
inline void NiPhysXProp::ReleaseSnapshot()
{
    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
inline bool NiPhysXProp::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
