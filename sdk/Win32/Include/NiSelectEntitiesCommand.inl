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
inline NiSelectEntitiesCommand::NiSelectEntitiesCommand(
    NiEntitySelectionSet* pkSelectionSet, 
    bool bAddToSelectionSet,
    unsigned int uiEntityArraySize, 
    unsigned int uiEntityArrayGrowBy)
    :
    m_spSelectionSet(pkSelectionSet), 
    m_kNewEntities(uiEntityArraySize, uiEntityArrayGrowBy), 
    m_kOldEntities(0),
    m_bAddToSelectionSet(bAddToSelectionSet), 
    m_bOldDataValid(false),
    m_bNewEntitiesValid(false)
{
    NIASSERT(m_spSelectionSet);

    // Generate command name.
    char acCommandName[1024];
    if (m_bAddToSelectionSet)
    {
        NiSprintf(acCommandName, 1024, 
            "Add entities to \"%s\" selection",
            (const char*)m_spSelectionSet->GetName());
    }
    else
    {
        NiSprintf(acCommandName, 1024, 
            "Remove entities from \"%s\" selection", 
            (const char*)m_spSelectionSet->GetName());
    }
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiSelectEntitiesCommand::NiSelectEntitiesCommand(
    const NiFixedString& kCommandName, 
    NiEntitySelectionSet* pkSelectionSet,
    bool bAddToSelectionSet, 
    unsigned int uiEntityArraySize,
    unsigned int uiEntityArrayGrowBy) 
    : 
    m_spSelectionSet(pkSelectionSet),
    m_kNewEntities(uiEntityArraySize, uiEntityArrayGrowBy), 
    m_kOldEntities(0),
    m_kCommandName(kCommandName), 
    m_bAddToSelectionSet(bAddToSelectionSet),
    m_bOldDataValid(false), 
    m_bNewEntitiesValid(false)
{
    NIASSERT(m_spSelectionSet && m_kCommandName.Exists());
}
//---------------------------------------------------------------------------
inline NiEntitySelectionSet* NiSelectEntitiesCommand::GetSelectionSet() const
{
    return m_spSelectionSet;
}
//---------------------------------------------------------------------------
inline const NiTObjectArray<NiEntityInterfaceIPtr>&
    NiSelectEntitiesCommand::GetNewEntities() const
{
    return m_kNewEntities;
}
//---------------------------------------------------------------------------
inline const NiTObjectArray<NiEntityInterfaceIPtr>&
    NiSelectEntitiesCommand::GetOldEntities() const
{
    return m_kOldEntities;
}
//---------------------------------------------------------------------------
inline NiBool NiSelectEntitiesCommand::GetAddToSelectionSet() const
{
    return m_bAddToSelectionSet;
}
//---------------------------------------------------------------------------
inline void NiSelectEntitiesCommand::AddEntityToArray(
    NiEntityInterface* pkEntity)
{
    m_kNewEntities.Add(pkEntity);
    m_bNewEntitiesValid = true;
}
//---------------------------------------------------------------------------
