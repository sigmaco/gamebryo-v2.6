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
// NiUIManager Inline Functions
//---------------------------------------------------------------------------
inline bool NiUIManager::IsLower(NiPoint2& kP1, NiPoint2& kP2) const
{
    return ((kP1[1] < kP2[1]) ? true : 
        ((kP1[1] == kP2[1]) ? (kP1[0] < kP2[0]) : false));
}
//---------------------------------------------------------------------------
inline float NiUIManager::GetMaxCharWidthInNSC() const
{
    return m_kMaxCharDimensionsInNSC.x;
}
//---------------------------------------------------------------------------
inline float NiUIManager::GetMaxCharHeightInNSC() const
{
    return m_kMaxCharDimensionsInNSC.y;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::ReserveMouse(NiUIBaseSlot1<unsigned char>& 
    kLeftMouseSlot)
{
    if ((m_bMouseReserved) || !(m_bMouseExists))
    {
        return false;
    }
    m_bMouseReserved = true;
    m_kLeftMouseSignal.Subscribe(&kLeftMouseSlot);
    return true;
}
//---------------------------------------------------------------------------
inline void NiUIManager::UnreserveMouse(NiUIBaseSlot1<unsigned char>& 
    kLeftMouseSlot)
{
    if (!m_bMouseExists)
        return;

    m_kLeftMouseSignal.Unsubscribe(&kLeftMouseSlot);
    if (m_kLeftMouseSignal.NumSubscribed() == 0)
        m_bMouseReserved = false;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::ReserveMouseButton(NiInputMouse::Button eHotkey, 
    NiUIBaseSlot1<unsigned char>* pkSlot, unsigned char ucButtonState, 
    NiInputMouse::Modifiers eModifier)
{
    if (!m_bMouseExists)
        return false;

    // First check to see if the hotkey has already been reserved
    if (MouseButtonReservedAt(eHotkey, eModifier) != -1)
    {
        return false;
    }

    // It has not been reserved, allow them to reserve it
    NiMouseHotkeySignalPtr spElement = NiNew NiMouseHotkeySignal;
    spElement->m_eHotkey = eHotkey;
    spElement->m_eModifier = eModifier;
    spElement->m_ucState = ucButtonState;
    spElement->m_kSignal.Subscribe(pkSlot);
    m_kMouseActionSet.Add(spElement);
    spElement = NULL;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::ReserveGamePadButton(NiInputGamePad::Button eHotkey, 
    NiUIBaseSlot1<unsigned char>* pkSlot, unsigned char ucButtonState, 
    NiInputGamePad::ButtonMask eModifier)
{
    if (!m_bGamePadExists)
        return false;

    // First check to see if the hotkey has already been reserved
    if (GamePadButtonReservedAt(eHotkey, eModifier) != -1)
    {
        return false;
    }

    // It has not been reserved, allow them to reserve it
    NiGamePadHotkeySignalPtr spElement = NiNew NiGamePadHotkeySignal;
    spElement->m_eHotkey = eHotkey;
    spElement->m_uiModifier = eModifier;
    spElement->m_bIsHotkey = true;
    spElement->m_ucState = ucButtonState;
    spElement->m_kSignal.Subscribe(pkSlot);
    // This should never be referenced anyway
    spElement->m_eAxis = NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ;
    m_kGamePadActionSet.Add(spElement);
    spElement = NULL;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::ReserveGamePadAxis(NiInputGamePad::DefaultAxis eAxis,
    NiUIBaseSlot1<unsigned char>* pkSlot)
{
    if (!m_bGamePadExists)
        return false;

    // First check to see if the hotkey has already been reserved
    if (GamePadAxisReservedAt(eAxis) != -1)
        return false;

    // It has not been reserved, allow them to reserve it
    NiGamePadHotkeySignalPtr spElement = NiNew NiGamePadHotkeySignal;
    spElement->m_eAxis = eAxis;
    spElement->m_bIsHotkey = false;
    spElement->m_ucState = 0;
    spElement->m_uiModifier = NiInputGamePad::NIGP_MASK_NONE;
    spElement->m_eHotkey = NiInputGamePad::NIGP_NONE;
    spElement->m_kSignal.Subscribe(pkSlot);
    m_kGamePadActionSet.Add(spElement);
    spElement = NULL;

    return true;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::ReserveKeyboardButton(
    NiInputKeyboard::KeyCode eHotkey, NiUIBaseSlot1<unsigned char>* pkSlot, 
    unsigned char ucButtonState, NiInputKeyboard::Modifiers eModifier)
{
    if (!m_bKeyboardExists)
        return false;

    // First check to see if the hotkey has already been reserved
    if (KeyboardButtonReservedAt(eHotkey, eModifier) != -1)
        return false;

    // It has not been reserved, allow them to reserve it
    NiKeyboardHotkeySignalPtr spElement = NiNew NiKeyboardHotkeySignal;
    spElement->m_eHotkey = eHotkey;
    spElement->m_eModifier = eModifier;
    spElement->m_ucState = ucButtonState;
    spElement->m_kSignal.Subscribe(pkSlot);
    m_kKeyboardActionSet.Add(spElement);
    spElement = NULL;

    return true;
}
//---------------------------------------------------------------------------
inline void NiUIManager::UnreserveMouseButton(NiInputMouse::Button eHotkey, 
    NiUIBaseSlot1<unsigned char>* pkSlot, NiInputMouse::Modifiers eModifier)
{
    if (!m_bMouseExists)
        return;

    int iLocation = MouseButtonReservedAt(eHotkey, eModifier);
    if ((iLocation >= 0) && 
        m_kMouseActionSet.GetAt(iLocation)->m_kSignal.IsSubscribed(pkSlot))
    {
        m_kMouseActionSet.RemoveAt((unsigned int)iLocation);
    }
}
//---------------------------------------------------------------------------
inline void NiUIManager::UnreserveGamePadButton(
    NiInputGamePad::Button eHotkey, NiUIBaseSlot1<unsigned char>* pkSlot, 
    NiInputGamePad::ButtonMask eModifier)
{
    if (!m_bGamePadExists)
        return;

    int iLocation = GamePadButtonReservedAt(eHotkey, eModifier);
    if ((iLocation >= 0) &&
        m_kGamePadActionSet.GetAt(iLocation)->m_kSignal.IsSubscribed(pkSlot))
    {
        m_kGamePadActionSet.RemoveAt((unsigned int)iLocation);
    }
}
//---------------------------------------------------------------------------
inline void NiUIManager::UnreserveGamePadAxis(
    NiInputGamePad::DefaultAxis eAxis, NiUIBaseSlot1<unsigned char>* pkSlot)
{
    if (!m_bGamePadExists)
        return;

    int iLocation = GamePadAxisReservedAt(eAxis);
    if ((iLocation >= 0) && 
        m_kGamePadActionSet.GetAt(iLocation)->m_kSignal.IsSubscribed(pkSlot))
    {
        m_kGamePadActionSet.RemoveAt((unsigned int)iLocation);
    }
}
//---------------------------------------------------------------------------
inline void NiUIManager::UnreserveKeyboardButton(
    NiInputKeyboard::KeyCode eHotkey, NiUIBaseSlot1<unsigned char>* pkSlot, 
    NiInputKeyboard::Modifiers eModifier)
{
    if (!m_bKeyboardExists)
        return;

    int iLocation = KeyboardButtonReservedAt(eHotkey, eModifier);
    if ((iLocation >= 0) && 
        m_kKeyboardActionSet.GetAt(iLocation)->m_kSignal.IsSubscribed(pkSlot))
    {
        m_kKeyboardActionSet.RemoveAt((unsigned int)iLocation);
    }
}
//---------------------------------------------------------------------------
inline int NiUIManager::MouseButtonReservedAt(NiInputMouse::Button eHotkey,
        NiInputMouse::Modifiers eModifier)
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kMouseActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if ((m_kMouseActionSet.GetAt(uiIdx)->m_eHotkey == eHotkey) &&
            (m_kMouseActionSet.GetAt(uiIdx)->m_eModifier == eModifier))
        {
            return uiIdx;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
inline int NiUIManager::GamePadButtonReservedAt(
    NiInputGamePad::Button eHotkey, NiInputGamePad::ButtonMask eModifier)
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kGamePadActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if ((m_kGamePadActionSet.GetAt(uiIdx)->m_bIsHotkey) &&
            (m_kGamePadActionSet.GetAt(uiIdx)->m_eHotkey == eHotkey) &&
            (m_kGamePadActionSet.GetAt(uiIdx)->m_uiModifier == 
            (unsigned int)eModifier))
        {
            return uiIdx;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
inline int NiUIManager::GamePadAxisReservedAt(
    NiInputGamePad::DefaultAxis eAxis)
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kGamePadActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if ((!m_kGamePadActionSet.GetAt(uiIdx)->m_bIsHotkey) &&
            (m_kGamePadActionSet.GetAt(uiIdx)->m_eAxis == eAxis))
        {
            return uiIdx;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
inline int NiUIManager::KeyboardButtonReservedAt(
    NiInputKeyboard::KeyCode eHotkey, NiInputKeyboard::Modifiers eModifier)
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kKeyboardActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if ((m_kKeyboardActionSet.GetAt(uiIdx)->m_eHotkey == eHotkey) &&
            (m_kKeyboardActionSet.GetAt(uiIdx)->m_eModifier == eModifier))
        {
            return uiIdx;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------
inline const NiUIAtlasMap* NiUIManager::GetAtlasMap()
{
    return &m_kAtlasMap;
}
//---------------------------------------------------------------------------
inline NiFont* NiUIManager::GetFont()
{
    return m_spFont;
}
//---------------------------------------------------------------------------
inline void NiUIManager::RegisterUIRenderGroup(NiUIRenderGroup* pkRenderGroup)
{
    if (pkRenderGroup == NULL)
    {
        return;
    }
    m_kUIRenderGroups.AddFirstEmpty(pkRenderGroup);
}
//---------------------------------------------------------------------------
inline bool NiUIManager::KeyboardExists()
{
    return m_bKeyboardExists;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::MouseExists()
{
    return m_bMouseExists;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::GamePadExists()
{
    return m_bGamePadExists;
}
//---------------------------------------------------------------------------
inline NiInputKeyboard* NiUIManager::GetKeyboard()
{
    if (!KeyboardExists())
        return NULL;
    return m_spInputSystem->GetKeyboard();
}
//---------------------------------------------------------------------------
inline NiInputMouse* NiUIManager::GetMouse()
{
    if (!MouseExists())
        return NULL;
    return m_spInputSystem->GetMouse();
}
//---------------------------------------------------------------------------
inline NiInputSystem* NiUIManager::GetInputSystem()
{
    return m_spInputSystem;
}
//---------------------------------------------------------------------------
inline NiCursor* NiUIManager::GetCursor()
{
    return m_spCursor;
}
//---------------------------------------------------------------------------
inline void NiUIManager::SetVisible(bool bVisible)
{
    m_bVisible = bVisible;
}
//---------------------------------------------------------------------------
inline bool NiUIManager::IsVisible()
{
    return m_bVisible;
}
//---------------------------------------------------------------------------
inline NiUIGroup* NiUIManager::GetUIGroupAt(unsigned int ui) const
{
    return m_kUIGroups.GetAt(ui);
}
//---------------------------------------------------------------------------
inline unsigned int NiUIManager::GetUIGroupCount() const
{
    return m_kUIGroups.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiUIManager::GetSeletedUIGroup() const
{
    if (m_iCurUIGroup == -1)
        return UINT_MAX;
    else
        return (unsigned int)m_iCurUIGroup;
}
//---------------------------------------------------------------------------
inline NiUIGroup* NiUIManager::SetSelectedUIGroup(unsigned int ui)
{
    NIASSERT(ui < GetUIGroupCount());
    if ((int)ui != m_iCurUIGroup)
    {
        if (m_iCurUIGroup != -1)
            m_kUIGroups.GetAt(m_iCurUIGroup)->SetFocus(false);
            
        m_iCurUIGroup = (int)ui;
        NiUIGroup* pkGroup = m_kUIGroups.GetAt(m_iCurUIGroup);
        pkGroup->SetFocus(true);
        return pkGroup;
    }
    else
    {
        return m_kUIGroups.GetAt(m_iCurUIGroup);
    }
}
//---------------------------------------------------------------------------

