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

#include "MChangeFrozenStateCommand.h"
#include "MEventManager.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MChangeFrozenStateCommand::MChangeFrozenStateCommand(MEntity* pmEntity,
    bool bNewFrozenState) : m_pmEntity(pmEntity),
    m_bNewFrozenState(bNewFrozenState), m_bOldDataValid(false)
{
}
//---------------------------------------------------------------------------
void MChangeFrozenStateCommand::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MChangeFrozenStateCommand::get_Name()
{
    MVerifyValidInstance;

    String* strCommand = m_bNewFrozenState ? "Freeze" : "Unfreeze";
    return String::Format("{0} \"{1}\" entity", strCommand, m_pmEntity->Name);
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MChangeFrozenStateCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MChangeFrozenStateCommand::DoCommand(bool, bool)
{
    MVerifyValidInstance;

    // Execute command.
    m_bOldFrozenState = m_pmEntity->Frozen;
    m_bOldDataValid = true;
    m_pmEntity->m_bFrozen = m_bNewFrozenState;

    // Raise event.
    MEventManager::Instance->RaiseEntityFrozenStateChanged(m_pmEntity,
        m_bNewFrozenState);
}
//---------------------------------------------------------------------------
void MChangeFrozenStateCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Report errors.
    if (!m_bOldDataValid)
    {
        const char* pcCommandName = MStringToCharPointer(this->Name);
        const char* pcEntityName = MStringToCharPointer(m_pmEntity->Name);

        NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler(1);
        spErrors->ReportError("Can't undo command; the command must be done "
            "before it can be undone.", pcCommandName, pcEntityName, NULL);

        MFreeCharPointer(pcEntityName);
        MFreeCharPointer(pcCommandName);

        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
    }

    // Undo command.
    m_pmEntity->m_bFrozen = m_bOldFrozenState;

    // Raise event.
    MEventManager::Instance->RaiseEntityFrozenStateChanged(m_pmEntity,
        m_bOldFrozenState);
}
//---------------------------------------------------------------------------
