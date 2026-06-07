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
//  NiParallelUpdateTaskManager inline functions

//---------------------------------------------------------------------------
inline NiParallelUpdateTaskManager* NiParallelUpdateTaskManager::Get()
{
    return ms_pkManager;
}
//---------------------------------------------------------------------------
inline bool NiParallelUpdateTaskManager::AddTask(NiTask* pkTask, 
    NiTaskManager::TaskPriority ePriority)
{
    return AddTask_Internal(false, pkTask, ePriority);
}
//---------------------------------------------------------------------------
inline int NiParallelUpdateTaskManager::SignalRenderSema()
{
    if (ms_pkManager)
        return ms_pkManager->m_kRenderSema.Signal();
    return 0;
}
//---------------------------------------------------------------------------
inline int NiParallelUpdateTaskManager::WaitRenderSema()
{
    if (ms_pkManager)
        return ms_pkManager->m_kRenderSema.Wait();
    return 0;
}
//---------------------------------------------------------------------------
inline int NiParallelUpdateTaskManager::SignalUpdateSema()
{
    return m_kUpdateSema.Signal();
}
//---------------------------------------------------------------------------
inline int NiParallelUpdateTaskManager::WaitUpdateSema()
{
    return m_kUpdateSema.Wait();
}
//---------------------------------------------------------------------------
inline const NiThread* NiParallelUpdateTaskManager::GetThread()
{
    return m_pkThread;
}
//---------------------------------------------------------------------------
inline bool NiParallelUpdateTaskManager::IsActive()
{
    return (ms_pkManager && ms_pkManager->m_bActive);
}
//---------------------------------------------------------------------------

