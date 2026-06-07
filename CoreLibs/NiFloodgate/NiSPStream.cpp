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
#include "NiFloodgatePCH.h"
#include "NiSPStream.h"
#include "NiSPTask.h"
#include "NiSPAlgorithms.h"

//---------------------------------------------------------------------------
NiSPStream::NiSPStream(void* pvData, NiUInt32 uiStride, NiUInt32 uiBlockCount, 
    bool bIsFixedInput) : 
    m_bIsLocked(false),
    m_pvData(pvData),
    m_uiDataSize(0),
    m_pkDataStreamSource(NULL),
    m_uiRegionIdx(0),
    m_uiElementOffset(0), 
    m_uiBlockCount(uiBlockCount)
{
    m_uiStride = (NiUInt16)uiStride;
    m_uFlags = (NiUInt8)(bIsFixedInput ? FLAG_ISFIXEDINPUT : 0);
}
//---------------------------------------------------------------------------
NiSPStream::~NiSPStream()
{
    RemoveFromDependentTasks();
}
//---------------------------------------------------------------------------
void NiSPStream::Prepare()
{
    NIASSERT(m_uiStride > 0 && "Error: Stride was zero. Stride must be > 0");
    if (m_uiStride <= 0)
    {
        NILOG("Error: Stride was zero. Stride must be > 0");
        return;
    }

    // Compute size of buffer
    m_uiDataSize = m_uiBlockCount * m_uiStride;
}
//---------------------------------------------------------------------------
void* NiSPStream::GetDataForBlockIndex(NiUInt32 uiBlockIndex)
{
    if (uiBlockIndex >= m_uiBlockCount)
        return 0;

    if (GetData() == NULL)
        return NULL;

    return ((unsigned char*)(GetData())) + uiBlockIndex * m_uiStride;
}
//---------------------------------------------------------------------------
void* NiSPStream::GetAlignedDataForBlockIndex(NiUInt32 uiBlockIndex)
{
    if (uiBlockIndex >= m_uiBlockCount)
        return 0;

    if (GetData() == NULL)
        return NULL;

    return reinterpret_cast<void*>(NiSPAlgorithms::PreAlign128(
        (unsigned char*)(GetData()) + uiBlockIndex * m_uiStride));
}
//---------------------------------------------------------------------------
NiUInt32 NiSPStream::GetDataOffsetForBlockIndex(NiUInt32 uiBlockIndex)
{
    if (uiBlockIndex >= m_uiBlockCount)
        return 0;

    if (GetData() == NULL)
        return 0;

    return uiBlockIndex * m_uiStride;
}
//---------------------------------------------------------------------------
void NiSPStream::NotifyDependentTasks()
{
    NiUInt32 uiInputCount = GetInputSize();
    for (NiUInt32 i = 0; i < uiInputCount; i++)
    {
        NiSPTask* pkTask = GetInputAt(i);
        if (pkTask)
            pkTask->Reset(true);
    }
    NiUInt32 uiOutputCount = GetOutputSize();
    for (NiUInt32 i = 0; i < uiOutputCount; i++)
    {
        NiSPTask* pkTask = GetOutputAt(i);
        if (pkTask)
            pkTask->Reset(true);
    }
}
//---------------------------------------------------------------------------
void NiSPStream::RemoveFromDependentTasks()
{
    NiUInt32 uiInputCount = GetInputSize();
    for (NiUInt32 i = 0; i < uiInputCount; i++)
    {
        NiSPTask* pkTask = GetInputAt(i);
        if (pkTask)
        {
            pkTask->RemoveInput(this);
        }
    }

    NiUInt32 uiOutputCount = GetOutputSize();
    for (NiUInt32 i = 0; i < uiOutputCount; i++)
    {
        NiSPTask* pkTask = GetOutputAt(i);
        if (pkTask)
        {
            pkTask->RemoveOutput(this);
        }
    }
}
//---------------------------------------------------------------------------

