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
inline NiUInt32 NiSPJobList::GetJobCount()
{
    return m_uiJobCount;
}
//---------------------------------------------------------------------------
inline NiSPWorkload* NiSPJobList::GetWorkload(NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kWorkloads.GetSize());
    return m_kWorkloads[uiIndex];
}
//---------------------------------------------------------------------------
inline void NiSPJobList::SetIsFinished(bool bIsFinished)
{
    m_bIsFinished = bIsFinished;
}
//---------------------------------------------------------------------------
inline bool NiSPJobList::IsFinished() const
{
    return m_bIsFinished;
}
//---------------------------------------------------------------------------
inline void NiSPJobList::SetWorkflow(NiSPWorkflow* pWorkflow)
{
    m_pkWorkflow = pWorkflow;
}
//---------------------------------------------------------------------------
inline NiSPWorkflow* NiSPJobList::GetWorkflow()
{
    return m_pkWorkflow;
}
//---------------------------------------------------------------------------
inline void NiSPJobList::SetWorkflowID(NiUInt32 uiWorkflowID)
{
    m_uiWorkflowId = uiWorkflowID;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPJobList::GetWorkflowID()
{
    return m_uiWorkflowId;
}
//---------------------------------------------------------------------------
inline void NiSPJobList::SetStage(NiUInt32 uiStage)
{
    m_uiStage = uiStage;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPJobList::GetStage()
{
    return m_uiStage;
}
//---------------------------------------------------------------------------