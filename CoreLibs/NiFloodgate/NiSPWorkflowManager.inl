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
inline NiUInt16 NiSPWorkflowManager::GetSubmitThreshold() const
{
    return m_uiSubmitThreshold;
}
//---------------------------------------------------------------------------
inline NiStreamProcessor::Priority NiSPWorkflowManager::GetWorkflowPriority()
    const
{
    return m_eWorkflowPriority;
}
//---------------------------------------------------------------------------
inline NiSPWorkflowManager::TaskGroupInfo*
NiSPWorkflowManager::GetTaskGroup(NiUInt32 uiTaskGroup)
{
    TaskGroupInfo* pkTaskGroupInfo = 0;
    if (!m_kTaskGroupInfoMap.GetAt(uiTaskGroup, pkTaskGroupInfo))
    {
        pkTaskGroupInfo =
            NiNew TaskGroupInfo(m_uiSubmitThreshold, m_eWorkflowPriority);
        m_kTaskGroupInfoMap.SetAt(uiTaskGroup, pkTaskGroupInfo);
    }
    
    return pkTaskGroupInfo;
}
//---------------------------------------------------------------------------
inline NiSPWorkflowPtr
NiSPWorkflowManager::TaskGroupInfo::AddRelatedImmediateTask(NiSPTask* pkTask)
{
    if (!m_spImmediateWorkflow)
    {
        m_spImmediateWorkflow = NiStreamProcessor::Get()->GetFreeWorkflow();
        m_uiNumTasksImmediate = 0;
    }
    
    m_spImmediateWorkflow->Add(pkTask);
    m_uiNumTasksImmediate++;
    
    return m_spImmediateWorkflow;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflowManager::TaskGroupInfo::FinishRelatedImmediateTasks()
{
    if (m_uiNumTasksImmediate >= m_uiSubmitThreshold)
    {
        NIVERIFY(NiStreamProcessor::Get()->Submit(m_spImmediateWorkflow, 
            m_eWorkflowPriority));
        m_spImmediateWorkflow = 0;
    }
}
//---------------------------------------------------------------------------
inline NiSPWorkflowPtr
NiSPWorkflowManager::TaskGroupInfo::AddRelatedDelayedTask(NiSPTask* pkTask)
{
    if (!m_spDelayedWorkflow)
    {
        m_spDelayedWorkflow = NiStreamProcessor::Get()->GetFreeWorkflow();
        m_uiNumTasksDelayed = 0;
    }
    
    m_spDelayedWorkflow->Add(pkTask);
    m_uiNumTasksDelayed++;
    
    return m_spDelayedWorkflow;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflowManager::TaskGroupInfo::FinishRelatedDelayedTasks()
{
    if (m_uiNumTasksDelayed >= m_uiSubmitThreshold)
    {
        m_kDelayedWorkflows.Add(m_spDelayedWorkflow);
        m_spDelayedWorkflow = 0;
    }
}
//---------------------------------------------------------------------------

