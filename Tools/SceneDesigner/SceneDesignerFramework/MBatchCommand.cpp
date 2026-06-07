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

#include "MBatchCommand.h"
#include "MEventManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MBatchCommand::MBatchCommand(String* strName) : m_strName(strName)
{
    m_pmCommands = new ArrayList();
}
//---------------------------------------------------------------------------
void MBatchCommand::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        ClearCommandsArray();
    }
}
//---------------------------------------------------------------------------
void MBatchCommand::ClearCommandsArray()
{
    for (int i = 0; i < m_pmCommands->Count; i++)
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            m_pmCommands->Item[i]);
        if (pmDisposable)
        {
            pmDisposable->Dispose();
        }
    }
    m_pmCommands->Clear();
}
//---------------------------------------------------------------------------
void MBatchCommand::AddCommand(ICommand* pmCommand)
{
    MVerifyValidInstance;

    MAssert(pmCommand != NULL, "Null command provided to function!");

    m_pmCommands->Add(pmCommand);
}
//---------------------------------------------------------------------------
void MBatchCommand::RemoveCommand(ICommand* pmCommand)
{
    MVerifyValidInstance;

    MAssert(pmCommand != NULL, "Null command provided to function!");

    int iIndex = m_pmCommands->IndexOf(pmCommand);
    if (iIndex > -1)
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            m_pmCommands->Item[iIndex]);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
        m_pmCommands->RemoveAt(iIndex);
    }
}
//---------------------------------------------------------------------------
void MBatchCommand::RemoveAllCommands()
{
    MVerifyValidInstance;

    ClearCommandsArray();
}
//---------------------------------------------------------------------------
ICommand* MBatchCommand::GetCommands()[]
{
    MVerifyValidInstance;

    return dynamic_cast<ICommand*[]>(m_pmCommands->ToArray(
        __typeof(ICommand)));
}
//---------------------------------------------------------------------------
String* MBatchCommand::get_Name()
{
    MVerifyValidInstance;

    return m_strName;
}
//---------------------------------------------------------------------------
NiEntityCommandInterface* MBatchCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return NULL;
}
//---------------------------------------------------------------------------
void MBatchCommand::DoCommand(bool, bool bUndoable)
{
    MVerifyValidInstance;

    // Execute all sub-commands.
    for (int i = 0; i < m_pmCommands->Count; i++)
    {
        ICommand* pmCommand = dynamic_cast<ICommand*>(m_pmCommands->Item[i]);
        MAssert(pmCommand != NULL, "Null command in array!");

        pmCommand->DoCommand(true, bUndoable);
    }
}
//---------------------------------------------------------------------------
void MBatchCommand::UndoCommand(bool)
{
    MVerifyValidInstance;

    // Undo all sub-commands.
    for (int i = m_pmCommands->Count; i > 0; i--)
    {
        ICommand* pmCommand = dynamic_cast<ICommand*>(
            m_pmCommands->Item[i - 1]);
        MAssert(pmCommand != NULL, "Null command in array!");

        pmCommand->UndoCommand(true);
    }
}
//---------------------------------------------------------------------------
