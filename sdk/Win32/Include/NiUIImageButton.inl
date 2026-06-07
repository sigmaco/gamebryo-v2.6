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
inline void NiUIImageButton::SetState(bool bToggleState)
{
    m_bToggleState = bToggleState;
    ReinitializeDisplayElements();

    m_kToggleEvent.EmitSignal(m_bToggleState);
}
//---------------------------------------------------------------------------
inline bool NiUIImageButton::GetState() const
{
    return m_bToggleState;
}
//---------------------------------------------------------------------------
inline void NiUIImageButton::SetAtlasImageForState(
    NiUIImageButton::ImageState eState, bool bToggleState,
    NiUIAtlasMap::UIElement eElement)
{
    if (bToggleState)
    {
        m_aeOnElements[eState] = eElement;
    }
    else
    {
        m_aeOffElements[eState] = eElement;
    }
}
//---------------------------------------------------------------------------
inline void NiUIImageButton::SubscribeToToggleEvent(
    NiUIBaseSlot1<bool>* pkSlot)
{
    m_kToggleEvent.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIImageButton::Draw(NiRenderer*)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiUIImageButton::NumQuadsRequired()
{
    return 1;
}
//---------------------------------------------------------------------------
inline void NiUIImageButton::SetVisible(bool bVisible)
{
    NiUIBaseElement::SetVisible(bVisible);
}
//---------------------------------------------------------------------------
