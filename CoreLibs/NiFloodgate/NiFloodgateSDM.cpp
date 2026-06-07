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

#include "NiFloodgatePCH.h"
#include "NiFloodgateSDM.h"
#include "NiStreamProcessor.h"
#include "NiSPWorkflowManager.h"

//---------------------------------------------------------------------------
NiImplementSDMConstructor(NiFloodgate, "NiMain");
//---------------------------------------------------------------------------
#ifdef NIFLOODGATE_EXPORT
NiImplementDllMain(NiFloodgate);
#endif
//---------------------------------------------------------------------------
void NiFloodgateSDM::Init()
{
    NiImplementSDMInitCheck();

    NiStreamProcessor::InitializePools();
    
    NiUInt32 uiMaxQueueSize = 512;

    // Create the Stream Processor
    NiStreamProcessor::Create(uiMaxQueueSize);

#if !defined(_PS3) && !defined(_WII)
    // Configure Floodgate
    const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
    
    NiStreamProcessor::SetDefineWorkerThreadCountFunc(
         (NiStreamProcessor::DefineWorkerThreadCount)
        ((NiInitOptions*)pkInitOptions)->
        GetFloodgateDefineWorkerThreadCountFunc());

    NiStreamProcessor::SetDispatchThreadAffinityFunc(
        (NiStreamProcessor::AssignDispatcherThreadAffinity)
        ((NiInitOptions*)pkInitOptions)->GetDispatchThreadAffinityFunc());

    NiSPThreadPool::SetWorkerThreadAffinityFunc(
        (NiSPThreadPool::AssignWorkerThreadAffinity)
        ((NiInitOptions*)pkInitOptions)->GetWorkerThreadAffinityFunc());
#endif

    // Initialize the Stream Processor
    NiStreamProcessor::Initialize();
}
//---------------------------------------------------------------------------
void NiFloodgateSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiStreamProcessor::ShutdownPools();
    NiStreamProcessor::Shutdown();
    NiStreamProcessor::Destroy();
}
//---------------------------------------------------------------------------
