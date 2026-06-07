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
//  NiMeshUpdateProcess inline functions

//---------------------------------------------------------------------------
inline NiMeshUpdateProcess::NiMeshUpdateProcess(
    NiSPWorkflowManager* pkWorkflowManager)
    : NiUpdateProcess(),
    m_bOwnsManager(false)
{
    if (pkWorkflowManager == NULL)
    {
        pkWorkflowManager =
            NiNew NiSPWorkflowManager(64, NiStreamProcessor::MEDIUM, 3);
        m_bOwnsManager = true;
        NIASSERT(pkWorkflowManager);
    }

    SetWorkflowManager(pkWorkflowManager);
}
//---------------------------------------------------------------------------
inline NiSPWorkflowManager* NiMeshUpdateProcess::GetWorkflowManager() const
{
    return m_pkWorkflowManager;
}
//---------------------------------------------------------------------------
