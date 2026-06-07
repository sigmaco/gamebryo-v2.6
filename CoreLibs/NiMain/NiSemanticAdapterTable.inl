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
inline NiUInt32 NiSemanticAdapterTable::GetNumFilledTableEntries() const
{
    return m_uiFilledEntries;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSemanticAdapterTable::GetNumAllocatedTableEntries() const
{
    return m_kAdapterTable.GetAllocatedSize();
}
//---------------------------------------------------------------------------
inline NiFixedString NiSemanticAdapterTable::GetGenericSemanticName(
    NiUInt32 uiEntry) const
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
        return m_kAdapterTable.GetAt(uiEntry).m_kGenericSemantic;

    NiFixedString kNull;
    return kNull;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiSemanticAdapterTable::GetGenericSemanticIndex(
    NiUInt32 uiEntry) const
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
        return m_kAdapterTable.GetAt(uiEntry).m_uiGenericSemanticIndex;

    return 0;
}
//---------------------------------------------------------------------------
inline NiFixedString NiSemanticAdapterTable::GetRendererSemanticName(
    NiUInt32 uiEntry) const
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
        return m_kAdapterTable.GetAt(uiEntry).m_kRendererSemantic;

    NiFixedString kNull;
    return kNull;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiSemanticAdapterTable::GetRendererSemanticIndex(
    NiUInt32 uiEntry) const
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
        return m_kAdapterTable.GetAt(uiEntry).m_uiRendererSemanticIndex;

    return 0;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiSemanticAdapterTable::GetComponentCount(
    NiUInt32 uiEntry) const
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
        return m_kAdapterTable.GetAt(uiEntry).m_uiComponentCount;

    return 0;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiSemanticAdapterTable::GetSharedRendererSemanticPosition(
    NiUInt32 uiEntry) const
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
        return m_kAdapterTable.GetAt(uiEntry).
            m_uiSharedRendererSemanticPosition;

    return 0;
}
//---------------------------------------------------------------------------
inline void NiSemanticAdapterTable::ClearEntry(NiUInt32 uiEntry)
{
    if (uiEntry < m_kAdapterTable.GetAllocatedSize())
    {
        SemanticMapping kMapping = m_kAdapterTable.GetAt(uiEntry);
        // Non-null generic semantic used as indication of "filled" entry.
        if (kMapping.m_kGenericSemantic.Exists())
            m_uiFilledEntries--;
        m_kAdapterTable.SetAt(uiEntry, 0);
    }
}
//---------------------------------------------------------------------------
