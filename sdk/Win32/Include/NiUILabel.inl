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
inline void NiUILabel::EnterPressed(unsigned char)
{
    // I don't do anything with events
}
//---------------------------------------------------------------------------
inline void NiUILabel::LeftMouseButton(unsigned char)
{
    // I don't do anything with events
}
//---------------------------------------------------------------------------
inline void NiUILabel::SetText(const char* pcLabelText)
{
    m_kLabelOriginalText = pcLabelText;

    FormatViewableText();
}
//---------------------------------------------------------------------------
inline void NiUILabel::UpdateLayout()
{
    FormatViewableText();
}
//---------------------------------------------------------------------------
inline void NiUILabel::SetBackgroundColor(const NiColorA& kBackgroundColor)
{
    m_kBackgroundColor = kBackgroundColor;

    if (m_spScreenElements)
    {
        m_spScreenElements->SetColors(m_iBaseQuadIdx, m_kBackgroundColor);
    }
}
//---------------------------------------------------------------------------
inline void NiUILabel::SetTextColor(const NiColor& kTextColor)
{
    m_kFontColor = NiColorA(kTextColor.r, kTextColor.g, kTextColor.b, 1.0f);

    if (m_spViewableText)
    {
        m_spViewableText->SetColor(m_kFontColor);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiUILabel::NumQuadsRequired()
{
    return 1;
}
//---------------------------------------------------------------------------
inline void NiUILabel::Draw(NiRenderer* pkRenderer)
{
    if (m_bVisible && (m_spViewableText != NULL))
    {
        m_spViewableText->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
inline bool NiUILabel::IsPrintableChar(char cChar)
{
    return ((cChar >= 33) && (cChar <= 126));
}
//---------------------------------------------------------------------------
inline void NiUILabel::SetAlignment(LabelVerticalAlign eVert, 
    LabelHorizontalAlign eHoriz)
{
    m_eVertAlign = eVert;
    m_eHorizAlign = eHoriz;

    AlignViewableText();
}
//---------------------------------------------------------------------------
