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
// command processing
//---------------------------------------------------------------------------
// OnCommand()
// Handles WM_COMMAND events
//   sParam1 = WORD wNotifyCode
//   sParam2 = WORD wID
//   pCtl = hwndCtl
inline bool NiSceneViewerWindow::OnCommand(int, int, 
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetInstanceReference(
    NiInstanceRef pkInstance)
{
    ms_pkInstance = pkInstance;
}
//---------------------------------------------------------------------------
inline NiInstanceRef NiSceneViewerWindow::GetInstanceReference()
{
    return ms_pkInstance;
}
//---------------------------------------------------------------------------
inline void  NiSceneViewerWindow::SetAcceleratorReference(
    NiAcceleratorRef pkAccel)
{
    ms_pkAccel = pkAccel;
}
//---------------------------------------------------------------------------
inline NiAcceleratorRef NiSceneViewerWindow::GetAcceleratorReference()
{
    return ms_pkAccel;
}
//---------------------------------------------------------------------------
inline NiSceneKeyboard* NiSceneViewerWindow::GetKeyboard() const
{
    return m_pkKeyboard;
}
//---------------------------------------------------------------------------
inline NiSceneMouse* NiSceneViewerWindow::GetMouse() const
{
    return m_pkMouse;
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::SetUI(NiUIMap* pkUI) 
{
    m_pkSceneEventHandler->SetupUI(pkUI);
    return true;
}
//---------------------------------------------------------------------------
inline NiUIMap* NiSceneViewerWindow::GetUI() 
{
    return m_pkSceneEventHandler->GetUI();
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::Terminate()
{
    m_spCamera = NULL;
    return true;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetFullscreen(bool bOn)
{
    m_bFullscreen = bOn;
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::GetFullscreen() const
{
    return m_bFullscreen;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetHardware(bool bOn)
{
    m_bHardware = bOn;
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::GetHardware() const
{
    return m_bHardware;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetStencil(bool bOn)
{
    m_bStencil = bOn;
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::GetStencil() const
{
    return m_bStencil;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetMultitexture(bool bOn)
{
    m_bMultitexture = bOn;
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::GetMultitexture() const
{
    return m_bMultitexture;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetBackgroundColor(NiColor kColor)
{
    m_kBackground = kColor;
    if (m_pkSceneViewer != NULL)
        m_pkSceneViewer->SetBackgroundColor(kColor);
}
//---------------------------------------------------------------------------
inline NiColor NiSceneViewerWindow::GetBackgroundColor()
{
    return m_kBackground;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetD3D10(bool bD3D10)
{
    m_bD3D10 = bD3D10;
}
//---------------------------------------------------------------------------
inline bool NiSceneViewerWindow::GetD3D10() const
{
    return m_bD3D10;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetWindowReference(NiWindowRef pkWnd)
{
    m_pkWnd = pkWnd;
}
//---------------------------------------------------------------------------
inline NiWindowRef NiSceneViewerWindow::GetWindowReference()
{
    return m_pkWnd;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetRenderWindowReference(NiWindowRef pkWnd)
{
    m_pkRenderWnd = pkWnd;
}
//---------------------------------------------------------------------------
inline NiWindowRef NiSceneViewerWindow::GetRenderWindowReference()
{
    return m_pkRenderWnd;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetWidth(unsigned int uiWidth)
{
    m_uiWidth = uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiSceneViewerWindow::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetHeight(unsigned int uiHeight)
{
    m_uiHeight = uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiSceneViewerWindow::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetParentWidth(unsigned int uiWidth)
{
    m_uiParentWidth = uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiSceneViewerWindow::GetParentWidth() const
{
    return m_uiParentWidth;
}
//---------------------------------------------------------------------------
inline void NiSceneViewerWindow::SetParentHeight(unsigned int uiHeight)
{
    m_uiParentHeight = uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiSceneViewerWindow::GetParentHeight() const
{
    return m_uiParentHeight;
}
//---------------------------------------------------------------------------