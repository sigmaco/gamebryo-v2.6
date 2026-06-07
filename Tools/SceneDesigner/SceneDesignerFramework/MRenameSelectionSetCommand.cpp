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

#include "MRenameSelectionSetCommand.h"
#include "MEventManager.h"
#include "MSelectionSetFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MRenameSelectionSetCommand::MRenameSelectionSetCommand(
    NiRenameSelectionSetCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MRenameSelectionSetCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MRenameSelectionSetCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MRenameSelectionSetCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MRenameSelectionSetCommand::DoCommand(bool bInBatch, bool bUndoable)
{
    MVerifyValidInstance;

    // Raise event.
    MEventManager::Instance->RaiseSelectionSetNameChanging(
        MSelectionSetFactory::Instance->Get(m_pkCommand->GetSelectionSet()),
        m_pkCommand->GetNewName(), bInBatch);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseSelectionSetNameChanged(
        MSelectionSetFactory::Instance->Get(m_pkCommand->GetSelectionSet()),
        m_pkCommand->GetOldName(), bInBatch);
}
//---------------------------------------------------------------------------
void MRenameSelectionSetCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Raise event.
    MEventManager::Instance->RaiseSelectionSetNameChanging(
        MSelectionSetFactory::Instance->Get(m_pkCommand->GetSelectionSet()),
        m_pkCommand->GetOldName(), bInBatch);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseSelectionSetNameChanged(
        MSelectionSetFactory::Instance->Get(m_pkCommand->GetSelectionSet()),
        m_pkCommand->GetNewName(), bInBatch);
}
//---------------------------------------------------------------------------
