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
#include "StdPluginsCppPCH.h"

#include "MCommandService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MCommandService::MCommandService() : m_bUndoingRedoing(false)
{
    m_pmUndoStack = new Stack();
    m_pmRedoStack = new Stack();
    m_pmUndoFrameStack = new Stack();
}
//---------------------------------------------------------------------------
void MCommandService::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        __unhook(&MEventManager::SceneClosing,
            MFramework::Instance->EventManager,
            &MCommandService::OnSceneClosing);
        __unhook(&MEventManager::NewSceneLoaded,
            MFramework::Instance->EventManager,
            &MCommandService::OnNewSceneLoaded);

        __unhook(&MEventManager::PaletteRemoved,
            MFramework::Instance->EventManager,
            &MCommandService::OnPaletteRemoved);

        ClearUndoStack(false);
        ClearRedoStack(false);
        ClearUndoFrameStack();
    }
}
//---------------------------------------------------------------------------
bool MCommandService::IsUndoFrameOpen()
{
    return (m_pmUndoFrameStack->Count > 0);
}
//---------------------------------------------------------------------------
void MCommandService::RemoveFromUndoRedoStack(ICommand* pmCommand)
{
    MAssert(pmCommand != NULL);
    if (pmCommand == NULL)
        return;

    // Collect commands.
    ICommand* amCommands[] = new ICommand*[m_pmUndoStack->Count];
    int iIndex = 0;
    IEnumerator* pmEnumerator = m_pmUndoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        ICommand* pmCmdToCheck = dynamic_cast<ICommand*>(
            pmEnumerator->Current);
        MAssert(pmCommand != NULL, "Invalid command in undo stack!");
        
        if (pmCommand != pmCmdToCheck)
            amCommands[iIndex++] = pmCmdToCheck;
    }

    // Clear undo stack.
    m_pmUndoStack->Clear();
    m_pmUndoStack = new Stack();
    for (int i = iIndex - 1; i >= 0; i--)
        m_pmUndoStack->Push(amCommands[i]);

    CommandsRemovedFromUndoStack(NULL);

    IDisposable* pmDisposable = dynamic_cast<IDisposable*>(pmCommand);
    if (pmDisposable != NULL)
    {
        pmDisposable->Dispose();
    }
}
//---------------------------------------------------------------------------
void MCommandService::ClearUndoStack(bool bRaiseEvent)
{
    // Skip if stack is empty.
    if (m_pmUndoStack->Count == 0)
    {
        return;
    }

    // Collect commands.
    ICommand* amCommands[] = new ICommand*[m_pmUndoStack->Count];
    int iIndex = 0;
    IEnumerator* pmEnumerator = m_pmUndoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        ICommand* pmCommand = dynamic_cast<ICommand*>(
            pmEnumerator->Current);
        MAssert(pmCommand != NULL, "Invalid command in undo stack!");
        amCommands[iIndex++] = pmCommand;
    }

    // Clear undo stack.
    m_pmUndoStack->Clear();

    // Raise event.
    if (bRaiseEvent)
    {
        CommandsRemovedFromUndoStack(amCommands);
    }

    // Dispose commands.
    for (int i = 0; i < amCommands->Length; i++)
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            amCommands->Item[i]);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
    }
}
//---------------------------------------------------------------------------
void MCommandService::ClearRedoStack(bool bRaiseEvent)
{
    // Skip if stack is empty.
    if (m_pmRedoStack->Count == 0)
    {
        return;
    }

    // Collect commands.
    ICommand* amCommands[] = new ICommand*[m_pmRedoStack->Count];
    int iIndex = 0;
    IEnumerator* pmEnumerator = m_pmRedoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        ICommand* pmCommand = dynamic_cast<ICommand*>(
            pmEnumerator->Current);
        MAssert(pmCommand != NULL, "Invalid command in redo stack!");
        amCommands[iIndex++] = pmCommand;
    }

    // Clear redo stack.
    m_pmRedoStack->Clear();

    // Raise event.
    if (bRaiseEvent)
    {
        CommandsRemovedFromRedoStack(amCommands);
    }

    // Dispose commands.
    for (int i = 0; i < amCommands->Length; i++)
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            amCommands->Item[i]);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
    }
}
//---------------------------------------------------------------------------
void MCommandService::ClearUndoFrameStack()
{
    // Call dispose on all commands in the stack.
    IEnumerator* pmEnumerator = m_pmUndoFrameStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
            pmEnumerator->Current);
        if (pmDisposable)
        {
            pmDisposable->Dispose();
        }
    }

    m_pmUndoFrameStack->Clear();
}
//---------------------------------------------------------------------------
void MCommandService::PushOntoUndoStack(ICommand* pmCommand)
{
    // Push onto undo stack.
    m_pmUndoStack->Push(pmCommand);

    // Raise event.
    ICommand* amCommands[] = new ICommand*[1];
    amCommands[0] = pmCommand;
    CommandsAddedToUndoStack(amCommands);
}
//---------------------------------------------------------------------------
ICommand* MCommandService::PopFromUndoStack()
{
    // Pop from undo stack.
    ICommand* pmCommand = dynamic_cast<ICommand*>(m_pmUndoStack->Pop());
    MAssert(pmCommand != NULL, "Invalid command in undo stack!");

    // Raise event.
    ICommand* amCommands[] = new ICommand*[1];
    amCommands[0] = pmCommand;
    CommandsRemovedFromUndoStack(amCommands);

    return pmCommand;
}
//---------------------------------------------------------------------------
void MCommandService::PushOntoRedoStack(ICommand* pmCommand)
{
    // Push onto redo stack.
    m_pmRedoStack->Push(pmCommand);

    // Raise event.
    ICommand* amCommands[] = new ICommand*[1];
    amCommands[0] = pmCommand;
    CommandsAddedToRedoStack(amCommands);
}
//---------------------------------------------------------------------------
ICommand* MCommandService::PopFromRedoStack()
{
    // Pop from redo stack.
    ICommand* pmCommand = dynamic_cast<ICommand*>(m_pmRedoStack->Pop());
    MAssert(pmCommand != NULL, "Invalid command in redo stack!");

    // Raise event.
    ICommand* amCommands[] = new ICommand*[1];
    amCommands[0] = pmCommand;
    CommandsRemovedFromRedoStack(amCommands);

    return pmCommand;
}
//---------------------------------------------------------------------------
void MCommandService::OnUndo(Object*, EventArgs*)
{
    UndoCommands(1);
}
//---------------------------------------------------------------------------
void MCommandService::OnValidateUndo(Object*, UIState* pmState)
{
    if (UndoStackCount > 0)
    {
        pmState->Enabled = true;
    }
    else
    {
        pmState->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void MCommandService::OnRedo(Object*, EventArgs*)
{
    RedoCommands(1);
}
//---------------------------------------------------------------------------
void MCommandService::OnValidateRedo(Object*, UIState* pmState)
{
    if (RedoStackCount > 0)
    {
        pmState->Enabled = true;
    }
    else
    {
        pmState->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void MCommandService::OnSceneClosing(MScene*)
{
    MVerifyValidInstance;

    ClearUndoAndRedoStacks();
}
//---------------------------------------------------------------------------
void MCommandService::OnNewSceneLoaded(MScene*)
{
    MVerifyValidInstance;

    ClearUndoAndRedoStacks();
}
//---------------------------------------------------------------------------
void MCommandService::OnPaletteRemoved(MPalette* pmPalette)
{
    MVerifyValidInstance;

    if (!pmPalette->DontSave)
    {
        ClearUndoStack(true);
        ClearRedoStack(true);
        ClearUndoFrameStack();
    }
}
//---------------------------------------------------------------------------
IMessageService* MCommandService::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MCommandService::get_Name()
{
    MVerifyValidInstance;

    return "Command Service";
}
//---------------------------------------------------------------------------
bool MCommandService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MCommandService::Start()
{
    MVerifyValidInstance;

    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    MAssert(pmUICommandService != NULL, "UI command service not found!");
    pmUICommandService->BindCommands(this);

    __hook(&MEventManager::SceneClosing, MFramework::Instance->EventManager,
        &MCommandService::OnSceneClosing);
    __hook(&MEventManager::NewSceneLoaded, MFramework::Instance->EventManager,
        &MCommandService::OnNewSceneLoaded);
    __hook(&MEventManager::PaletteRemoved, MFramework::Instance->EventManager,
        &MCommandService::OnPaletteRemoved);

    return true;
}
//---------------------------------------------------------------------------
// ICommandService members.
//---------------------------------------------------------------------------
void MCommandService::ExecuteCommand(ICommand* pmCommand,
    bool bAddToUndoStack)
{
    MVerifyValidInstance;

    MAssert(pmCommand != NULL, "Null command provided to function!");

    // Ignore any undoable commands executed during an undo or redo operation.
    // This is to avoid corruption of the undo stack during such an
    // operation.
    if (m_bUndoingRedoing && bAddToUndoStack)
    {
        return;
    }

    // Begin an undo frame here with the same name as the command. This is
    // done so that if any event handlers responding to the command issue
    // undoable commands of their own, those commands will be added to a
    // single batch with this command. This will ensure that only one entry
    // will exist in the undo stack for this command.
    BeginUndoFrame(pmCommand->Name, false);

    // Add this command to the undo stack. This is done prior to executing the
    // command so that it will be the first command in the undo frame, prior
    // to any other commands that may be executed in response to events raised
    // by this command.
    if (bAddToUndoStack)
    {
        // Add to batch or undo stack.
        if (IsUndoFrameOpen())
        {
            MBatchCommand* pmUndoFrame = dynamic_cast<MBatchCommand*>(
                m_pmUndoFrameStack->Peek());
            MAssert(pmUndoFrame != NULL, "Invalid undo frame!");
            pmUndoFrame->AddCommand(pmCommand);
        }
        else
        {
            PushOntoUndoStack(pmCommand);
            ClearRedoStack(true);
        }
    }

    // Execute command.
    pmCommand->DoCommand(false, bAddToUndoStack);

    // End the undo frame for this command.
    EndUndoFrame(bAddToUndoStack, false);

    // If not added to undo stack, dispose command.
    if (!bAddToUndoStack)
    {
        IDisposable* pmDisposable = dynamic_cast<IDisposable*>(pmCommand);
        if (pmDisposable != NULL)
        {
            pmDisposable->Dispose();
        }
    }
}
//---------------------------------------------------------------------------
void MCommandService::UndoCommands(unsigned int uiNumToUndo)
{
    MVerifyValidInstance;

    MAssert(!IsUndoFrameOpen(), "Undo frame is open!");
    if (IsUndoFrameOpen())
    {
        // Undo batch is open; cannot undo commands. Report error and return.
        NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
        spErrors->ReportError(NiEntityCommandManager::ERR_UNDO_FRAME_OPEN,
            NiEntityCommandManager::ERR_UNDO_FRAME_OPEN_DESCRIPTION, NULL,
            NULL);
        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
        return;
    }

    MFramework::Instance->BeginLongOperation();

    m_bUndoingRedoing = true;
    for (unsigned int ui = 0; m_pmUndoStack->Count > 0 && ui < uiNumToUndo;
        ui++)
    {
        // Get command from undo stack.
        ICommand* pmCommand = PopFromUndoStack();

        // Undo command.
        pmCommand->UndoCommand(false);

        // Push command onto redo stack.
        PushOntoRedoStack(pmCommand);
    }
    m_bUndoingRedoing = false;

    MFramework::Instance->EndLongOperation();
}
//---------------------------------------------------------------------------
void MCommandService::RedoCommands(unsigned int uiNumToRedo)
{
    MVerifyValidInstance;

    MAssert(!IsUndoFrameOpen(), "Undo frame is open!");
    if (IsUndoFrameOpen())
    {
        // Undo batch is open; cannot redo commands. Report error and return.
        NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
        spErrors->ReportError(NiEntityCommandManager::ERR_UNDO_FRAME_OPEN,
            NiEntityCommandManager::ERR_UNDO_FRAME_OPEN_DESCRIPTION, NULL,
            NULL);
        MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
            spErrors);
        return;
    }

    MFramework::Instance->BeginLongOperation();

    m_bUndoingRedoing = true;
    for (unsigned int ui = 0; m_pmRedoStack->Count > 0 && ui < uiNumToRedo;
        ui++)
    {
        // Get command from redo stack.
        ICommand* pmCommand = PopFromRedoStack();

        // Redo commands.
        pmCommand->DoCommand(false, true);

        // Push command onto undo stack.
        PushOntoUndoStack(pmCommand);
    }
    m_bUndoingRedoing = false;

    MFramework::Instance->EndLongOperation();
}
//---------------------------------------------------------------------------
bool MCommandService::BeginUndoFrame(String* strName)
{
    MVerifyValidInstance;

    return BeginUndoFrame(strName, true);
}
//---------------------------------------------------------------------------
bool MCommandService::BeginUndoFrame(String* strName, bool bLongOperation)
{
    MVerifyValidInstance;

    // Ignore any undo frames begun during an undo or redo operation.
    // This is to avoid corruption of the undo stack during such an
    // operation.
    if (m_bUndoingRedoing)
    {
        return false;
    }

    m_pmUndoFrameStack->Push(new MBatchCommand(strName));

    if (bLongOperation)
    {
        MFramework::Instance->BeginLongOperation();
    }

    return true;
}
//---------------------------------------------------------------------------
bool MCommandService::EndUndoFrame(bool bAddToUndoStack)
{
    MVerifyValidInstance;

    return EndUndoFrame(bAddToUndoStack, true);
}
//---------------------------------------------------------------------------
bool MCommandService::EndUndoFrame(bool bAddToUndoStack, bool bLongOperation)
{
    MVerifyValidInstance;

    // Ignore any undo frames ended during an undo or redo operation.
    // This is to avoid corruption of the undo stack during such an
    // operation.
    if (m_bUndoingRedoing)
    {
        return false;
    }

    if (IsUndoFrameOpen())
    {
        MBatchCommand* pmUndoFrame = dynamic_cast<MBatchCommand*>(
            m_pmUndoFrameStack->Pop());
        MAssert(pmUndoFrame != NULL, "Invalid undo frame!");
        if (bAddToUndoStack)
        {
            if (IsUndoFrameOpen())
            {
                MBatchCommand* pmParentFrame = dynamic_cast<MBatchCommand*>(
                    m_pmUndoFrameStack->Peek());
                MAssert(pmParentFrame != NULL, "Invalid undo frame!");
                pmParentFrame->AddCommand(pmUndoFrame);
            }
            else
            {
                PushOntoUndoStack(pmUndoFrame);
                ClearRedoStack(true);
            }
        }
        else
        {
            // If not added to undo stack, dispose command.
            IDisposable* pmDisposable = dynamic_cast<IDisposable*>(
                pmUndoFrame);
            if (pmDisposable != NULL)
            {
                pmDisposable->Dispose();
            }
        }

        if (bLongOperation)
        {
            MFramework::Instance->EndLongOperation();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int MCommandService::get_UndoStackCount()
{
    MVerifyValidInstance;

    return m_pmUndoStack->Count;
}
//---------------------------------------------------------------------------
unsigned int MCommandService::get_RedoStackCount()
{
    MVerifyValidInstance;

    return m_pmRedoStack->Count;
}
//---------------------------------------------------------------------------
String* MCommandService::GetUndoCommandNames()[]
{
    MVerifyValidInstance;

    String* amCommandNames[] = new String*[m_pmUndoStack->Count];
    int iIndex = 0;

    IEnumerator* pmEnumerator = m_pmUndoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        amCommandNames[iIndex++] = dynamic_cast<ICommand*>(
            pmEnumerator->Current)->Name;
    }

    return amCommandNames;
}
//---------------------------------------------------------------------------
String* MCommandService::GetRedoCommandNames()[]
{
    MVerifyValidInstance;

    String* amCommandNames[] = new String*[m_pmRedoStack->Count];
    int iIndex = 0;

    IEnumerator* pmEnumerator = m_pmRedoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        amCommandNames[iIndex++] = dynamic_cast<ICommand*>(
            pmEnumerator->Current)->Name;
    }

    return amCommandNames;
}
//---------------------------------------------------------------------------
ICommand* MCommandService::GetUndoCommands()[]
{
    MVerifyValidInstance;

    ICommand* amCommands[] = new ICommand*[m_pmUndoStack->Count];
    int iIndex = 0;

    IEnumerator* pmEnumerator = m_pmUndoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        amCommands[iIndex++] = dynamic_cast<ICommand*>(pmEnumerator->Current);
    }

    return amCommands;
}
//---------------------------------------------------------------------------
ICommand* MCommandService::GetRedoCommands()[]
{
    MVerifyValidInstance;

    ICommand* amCommands[] = new ICommand*[m_pmRedoStack->Count];
    int iIndex = 0;

    IEnumerator* pmEnumerator = m_pmRedoStack->GetEnumerator();
    while (pmEnumerator->MoveNext())
    {
        amCommands[iIndex++] = dynamic_cast<ICommand*>(pmEnumerator->Current);
    }

    return amCommands;
}
//---------------------------------------------------------------------------
void MCommandService::ClearUndoAndRedoStacks()
{
    ClearUndoStack(true);
    ClearRedoStack(true);
    ClearUndoFrameStack();
}
//---------------------------------------------------------------------------
