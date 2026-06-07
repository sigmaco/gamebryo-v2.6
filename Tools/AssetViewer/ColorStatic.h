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

// ColorStatic.h

#ifndef COLORSTATIC_H
#define COLORSTATIC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColorStatic : public CStatic
{
// Construction
public:
    CColorStatic();
    void SetTextColor(NiColor& clrText);
    void SetBackgroundColor(NiColor& clrBack);
    NiColor& GetTextColor();
    NiColor& GetBackgroundColor();
   
    // Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColorStatic)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CColorStatic();

    // Generated message map functions
protected:
    NiColor m_kColorText;
    NiColor m_kColorBackground;
    CBrush m_brBackground;

    static COLORREF NiColorToCOLORREF(NiColor& kColor);
    //{{AFX_MSG(CColorStatic)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG

    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // COLORSTATIC_H
