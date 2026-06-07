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
inline NiUInt32 NiSPStreamPartitioner::GetOptimalBlockCount()
{
    return m_uiOptimalBlockCount;
}
//---------------------------------------------------------------------------
inline bool NiSPStreamPartitioner::HasMoreBlocks()
{
    return (m_uiCurrentBlockIndex < m_uiTotalBlockCount);
}
//---------------------------------------------------------------------------
inline void NiSPStreamPartitioner::Reset()
{
    m_uiCurrentBlockIndex = 0;
}
//---------------------------------------------------------------------------
inline NiInt32 NiSPStreamPartitioner::GetBlockIndex()
{
    return m_uiCurrentBlockIndex;
}
//---------------------------------------------------------------------------
inline void NiSPStreamPartitioner::SetTask(NiSPTask* pkTask)
{
    m_pkTask = pkTask;
}
//---------------------------------------------------------------------------
inline void NiSPStreamPartitioner::AdvanceBlockIndex()
{
    m_uiCurrentBlockIndex += GetTransferBlockCount();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStreamPartitioner::GetTransferSize()
{
    return m_uiTransferSize;
}
//---------------------------------------------------------------------------
inline NiInt32 NiSPStreamPartitioner::GetTransferBlockCount()
{
    return NiMin(
        (int)m_uiTotalBlockCount - m_uiCurrentBlockIndex, 
        (int)m_uiOptimalBlockCount);
}
//---------------------------------------------------------------------------
