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
inline NiRenameSelectionSetCommand::NiRenameSelectionSetCommand(
    NiEntitySelectionSet* pkSelectionSet,
    const NiFixedString& kNewName) 
    :
    m_spSelectionSet(pkSelectionSet),
    m_kNewName(kNewName),
    m_bOldDataValid(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, "Rename \"%s\" selection set to \"%s\"",
        (const char*)m_spSelectionSet->GetName(),
        (const char*)m_kNewName);
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiEntitySelectionSet*
    NiRenameSelectionSetCommand::GetSelectionSet() const
{
    return m_spSelectionSet;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenameSelectionSetCommand::GetNewName() const
{
    return m_kNewName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenameSelectionSetCommand::GetOldName() const
{
    return m_kOldName;
}
//---------------------------------------------------------------------------
