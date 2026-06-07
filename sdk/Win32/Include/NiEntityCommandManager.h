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

#ifndef NIENTITYCOMMANDMANAGER_H
#define NIENTITYCOMMANDMANAGER_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include <NiBool.h>
#include <NiTPointerList.h>
#include "NiBatchCommand.h"

class NIENTITY_ENTRY NiEntityCommandManager : public NiRefObject
{
public:
    typedef NiTPointerList<NiEntityCommandInterfaceIPtr> CommandList;

    // Error strings.
    static NiFixedString ERR_UNDO_FRAME_OPEN;
    static NiFixedString ERR_UNDO_FRAME_OPEN_DESCRIPTION;

    inline void ExecuteCommand(NiEntityCommandInterface* pkCommand,
        NiEntityErrorInterface* pkErrors, bool bAddToUndoStack);

    void UndoCommands(unsigned int uiNumToUndo,
        NiEntityErrorInterface* pkErrors);
    void RedoCommands(unsigned int uiNumToRedo,
        NiEntityErrorInterface* pkErrors);

    inline void BeginUndoFrame(const NiFixedString& kName);
    inline void EndUndoFrame(bool bAddToUndoStack);

    inline const CommandList& GetUndoStack() const;
    inline const CommandList& GetRedoStack() const;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

private:
    inline NiBool IsUndoFrameOpen();

    CommandList m_kUndoStack;
    CommandList m_kRedoStack;
    NiTPointerList<NiBatchCommandPtr> m_kUndoFrameStack;
};

NiSmartPointer(NiEntityCommandManager);

#include "NiEntityCommandManager.inl"

#endif // NIENTITYCOMMANDMANAGER_H
