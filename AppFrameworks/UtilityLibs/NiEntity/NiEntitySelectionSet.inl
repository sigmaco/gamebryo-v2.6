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
inline NiEntitySelectionSet::NiEntitySelectionSet(const NiFixedString& kName,
    unsigned int uiInitialSize) : m_kName(kName), m_kEntities(uiInitialSize)
{
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiEntitySelectionSet::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiEntitySelectionSet::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline void NiEntitySelectionSet::AddEntity(NiEntityInterface* pkEntity)
{
    m_kEntities.Add(pkEntity);
}
//---------------------------------------------------------------------------
inline unsigned int NiEntitySelectionSet::GetEntityCount() const
{
    return m_kEntities.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiEntitySelectionSet::GetEntityAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kEntities.GetSize());
    return m_kEntities.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiEntitySelectionSet::RemoveEntity(NiEntityInterface* pkEntity)
{
    int iIndex = m_kEntities.Find(pkEntity);
    if (iIndex > -1)
    {
        m_kEntities.OrderedRemoveAt(iIndex);
    }
}
//---------------------------------------------------------------------------
inline void NiEntitySelectionSet::RemoveEntityAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kEntities.GetSize());
    m_kEntities.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiEntitySelectionSet::RemoveAllEntities()
{
    m_kEntities.RemoveAll();
}
//---------------------------------------------------------------------------
