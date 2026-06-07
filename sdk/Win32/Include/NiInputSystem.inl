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
// NiInputSystem inline functions
//---------------------------------------------------------------------------
inline NiInputSystem::CreateParams::CreateParams(NiActionMap* pkActionMap, 
    unsigned int uiKeyboard, unsigned int uiMouse, unsigned int uiGamePads, 
    int iAxisRangeLow, int iAxisRangeHigh) :
    m_pkActionMap(pkActionMap), 
    m_uiKeyboard(uiKeyboard), 
    m_uiMouse(uiMouse), 
    m_uiGamePads(uiGamePads), 
    m_iAxisRangeLow(iAxisRangeLow), 
    m_iAxisRangeHigh(iAxisRangeHigh)
{
}
//---------------------------------------------------------------------------
inline NiInputSystem::CreateParams::~CreateParams()
{
}
//---------------------------------------------------------------------------
inline NiActionMap* NiInputSystem::CreateParams::GetActionMap()
{
    return m_pkActionMap;
}
//---------------------------------------------------------------------------
inline const NiActionMap* NiInputSystem::CreateParams::GetActionMap() const
{
    return m_pkActionMap;
}
//---------------------------------------------------------------------------
inline const bool NiInputSystem::CreateParams::UseKeyboard() const
{
    return (m_uiKeyboard ? true : false);
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputSystem::CreateParams::GetKeyboardUsage() 
    const
{
    return m_uiKeyboard;
}
//---------------------------------------------------------------------------
inline const bool NiInputSystem::CreateParams::UseMouse() const
{
    return (m_uiMouse ? true : false);
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputSystem::CreateParams::GetMouseUsage() const
{
    return m_uiMouse;
}
//---------------------------------------------------------------------------
inline const unsigned int NiInputSystem::CreateParams::GamePadCount() const
{
    return m_uiGamePads;
}
//---------------------------------------------------------------------------
inline const void NiInputSystem::CreateParams::GetAxisRange(
    int& iLow, int& iHigh) const
{
    iLow = m_iAxisRangeLow;
    iHigh = m_iAxisRangeHigh;
}
//---------------------------------------------------------------------------
inline void NiInputSystem::CreateParams::SetActionMap(
    NiActionMap* pkActionMap)
{
    m_pkActionMap = pkActionMap;
}
//---------------------------------------------------------------------------
inline void NiInputSystem::CreateParams::SetKeyboardUsage(
    unsigned int uiUsage)
{
    m_uiKeyboard = uiUsage;
}
//---------------------------------------------------------------------------
inline void NiInputSystem::CreateParams::SetMouseUsage(unsigned int uiUsage)
{
    m_uiMouse = uiUsage;
}
//---------------------------------------------------------------------------
inline void NiInputSystem::CreateParams::SetGamePadCount(unsigned int uiCount)
{
    m_uiGamePads = uiCount;
}
//---------------------------------------------------------------------------
inline void NiInputSystem::CreateParams::SetAxisRange(int iLow, int iHigh)
{
    m_iAxisRangeLow = iLow;
    m_iAxisRangeHigh = iHigh;
}
//---------------------------------------------------------------------------
// NiInputSystem
//---------------------------------------------------------------------------
inline const NiInputErr NiInputSystem::GetLastError() const
{
    return m_eLastError;
}
//---------------------------------------------------------------------------
inline const NiInputSystem::Mode NiInputSystem::GetMode() const
{
    return m_eMode;
}
//---------------------------------------------------------------------------
inline const int NiInputSystem::GetAxisRangeLow() const
{
    return m_iAxisRangeLow;
}
//---------------------------------------------------------------------------
inline const int NiInputSystem::GetAxisRangeHigh() const
{
    return m_iAxisRangeHigh;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputSystem::AddAvailableDevice(
    NiInputDevice::Description* pkDescription)
{
    m_kAvailableDevices.AddTail(pkDescription);
    return m_kAvailableDevices.GetSize();
}
//---------------------------------------------------------------------------
inline NiActionData* NiInputSystem::GetActionData()
{
    if (m_pkActionDataPool)
        return m_pkActionDataPool->GetFreeObject();
    return 0;
}
//---------------------------------------------------------------------------
inline void NiInputSystem::ReleaseActionData(NiActionData* pkActionData)
{
    NIASSERT(m_pkActionDataPool);
    m_pkActionDataPool->ReleaseObject(pkActionData);
}
//---------------------------------------------------------------------------
inline unsigned int NiInputSystem::GetAvailableDeviceCount()
{
    return m_kAvailableDevices.GetSize();
}
//---------------------------------------------------------------------------
inline NiInputDevice::Description* NiInputSystem::GetFirstDeviceDesc()
{
    m_kAvailableDeviceIter = m_kAvailableDevices.GetHeadPos();
    if (m_kAvailableDeviceIter)
        return m_kAvailableDevices.GetNext(m_kAvailableDeviceIter);
    return 0;
}
//---------------------------------------------------------------------------
inline NiInputDevice::Description* NiInputSystem::GetNextDeviceDesc()
{
    if (m_kAvailableDeviceIter)
        return m_kAvailableDevices.GetNext(m_kAvailableDeviceIter);
    return 0;
}
//---------------------------------------------------------------------------
// NiInputSystem
//---------------------------------------------------------------------------
inline NiInputGamePad* NiInputSystem::GetGamePad(unsigned int uiPort, 
    unsigned int uiSlot) const
{
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
        {
            if ((m_aspGamePads[ui]->GetPort() == uiPort) &&
                (m_aspGamePads[ui]->GetSlot() == uiSlot))
            {
                return m_aspGamePads[ui];
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
inline NiInputGamePad* NiInputSystem::GetGamePad(
    unsigned int uiPadIndex) const
{
    if (uiPadIndex < MAX_GAMEPADS)
        return m_aspGamePads[uiPadIndex];
    return 0;
}
//---------------------------------------------------------------------------
inline NiInputMouse* NiInputSystem::GetMouse()
{
    return m_spMouse;
}
//---------------------------------------------------------------------------
inline NiInputKeyboard* NiInputSystem::GetKeyboard()
{
    return m_spKeyboard;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputSystem::GetActionDataCount()
{
    return m_kActionDataList.GetSize();
}
//---------------------------------------------------------------------------
inline NiActionData* NiInputSystem::PopActionData()
{
    if (m_kActionDataList.GetSize())
        return m_kActionDataList.RemoveHead();
    return 0;
}
//---------------------------------------------------------------------------
inline NiActionData* NiInputSystem::GetFirstActionData()
{
    m_kActionDataIter = m_kActionDataList.GetHeadPos();
    if (m_kActionDataIter)
        return m_kActionDataList.GetNext(m_kActionDataIter);
    return 0;
}
//---------------------------------------------------------------------------
inline NiActionData* NiInputSystem::GetNextActionData()
{
    if (m_kActionDataIter)
        return m_kActionDataList.GetNext(m_kActionDataIter);
    return 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputSystem::MakeDeviceID(unsigned int uiPort, 
    unsigned int uiSlot)
{
    return ((uiSlot << 16) | uiPort);
}
//---------------------------------------------------------------------------
inline unsigned int NiInputSystem::DeviceIDPort(unsigned int uiDeviceID)
{
    return (uiDeviceID & 0x0000ffff);
}
//---------------------------------------------------------------------------
inline unsigned int NiInputSystem::DeviceIDSlot(unsigned int uiDeviceID)
{
    return ((uiDeviceID & 0xffff0000) >> 16);
}
//---------------------------------------------------------------------------
