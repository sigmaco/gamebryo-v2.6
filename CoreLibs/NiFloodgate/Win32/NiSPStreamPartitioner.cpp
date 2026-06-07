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
#include "NiSPStreamPartitioner.h"
#include "NiSPJobList.h"
#include "NiSPTask.h"
#include "NiSPWorkload.h"
#include "NiSPStream.h"
#include "NiStreamProcessor.h"
//---------------------------------------------------------------------------
NiSPStreamPartitioner::NiSPStreamPartitioner() :
    m_uiOptimalBlockCount(0x7FFFFFFF), 
    m_uiCurrentBlockIndex(0), 
    m_uiTotalBlockCount(0),
    m_uiTransferSize(0),
    m_pkTask(NULL)
{
}
//---------------------------------------------------------------------------
NiUInt32 NiSPStreamPartitioner::ComputeTotalBlockCount(NiSPTask* pkTask)
{
    NiUInt32 uiTotalBlockCount = 0;

    // Iterate through the input streams to determine the total block count.  
    // Also verify that each non-fixed input stream has the same block count. 
    NiUInt32 uiStreamCount = pkTask->GetInputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiStreamCount; ++uiIndex)
    {
        NiSPStream* pkStream = pkTask->GetInputAt(uiIndex);
        if (!pkStream->IsFixedInput())
        {
            if (uiTotalBlockCount == 0)
            {
                uiTotalBlockCount = pkStream->GetBlockCount();
                NIASSERT((uiTotalBlockCount || !pkStream->GetData()) &&
                    "Input stream has 0 block count!");
            }
            else
            {
                NIASSERT((uiTotalBlockCount == pkStream->GetBlockCount() ||
                    !pkStream->GetData()) && "Non-empty input and output "
                    "streams must have equal block counts!");
            }
        }
    }

    // Verify the block count on the output streams as well.
    uiStreamCount = pkTask->GetOutputCount();
    NIASSERT(uiStreamCount > 0 &&
        "Task must have at least one output stream.");
    for (NiUInt32 uiIndex = 0; uiIndex < uiStreamCount; ++uiIndex)
    {
        NiSPStream* pkStream = pkTask->GetOutputAt(uiIndex);

        // You hit this assert if a stream is declared as fixed yet is output.
        NIASSERT(!pkStream->IsFixedInput() && "Fixed output stream");

        if (uiTotalBlockCount == 0)
        {
            uiTotalBlockCount = pkStream->GetBlockCount();
            NIASSERT((uiTotalBlockCount || !pkStream->GetData()) &&
                "Output stream has 0 block count!");
        }
        else
        {
            NIASSERT((uiTotalBlockCount == pkStream->GetBlockCount() ||
                !pkStream->GetData()) && "Non-empty input and output "
                "streams must have equal block counts!");
        }
    }

    return uiTotalBlockCount;
}
//---------------------------------------------------------------------------
void NiSPStreamPartitioner::Initialize(NiSPTask* pkTask)
{
    NIASSERT(pkTask != NULL);
    m_pkTask = pkTask;
    m_uiOptimalBlockCount = 0; 
    m_uiCurrentBlockIndex = 0; 
    m_uiTotalBlockCount = 0;
    m_uiTransferSize = 0;

    m_uiTotalBlockCount = 
        NiSPStreamPartitioner::ComputeTotalBlockCount(pkTask);

    m_uiOptimalBlockCount = pkTask->GetOptimalBlockCount();
    if (m_uiOptimalBlockCount == NiSPTask::AUTO_BLOCK_COUNT)
    {
        EstimateOptimalBlockCount();
    }
}
//---------------------------------------------------------------------------
void NiSPStreamPartitioner::Partition(NiSPJobList* pkJob, 
    NiUInt32 uiJobIndex, NiUInt32& uiInputSize, NiUInt32& uiOutputSize)
{
    uiInputSize = 0;
    uiOutputSize = 0;

    // Initialize workload with task data
    NiSPWorkload* pkWorkload = pkJob->GetWorkload(uiJobIndex);

    NIASSERT(pkWorkload->m_uiInputCount == m_pkTask->GetInputCount());
    NIASSERT(pkWorkload->m_uiOutputCount == m_pkTask->GetOutputCount());

    pkWorkload->m_uiBlockCount = GetTransferBlockCount();
    pkWorkload->m_uiRangeStart = GetBlockIndex();

    // Partition input streams
    NiUInt32 uiInputCount = m_pkTask->GetInputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputCount; ++uiIndex)
    {
        NiSPStream* pkStream = m_pkTask->GetInputAt(uiIndex);
        if (pkStream->GetData())
        {
            uiInputSize += PartitionInput(pkJob, uiJobIndex, pkStream, 
                pkWorkload->m_pkInputsOutputs[uiIndex]);
        }
        else
        {
            NiSPWorkload::Header& kWorkloadHeader =
                pkWorkload->m_pkInputsOutputs[uiIndex];
            kWorkloadHeader.m_uiBlockCount = 0;
            kWorkloadHeader.m_pvBaseAddress = 0;
            kWorkloadHeader.m_uiDataOffset = 0;
            kWorkloadHeader.m_uiStride = 0;
        }
    }

    // Partition output streams
    NiUInt32 uiOutputCount = m_pkTask->GetOutputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputCount; ++uiIndex)
    {
        NiSPStream* pkStream = m_pkTask->GetOutputAt(uiIndex);
        if (pkStream->GetData())
        {
            uiOutputSize += PartitionOutput(pkJob, uiJobIndex, pkStream, 
                pkWorkload->m_pkInputsOutputs[uiIndex + uiInputCount]);
        }
        else
        {
            NiSPWorkload::Header& kWorkloadHeader =
                pkWorkload->m_pkInputsOutputs[uiIndex + uiInputCount];
            kWorkloadHeader.m_uiBlockCount = 0;
            kWorkloadHeader.m_pvBaseAddress = 0;
            kWorkloadHeader.m_uiDataOffset = 0;
            kWorkloadHeader.m_uiStride = 0;
        }
    }

    // Move to next set of blocks
    AdvanceBlockIndex();
}
//---------------------------------------------------------------------------
NiUInt32 NiSPStreamPartitioner::PartitionInput(NiSPJobList*, 
    NiUInt32, NiSPStream* pkStream, 
    NiSPWorkload::Header& kWorkloadHeader)
{
    // If partitioning enabled then partition the stream.
    // Otherwise, send the entire stream contents
    NiUInt32 uiBytes = 0;
    if (pkStream->IsFixedInput())
    {
        kWorkloadHeader.m_uiBlockCount = pkStream->GetBlockCount();
        kWorkloadHeader.m_pvBaseAddress = 
            ((NiUInt8*)pkStream->GetData() + pkStream->GetElementOffset());
        kWorkloadHeader.m_uiDataOffset = 0;
        kWorkloadHeader.m_uiStride = pkStream->GetStride();
        uiBytes = pkStream->GetDataSize();
    }
    else
    {
        kWorkloadHeader.m_uiBlockCount = GetTransferBlockCount();
        kWorkloadHeader.m_pvBaseAddress = 
            ((NiUInt8*)pkStream->GetData() + pkStream->GetElementOffset());
        kWorkloadHeader.m_uiDataOffset = 
            pkStream->GetDataOffsetForBlockIndex(m_uiCurrentBlockIndex);
        kWorkloadHeader.m_uiStride = pkStream->GetStride();
        uiBytes = kWorkloadHeader.m_uiStride * kWorkloadHeader.m_uiBlockCount;
    }
    return uiBytes;
}
//---------------------------------------------------------------------------
NiUInt32 NiSPStreamPartitioner::PartitionOutput(NiSPJobList*, 
   NiUInt32, NiSPStream* pkStream, 
   NiSPWorkload::Header& kWorkloadHeader)
{
    kWorkloadHeader.m_pvBaseAddress = 
        ((NiUInt8*)pkStream->GetData() + pkStream->GetElementOffset());
    kWorkloadHeader.m_uiDataOffset = 
        pkStream->GetDataOffsetForBlockIndex(m_uiCurrentBlockIndex);
    kWorkloadHeader.m_uiBlockCount = GetTransferBlockCount();
    kWorkloadHeader.m_uiStride = pkStream->GetStride();
    return kWorkloadHeader.m_uiStride * kWorkloadHeader.m_uiBlockCount;
}
//---------------------------------------------------------------------------
void NiSPStreamPartitioner::EstimateOptimalBlockCount()
{
    // On Win32, we have no reason to use anything other than the maximum
    // block count. This might change in future if pre-fetching is introduced
    // or other optimizations are implemented.
    // Use int max because at some point we convert this to an int to take the
    // min with something else.
    m_uiOptimalBlockCount = (NiUInt32)0x7FFFFFFF;
}
//---------------------------------------------------------------------------
