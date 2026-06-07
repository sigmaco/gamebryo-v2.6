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
inline int NiNavManager::GetControllerIdx(
    NiNavBaseController* pkNavController)
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kControllers.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (m_kControllers.GetAt(uiIdx) == pkNavController)
            return uiIdx;
    }

    return -1;
}
//---------------------------------------------------------------------------
inline NiNavManager* NiNavManager::GetNavManager()
{
    return m_pkNavManager;
}
//---------------------------------------------------------------------------
inline void NiNavManager::Shutdown()
{
    NiDelete m_pkNavManager;
    m_pkNavManager = NULL;
}
//---------------------------------------------------------------------------
inline void NiNavManager::Update(float fCurTime)
{
    m_fThisFrameDelta = fCurTime - m_fLastFrameTime;
    m_fLastFrameTime = fCurTime;

    if (m_kControllers.GetSize() > 0)
    {
        m_kControllers.GetAt(m_uiCurController)->SampleAndUpdate();
    }
}
//---------------------------------------------------------------------------
inline float NiNavManager::GetFrameTime()
{
    return m_fThisFrameDelta;
}
//---------------------------------------------------------------------------
inline unsigned int NiNavManager::GetNumNavigationControllers() const
{
    return m_kControllers.GetSize();
}
//---------------------------------------------------------------------------
