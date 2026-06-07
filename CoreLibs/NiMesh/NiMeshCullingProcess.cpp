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

#include "NiMeshPCH.h"

#include <NiSPWorkflowManager.h>

#include "NiMeshCullingProcess.h"

NiImplementRTTI(NiMeshCullingProcess, NiCullingProcess);

//---------------------------------------------------------------------------
// Constructors and Destructors
//---------------------------------------------------------------------------
NiMeshCullingProcess::NiMeshCullingProcess(NiVisibleArray* pkVisibleSet,
    NiSPWorkflowManager* pkWorkflowManager) :
    NiCullingProcess(pkVisibleSet, pkWorkflowManager),
    m_bOwnsManager(false)
{
    if (pkWorkflowManager == NULL)
    {
        m_bOwnsManager = true;
        m_pkWorkflowManager =
            NiNew NiSPWorkflowManager(64, NiStreamProcessor::MEDIUM, 3);
    }
}
//---------------------------------------------------------------------------
NiMeshCullingProcess::NiMeshCullingProcess(NiVisibleArray* pkVisibleSet, 
    NiSPWorkflowManager* pkWorkflowManager, bool bUseVirtualAppend) :
    NiCullingProcess(pkVisibleSet, pkWorkflowManager, bUseVirtualAppend),
    m_bOwnsManager(false)
{
    if (pkWorkflowManager == NULL)
    {
        m_bOwnsManager = true;
        m_pkWorkflowManager =
            NiNew NiSPWorkflowManager(64, NiStreamProcessor::MEDIUM, 3);
    }
}
//---------------------------------------------------------------------------
NiMeshCullingProcess::~NiMeshCullingProcess()
{
    if (m_bOwnsManager)
    {
        NiDelete m_pkWorkflowManager;
        m_pkWorkflowManager = NULL;
    }
}
//---------------------------------------------------------------------------
NiSPWorkflowManager* NiMeshCullingProcess::GetWorkflowManager() const
{
    return m_pkWorkflowManager;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Methods called by Cull
//---------------------------------------------------------------------------
void NiMeshCullingProcess::Process(const NiCamera* pkCamera, 
    NiAVObject* pkScene, NiVisibleArray* pkVisibleSet)
{
    // Call back to the default implementation in NiCullingProcess. Note that
    // we must do this call explicitly because that implementation is 
    // declared as pure virtual.
    NiCullingProcess::Process(pkCamera, pkScene, pkVisibleSet);
}
//---------------------------------------------------------------------------
void NiMeshCullingProcess::PreProcess()
{
    // Empty at this time. Provided as a future extension point.
}
//---------------------------------------------------------------------------
void NiMeshCullingProcess::PostProcess()
{
    m_pkWorkflowManager->FlushTaskGroup(NiSyncArgs::SYNC_VISIBLE,
        NiSyncArgs::SYNC_ANY);
}
//---------------------------------------------------------------------------
