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

// ShadowGeneratorDlg.h

#ifndef SHADOWGENERATORDLG_H
#define SHADOWGENERATORDLG_H

#if _MSC_VER > 1000
#pragma once
#endif  // #if _MSC_VER > 1000

#include "NiObjectDlg.h"
#include "afxwin.h"

class CShadowGeneratorDlg : public CNiObjectDlg
{
    // Construction
public:
    CShadowGeneratorDlg(CWnd* pParent = NULL);   // standard constructor
    bool DoUpdate();

// Dialog Data
    //{{AFX_DATA(CShadowGeneratorDlg)
    enum { IDD = IDD_SHADOW_GENERATOR };
    CButton m_wndActiveCheckBox;
    CButton m_wndStaticCheckBox;
    CButton m_wndRenderBackfacesCheckBox;
    CButton m_wndStrictlyObserveSizeHintCheckBox;
    CEdit m_wndShadowTechniqueEditBox;
    CEdit m_wndDepthBiasEditBox;
    CEdit m_wndSizeHintEditBox;
    CListBox m_wndUnaffectedCastersList;
    CListBox m_wndUnaffectedReceiversList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CShadowGeneratorDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CShadowGeneratorDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnLbnDblclkListUnaffectedCasters();
    afx_msg void OnLbnDblclkListUnaffectedReceivers();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif  // #ifndef SHADOWGENERATORDLG_H
