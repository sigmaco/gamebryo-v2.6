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
inline NiViewRenderClick::NiViewRenderClick() : m_pvProcessorData(NULL),
    m_kProcessedGeometry(0, 4), m_uiNumObjectsDrawn(0), m_fCullTime(0.0f),
    m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::_SDMInit()
{
    ms_spDefaultProcessor = NiNew NiRenderListProcessor;
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::_SDMShutdown()
{
    ms_spDefaultProcessor = NULL;
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::AppendRenderView(NiRenderView* pkRenderView)
{
    NIASSERT(pkRenderView);
    m_kRenderViews.AddTail(pkRenderView);
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::PrependRenderView(NiRenderView* pkRenderView)
{
    NIASSERT(pkRenderView);
    m_kRenderViews.AddHead(pkRenderView);
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::RemoveRenderView(NiRenderView* pkRenderView)
{
    NIASSERT(pkRenderView);
    m_kRenderViews.Remove(pkRenderView);
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::RemoveAllRenderViews()
{
    m_kRenderViews.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiRenderViewPtr>& NiViewRenderClick::GetRenderViews()
{
    return m_kRenderViews;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiRenderViewPtr>&
    NiViewRenderClick::GetRenderViews() const
{
    return m_kRenderViews;
}
//---------------------------------------------------------------------------
inline NiRenderView* NiViewRenderClick::GetRenderViewByName(
    const NiFixedString& kName) const
{
    NiTListIterator kIter;
    return GetRenderViewAndPosByName(kName, kIter);
}
//---------------------------------------------------------------------------
inline NiTListIterator NiViewRenderClick::GetRenderViewPosByName(
    const NiFixedString& kName) const
{
    NiTListIterator kIter;
    GetRenderViewAndPosByName(kName, kIter);
    return kIter;
}
//---------------------------------------------------------------------------
inline NiRenderView* NiViewRenderClick::GetRenderViewAndPosByName(
    const NiFixedString& kName, NiTListIterator& kIter) const
{
    kIter = m_kRenderViews.GetHeadPos();
    while (kIter)
    {
        NiRenderView* pkRenderView = m_kRenderViews.Get(kIter);
        if (pkRenderView->GetName() == kName)
        {
            return pkRenderView;
        }
        kIter = m_kRenderViews.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
inline void NiViewRenderClick::SetProcessor(NiRenderListProcessor* pkProcessor,
    void* pvProcessorData)
{
    m_spProcessor = pkProcessor;
    m_pvProcessorData = pvProcessorData;
}
//---------------------------------------------------------------------------
inline NiRenderListProcessor* NiViewRenderClick::GetProcessor() const
{
    return m_spProcessor;
}
//---------------------------------------------------------------------------
inline void* NiViewRenderClick::GetProcessorData() const
{
    return m_pvProcessorData;
}
//---------------------------------------------------------------------------
