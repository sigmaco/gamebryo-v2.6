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
// NiD3DRenderStateGroup inline functions
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup::NiD3DRSEntry::NiD3DRSEntry() :
    m_uiRenderState(0), 
    m_uiValue(0), 
    m_pkNext(0), 
    m_pkPrev(0)
{
}
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup::NiD3DRSEntry::NiD3DRSEntry(
    unsigned int uiRenderState, unsigned int uiValue) :
    m_uiRenderState(uiRenderState), 
    m_uiValue(uiValue) 
{
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DRenderStateGroup::NiD3DRSEntry::GetRenderState()
{
    return m_uiRenderState;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3DRenderStateGroup::NiD3DRSEntry::GetValue()
{
    return m_uiValue;
}
//---------------------------------------------------------------------------
inline void NiD3DRenderStateGroup::NiD3DRSEntry::SetRenderState(
    unsigned int uiRenderState)
{
    m_uiRenderState = uiRenderState;
}
//---------------------------------------------------------------------------
inline void NiD3DRenderStateGroup::NiD3DRSEntry::SetValue(
    unsigned int uiValue)
{
    m_uiValue = uiValue;
}
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup::NiD3DRSEntry* 
NiD3DRenderStateGroup::NiD3DRSEntry::GetNext()
{
    return m_pkNext;
}
//---------------------------------------------------------------------------
inline void NiD3DRenderStateGroup::NiD3DRSEntry::SetNext(
    NiD3DRenderStateGroup::NiD3DRSEntry* pkEntry)
{
    m_pkNext = pkEntry;
}
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup::NiD3DRSEntry* 
NiD3DRenderStateGroup::NiD3DRSEntry::GetPrev()
{
    return m_pkPrev;
}
//---------------------------------------------------------------------------
inline void NiD3DRenderStateGroup::NiD3DRSEntry::SetPrev(
    NiD3DRenderStateGroup::NiD3DRSEntry* pkEntry)
{
    m_pkPrev = pkEntry;
}
//---------------------------------------------------------------------------
inline NiD3DRenderStateGroup::NiD3DRenderStateGroup() :
    m_bRendererOwned(false), 
    m_uiStateCount(0), 
    m_pkRenderStates(0), 
    m_uiSavedStateCount(0), 
    m_pkSavedRenderStates(0)
{
}
//---------------------------------------------------------------------------
inline void NiD3DRenderStateGroup::InsertRSEntryIntoNoSaveList(
    NiD3DRenderStateGroup::NiD3DRSEntry* pkEntry)
{
    if (m_pkRenderStates)
    {
        m_pkRenderStates->SetPrev(pkEntry);
        pkEntry->SetNext(m_pkRenderStates);
    }
    pkEntry->SetPrev(0);
    m_pkRenderStates = pkEntry;
    m_uiStateCount++;
}
//---------------------------------------------------------------------------
inline void NiD3DRenderStateGroup::InsertRSEntryIntoSavedList(
    NiD3DRenderStateGroup::NiD3DRSEntry* pkEntry)
{
    if (m_pkSavedRenderStates)
    {
        m_pkSavedRenderStates->SetPrev(pkEntry);
        pkEntry->SetNext(m_pkSavedRenderStates);
    }
    pkEntry->SetPrev(0);
    m_pkSavedRenderStates = pkEntry;
    m_uiSavedStateCount++;
}
//---------------------------------------------------------------------------
