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

// NiLightDlg.h

#ifndef NILIGHTDLG_H
#define NILIGHTDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"
#include "ColorStatic.h"

class CNiLightDlg : public CNiObjectDlg
{
// Construction
public:
    CNiLightDlg(CWnd* pParent = NULL);   // standard constructor
    virtual bool NeedsUpdateAfterCreation(){return true;}
    virtual bool DoUpdate();

// Dialog Data
    //{{AFX_DATA(CNiLightDlg)
    enum { IDD = IDD_NILIGHT };
    CEdit   m_wndSpecularColorRedEdit;
    CEdit   m_wndSpecularColorGreenEdit;
    CEdit   m_wndSpecularColorBlueEdit;
    CColorStatic m_wndSpecularColor;
    CEdit   m_wndDimmerEdit;
    CColorStatic m_wndDiffuseColor;
    CEdit   m_wndDiffuseColorRedEdit;
    CEdit   m_wndDiffuseColorGreenEdit;
    CEdit   m_wndDiffuseColorBlueEdit;
    CColorStatic m_wndAmbientColor;
    CEdit   m_wndAmbientColorBlueEdit;
    CEdit   m_wndAmbientColorGreenEdit;
    CEdit   m_wndAmbientColorRedEdit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiLightDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiLightDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NILIGHTDLG_H
