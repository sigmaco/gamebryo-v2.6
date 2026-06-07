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
inline void NiUIHotkeyImage::EnterPressed(unsigned char)
{
}
//---------------------------------------------------------------------------
inline void NiUIHotkeyImage::LeftMouseButton(unsigned char)
{
}
//---------------------------------------------------------------------------
inline unsigned int NiUIHotkeyImage::NumQuadsRequired()
{
    return 0;
}
//---------------------------------------------------------------------------
inline void NiUIHotkeyImage::Draw(NiRenderer* pkRenderer)
{
    if (!m_bVisible)
        return;

    unsigned int uiIdx, uiSize;
    uiSize = m_kImages.GetSize();
    // First draw the images
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (m_kImages.GetAt(uiIdx) != NULL)
            m_kImages.GetAt(uiIdx)->Draw(pkRenderer);
    }
    // Then draw the text over them
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (m_kLabels.GetAt(uiIdx) != NULL)
            m_kLabels.GetAt(uiIdx)->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
inline void NiUIHotkeyImage::SetVisible(bool bVisible)
{
    NiUIBaseElement::SetVisible(bVisible);
}
//---------------------------------------------------------------------------
inline bool NiUIHotkeyImage::IsDoubleWide(NiInputKeyboard::KeyCode eKey)
{
    return ((eKey == NiInputKeyboard::KEY_BACK) || 
        (eKey == NiInputKeyboard::KEY_TAB) || 
        (eKey == NiInputKeyboard::KEY_RETURN) || 
        (eKey == NiInputKeyboard::KEY_RCONTROL) || 
        (eKey == NiInputKeyboard::KEY_LCONTROL) || 
        (eKey == NiInputKeyboard::KEY_LSHIFT) || 
        (eKey == NiInputKeyboard::KEY_RSHIFT) || 
        (eKey == NiInputKeyboard::KEY_LMENU) || 
        (eKey == NiInputKeyboard::KEY_RMENU) || 
        (eKey == NiInputKeyboard::KEY_SPACE) || 
        (eKey == NiInputKeyboard::KEY_CAPITAL) || 
        (eKey == NiInputKeyboard::KEY_NUMLOCK) || 
        (eKey == NiInputKeyboard::KEY_SCROLL) || 
        (eKey == NiInputKeyboard::KEY_APPS) || 
        (eKey == NiInputKeyboard::KEY_POWER) || 
        (eKey == NiInputKeyboard::KEY_WEBSTOP) || 
        (eKey == NiInputKeyboard::KEY_MAIL) || 
        (eKey == NiInputKeyboard::KEY_MYCOMPUTER) || 
        (eKey == NiInputKeyboard::KEY_WEBREFRESH) || 
        (eKey == NiInputKeyboard::KEY_SLEEP) || 
        (eKey == NiInputKeyboard::KEY_WEBBACK) || 
        (eKey == NiInputKeyboard::KEY_WAKE) || 
        (eKey == NiInputKeyboard::KEY_WEBFAVORITES) || 
        (eKey == NiInputKeyboard::KEY_WEBSEARCH) || 
        (eKey == NiInputKeyboard::KEY_WEBFORWARD) || 
        (eKey == NiInputKeyboard::KEY_MEDIASELECT));
}
//---------------------------------------------------------------------------
inline float NiUIHotkeyImage::GetTotalWidth()
{
    float fRetVal = 0;
    unsigned int uiIdx, uiSize;
    uiSize = m_kImageWidth.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        fRetVal += m_kImageWidth.GetAt(uiIdx);
    }
    return fRetVal;
}
//---------------------------------------------------------------------------
