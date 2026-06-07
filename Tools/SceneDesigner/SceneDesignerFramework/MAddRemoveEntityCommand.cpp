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

#include "MAddRemoveEntityCommand.h"
#include "MEventManager.h"
#include "MSceneFactory.h"
#include "MEntityFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MAddRemoveEntityCommand::MAddRemoveEntityCommand(
    NiAddRemoveEntityCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MAddRemoveEntityCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MAddRemoveEntityCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MAddRemoveEntityCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MAddRemoveEntityCommand::DoCommand(bool, bool bUndoable)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddEntity())
    {
        MEventManager::Instance->RaiseEntityAddedToScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()));
    }
    else
    {
        MEventManager::Instance->RaiseEntityRemovedFromScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()));
    }
}
//---------------------------------------------------------------------------
void MAddRemoveEntityCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Undo command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddEntity())
    {
        MEventManager::Instance->RaiseEntityRemovedFromScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()));
    }
    else
    {
        MEventManager::Instance->RaiseEntityAddedToScene(
            MSceneFactory::Instance->Get(m_pkCommand->GetScene()),
            MEntityFactory::Instance->Get(m_pkCommand->GetEntity()));
    }
}
//---------------------------------------------------------------------------
