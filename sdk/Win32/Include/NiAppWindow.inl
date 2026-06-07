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
inline void NiAppWindow::SetWindowReference(NiWindowRef pWnd)
{
    m_pWnd = pWnd;
}
//---------------------------------------------------------------------------
inline NiWindowRef NiAppWindow::GetWindowReference()
{
    return m_pWnd;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetRenderWindowReference(NiWindowRef pWnd)
{
    m_pRenderWnd = pWnd;
}
//---------------------------------------------------------------------------
inline NiWindowRef NiAppWindow::GetRenderWindowReference()
{
    return m_pRenderWnd;
}
//---------------------------------------------------------------------------
inline const char* NiAppWindow::GetWindowClassName()
{
    return ms_acWindowClassName;
}

//---------------------------------------------------------------------------
inline unsigned long NiAppWindow::GetWindowStyle()
{
    return m_ulWindowStyle;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetWindowStyle(unsigned long ulWindowStyle)
{
    m_ulWindowStyle = ulWindowStyle;
}
//---------------------------------------------------------------------------
inline const char* NiAppWindow::GetWindowCaption() const
{
    return m_pcWindowCaption;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetWidth(unsigned int uiWidth)
{
    m_uiWidth = uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetHeight(unsigned int uiHeight)
{
    m_uiHeight = uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetParentWidth(unsigned int uiWidth)
{
    m_uiParentWidth = uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetParentWidth() const
{
    return m_uiParentWidth;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetParentHeight(unsigned int uiHeight)
{
    m_uiParentHeight = uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetParentHeight() const
{
    return m_uiParentHeight;
}
//---------------------------------------------------------------------------
inline void NiAppWindow::SetStatusWindowReference(NiStatusWindowRef pWnd, 
    const unsigned int uiPane = 0)
{
    if (uiPane < GetNumStatusPanes())
    {
        if (m_ppStatusWnd == NULL)
            m_ppStatusWnd = NiAlloc(NiStatusWindowRef, GetNumStatusPanes());
        m_ppStatusWnd[uiPane] = pWnd;
    }
}
//---------------------------------------------------------------------------
inline NiStatusWindowRef NiAppWindow::GetStatusWindowReference
    (const unsigned int uiPane = 0) const
{
    if (uiPane < GetNumStatusPanes() && m_ppStatusWnd)
        return m_ppStatusWnd[uiPane];
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline bool NiAppWindow::GetStatusPanesExist() const
{
    return m_bStatusPanes;
}
//---------------------------------------------------------------------------
inline unsigned int NiAppWindow::GetNumStatusPanes() const
{
    return m_uiNumStatusPanes;
}
//---------------------------------------------------------------------------

