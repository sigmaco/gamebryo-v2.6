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
inline void NiRenderFrame::_SDMInit()
{
    ms_kDefaultName = "Render Frame";
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::_SDMShutdown()
{
    ms_kDefaultName = NULL;
}
//---------------------------------------------------------------------------
inline NiRenderFrame::NiRenderFrame() : m_pfnPreProcessingCallback(NULL),
    m_pfnPostProcessingCallback(NULL), m_pvPreProcessingCallbackData(NULL),
    m_pvPostProcessingCallbackData(NULL), m_kName(ms_kDefaultName)
{
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiRenderFrame::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::AppendRenderStep(NiRenderStep* pkRenderStep)
{
    NIASSERT(pkRenderStep);
    m_kRenderSteps.AddTail(pkRenderStep);
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::PrependRenderStep(NiRenderStep* pkRenderStep)
{
    NIASSERT(pkRenderStep);
    m_kRenderSteps.AddHead(pkRenderStep);
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::RemoveRenderStep(NiRenderStep* pkRenderStep)
{
    NIASSERT(pkRenderStep);
    m_kRenderSteps.Remove(pkRenderStep);
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::RemoveAllRenderSteps()
{
    m_kRenderSteps.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiRenderStepPtr>& NiRenderFrame::GetRenderSteps()
{
    return m_kRenderSteps;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiRenderStepPtr>& NiRenderFrame::GetRenderSteps()
    const
{
    return m_kRenderSteps;
}
//---------------------------------------------------------------------------
inline NiRenderStep* NiRenderFrame::GetRenderStepByName(
    const NiFixedString& kName) const
{
    NiTListIterator kIter;
    return GetRenderStepAndPosByName(kName, kIter);
}
//---------------------------------------------------------------------------
inline NiTListIterator NiRenderFrame::GetRenderStepPosByName(
    const NiFixedString& kName) const
{
    NiTListIterator kIter;
    GetRenderStepAndPosByName(kName, kIter);
    return kIter;
}
//---------------------------------------------------------------------------
inline NiRenderStep* NiRenderFrame::GetRenderStepAndPosByName(
    const NiFixedString& kName, NiTListIterator& kIter) const
{
    kIter = m_kRenderSteps.GetHeadPos();
    while (kIter)
    {
        NiRenderStep* pkRenderStep = m_kRenderSteps.Get(kIter);
        if (pkRenderStep->GetName() == kName)
        {
            return pkRenderStep;
        }
        kIter = m_kRenderSteps.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::GatherStatistics(unsigned int& uiNumObjectsDrawn,
    float& fCullTime, float& fRenderTime) const
{
    // Reset statistics.
    uiNumObjectsDrawn = 0;
    fCullTime = fRenderTime = 0.0f;

    // Iterate over render steps, gathering statistics.
    NiTListIterator kIter = m_kRenderSteps.GetHeadPos();
    while (kIter)
    {
        NiRenderStep* pkStep = m_kRenderSteps.GetNext(kIter);
        NIASSERT(pkStep);
        uiNumObjectsDrawn += pkStep->GetNumObjectsDrawn();
        fCullTime += pkStep->GetCullTime();
        fRenderTime += pkStep->GetRenderTime();
    }
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::SetPreProcessingCallbackFunc(Callback pfnCallback,
    void* pvCallbackData)
{
    m_pfnPreProcessingCallback = pfnCallback;
    m_pvPreProcessingCallbackData = pvCallbackData;
}
//---------------------------------------------------------------------------
inline NiRenderFrame::Callback NiRenderFrame::GetPreProcessingCallbackFunc()
    const
{
    return m_pfnPreProcessingCallback;
}
//---------------------------------------------------------------------------
inline void* NiRenderFrame::GetPreProcessingCallbackFuncData() const
{
    return m_pvPreProcessingCallbackData;
}
//---------------------------------------------------------------------------
inline void NiRenderFrame::SetPostProcessingCallbackFunc(Callback pfnCallback,
    void* pvCallbackData)
{
    m_pfnPostProcessingCallback = pfnCallback;
    m_pvPostProcessingCallbackData = pvCallbackData;
}
//---------------------------------------------------------------------------
inline NiRenderFrame::Callback NiRenderFrame::GetPostProcessingCallbackFunc()
    const
{
    return m_pfnPostProcessingCallback;
}
//---------------------------------------------------------------------------
inline void* NiRenderFrame::GetPostProcessingCallbackFuncData() const
{
    return m_pvPostProcessingCallbackData;
}
//---------------------------------------------------------------------------
