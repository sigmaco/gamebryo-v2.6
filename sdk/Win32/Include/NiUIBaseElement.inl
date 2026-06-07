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
inline bool NiUIBaseElement::MouseLocation(unsigned int uiMouseX, 
    unsigned int uiMouseY)
{
    bool bIsInside = IsInside(uiMouseX, uiMouseY);

    if (m_bCanTakeFocus)
    {
        if (bIsInside && !m_bMouseInside)
            m_kGotFocus.EmitSignal();
        else if (!bIsInside && m_bMouseInside)
            m_kLostFocus.EmitSignal();

        m_bMouseInside = bIsInside;
    }

    return (bIsInside && m_bCanTakeFocus);
}
//---------------------------------------------------------------------------
inline bool NiUIBaseElement::IsInside(unsigned int uiX, unsigned int uiY)
{
    bool bIsInside = ((uiX <= m_kPixelRect.m_right) && 
        (uiX >= m_kPixelRect.m_left) && 
        (uiY <= m_kPixelRect.m_bottom) && (uiY >= m_kPixelRect.m_top));

    return bIsInside;
}
//---------------------------------------------------------------------------
inline bool NiUIBaseElement::IsVisible()
{
    return m_bVisible;
}
//---------------------------------------------------------------------------
inline void NiUIBaseElement::SetVisible(bool bVisible)
{
    m_bVisible = bVisible;

    // Pass the news to your kids
    unsigned int uiChildCount = GetChildElementCount();
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < uiChildCount; uiIdx++)
    {
        NiUIBaseElement* pkElement = GetChildElement(uiIdx);
        if (pkElement)
        {
            pkElement->SetVisible(bVisible);
        }
    }

    ReinitializeDisplayElements();

    if (bVisible)
        m_kShown.EmitSignal();
    else
        m_kHidden.EmitSignal();
}
//---------------------------------------------------------------------------
inline void NiUIBaseElement::SubscribeShown(NiUIBaseSlot0* pkSlot)
{
    m_kShown.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIBaseElement::SubscribeHidden(NiUIBaseSlot0* pkSlot)
{
    m_kHidden.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIBaseElement::SubscribeResize(
    NiUIBaseSlot1<NiRect<float> >* pkSlot)
{
    m_kResized.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIBaseElement::SubscribeGotFocus(NiUIBaseSlot0* pkSlot)
{
    m_kGotFocus.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline void NiUIBaseElement::SubscribeLostFocus(NiUIBaseSlot0* pkSlot)
{
    m_kLostFocus.Subscribe(pkSlot);
}
//---------------------------------------------------------------------------
inline bool NiUIBaseElement::SetFocus(bool bFocus)
{
    if (!m_bCanTakeFocus)
        return false;

    if (bFocus)
        m_kGotFocus.EmitSignal();
    else
        m_kLostFocus.EmitSignal();
    m_bFocussed = bFocus;
    return true;
}
//---------------------------------------------------------------------------
