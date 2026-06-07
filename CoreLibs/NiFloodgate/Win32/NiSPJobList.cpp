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

#include "NiSPJobList.h"
#include "NiSPTask.h"

NiUInt32 NiSPJobList::MAX_JOB_COUNT = 65536;

//---------------------------------------------------------------------------
NiTObjectPool<NiSPWorkload>* NiSPJobList::ms_pkWorkloadPool = NULL;
NiCriticalSection NiSPJobList::ms_kWorkloadPoolLock;
//---------------------------------------------------------------------------
NiSPJobList::NiSPJobList(NiUInt32 uiJobCountEstimate) 
    : m_kWorkloads(uiJobCountEstimate)
    , m_bIsFinished(false)
    , m_pkWorkflow(NULL)
    , m_uiWorkflowId(0)
    , m_uiStage(0)
{
}
//---------------------------------------------------------------------------
NiSPJobList::~NiSPJobList()
{
}
//---------------------------------------------------------------------------
void NiSPJobList::InitializePools(NiUInt32 uiWorkloadPoolSize)
{
    ms_kWorkloadPoolLock.Lock();
    NIASSERT(ms_pkWorkloadPool == NULL);
    ms_pkWorkloadPool = NiNew NiTObjectPool<NiSPWorkload>(uiWorkloadPoolSize);
    ms_kWorkloadPoolLock.Unlock();
}
//---------------------------------------------------------------------------
void NiSPJobList::ShutdownPools()
{
    ms_kWorkloadPoolLock.Lock();
    NIASSERT(ms_pkWorkloadPool != NULL);
    NiDelete ms_pkWorkloadPool;
    ms_pkWorkloadPool = NULL;
    ms_kWorkloadPoolLock.Unlock();
}
//---------------------------------------------------------------------------
bool NiSPJobList::Initialize(NiSPTask* pkTask)
{
    m_bIsFinished = false;

    NiSPStreamPartitioner* pkPartitioner = 
        pkTask->GetImpl().GetStreamPartitioner();
     
    ms_kWorkloadPoolLock.Lock();
    m_uiJobCount = 0;
    if (pkTask->IsSync())
    {
        // Create a single streamless job
        Build(m_uiJobCount++, pkTask);
    }
    else
    {
        // Create a job for each group of blocks
        while (pkPartitioner->HasMoreBlocks() && m_uiJobCount < MAX_JOB_COUNT)
        {
            Build(m_uiJobCount++, pkTask);
        }
    }
    NiUInt32 uiTotalWorkloadCount = m_kWorkloads.GetSize();
    while (uiTotalWorkloadCount > m_uiJobCount)
    {
        --uiTotalWorkloadCount;
        ms_pkWorkloadPool->ReleaseObject(m_kWorkloads.RemoveEnd());
    }
    ms_kWorkloadPoolLock.Unlock();
    
    if (pkTask->IsCompacted())
        m_kWorkloads.Compact();

    if (pkTask->IsSync() && m_uiJobCount != 1)
    {
        NIDEBUGBREAK();
    }

    // If we have a partitioner then return true only if there is more 
    // data that needs to be scheduled for execution in an additional list
    if (pkPartitioner)
        return pkPartitioner->HasMoreBlocks();
    
    // All data has been processed, so no more iterations required
    return false;
}
//---------------------------------------------------------------------------
void NiSPJobList::Build(NiUInt32 uiJobIndex, NiSPTask* pkTask)
{
    // If there is a partitioner, use it to partition the data
    NiUInt32 uiInputSize = 0;
    NiUInt32 uiOutputSize = 0;
    
    // Get a workload
    NiSPWorkload* pkWorkload = 0;
    if (uiJobIndex < m_kWorkloads.GetSize())
    {
        pkWorkload = m_kWorkloads.GetAt(uiJobIndex);
    }
    else
    {
        pkWorkload = ms_pkWorkloadPool->GetFreeObject();
        m_kWorkloads.SetAtGrow(uiJobIndex, pkWorkload);
    }
    NIASSERT(pkWorkload);

    // Make certain the workload has the required number of inputs and outputs.
    // This also clears the workload for sync tasks.
    pkWorkload->Reset(pkTask->GetInputCount(), pkTask->GetOutputCount(),
        pkTask->IsCompacted());
    
    if (!pkTask->IsSync())
    {
        NiSPStreamPartitioner* pkPartitioner = 
            pkTask->GetImpl().GetStreamPartitioner();
        pkPartitioner->Partition(this, uiJobIndex, uiInputSize, uiOutputSize);
    }

    // Set kernel
    NiSPKernel *pkKernel = pkTask->GetKernel();
    NIASSERT(pkKernel && "Invalid Kernel");
    pkWorkload->SetKernel(pkKernel);
}
//---------------------------------------------------------------------------
