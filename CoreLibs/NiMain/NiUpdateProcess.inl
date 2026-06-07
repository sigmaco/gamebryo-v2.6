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
//  NiUpdateProcess inline functions

//---------------------------------------------------------------------------
inline NiUpdateProcess::NiUpdateProcess() :
    m_pkWorkflowManager(NULL),
    m_fTime(0.0f),
    m_bUpdateControllers(true),
    m_bSubmitModifiers(true)
{
}
//---------------------------------------------------------------------------
inline float NiUpdateProcess::GetTime() const
{
    return m_fTime;
}
//---------------------------------------------------------------------------
inline void NiUpdateProcess::SetTime(float fTime)
{
    m_fTime = fTime;
}
//---------------------------------------------------------------------------
inline bool NiUpdateProcess::GetUpdateControllers() const
{
    return m_bUpdateControllers;
}
//---------------------------------------------------------------------------
inline void NiUpdateProcess::SetUpdateControllers(bool bUpdateControllers)
{
    m_bUpdateControllers = bUpdateControllers;
}
//---------------------------------------------------------------------------
inline bool NiUpdateProcess::GetSubmitModifiers() const
{
    return m_bSubmitModifiers;
}
//---------------------------------------------------------------------------
inline void NiUpdateProcess::SetSubmitModifiers(bool bSubmitModifiers)
{
    m_bSubmitModifiers = bSubmitModifiers;
}
//---------------------------------------------------------------------------
inline NiSPWorkflowManager* NiUpdateProcess::GetWorkflowManager() const
{
    return m_pkWorkflowManager;
}
//---------------------------------------------------------------------------
inline void NiUpdateProcess::SetWorkflowManager(NiSPWorkflowManager* pkManager)
{
    m_pkWorkflowManager = pkManager;
}
//---------------------------------------------------------------------------
// NiUpdateSyncArgs
//---------------------------------------------------------------------------
inline NiUpdateSyncArgs::NiUpdateSyncArgs(NiUpdateProcess& kUpdateProcess)
    : m_kUpdateProcess(kUpdateProcess)
{
}
//---------------------------------------------------------------------------
