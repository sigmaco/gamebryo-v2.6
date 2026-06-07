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
inline NiAddRemoveEntityCommand::NiAddRemoveEntityCommand(
    NiScene* pkScene,
    NiEntityInterface* pkEntity, 
    bool bAddEntity) 
    : 
    m_spScene(pkScene),
    m_spEntity(pkEntity),
    m_bAddEntity(bAddEntity),
    m_bSuccessfullyExecuted(false)
{
    NIASSERT(m_spScene && m_spEntity);

    // Generate command name.
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, "%s \"%s\" entity %s \"%s\" scene",
        m_bAddEntity ? "Add" : "Remove", 
        (const char*)m_spEntity->GetName(),
        m_bAddEntity ? "to" : "from", 
        (const char*)m_spScene->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiScene* NiAddRemoveEntityCommand::GetScene() const
{
    return m_spScene;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiAddRemoveEntityCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline NiBool NiAddRemoveEntityCommand::GetAddEntity() const
{
    return m_bAddEntity;
}
//---------------------------------------------------------------------------
