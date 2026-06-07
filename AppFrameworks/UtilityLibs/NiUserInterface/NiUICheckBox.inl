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
inline void NiUICheckBox::EnterPressed(unsigned char ucCheckBoxState)
{
    HotkeyPressed(ucCheckBoxState);
}
//---------------------------------------------------------------------------
inline void NiUICheckBox::LeftMouseButton(unsigned char ucCheckBoxState)
{
    PressLogic(ucCheckBoxState, false);
}
//---------------------------------------------------------------------------
inline void NiUICheckBox::SetPressed(bool bPressed)
{
    m_bPressed = bPressed;

//    m_bHotkeyCaused = false;
    ReinitializeDisplayElements();
}
//---------------------------------------------------------------------------
inline bool NiUICheckBox::IsPressed() const
{
    return m_bPressed;
}
//---------------------------------------------------------------------------
inline void NiUICheckBox::SetText(const char* pcCheckBoxText)
{
    m_spLabel->SetText(pcCheckBoxText);
}
//---------------------------------------------------------------------------
inline void NiUICheckBox::SubscribeToToggleEvent(NiUIBaseSlot1<bool>* pkSlot)
{
    m_kToggleEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUICheckBox::Draw(NiRenderer* pkRenderer)
{
    if (m_bVisible)
    {
        m_spHotkeyImages->Draw(pkRenderer);
        m_spLabel->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiUICheckBox::NumQuadsRequired()
{
    return 2;
}
//---------------------------------------------------------------------------
inline void NiUICheckBox::SetVisible(bool bVisible)
{
    NiUIBaseElement::SetVisible(bVisible);
}
//---------------------------------------------------------------------------
