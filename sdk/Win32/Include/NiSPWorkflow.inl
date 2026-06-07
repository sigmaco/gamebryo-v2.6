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
inline NiUInt32 NiSPWorkflow::GetId() const
{
    return m_uiId;
}
//---------------------------------------------------------------------------
inline NiSPWorkflow::Status NiSPWorkflow::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::SetStatus(NiSPWorkflow::Status eStatus)
{
    m_eStatus = eStatus;
}
//---------------------------------------------------------------------------
inline NiInt32 NiSPWorkflow::GetCurrentStage() const
{
    return m_iCurrentStage;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::SetCurrentStage(NiInt32 iStage)
{
     m_iCurrentStage = iStage;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::AddPendingJobList(NiSPJobList* pkJobList)
{
    m_kPendingJobLists.Add(pkJobList);
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::RemovePendingJobList(NiSPJobList* pkJobList)
{
    int iFound = m_kPendingJobLists.Find(pkJobList);
    if (iFound > 0)
        m_kPendingJobLists.RemoveAt(iFound);
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::ClearPendingJobLists()
{
    m_kPendingJobLists.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPWorkflow::GetNumPendingJobLists() const
{
    return m_kPendingJobLists.GetSize();
}
//---------------------------------------------------------------------------
inline const NiSPJobList* NiSPWorkflow::GetPendingJobList(
    NiUInt32 uiIndex) const
{
    return m_kPendingJobLists.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::Execute()
{
#if defined(_WII)
    ExecuteSerial();
#else
    m_kWorkflowImpl.Execute(this);
#endif
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::SetCompletionHandler(CompletionHandler* pkHandler)
{
    m_pkCompletionHandler = pkHandler;
}
//---------------------------------------------------------------------------
inline bool NiSPWorkflow::IsAtomicUpdateComplete() const
{
    return m_uiAtomicUpdateFlag != 0;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::ResetAtomicUpdateComplete()
{
    m_uiAtomicUpdateFlag = 0;
}
//---------------------------------------------------------------------------
inline NiUInt32* NiSPWorkflow::GetAtomicUpdateCompleteAddress()
{
    return &m_uiAtomicUpdateFlag;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::IncRefCount()
{
    NiAtomicIncrement(m_uiRefCount);
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::DecRefCount()
{
    if (NiAtomicDecrement(m_uiRefCount) == 0)
        ReleaseWorkflow();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPWorkflow::GetSize() const
{
    return m_kTasks.GetSize();
}
//---------------------------------------------------------------------------
inline NiSPTask* NiSPWorkflow::GetAt(NiUInt32 uiTaskIdx) const
{
    return m_kTasks.GetAt(uiTaskIdx);
}
//---------------------------------------------------------------------------
