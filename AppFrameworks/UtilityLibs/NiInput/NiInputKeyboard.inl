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
// NiInputKeyboard inline functions
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::AnyKeyDown() const
{
    unsigned int ui = 0;

    do
    {      
        if (m_aucKeystate[ui])
            return true;

        ui++;
    } while (ui < 32);

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::KeyIsDown(KeyCode eKey, 
    unsigned int& uiModifiers) const
{
    if (eKey == KEY_NOKEY)
        return false;

    if ((m_aucKeystate[eKey >> 3] >> (eKey & 7)) & 1)
    {
        uiModifiers = m_uiModifiers;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::KeyIsDown(KeyCode eKey) const
{
    if (eKey == KEY_NOKEY)
        return false;

    if ((m_aucKeystate[eKey >> 3] >> (eKey & 7)) & 1)
        return true;

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::KeyWasPressed(KeyCode eKey, 
    unsigned int& uiModifiers) const
{
    if (eKey == KEY_NOKEY)
        return false;

    if (((m_aucKeystate[eKey >> 3] >> (eKey & 7)) & 1) &&
        !((m_aucKeystateLastUpdate[eKey >> 3] >> (eKey & 7)) & 1))
    {
        uiModifiers = m_uiModifiers;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::KeyWasPressed(KeyCode eKey) const
{
    if (eKey == KEY_NOKEY)
        return false;

    if (((m_aucKeystate[eKey >> 3] >> (eKey & 7)) & 1) &&
        !((m_aucKeystateLastUpdate[eKey >> 3] >> (eKey & 7)) & 1))
        return true;

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::KeyWasReleased(KeyCode eKey, 
    unsigned int& uiModifiers) const
{
    if (eKey == KEY_NOKEY)
        return false;

    if (!((m_aucKeystate[eKey >> 3] >> (eKey & 7)) & 1) &&
        ((m_aucKeystateLastUpdate[eKey >> 3] >> (eKey & 7)) & 1))
    {
        uiModifiers = m_uiModifiers;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline const bool NiInputKeyboard::KeyWasReleased(KeyCode eKey) const
{
    if (eKey == KEY_NOKEY)
        return false;

    if (!((m_aucKeystate[eKey >> 3] >> (eKey & 7)) & 1) &&
        ((m_aucKeystateLastUpdate[eKey >> 3] >> (eKey & 7)) & 1))
        return true;

    return false;
}
//---------------------------------------------------------------------------
inline void NiInputKeyboard::RecordKeyPress(KeyCode eKey)
{
    if (!KeyIsDown(eKey))
    {
        // Set the keystate
        m_aucKeystate[eKey >> 3] |= 1 << (eKey & 7);

        // Set the modifier
        AddModifiers(GetKeyModifierValue(eKey));

        // Toggle the caps lock state
        if (eKey == KEY_CAPITAL)
            SetCapsLockState(!GetCapsLockState());
    }
}
//---------------------------------------------------------------------------
inline void NiInputKeyboard::RecordKeyRelease(KeyCode eKey)
{
    if (KeyIsDown(eKey))
    {
        m_aucKeystate[eKey >> 3] &= ~(1 << (eKey & 7));

        // Remove the modifier
        RemoveModifiers(GetKeyModifierValue(eKey));
    }
}
//---------------------------------------------------------------------------
inline bool NiInputKeyboard::GetCapsLockState()
{
    return m_bCapsLock;
}
//---------------------------------------------------------------------------
inline void NiInputKeyboard::SetCapsLockState(bool bCapsLock)
{
    m_bCapsLock = bCapsLock;
}
//---------------------------------------------------------------------------
inline unsigned int NiInputKeyboard::GetKeyModifierValue(KeyCode eKey)
{
    switch (eKey)
    {
    case KEY_LSHIFT:
        return KMOD_LSHIFT;
    case KEY_RSHIFT:
        return KMOD_RSHIFT;
    case KEY_LCONTROL:
        return KMOD_LCONTROL;
    case KEY_RCONTROL:
        return KMOD_RCONTROL;
    case KEY_LMENU:
        return KMOD_LMENU;
    case KEY_RMENU:
        return KMOD_RMENU;
    case KEY_LWIN:
        return KMOD_LWIN;
    case KEY_RWIN:
        return KMOD_RWIN;
    case KEY_CAPITAL:
        return KMOD_CAPS_LOCK;
    default:
        return KMOD_NONE;
    }
}
//---------------------------------------------------------------------------
