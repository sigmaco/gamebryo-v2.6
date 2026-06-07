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
// NiInputMouse inline functions
//---------------------------------------------------------------------------
inline const bool NiInputMouse::ButtonIsDown(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (eButton >= NIM_NUM_BUTTONS)
        return false;

    return m_akButtons[eButton].IsPressed(uiModifiers);
}
//---------------------------------------------------------------------------
inline const bool NiInputMouse::ButtonIsDown(Button eButton) const
{
    unsigned int uiModifiers;
    return ButtonIsDown(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
inline const bool NiInputMouse::ButtonWasPressed(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (eButton >= NIM_NUM_BUTTONS)
        return false;

    return m_akButtons[eButton].WasPressed(uiModifiers);
}
//---------------------------------------------------------------------------
inline const bool NiInputMouse::ButtonWasPressed(Button eButton) const
{
    unsigned int uiModifiers;
    return ButtonWasPressed(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
inline const bool NiInputMouse::ButtonWasReleased(Button eButton, 
    unsigned int& uiModifiers) const
{
    if (eButton >= NIM_NUM_BUTTONS)
        return false;

    return m_akButtons[eButton].WasReleased(uiModifiers);
}
//---------------------------------------------------------------------------
inline const bool NiInputMouse::ButtonWasReleased(Button eButton) const
{
    unsigned int uiModifiers;
    return ButtonWasReleased(eButton, uiModifiers);
}
//---------------------------------------------------------------------------
inline bool NiInputMouse::GetPositionDelta(unsigned int uiAxis, int& iDelta)
{
    if (uiAxis >= NIM_AXIS_COUNT)
        return false;

    iDelta = m_akAxes[uiAxis].GetValue();
    return true;
}
//---------------------------------------------------------------------------
inline bool NiInputMouse::GetPositionDelta(int& iX, int& iY, int& iZ)
{
    iX = m_akAxes[NIM_AXIS_X].GetValue();
    iY = m_akAxes[NIM_AXIS_Y].GetValue();
    iZ = m_akAxes[NIM_AXIS_Z].GetValue();

    return true;
}
//---------------------------------------------------------------------------
inline void NiInputMouse::RecordButtonPress(Button eButton)
{
    if (eButton < NIM_NUM_BUTTONS)
    {
        m_akButtons[eButton].SetButtonPress(m_uiModifiers);

        AddModifiers(1 << eButton);
    }
}
//---------------------------------------------------------------------------
inline void NiInputMouse::RecordButtonRelease(Button eButton)
{
    if (eButton < NIM_NUM_BUTTONS)
    {
        m_akButtons[eButton].SetButtonRelease();

        RemoveModifiers(1 << eButton);
    }
}
//---------------------------------------------------------------------------
inline void NiInputMouse::RecordPositionChange(unsigned int uiAxis, 
    int iDelta)
{
    if (uiAxis < NIM_AXIS_COUNT)
        m_akAxes[uiAxis].SetValue(iDelta);
}
//---------------------------------------------------------------------------
inline void NiInputMouse::RecordPositionChange(int iX, int iY, int iZ)
{
    m_akAxes[NIM_AXIS_X].SetValue(iX);
    m_akAxes[NIM_AXIS_Y].SetValue(iY);
    m_akAxes[NIM_AXIS_Z].SetValue(iZ);
}
//---------------------------------------------------------------------------
