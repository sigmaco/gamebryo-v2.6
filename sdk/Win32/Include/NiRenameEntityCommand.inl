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
inline NiRenameEntityCommand::NiRenameEntityCommand(
    NiEntityInterface* pkEntity,
    const NiFixedString& kNewName)
    :
    m_spEntity(pkEntity), 
    m_kNewName(kNewName),
    m_bOldDataValid(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, "Rename \"%s\" entity to \"%s\"",
        (const char*)m_spEntity->GetName(), 
        (const char*)m_kNewName);
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiRenameEntityCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenameEntityCommand::GetNewName() const
{
    return m_kNewName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenameEntityCommand::GetOldName() const
{
    return m_kOldName;
}
//---------------------------------------------------------------------------
