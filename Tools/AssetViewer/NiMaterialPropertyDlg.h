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

// NiMaterialPropertyDlg.h

#ifndef NIMATERIALPROPERTYDLG_H
#define NIMATERIALPROPERTYDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"
#include "ColorStatic.h"

class CNiMaterialPropertyDlg : public CNiObjectDlg
{
// Construction
public:
    CNiMaterialPropertyDlg(CWnd* pParent = NULL);   // standard constructor

    virtual bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CNiMaterialPropertyDlg)
    enum { IDD = IDD_NIMATERIALPROPERTY };
    
    CEdit   m_wndSpecularRedEdit;
    CEdit   m_wndSpecularGreenEdit;
    CEdit   m_wndSpecularBlueEdit;
    CColorStatic m_wndSpecularColor;
    
    CEdit   m_wndEmissiveRedEdit;
    CEdit   m_wndEmissiveGreenEdit;
    CEdit   m_wndEmissiveBlueEdit;
    CColorStatic m_wndEmissiveColor;

    CEdit   m_wndDiffuseGreenEdit;
    CEdit   m_wndDiffuseRedEdit;
    CEdit   m_wndDiffuseBlueEdit;
    CColorStatic m_wndDiffuseColor;

    CEdit   m_wndAmbientRedEdit;
    CEdit   m_wndAmbientGreenEdit;
    CEdit   m_wndAmbientBlueEdit;
    CColorStatic m_wndAmbientColor;

    CEdit   m_wndAlphaEdit;
    CEdit   m_wndShineEdit;
    
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiMaterialPropertyDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiMaterialPropertyDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIMATERIALPROPERTYDLG_H
