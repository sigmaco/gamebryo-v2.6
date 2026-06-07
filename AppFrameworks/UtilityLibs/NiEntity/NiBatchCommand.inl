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
inline NiBatchCommand::NiBatchCommand(
    const NiFixedString& kName,
    unsigned int uiCommandArraySize) 
    :
    m_kCommands(uiCommandArraySize),
    m_kName(kName)
{
}
//---------------------------------------------------------------------------
inline void NiBatchCommand::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline void NiBatchCommand::AddCommand(
    NiEntityCommandInterface* pkCommand)
{
    NIASSERT(pkCommand != NULL);
    m_kCommands.Add(pkCommand);
}
//---------------------------------------------------------------------------
inline unsigned int NiBatchCommand::GetCommandCount() const
{
    return m_kCommands.GetSize();
}
//---------------------------------------------------------------------------
inline NiEntityCommandInterface* NiBatchCommand::GetCommandAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kCommands.GetSize());
    return m_kCommands.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiBatchCommand::RemoveCommand(
    NiEntityCommandInterface* pkCommand)
{
    NIASSERT(pkCommand);
    int iIndex = m_kCommands.Find(pkCommand);
    if (iIndex != -1)
    {
        m_kCommands.OrderedRemoveAt(iIndex);
    }
}
//---------------------------------------------------------------------------
inline void NiBatchCommand::RemoveCommandAt(unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_kCommands.GetSize());
    m_kCommands.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiBatchCommand::RemoveAllCommands()
{
    m_kCommands.RemoveAll();
}
//---------------------------------------------------------------------------
