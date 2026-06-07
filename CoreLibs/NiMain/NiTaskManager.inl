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
//  NiTaskManager inline functions

//---------------------------------------------------------------------------
inline NiTaskManager::ManagerStatus NiTaskManager::Stop()
{
    m_kManagerLock.Lock();
    m_eStatus = STOPPED;
    m_kManagerLock.Unlock();
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline NiTaskManager::ManagerStatus NiTaskManager::GetStatus()
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline int NiTaskManager::GetTotalNumTasks()
{
    int iNumTasks = 0;
    for (unsigned int ui = 0; ui < NUM_PRIORITIES; ui++)
        iNumTasks += m_akTaskQueues[ui].GetSize();
    return iNumTasks;
}
//---------------------------------------------------------------------------
inline int NiTaskManager::GetNumTasks(TaskPriority ePriority)
{
    return m_akTaskQueues[ePriority].GetSize();
}
//---------------------------------------------------------------------------
