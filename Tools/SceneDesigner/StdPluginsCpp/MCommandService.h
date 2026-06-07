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

using namespace System::Collections;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MCommandService : public MDisposable,
        public ICommandService
    {
    public:
        MCommandService();

    private:
        bool IsUndoFrameOpen();

        void ClearUndoStack(bool bRaiseEvent);
        void ClearRedoStack(bool bRaiseEvent);
        void ClearUndoFrameStack();

        void PushOntoUndoStack(ICommand* pmCommand);
        ICommand* PopFromUndoStack();
        void PushOntoRedoStack(ICommand* pmCommand);
        ICommand* PopFromRedoStack();

        bool BeginUndoFrame(String* strName, bool bLongOperation);
        bool EndUndoFrame(bool bAddToUndoStack, bool bLongOperation);

        // UI Command handlers.
        [UICommandHandlerAttribute("Undo")]
        void OnUndo(Object* pmSender, EventArgs* pmEventArgs);
        [UICommandValidatorAttribute("Undo")]
        void OnValidateUndo(Object* pmSender, UIState* pmState);
        [UICommandHandlerAttribute("Redo")]
        void OnRedo(Object* pmSender, EventArgs* pmEventArgs);
        [UICommandValidatorAttribute("Redo")]
        void OnValidateRedo(Object* pmSender, UIState* pmState);

        void OnSceneClosing(MScene* pmScene);
        void OnNewSceneLoaded(MScene* pmScene);
        void OnPaletteRemoved(MPalette* pmPalette);


        Stack* m_pmUndoStack;
        Stack* m_pmRedoStack;
        Stack* m_pmUndoFrameStack;
        bool m_bUndoingRedoing;

        static IMessageService* ms_pmMessageService;
        __property static IMessageService* get_MessageService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // ICommandService members.
    public:
        virtual __event UndoRedoHandler* CommandsAddedToUndoStack;
        virtual __event UndoRedoHandler* CommandsRemovedFromUndoStack;
        virtual __event UndoRedoHandler* CommandsAddedToRedoStack;
        virtual __event UndoRedoHandler* CommandsRemovedFromRedoStack;
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
        void ClearUndoAndRedoStacks();
        void RemoveFromUndoRedoStack(ICommand* pmCommand);
    };
}}}}
