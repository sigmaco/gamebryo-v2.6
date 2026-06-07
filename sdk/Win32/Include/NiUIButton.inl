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
inline void NiUIButton::EnterPressed(unsigned char ucButtonState)
{
    HotkeyPressed(ucButtonState);
}
//---------------------------------------------------------------------------
inline void NiUIButton::LeftMouseButton(unsigned char ucButtonState)
{
    PressLogic(ucButtonState, false);
}
//---------------------------------------------------------------------------
inline bool NiUIButton::IsPressed() const
{
    return m_bPressed;
}
//---------------------------------------------------------------------------
inline void NiUIButton::SetButtonType(NiButtonType eButtonType)
{
    NIASSERT(eButtonType >= 0);
    NIASSERT(eButtonType <= TOGGLEBUTTON);
    m_eButtonType = eButtonType;
}
//---------------------------------------------------------------------------
inline NiUIButton::NiButtonType NiUIButton::GetButtonType() const
{
    return m_eButtonType;
}
//---------------------------------------------------------------------------
inline void NiUIButton::SetText(const char* pcButtonText)
{
    m_spLabel->SetText(pcButtonText);
}
//---------------------------------------------------------------------------
inline void NiUIButton::SubscribeToPressEvent(NiUIBaseSlot0* pkSlot)
{
    m_kPressEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIButton::SubscribeToReleaseEvent(NiUIBaseSlot0* pkSlot)
{
    m_kReleaseEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIButton::SubscribeToWhilePressedEvent(NiUIBaseSlot0* pkSlot)
{
    m_kWhilePressEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIButton::SubscribeToToggleEvent(NiUIBaseSlot1<bool>* pkSlot)
{
    m_kToggleEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIButton::Draw(NiRenderer* pkRenderer)
{
    if (m_bVisible)
    {
        m_spHotkeyImages->Draw(pkRenderer);
        m_spLabel->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiUIButton::NumQuadsRequired()
{
    return 9;
}
//---------------------------------------------------------------------------
inline void NiUIButton::SetVisible(bool bVisible)
{
    NiUIBaseElement::SetVisible(bVisible);
}
//---------------------------------------------------------------------------
