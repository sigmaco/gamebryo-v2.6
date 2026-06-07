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
#include "NiEntityPCH.h"

#include "NiEntityCommandManager.h"

NiFixedString NiEntityCommandManager::ERR_UNDO_FRAME_OPEN;
NiFixedString NiEntityCommandManager::ERR_UNDO_FRAME_OPEN_DESCRIPTION;

//---------------------------------------------------------------------------
void NiEntityCommandManager::_SDMInit()
{
    ERR_UNDO_FRAME_OPEN = "Undo/redo command not issued; undo frame "
        "open.";
    ERR_UNDO_FRAME_OPEN_DESCRIPTION = "Undo/redo commands cannot be issued "
        "while an undo frame is open.";
}
//---------------------------------------------------------------------------
void NiEntityCommandManager::_SDMShutdown()
{
    ERR_UNDO_FRAME_OPEN = NULL;
    ERR_UNDO_FRAME_OPEN_DESCRIPTION = NULL;
}
//---------------------------------------------------------------------------
void NiEntityCommandManager::UndoCommands(unsigned int uiNumToUndo,
    NiEntityErrorInterface* pkErrors)
{
    NIASSERT(!IsUndoFrameOpen() && "Undo frame is open!");
    if (IsUndoFrameOpen())
    {
        pkErrors->ReportError(ERR_UNDO_FRAME_OPEN,
            ERR_UNDO_FRAME_OPEN_DESCRIPTION, NULL, NULL);
        return;
    }

    unsigned int uiCount = 0;
    while (!m_kUndoStack.IsEmpty() && uiCount++ < uiNumToUndo)
    {
        NiEntityCommandInterfaceIPtr spCommand = m_kUndoStack.RemoveHead();
        spCommand->UndoCommand(pkErrors);
        m_kRedoStack.AddHead(spCommand);
    }
}
//---------------------------------------------------------------------------
void NiEntityCommandManager::RedoCommands(unsigned int uiNumToRedo,
    NiEntityErrorInterface* pkErrors)
{
    NIASSERT(!IsUndoFrameOpen() && "Undo frame is open!");
    if (IsUndoFrameOpen())
    {
        pkErrors->ReportError(ERR_UNDO_FRAME_OPEN,
            ERR_UNDO_FRAME_OPEN_DESCRIPTION, NULL, NULL);
        return;
    }

    unsigned int uiCount = 0;
    while (!m_kRedoStack.IsEmpty() && uiCount++ < uiNumToRedo)
    {
        NiEntityCommandInterfaceIPtr spCommand = m_kRedoStack.RemoveHead();
        spCommand->DoCommand(pkErrors, true);
        m_kUndoStack.AddHead(spCommand);
    }
}
//---------------------------------------------------------------------------
