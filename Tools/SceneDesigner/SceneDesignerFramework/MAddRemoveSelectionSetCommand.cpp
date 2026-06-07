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

#include "MAddRemoveSelectionSetCommand.h"
#include "MEventManager.h"
#include "MSceneFactory.h"
#include "MSelectionSetFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MAddRemoveSelectionSetCommand::MAddRemoveSelectionSetCommand(
    NiAddRemoveSelectionSetCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MAddRemoveSelectionSetCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MAddRemoveSelectionSetCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MAddRemoveSelectionSetCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MAddRemoveSelectionSetCommand::DoCommand(bool, bool bUndoable)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddSelectionSet())
    {
        MEventManager::Instance->RaiseSelectionSetAddedToScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MSelectionSetFactory::Instance->Get(
            m_pkCommand->GetSelectionSet()));
    }
    else
    {
        MEventManager::Instance->RaiseSelectionSetRemovedFromScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MSelectionSetFactory::Instance->Get(
            m_pkCommand->GetSelectionSet()));
    }
}
//---------------------------------------------------------------------------
void MAddRemoveSelectionSetCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Undo command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddSelectionSet())
    {
        MEventManager::Instance->RaiseSelectionSetRemovedFromScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MSelectionSetFactory::Instance->Get(
            m_pkCommand->GetSelectionSet()));
    }
    else
    {
        MEventManager::Instance->RaiseSelectionSetAddedToScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MSelectionSetFactory::Instance->Get(
            m_pkCommand->GetSelectionSet()));
    }
}
//---------------------------------------------------------------------------
