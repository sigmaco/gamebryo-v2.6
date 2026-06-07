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
inline void NiDefaultClickRenderStep::AppendRenderClick(
    NiRenderClick* pkRenderClick)
{
    NIASSERT(pkRenderClick);
    m_kRenderClicks.AddTail(pkRenderClick);
}
//---------------------------------------------------------------------------
inline void NiDefaultClickRenderStep::PrependRenderClick(
    NiRenderClick* pkRenderClick)
{
    NIASSERT(pkRenderClick);
    m_kRenderClicks.AddHead(pkRenderClick);
}
//---------------------------------------------------------------------------
inline void NiDefaultClickRenderStep::RemoveRenderClick(
    NiRenderClick* pkRenderClick)
{
    NIASSERT(pkRenderClick);
    m_kRenderClicks.Remove(pkRenderClick);
}
//---------------------------------------------------------------------------
inline void NiDefaultClickRenderStep::RemoveAllRenderClicks()
{
    m_kRenderClicks.RemoveAll();
}
//---------------------------------------------------------------------------
inline NiTPointerList<NiRenderClickPtr>&
    NiDefaultClickRenderStep::GetRenderClickList()
{
    return m_kRenderClicks;
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiRenderClickPtr>&
    NiDefaultClickRenderStep::GetRenderClickList() const
{
    return m_kRenderClicks;
}
//---------------------------------------------------------------------------
inline NiRenderClick* NiDefaultClickRenderStep::GetRenderClickByName(
    const NiFixedString& kName) const
{
    NiTListIterator kIter;
    return GetRenderClickAndPosByName(kName, kIter);
}
//---------------------------------------------------------------------------
inline NiTListIterator NiDefaultClickRenderStep::GetRenderClickPosByName(
    const NiFixedString& kName) const
{
    NiTListIterator kIter;
    GetRenderClickAndPosByName(kName, kIter);
    return kIter;
}
//---------------------------------------------------------------------------
inline NiRenderClick* NiDefaultClickRenderStep::GetRenderClickAndPosByName(
    const NiFixedString& kName, NiTListIterator& kIter) const
{
    kIter = m_kRenderClicks.GetHeadPos();
    while (kIter)
    {
        NiRenderClick* pkRenderClick = m_kRenderClicks.Get(kIter);
        if (pkRenderClick->GetName() == kName)
        {
            return pkRenderClick;
        }
        kIter = m_kRenderClicks.GetNextPos(kIter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
