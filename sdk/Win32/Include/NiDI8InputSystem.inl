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
// NiDI8InputSystem inline functions
//---------------------------------------------------------------------------
// DI8CreateParams
//---------------------------------------------------------------------------
inline NiDI8InputSystem::DI8CreateParams::DI8CreateParams() :
    NiInputSystem::CreateParams(0, false, false, MAX_GAMEPADS),
    m_hOwnerInst(0), 
    m_hOwnerWnd(0) 
{
    for (unsigned int ui = 0; ui < DEFACTION_COUNT; ui++)
        m_apcMappingNames[ui] = 0;
}
//---------------------------------------------------------------------------
inline NiDI8InputSystem::DI8CreateParams::DI8CreateParams(HINSTANCE hInst, 
    HWND hWnd, NiActionMap* pkActionMap, unsigned int uiKeyboard, 
    unsigned int uiMouse, unsigned int uiGamePads, int iAxisRangeLow, 
    int iAxisRangeHigh) :
    NiInputSystem::CreateParams(pkActionMap, uiKeyboard, uiMouse, uiGamePads, 
    iAxisRangeLow, iAxisRangeHigh),
    m_hOwnerInst(hInst), 
    m_hOwnerWnd(hWnd)
{
    for (unsigned int ui = 0; ui < DEFACTION_COUNT; ui++)
        m_apcMappingNames[ui] = 0;
}
//---------------------------------------------------------------------------
inline NiDI8InputSystem::DI8CreateParams::~DI8CreateParams()
{
    for (unsigned int ui = 0; ui < DEFACTION_COUNT; ui++)
    {
        if (m_apcMappingNames[ui])
        {
            NiFree(m_apcMappingNames[ui]);
        }
    }
}
//---------------------------------------------------------------------------
inline const HINSTANCE NiDI8InputSystem::DI8CreateParams::GetOwnerInstance() 
    const
{
    return m_hOwnerInst;
}
//---------------------------------------------------------------------------
inline const HWND NiDI8InputSystem::DI8CreateParams::GetOwnerWindow() const
{
    return m_hOwnerWnd;
}
//---------------------------------------------------------------------------
inline const LPDIRECTINPUT8 NiDI8InputSystem::GetDirectInput8() const
{
    return m_pkDirectInput8;
}
//---------------------------------------------------------------------------
inline void NiDI8InputSystem::DI8CreateParams::SetOwnerInstance(
    HINSTANCE hInst)
{
    m_hOwnerInst = hInst;
}
//---------------------------------------------------------------------------
inline void NiDI8InputSystem::DI8CreateParams::SetOwnerWindow(HWND hWnd)
{
    m_hOwnerWnd = hWnd;
}
//---------------------------------------------------------------------------
// DI8Description
//---------------------------------------------------------------------------
inline NiDI8InputSystem::DI8Description::DI8Description() :
    NiInputDevice::Description()
{
    SetGUIDInstance(0);
}
//---------------------------------------------------------------------------
inline NiDI8InputSystem::DI8Description::DI8Description(
    NiInputDevice::Type eType, unsigned int uiPort, unsigned int uiSlot, 
    const char* pcName, const GUID* pkGUIDInstance) :
    NiInputDevice::Description(eType, uiPort, uiSlot, pcName)
{
    SetGUIDInstance(pkGUIDInstance);
}
//---------------------------------------------------------------------------
inline NiDI8InputSystem::DI8Description::~DI8Description()
{
}
//---------------------------------------------------------------------------
inline const GUID* NiDI8InputSystem::DI8Description::GetGUIDInstance() const
{
    return &m_kGUIDInstance;
}
//---------------------------------------------------------------------------
inline void NiDI8InputSystem::DI8Description::SetGUIDInstance(
    const GUID* pkGUID)
{
    unsigned int uiSize = sizeof(GUID);
    if (pkGUID)
        NiMemcpy((void*)&m_kGUIDInstance, (const void*)pkGUID, uiSize);
    else
        memset((void*)&m_kGUIDInstance, 0, uiSize);
}
//---------------------------------------------------------------------------
inline HINSTANCE NiDI8InputSystem::GetOwnerInstance()
{
    return ms_hOwnerInst;
}
//---------------------------------------------------------------------------
inline HWND NiDI8InputSystem::GetOwnerWindow()
{
    return ms_hOwnerWnd;
}
//---------------------------------------------------------------------------
inline HINSTANCE NiDI8InputSystem::GetXInputLibraryInstance()
{
    return ms_hXInputLib;
}
//---------------------------------------------------------------------------
inline void NiDI8InputSystem::SetOwnerInstance(HINSTANCE hInst)
{
    ms_hOwnerInst = hInst;
}
//---------------------------------------------------------------------------
inline void NiDI8InputSystem::SetOwnerWindow(HWND hWnd)
{
    ms_hOwnerWnd = hWnd;
}
//---------------------------------------------------------------------------
inline unsigned int NiDI8InputSystem::GetDefaultActionBufferSize()
{
    return ms_uiDefaultActionBufferSize;
}
//---------------------------------------------------------------------------
