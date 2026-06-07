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

#include "NiSPWorkflowManager.h"

//---------------------------------------------------------------------------
// NiSPWorkflowManager functions
//---------------------------------------------------------------------------
NiSPWorkflowManager::NiSPWorkflowManager(const NiUInt16 uiSubmitThreshold,
    const NiStreamProcessor::Priority eWorkflowPriority,
    const NiUInt32 uiNumTaskGroups)
    : m_kTaskGroupInfoMap(uiNumTaskGroups),
    m_eWorkflowPriority(eWorkflowPriority),
#if defined(_WII)
    // On the Wii, we want all tasks to be submitted at once by default.
    m_uiSubmitThreshold(1024)
#else
    m_uiSubmitThreshold(uiSubmitThreshold)
#endif
{
}
//---------------------------------------------------------------------------
NiSPWorkflowManager::~NiSPWorkflowManager()
{
    ClearTaskGroupInfoMap();
}
//---------------------------------------------------------------------------
void NiSPWorkflowManager::SetSubmitThreshold(const NiUInt16 uiSubmitThreshold)
{
    m_uiSubmitThreshold = uiSubmitThreshold;
    
    NiTMapIterator kIter = m_kTaskGroupInfoMap.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiTaskGroupID = 0;
        TaskGroupInfo* pkInfo = 0;
        m_kTaskGroupInfoMap.GetNext(kIter, uiTaskGroupID, pkInfo);
        pkInfo->m_uiSubmitThreshold = uiSubmitThreshold;
    }
}
//---------------------------------------------------------------------------
void NiSPWorkflowManager::SetWorkflowPriority(
    const NiStreamProcessor::Priority ePriority)
{
    m_eWorkflowPriority = ePriority;
}
//---------------------------------------------------------------------------
NiSPWorkflowPtr NiSPWorkflowManager::AddTask(NiSPTask* pkTask,
    const NiUInt32 uiTaskGroup, const bool bDelaySubmission)
{
    TaskGroupInfo* pkTaskGroupInfo = GetTaskGroup(uiTaskGroup);
    
    NiSPWorkflowPtr spResult = 0;
    if (bDelaySubmission)
    {
        spResult = pkTaskGroupInfo->AddRelatedDelayedTask(pkTask);
        pkTaskGroupInfo->FinishRelatedDelayedTasks();
    }
    else
    {
        spResult = pkTaskGroupInfo->AddRelatedImmediateTask(pkTask);
        pkTaskGroupInfo->FinishRelatedImmediateTasks();
    }
        
    return spResult;
}
//---------------------------------------------------------------------------
NiSPWorkflowPtr NiSPWorkflowManager::AddRelatedTask(NiSPTask* pkTask,
    const NiUInt32 uiTaskGroup, const bool bDelaySubmission)
{
    TaskGroupInfo* pkTaskGroupInfo = GetTaskGroup(uiTaskGroup);
    
    if (bDelaySubmission)
        return pkTaskGroupInfo->AddRelatedDelayedTask(pkTask);
    else
        return pkTaskGroupInfo->AddRelatedImmediateTask(pkTask);
}
//---------------------------------------------------------------------------
void NiSPWorkflowManager::FinishRelatedTasks(const NiUInt32 uiTaskGroup,
    const NiUInt32 uiTaskGroupMask)
{
    TaskGroupInfo* pkTaskGroupInfo = 0;
    if (uiTaskGroupMask == 0xFFFFFFFF &&
        m_kTaskGroupInfoMap.GetAt(uiTaskGroup, pkTaskGroupInfo))
    {
        pkTaskGroupInfo->FinishRelatedImmediateTasks();
        pkTaskGroupInfo->FinishRelatedDelayedTasks();
        return;
    }
    
    NiTMapIterator kIter = m_kTaskGroupInfoMap.GetFirstPos();
    NiUInt32 uiKey;
    NiUInt32 uiMaskedTask = uiTaskGroup & uiTaskGroupMask;
    while (kIter)
    {
        m_kTaskGroupInfoMap.GetNext(kIter, uiKey, pkTaskGroupInfo);
        if ((uiKey & uiTaskGroupMask) == uiMaskedTask)
        {
            pkTaskGroupInfo->FinishRelatedImmediateTasks();
            pkTaskGroupInfo->FinishRelatedDelayedTasks();
        }
    }
}
//---------------------------------------------------------------------------
bool NiSPWorkflowManager::FlushTaskGroup(const NiUInt32 uiTaskGroup,
    const NiUInt32 uiTaskGroupMask)
{
    TaskGroupInfo* pkTaskGroupInfo = 0;
    if (uiTaskGroupMask == 0xFFFFFFFF &&
        m_kTaskGroupInfoMap.GetAt(uiTaskGroup, pkTaskGroupInfo))
    {
        return pkTaskGroupInfo->Flush();
    }
    
    NiTMapIterator kIter = m_kTaskGroupInfoMap.GetFirstPos();
    NiUInt32 uiKey;
    NiUInt32 uiMaskedTask = uiTaskGroup & uiTaskGroupMask;
    bool bResult = false;
    while (kIter)
    {
        m_kTaskGroupInfoMap.GetNext(kIter, uiKey, pkTaskGroupInfo);
        if ((uiKey & uiTaskGroupMask) == uiMaskedTask)
        {
            bResult = pkTaskGroupInfo->Flush() || bResult; // Don't re-order
        }
    }
    
    return bResult;
}
//---------------------------------------------------------------------------
void NiSPWorkflowManager::ClearTaskGroupInfoMap()
{
    NiTMapIterator kIter = m_kTaskGroupInfoMap.GetFirstPos();
    NiUInt32 uiTaskID;
    TaskGroupInfo* pkTaskGroupInfo;
    while (kIter)
    {
        m_kTaskGroupInfoMap.GetNext(kIter, uiTaskID, pkTaskGroupInfo);
        NiDelete pkTaskGroupInfo;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiSPWorkflowManager::TaskGroupInfo functions
//---------------------------------------------------------------------------
NiSPWorkflowManager::TaskGroupInfo::TaskGroupInfo(
    const NiUInt16 uiSubmitThreshold,
    const NiStreamProcessor::Priority eWorkflowPriority)
    : m_spImmediateWorkflow(0),
    m_spDelayedWorkflow(0),
    m_uiNumTasksImmediate(0),
    m_uiNumTasksDelayed(0),
    m_kDelayedWorkflows(),
    m_eWorkflowPriority(eWorkflowPriority),
    m_uiSubmitThreshold(uiSubmitThreshold)
{
}
//---------------------------------------------------------------------------
NiSPWorkflowManager::TaskGroupInfo::~TaskGroupInfo()
{
}
//---------------------------------------------------------------------------
bool NiSPWorkflowManager::TaskGroupInfo::Flush()
{
    // Submit all the workflows.

    // If any of the calls to Submit fail, it is possible the size of the 
    // NiStreamProcessor max queue size should be increased, but more likely 
    // there is a pipelining flaw and the workflows are not being executed
    // quickly enough. NIVERIFY is used to confirm success because either 
    // of these are considered error conditions.

    if (m_spImmediateWorkflow)
    {
        NIVERIFY(NiStreamProcessor::Get()->Submit(m_spImmediateWorkflow, 
            m_eWorkflowPriority));
        m_spImmediateWorkflow = 0;
    }

    if (m_spDelayedWorkflow)
    {
        NIVERIFY(NiStreamProcessor::Get()->Submit(m_spDelayedWorkflow, 
            m_eWorkflowPriority));
        m_spDelayedWorkflow = 0;
    }

    NiUInt32 uiNumDelayed = m_kDelayedWorkflows.GetSize();
    while (uiNumDelayed > 0)
    {
        uiNumDelayed--;

        NIVERIFY(NiStreamProcessor::Get()->Submit(
            m_kDelayedWorkflows.GetAt(uiNumDelayed), 
            m_eWorkflowPriority));
        // Release reference to workflow and remove it from the set
        m_kDelayedWorkflows.ReplaceAt(uiNumDelayed, 0);
        m_kDelayedWorkflows.RemoveAt(uiNumDelayed);
    }

    return true;
}
//---------------------------------------------------------------------------
