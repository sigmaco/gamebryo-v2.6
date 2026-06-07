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

#include "MAddRemovePropertyCommand.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MComponentFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MAddRemovePropertyCommand::MAddRemovePropertyCommand(
    NiAddRemovePropertyCommand* pkCommand, 
    MPropertyContainer* pmPropertyContainer) : m_pkCommand(pkCommand),
    m_pmPropertyContainer(pmPropertyContainer)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MAddRemovePropertyCommand::Do_Dispose(bool)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MAddRemovePropertyCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MAddRemovePropertyCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MAddRemovePropertyCommand::DoCommand(bool bInBatch, bool bUndoable)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddProperty())
    {
        if (m_pmPropertyContainer->GetType() == __typeof (MEntity))
        {
            MEventManager::Instance->RaiseEntityPropertyAdded(
                MEntityFactory::Instance->Get(
                (NiEntityInterface*)m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
        else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
        {
            MEventManager::Instance->RaiseComponentPropertyAdded(
                MComponentFactory::Instance->Get((NiEntityComponentInterface*)
                m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
    }
    else
    {
        if (m_pmPropertyContainer->GetType() == __typeof (MEntity))
        {
            MEventManager::Instance->RaiseEntityPropertyRemoved(
                MEntityFactory::Instance->Get(
                (NiEntityInterface*)m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
        else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
        {
            MEventManager::Instance->RaiseComponentPropertyRemoved(
                MComponentFactory::Instance->Get((NiEntityComponentInterface*)
                m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
    }
}
//---------------------------------------------------------------------------
void MAddRemovePropertyCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Undo command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    if (m_pkCommand->GetAddProperty())
    {
        if (m_pmPropertyContainer->GetType() == __typeof (MEntity))
        {
            MEventManager::Instance->RaiseEntityPropertyRemoved(
                MEntityFactory::Instance->Get(
                (NiEntityInterface*)m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
        else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
        {
            MEventManager::Instance->RaiseComponentPropertyRemoved(
                MComponentFactory::Instance->Get((NiEntityComponentInterface*)
                m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
    }
    else
    {
        if (m_pmPropertyContainer->GetType() == __typeof (MEntity))
        {
            MEventManager::Instance->RaiseEntityPropertyAdded(
                MEntityFactory::Instance->Get(
                (NiEntityInterface*)m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
        else if (m_pmPropertyContainer->GetType() == __typeof(MComponent))
        {
            MEventManager::Instance->RaiseComponentPropertyAdded(
                MComponentFactory::Instance->Get((NiEntityComponentInterface*)
                m_pkCommand->GetPropertyInterface()),
                m_pkCommand->GetPropertyName(), bInBatch);
        }
    }
}
//---------------------------------------------------------------------------
