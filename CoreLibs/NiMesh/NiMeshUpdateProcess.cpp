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

// Precompiled Header
#include "NiMeshPCH.h"

#include <NiAVObject.h>
#include <NiRenderer.h>

#include "NiMesh.h"
#include "NiMeshUpdateProcess.h"

NiImplementRTTI(NiMeshUpdateProcess, NiUpdateProcess);

//---------------------------------------------------------------------------
NiMeshUpdateProcess::~NiMeshUpdateProcess()
{
    if (m_bOwnsManager)
    {
        NiDelete m_pkWorkflowManager;
    }
}
//---------------------------------------------------------------------------
void NiMeshUpdateProcess::PreUpdate(NiAVObject*)
{
}
//---------------------------------------------------------------------------
void NiMeshUpdateProcess::PostUpdate(NiAVObject*)
{
    m_pkWorkflowManager->FlushTaskGroup(NiSyncArgs::SYNC_ANY,
        NiSyncArgs::SYNC_ANY);
}
//---------------------------------------------------------------------------
