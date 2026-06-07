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
inline const NiFixedString& NiPrefabComponent::PROP_PREFAB_PATH()
{
    return ms_kPropPrefabPath;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPrefabComponent::PROP_PREFAB_ENTITIES()
{
    return ms_kPropPrefabEntities;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiPrefabComponent::GetPrefabPath() const
{
    return m_kPrefabPath;
}
//---------------------------------------------------------------------------
inline void NiPrefabComponent::SetPrefabPath(const NiFixedString& kPrefabPath)
{
    m_kPrefabPath = kPrefabPath;
}
//---------------------------------------------------------------------------
inline const unsigned int NiPrefabComponent::GetPrefabEntityCount() const
{
    return m_kPrefabEntities.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiPrefabComponent::GetPrefabEntityAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kPrefabEntities.GetSize());

    return m_kPrefabEntities.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiPrefabComponent::AddPrefabEntity(NiEntityInterface* pkEntity)
{
    m_kPrefabEntities.Add(pkEntity);
}
//---------------------------------------------------------------------------
inline void NiPrefabComponent::RemovePrefabEntity(NiEntityInterface* pkEntity)
{
    int iIndex = m_kPrefabEntities.Find(pkEntity);
    if (iIndex > -1)
    {
        m_kPrefabEntities.RemoveAt((unsigned int) iIndex);
    }
}
//---------------------------------------------------------------------------
inline void NiPrefabComponent::RemovePrefabEntityAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kPrefabEntities.GetSize());

    m_kPrefabEntities.RemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiPrefabComponent::RemoveAllPrefabEntities()
{
    m_kPrefabEntities.RemoveAll();
}
//---------------------------------------------------------------------------
