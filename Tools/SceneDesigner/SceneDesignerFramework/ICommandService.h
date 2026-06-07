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

#pragma once

#include "IService.h"
#include "ICommand.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    public __delegate void UndoRedoHandler(ICommand* amCommands[]);

    /// <summary>
    /// Summary description for ICommandService.
    /// </summary>
    public __gc __interface ICommandService : public IService
    {
        __event UndoRedoHandler* CommandsAddedToUndoStack;
        __event UndoRedoHandler* CommandsRemovedFromUndoStack;
        __event UndoRedoHandler* CommandsAddedToRedoStack;
        __event UndoRedoHandler* CommandsRemovedFromRedoStack;

        void ExecuteCommand(ICommand* pmCommand, bool bAddToUndoStack);

        void UndoCommands(unsigned int uiNumToUndo);
        void RedoCommands(unsigned int uiNumToRedo);

        bool BeginUndoFrame(String* strName);
        bool EndUndoFrame(bool bAddToUndoStack);

        __property unsigned int get_UndoStackCount();
        __property unsigned int get_RedoStackCount();

        String* GetUndoCommandNames()[];
        String* GetRedoCommandNames()[];

        ICommand* GetUndoCommands()[];
        ICommand* GetRedoCommands()[];

        void RemoveFromUndoRedoStack(ICommand* pmCommand);

        void ClearUndoAndRedoStacks();
    };
}}}}}
