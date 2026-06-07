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
#include "NiSPTask.h"
#include "NiSPStream.h"
#include "NiSPJob.h"
#include "NiSPKernel.h"
#include "NiNopKernel.h"
//---------------------------------------------------------------------------
const NiUInt32 NiSPTaskImpl::MAX_INPUT_COUNT = 0xFFFFFFFF;
const NiUInt32 NiSPTaskImpl::MAX_FIXED_INPUT_SIZE = 0xFFFFFFFF;
NiTObjectPool<NiSPStreamPartitioner>* 
    NiSPTaskImpl::ms_pkStreamPartPool = NULL;
NiCriticalSection NiSPTaskImpl::ms_kStreamPartPoolLock;
NiSPKernel* NiSPTaskImpl::ms_pkSignalKernel = NULL;
//---------------------------------------------------------------------------
void NiSPTaskImpl::InitializePools(NiUInt32 uiPartitionPoolSize)
{
    NIASSERT(ms_pkStreamPartPool == NULL);
    ms_kStreamPartPoolLock.Lock();
    ms_pkStreamPartPool = NiNew 
        NiTObjectPool<NiSPStreamPartitioner>(uiPartitionPoolSize);
    ms_kStreamPartPoolLock.Unlock();

    // Initialize JobList Pool
    NiSPJob::InitializePools(128);
    
    // Initialize Workload pool list
    NiSPJobList::InitializePools(1024);

    // Create signal kernel
    ms_pkSignalKernel = NiNew NiNopKernel;
}
//---------------------------------------------------------------------------
void NiSPTaskImpl::ShutdownPools()
{
    NiSPJobList::ShutdownPools();
    NiSPJob::ShutdownPools();

    ms_kStreamPartPoolLock.Lock();
    NIASSERT(ms_pkStreamPartPool != NULL);
    NiDelete ms_pkStreamPartPool;
    ms_pkStreamPartPool = NULL;
    ms_kStreamPartPoolLock.Unlock();

    NiDelete ms_pkSignalKernel;
    ms_pkSignalKernel = NULL;
}
//---------------------------------------------------------------------------
NiSPTaskImpl::NiSPTaskImpl() 
    : m_pkPartitioner(NULL)
{
}
//---------------------------------------------------------------------------
NiSPTaskImpl::~NiSPTaskImpl()
{
}
//---------------------------------------------------------------------------
void NiSPTaskImpl::Initialize(NiSPTask* pkTask)
{
    // The partitioner is not locked here because this function is only
    // ever called from within the owning tasks's Prepare function, which is
    // itself locked from multi-threaded access.
    
    // You hit this assert if the task is being initialized twice without
    // being cleared.
    NIASSERT(!m_pkPartitioner);

    m_pkTask = pkTask;
    if (!m_pkTask->IsSync())
    {
        // Initialize the stream partitioner with this task
        ms_kStreamPartPoolLock.Lock();
        m_pkPartitioner = ms_pkStreamPartPool->GetFreeObject();
        ms_kStreamPartPoolLock.Unlock();
        m_pkPartitioner->Initialize(m_pkTask);
    }
    else
    {
        // Initialize the stream partitioner to NULL
        m_pkPartitioner = NULL;
    }

    // Initialize the job
    NiSPJob::Initialize(m_pkTask);
}
//---------------------------------------------------------------------------
void NiSPTaskImpl::Clear()
{
    // This lock also protects this object's access to its partitioner.
    ms_kStreamPartPoolLock.Lock();

    NiSPJob::Clear();

    if (!m_pkPartitioner)
    {
        ms_kStreamPartPoolLock.Unlock();
        return;
    }
    
    ms_pkStreamPartPool->ReleaseObject(m_pkPartitioner);
    m_pkPartitioner = NULL;

    ms_kStreamPartPoolLock.Unlock();
}
//---------------------------------------------------------------------------
