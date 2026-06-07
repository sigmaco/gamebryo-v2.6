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

 // Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MSetElementCountCommand.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MSetElementCountCommand::MSetElementCountCommand(
    NiSetElementCountCommand* pkCommand,
    MPropertyContainer* pmPropertyContainer) : m_pkCommand(pkCommand),
    m_pmPropertyContainer(pmPropertyContainer)
{
    MInitRefObject(m_pkCommand);

    MAssert(m_pmPropertyContainer != NULL, "Null property container provided "
        "to constructor!");
    MAssert(m_pmPropertyContainer->PropertyInterface ==
        m_pkCommand->GetPropertyInterface(), "Command and container "
        "don't match!");
}
//---------------------------------------------------------------------------
void MSetElementCountCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
String* MSetElementCountCommand::get_PropertyName()
{
    MVerifyValidInstance;

    return m_pkCommand->GetPropertyName();
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MSetElementCountCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MSetElementCountCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MSetElementCountCommand::DoCommand(bool bInBatch, bool bUndoable)
{
    MVerifyValidInstance;

    // Raise event.
    if (m_pmPropertyContainer->GetType() == __typeof(MEntity))
    {
        MEntity* pmEntity = static_cast<MEntity*>(m_pmPropertyContainer);
        MEventManager::Instance->RaiseEntityPropertyChanging(pmEntity,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }
    else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
    {
        MComponent* pmComponent = static_cast<MComponent*>(
            m_pmPropertyContainer);
        MEventManager::Instance->RaiseComponentPropertyChanging(pmComponent,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pmPropertyContainer->GetType() == __typeof(MEntity))
    {
        MEntity* pmEntity = static_cast<MEntity*>(m_pmPropertyContainer);
        MEventManager::Instance->RaiseEntityPropertyChanged(pmEntity,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }
    else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
    {
        MComponent* pmComponent = static_cast<MComponent*>(
            m_pmPropertyContainer);
        MEventManager::Instance->RaiseComponentPropertyChanged(pmComponent,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }
}
//---------------------------------------------------------------------------
void MSetElementCountCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Raise event.
    if (m_pmPropertyContainer->GetType() == __typeof(MEntity))
    {
        MEntity* pmEntity = static_cast<MEntity*>(m_pmPropertyContainer);
        MEventManager::Instance->RaiseEntityPropertyChanging(pmEntity,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }
    else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
    {
        MComponent* pmComponent = static_cast<MComponent*>(
            m_pmPropertyContainer);
        MEventManager::Instance->RaiseComponentPropertyChanging(pmComponent,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pmPropertyContainer->GetType() == __typeof(MEntity))
    {
        MEntity* pmEntity = static_cast<MEntity*>(m_pmPropertyContainer);
        MEventManager::Instance->RaiseEntityPropertyChanged(pmEntity,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }
    else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
    {
        MComponent* pmComponent = static_cast<MComponent*>(
            m_pmPropertyContainer);
        MEventManager::Instance->RaiseComponentPropertyChanged(pmComponent,
            m_pkCommand->GetPropertyName(), 0, bInBatch);
    }
}
//---------------------------------------------------------------------------
