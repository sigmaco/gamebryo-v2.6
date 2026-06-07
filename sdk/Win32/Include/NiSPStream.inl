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
inline void* NiSPStream::GetData()
{
    return m_pvData;
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetData(void* pvData)
{
    if (m_pvData != pvData)
    {
        m_pvData = pvData;
        NotifyDependentTasks();
    }
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetDataSource(NiSPDataStream* pkDataStreamSource, 
    bool bAutoSetBlockCount, NiUInt32 uiRegionIdx, NiUInt32 uiElementOffset)
{
    if (pkDataStreamSource)
    {
        m_uiStride = (NiUInt16)pkDataStreamSource->GetStride();
        m_uiBlockCount = pkDataStreamSource->GetBlockCount(uiRegionIdx);

        if (GetEffectiveInputSize() > 0)
        {
            pkDataStreamSource->EnableReadFlag();
        }
        if (GetEffectiveOutputSize() > 0)
        {
            pkDataStreamSource->EnableWriteFlag();
        }
    }

    m_pkDataStreamSource = pkDataStreamSource;
    SetBit(bAutoSetBlockCount, FLAG_AUTOSETBLOCKCOUNT);
    m_uiRegionIdx = uiRegionIdx;
    m_uiElementOffset = uiElementOffset;

    // This causes dependent tasks to reset themselves to cope with any changes
    // to the stream that this call may have caused.
    NotifyDependentTasks();
}
//---------------------------------------------------------------------------
inline NiSPDataStream* NiSPStream::GetDataSource() const
{
    return m_pkDataStreamSource;
}
//---------------------------------------------------------------------------
inline bool NiSPStream::GetAutoSetBlockCount() const
{
    return GetBit(FLAG_AUTOSETBLOCKCOUNT);;
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetAutoSetBlockCount(bool bAutoSetBlockCount)
{
    SetBit(bAutoSetBlockCount, FLAG_AUTOSETBLOCKCOUNT);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetRegionIdx() const
{
    return m_uiRegionIdx;
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetRegionIdx(NiUInt32 uiRegionIdx)
{
    m_uiRegionIdx = uiRegionIdx;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetElementOffset() const
{
    return m_uiElementOffset;
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetElementOffset(NiUInt32 uiElementOffset)
{
    m_uiElementOffset = uiElementOffset;
}
//---------------------------------------------------------------------------
inline bool NiSPStream::IsDataAligned()
{
    return ((NiUInt64)m_pvData & 127) == 0;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetDataSize()
{
    return m_uiDataSize;
}
//---------------------------------------------------------------------------
inline bool NiSPStream::IsFixedInput()
{
    return GetBit(FLAG_ISFIXEDINPUT);
}
//---------------------------------------------------------------------------
inline unsigned short NiSPStream::GetStride()
{
    return m_uiStride;
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetStride(unsigned short uiStride)
{
    if (m_uiStride != uiStride)
    {
        m_uiStride = uiStride;
        NotifyDependentTasks();
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetBlockCount()
{
    return m_uiBlockCount;
}
//---------------------------------------------------------------------------
inline void NiSPStream::SetBlockCount(NiUInt32 uiBlockCount)
{
    SetBit(false, FLAG_AUTOSETBLOCKCOUNT);
    Do_SetBlockCount(uiBlockCount);
}
//---------------------------------------------------------------------------
inline void NiSPStream::Do_SetBlockCount(NiUInt32 uiBlockCount)
{
    if (m_uiBlockCount != uiBlockCount)
    {
        m_uiBlockCount = uiBlockCount;
        NotifyDependentTasks();
    }
}
//---------------------------------------------------------------------------
inline void NiSPStream::AddInput(NiSPTask* pkTask)
{
    m_kInputs.Add(pkTask);
}
//---------------------------------------------------------------------------
inline void NiSPStream::RemoveInput(NiSPTask* pkTask)
{
    NiInt32 uiLocation = m_kInputs.Find(pkTask);
    if (uiLocation != -1)
    {
        m_kInputs.SetAt(uiLocation, NULL);
    }
}
//---------------------------------------------------------------------------
inline NiSPTask* NiSPStream::GetInputAt(NiUInt32 uiIndex) const
{
    return m_kInputs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetInputSize() const
{
    return m_kInputs.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetEffectiveInputSize() const
{
    return m_kInputs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline void NiSPStream::AddOutput(NiSPTask* pkTask)
{
    m_kOutputs.Add(pkTask);
}
//---------------------------------------------------------------------------
inline void NiSPStream::RemoveOutput(NiSPTask* pkTask)
{
    NiInt32 uiLocation = m_kOutputs.Find(pkTask);
    if (uiLocation != -1)
    {
        m_kOutputs.SetAt(uiLocation, NULL);
    }
}
//---------------------------------------------------------------------------
inline NiSPTask*  NiSPStream::GetOutputAt(NiUInt32 uiIndex) const
{
    return m_kOutputs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetOutputSize() const
{
    return m_kOutputs.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::GetEffectiveOutputSize() const
{
    return m_kOutputs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline void NiSPStream::ClearTaskArrays()
{
    // The tasks we are input/output for may not have been deleted yet, so
    // we need to remove ourself from the tasks' arrays.
    RemoveFromDependentTasks();
    
    // Now clear our arrays.
    m_kInputs.RemoveAll();
    m_kOutputs.RemoveAll();
}
//---------------------------------------------------------------------------
inline void NiSPStream::Lock()
{
    if (m_pkDataStreamSource && !m_bIsLocked)
    {
        void* pkData = (void*)m_pkDataStreamSource->LockSource(m_uiRegionIdx);
        NIASSERT(pkData);
        SetData(pkData);
        if (GetAutoSetBlockCount())
        {
            Do_SetBlockCount(m_pkDataStreamSource->GetBlockCount(
                m_uiRegionIdx));
        }
        m_bIsLocked = true;
    }    
}
//---------------------------------------------------------------------------
inline void NiSPStream::Unlock()
{
    if (m_pkDataStreamSource && m_bIsLocked)
    {
        m_pkDataStreamSource->UnlockSource();
        m_bIsLocked = false;
    }
}
//---------------------------------------------------------------------------
