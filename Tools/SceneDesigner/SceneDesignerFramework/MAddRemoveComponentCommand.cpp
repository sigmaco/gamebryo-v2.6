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

#include "MAddRemoveComponentCommand.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MComponentFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MAddRemoveComponentCommand::MAddRemoveComponentCommand(
    NiAddRemoveComponentCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MAddRemoveComponentCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MAddRemoveComponentCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MAddRemoveComponentCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MAddRemoveComponentCommand::DoCommand(bool, bool bUndoable)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddComponent())
    {
        MEventManager::Instance->RaiseEntityComponentAdded(
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
            MComponentFactory::Instance->Get(m_pkCommand->GetComponent()));
    }
    else
    {
        MEventManager::Instance->RaiseEntityComponentRemoved(
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
            MComponentFactory::Instance->Get(m_pkCommand->GetComponent()));
    }
}
//---------------------------------------------------------------------------
void MAddRemoveComponentCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Undo command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddComponent())
    {
        MEventManager::Instance->RaiseEntityComponentRemoved(
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
            MComponentFactory::Instance->Get(m_pkCommand->GetComponent()));
    }
    else
    {
        MEventManager::Instance->RaiseEntityComponentAdded(
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
            MComponentFactory::Instance->Get(m_pkCommand->GetComponent()));
    }
}
//---------------------------------------------------------------------------
