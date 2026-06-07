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
inline NiAddRemoveSelectionSetCommand::NiAddRemoveSelectionSetCommand(
    NiScene* pkScene, 
    NiEntitySelectionSet* pkSelectionSet,
    bool bAddSelectionSet) 
    : 
    m_spScene(pkScene),
    m_spSelectionSet(pkSelectionSet), 
    m_bAddSelectionSet(bAddSelectionSet),
    m_bSuccessfullyExecuted(false)
{
    NIASSERT(m_spScene && m_spSelectionSet);

    // Generate command name.
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" selection set %s \"%s\" scene",
        m_bAddSelectionSet ? "Add" : "Remove", 
        (const char*)m_spSelectionSet->GetName(),
        m_bAddSelectionSet ? "to" : "from", 
        (const char*)m_spScene->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiScene* NiAddRemoveSelectionSetCommand::GetScene() const
{
    return m_spScene;
}
//---------------------------------------------------------------------------
inline NiEntitySelectionSet* NiAddRemoveSelectionSetCommand::GetSelectionSet()
    const
{
    return m_spSelectionSet;
}
//---------------------------------------------------------------------------
inline NiBool NiAddRemoveSelectionSetCommand::GetAddSelectionSet() const
{
    return m_bAddSelectionSet;
}
//---------------------------------------------------------------------------
