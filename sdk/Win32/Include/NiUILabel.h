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

#ifndef NIUILABEL_H
#define NIUILABEL_H

#include "NiUserInterfaceLibType.h"
#include <NiFont.h>
#include <NiColor.h>
#include <NiString.h>
#include <Ni2DString.h>
#include "NiUIBaseElement.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUILabel : public NiUIBaseElement
{
    NiDeclareRTTI;
public:
    NiUILabel(const char* pcLabelText = "",
        const NiColorA& kBackgroundColor = NiColorA::WHITE, 
        const NiColor& kTextColor = NiColor::BLACK);
    virtual ~NiUILabel();

    inline void SetText(const char* pcLabelText);
    inline void SetBackgroundColor(const NiColorA& kBackgroundColor);
    inline void SetTextColor(const NiColor& kTextColor);

    virtual void Draw(NiRenderer* pkRenderer);
    virtual void EnterPressed(unsigned char ucButtonState);
    virtual void LeftMouseButton(unsigned char ucButtonState);

    enum LabelVerticalAlign
    {
        TOP,
        BOTTOM,
        VERT_MIDDLE
    };
    enum LabelHorizontalAlign
    {
        LEFT,
        RIGHT,
        HORIZ_MIDDLE
    };
    inline void SetAlignment(LabelVerticalAlign eVert = TOP, 
        LabelHorizontalAlign eHoriz = LEFT);

    unsigned int GetVisualLineCount() const;

protected:
    virtual unsigned int NumQuadsRequired();
    virtual void ReinitializeDisplayElements();
    virtual void UpdateLayout();

    inline bool IsPrintableChar(char cChar);

    void FormatViewableText();
    void AlignViewableText();

    NiRect<float> m_kNDCRect;

    NiColorA m_kBackgroundColor;
    NiColorA m_kFontColor;
    NiString m_kLabelOriginalText;
    NiString m_kFormattedText;
    Ni2DStringPtr m_spViewableText;
    LabelVerticalAlign m_eVertAlign;
    LabelHorizontalAlign m_eHorizAlign;
};
NiSmartPointer(NiUILabel);
//---------------------------------------------------------------------------
#include "NiUILabel.inl"

#endif

