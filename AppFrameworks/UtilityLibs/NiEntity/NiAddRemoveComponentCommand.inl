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
inline NiAddRemoveComponentCommand::NiAddRemoveComponentCommand(
    NiEntityInterface* pkEntity, 
    NiEntityComponentInterface* pkComponent,
    bool bAddComponent, 
    bool bPerformErrorChecking) 
    : 
    m_spEntity(pkEntity),
    m_spComponent(pkComponent), 
    m_bAddComponent(bAddComponent),
    m_bPerformErrorChecking(bPerformErrorChecking),
    m_bSuccessfullyExecuted(false)
{
    NIASSERT(m_spComponent && m_spEntity);

    // Generate command name.
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, "%s \"%s\" component %s \"%s\" entity",
        m_bAddComponent ? "Add" : "Remove", 
        (const char*)m_spComponent->GetName(),
        m_bAddComponent ? "to" : "from", 
        (const char*)m_spEntity->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiEntityInterface* NiAddRemoveComponentCommand::GetEntity() const
{
    return m_spEntity;
}
//---------------------------------------------------------------------------
inline NiEntityComponentInterface* NiAddRemoveComponentCommand::GetComponent() 
    const
{
    return m_spComponent;
}
//---------------------------------------------------------------------------
inline NiBool NiAddRemoveComponentCommand::GetAddComponent() const
{
    return m_bAddComponent;
}
//---------------------------------------------------------------------------
inline NiBool NiAddRemoveComponentCommand::GetPerformErrorChecking() const
{
    return m_bPerformErrorChecking;
}
//---------------------------------------------------------------------------
