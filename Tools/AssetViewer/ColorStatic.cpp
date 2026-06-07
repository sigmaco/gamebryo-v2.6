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

// ColorStatic.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CColorStatic
//---------------------------------------------------------------------------
CColorStatic::CColorStatic()
{
    m_kColorText = NiColor::BLACK;
    m_kColorBackground = NiColor::WHITE;

    m_brBackground.CreateSolidBrush(NiColorToCOLORREF(m_kColorBackground));
}
//---------------------------------------------------------------------------
CColorStatic::~CColorStatic()
{
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
    //{{AFX_MSG_MAP(CColorStatic)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        ON_WM_CTLCOLOR_REFLECT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CColorStatic message handlers
//---------------------------------------------------------------------------
void CColorStatic::SetTextColor(NiColor& clrText)
{
    m_kColorText = clrText;
    Invalidate();
}
//---------------------------------------------------------------------------
void CColorStatic::SetBackgroundColor(NiColor& clrBack)
{
    m_kColorBackground = clrBack;
    m_brBackground.DeleteObject();
    m_brBackground.CreateSolidBrush(NiColorToCOLORREF(m_kColorBackground));
    Invalidate();
}
//---------------------------------------------------------------------------
HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
    pDC->SetTextColor(NiColorToCOLORREF(m_kColorText));
    pDC->SetBkColor(NiColorToCOLORREF(m_kColorBackground));
    return (HBRUSH) m_brBackground;
}
//---------------------------------------------------------------------------
NiColor& CColorStatic::GetTextColor() 
{
    return m_kColorText;
}
//---------------------------------------------------------------------------
NiColor& CColorStatic::GetBackgroundColor()
{
    return m_kColorBackground;
}
//---------------------------------------------------------------------------
COLORREF CColorStatic::NiColorToCOLORREF(NiColor& kColor)
{
    int R, G, B;

    R = (int)(kColor.r*255);
    G = (int)(kColor.g*255);
    B = (int)(kColor.b*255);

    return RGB(R, G, B);
}
//---------------------------------------------------------------------------
