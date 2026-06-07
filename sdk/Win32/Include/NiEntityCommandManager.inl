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

//---------------------------------------------------------------------------
inline NiBool NiEntityCommandManager::IsUndoFrameOpen()
{
    return !m_kUndoFrameStack.IsEmpty();
}
//---------------------------------------------------------------------------
inline void NiEntityCommandManager::ExecuteCommand(
    NiEntityCommandInterface* pkCommand, NiEntityErrorInterface* pkErrors,
    bool bAddToUndoStack)
{
    pkCommand->DoCommand(pkErrors, bAddToUndoStack);

    if (bAddToUndoStack)
    {
        if (IsUndoFrameOpen())
        {
            m_kUndoFrameStack.GetHead()->AddCommand(pkCommand);
        }
        else
        {
            m_kUndoStack.AddHead(pkCommand);
            m_kRedoStack.RemoveAll();
        }
    }
}
//---------------------------------------------------------------------------
inline void NiEntityCommandManager::BeginUndoFrame(const NiFixedString& kName)
{
    m_kUndoFrameStack.AddHead(NiNew NiBatchCommand(kName));
}
//---------------------------------------------------------------------------
inline void NiEntityCommandManager::EndUndoFrame(bool bAddToUndoStack)
{
    NIASSERT(IsUndoFrameOpen() && "Undo frame not open!");
    if (IsUndoFrameOpen())
    {
        NiBatchCommandPtr spUndoFrame = m_kUndoFrameStack.RemoveHead();
        if (bAddToUndoStack)
        {
            if (IsUndoFrameOpen())
            {
                m_kUndoFrameStack.GetHead()->AddCommand(spUndoFrame);
            }
            else
            {
                m_kUndoStack.AddHead(NiSmartPointerCast(NiBatchCommand,
                    spUndoFrame));
                m_kRedoStack.RemoveAll();
            }
        }
    }
}
//---------------------------------------------------------------------------
inline const NiEntityCommandManager::CommandList&
    NiEntityCommandManager::GetUndoStack() const
{
    return m_kUndoStack;
}
//---------------------------------------------------------------------------
inline const NiEntityCommandManager::CommandList&
    NiEntityCommandManager::GetRedoStack() const
{
    return m_kRedoStack;
}
//---------------------------------------------------------------------------
