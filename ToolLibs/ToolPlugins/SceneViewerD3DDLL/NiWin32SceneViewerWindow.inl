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
inline const char* NiWin32SceneViewerWindow::GetWindowClassName()
{
    return ms_pcWindowClassName;
}
//---------------------------------------------------------------------------
inline void NiWin32SceneViewerWindow::SetWindowClassName(
    const char* pcClassName)
{
    if (pcClassName != NULL)
    {
        NiFree(ms_pcWindowClassName);
        size_t stLen = strlen(pcClassName) + 1;
        ms_pcWindowClassName = NiAlloc(char, stLen);
        NiStrcpy(ms_pcWindowClassName, stLen, pcClassName);
    }
}
//---------------------------------------------------------------------------
inline unsigned long NiWin32SceneViewerWindow::GetWindowStyle()
{
    return m_ulWindowStyle;
}
//---------------------------------------------------------------------------
inline void NiWin32SceneViewerWindow::SetWindowStyle(
    unsigned long ulWindowStyle)
{
    m_ulWindowStyle = ulWindowStyle;
}
//---------------------------------------------------------------------------
inline const char* NiWin32SceneViewerWindow::GetWindowCaption() const
{
    return m_pcWindowCaption;
}
//---------------------------------------------------------------------------
inline void NiWin32SceneViewerWindow::SetWindowCaption(char* pcString) 
{
    m_pcWindowCaption = pcString;
}
//---------------------------------------------------------------------------
inline unsigned int NiWin32SceneViewerWindow::GetMenuID() const
{
    return m_uiMenuID;
}
//---------------------------------------------------------------------------
