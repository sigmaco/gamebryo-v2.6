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
inline bool NiPalette::GetHasAlpha() const
{
    return m_bAlpha;
}
//---------------------------------------------------------------------------
inline unsigned int NiPalette::GetNumEntries() const
{
    return m_uiEntries;
}
//---------------------------------------------------------------------------
inline NiPalette::PaletteEntry* NiPalette::GetEntries()
{
    return m_pkPalette;
}
//---------------------------------------------------------------------------
inline const NiPalette::PaletteEntry* NiPalette::GetEntries() const
{
    return m_pkPalette;
}
//---------------------------------------------------------------------------
inline void NiPalette::MarkAsChanged()
{
    m_uiRevID++;
}
//---------------------------------------------------------------------------
inline unsigned int NiPalette::GetRevisionID() const
{
    return m_uiRevID;
}
//---------------------------------------------------------------------------
inline NiPalette* NiPalette::GetListNext()
{
    return m_pkNext;
}
//---------------------------------------------------------------------------
inline NiPalette* NiPalette::GetListPrev()
{
    return m_pkPrev;
}
//---------------------------------------------------------------------------
inline void NiPalette::SetRendererData(RendererData* pkRendererData)
{
    m_pkRendererData = pkRendererData;
}
//---------------------------------------------------------------------------
inline NiPalette::RendererData::RendererData(NiPalette* pkPalette)
{
    m_pkPalette = pkPalette;
}
//---------------------------------------------------------------------------
inline NiPalette::RendererData::~RendererData()
{
    /* */
}
//---------------------------------------------------------------------------
inline NiPalette::RendererData* NiPalette::GetRendererData() const
{
    return m_pkRendererData;
}
//---------------------------------------------------------------------------
inline NiPalette* NiPalette::GetListHead()
{
    return ms_pkHead;
}
//---------------------------------------------------------------------------
inline NiPalette* NiPalette::GetListTail()
{
    return ms_pkTail;
}
//---------------------------------------------------------------------------
inline void NiPalette::AddPaletteToList()
{
    ms_kPaletteListCriticalSection.Lock();
    if (!ms_pkHead)
        ms_pkHead = this;
    if (ms_pkTail)
    {
        ms_pkTail->m_pkNext = this;
        m_pkPrev = ms_pkTail;
    }
    else
    {
        m_pkPrev = 0;
    }
    ms_pkTail = this;
    m_pkNext = 0;
    ms_kPaletteListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
inline void NiPalette::RemovePaletteFromList()
{
    ms_kPaletteListCriticalSection.Lock();
    if (ms_pkHead == this)
        ms_pkHead = m_pkNext;
    if (ms_pkTail == this)
        ms_pkTail = m_pkPrev;

    if (m_pkPrev)
        m_pkPrev->m_pkNext = m_pkNext;
    if (m_pkNext)
        m_pkNext->m_pkPrev = m_pkPrev;
    ms_kPaletteListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
