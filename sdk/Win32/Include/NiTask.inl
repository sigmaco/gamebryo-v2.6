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
//  NiTask inline functions

//---------------------------------------------------------------------------
inline NiTask::NiTask() : m_eStatus(IDLE)
{
}
//---------------------------------------------------------------------------
inline NiTask::~NiTask()
{
}
//---------------------------------------------------------------------------
inline bool NiTask::CanExecute()
{
    return true;
}
//---------------------------------------------------------------------------
inline void NiTask::SetStatus(TaskStatus eStatus)
{
    m_eStatus = eStatus;
}
//---------------------------------------------------------------------------
inline NiTask::TaskStatus NiTask::GetStatus()
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline bool NiTask::Clear()
{
    if (m_eStatus == ABORTED || m_eStatus == COMPLETED)
        return true;
    return false;
}
//---------------------------------------------------------------------------
