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

#include "MRenameEntityCommand.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MRenameEntityCommand::MRenameEntityCommand(
    NiRenameEntityCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MRenameEntityCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MRenameEntityCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MRenameEntityCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MRenameEntityCommand::DoCommand(bool bInBatch, bool bUndoable)
{
    MVerifyValidInstance;

    // Raise event.
    MEventManager::Instance->RaiseEntityNameChanging(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetNewName(), bInBatch);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseEntityNameChanged(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetOldName(), bInBatch);
}
//---------------------------------------------------------------------------
void MRenameEntityCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Raise event.
    MEventManager::Instance->RaiseEntityNameChanging(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetOldName(), bInBatch);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseEntityNameChanged(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetNewName(), bInBatch);
}
//---------------------------------------------------------------------------
