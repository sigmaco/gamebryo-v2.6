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
inline void NiRenderStep::_SDMInit()
{
    ms_kDefaultName = "Render Step";
}
//---------------------------------------------------------------------------
inline void NiRenderStep::_SDMShutdown()
{
    ms_kDefaultName = NULL;
}
//---------------------------------------------------------------------------
inline NiRenderStep::NiRenderStep() : m_pfnPreProcessingCallback(NULL),
    m_pfnPostProcessingCallback(NULL), m_pvPreProcessingCallbackData(NULL),
    m_pvPostProcessingCallbackData(NULL), m_kName(ms_kDefaultName),
    m_bActive(true)
{
}
//---------------------------------------------------------------------------
inline void NiRenderStep::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderStep::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiRenderStep::Render()
{
    // Pre-processing callback.
    if (m_pfnPreProcessingCallback)
    {
        if (!m_pfnPreProcessingCallback(this, m_pvPreProcessingCallbackData))
        {
            return;
        }
    }

    PerformRendering();

    // Post-processing callback.
    if (m_pfnPostProcessingCallback)
    {
        if (!m_pfnPostProcessingCallback(this, m_pvPostProcessingCallbackData))
        {
            return;
        }
    }
}
//---------------------------------------------------------------------------
inline void NiRenderStep::SetActive(bool bActive)
{
    m_bActive = bActive;
}
//---------------------------------------------------------------------------
inline bool NiRenderStep::GetActive() const
{
    return m_bActive;
}
//---------------------------------------------------------------------------
inline void NiRenderStep::SetPreProcessingCallbackFunc(Callback pfnCallback,
    void* pvCallbackData)
{
    m_pfnPreProcessingCallback = pfnCallback;
    m_pvPreProcessingCallbackData = pvCallbackData;
}
//---------------------------------------------------------------------------
inline NiRenderStep::Callback NiRenderStep::GetPreProcessingCallbackFunc()
    const
{
    return m_pfnPreProcessingCallback;
}
//---------------------------------------------------------------------------
inline void* NiRenderStep::GetPreProcessingCallbackFuncData() const
{
    return m_pvPreProcessingCallbackData;
}
//---------------------------------------------------------------------------
inline void NiRenderStep::SetPostProcessingCallbackFunc(Callback pfnCallback,
    void* pvCallbackData)
{
    m_pfnPostProcessingCallback = pfnCallback;
    m_pvPostProcessingCallbackData = pvCallbackData;
}
//---------------------------------------------------------------------------
inline NiRenderStep::Callback NiRenderStep::GetPostProcessingCallbackFunc()
    const
{
    return m_pfnPostProcessingCallback;
}
//---------------------------------------------------------------------------
inline void* NiRenderStep::GetPostProcessingCallbackFuncData() const
{
    return m_pvPostProcessingCallbackData;
}
//---------------------------------------------------------------------------
