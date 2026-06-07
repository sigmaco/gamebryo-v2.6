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

#include "NiSPJob.h"
#include "NiSPTask.h"

//---------------------------------------------------------------------------
NiTObjectPool<NiSPJobList>* NiSPJob::ms_pkJobListPool = NULL;
NiCriticalSection NiSPJob::ms_kJobListPoolLock;
//---------------------------------------------------------------------------
void NiSPJob::InitializePools(NiUInt32 uiJobListPoolSize)
{
    ms_kJobListPoolLock.Lock();
    NIASSERT(ms_pkJobListPool == NULL);
    ms_pkJobListPool = NiNew NiTObjectPool<NiSPJobList>(uiJobListPoolSize);
    ms_kJobListPoolLock.Unlock();
}
//---------------------------------------------------------------------------
void NiSPJob::ShutdownPools()
{
    ms_kJobListPoolLock.Lock();
    NIASSERT(ms_pkJobListPool != NULL);
    NiDelete ms_pkJobListPool;
    ms_pkJobListPool = NULL;
    ms_kJobListPoolLock.Unlock();
}
//---------------------------------------------------------------------------
NiSPJob::NiSPJob()
{
}
//---------------------------------------------------------------------------
NiSPJob::~NiSPJob()
{
}
//---------------------------------------------------------------------------
void NiSPJob::Initialize(NiSPTask* pkTask)
{
    ms_kJobListPoolLock.Lock();
    bool bHasMoreData = true;
    do
    {
        // Get a job list and add to the job
        NiSPJobList* pkJobList = ms_pkJobListPool->GetFreeObject();

        Add(pkJobList);

        // Process the list and see if we are done
        bHasMoreData = pkJobList->Initialize(pkTask);
    } while (bHasMoreData);
    ms_kJobListPoolLock.Unlock();
    
    if (pkTask->IsCompacted())
        Realloc();
}
//---------------------------------------------------------------------------
void NiSPJob::Clear()
{
    // Release all job lists back to the queue
    ms_kJobListPoolLock.Lock();

    NiUInt32 uiSize = GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiSize; uiIndex++)
    {
        ms_pkJobListPool->ReleaseObject(GetAt(uiIndex));
    }

    // Remove all job lists from this job
    RemoveAll();
    ms_kJobListPoolLock.Unlock();
}
//---------------------------------------------------------------------------
